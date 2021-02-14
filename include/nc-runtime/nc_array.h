#pragma once

#include "basic_types.h"

template <typename T>
class MyArray : public NcObject {
public:
  static sp<MyArray<T>> alloc() { return make_shared<MyArray<T>>(); }

  void addObject(sp<T> obj) { this->array_.push_back(obj); }
  sp<T> objectAtIndex(int i) { return this->array_[i]; }

  // from NcObject
  virtual sp<NcString> toString() override { return NcString::allocWithCString("This is an array"); }

private:
  svector<T> array_;
};

template <typename T>
class MyArrayRef {
public:
  MyArrayRef(sp<MyArray<T>> arr) : array_(arr) {}
  sp<T> operator[](size_t i) { return this->array_->objectAtIndex((int)i); }

  sp<MyArray<T>> operator->() { return array_; }

private:
  sp<MyArray<T>> array_;
};
