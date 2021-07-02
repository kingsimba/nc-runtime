#pragma once

#include "nc_object.h"
#include "nc_string.h"

class NcData : public NcObject
{
public:
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
    size_t m_length;
    void* m_bytes;
    bool m_shouldFree = false;
};
