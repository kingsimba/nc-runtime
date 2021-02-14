#pragma once

#include "basic_types.h"
#include "string_slice.h"

class NcString : public NcObject, public StringSlice {
public:
  static sp<NcString> allocWithCString(const char* str) {
    auto o = std::make_shared<NcString>();
    o->initWithCString(str);
    return o;
  }
  static sp<NcString> allocWithBytes(const char* str, size_t len) {
    auto o = std::make_shared<NcString>();
    o->initWithBytes(str, len);
    return o;
  }
  static sp<NcString> allocWithSlice(StringSlice str) {
    auto o = std::make_shared<NcString>();
    o->initWithBytes(str.bytes(), str.length());
    return o;
  }
  static sp<NcString> allocByJoiningSlices(const std::vector<StringSlice>& slices, StringSlice sep) {
    auto o = std::make_shared<NcString>();
    o->initByJoiningSlices(slices, sep);
    return o;
  }

  forceinline const char* cstr() { return m_str; }
  forceinline int length() { return m_length; }

  // from NcObject
  virtual sp<NcString> toString() override { return sp<NcString>(this); }

  NcString() : m_internal(false) {}
  ~NcString();

private:
  void initWithCString(const char* str);
  void initWithBytes(const char* str, size_t len);
  void initByJoiningSlices(const std::vector<StringSlice>& slices, StringSlice sep);

private:
  bool m_internal;
};
