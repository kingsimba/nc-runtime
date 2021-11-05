#pragma once

#include "vector2.h"
#include "nc_object.h"

class NcPolyline : public NcObject
{
public:
    NcPolyline(const nc::Vector2* points, int count, bool closed);
    ~NcPolyline();

    static sp<NcPolyline> alloc(const nc::Vector2* points, int count, bool closed)
    {
        sp<NcPolyline> o = sp<NcPolyline>::alloc(points, count, closed);
        return o;
    }

    forceinline bool isClosed() { return m_closed; }
    forceinline int pointCount() { return m_pointCount; }
    forceinline const nc::Vector2* points() { return m_points; }

    bool testPoint(nc::Vector2 pt);

    RectF calculateBBox();

protected:
    bool m_closed;
    int m_pointCount;
    nc::Vector2* m_points;
};
