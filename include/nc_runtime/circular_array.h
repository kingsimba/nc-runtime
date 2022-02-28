#pragma once

#include "nc_types.h"
#include "nc_log.h"

template <typename T, size_t SIZE>
class FixedCircularArray
{
public:
    FixedCircularArray() = default;

    forceinline bool empty() const { return m_start == m_end; }
    forceinline bool full() const { return nextIndex(m_end) == m_start; }
    forceinline size_t size() const { return m_start <= m_end ? m_end - m_start : BUFFER_SIZE - (m_start - m_end); }

    void push(const T& t)
    {
        if (full())
            pop();

        m_arr[m_end] = t;
        m_end = nextIndex(m_end);
    }

    T pop()
    {
        NC_ASSERT(!empty());
        T rtn = m_arr[m_start];
        m_start = nextIndex(m_start);
        return rtn;
    }

    const T& operator[](size_t i) const
    {
        i += m_start;
        if (i >= BUFFER_SIZE)
        {
            i -= BUFFER_SIZE;
        }
        return m_arr[i];
    }

private:
    forceinline static size_t nextIndex(size_t i) { return i == SIZE ? 0 : i + 1; }

private:
    static constexpr size_t BUFFER_SIZE = SIZE + 1;
    T m_arr[BUFFER_SIZE];
    size_t m_start = 0;
    size_t m_end = 0;
};
