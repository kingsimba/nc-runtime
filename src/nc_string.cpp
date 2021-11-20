#include "stdafx_nc_runtime.h"
#include "nc_runtime/nc_string.h"
#include "nc_runtime/nc_stdlib.h"
#include "nc_runtime/nc_array.h"
#include <mutex>
#include <unordered_map>
#include <stdarg.h>

using namespace std;

//////////////////////////////////////////////////////////////////////////
// NcString

sp<NcString> NcString::allocWithBytes(const char* str, size_t len)
{
    char* buffer;
    auto rtn = allocButFillContentLater(len, &buffer);
    memcpy(buffer, str, len);
    buffer[len] = 0;
    return rtn;
}

sp<NcString> NcString::allocWithSlice(const StringSlice& slice)
{
    NcString* internalStr = slice.internalString();
    if (internalStr != NULL && slice.length() == internalStr->length())
        return retainToSp(internalStr);
    return NcString::allocWithBytes(slice.internalBytes(), slice.length());
}

sp<NcString> NcString::allocButFillContentLater(size_t strLength, char** strOut)
{
    size_t totalLen = sizeof(NcString) + strLength + 1;
    NcString* o = (NcString*)NcObject::allocRawObjectWithSize(totalLen, false);
    ::new (o) NcString();
    char* buffer = (char*)(o + 1);
    o->m_str = buffer;
    o->m_length = (int)strLength;
    *strOut = buffer;
    return sp<NcString>::takeRaw(o);
}

sp<NcString> NcString::format(const char* format, ...)
{
    va_list va;
    va_start(va, format);
    size_t len = vsnprintf(NULL, 0, format, va);
    va_end(va);
    va_start(va, format);
    char* buffer;
    auto o = NcString::allocButFillContentLater(len, &buffer);
    vsnprintf(buffer, len + 1, format, va);
    va_end(va);
    return o;
}

sp<NcString> NcString::formatVa(const char* format, va_list va)
{
    va_list vaCopy;
    va_copy(vaCopy, va);
    size_t len = vsnprintf(NULL, 0, format, va);
    char* buffer;
    auto o = NcString::allocButFillContentLater(len, &buffer);
    vsnprintf(buffer, len + 1, format, vaCopy);
    va_end(vaCopy);
    return o;
}

bool NcString::equals(NcObject* r)
{
    if (this == r)
        return true;
    if (r->isKindOf<NcString>())
    {
        NcString* rstr = (NcString*)r;
        return m_length == rstr->m_length && memcmp(m_str, rstr->m_str, m_length) == 0;
    }
    return false;
}

void NcString::initByJoiningSlices(const StringSlice* slices, size_t sliceCount, const StringSlice& sep)
{
    if (sliceCount == 0)
    {
        m_str = "";
        return;
    }

    size_t totalLen = 0;
    for (size_t i = 0; i < sliceCount; i++)
    {
        const StringSlice& s = slices[i];
        totalLen += s.length();
    }
    totalLen += sep.length() * (sliceCount - 1);

    char* str = (char*)malloc(totalLen + 1);
    if (str != NULL)
    {
        totalLen = 0;
        for (size_t i = 0; i < sliceCount; i++)
        {
            const StringSlice& s = slices[i];
            memcpy(str + totalLen, s.internalBytes(), s.length());
            totalLen += s.length();
            if (i != sliceCount - 1)
            {
                memcpy(str + totalLen, sep.internalBytes(), sep.length());
                totalLen += sep.length();
            }
        }
        str[totalLen] = '\0';
    }

    initByTakingBytes(str, totalLen);
}

void NcString::initByJoiningStrings(NcArray<NcString>* strs, const StringSlice& sep)
{
    StackOrHeapAllocator allocator(nc_alloca(1024), 1024);

    int n = strs->size();
    StringSlice* slices = allocator.allocArray<StringSlice>(n);
    for (int i = 0; i < n; i++)
    {
        ::new (slices + i) StringSlice();
        slices[i] = strs->objectAtIndex(i)->toSlice();
    }
    initByJoiningSlices(slices, n, sep);
}
