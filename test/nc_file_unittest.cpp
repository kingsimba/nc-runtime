#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/nc_file.h"

TEST(NcFile, read)
{
    auto o = NcFile::alloc("test_data/hello.txt"_s, FileOpenFlag::read);
    ASSERT_TRUE(o != nullptr);
    EXPECT_EQ(o->length(), 5);
    char buffer[10];
    ASSERT_EQ(o->read(buffer, 10), 5);
    auto str = StringSlice::makeEphemeral(buffer, 5);
    EXPECT_EQ(str, "hello");
}
