#include "stdafx_nc_runtime.h"
#include "nc_runtime/nc_polyline.h"

sp<NcPolyline> NcPolyline::alloc(const nc::Vector2* points, int count, bool closed)
{
    sp<NcPolyline> o = new NcPolyline();
    o->initWithPoints(points, count);
    o->m_closed = closed;
    return o;
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
