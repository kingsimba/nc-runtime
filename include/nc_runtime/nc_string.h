#pragma once

#include "nc_types.h"
#include "nc_object.h"
#include "string_slice.h"

template <typename T>
class NcArray;

class NcString : public NcObject
{
public:
    NcString() = default;
    ~NcString() = default;

    /**
     * Create a string by C string. |str| must ends with \0.
     */
    forceinline static sp<NcString> allocWithCString(const char* str) { return allocWithBytes(str, strlen(str)); }

    /**
     * Create a string by copying bytes. |str| could not ends with \0.
     */
    static sp<NcString> allocWithBytes(const char* str, size_t len);

    static sp<NcString> allocWithSlice(const StringSlice& slice);

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
        auto o = alloc<NcString>();
        o->initByTakingBytes(str, len);
        return o;
    }

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
        auto o = alloc<NcString>();
        o->initByJoiningSlices(&slices[0], slices.size(), sep);
        return o;
    }

    static sp<NcString> allocByJoiningSlices(StringSlice* slices, size_t sliceCount, const StringSlice& sep)
    {
        auto o = alloc<NcString>();
        o->initByJoiningSlices(slices, sliceCount, sep);
        return o;
    }

    static sp<NcString> allocByJoiningStrings(NcArray<NcString>* strs, const StringSlice& sep)
    {
        auto o = alloc<NcString>();
        o->initByJoiningStrings(strs, sep);
        return o;
    }

    static sp<NcString> format(const char* format, ...);

    //////////////////////////////////////////////////////////////////////////
    /// Accessors

    forceinline const char* cstr() { return m_str; }
    forceinline int length() { return m_length; }

    forceinline StringSlice subslice(int start, int length) { return toSlice().subslice(start, length); }
    forceinline StringSlice subsliceInRange(Range range) { return toSlice().subsliceInRange(range); }
    forceinline StringSlice subsliceFrom(int start) { return toSlice().subsliceFrom(start); }

    //////////////////////////////////////////////////////////////////////////
    /// Operations

    forceinline StringSlice toSlice() { return StringSlice::makeWithString(this); }
    forceinline StringSlice toEphemeralSlice() { return StringSlice(m_str, m_length, NULL); }

    // Design Notes: overwrite functions in StringSlice. Because we have to keep the RC
    forceinline std::vector<StringSlice> split(const StringSlice& sep) { return this->toSlice().split(sep); }

    forceinline int splitWithLimit(const StringSlice& sep, StringSlice* slicesOut, int maxNum)
    {
        return toSlice().splitWithLimit(sep, slicesOut, maxNum);
    }

    forceinline sp<NcString> join(NcArray<NcString>* strs)
    {
        return NcString::allocByJoiningStrings(strs, this->toSlice());
    }

    forceinline bool startsWith(const StringSlice& r) { return toEphemeralSlice().startsWith(r); }
    forceinline bool endsWith(const StringSlice& r) { return toEphemeralSlice().endsWith(r); }

    forceinline bool equalsCaseInsensitive(const StringSlice& r) { return toEphemeralSlice().equalsCaseInsensitive(r); }

    // from NcObject
    virtual StringSlice toString() override { return this->toSlice(); }
    virtual bool equals(NcObject* r) override;
    forceinline bool equals(const StringSlice& r) { return this->toEphemeralSlice().equals(r); }
    forceinline bool equals(const char* r) { return this->toEphemeralSlice().equals(r); }

protected:
    void initByTakingBytes(char* str, size_t len)
    {
        m_str = str;
        m_length = (int)len;
    }

    void initByJoiningSlices(const StringSlice* slices, size_t count, const StringSlice& sep);
    void initByJoiningStrings(NcArray<NcString>* strs, const StringSlice& sep);

private:
    const char* m_str = NULL;
    int m_length = 0;
};

forceinline sp<NcString> operator""_str(const char* literalStr, size_t len)
{
    return NcString::allocWithBytes(literalStr, len);
}

forceinline bool operator==(NcString* str, const StringSlice& r)
{
    return str->equals(r);
}