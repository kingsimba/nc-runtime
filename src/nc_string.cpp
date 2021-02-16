#include "stdafx_nc_runtime.h"
#include "nc_string.h"

sptr<NcString> NcString::allocWithBytes(const char* str, size_t len) {
  char* buffer;
  auto rtn = allocButFillContentLater(len, &buffer);
  memcpy(buffer, str, len);
  buffer[len] = 0;
  return rtn;
}

sptr<NcString> NcString::allocButFillContentLater(size_t strLength, char** strOut) {
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
    free(m_str);
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
