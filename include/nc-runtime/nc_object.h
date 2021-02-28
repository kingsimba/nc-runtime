#pragma once

#include "basic_types.h"

// An experimental implementation. It should be optimized in the future.
class ControlBlock {
public:
  ControlBlock() : m_rc(1), m_wc(1) {}
  int m_rc;
  int m_wc;
  Spinlock m_lock;
  char m_unused[7]; // padding to 16 bytes
};

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
    if (m_ptr) m_ptr->_retain();
  }
  template <typename Derived>
  forceinline sp(const sp<Derived>& p) {
    m_ptr = p.get();
    if (m_ptr) m_ptr->_retain();
  }
  forceinline sp(sp&& r) noexcept {
    m_ptr = r.m_ptr;
    r.m_ptr = NULL;
  }
  ~sp() { release(m_ptr); }

  sp<T>& operator=(const sp<T>& p) {
    release(m_ptr);
    m_ptr = p.get();
    if (m_ptr) m_ptr->_retain();
    return *this;
  }

  template <typename Derived>
  sp<T>& operator=(const sp<Derived>& p) {
    release(m_ptr);
    m_ptr = p.get();
    if (m_ptr) m_ptr->_retain();
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

  /**
   * For accessing array element(If it's an array)
   */
  forceinline sp<typename T::ArrayElement>& operator[](int index) {
    return m_ptr->objectAtIndex(index);
  }

private:
  T* m_ptr;
};

template <class T1, class T2>
sp<T1> static_pointer_cast(const sp<T2>& r) noexcept {
  T1* derived = static_cast<T1*>(r.get());
  return sp<T1>(retain(derived));
}

template<typename T>
class wp {
public:
  wp(T* r) {
    m_ptr = r;
    m_ptr->_retainWeak();
  }
  ~wp() { m_ptr->_releaseWeak(); }

  sp<T> lock() { return m_ptr == NULL ? NULL : (T*) m_ptr->_lockStrong(); }

private:
  T* m_ptr;
};

/**
 * The base class of the foundation framework. It contains useful functions, such as toString() and isKindOf().
 */
class NcObject {
public:
  using ArrayElement = NcObject;

public:
  static sp<NcObject> alloc() { return new NcObject(); }

  void* operator new(size_t size) {
    void* buffer = malloc(size + sizeof(ControlBlock));
    ControlBlock* ctrl = (ControlBlock*)buffer;
    new (ctrl) ControlBlock();
    return (u8*)(ctrl) + sizeof(ControlBlock);
  }

  void* operator new(size_t size, void* place) {
    void* buffer = malloc(size + sizeof(ControlBlock));
    ControlBlock* ctrl = (ControlBlock*)buffer;
    new (ctrl) ControlBlock();
    return (u8*)(ctrl) + sizeof(ControlBlock);
  }

  void operator delete(void* p) { p = p;
  }

  ControlBlock* _controlBlock() { return (ControlBlock*)((u8*)(this) - sizeof(ControlBlock)); }

  forceinline int retainCount() { return _controlBlock()->m_rc; }
  forceinline int weakCount() { return _controlBlock()->m_wc; }

  /**
   * Convert any object to string
   */
  virtual sp<NcString> toString();

  virtual bool equals(NcObject* r) { return this == r; }

  template<typename ToType>
  forceinline bool isKindOf() {
    return dynamic_cast<ToType*>(this) != NULL;
  }

  // private:
  forceinline void _retain() {
    ControlBlock* ctrl = _controlBlock();
    if (ctrl->m_rc != INT_MAX) {
      ctrl->m_lock.lock();
      ctrl->m_rc++;
      ctrl->m_lock.unlock();
    }
  }
  inline void _release() {
    ControlBlock* ctrl = _controlBlock();
    if (ctrl->m_rc != INT_MAX) {
      int rc;
      bool shouldFree;
      ctrl->m_lock.lock();
      rc = ctrl->m_rc;
      shouldFree = rc == 1 && ctrl->m_wc == 1;
      if (rc == 1) ctrl->m_wc--;
      ctrl->m_rc--;
      ctrl->m_lock.unlock();
      if (rc == 1) {
        this->~NcObject();
      }
      if (shouldFree) {
        free(_controlBlock());
      }
    }
  }

  forceinline void _retainWeak() {
    ControlBlock* ctrl = _controlBlock();
    if (ctrl->m_rc != INT_MAX) {
      ctrl->m_lock.lock();
      ctrl->m_wc++;
      ctrl->m_lock.unlock();
    }
  }

  forceinline void _releaseWeak() {
    ControlBlock* ctrl = _controlBlock();
    if (ctrl->m_rc != INT_MAX) {
      bool shouldFree;
      ctrl->m_lock.lock();
      shouldFree = ctrl->m_wc == 1;
      ctrl->m_wc--;
      ctrl->m_lock.unlock();
      if (shouldFree) {
        free(ctrl);
      }
    }
  }

  forceinline NcObject* _lockStrong() {
    ControlBlock* ctrl = _controlBlock();
    NcObject* thiz = this;
    if (ctrl->m_rc != INT_MAX) {
      ctrl->m_lock.lock();
      if (ctrl->m_rc != 0) {
        ctrl->m_rc++;
      } else {
        thiz = NULL;
      }
      ctrl->m_lock.unlock();  
    }
    
    return thiz;
  }

  forceinline void _deleteThis() {
    this->~NcObject();
    free(_controlBlock());
  }

protected:
  NcObject() {}
  NcObject(bool /*isStatic*/) {
    ControlBlock* ctrl = _controlBlock();
    ctrl->m_lock.lock();
    ctrl->m_rc = INT_MAX;
    ctrl->m_lock.unlock();
  }
  virtual ~NcObject() {}
};

template<typename T>
forceinline T* retain(T* o) {
  if (o) o->_retain();
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
