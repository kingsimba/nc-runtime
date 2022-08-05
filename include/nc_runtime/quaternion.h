#pragma once

#include "nc_runtime/nc_types.h"
#include "vector3.h"

namespace nc
{

template <typename T>
struct RollPitchYawT
{
    RollPitchYawT() = default;
    RollPitchYawT(T r, T p, T y) : roll(r), pitch(p), yaw(y) {}
    T roll, pitch, yaw;
};

template <typename T>
forceinline bool operator==(RollPitchYawT<T> l, RollPitchYawT<T> r)
{
    return l.roll == r.roll && l.pitch == r.pitch && l.yaw == r.yaw;
}

template <typename T>
forceinline bool operator!=(RollPitchYawT<T> l, RollPitchYawT<T> r)
{
    return l.roll != r.roll || l.pitch != r.pitch || l.yaw != r.yaw;
}

////////////////////////////////

template <typename T>
class QuaternionT
{
public:
    T x, y, z, w;

    static QuaternionT zero() { return QuaternionT{0, 0, 0, 0}; }
    static QuaternionT identity() { return QuaternionT{0, 0, 0, 1}; }
    static forceinline QuaternionT makeWithRPY(const RollPitchYawT<T>& rpy)
    {
        QuaternionT q;
        q.initWithRPY(rpy.roll, rpy.pitch, rpy.yaw);
        return q;
    }
    static forceinline QuaternionT makeWithRPY(float roll, float pitch, float yaw)
    {
        QuaternionT q;
        q.initWithRPY(roll, pitch, yaw);
        return q;
    }

    QuaternionT() = default;
    QuaternionT(T x_, T y_, T z_, T w_) : x(x_), y(y_), z(z_), w(w_) {}

    forceinline void init(T x_, T y_, T z_, T w_)
    {
        this->x = x_;
        this->y = y_;
        this->z = z_;
        this->w = w_;
    }

    forceinline void set(T x_, T y_, T z_, T w_)
    {
        this->x = x_;
        this->y = y_;
        this->z = z_;
        this->w = w_;
    }

    forceinline void initWithIdentity()
    {
        this->x = this->y = this->z = 0;
        this->w = 1;
    }

    forceinline QuaternionT inverse() const { return QuaternionT(x, y, z, -w); }

    void initWithRPY(T roll, T pitch, T yaw);
    forceinline void initWithRPY(RollPitchYawT<T> rpy) { initWithRPY(rpy.roll, rpy.pitch, rpy.yaw); }

    void multiply(const QuaternionT& r);
    Vector3T<T> transformVector(Vector3T<T> value) const;

    RollPitchYawT<T> toRPY() const;
};

template <typename T>
QuaternionT<T> Quaternion_multiply(const QuaternionT<T>& second, const QuaternionT<T>& first);

template <typename T>
forceinline QuaternionT<T> operator*(const QuaternionT<T>& second, const QuaternionT<T>& first)
{
    return Quaternion_multiply(second, first);
}

template <typename T>
forceinline Vector3T<T> operator*(const QuaternionT<T>& ori, const Vector3T<T>& v)
{
    return ori.transformVector(v);
}

template <typename T>
forceinline bool operator==(const QuaternionT<T>& l, const QuaternionT<T>& r)
{
    return l.x == r.x && l.y == r.y && l.z == r.z && l.w == r.w;
}

template <typename T>
forceinline bool operator!=(const QuaternionT<T>& l, const QuaternionT<T>& r)
{
    return l.x != r.x || l.y != r.y || l.z != r.z || l.w != r.w;
}

////////////////////////////////////////////////////////////////////

typedef RollPitchYawT<float> RollPitchYaw;
typedef RollPitchYawT<double> RollPitchYawD;

typedef QuaternionT<float> Quaternion;
typedef QuaternionT<double> QuaternionD;

} // namespace nc
