#include "stdafx_nc_runtime.h"
#include "nc_string.h"

NcString::~NcString() {
  if (m_internal) {
    free(m_str);
  }
}

void NcString::initWithCString(const char* str) {
  m_str = strdup(str);
  m_length = strlen(str);
  m_internal = true;
}

void NcString::initWithBytes(const char* str, size_t len) {
  m_str = (char*)malloc(len + 1);
  memcpy(m_str, str, len);
  m_str[len] = 0;
  m_length = (int)len;
  m_internal = true;
}

void NcString::initByJoiningSlices(const std::vector<StringSlice>& slices, StringSlice sep) {}
