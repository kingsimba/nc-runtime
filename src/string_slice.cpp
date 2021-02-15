#include "stdafx_nc_runtime.h"
#include "string_slice.h"
#include "jansson/utf.h"
#include "nc_string.h"

bool StringCharIter::next(wchar32* cOut, int* consumedBytesOut) {
  int32_t c;

  const char* newStr = utf8_iterate(m_str, m_length, &c);
  if (newStr == m_str) {
    return false;
  }

  int consumedBytes = (int)(newStr - m_str);
  m_length -= consumedBytes;
  m_str = newStr;
  *cOut = c;
  if (consumedBytesOut != NULL) *consumedBytesOut = consumedBytes;

  return true;
}

//////////////////////////////////////////////////////////////////////////
static const char* _strchr(const char* s, const char* sEnd, char c) {
  for (; s != sEnd; s++) {
    if (*s == c) return s;
  }
  return s;
}

static int _strncmp(const char* s1, const char* s1End, const char* s2, size_t s2len) {
  unsigned char u1, u2;
  while (s2len-- > 0 && s1 != s1End) {
    u1 = (unsigned char)*s1++;
    u2 = (unsigned char)*s2++;
    if (u1 != u2) return u1 - u2;
    if (u1 == '\0') return 0;
  }
  return 0;
}

static const char* _strstr(const char* s1, const char* s1End, const char* s2, size_t s2len) {
  const char* p = s1;
  for (; (p = _strchr(p, s1End, *s2)) != NULL; p++) {
    if (_strncmp(p, s1End, s2, s2len) == 0) return p;
  }
  return p;
}

bool StringSubsliceIter::next(StringSlice* cOut, Range* rangeOut) {
  if (m_str == m_strEnd) return false;

  const char* newStr = _strstr(m_str, m_strEnd, m_sep, m_sepLength);

  *cOut = StringSlice(m_str, (int)(newStr - m_str));
  if (rangeOut != NULL) *rangeOut = Range_make((int)(m_str - m_strBegin), cOut->length());

  if (newStr != m_strEnd) {
    m_str = newStr + m_sepLength;
  } else {
    m_str = newStr;
  }

  return true;
}

std::vector<StringSlice> StringSlice::split(StringSlice sep) {
  std::vector<StringSlice> sv;
  auto iter = this->iterBySpliting(sep);
  StringSlice slice;
  while (iter.next(&slice)) {
    sv.push_back(slice);
  }
  return sv;
}

//////////////////////////////////////////////////////////////////////////

Range StringSlice::findFrom(int start, wchar32 code) {
  auto iter = this->subslice(start, m_length - start).iter();
  wchar32 c;
  int consumedBytes;
  int totalBytes = 0;
  while (iter.next(&c, &consumedBytes)) {
    if (c == code) {
      return Range_make(start + totalBytes, consumedBytes);
    }
    totalBytes += consumedBytes;
  }

  return invalidRange;
}

int StringSlice::rfind(char c) {
  for (int i = m_length - 1; i >= 0; i--) {
    if (m_str[i] == c) {
      return i;
    }
  }

  return -1;
}

Range StringSlice::findSliceFrom(int start, StringSlice needle) {
  const char* strEnd = m_str + m_length;
  const char* newStr = _strstr(m_str + start, strEnd, needle.m_str, needle.m_length);
  if (newStr == strEnd) {
    return invalidRange;
  }

  return Range_make((int)(newStr - m_str), needle.m_length);
}

sptr<NcString> StringSlice::toString() { return NcString::allocWithBytes(m_str, m_length); }
