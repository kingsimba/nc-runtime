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

nc::Vector2 NcPolyline::calculateCentroid()
{
    float twiceArea = 0.0f;
    nc::Vector2 c = {0.0f, 0.0f};
    for (int i = 0; i < m_pointCount - 1; i++)
    {
        float d = (m_points[i].x * m_points[i + 1].y - m_points[i + 1].x * m_points[i].y);
        twiceArea += d;
        c.x += (m_points[i].x + m_points[i + 1].x) * d;
        c.y += (m_points[i].y + m_points[i + 1].y) * d;
    }

    c /= 3 * twiceArea;
    return c;
}
