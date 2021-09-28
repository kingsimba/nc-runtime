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

    QuaternionD() = default;
    QuaternionD(double x_, double y_, double z_, double w_) : x(x_), y(y_), z(z_), w(w_) {}

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

QuaternionD QuaternionD_multiply(const QuaternionD& second, const QuaternionD& first);

} // namespace nc
