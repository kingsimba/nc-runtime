#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/nc_file.h"

TEST(NcFile, read)
{
    auto o = NcFile::alloc("test_data/hello.txt"_s, FileOpenFlag::read);
    ASSERT_TRUE(o != nullptr);
    EXPECT_EQ(o->length(), 5);
    char buffer[10];
    ASSERT_EQ(o->read(buffer, 10), 5);
    auto str = StringSlice::makeEphemeralWithBytes(buffer, 5);
    EXPECT_EQ(str, "hello");
}

TEST(NcFile, write)
{
    auto o = NcFile::alloc("test_data/hello.txt"_s, FileOpenFlag::write | FileOpenFlag::createAlways);
    ASSERT_TRUE(o != nullptr);
    EXPECT_EQ(o->length(), 0);
    ASSERT_EQ(o->write("hello", 5), 5);
    o->close();

    EXPECT_EQ(StringSlice::makeWithContentsOfFile("test_data/hello.txt").value(), "hello"_s);

    o = NcFile::alloc("test_data/hello.txt"_s, FileOpenFlag::write);
    ASSERT_TRUE(o != nullptr);
    EXPECT_EQ(o->length(), 5);
    o->seek(FileSeekOrigin::end, 0);
    ASSERT_EQ(o->write(" world", 6), 6);
    o->close();

    EXPECT_EQ(StringSlice::makeWithContentsOfFile("test_data/hello.txt").value(), "hello world"_s);

    o = NcFile::alloc("test_data/hello.txt"_s, FileOpenFlag::write | FileOpenFlag::createAlways);
    ASSERT_TRUE(o != nullptr);
    EXPECT_EQ(o->length(), 0);
    ASSERT_EQ(o->write(" world", 6), 6);
    o->close();

    EXPECT_EQ(StringSlice::makeWithContentsOfFile("test_data/hello.txt").value(), " world"_s);

    o = NcFile::alloc("test_data/hello.txt"_s, FileOpenFlag::write | FileOpenFlag::append);
    ASSERT_TRUE(o != nullptr);
    EXPECT_EQ(o->length(), 6);
    ASSERT_EQ(o->write(" world", 6), 6);
    o->close();

    EXPECT_EQ(StringSlice::makeWithContentsOfFile("test_data/hello.txt").value(), " world world"_s);
}
