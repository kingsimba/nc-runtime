#include "stdafx_nc_runtime.h"
#include "nc_runtime/nc_math.h"

u32 Math_hashString(const char* str)
{
    u32 hash = 5381;
    int c;

    while ((c = *(u8*)(str)++) != 0)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

u32 Math_hashU32(u32 x)
{
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

u64 Math_hashU64(u64 x)
{
    x = (x ^ (x >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    x = (x ^ (x >> 27)) * UINT64_C(0x94d049bb133111eb);
    x = x ^ (x >> 31);
    return x;
}

size_t Math_hashSizeT(size_t o)
{
#if defined(NC_ENVIRONMENT_64)
    return Math_hashU64(o);
#else
    return Math_hashU32(o);
#endif
}

// Floating-point modulo
// The result (the remainder) has same sign as the divisor.
// Similar to matlab's mod(); Not similar to fmod() -   Mod(-3,4)= 1   fmod(-3,4)= -3
template <typename T> static T _mod(T x, T y)
{
    static_assert(!std::numeric_limits<T>::is_exact, "Mod: floating-point type expected");

    if (0. == y)
        return x;

    float m = x - y * floorf(x / y);

    // handle boundary cases resulted from floating-point cut off:

    if (y > 0) // modulo range: [0..y)
    {
        if (m >= y) // Mod(-1e-16             , 360.    ): m= 360.
            return 0;

        if (m < 0)
        {
            if (y + m == y)
                return 0; // just in case...
            else
                return y + m; // Mod(106.81415022205296 , _TWO_PI ): m= -1.421e-14
        }
    }
    else // modulo range: (y..0]
    {
        if (m <= y) // Mod(1e-16              , -360.   ): m= -360.
            return 0;

        if (m > 0)
        {
            if (y + m == y)
                return 0; // just in case...
            else
                return y + m; // Mod(-106.81415022205296, -_TWO_PI): m= 1.421e-14
        }
    }

    return m;
}

float Math_wrapTwoPi(float angle)
{
    return _mod(angle, NC_TWO_PI);
}

float Math_turnAngle(float from, float to)
{
    from = Math_wrapTwoPi(from);
    to = Math_wrapTwoPi(to);
    float diff = to - from;
    if (diff > NC_PI)
        diff -= NC_TWO_PI;
    else if (diff < -NC_PI)
        diff += NC_TWO_PI;
    return diff;
}
