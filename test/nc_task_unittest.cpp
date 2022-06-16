#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/nc_task.h"
#include "nc_runtime/nc_string.h"
#include "nc_runtime/nc_stdlib.h"
#include "nc_runtime/nc_array.h"
#include "nc_runtime/nc_event.h"

class MyStringConcatTask : public Task
{
public:
    MyStringConcatTask(TaskCallback callback, sp<NcString> s1, sp<NcString> s2) : Task(callback), m_str1(s1), m_str2(s2)
    {
    }

    static sp<MyStringConcatTask> alloc(TaskCallback callback, const sp<NcString>& s1, const sp<NcString>& s2)
    {
        return NcObject::alloc<MyStringConcatTask>(callback, s1, s2);
    }

    sp<NcObject> doTask() override
    {
        int count = 50;
        while (count-- != 0)
        {
            if (cancelled())
            {
                return nullptr;
            }
            Thread::sleep(1);
        }
        return NcString::format("%s %s", m_str1->cstr(), m_str2->cstr());
    }

private:
    sp<NcString> m_str1;
    sp<NcString> m_str2;
};

TEST(TaskTest, basic)
{
    sp<NcString> taskResult;
    auto task = MyStringConcatTask::alloc(
        [&taskResult](sp<NcObject> result) { taskResult = static_pointer_cast<NcString>(result); }, "hello"_str,
        "world"_str);
    task->start();

    // 等待任务结束，拿结果
    task->wait();
    EXPECT_EQ(taskResult, "hello world"_str);
}

TEST(TaskTest, cancel)
{
    TimeTick start = TimeTick::now();
    sp<NcString> taskResult;
    auto task = MyStringConcatTask::alloc(
        [&taskResult](sp<NcObject> result) { taskResult = static_pointer_cast<NcString>(result); }, "hello"_str,
        "world"_str);
    task->start();
    task->cancel();
    EXPECT_TRUE(task->cancelled());

    task->wait();
    EXPECT_TRUE(taskResult == nullptr);
    EXPECT_LT((TimeTick::now() - start).ms(), 5);
}

TEST(TaskTest, withoutWait)
{
    static bool exited = false;
    static bool resultIsDiscarded = false;

    sp<NcArray<NcString>> taskResult = NcArray<NcString>::alloc();
    ResetableEvent event(false, false);

    {
        wp<NcArray<NcString>> taskResultCapture(taskResult);

        // 用 weak_ptr 接收结果
        auto taskReceiveCallback = [taskResultCapture, &event](sp<NcObject> result) mutable {
            EXPECT_TRUE(exited);

            sp<NcArray<NcString>> r = taskResultCapture.lock();
            if (r != nullptr)
            {
                r->addObject(static_pointer_cast<NcString>(result));
            }
            else
            {
                resultIsDiscarded = true;
            }

            event.set();
        };
        auto task = MyStringConcatTask::alloc(taskReceiveCallback, "hello"_str, "world"_str);
        task->start();

    } // 此时 task 销毁，后续不应该崩溃

    // 此时应该还没有结果
    EXPECT_TRUE(taskResult->size() == 0);
    taskResult = nullptr; // 结果接收者销毁
    exited = true;

    EXPECT_TRUE(event.waitWithTimeout(200) == EventWaitResult::succ) << "wait for callback compete";
    EXPECT_TRUE(resultIsDiscarded) << "the result must have been discarded";
}
