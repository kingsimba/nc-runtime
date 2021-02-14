#pragma once

#include "basic_types.h"
#include "string_slice.h"

class NcString : public NcObject, public StringSlice {
public:

  /**
   * Create a string by C string. |str| must ends with \0.
   */
  static sp<NcString> allocWithCString(const char* str) {
    auto o = std::make_shared<NcString>();
    o->initWithCString(str);
    return o;
  }

  /**
   * Create a string by copying bytes. |str| could not ends with \0.
   */
  static sp<NcString> allocWithBytes(const char* str, size_t len) {
    auto o = std::make_shared<NcString>();
    o->initWithBytes(str, len);
    return o;
  }

  /**
   * Create a string by taking the ownership of the memory.
   * 
   * @param str
   *   |str| must be a valid C string(ends with \0) and created with malloc()/realloc()/calloc() 
   *   and can be freed with free(). 
   * @param len
   *   length of the string |str|.
   */
  static sp<NcString> allocByTakingBytes(char* str, size_t len) {
    auto o = std::make_shared<NcString>();
    o->initByTakingBytes(str, len);
    return o;
  }

  /**
   * Create a string from a slice. Same as StringSlice::toString().
   */
  static sp<NcString> allocWithSlice(StringSlice str) {
    auto o = std::make_shared<NcString>();
    o->initWithBytes(str.bytes(), str.length());
    return o;
  }

  /**
   * Create a string by joining pieces with a separator.
   * For example: joining ["hello", "world"] with "---" will produce "hello---world".
   */
  static sp<NcString> allocByJoiningSlices(const std::vector<StringSlice>& slices, StringSlice sep) {
    auto o = std::make_shared<NcString>();
    o->initByJoiningSlices(slices, sep);
    return o;
  }

  forceinline const char* cstr() { return m_str; }
  forceinline int length() { return m_length; }

  // from NcObject
  virtual sp<NcString> toString() override { return sp<NcString>(this); }

  NcString() : m_shouldFree(false) {}
  ~NcString();

private:
  void initWithCString(const char* str);
  void initWithBytes(const char* str, size_t len);
  void initByTakingBytes(char* str, size_t len) {
    m_str = str;
    m_length = (int)len;
    m_shouldFree = true;
  }
  
  void initByJoiningSlices(const std::vector<StringSlice>& slices, StringSlice sep);

private:
  bool m_shouldFree;
};
