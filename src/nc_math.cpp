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
template <typename T>
static T _mod(T x, T y)
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

float Math_linePointDistance(nc::Vector2 pt, nc::Vector2 p1, nc::Vector2 p2, nc::Vector2* projectionOut)
{
    Vector2 v = p2 - p1;
    Vector2 w = pt - p1;

    float c1 = Vector2_dot(w, v);
    float c2 = Vector2_dot(v, v);
    float b = c1 / c2;
    Vector2 pb = p1 + v * b;
    Vector2 tmp = pt - pb;
    float d = tmp.length();

    if (projectionOut != NULL)
        *projectionOut = pb;

    return d;
}

bool Math_segmentsIntersect(nc::Vector2 a1, nc::Vector2 a2, nc::Vector2 b1, nc::Vector2 b2)
{
    if (std::max(a1.x, a2.x) < std::min(b1.x, b2.x) || std::max(b1.x, b2.x) < std::min(a1.x, a2.x)
        || std::max(a1.y, a2.y) < std::min(b1.y, b2.y) || std::max(b1.y, b2.y) < std::min(a1.y, a2.y))
        return false;

    return Vector2_cross(a2 - a1, b1 - a1) * Vector2_cross(a2 - a1, b2 - a1) <= 0
           && Vector2_cross(b2 - b1, a1 - b1) * Vector2_cross(b2 - b1, a2 - b1) <= 0;
}

namespace
{
enum class RectRegionCode
{
    inside = 0,
    left = 1,
    right = 2,
    top = 4,
    bottom = 8
};

static forceinline RectRegionCode operator|(RectRegionCode r, RectRegionCode l)
{
    return (RectRegionCode)((int)r | (int)l);
}

static forceinline RectRegionCode& operator|=(RectRegionCode& r, RectRegionCode l)
{
    return r = r | l;
}

static forceinline bool operator&(RectRegionCode r, RectRegionCode l)
{
    return ((int)r & (int)l) != 0;
}

class BoundaryFloat
{
public:
    static forceinline bool greater(float l, float r) { return l > r; }
    static forceinline float value() { return 0; }
};

class BoundaryInt
{
public:
    static forceinline bool greater(int l, int r) { return l >= r; }
    static forceinline int value() { return 1; }
};
} // namespace

template <typename RECT, typename VECTOR, typename Boundary>
static RectRegionCode _calcRectRegionCode(RECT rect, VECTOR pt)
{
    RectRegionCode code = RectRegionCode::inside;

    if (pt.x < rect.left)
        code = RectRegionCode::left;
    else if (Boundary::greater(pt.x, rect.right))
        code = RectRegionCode::right;

    if (pt.y < rect.top)
        code |= RectRegionCode::top;
    else if (Boundary::greater(pt.y, rect.bottom))
        code |= RectRegionCode::bottom;

    return code;
}

/**
 * @sa https://www.geeksforgeeks.org/line-clipping-set-1-cohen-sutherland-algorithm/
 */
template <typename RECT, typename VECTOR, typename Boundary>
static bool _clipLineByRect(RECT rect, VECTOR* p1, VECTOR* p2)
{
    RectRegionCode code1 = _calcRectRegionCode<RECT, VECTOR, Boundary>(rect, *p1);
    RectRegionCode code2 = _calcRectRegionCode<RECT, VECTOR, Boundary>(rect, *p2);

    for (;;)
    {
        if (code1 == RectRegionCode::inside && code2 == RectRegionCode::inside)
            return true;

        if (code1 & code2)
            return false;

        RectRegionCode codeOut = (int)code1 ? code1 : code2;
        VECTOR p;

        /*
         *  Find intersection point, using formulas:
         *  y = y1 + slope * (x - x1)
         *  x = x1 + (1 / slope) * (y - y1)
         */
        if (codeOut & RectRegionCode::left)
        {
            /* point is to the left of rectangle */
            p.y = p1->y + (p2->y - p1->y) * (rect.left - p1->x) / (p2->x - p1->x);
            p.x = rect.left;
        }
        else if (codeOut & RectRegionCode::right)
        {
            /* point is to the right of rectangle */
            p.y = p1->y + (p2->y - p1->y) * (rect.right - Boundary::value() - p1->x) / (p2->x - p1->x);
            p.x = rect.right - Boundary::value();
        }
        else if (codeOut & RectRegionCode::top)
        {
            /* point is above the clip rectangle */
            p.x = p1->x + (p2->x - p1->x) * (rect.top - p1->y) / (p2->y - p1->y);
            p.y = rect.top;
        }
        else
        {
            NC_ASSERT(codeOut & RectRegionCode::bottom);
            /* point is below the rectangle */
            p.x = p1->x + (p2->x - p1->x) * (rect.bottom - Boundary::value() - p1->y) / (p2->y - p1->y);
            p.y = rect.bottom - Boundary::value();
        }

        if (codeOut == code1)
        {
            *p1 = p;
            code1 = _calcRectRegionCode<RECT, VECTOR, Boundary>(rect, *p1);
        }
        else
        {
            *p2 = p;
            code2 = _calcRectRegionCode<RECT, VECTOR, Boundary>(rect, *p2);
        }
    }
}

bool Math_clipSegmentByRect(RectF rect, Vector2* p1, Vector2* p2)
{
    return _clipLineByRect<RectF, Vector2, BoundaryFloat>(rect, p1, p2);
}

bool Math_clipSegmentByRectI(Rect rect, nc::Vector2i* p1, nc::Vector2i* p2)
{
    return _clipLineByRect<Rect, Vector2i, BoundaryInt>(rect, p1, p2);
}
