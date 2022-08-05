#pragma once

#include "nc_types.h"
#include "vector2.h"

namespace nc
{

class Pose2
{
public:
    Vector2 pos;
    float ori;

    float oriInDegree() const { return nc_radianToDegree(this->ori); }

    Pose2() = default;
    Pose2(float x, float y, float ori_) : pos(x, y), ori(ori_) {}
};

forceinline bool operator==(const Pose2& l, const Pose2& r)
{
    return l.pos == r.pos && l.ori == r.ori;
}

forceinline bool operator!=(const Pose2& l, const Pose2& r)
{
    return l.pos != r.pos || l.ori != r.ori;
}

} // namespace nc
