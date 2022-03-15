#pragma once

#include "nc_runtime/nc_types.h"
#include "vector3.h"

namespace nc
{

template <typename T>
struct RollPitchYawT
{
    T roll, pitch, yaw;
};

template <typename T>
class QuaternionT
{
public:
    T x, y, z, w;

    QuaternionT() = default;
    QuaternionT(T x_, T y_, T z_, T w_) : x(x_), y(y_), z(z_), w(w_) {}

    forceinline void init(T x_, T y_, T z_, T w_)
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

    void initWithRPY(T roll, T pitch, T yaw);
    forceinline void initWithRPY(RollPitchYawT<T> rpy) { initWithRPY(rpy.roll, rpy.pitch, rpy.yaw); }

    void multiply(const QuaternionT& r);
    Vector3T<T> transformVector(Vector3T<T> value);

    RollPitchYawT<T> toRPY();
};

template <typename T>
QuaternionT<T> Quaternion_multiply(const QuaternionT<T>& second, const QuaternionT<T>& first);

////////////////////////////////////////////////////////////////////

typedef RollPitchYawT<float> RollPitchYaw;
typedef RollPitchYawT<double> RollPitchYawD;

typedef QuaternionT<float> Quaternion;
typedef QuaternionT<double> QuaternionD;

} // namespace nc
