#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/line_parser.h"

TEST(LineParserTest, basic)
{
    nc::LineParser parser;
    parser.feedStr("Hell");
    parser.feedStr("o\n\r\nWorld");
    EXPECT_STREQ(parser.nextLine(), "Hello");
    EXPECT_STREQ(parser.nextLine(), "");
    EXPECT_STREQ(parser.nextLine(), NULL);
    EXPECT_STREQ(parser.nextLine(), NULL);
    parser.finish();
    EXPECT_STREQ(parser.nextLine(), "World");
    EXPECT_STREQ(parser.nextLine(), NULL);
    EXPECT_STREQ(parser.nextLine(), NULL);
}

TEST(LineParserTest, test2)
{
    nc::LineParser parser;
    parser.feedStr("Hell");
    EXPECT_STREQ(parser.nextLine(), NULL);
    parser.feedStr("o");
    EXPECT_STREQ(parser.nextLine(), NULL);
    parser.feedStr("\n");
    EXPECT_STREQ(parser.nextLine(), "Hello");
}

TEST(LineParserTest, test3)
{
    nc::LineParser parser;
    EXPECT_EQ(parser._bufferSize(), parser._maxBufferSize());

    for (size_t i = 0; i < parser._maxBufferSize(); i++)
    {
        parser.feedStr("Hell");
        EXPECT_STREQ(parser.nextLine(), NULL);
        parser.feedStr("o");
        EXPECT_STREQ(parser.nextLine(), NULL);
        parser.feedStr("\n");
        EXPECT_STREQ(parser.nextLine(), "Hello");
    }

    EXPECT_EQ(parser._bufferSize(), parser._maxBufferSize());
}
