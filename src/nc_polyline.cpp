#include "stdafx_nc_runtime.h"
#include "nc_runtime/nc_polyline.h"

NcPolyline::~NcPolyline()
{
    free(m_points);
}

NcPolyline::NcPolyline(const nc::Vector2* points, int count, bool closed)
{
    size_t bufferSize = sizeof(nc::Vector2) * count;
    m_points = (nc::Vector2*)malloc(bufferSize);
    m_closed = closed;
    memcpy(m_points, points, bufferSize);
    m_pointCount = count;

    m_boundingBox.invliad();
    if (m_closed)
    {
        for (int i = 0; i < count; i++)
        {
            m_boundingBox.combinePointF(m_points[i].x, m_points[i].y);
        }
    }
}

static float _crossProduct(nc::Vector2 pt1, nc::Vector2 pt2)
{
    return pt1.x * pt2.y - pt1.y * pt2.x;
}

bool NcPolyline::isPointInConvexPolygon(nc::Vector2 pt)
{
    if (!m_closed || m_pointCount < 3)
        return false;

    if (!m_boundingBox.testPointF(pt.x, pt.y))
        return false;

    for (int i = 0; i < m_pointCount; i++)
    {
        int iNext = (i + 1) % m_pointCount;
        int i2Next = (iNext + 1) % m_pointCount;

        // normalization to prevent cross product overflow caused by too large image coordinates
        nc::Vector2 v1 = {m_points[i].x - pt.x, m_points[i].y - pt.y};
        nc::Vector2 v2 = {m_points[iNext].x - pt.x, m_points[iNext].y - pt.y};
        float preCross = _crossProduct(v1, v2);

        nc::Vector2 v3 = {m_points[i2Next].x - pt.x, m_points[i2Next].y - pt.y};
        float nextCross = _crossProduct(v2, v3);

        if (preCross * nextCross < 0)
            return false;
    }

    return true;
}