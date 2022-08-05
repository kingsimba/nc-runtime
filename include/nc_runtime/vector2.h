#pragma once

#include "nc_runtime/nc_types.h"

namespace nc
{
template <typename T>
class Vector2T
{
public:
    T x, y;

    static Vector2T zero() { return Vector2T{0, 0}; }
    static Vector2T fill(T v) { return Vector2T{v, v}; }

    Vector2T() = default;
    forceinline Vector2T(T x_, T y_) : x(x_), y(y_) {}

    forceinline void set(T x_, T y_) { x = x_, y = y_; }
    forceinline void setZero() { x = y = 0; }
    forceinline T length() const { return (T)std::sqrt(x * x + y * y); }
    forceinline T lengthSquared() const { return x * x + y * y; }

    forceinline void operator+=(Vector2T r)
    {
        x += r.x;
        y += r.y;
    }

    forceinline void operator-=(Vector2T r)
    {
        x -= r.x;
        y -= r.y;
    }

    forceinline void operator*=(T r)
    {
        x *= r;
        y *= r;
    }

    forceinline void operator/=(T r)
    {
        x /= r;
        y /= r;
    }

    forceinline Vector2T operator-() const { return Vector2T(-x, -y); }
};

template <typename T>
forceinline T Vector2_dot(Vector2T<T> l, Vector2T<T> r)
{
    return l.x * r.x + l.y * r.y;
}

template <typename T>
forceinline T Vector2_cross(Vector2T<T> l, Vector2T<T> r)
{
    return l.x * r.y - l.y * r.x;
}

template <typename T>
forceinline bool operator==(Vector2T<T> l, Vector2T<T> r)
{
    return l.x == r.x && l.y == r.y;
}

template <typename T>
forceinline bool operator!=(Vector2T<T> l, Vector2T<T> r)
{
    return l.x != r.x || l.y != r.y;
}

template <typename T>
forceinline Vector2T<T> operator-(Vector2T<T> l, Vector2T<T> r)
{
    return Vector2T<T>{l.x - r.x, l.y - r.y};
}

template <typename T>
forceinline Vector2T<T> operator+(Vector2T<T> l, Vector2T<T> r)
{
    return Vector2T<T>{l.x + r.x, l.y + r.y};
}

template <typename T>
forceinline Vector2T<T> operator*(Vector2T<T> l, T scaler)
{
    return Vector2T<T>{l.x * scaler, l.y * scaler};
}

template <typename T>
forceinline Vector2T<T> operator/(Vector2T<T> l, T scaler)
{
    return Vector2T<T>{l.x / scaler, l.y / scaler};
}

typedef Vector2T<float> Vector2;
typedef Vector2T<double> Vector2d;
typedef Vector2T<int> Vector2i;

} // namespace nc
