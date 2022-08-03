#pragma once

#include <vector>

namespace nc
{

/**
 * Split stream into lines. Support "\n" or "\r\n".
 *
 * @code
 * TEST(LineParserTest, basic)
 * {
 *     nc::LineParser parser;
 *     parser.feed("Hell", 4);
 *     parser.feedStr("o\n\nWorld");
 *     EXPECT_STREQ(parser.nextLine(), "Hello");
 *     EXPECT_STREQ(parser.nextLine(), "");
 *     EXPECT_STREQ(parser.nextLine(), NULL);
 *     parser.finish();
 *     EXPECT_STREQ(parser.nextLine(), "World");
 *     EXPECT_STREQ(parser.nextLine(), NULL);
 * }
 * @endcode
 */
class LineParser
{
public:
    LineParser();

    void feed(const void* bytes, size_t len);
    void feedStr(const char* str) { feed(str, strlen(str)); }
    void finish(); // append \n if there is no \n on last line

    const char* nextLine(); // return NULL when no new line is available

    size_t _maxBufferSize() const;
    size_t _bufferSize() const { return m_buffer.capacity(); }

private:
    std::vector<char> m_buffer;
    size_t m_lineStart = 0;
    size_t m_parsedPos = 0;
};
} // namespace nc
