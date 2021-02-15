#pragma once

#include "basic_types.h"
#include "nc_object.h"

template <typename T>
class NcArray : public NcObject {
public:
  static sptr<NcArray<T>> alloc() { return new NcArray(); }

  void addObject(const sptr<T>& obj) { this->array_.push_back(obj); }
  sptr<T> objectAtIndex(int i) { return this->array_[i]; }

  // from NcObject
  virtual sptr<NcString> toString() override { return NcString::allocWithCString("This is an array"); }

private:
  std::vector< sptr<T> > array_;
};

template <typename T>
class NcArrayRef {
public:
  static NcArrayRef<T> alloc() { return NcArray<T>::alloc(); }
  NcArrayRef(sptr<NcArray<T>>& arr) : array_(arr) {}
  sptr<T> operator[](size_t i) { return this->array_->objectAtIndex((int)i); }

  NcArray<T>* operator->() { return array_.get(); }

private:
  sptr<NcArray<T>> array_;
};
