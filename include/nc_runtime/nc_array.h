#pragma once

#include "nc_types.h"
#include "nc_string.h"

template <typename T>
class NcArray : public NcObject {
public:
  using ArrayElement = T;

public:
  static sp<NcArray<T>> alloc() { return new NcArray(); }

  forceinline int capacity() { return (int)m_array.capacity(); }
  forceinline int size() { return (int)m_array.size(); }
  forceinline void reserve(int newCapacity) { return m_array.reserve(newCapacity); }

  forceinline void addObject(T* obj) { this->m_array.push_back(retain(obj)); }
  forceinline sp<T>& firstObject() { return this->m_array[0]; }
  forceinline sp<T>& lastObject() { return this->m_array[m_array.size() - 1]; }
  forceinline sp<T>& objectAtIndex(int i) { return this->m_array[i]; }
  forceinline void removeObjectAtIndex(int i) {
    assert(!m_array.empty());
    m_array.erase(m_array.begin() + i);
  }

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
  template <typename Func>
  sp<T> findWithCondition(Func func) {
    for (auto& obj : m_array)
      if (func(obj.get())) return obj;
    return NULL;
  }

  /*
   * return -1 if not found. It uses NcObject::equals() for comparison
   */
  int indexOfObject(T* r) {
    return indexOfObjectWithCondition([=](T* obj) { return obj == r || obj->equals(r); });
  }

  template <typename Func>
  int indexOfObjectWithCondition(Func func) {
    int len = (int)m_array.size();
    for (int i = 0; i < len; i++)
      if (func(m_array[i].get())) return i;
    return -1;
  }

private:
  std::vector<sp<T>> m_array;
};
