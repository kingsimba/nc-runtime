#include "stdafx_nc_runtime.h"
#include "nc_runtime/nc_string.h"
#include "nc_runtime/nc_stdlib.h"
#include <mutex>
#include <unordered_map>
#include <stdarg.h>

using namespace std;

// create one NcString object for each literal string
class GlobalStringManager
{
  public:
    sp<NcString> addString(const char* cstr, size_t len)
    {
        static mutex s_mutex;

        sp<NcString> str;

        s_mutex.lock();
        auto iter = m_map.find((size_t)cstr);
        if (iter == m_map.end())
        {
            str = NcString::allocWithLiteralCString(cstr, len);
            m_map.insert(iter, std::make_pair((size_t)cstr, str));
        }
        else
        {
            str = iter->second;
        }
        s_mutex.unlock();

        return str;
    }

    ~GlobalStringManager()
    {
        auto iter = m_map.begin();
        while (iter != m_map.end())
        {
            NcObject* obj = iter->second;
            obj->_deleteThis();
            iter++;
        }
    }

  private:
    unordered_map<size_t, NcString*> m_map;
};

static GlobalStringManager g_stringManager;

sp<NcString> operator"" _str(const char* literalStr, size_t len)
{
    return g_stringManager.addString(literalStr, len);
}

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

sp<NcString> NcString::allocButFillContentLater(size_t strLength, char** strOut)
{
    size_t totalLen = sizeof(NcString) + strLength + 1;
    NcString* o = (NcString*)NcObject::allocRawObjectWithSize(totalLen, false);
    ::new (o) NcString();
    char* buffer = (char*)(o + 1);
    o->m_str = buffer;
    o->m_length = (int)strLength;
    o->m_shouldFree = false;
    *strOut = buffer;
    return o;
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

NcString::~NcString()
{
    if (m_shouldFree)
    {
        free((char*)m_str);
    }
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
    totalLen = 0;
    for (size_t i = 0; i < sliceCount; i++)
    {
        const StringSlice& s = slices[i];
        memcpy(str + totalLen, s.bytes(), s.length());
        totalLen += s.length();
        if (i != sliceCount - 1)
        {
            memcpy(str + totalLen, sep.bytes(), sep.length());
            totalLen += sep.length();
        }
    }
    str[totalLen] = '\0';

    initByTakingBytes(str, totalLen);
}
