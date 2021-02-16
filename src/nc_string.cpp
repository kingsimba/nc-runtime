#include "stdafx_nc_runtime.h"
#include "nc_string.h"
#include "nc_stdlib.h"
#include <mutex>
#include <unordered_map>

using namespace std;

// create one NcString object for each literal string
class GlobalStringManager {
public:
  sp<NcString> addString(const char* cstr, size_t len) {
    static mutex s_mutex;

    sp<NcString> str;

    s_mutex.lock();
    auto iter = m_map.find((size_t)cstr);
    if (iter == m_map.end()) {
      str = NcString::allocWithLiteralCString(cstr, len);
      m_map.insert(iter, std::make_pair((size_t)cstr, str));
    } else {
      str = iter->second;
    }
    s_mutex.unlock();

    return str;
  }

  ~GlobalStringManager() {
    auto iter = m_map.begin();
    while (iter != m_map.end()) {
      iter->second->_deleteThis();
      iter++;
    }
  }

private:
  unordered_map<size_t, sp<NcString>> m_map;
};

static GlobalStringManager g_stringManager;

sp<NcString> operator"" _str(const char* literalStr, size_t len) { return g_stringManager.addString(literalStr, len); }

//////////////////////////////////////////////////////////////////////////
// NcString

sp<NcString> NcString::allocWithBytes(const char* str, size_t len) {
  char* buffer;
  auto rtn = allocButFillContentLater(len, &buffer);
  memcpy(buffer, str, len);
  buffer[len] = 0;
  return rtn;
}

sp<NcString> NcString::allocButFillContentLater(size_t strLength, char** strOut) {
  size_t totalLen = sizeof(NcString) + strLength + 1;
  NcString* o = (NcString*)malloc(totalLen);
  new (o) NcString();
  char* buffer = (char*)(o + 1);
  o->m_str = buffer;
  o->m_length = (int)strLength;
  o->m_shouldFree = false;
  *strOut = buffer;
  return o;
}

NcString::~NcString() {
  if (m_shouldFree) {
    free((char*)m_str);
  }
}

void NcString::initByJoiningSlices(const std::vector<StringSlice>& slices, const StringSlice& sep) {
  if (slices.empty()) {
    m_str = "";
    return;
  }

  size_t totalLen = 0;
  for (const auto& s : slices) {
    totalLen += s.length();
  }
  totalLen += sep.length() * (slices.size() - 1);

  char* str = (char*)malloc(totalLen + 1);
  totalLen = 0;
  for (size_t i = 0; i < slices.size(); i++) {
    const StringSlice& s = slices[i];
    memcpy(str + totalLen, s.bytes(), s.length());
    totalLen += s.length();
    if (i != slices.size() - 1) {
      memcpy(str + totalLen, sep.bytes(), sep.length());
      totalLen += sep.length();
    }
  }
  str[totalLen] = '\0';

  initByTakingBytes(str, totalLen);
}
