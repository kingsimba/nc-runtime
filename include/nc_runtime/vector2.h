#pragma once

#include "nc_runtime/nc_types.h"

namespace nc
{

class Vector2
{
public:
    float x, y;

    Vector2() = default;
    forceinline Vector2(float x_, float y_) : x(x_), y(y_) {}

    forceinline void set(float x_, float y_) { x = x_, y = y_; }
    forceinline void setZero() { x = y = 0; }
    forceinline float length() const { return sqrtf(x * x + y * y); }
    forceinline float lengthSquared() const { return x * x + y * y; }

    forceinline void operator+=(Vector2 r)
    {
        x += r.x;
        y += r.y;
    }

    forceinline void operator-=(Vector2 r)
    {
        x -= r.x;
        y -= r.y;
    }

    forceinline void operator*=(float r)
    {
        x *= r;
        y *= r;
    }

    forceinline void operator/=(float r)
    {
        x /= r;
        y /= r;
    }
};

NC_DEPRECATED forceinline Vector2 vec2(float x, float y)
{
    return Vector2(x, y);
}

forceinline float Vector2_dot(Vector2 l, Vector2 r)
{
    return l.x * r.x + l.y * r.y;
}

forceinline float Vector2_cross(Vector2 l, Vector2 r)
{
    return l.x * r.y - l.y * r.x;
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
