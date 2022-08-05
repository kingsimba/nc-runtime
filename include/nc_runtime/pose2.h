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

    forceinline float oriInDegree() const { return nc_radianToDegree(this->ori); }

    Pose2() = default;
    forceinline Pose2(const Pose2& r) : pos(r.pos), ori(r.ori) {}
    forceinline Pose2(const Vector2& pos_, float ori_) : pos(pos_), ori(ori_) {}
    forceinline Pose2(float x, float y, float ori_) : pos(x, y), ori(ori_) {}
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
