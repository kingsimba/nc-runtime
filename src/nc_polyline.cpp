#include "stdafx_nc_runtime.h"
#include "nc_runtime/nc_polyline.h"
#include "nc_runtime/nc_math.h"

using namespace nc;

NcPolyline::~NcPolyline()
{
    if (m_shouldFree)
        free(m_points);
}

NcPolyline::NcPolyline(const nc::Vector2* points, int count, bool closed)
{
    size_t bufferSize = sizeof(nc::Vector2) * count;
    m_points = (nc::Vector2*)malloc(bufferSize);
    m_closed = closed;
    m_shouldFree = true;
    memcpy(m_points, points, bufferSize);
    m_pointCount = count;
}

bool NcPolyline::testPoint(nc::Vector2 pt)
{
    return Math_pointInPolygon(m_points, m_pointCount, pt);
}

RectF NcPolyline::calculateBBox()
{
    RectF rtn;
    rtn.setAsNegativeMinimum();
    const Vector2* end = m_points + m_pointCount;
    for (const Vector2* p = m_points; p != end; p++)
    {
        rtn.combinePoint(p->x, p->y);
    }

    return rtn;
}
