#pragma once

#include "nc_runtime/nc_types.h"
#include "vector3.h"

namespace nc
{

struct RollPitchYaw
{
    float roll, pitch, yaw;
};

class Quaternion
{
public:
    float x, y, z, w;

    forceinline void init(float x_, float y_, float z_, float w_)
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

    void initWithRPY(float roll, float pitch, float yaw);
    forceinline void initWithRPY(RollPitchYaw rpy) { initWithRPY(rpy.roll, rpy.pitch, rpy.yaw); }

    void multiply(const Quaternion& r);
    Vector3 transformVector(Vector3 value);

    RollPitchYaw toRPY();
};

static forceinline Quaternion quaternion(float x, float y, float z, float w)
{
    return Quaternion{x, y, z, w};
}

Quaternion Quaternion_multiply(const Quaternion& second, const Quaternion& first);

} // namespace nc
