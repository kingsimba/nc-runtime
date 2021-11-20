#pragma once

#include <mutex>

/*
Example code:

class MutexTest
{
public:
    Mutex m_mutex;
    int m_counter GUARDED_BY(m_mutex) = 0; // GUARDED_BY is only effective with clang

    void add() {
        LockGuard lg(m_mutex);
        m_counter++;
    }
};

*/

#if defined(__clang__) && (!defined(SWIG))
#    define THREAD_ANNOTATION_ATTRIBUTE__(x) __attribute__((x))
#else
#    define THREAD_ANNOTATION_ATTRIBUTE__(x) // no-op
#endif

#define GUARDED_BY(x) THREAD_ANNOTATION_ATTRIBUTE__(guarded_by(x))
#define PT_GUARDED_BY(x) THREAD_ANNOTATION_ATTRIBUTE__(pt_guarded_by(x))

#define __CAP_CAPABILITY(x) THREAD_ANNOTATION_ATTRIBUTE__(capability(x))
#define __CAP_SCOPED_CAPABILITY THREAD_ANNOTATION_ATTRIBUTE__(scoped_lockable)
#define __CAP_ACQUIRE(...) THREAD_ANNOTATION_ATTRIBUTE__(acquire_capability(__VA_ARGS__))
#define __CAP_RELEASE(...) THREAD_ANNOTATION_ATTRIBUTE__(release_capability(__VA_ARGS__))

/*


#define ACQUIRED_BEFORE(...) THREAD_ANNOTATION_ATTRIBUTE__(acquired_before(__VA_ARGS__))

#define ACQUIRED_AFTER(...) THREAD_ANNOTATION_ATTRIBUTE__(acquired_after(__VA_ARGS__))

#define REQUIRES(...) THREAD_ANNOTATION_ATTRIBUTE__(requires_capability(__VA_ARGS__))

#define REQUIRES_SHARED(...) THREAD_ANNOTATION_ATTRIBUTE__(requires_shared_capability(__VA_ARGS__))

#define ACQUIRE_SHARED(...) THREAD_ANNOTATION_ATTRIBUTE__(acquire_shared_capability(__VA_ARGS__))

#define RELEASE_SHARED(...) THREAD_ANNOTATION_ATTRIBUTE__(release_shared_capability(__VA_ARGS__))

#define TRY_ACQUIRE(...) THREAD_ANNOTATION_ATTRIBUTE__(try_acquire_capability(__VA_ARGS__))

#define TRY_ACQUIRE_SHARED(...) THREAD_ANNOTATION_ATTRIBUTE__(try_acquire_shared_capability(__VA_ARGS__))

#define EXCLUDES(...) THREAD_ANNOTATION_ATTRIBUTE__(locks_excluded(__VA_ARGS__))

#define ASSERT_CAPABILITY(x) THREAD_ANNOTATION_ATTRIBUTE__(assert_capability(x))

#define ASSERT_SHARED_CAPABILITY(x) THREAD_ANNOTATION_ATTRIBUTE__(assert_shared_capability(x))

#define RETURN_CAPABILITY(x) THREAD_ANNOTATION_ATTRIBUTE__(lock_returned(x))

#define NO_THREAD_SAFETY_ANALYSIS THREAD_ANNOTATION_ATTRIBUTE__(no_thread_safety_analysis)
*/

namespace nc
{

// NOTE: Wrappers for std::mutex and std::unique_lock are provided so that
// we can annotate them with thread safety attributes and use the
// -Wthread-safety warning with clang. The standard library types cannot be
// used directly because they do not provided the required annotations.
class __CAP_CAPABILITY("mutex") Mutex
{
public:
    Mutex() {}

    void lock() __CAP_ACQUIRE() { mut_.lock(); }
    void unlock() __CAP_RELEASE() { mut_.unlock(); }
    std::recursive_mutex& native_handle() { return mut_; }

private:
    std::recursive_mutex mut_;
};

class __CAP_SCOPED_CAPABILITY UniqueLock
{
    typedef std::unique_lock<std::recursive_mutex> UniqueLockImp;

public:
    UniqueLock(Mutex& m) __CAP_ACQUIRE(m) : ul_(m.native_handle()) {}
    ~UniqueLock() __CAP_RELEASE() {}
    UniqueLockImp& native_handle() { return ul_; }

private:
    UniqueLockImp ul_;
};

class __CAP_SCOPED_CAPABILITY LockGuard
{
    typedef std::lock_guard<std::recursive_mutex> LockGuardImp;

public:
    LockGuard(Mutex& m) __CAP_ACQUIRE(m) : lg_(m.native_handle()) {}
    ~LockGuard() __CAP_RELEASE() {}
    LockGuardImp& native_handle() { return lg_; }

private:
    LockGuardImp lg_;
};

} // namespace nc
