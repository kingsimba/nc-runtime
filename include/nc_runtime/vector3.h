#pragma once

#include "nc_runtime/nc_types.h"

namespace nc
{

class Vector3
{
public:
    float x, y, z;

    Vector3() = default;
    forceinline Vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

    forceinline void setZero() { x = y = z = 0; }
    forceinline float length() const { return sqrtf(x * x + y * y + z * z); }
    forceinline float lengthSquared() const { return x * x + y * y + z * z; }

    forceinline void operator+=(Vector3 r)
    {
        x += r.x;
        y += r.y;
        z += r.z;
    }

    forceinline void operator-=(Vector3 r)
    {
        x -= r.x;
        y -= r.y;
        z -= r.z;
    }

    forceinline void operator*=(float r)
    {
        x *= r;
        y *= r;
        z *= r;
    }

    forceinline void operator/=(float r)
    {
        x /= r;
        y /= r;
        z /= r;
    }
};

forceinline float Vector3_dot(Vector3 l, Vector3 r)
{
    return l.x * r.x + l.y * r.y + l.z * r.z;
}

forceinline Vector3 Vector3_cross(Vector3 l, Vector3 r)
{
    return Vector3{l.y * r.z - l.z * r.y, l.x * r.z - l.z * r.x, l.x * r.y - l.y * r.x};
}

forceinline Vector3 vec3(float x, float y, float z)
{
    return Vector3(x, y, z);
}

forceinline bool operator==(Vector3 l, Vector3 r)
{
    return l.x == r.x && l.y == r.y && l.z == r.z;
}

forceinline bool operator!=(Vector3 l, Vector3 r)
{
    return l.x != r.x || l.y != r.y || l.z != r.z;
}

forceinline Vector3 operator-(Vector3 l, Vector3 r)
{
    return Vector3{l.x - r.x, l.y - r.y, l.z - r.z};
}

forceinline Vector3 operator+(Vector3 l, Vector3 r)
{
    return Vector3{l.x + r.x, l.y + r.y, l.z + r.z};
}

forceinline Vector3 operator*(Vector3 l, float scaler)
{
    return Vector3{l.x * scaler, l.y * scaler, l.z * scaler};
}

forceinline Vector3 operator/(Vector3 l, float scaler)
{
    return Vector3{l.x / scaler, l.y / scaler, l.z / scaler};
}

} // namespace nc
