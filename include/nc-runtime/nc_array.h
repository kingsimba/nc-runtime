#pragma once

#include "basic_types.h"
#include "nc_string.h"

template <typename T>
class NcArray : public NcObject {
public:
  static sp<NcArray<T>> alloc() { return new NcArray(); }

  void addObject(const sp<T>& obj) { this->m_array.push_back(obj); }
  sp<T> objectAtIndex(int i) { return this->m_array[i]; }

  // from NcObject
  virtual sp<NcString> toString() override { return NcString::allocWithCString("This is an array"); }

private:
  std::vector< sp<T> > m_array;
};

template <typename T>
class NcArrayRef {
public:
  static NcArrayRef<T> alloc() { return NcArray<T>::alloc(); }
  NcArrayRef(sp<NcArray<T>>& arr) : m_array(arr) {}
  NcArrayRef(sp<NcArray<T>>&& arr) : m_array(std::move(arr)) { }

  // allow using []
  sp<T> operator[](size_t i) { return m_array->objectAtIndex((int)i); }

  NcArray<T>* operator->() { return m_array.get(); }

private:
  sp<NcArray<T>> m_array;
};
