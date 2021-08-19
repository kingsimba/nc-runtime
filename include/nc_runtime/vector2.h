#pragma once

#include "nc_runtime/nc_types.h"

namespace nc
{

class Vector2
{
public:
    float x, y;

    void setZero() { x = y = 0; }
    float length() const { return sqrtf(x * x + y * y); }
    float lengthSquared() const { return x * x + y * y; }

    void operator+=(Vector2 r)
    {
        x += r.x;
        y += r.y;
    }

    void operator-=(Vector2 r)
    {
        x -= r.x;
        y -= r.y;
    }

    void operator*=(float r)
    {
        x *= r;
        y *= r;
    }

    void operator/=(float r)
    {
        x /= r;
        y /= r;
    }
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
