#pragma once

#include "vector2.h"
#include "nc_object.h"

class NcPolyline : public NcObject
{
public:
    static sp<NcPolyline> alloc(const nc::Vector2* points, int count, bool closed);

    inline bool isClosed() { return m_closed; }

    inline int pointCount() { return m_pointCount; }
    inline const nc::Vector2* points() { return m_points; }

protected:
    void initWithPoints(const nc::Vector2* points, int count);

    NcPolyline() = default;
    ~NcPolyline();

    bool m_closed;
    int m_pointCount;
    nc::Vector2* m_points;
};
