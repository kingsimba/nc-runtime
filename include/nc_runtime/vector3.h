#pragma once

#include "nc_runtime/nc_types.h"

namespace nc
{

template <typename T>
class Vector3T
{
public:
    T x, y, z;

    Vector3T() = default;
    forceinline Vector3T(T x_, T y_, T z_) : x(x_), y(y_), z(z_) {}

    forceinline void set(T x_, T y_, T z_) { x = x_, y = y_, z = z_; }
    forceinline void setZero() { x = y = z = 0; }
    forceinline T length() const { return (T)std::sqrt(x * x + y * y + z * z); }
    forceinline T lengthSquared() const { return x * x + y * y + z * z; }

    forceinline void operator+=(Vector3T r)
    {
        x += r.x;
        y += r.y;
        z += r.z;
    }

    forceinline void operator-=(Vector3T r)
    {
        x -= r.x;
        y -= r.y;
        z -= r.z;
    }

    forceinline void operator*=(T r)
    {
        x *= r;
        y *= r;
        z *= r;
    }

    forceinline void operator/=(T r)
    {
        x /= r;
        y /= r;
        z /= r;
    }
};

template <typename T>
forceinline T Vector3_dot(Vector3T<T> l, Vector3T<T> r)
{
    return l.x * r.x + l.y * r.y + l.z * r.z;
}

template <typename T>
forceinline Vector3T<T> Vector3_cross(Vector3T<T> l, Vector3T<T> r)
{
    return Vector3T<T>{l.y * r.z - l.z * r.y, l.x * r.z - l.z * r.x, l.x * r.y - l.y * r.x};
}

template <typename T>
forceinline bool operator==(Vector3T<T> l, Vector3T<T> r)
{
    return l.x == r.x && l.y == r.y && l.z == r.z;
}

template <typename T>
forceinline bool operator!=(Vector3T<T> l, Vector3T<T> r)
{
    return l.x != r.x || l.y != r.y || l.z != r.z;
}

template <typename T>
forceinline Vector3T<T> operator-(Vector3T<T> l, Vector3T<T> r)
{
    return Vector3T<T>{l.x - r.x, l.y - r.y, l.z - r.z};
}

template <typename T>
forceinline Vector3T<T> operator+(Vector3T<T> l, Vector3T<T> r)
{
    return Vector3T<T>{l.x + r.x, l.y + r.y, l.z + r.z};
}

template <typename T>
forceinline Vector3T<T> operator*(Vector3T<T> l, T scaler)
{
    return Vector3T<T>{l.x * scaler, l.y * scaler, l.z * scaler};
}

template <typename T>
forceinline Vector3T<T> operator/(Vector3T<T> l, T scaler)
{
    return Vector3T<T>{l.x / scaler, l.y / scaler, l.z / scaler};
}

////////////////////////////////////////////////////////////////////

typedef Vector3T<float> Vector3;
typedef Vector3T<double> Vector3d;
typedef Vector3T<int> Vector3i;

} // namespace nc
