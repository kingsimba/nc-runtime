#pragma once

#include "nc_object.h"

class NcString;
class StringSlice;

/**
 * Iterate characters(as Unicode Code Points) in a string
 *
 * ```
 * auto iter = StringSlice(u8"hello world").iter();
 * wchar32 c;
 * while(iter.next(&c)) {
 *     use(c);
 * }
 * ```
 */
class StringCharIter {
public:
  StringCharIter(const StringSlice& slice);

  bool next(wchar32* cOut) { return next(cOut, NULL); }
  bool next(wchar32* cOut, int* consumedBytesOut);

private:
  const char* m_str;
  int m_length;
};

/**
 * Split a string with a separate string
 *
 * For example. "hello---world".split("---") shall become "hello" and "world"
 *
 * ```
 * auto iter = StringSlice("hello---world").iterBySpliting("---");
 * StringSlice slice;
 * while(iter.next(&slice)) {
 *     use(slice);
 * }
 * ```
 */
class StringSubsliceIter {
public:
  StringSubsliceIter(const StringSlice& slice, const StringSlice& sep);

  bool next(StringSlice* slice_out) { return next(slice_out, NULL); }
  bool next(StringSlice* slice_out, Range* rangeOut);

private:
  const char* m_strBegin;
  const char* m_strEnd;
  const char* m_str;

  const char* m_sep;
  int m_sepLength;
};

/**
 * StringSlice uses UTF-8 encoding
 */
class StringSlice {
public:
  forceinline StringSlice() {
    m_str = NULL;
    m_length = 0;
  }
  forceinline StringSlice(const char* str) {
    m_str = (char*)str;
    m_length = (int)strlen(str);
  }
  forceinline StringSlice(const char* str, int len) {
    m_str = (char*)str;
    m_length = len;
  }

  forceinline void init(const char* str, int len) {
    m_str = (char*)str;
    m_length = len;
  }

  //////////////////////////////////////////////////////////////////////////
  // Accessors

  forceinline const char* bytes() const { return m_str; }
  forceinline int length() const { return m_length; }

  forceinline StringCharIter iter() const { return StringCharIter(*this); }
  forceinline StringSubsliceIter iterBySpliting(StringSlice sep) const { return StringSubsliceIter(*this, sep); }

  //////////////////////////////////////////////////////////////////////////
  // Conversions

  // copy to C string
  forceinline void toCString(char* str, size_t max_len) {
    if (max_len < (size_t)m_length + 1) {
      str[0] = 0;
      return;
    }
    memcpy(str, m_str, m_length);
    str[m_length] = 0;
  }

  // Create a standalone NcString
  sp<NcString> toString();

  //////////////////////////////////////////////////////////////////////////
  // Search

  // Find subslice
  forceinline Range findSlice(StringSlice needle) { return findSliceFrom(0, needle); }
  Range findSliceFrom(int start, StringSlice needle);

  // Find Unicode character
  forceinline Range find(wchar32 code) { return findFrom(0, code); }
  Range findFrom(int start, wchar32 code);

  // Find ANSI character.
  // If the string contains multi-byte character in UTF8, it will misbehave.
  forceinline int find(char c) { return findFrom(0, c); }
  int findFrom(int start, char c);
  int rfind(char c);

  //////////////////////////////////////////////////////////////////////////
  // Actions

  forceinline StringSlice subslice(int start, int length) { return StringSlice(m_str + start, length); }
  std::vector<StringSlice> split(StringSlice sep);

  //////////////////////////////////////////////////////////////////////////
  // Equals

  forceinline bool equals(StringSlice* r) { return m_length == r->m_length && memcmp(m_str, r->m_str, m_length) == 0; }
  forceinline bool equals(const char* r) { return m_length == strlen(r) && memcmp(m_str, r, m_length) == 0; }

protected:
  char* m_str;
  int m_length;
};

inline StringCharIter::StringCharIter(const StringSlice& slice) {
  m_str = slice.bytes();
  m_length = slice.length();
}

inline StringSubsliceIter::StringSubsliceIter(const StringSlice& slice, const StringSlice& sep) {
  m_str = m_strBegin = slice.bytes();
  m_strEnd = m_str + slice.length();
  m_sep = sep.bytes();
  m_sepLength = sep.length();
}

inline int StringSlice::findFrom(int start, char c) {
  for (const char* p = m_str + start; *p; p++) {
    if (*p == c) return (int)(p - m_str);
  }
  return -1;
}
