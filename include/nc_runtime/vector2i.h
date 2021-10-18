#pragma once

#include "nc_runtime/nc_types.h"

namespace nc
{

class Vector2i
{
public:
    int x, y;

    Vector2i() = default;
    forceinline Vector2i(int x_, int y_) : x(x_), y(y_) {}

    forceinline void setZero() { x = y = 0; }
    forceinline int length() const { return (int)sqrtf((float)(x * x + y * y)); }
    forceinline int lengthSquared() const { return x * x + y * y; }

    forceinline void operator+=(Vector2i r)
    {
        x += r.x;
        y += r.y;
    }

    forceinline void operator-=(Vector2i r)
    {
        x -= r.x;
        y -= r.y;
    }

    forceinline void operator*=(int r)
    {
        x *= r;
        y *= r;
    }

    forceinline void operator/=(int r)
    {
        x /= r;
        y /= r;
    }
};

forceinline int Vector2i_dot(Vector2i l, Vector2i r)
{
    return l.x * r.x + l.y * r.y;
}

forceinline int Vector2i_cross(Vector2i l, Vector2i r)
{
    return l.x * r.y - l.y * r.x;
}

forceinline bool operator==(Vector2i l, Vector2i r)
{
    return l.x == r.x && l.y == r.y;
}

forceinline bool operator!=(Vector2i l, Vector2i r)
{
    return l.x != r.x || l.y != r.y;
}

forceinline Vector2i operator-(Vector2i l, Vector2i r)
{
    return Vector2i{l.x - r.x, l.y - r.y};
}

forceinline Vector2i operator+(Vector2i l, Vector2i r)
{
    return Vector2i{l.x + r.x, l.y + r.y};
}

forceinline Vector2i operator*(Vector2i l, int scaler)
{
    return Vector2i{l.x * scaler, l.y * scaler};
}

forceinline Vector2i operator/(Vector2i l, int scaler)
{
    return Vector2i{l.x / scaler, l.y / scaler};
}

} // namespace nc
