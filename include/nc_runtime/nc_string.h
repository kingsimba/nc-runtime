#pragma once

#include "nc_types.h"
#include "string_slice.h"

class NcString : public NcObject, public StringSlice
{
public:
    /**
     * Create a string by C string. |str| must ends with \0.
     */
    static sp<NcString> allocWithCString(const char* str) { return allocWithBytes(str, strlen(str)); }

    /**
     * Create a string by copying bytes. |str| could not ends with \0.
     */
    static sp<NcString> allocWithBytes(const char* str, size_t len);

    /**
     * Create a string by taking the ownership of the memory.
     *
     * @param str
     *   |str| must be a valid C string(ends with \0) and created with malloc()/realloc()/calloc()
     *   and can be freed with free().
     * @param len
     *   length of the string |str|.
     */
    static sp<NcString> allocByTakingBytes(char* str, size_t len)
    {
        auto o = new NcString();
        o->initByTakingBytes(str, len);
        return o;
    }

    /**
     * Create a string from a slice. Same as StringSlice::toString().
     */
    static sp<NcString> allocWithSlice(const StringSlice& str) { return allocWithBytes(str.bytes(), str.length()); }

    /**
     * Return a partially created String.
     *
     * @remarks
     *   This is an optimization. The string object and the text buffer are created with a single malloc().
     *   The user should fill the content right after, including \0.
     */
    static sp<NcString> allocButFillContentLater(size_t strLength, char** strOut);

    /**
     * Create a string by joining pieces with a separator.
     * For example: joining ["hello", "world"] with "---" will produce "hello---world".
     */
    static sp<NcString> allocByJoiningSlices(const std::vector<StringSlice>& slices, const StringSlice& sep)
    {
        auto o = new NcString();
        o->initByJoiningSlices(&slices[0], slices.size(), sep);
        return o;
    }

    static sp<NcString> allocByJoiningSlices(StringSlice* slices, size_t sliceCount, const StringSlice& sep)
    {
        auto o = new NcString();
        o->initByJoiningSlices(slices, sliceCount, sep);
        return o;
    }

    static sp<NcString> allocWithLiteralCString(const char* str, size_t len)
    {
        auto o = new NcString(true);
        o->m_str = (char*)str;
        o->m_length = (int)len;
        o->m_shouldFree = false;
        return o;
    }

    static sp<NcString> format(const char* format, ...);

    //////////////////////////////////////////////////////////////////////////
    /// Accessors

    forceinline const char* cstr() { return m_str; }
    forceinline int length() { return m_length; }

    forceinline StringSlice subslice(int start, int length) { return this->toSlice().subslice(start, length); }
    forceinline StringSlice subsliceInRange(Range range) { return this->toSlice().subsliceInRange(range); }
    forceinline StringSlice subsliceFrom(int start) { return this->toSlice().subsliceFrom(start); }

    //////////////////////////////////////////////////////////////////////////
    /// Operations

    forceinline StringSlice toSlice() { return StringSlice(this); }

    // Design Notes: overwrite functions in StringSlice. Because we have to keep the RC
    forceinline std::vector<StringSlice> split(const StringSlice& sep) { return this->toSlice().split(sep); }

    forceinline int splitWithLimit(const StringSlice& sep, StringSlice* slicesOut, int maxNum)
    {
        return toSlice().splitWithLimit(sep, slicesOut, maxNum);
    }

    // from NcObject
    virtual sp<NcString> toString() override { return sp<NcString>(this); }
    virtual bool equals(NcObject* r) override;
    forceinline bool equals(const StringSlice& r) { return StringSlice::equals(r); }
    forceinline bool equals(const char* r) { return StringSlice::equals(r); }

private:
    NcString() : m_shouldFree(false) {}
    NcString(bool isStatic) : NcObject(true), m_shouldFree(false) { UNUSED_VAR(isStatic); }
    ~NcString();

    void initByTakingBytes(char* str, size_t len)
    {
        m_str = str;
        m_length = (int)len;
        m_shouldFree = true;
    }

    void initByJoiningSlices(const StringSlice* slices, size_t sliceCount, const StringSlice& sep);

private:
    bool m_shouldFree;
};

sp<NcString> operator""_str(const char* literalStr, size_t len);
