#include "stdafx_nc_runtime.h"
#include "nc_string.h"

NcString::~NcString() {
  if (m_shouldFree) {
    free(m_str);
  }
}

void NcString::initWithCString(const char* str) {
  m_str = strdup(str);
  m_length = strlen(str);
  m_shouldFree = true;
}

void NcString::initWithBytes(const char* str, size_t len) {
  char* copy = (char*)malloc(len + 1);
  memcpy(copy, str, len);
  copy[len] = 0;
  initByTakingBytes(copy, len);
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
