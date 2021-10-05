#include "stdafx_nc_runtime.h"
#include "nc_runtime/nc_math.h"

using namespace nc;

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

/* ======= Crossings algorithm ============================================ */
// http://www.realtimerendering.com/resources/GraphicsGems//gemsiv/ptpoly_haines/ptinpoly.cpp

/* Shoot a test ray along +X axis.  The strategy, from MacMartin, is to
 * compare vertex Y values to the testing point's Y and quickly discard
 * edges which are entirely to one side of the test ray.
 *
 * Input 2D polygon _pgon_ with _numverts_ number of vertices and test point
 * _point_, returns 1 if inside, 0 if outside.    WINDING and CONVEX can be
 * defined for this test.
 */
bool Math_pointInPolygon(const Vector2* pgon, size_t numverts, Vector2 pt)
{
    if (numverts <= 2)
        return false;

#ifdef WINDING
    int crossings;
#endif
    int j, yflag0, yflag1, inside_flag, xflag0;
    float ty, tx;
    const Vector2* vtx0;
    const Vector2* vtx1;
#ifdef CONVEX
    int line_flag;
#endif

    tx = pt.x;
    ty = pt.y;

    vtx0 = &pgon[numverts - 1];
    /* get test bit for above/below X axis */
    yflag0 = (vtx0->y >= ty);
    vtx1 = &pgon[0];

#ifdef WINDING
    crossings = 0;
#else
    inside_flag = 0;
#endif
#ifdef CONVEX
    line_flag = 0;
#endif
    for (j = (int)numverts + 1; --j;)
    {

        yflag1 = (vtx1->y >= ty);
        /* check if edge's endpoints straddle (are on opposite sides) of X axis
         * (i.e. the Y's differ); if so, +X ray could intersect this edge.
         */
        if (yflag0 != yflag1)
        {
            xflag0 = (vtx0->x >= tx);
            /* check if endpoints are on same side of the Y axis (i.e. X's
             * are the same); if so, it's easy to test if edge hits or misses.
             */
            if (xflag0 == (vtx1->x >= tx))
            {

                /* if edge's X values both right of the point, must hit */
#ifdef WINDING
                if (xflag0)
                    crossings += (yflag0 ? -1 : 1);
#else
                if (xflag0)
                    inside_flag = !inside_flag;
#endif
            }
            else
            {
                /* compute intersection of polygon edge with +X ray, note
                 * if >= point's X; if so, the ray hits it.
                 */
                if ((vtx1->x - (vtx1->y - ty) * (vtx0->x - vtx1->x) / (vtx0->y - vtx1->y)) >= tx)
                {
#ifdef WINDING
                    crossings += (yflag0 ? -1 : 1);
#else
                    inside_flag = !inside_flag;
#endif
                }
            }
#ifdef CONVEX
            /* if this is second edge hit, then done testing */
            if (line_flag)
                goto Exit;

            /* note that one edge has been hit by the ray's line */
            line_flag = TRUE;
#endif
            /* small improvement over original code: update yflag0 only when
             * we know it differs from yflag1.
             */
            yflag0 = yflag1;
        }

        /* move to next pair of vertices, retaining info as possible */
        vtx0 = vtx1;
        vtx1++;
    }
#ifdef CONVEX
Exit:;
#endif
#ifdef WINDING
    /* test if crossings is not zero */
    inside_flag = (crossings != 0);
#endif

    return (inside_flag);
}
