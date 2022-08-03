#include "nc_runtime/line_parser.h"
#include <memory.h>

namespace nc
{

#define BUFFER_SIZE 64

size_t LineParser::_maxBufferSize() const
{
    return BUFFER_SIZE;
}

LineParser::LineParser()
{
    m_buffer.reserve(BUFFER_SIZE);
}

void LineParser::feed(const void* bytes, size_t len)
{
    if (m_lineStart + len >= BUFFER_SIZE)
    {
        m_buffer.erase(m_buffer.begin(), m_buffer.begin() + m_lineStart);
        m_parsedPos = m_lineStart = 0;
    }

    size_t oldSize = m_buffer.size();
    m_buffer.resize(oldSize + len);
    memcpy(&m_buffer[oldSize], bytes, len);
}

void LineParser::finish()
{
    if (!m_buffer.empty() && m_buffer.back() != '\n')
    {
        feed("\n", 1);
    }
}

const char* LineParser::nextLine()
{
    char* line = &m_buffer[m_lineStart];
    char* p = &m_buffer[m_parsedPos];
    char* pend = &m_buffer[m_buffer.size()];
    for (; p != pend; p++)
    {
        if (*p == '\n')
        {
            *p = 0;
            m_parsedPos = m_lineStart = (p - &m_buffer[0]) + 1;
            return line;
        }
        else if (*p == '\r' && p + 1 != pend && *(p + 1) == '\n')
        {
            *p = 0;
            m_parsedPos = m_lineStart = (p - &m_buffer[0]) + 2;
            return line;
        }
    }

    m_parsedPos = m_buffer.size();

    return NULL;
}

} // namespace nc