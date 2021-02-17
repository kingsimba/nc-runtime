#pragma once

#include "basic_types.h"
#include "nc_string.h"

template <typename T>
class NcArray : public NcObject {
public:
  static sp<NcArray<T>, T> alloc() { return new NcArray(); }

  forceinline int capacity() { return (int)m_array.capacity(); }
  forceinline int size() { return (int)m_array.size(); }
  forceinline void reserve(int newCapacity) { return m_array.reserve(newCapacity); }

  forceinline void addObject(const sp<T>& obj) { this->m_array.push_back(obj); }
  forceinline sp<T>& objectAtIndex(int i) { return this->m_array[i]; }

  /**
   * Find an object
   * 
   * @return
   *  return NULL if not found.
   * 
   * @remarks
   * ```
   * auto v = NcArray<NcString>::alloc();
   * v->addObject("hello"_str);
   * v->addObject("world"_str);
   * auto startWord = "w"_s;
   * auto obj = v->findWithCondition([&](NcString* v) {
   *   return v->startsWith(startWord);
   * });
   * ```
   */
  sp<T> findWithCondition(std::function<bool (T* obj)> func);

  /*
   * return -1 if not found. It uses NcObject::equals() for comparison
   */
  int indexOfObject(T* r) {
    return indexOfObjectWithCondition([=](T* obj) { return obj == r || obj->equals(r); });
  }
  
  int indexOfObjectWithCondition(std::function<bool(T* obj)> func);

  forceinline sp<T>& operator[](int index) {
    return m_array[index];
  }

private:
  std::vector< sp<T> > m_array;
};

template <typename T>
sp<T> NcArray<T>::findWithCondition(std::function<bool(T* obj)> func) {
  for (auto& obj : m_array) {
    if (func(obj.get())) {
      return obj;
    }
  }

  return NULL;
}

template <typename T>
int NcArray<T>::indexOfObjectWithCondition(std::function<bool(T* obj)> func) {
  int len = (int)m_array.size();
  for (int i = 0; i < len; i++) {
    if (func(m_array[i].get())) {
      return i;
    }
  }

  return -1;
}
