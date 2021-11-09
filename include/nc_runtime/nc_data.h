#pragma once

#include "nc_object.h"
#include "nc_string.h"

class NcData : public NcObject
{
public:
    NcData() = default;
    ~NcData() = default;

    static forceinline NcData makeWithBytesNoCopy(const void* bytes, size_t len)
    {
        NcData o;
        o.m_bytes = (void*)bytes;
        o.m_length = len;
        return o;
    }

    static sp<NcData> allocWithBytes(const void* bytes, size_t len);
    static sp<NcData> allocByTakingBytes(void* bytes, size_t len);
    static sp<NcData> allocWithContentsOfFile(NcString* fileName)
    {
        return allocWithContentsOfFile(fileName->toSlice());
    }
    static sp<NcData> allocWithContentsOfFile(const StringSlice& fileName);

    size_t length() { return m_length; }
    void* bytes() { return m_bytes; }

private:
    size_t m_length = 0;
    void* m_bytes = NULL;
    bool m_shouldFree = false;
};
