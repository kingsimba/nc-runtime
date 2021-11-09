#pragma once

#include "vector2.h"
#include "nc_object.h"

class NcPolyline : public NcObject
{
public:
    static forceinline NcPolyline makeWithPointsNoCopy(const nc::Vector2* points, int count, bool closed)
    {
        NcPolyline o;
        o.m_points = (nc::Vector2*)points;
        o.m_pointCount = count;
        o.m_closed = closed;
        return o;
    }

    static sp<NcPolyline> alloc(const nc::Vector2* points, int count, bool closed)
    {
        sp<NcPolyline> o = NcObject::alloc<NcPolyline>(points, count, closed);
        return o;
    }

    NcPolyline() : m_closed(false), m_shouldFree(false), m_pointCount(0), m_points(nullptr) {}
    NcPolyline(const nc::Vector2* points, int count, bool closed);
    ~NcPolyline();

    forceinline bool isClosed() { return m_closed; }
    forceinline int pointCount() { return m_pointCount; }
    forceinline const nc::Vector2* points() { return m_points; }

    bool testPoint(nc::Vector2 pt);

    RectF calculateBBox();

protected:
    bool m_closed;
    bool m_shouldFree;
    int m_pointCount;
    nc::Vector2* m_points;
};
