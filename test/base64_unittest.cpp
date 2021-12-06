#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/base64.h"

TEST(Base64Test, basic)
{
    const char* testData = "this is a mistake";
    auto str = Base64_encode(testData, strlen(testData));
    EXPECT_EQ(str->toSlice(), "dGhpcyBpcyBhIG1pc3Rha2U=");
    auto buffer = Base64_decode(str->cstr(), str->length());
    EXPECT_EQ(buffer->toSlice(), testData);
}
