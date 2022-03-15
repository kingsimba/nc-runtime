#pragma once

#include "nc_runtime/nc_types.h"

namespace nc
{

class Vector3d
{
public:
    using TYPE = double;
    double x, y, z;

    Vector3d() = default;
    forceinline Vector3d(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

    forceinline void set(double x_, double y_, double z_) { x = x_, y = y_, z = z_; }
    forceinline void setZero() { x = y = z = 0; }
    forceinline double length() const { return (TYPE)sqrt(x * x + y * y + z * z); }
    forceinline double lengthSquared() const { return x * x + y * y + z * z; }

    forceinline void operator+=(Vector3d r)
    {
        x += r.x;
        y += r.y;
        z += r.z;
    }

    forceinline void operator-=(Vector3d r)
    {
        x -= r.x;
        y -= r.y;
        z -= r.z;
    }

    forceinline void operator*=(double r)
    {
        x *= r;
        y *= r;
        z *= r;
    }

    forceinline void operator/=(double r)
    {
        x /= r;
        y /= r;
        z /= r;
    }
};

forceinline double Vector3_dot(Vector3d l, Vector3d r)
{
    return l.x * r.x + l.y * r.y + l.z * r.z;
}

forceinline Vector3d Vector3_cross(Vector3d l, Vector3d r)
{
    return Vector3d{l.y * r.z - l.z * r.y, l.x * r.z - l.z * r.x, l.x * r.y - l.y * r.x};
}

NC_DEPRECATED forceinline Vector3d vec3(double x, double y, double z)
{
    return Vector3d(x, y, z);
}

forceinline bool operator==(Vector3d l, Vector3d r)
{
    return l.x == r.x && l.y == r.y && l.z == r.z;
}

forceinline bool operator!=(Vector3d l, Vector3d r)
{
    return l.x != r.x || l.y != r.y || l.z != r.z;
}

forceinline Vector3d operator-(Vector3d l, Vector3d r)
{
    return Vector3d{l.x - r.x, l.y - r.y, l.z - r.z};
}

forceinline Vector3d operator+(Vector3d l, Vector3d r)
{
    return Vector3d{l.x + r.x, l.y + r.y, l.z + r.z};
}

forceinline Vector3d operator*(Vector3d l, double scaler)
{
    return Vector3d{l.x * scaler, l.y * scaler, l.z * scaler};
}

forceinline Vector3d operator/(Vector3d l, double scaler)
{
    return Vector3d{l.x / scaler, l.y / scaler, l.z / scaler};
}

} // namespace nc
