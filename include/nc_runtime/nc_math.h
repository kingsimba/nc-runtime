#pragma once

#include "nc_types.h"
#include "vector2.h"

u32 Math_hashString(const char* str);
u32 Math_hashU32(u32 o);
u64 Math_hashU64(u64 o);
size_t Math_hashSizeT(size_t o);

// wrap a angle between [0, NC_TWO_PI)
float Math_wrapTwoPi(float angle);

// calculate the nearest turn angle between two directions.
float Math_turnAngle(float from, float to);

bool Math_pointInPolygon(const nc::Vector2* points, size_t count, nc::Vector2 pt);

bool Math_clipLineByRect(RectF rect, nc::Vector2* p1, nc::Vector2* p2);
