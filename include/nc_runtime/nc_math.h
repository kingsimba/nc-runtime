#pragma once

#include "nc_types.h"
#include "vector2.h"
#include "vector2i.h"

u32 Math_hashString(const char* str);
u32 Math_hashU32(u32 o);
u64 Math_hashU64(u64 o);
size_t Math_hashSizeT(size_t o);

// wrap a angle between [0, NC_TWO_PI)
float Math_wrapTwoPi(float angle);

// calculate the nearest turn angle between two directions.
float Math_turnAngle(float from, float to);

bool Math_pointInPolygon(const nc::Vector2* points, size_t count, nc::Vector2 pt);

// calculate distance from a straight line to a point
float Math_linePointDistance(nc::Vector2 pt, nc::Vector2 p1, nc::Vector2 p2, nc::Vector2* projectionOut);

// calculate distance from a line segment to a point
float Math_segmentPointDistance(nc::Vector2 pt, nc::Vector2 p1, nc::Vector2 p2, nc::Vector2* nearestPtOut);

// determine if two line segments intersect
bool Math_segmentsIntersect(nc::Vector2 a1, nc::Vector2 a2, nc::Vector2 b1, nc::Vector2 b2);

/*
 * @brief Clip line segment (p1, p2) by rectangle
 * @return
 *  true: part or whole of the line segment p1-p2 lies within the rectangle;
 *  false: the line segment lies outside the rectangle.
 */
bool Math_clipSegmentByRect(RectF rect, nc::Vector2* p1, nc::Vector2* p2);
bool Math_clipSegmentByRectI(Rect rect, nc::Vector2i* p1, nc::Vector2i* p2);
