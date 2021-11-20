#pragma once
#include <nc_runtime/nc_types.h>

class NcString;
class StringSlice;

/**
 * Iterate characters(as Unicode Code Points) in a string
 *
 * ```
 * auto iter = StringSlice(u8"hello world").iter();
 * wchar32 c;
 * while(iter.next(&c)) {
 *     use(c);
 * }
 * ```
 */
class StringCharIter
{
public:
    StringCharIter(const StringSlice& slice);

    bool next(wchar32* cOut) { return next(cOut, NULL); }
    bool next(wchar32* cOut, int* consumedBytesOut);

private:
    const char* m_str;
    int m_length;
};

/**
 * Split a string with a separate string
 *
 * For example. "hello---world".split("---") shall become "hello" and "world"
 *
 * ```
 * auto iter = StringSlice("hello---world").iterBySpliting("---");
 * StringSlice slice;
 * while(iter.next(&slice)) {
 *     use(slice);
 * }
 * ```
 */
class StringSubsliceIter
{
public:
    StringSubsliceIter(const StringSlice& slice, const StringSlice& sep);
    StringSubsliceIter(StringSubsliceIter&& r) noexcept
    {
        this->m_strBegin = r.m_strBegin;
        this->m_strEnd = r.m_strEnd;
        this->m_str = r.m_str;
        this->m_ncstring = r.m_ncstring;
        this->m_sep = r.m_sep;
        this->m_sepLength = r.m_sepLength;
        this->m_foundSep = r.m_foundSep;
        r.m_ncstring = NULL;
    }
    ~StringSubsliceIter();

    forceinline bool next(StringSlice* sliceOut) { return next(sliceOut, NULL); }
    bool next(StringSlice* sliceOut, Range* rangeOut);

protected:
    const char* m_strBegin;
    const char* m_strEnd;
    const char* m_str;
    NcString* m_ncstring;

    const char* m_sep;
    int m_sepLength;
    bool m_foundSep;
};

/**
 * Split a string with a separator characters. It works like C function strtok().
 *
 * For example. "Whoa! Fireworks?".split(" |?") shall become ["Whoa" and "Fireworks"]
 *
 * ```
 * auto iter = StringSlice("hello---world").iterBySpliting("---");
 * StringSlice slice;
 * while(iter.next(&slice)) {
 *     use(slice);
 * }
 * ```
 */
class StringTokenIter : private StringSubsliceIter
{
public:
    StringTokenIter(const StringSlice& slice, const StringSlice& sep) : StringSubsliceIter(slice, sep) {}
    StringTokenIter(StringTokenIter&& r) noexcept : StringSubsliceIter(std::move(r)) {}

    forceinline bool next(StringSlice* sliceOut) { return next(sliceOut, NULL); }
    bool next(StringSlice* sliceOut, Range* rangeOut);
};

/**
 * StringSlice uses UTF-8 encoding
 */
class StringSlice
{
public:
    forceinline StringSlice()
    {
        m_str = NULL;
        m_length = 0;
        m_ncstring = NULL;
    }

    StringSlice(const StringSlice& r) noexcept;

    // ephemeral string slice
    forceinline StringSlice(const char* str);

    static StringSlice make(const char* buffer);                   // create a copy of C string
    static StringSlice makeEphemeral(const char* str);             // make NO copy of buffer
    static StringSlice makeEphemeral(const char* str, size_t len); // make NO copy of buffer
    static StringSlice makeWithBytes(const char* buffer, size_t size);
    static StringSlice makeByTakingBytes(char* buffer, size_t size);
    static StringSlice makeWithString(NcString* str);

    forceinline StringSlice(StringSlice&& r) noexcept
    {
        m_str = r.m_str;
        m_length = r.m_length;
        m_ncstring = r.m_ncstring;
        r.m_ncstring = NULL;
    }

    ~StringSlice();

    //////////////////////////////////////////////////////////////////////////
    // Accessors

    forceinline int length() const { return m_length; }

    // iterate Unicode points
    forceinline StringCharIter iter() const { return StringCharIter(*this); }
    // iterate slices separated by a |sep| string
    forceinline StringSubsliceIter iterForSplitting(const StringSlice& sep) const
    {
        return StringSubsliceIter(*this, sep);
    }
    // iterate slices separated by a groups of separators. Works like strtok()
    forceinline StringTokenIter iterForTokenizing(const StringSlice& seps) const
    {
        return StringTokenIter(*this, seps);
    }

    forceinline bool startsWith(const StringSlice& r)
    {
        return m_length >= r.m_length && memcmp(m_str, r.m_str, r.m_length) == 0;
    }
    forceinline bool endsWith(const StringSlice& r)
    {
        return m_length >= r.m_length && memcmp(m_str + m_length - r.m_length, r.m_str, r.m_length) == 0;
    }

    //////////////////////////////////////////////////////////////////////////
    // Conversions

    // copy to C string
    forceinline void toCString(char* str, size_t max_len) const
    {
        if (max_len < (size_t)m_length + 1)
        {
            str[0] = 0;
            return;
        }
        memcpy(str, m_str, m_length);
        str[m_length] = 0;
    }

    //////////////////////////////////////////////////////////////////////////
    // Search

    // Find subslice
    forceinline Range findSlice(const StringSlice& needle) { return findSliceFrom(0, needle); }
    Range findSliceFrom(int start, const StringSlice& needle);

    // Find Unicode character
    forceinline Range find(wchar32 code) { return findFrom(0, code); }
    Range findFrom(int start, wchar32 code);

    // Find ANSI character.
    // If the string contains multi-byte character in UTF8, it will misbehave.
    forceinline int find(char c) { return findFrom(0, c); }
    int findFrom(int start, char c);
    int rfind(char c);

    //////////////////////////////////////////////////////////////////////////
    // Actions

    /**
     * Create a subslice
     */
    StringSlice subslice(int start, int length);
    StringSlice subsliceInRange(Range range);

    /**
     * Negative number means from the end. For example "hello".subsliceFrom(-3) == "llo"
     */
    StringSlice subsliceFrom(int start);

    /**
     * Split into pieces with a separator string
     *
     * split "hello--world" with "--" will produce ["hello", "world"]
     */
    std::vector<StringSlice> split(const StringSlice& sep);

    /**
     * A fast on stack version. return the number of slices actually created
     */
    int splitWithLimit(const StringSlice& sep, StringSlice* slicesOut, int maxNum);

    /**
     * Split into tokens. With SliceTokenIterator.
     *
     * For "Whoa! Fireworks?", with |seps| = " ,.?!", the result is ["Whoa", "Fireworks"]
     */
    std::vector<StringSlice> tokenize(const StringSlice& seps);

    /**
     * Replace part of a string
     *
     * @rtn return the new string
     */
    StringSlice replaceInRange(Range range, const StringSlice& replacement);

    /**
     * Count the occurrences of target string
     */
    int countSlice(const StringSlice& target) const;

    //////////////////////////////////////////////////////////////////////////
    // Equals

    forceinline bool equals(const StringSlice& r) const
    {
        return m_length == r.m_length && memcmp(m_str, r.m_str, m_length) == 0;
    }

    bool equalsCaseInsensitive(const StringSlice& r) const;

    //////////////////////////////////////////////////////////////////////////
    // private

    // WARNING: it may not be null terminated
    forceinline const char* internalBytes() const { return m_str; }
    forceinline NcString* internalString() const { return m_ncstring; }
    const StringSlice& operator=(const StringSlice& r);

private:
    forceinline StringSlice(const char* str, size_t len, NcString* ncstr)
        : m_str(str), m_length((int)len), m_ncstring(ncstr)
    {
    }

    friend class NcString;
    friend class StringSubsliceIter;
    friend class StringTokenIter;

protected:
    const char* m_str;
    int m_length;
    NcString* m_ncstring;
};

// ephemeral string slice
forceinline StringSlice::StringSlice(const char* str)
{
    m_str = str;
    m_length = (int)strlen(str);
    m_ncstring = NULL;
}

forceinline StringSlice StringSlice::makeEphemeral(const char* str)
{
    return StringSlice(str, strlen(str), NULL);
}

forceinline StringSlice StringSlice::makeEphemeral(const char* str, size_t len)
{
    return StringSlice(str, len, NULL);
}

forceinline bool operator==(const StringSlice& l, const StringSlice& r)
{
    return l.equals(r);
}

inline StringCharIter::StringCharIter(const StringSlice& slice)
{
    m_str = slice.internalBytes();
    m_length = slice.length();
}

inline int StringSlice::findFrom(int start, char c)
{
    for (const char* p = m_str + start; *p; p++)
    {
        if (*p == c)
            return (int)(p - m_str);
    }
    return -1;
}

inline StringSlice operator""_s(const char* literalStr, size_t len)
{
    return StringSlice::makeEphemeral(literalStr, len);
}
