#pragma once

#include "nc_runtime/nc_types.h"

namespace nc
{

class Vector2
{
public:
    float x, y;

    float length() const { return sqrtf(x * x + y * y); }
    float lengthSquared() const { return x * x + y * y; }
};

static forceinline Vector2 vec2(float x, float y)
{
    return Vector2{x, y};
}

forceinline bool operator==(Vector2 l, Vector2 r)
{
    return l.x == r.x && l.y == r.y;
}

forceinline bool operator!=(Vector2 l, Vector2 r)
{
    return l.x != r.x || l.y != r.y;
}

forceinline Vector2 operator-(Vector2 l, Vector2 r)
{
    return Vector2{l.x - r.x, l.y - r.y};
}

forceinline Vector2 operator+(Vector2 l, Vector2 r)
{
    return Vector2{l.x + r.x, l.y + r.y};
}

forceinline Vector2 operator*(Vector2 l, float scaler)
{
    return Vector2{l.x * scaler, l.y * scaler};
}

forceinline Vector2 operator/(Vector2 l, float scaler)
{
    return Vector2{l.x / scaler, l.y / scaler};
}

} // namespace nc
