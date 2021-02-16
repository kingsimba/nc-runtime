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
  StringSubsliceIter(StringSubsliceIter&& r) noexcept {
    this->m_strBegin = r.m_strBegin;
    this->m_strEnd = r.m_strEnd;
    this->m_str = r.m_str;
    this->m_ncstring = r.m_ncstring;
    this->m_sep = r.m_sep;
    this->m_sepLength = r.m_sepLength;
    r.m_ncstring = NULL;
  }
  ~StringSubsliceIter();

  bool next(StringSlice* slice_out) { return next(slice_out, NULL); }
  bool next(StringSlice* slice_out, Range* rangeOut);

private:
  const char* m_strBegin;
  const char* m_strEnd;
  const char* m_str;
  NcString* m_ncstring;

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
    m_ncstring = NULL;
  }
  StringSlice(const StringSlice& r) noexcept;
  forceinline StringSlice(StringSlice&& r) noexcept {
    m_str = r.m_str;
    m_length = r.m_length;
    m_ncstring = r.m_ncstring;
    r.m_ncstring = NULL;
  }
  forceinline StringSlice(const char* str) {
    m_str = (char*)str;
    m_length = (int)strlen(str);
    m_ncstring = NULL;
  }
  forceinline StringSlice(const char* str, int len) {
    m_str = (char*)str;
    m_length = len;
    m_ncstring = NULL;
  }
  forceinline StringSlice(const char* str, int len, NcString* internalStr) {
    m_ncstring = NULL;
    initWithString(str, len, internalStr);
  };
  StringSlice(NcString* str);
  forceinline ~StringSlice() { release((NcObject*)m_ncstring); }

  void initWithString(const char* str, int len, NcString* ncstring);

  //////////////////////////////////////////////////////////////////////////
  // Accessors

  forceinline const char* bytes() const { return m_str; }
  forceinline int length() const { return m_length; }

  forceinline StringCharIter iter() const { return StringCharIter(*this); }
  forceinline StringSubsliceIter iterBySpliting(const StringSlice& sep) const { return StringSubsliceIter(*this, sep); }

  forceinline bool startsWith(const StringSlice& r) {
    return m_length >= r.m_length && memcmp(m_str, r.m_str, r.m_length) == 0;
  }
  forceinline bool endsWith(const StringSlice& r) {
    return m_length >= r.m_length && memcmp(m_str + m_length - r.m_length, r.m_str, r.m_length) == 0;
  }

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
  forceinline Range findSlice(const StringSlice& needle) { return findSliceFrom(0, needle); }
  Range findSliceFrom(int start, const StringSlice& needle);

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

  /**
   * Create a subslice
   */
  forceinline StringSlice subslice(int start, int length) { return StringSlice(m_str + start, length, m_ncstring); }
  forceinline StringSlice subsliceInRange(Range range) {
    return StringSlice(m_str + range.location, range.length, m_ncstring);
  }

  /**
   * Negative number means from the end. For example "hello".subsliceFrom(-3) == "llo"
   */
  StringSlice subsliceFrom(int start);

  /**
   * Split into pieces with a separator string
   * 
   * split "hello--world" with "--" will produce ["hello", "world"]
   */
  std::vector<StringSlice> split(const StringSlice& sep);

  /**
    * A fast on stack version. return the number of slices actually created
    */
  int splitWithLimit(const StringSlice& sep, StringSlice* slicesOut, int maxNum);

  /**
   * Replace part of a string
   */
  sp<NcString> stringByReplacingStringInRange(Range range, const StringSlice& replacement);

  //////////////////////////////////////////////////////////////////////////
  // Equals

  forceinline bool equals(StringSlice* r) { return m_length == r->m_length && memcmp(m_str, r->m_str, m_length) == 0; }
  forceinline bool equals(const char* r) { return m_length == strlen(r) && memcmp(m_str, r, m_length) == 0; }

  //////////////////////////////////////////////////////////////////////////
  // private

  forceinline NcString* internalString() const { return m_ncstring; }
  forceinline const StringSlice& operator=(const StringSlice& r) {
    this->initWithString(r.m_str, r.m_length, r.m_ncstring);
    return *this;
  }

protected:
  char* m_str;
  int m_length;
  NcString* m_ncstring;
};

inline StringCharIter::StringCharIter(const StringSlice& slice) {
  m_str = slice.bytes();
  m_length = slice.length();
}

inline int StringSlice::findFrom(int start, char c) {
  for (const char* p = m_str + start; *p; p++) {
    if (*p == c) return (int)(p - m_str);
  }
  return -1;
}

inline StringSlice operator""_s(const char* literalStr, size_t len) { return StringSlice(literalStr, len); }
