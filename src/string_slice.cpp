#include "stdafx_nc_runtime.h"
#include "nc_runtime/nc_types.h"
#include "nc_runtime/string_slice.h"
#include "../3rd-party/include/jansson/utf.h"
#include "nc_runtime/nc_string.h"
#include <stdarg.h>

bool StringCharIter::next(wchar32* cOut, int* consumedBytesOut)
{
    int32_t c;

    const char* newStr = utf8_iterate(m_str, m_length, &c);
    if (newStr == m_str)
    {
        return false;
    }

    int consumedBytes = (int)(newStr - m_str);
    m_length -= consumedBytes;
    m_str = newStr;
    *cOut = c;
    if (consumedBytesOut != NULL)
        *consumedBytesOut = consumedBytes;

    return true;
}

//////////////////////////////////////////////////////////////////////////
static const char* _strchr(const char* s, const char* sEnd, char c)
{
    for (; s != sEnd; s++)
    {
        if (*s == c)
            return s;
    }
    return s;
}

static int _strncmp(const char* s1, const char* s1End, const char* s2, size_t s2len)
{
    unsigned char u1, u2;
    while (s2len-- > 0 && s1 != s1End)
    {
        u1 = (unsigned char)*s1++;
        u2 = (unsigned char)*s2++;
        if (u1 != u2)
            return u1 - u2;
        if (u1 == '\0')
            return 0;
    }
    return 0;
}

static const char* _strstr(const char* s1, const char* s1End, const char* s2, size_t s2len)
{
    const char* p = s1;
    for (; (p = _strchr(p, s1End, *s2)) != NULL; p++)
    {
        if ((size_t)(s1End - p) < s2len)
        {
            return s1End;
        }
        if (_strncmp(p, s1End, s2, s2len) == 0)
            return p;
    }
    return p;
}

StringSubsliceIter::StringSubsliceIter(const StringSlice& slice, const StringSlice& sep)
{
    m_str = m_strBegin = slice.internalBytes();
    m_strEnd = m_str + slice.length();
    m_ncstring = retain(slice.internalString());
    m_sep = sep.internalBytes();
    m_sepLength = sep.length();
    m_foundSep = false;
}

StringSubsliceIter::~StringSubsliceIter()
{
    release(m_ncstring);
}

bool StringSubsliceIter::next(StringSlice* cOut, Range* rangeOut)
{
    if (m_str == m_strEnd && !m_foundSep)
    {
        return false;
    }

    const char* newStr = _strstr(m_str, m_strEnd, m_sep, m_sepLength);

    *cOut = StringSlice(m_str, (int)(newStr - m_str), retain(m_ncstring));
    if (rangeOut != NULL)
        *rangeOut = Range((int)(m_str - m_strBegin), cOut->length());

    if (newStr != m_strEnd)
    {
        m_str = newStr + m_sepLength;
        m_foundSep = true;
    }
    else
    {
        m_str = newStr;
        m_foundSep = false;
    }

    return true;
}

StringSlice StringSlice::subsliceFrom(int start)
{
    if (start < 0)
    {
        assert(m_length + start >= 0);
        return StringSlice(m_str + (m_length + start), -start, retain(m_ncstring));
    }
    else
    {
        assert(start <= m_length);
        return StringSlice(m_str + start, m_length - start, retain(m_ncstring));
    }
}

std::vector<StringSlice> StringSlice::split(const StringSlice& sep)
{
    std::vector<StringSlice> sv;
    auto iter = this->iterForSplitting(sep);
    StringSlice slice;
    while (iter.next(&slice))
    {
        sv.push_back(slice);
    }
    return sv;
}

int StringSlice::splitWithLimit(const StringSlice& sep, StringSlice* slicesOut, int maxNum)
{
    int created = 0;
    std::vector<StringSlice> sv;
    auto iter = this->iterForSplitting(sep);
    StringSlice slice;
    while (created < maxNum && iter.next(&slice))
    {
        slicesOut[created++] = std::move(slice);
    }
    return created;
}

std::vector<StringSlice> StringSlice::tokenize(const StringSlice& seps)
{
    std::vector<StringSlice> sv;
    auto iter = this->iterForTokenizing(seps);
    StringSlice slice;
    while (iter.next(&slice))
    {
        sv.push_back(slice);
    }
    return sv;
}

StringSlice StringSlice::replaceInRange(Range range, const StringSlice& replacement)
{
    if (!range.isValid())
    {
        return *this;
    }
    assert(range.end() <= m_length);

    size_t len = (size_t)m_length + replacement.length() - range.length;
    char* buffer;
    auto rtn = NcString::allocButFillContentLater(len, &buffer);

    size_t loc = 0;
    memcpy(buffer, m_str, range.location);
    loc += range.location;
    memcpy(buffer + loc, replacement.internalBytes(), replacement.length());
    loc += replacement.length();
    memcpy(buffer + loc, m_str + range.end(), (size_t)m_length - range.end());
    buffer[len] = '\0';
    return rtn->toSlice();
}

int StringSlice::countSlice(const StringSlice& target) const
{
    if (target.length() > m_length)
        return 0;

    const char* t = target.internalBytes();
    int tLength = target.length();
    const char* s = m_str;
    const char* yEnd = m_str + m_length - tLength;

    /* Brutal force Searching */
    // TODO: replace with a better algorithm at http://www-igm.univ-mlv.fr/~lecroq/string/
    int count = 0;
    for (; s <= yEnd; s++)
    {
        if (memcmp(t, s, tLength) == 0)
        {
            count++;
        }
    }

    return count;
}

bool StringSlice::equalsCaseInsensitive(const StringSlice& r) const
{
#ifdef NC_OS_WIN
    return m_length == r.m_length && _stricmp(m_str, r.m_str) == 0;
#else
    return m_length == r.m_length && strcasecmp(m_str, r.m_str) == 0;
#endif
}

const StringSlice& StringSlice::operator=(const StringSlice& r)
{
    m_str = r.m_str;
    m_length = r.m_length;

    retain(r.m_ncstring);
    release(m_ncstring);
    m_ncstring = r.m_ncstring;
    return *this;
}

//////////////////////////////////////////////////////////////////////////

StringSlice StringSlice::makeWithString(NcString* str)
{
    return StringSlice(str->cstr(), str->length(), retain(str));
}

StringSlice StringSlice::format(const char* format, ...)
{
    va_list va;
    va_start(va, format);
    auto str = NcString::formatVa(format, va);
    va_end(va);
    return StringSlice::makeWithString(str.get());
}

StringSlice StringSlice::join(const std::vector<StringSlice>& slices)
{
    auto str = NcString::allocByJoiningSlices(slices, *this);
    return StringSlice::makeWithString(str.get());
}

StringSlice StringSlice::join(StringSlice* slices, size_t sliceCount)
{
    auto str = NcString::allocByJoiningSlices(slices, sliceCount, *this);
    return StringSlice::makeWithString(str.get());
}

StringSlice::~StringSlice()
{
    release((NcObject*)m_ncstring);
}

StringSlice::StringSlice(const StringSlice& r) noexcept
{
    m_str = r.m_str;
    m_length = r.m_length;
    m_ncstring = retain(r.m_ncstring);
}

StringSlice::StringSlice(const std::string& r) noexcept
{
    sp<NcString> str = NcString::allocWithBytes(r.c_str(), r.length());
    m_ncstring = retain(str.get());
    m_str = str->cstr();
    m_length = str->length();
}

StringSlice StringSlice::make(const char* buffer)
{
    StringSlice o;
    sp<NcString> str = NcString::allocWithCString(buffer);
    o.m_ncstring = retain(str.get());
    o.m_str = str->cstr();
    o.m_length = str->length();

    return o;
}

StringSlice StringSlice::makeWithBytes(const char* buffer, size_t size)
{
    StringSlice o;
    sp<NcString> str = NcString::allocWithBytes(buffer, size);
    o.m_ncstring = retain(str.get());
    o.m_str = str->cstr();
    o.m_length = str->length();

    return o;
}

StringSlice StringSlice::makeByTakingBytes(char* buffer, size_t size)
{
    StringSlice o;
    sp<NcString> str = NcString::allocByTakingBytes(buffer, size);
    o.m_ncstring = retain(str.get());
    o.m_str = buffer;
    o.m_length = (int)size;

    return o;
}

Range StringSlice::findFrom(int start, wchar32 code)
{
    auto iter = this->subslice(start, m_length - start).iter();
    wchar32 c;
    int consumedBytes;
    int totalBytes = 0;
    while (iter.next(&c, &consumedBytes))
    {
        if (c == code)
        {
            return Range(start + totalBytes, consumedBytes);
        }
        totalBytes += consumedBytes;
    }

    return invalidRange;
}

int StringSlice::rfind(char c)
{
    for (int i = m_length - 1; i >= 0; i--)
    {
        if (m_str[i] == c)
        {
            return i;
        }
    }

    return -1;
}

StringSlice StringSlice::subslice(int start, int length)
{
    return StringSlice(m_str + start, length, retain(m_ncstring));
}

StringSlice StringSlice::subslice(Range range)
{
    return StringSlice(m_str + range.location, range.length, retain(m_ncstring));
}

const StringSlice& StringSlice::operator=(const std::string& r)
{
    sp<NcString> str = NcString::allocWithBytes(r.c_str(), r.length());
    release(m_ncstring);
    m_ncstring = retain(str.get());
    m_str = str->cstr();
    m_length = str->length();
    return *this;
}

Range StringSlice::findSliceFrom(int start, const StringSlice& needle)
{
    const char* strEnd = m_str + m_length;
    const char* newStr = _strstr(m_str + start, strEnd, needle.m_str, needle.m_length);
    if (newStr == strEnd)
    {
        return invalidRange;
    }

    return Range((int)(newStr - m_str), needle.m_length);
}

bool StringTokenIter::next(StringSlice* sliceOut, Range* rangeOut)
{
    if (m_str == m_strEnd)
        return false;

    // find first none-sep
    const char* firstNonSep = NULL;
    for (; m_str != m_strEnd; m_str++)
    {
        if (memchr(m_sep, *m_str, m_sepLength) == NULL)
        {
            firstNonSep = m_str;
            break;
        }
    }

    if (firstNonSep == NULL)
        return false;

    // find first sep
    m_str++;
    for (; m_str != m_strEnd; m_str++)
    {
        if (memchr(m_sep, *m_str, m_sepLength) != NULL)
        {
            break;
        }
    }

    *sliceOut = StringSlice(firstNonSep, (int)(m_str - firstNonSep), retain(m_ncstring));
    if (rangeOut != NULL)
        *rangeOut = Range((int)(firstNonSep - m_strBegin), sliceOut->length());

    return true;
}
