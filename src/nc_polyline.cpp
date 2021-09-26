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
}
