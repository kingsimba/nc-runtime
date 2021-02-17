#pragma once

#include "basic_types.h"

/**
 * Smart pointer of NcObject
 */
template <typename T>
class sp {
public:
  forceinline sp() { m_ptr = NULL; }
  forceinline sp(T* p) { m_ptr = p; }
  forceinline sp(const sp<T>& p) {
    m_ptr = p.get();
    m_ptr->_retain();
  }
  template <typename Derived>
  forceinline sp(const sp<Derived>& p) {
    m_ptr = p.get();
    m_ptr->_retain();
  }
  forceinline sp(sp&& r) {
    m_ptr = r.m_ptr;
    r.m_ptr = NULL;
  }
  ~sp() { release(m_ptr); }

  sp<T>& operator=(const sp<T>& p) {
    release(m_ptr);
    m_ptr = p.get();
    m_ptr->_retain();
    return *this;
  }

  template <typename Derived>
  sp<T>& operator=(const sp<Derived>& p) {
    release(m_ptr);
    m_ptr = p.get();
    m_ptr->_retain();
    return *this;
  }

  // compatible with std::shared_ptr
  forceinline int use_count() { return m_ptr->retainCount(); }
  forceinline void reset() {
    release(m_ptr);
    m_ptr = NULL;
  }
  forceinline T* get() const { return m_ptr; }
  forceinline T* operator->() const { return m_ptr; }

  /**
   * Implicit conversion to T*.
   * 
   * Usually, implicit conversion is not a good idea. https://www.informit.com/articles/article.aspx?p=31529&seqNum=7
   * But I still want it, because it allows for comparison (ptr == NULL) or calling C function with raw pointer.
   */
  forceinline operator T*() const { return m_ptr; }

private:
  T* m_ptr;
};

template <class T1, class T2>
sp<T1> static_pointer_cast(const sp<T2>& r) noexcept {
  T1* derived = static_cast<T1*>(r.get());
  return sp<T1>(retain(derived));
}

/**
 * The base class of the foundation framework. It contains useful functions, such as toString() and isKindOf().
 */
class NcObject {
public:
  forceinline int retainCount() { return m_rc; }

  /**
   * Convert any object to string
   */
  virtual sp<NcString> toString();

  template<typename ToType>
  forceinline bool isKindOf() {
    return dynamic_cast<ToType*>(this) != NULL;
  }
      // private:
  forceinline void _retain() {
    if (this != NULL && m_rc != INT_MAX) {
      m_rc.fetch_add(1);
    }
  }
  inline void _release() {
    if (m_rc != INT_MAX && m_rc.fetch_sub(1) == 1) {
      delete this;
    }
  }
  forceinline void _deleteThis() {
    delete this;
  }

protected:
  NcObject() : m_rc(1) {}
  NcObject(bool /*isStatic*/) : m_rc(INT_MAX) {}
  virtual ~NcObject() {}

private:
  std::atomic<int> m_rc;
};

template<typename T>
forceinline T* retain(T* o) {
  o->_retain();
  return (T*)o;
}

forceinline void release(NcObject* o) {
  if (o != NULL) o->_release();
}

// to prevent error caused by releasing smart pointer through implicit conversion to T*
template<typename T>
forceinline void release(sp<T>& o) {
  o->ERROR_should_not_call_release_on_smart_pointer();
}
