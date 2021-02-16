#pragma once

#include "basic_types.h"
#include "nc_string.h"

template <typename T>
class NcArray : public NcObject {
public:
  static sp<NcArray<T>> alloc() { return new NcArray(); }

  void addObject(const sp<T>& obj) { this->array_.push_back(obj); }
  sp<T> objectAtIndex(int i) { return this->array_[i]; }

  // from NcObject
  virtual sp<NcString> toString() override { return NcString::allocWithCString("This is an array"); }

private:
  std::vector< sp<T> > array_;
};

template <typename T>
class NcArrayRef {
public:
  static NcArrayRef<T> alloc() { return NcArray<T>::alloc(); }
  NcArrayRef(sp<NcArray<T>>& arr) : array_(arr) {}
  NcArrayRef(sp<NcArray<T>>&& arr) : array_(std::move(arr)) { }

  // allow using []
  sp<T> operator[](size_t i) { return this->array_->objectAtIndex((int)i); }

  NcArray<T>* operator->() { return array_.get(); }

private:
  sp<NcArray<T>> array_;
};
