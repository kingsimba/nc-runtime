#include "stdafx_nc_runtime.h"
#include "nc_runtime/quaternion.h"

namespace nc
{

void Quaternion::initWithRPY(float roll, float pitch, float yaw)
{
    float halfYaw = yaw * 0.5f;
    float halfPitch = pitch * 0.5f;
    float halfRoll = roll * 0.5f;
    float cosYaw = cosf(halfYaw);
    float sinYaw = sinf(halfYaw);
    float cosPitch = cosf(halfPitch);
    float sinPitch = sinf(halfPitch);
    float cosRoll = cosf(halfRoll);
    float sinRoll = sinf(halfRoll);
    this->x = sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw;
    this->y = cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw;
    this->z = cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw;
    this->w = cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw;
}

RollPitchYaw Quaternion::toRPY()
{
    RollPitchYaw angles;

    // roll (x-axis rotation)
    float sinr_cosp = 2 * (this->w * this->x + this->y * this->z);
    float cosr_cosp = 1 - 2 * (this->x * this->x + this->y * this->y);
    angles.roll = atan2f(sinr_cosp, cosr_cosp);

    // pitch (y-axis rotation)
    float sinp = 2 * (this->w * this->y - this->z * this->x);
    if (nc_abs(sinp) >= 1)
        angles.pitch = copysignf(NC_PI_2, sinp); // use 90 degrees if out of range
    else
        angles.pitch = asinf(sinp);

    // yaw (z-axis rotation)
    float siny_cosp = 2 * (this->w * this->z + this->x * this->y);
    float cosy_cosp = 1 - 2 * (this->y * this->y + this->z * this->z);
    angles.yaw = atan2f(siny_cosp, cosy_cosp);

    return angles;
}

void Quaternion::multiply(const Quaternion& l)
{
    Quaternion o;

    o.x = l.x * this->w + l.y * this->z - l.z * this->y + l.w * this->x;
    o.y = -l.x * this->z + l.y * this->w + l.z * this->x + l.w * this->y;
    o.z = l.x * this->y - l.y * this->x + l.z * this->w + l.w * this->z;
    o.w = -l.x * this->x - l.y * this->y - l.z * this->z + l.w * this->w;

    *this = o;
}

Quaternion Quaternion_multiply(const Quaternion& second, const Quaternion& first)
{
    Quaternion o;

    o.x = second.x * first.w + second.y * first.z - second.z * first.y + second.w * first.x;
    o.y = -second.x * first.z + second.y * first.w + second.z * first.x + second.w * first.y;
    o.z = second.x * first.y - second.y * first.x + second.z * first.w + second.w * first.z;
    o.w = -second.x * first.x - second.y * first.y - second.z * first.z + second.w * first.w;

    return o;
}

Vector3 Quaternion::transformVector(Vector3 v)
{
    Vector3 rtn;

    float num12 = this->x + this->x;
    float num2 = this->y + this->y;
    float num = this->z + this->z;
    float num11 = this->w * num12;
    float num10 = this->w * num2;
    float num9 = this->w * num;
    float num8 = this->x * num12;
    float num7 = this->x * num2;
    float num6 = this->x * num;
    float num5 = this->y * num2;
    float num4 = this->y * num;
    float num3 = this->z * num;
    float num15 = ((v.x * ((1.0f - num5) - num3)) + (v.y * (num7 - num9))) + (v.z * (num6 + num10));
    float num14 = ((v.x * (num7 + num9)) + (v.y * ((1.0f - num8) - num3))) + (v.z * (num4 - num11));
    float num13 = ((v.x * (num6 - num10)) + (v.y * (num4 + num11))) + (v.z * ((1.0f - num8) - num5));

    rtn.x = num15;
    rtn.y = num14;
    rtn.z = num13;

    return rtn;
}

} // namespace nc
