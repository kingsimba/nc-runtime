#pragma once

#include "nc_types.h"

// An experimental implementation. It might be optimized in the future.
// Keep watching https://github.com/kingsimba/nc-runtime for updates
class ControlBlock
{
public:
#ifdef NC_PLATFORM_WIN
#    pragma warning(push)
#    pragma warning(suppress : 26495)
#endif
    ControlBlock() : m_rc(1), m_wc(0) {}
#ifdef NC_PLATFORM_WIN
#    pragma warning(pop)
#endif

    inline void retain()
    {
        if (m_rc != INT_MAX)
        {
            m_lock.lock();
            m_rc++;
            m_lock.unlock();
        }
    }

    struct Action
    {
        bool freeObject;
        bool freeMemory;
    };

    inline Action release()
    {
        Action rtn = {false, false};
        if (m_rc != INT_MAX)
        {
            m_lock.lock();
            rtn.freeObject = m_rc == 1;
            rtn.freeMemory = m_rc == 1 && m_wc == 0;
            m_rc--;
            m_lock.unlock();
        }
        return rtn;
    }

    forceinline void retainWeak()
    {
        m_lock.lock();
        m_wc++;
        m_lock.unlock();
    }

    forceinline bool releaseWeak()
    {
        bool shouldFree = false;
        m_lock.lock();
        shouldFree = m_rc == 0 && m_wc == 1;
        m_wc--;
        m_lock.unlock();
        return shouldFree;
    }

    forceinline bool lockStrong()
    {
        bool succ = true;
        if (m_rc != INT_MAX)
        {
            m_lock.lock();
            if (m_rc != 0)
            {
                m_rc++;
            }
            else
            {
                succ = false;
            }
            m_lock.unlock();
        }

        return succ;
    }

    int m_rc;
    int m_wc;

private:
    Spinlock m_lock;

#if defined(NC_OS_UNIX)
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wunused-private-field"
#endif
    char m_unused[7]; // padding to 16 bytes
#if defined(NC_OS_UNIX)
#    pragma clang diagnostic pop
#endif
};

/**
 * Smart pointer of NcObject
 */
template <typename T> class sp
{
public:
    forceinline sp() { m_ptr = NULL; }
    forceinline sp(T* p) { m_ptr = p; }
    forceinline sp(const sp<T>& p)
    {
        m_ptr = p.get();
        if (m_ptr)
            m_ptr->_retain();
    }
    template <typename Derived> forceinline sp(const sp<Derived>& p)
    {
        m_ptr = p.get();
        if (m_ptr)
            m_ptr->_retain();
    }
    forceinline sp(sp&& r) noexcept
    {
        m_ptr = r.m_ptr;
        r.m_ptr = NULL;
    }
    ~sp() { release(m_ptr); }

    sp<T>& operator=(const sp<T>& p)
    {
        release(m_ptr);
        m_ptr = p.get();
        if (m_ptr)
            m_ptr->_retain();
        return *this;
    }

    template <typename Derived> sp<T>& operator=(const sp<Derived>& p)
    {
        release(m_ptr);
        m_ptr = p.get();
        if (m_ptr)
            m_ptr->_retain();
        return *this;
    }

    // compatible with std::shared_ptr
    forceinline int use_count() { return m_ptr->retainCount(); }
    forceinline void reset()
    {
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
    forceinline sp<typename T::ArrayElement>& operator[](int index) { return m_ptr->objectAtIndex(index); }

private:
    T* m_ptr;
};

template <class T1, class T2> sp<T1> static_pointer_cast(const sp<T2>& r) noexcept
{
    T1* derived = static_cast<T1*>(r.get());
    return sp<T1>(retain(derived));
}

/**
 * Weak Pointer
 */
template <typename T> class wp
{
public:
    wp() = default;

    // raw pointer constructor
    wp(T* r)
    {
        m_ptr = r;
        m_ptr->_retainWeak();
    }
    // strong pointer constructor
    wp(const sp<T>& r)
    {
        m_ptr = r.get();
        m_ptr->_retainWeak();
    }
    // copy constructor
    wp(const wp<T>& r)
    {
        m_ptr = r.m_ptr;
        m_ptr->_retainWeak();
    }
    // move constructor
    wp(wp<T>&& r)
    {
        m_ptr = r.m_ptr;
        r.m_ptr = NULL;
    }
    ~wp()
    {
        if (m_ptr)
            m_ptr->_releaseWeak();
    }

    forceinline void reset()
    {
        m_ptr->_releaseWeak();
        m_ptr = NULL;
    }

    // copy operator
    wp<T>& operator=(const wp<T>& r)
    {
        m_ptr = r.m_ptr;
        m_ptr->_retainWeak();
        return *this;
    }

    template <typename Derived> wp<T>& operator=(const sp<Derived>& r)
    {
        if (m_ptr)
            m_ptr->_releaseWeak();
        m_ptr = r.get();
        m_ptr->_retainWeak();
        return *this;
    }

    template <typename Derived> wp<T>& operator=(Derived* r)
    {
        if (m_ptr)
            m_ptr->_releaseWeak();
        m_ptr = r;
        m_ptr->_retainWeak();
        return *this;
    }

    sp<T> lock()
    {
        if (m_ptr == NULL)
            return NULL;

        ControlBlock* ctrl = m_ptr->_controlBlock();
        if (ctrl->lockStrong())
            return sp<T>(m_ptr);
        else
            return NULL;
    }

private:
    T* m_ptr;
};

/**
 * The base class of the foundation framework. It contains useful functions, such as toString() and isKindOf().
 *
 * Notes: This is an proof-of-concept implementation. It might be optimized in the future.
 * Keep watching https://github.com/kingsimba/nc-runtime for updates
 */
class NcObject
{
public:
    using ArrayElement = NcObject;

public:
    static sp<NcObject> alloc() { return new NcObject(); }

    /**
     * It malloc() a block of memory and initialize the control block.
     * But the object is not constructed. The caller should call placement new later.
     * For example:
     *
     * ```cpp
     * size_t totalLen = sizeof(NcString) + strLength + 1;
     * NcString* o = (NcString*)NcObject::allocRawObjectWithSize(totalLen, false);
     * ::new (o) NcString();
     * ```
     */
    static NcObject* allocRawObjectWithSize(size_t size, bool zero_memory);

#ifdef NC_PLATFORM_WIN
#    pragma warning(push)
#    pragma warning(suppress : 6386)
#endif
    void* operator new(size_t size)
    {
        void* buffer = malloc(sizeof(ControlBlock) + size);
        ControlBlock* ctrl = (ControlBlock*)buffer;

        new (ctrl) ControlBlock();
        return ctrl + 1;
    }
#ifdef NC_PLATFORM_WIN
#    pragma warning(pop)
#endif

    void operator delete(void* p) { UNUSED_VAR(p); }

    forceinline ControlBlock* _controlBlock() { return (ControlBlock*)(this) - 1; }

    forceinline int retainCount() { return _controlBlock()->m_rc; }
    forceinline int weakCount() { return _controlBlock()->m_wc; }

    /**
     * Convert any object to string
     */
    virtual sp<NcString> toString();

    virtual bool equals(NcObject* r) { return this == r; }

    template <typename ToType> forceinline bool isKindOf() { return dynamic_cast<ToType*>(this) != NULL; }

    // private:
    forceinline void _retain() { _controlBlock()->retain(); }
    inline void _release()
    {
        auto ctrl = _controlBlock();
        auto should = ctrl->release();
        if (should.freeObject)
        {
            this->~NcObject();
            if (should.freeMemory)
            {
                free(ctrl);
            }
        }
    }

    forceinline void _retainWeak() { _controlBlock()->retainWeak(); }

    forceinline void _releaseWeak()
    {
        ControlBlock* ctrl = _controlBlock();
        if (ctrl->releaseWeak())
        {
            free(ctrl);
        }
    }

    forceinline void _deleteThis()
    {
        this->~NcObject();
        free(_controlBlock());
    }

protected:
    NcObject() {}
    NcObject(bool /*isStatic*/)
    {
        ControlBlock* ctrl = _controlBlock();
        ctrl->m_rc = INT_MAX;
    }
    virtual ~NcObject() {}
};

template <typename T> forceinline T* retain(T* o)
{
    if (o)
        o->_retain();
    return (T*)o;
}

forceinline void release(NcObject* o)
{
    if (o != NULL)
        o->_release();
}

// to prevent error caused by releasing smart pointer through implicit conversion to T*
template <typename T> forceinline void release(sp<T>& o)
{
    o->ERROR_should_not_call_release_on_smart_pointer();
}
