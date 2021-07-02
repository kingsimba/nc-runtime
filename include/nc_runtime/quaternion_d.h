#pragma once

#include "nc_runtime/nc_types.h"

namespace nc
{

struct EulerAnglesD
{
    double roll, pitch, yaw;
};

class QuaternionD
{
public:
    double x, y, z, w;

    forceinline void init(double x_, double y_, double z_, double w_)
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

    void initWithRPY(double roll, double pitch, double yaw);

    void multiply(const QuaternionD& r);

    EulerAnglesD ToEulerAngles();
};

static forceinline QuaternionD vec2(double x, double y, double z, double w)
{
    return QuaternionD{x, y, z, w};
}

QuaternionD Quaternion_multiply(const QuaternionD& second, const QuaternionD& first);

} // namespace nc
