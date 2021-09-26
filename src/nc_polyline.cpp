#include "nc_runtime/nc_polyline.h"

NcPolyline::NcPolyline(const nc::Vector2* points, int count, bool closed)
{
    m_closed = closed;
    initWithPoints(points, count);
}

NcPolyline::~NcPolyline()
{
    free(m_points);
}

void NcPolyline::initWithPoints(const nc::Vector2* points, int count)
{
    size_t bufferSize = sizeof(nc::Vector2) * count;
    m_points = (nc::Vector2*)malloc(bufferSize);
    memcpy(m_points, points, bufferSize);
    m_pointCount = count;
}