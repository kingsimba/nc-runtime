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

/* ======= Crossings Multiply algorithm =================================== */
/* http://www.realtimerendering.com/resources/GraphicsGems//gemsiv/ptpoly_haines/ptinpoly.cpp */

/*
 * This version is usually somewhat faster than the original published in
 * Graphics Gems IV; by turning the division for testing the X axis crossing
 * into a tricky multiplication test this part of the test became faster,
 * which had the additional effect of making the test for "both to left or
 * both to right" a bit slower for triangles than simply computing the
 * intersection each time.  The main increase is in triangle testing speed,
 * which was about 15% faster; all other polygon complexities were pretty much
 * the same as before.  On machines where division is very expensive (not the
 * case on the HP 9000 series on which I tested) this test should be much
 * faster overall than the old code.  Your mileage may (in fact, will) vary,
 * depending on the machine and the test data, but in general I believe this
 * code is both shorter and faster.  This test was inspired by unpublished
 * Graphics Gems submitted by Joseph Samosky and Mark Haigh-Hutchinson.
 * Related work by Samosky is in:
 *
 * Samosky, Joseph, "SectionView: A system for interactively specifying and
 * visualizing sections through three-dimensional medical image data",
 * M.S. Thesis, Department of Electrical Engineering and Computer Science,
 * Massachusetts Institute of Technology, 1993.
 *
 */

/* Shoot a test ray along +X axis.  The strategy is to compare vertex Y values
 * to the testing point's Y and quickly discard edges which are entirely to one
 * side of the test ray.  Note that CONVEX and WINDING code can be added as
 * for the CrossingsTest() code; it is left out here for clarity.
 *
 * Input 2D polygon _pgon_ with _numverts_ number of vertices and test point
 * _point_, returns 1 if inside, 0 if outside.
 */
bool Math_pointInPolygon(const Vector2* pgon, size_t numverts, Vector2 pt)
{
    if (numverts <= 2)
        return false;

    bool inside = false;
    bool yflag0, yflag1;

    const Vector2* vtx0 = &pgon[numverts - 1];
    const Vector2* vtx1 = &pgon[0];

    /* get test bit for above/below X axis */
    yflag0 = (vtx0->y >= pt.y);

    for (int j = (int)numverts + 1; --j;)
    {
        yflag1 = (vtx1->y >= pt.y);
        /* Check if endpoints straddle (are on opposite sides) of X axis
         * (i.e. the Y's differ); if so, +X ray could intersect this edge.
         * The old test also checked whether the endpoints are both to the
         * right or to the left of the test point.  However, given the faster
         * intersection point computation used below, this test was found to
         * be a break-even proposition for most polygons and a loser for
         * triangles (where 50% or more of the edges which survive this test
         * will cross quadrants and so have to have the X intersection computed
         * anyway).  I credit Joseph Samosky with inspiring me to try dropping
         * the "both left or both right" part of my code.
         */
        if (yflag0 != yflag1)
        {
            /* Check intersection of pgon segment with +X ray.
             * Note if >= point's X; if so, the ray hits it.
             * The division operation is avoided for the ">=" test by checking
             * the sign of the first vertex wrto the test point; idea inspired
             * by Joseph Samosky's and Mark Haigh-Hutchinson's different
             * polygon inclusion tests.
             */
            if (((vtx1->y - pt.y) * (vtx0->x - vtx1->x) >= (vtx1->x - pt.x) * (vtx0->y - vtx1->y)) == yflag1)
            {
                inside = !inside;
            }
        }

        /* Move to the next pair of vertices, retaining info as possible. */
        yflag0 = yflag1;
        vtx0 = vtx1;
        vtx1++;
    }

    return inside;
}
