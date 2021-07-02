#pragma once

#include "nc_runtime/nc_types.h"

namespace nc
{

struct EulerAngles
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

    void multiply(const Quaternion& r);

    EulerAngles ToEulerAngles();
};

static forceinline Quaternion vec2(float x, float y, float z, float w)
{
    return Quaternion{x, y, z, w};
}

Quaternion Quaternion_multiply(const Quaternion& second, const Quaternion& first);

} // namespace nc
