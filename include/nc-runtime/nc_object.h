#pragma once

#include "basic_types.h"

/**
 * Smart pointer of NcObject
 */
template <typename T>
class sptr {
public:
  forceinline sptr() { m_ptr = NULL; }
  forceinline sptr(T* p) { m_ptr = p; }
  forceinline sptr(const sptr<T>& p) {
    m_ptr = p.get();
    m_ptr->_retain();
  }
  template <typename Derived>
  forceinline sptr(const sptr<Derived>& p) {
    m_ptr = p.get();
    m_ptr->_retain();
  }
  forceinline sptr(sptr&& r) {
    m_ptr = r.m_ptr;
    r.m_ptr = NULL;
  }
  ~sptr() { release(m_ptr); }

  sptr<T>& operator=(const sptr<T>& p) {
    m_ptr = p.get();
    m_ptr->_retain();
    return *this;
  }

  template <typename Derived>
  sptr<T>& operator=(const sptr<Derived>& p) {
    m_ptr = p.get();
    m_ptr->_retain();
    return *this;
  }

  forceinline T* get() const { return m_ptr; }
  forceinline int use_count() { return m_ptr->retainCount(); }
  forceinline T* operator->() const { return m_ptr; }

private:
  T* m_ptr;
};

template <class T1, class T2>
sptr<T1> static_pointer_cast(const sptr<T2>& r) noexcept {
  T1* derived = static_cast<typename T1*>(r.get());
  return sptr<T1>(derived);
}

class NcObject {
public:
  forceinline int retainCount() { return m_rc; }

  virtual sptr<NcString> toString();

// private:
  forceinline void _retain() { m_rc.fetch_add(1); }
  inline void _release() {
    if (m_rc.fetch_sub(1) == 0) {
      delete this;
    }
  }

protected:
  NcObject() : m_rc(1) {}
  virtual ~NcObject() {}

private:
  std::atomic<int> m_rc;
};

template<typename T>
forceinline T* retain(NcObject* o) {
  if (o != NULL) {
    o->_retain();
  }
  return (T*)o;
}

forceinline void release(NcObject* o) { if (o != NULL) o->_release(); }
