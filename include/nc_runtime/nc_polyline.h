#pragma once

#include <memory>
#include "vector2.h"

class NcPolyline
{
public:
    NcPolyline(const nc::Vector2* points, int count, bool closed);
    ~NcPolyline();

    inline bool isClosed() { return m_closed; }

    inline int pointCount() { return m_pointCount; }
    inline const nc::Vector2* points() { return m_points; }

protected:
    void initWithPoints(const nc::Vector2* points, int count);

    bool m_closed;
    int m_pointCount;
    nc::Vector2* m_points;
};
