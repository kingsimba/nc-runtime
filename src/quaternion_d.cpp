#include "stdafx_nc_runtime.h"
#include "nc_runtime/quaternion_d.h"

namespace nc
{

void QuaternionD::initWithRPY(double roll, double pitch, double yaw)
{
    double halfYaw = yaw * 0.5;
    double halfPitch = pitch * 0.5;
    double halfRoll = roll * 0.5;
    double cosYaw = cos(halfYaw);
    double sinYaw = sin(halfYaw);
    double cosPitch = cos(halfPitch);
    double sinPitch = sin(halfPitch);
    double cosRoll = cos(halfRoll);
    double sinRoll = sin(halfRoll);
    this->x = sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw;
    this->y = cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw;
    this->z = cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw;
    this->w = cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw;
}

EulerAnglesD QuaternionD::ToEulerAngles()
{
    EulerAnglesD angles;

    // roll (x-axis rotation)
    double sinr_cosp = 2 * (this->w * this->x + this->y * this->z);
    double cosr_cosp = 1 - 2 * (this->x * this->x + this->y * this->y);
    angles.roll = atan2(sinr_cosp, cosr_cosp);

    // pitch (y-axis rotation)
    double sinp = 2 * (this->w * this->y - this->z * this->x);
    if (nc_abs(sinp) >= 1)
        angles.pitch = copysign(NC_PI_2, sinp); // use 90 degrees if out of range
    else
        angles.pitch = asin(sinp);

    // yaw (z-axis rotation)
    double siny_cosp = 2 * (this->w * this->z + this->x * this->y);
    double cosy_cosp = 1 - 2 * (this->y * this->y + this->z * this->z);
    angles.yaw = atan2(siny_cosp, cosy_cosp);

    return angles;
}

void QuaternionD::multiply(const QuaternionD& l)
{
    QuaternionD o;

    o.x = l.x * this->w + l.y * this->z - l.z * this->y + l.w * this->x;
    o.y = -l.x * this->z + l.y * this->w + l.z * this->x + l.w * this->y;
    o.z = l.x * this->y - l.y * this->x + l.z * this->w + l.w * this->z;
    o.w = -l.x * this->x - l.y * this->y - l.z * this->z + l.w * this->w;

    *this = o;
}

QuaternionD QuaternionD_multiply(const QuaternionD& second, const QuaternionD& first)
{
    QuaternionD o;

    o.x = second.x * first.w + second.y * first.z - second.z * first.y + second.w * first.x;
    o.y = -second.x * first.z + second.y * first.w + second.z * first.x + second.w * first.y;
    o.z = second.x * first.y - second.y * first.x + second.z * first.w + second.w * first.z;
    o.w = -second.x * first.x - second.y * first.y - second.z * first.z + second.w * first.w;

    return o;
}

} // namespace nc