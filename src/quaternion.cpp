#include "stdafx_nc_runtime.h"
#include "nc_runtime/quaternion.h"
#include <cmath>

namespace nc
{

template <typename T>
void QuaternionT<T>::initWithRPY(T roll, T pitch, T yaw)
{
    T halfYaw = yaw * 0.5f;
    T halfPitch = pitch * 0.5f;
    T halfRoll = roll * 0.5f;
    T cosYaw = std::cos(halfYaw);
    T sinYaw = std::sin(halfYaw);
    T cosPitch = std::cos(halfPitch);
    T sinPitch = std::sin(halfPitch);
    T cosRoll = std::cos(halfRoll);
    T sinRoll = std::sin(halfRoll);
    this->x = sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw;
    this->y = cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw;
    this->z = cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw;
    this->w = cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw;
}

template <typename T>
RollPitchYawT<T> QuaternionT<T>::toRPY()
{
    RollPitchYawT<T> angles;

    // roll (x-axis rotation)
    T sinr_cosp = 2 * (this->w * this->x + this->y * this->z);
    T cosr_cosp = 1 - 2 * (this->x * this->x + this->y * this->y);
    angles.roll = std::atan2(sinr_cosp, cosr_cosp);

    // pitch (y-axis rotation)
    T sinp = 2 * (this->w * this->y - this->z * this->x);
    if (nc_abs(sinp) >= 1)
        angles.pitch = std::copysign(NC_PI_2, sinp); // use 90 degrees if out of range
    else
        angles.pitch = std::asin(sinp);

    // yaw (z-axis rotation)
    T siny_cosp = 2 * (this->w * this->z + this->x * this->y);
    T cosy_cosp = 1 - 2 * (this->y * this->y + this->z * this->z);
    angles.yaw = std::atan2(siny_cosp, cosy_cosp);

    return angles;
}

template <typename T>
void QuaternionT<T>::multiply(const QuaternionT<T>& l)
{
    QuaternionT<T> o;

    o.x = l.x * this->w + l.y * this->z - l.z * this->y + l.w * this->x;
    o.y = -l.x * this->z + l.y * this->w + l.z * this->x + l.w * this->y;
    o.z = l.x * this->y - l.y * this->x + l.z * this->w + l.w * this->z;
    o.w = -l.x * this->x - l.y * this->y - l.z * this->z + l.w * this->w;

    *this = o;
}

template <typename T>
QuaternionT<T> Quaternion_multiply(const QuaternionT<T>& second, const QuaternionT<T>& first)
{
    QuaternionT<T> o;

    o.x = second.x * first.w + second.y * first.z - second.z * first.y + second.w * first.x;
    o.y = -second.x * first.z + second.y * first.w + second.z * first.x + second.w * first.y;
    o.z = second.x * first.y - second.y * first.x + second.z * first.w + second.w * first.z;
    o.w = -second.x * first.x - second.y * first.y - second.z * first.z + second.w * first.w;

    return o;
}

template <typename T>
Vector3T<T> QuaternionT<T>::transformVector(Vector3T<T> v)
{
    Vector3T<T> rtn;

    T num12 = this->x + this->x;
    T num2 = this->y + this->y;
    T num = this->z + this->z;
    T num11 = this->w * num12;
    T num10 = this->w * num2;
    T num9 = this->w * num;
    T num8 = this->x * num12;
    T num7 = this->x * num2;
    T num6 = this->x * num;
    T num5 = this->y * num2;
    T num4 = this->y * num;
    T num3 = this->z * num;
    T num15 = ((v.x * ((1.0f - num5) - num3)) + (v.y * (num7 - num9))) + (v.z * (num6 + num10));
    T num14 = ((v.x * (num7 + num9)) + (v.y * ((1.0f - num8) - num3))) + (v.z * (num4 - num11));
    T num13 = ((v.x * (num6 - num10)) + (v.y * (num4 + num11))) + (v.z * ((1.0f - num8) - num5));

    rtn.x = num15;
    rtn.y = num14;
    rtn.z = num13;

    return rtn;
}

template class QuaternionT<float>;
template class QuaternionT<double>;

template QuaternionT<float> Quaternion_multiply(const QuaternionT<float>& second, const QuaternionT<float>& first);
template QuaternionT<double> Quaternion_multiply(const QuaternionT<double>& second, const QuaternionT<double>& first);

} // namespace nc
