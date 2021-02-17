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
   * auto obj = v->find([&](NcString* v) {
   *   return v->startsWith(startWord);
   * });
   * ```
   */
  template<typename Finder>
  sp<T> find(const Finder& finder) {
    for (auto& obj : m_array) {
      if (finder(obj.get())) {
        return obj;
      }
    }

    return NULL;
  }

  forceinline sp<T>& operator[](int index) {
    return m_array[index];
  }

private:
  std::vector< sp<T> > m_array;
};

