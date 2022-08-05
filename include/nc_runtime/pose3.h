#pragma once

#include "nc_types.h"
#include "vector3.h"
#include "quaternion.h"
#include "pose2.h"

namespace nc
{

class Pose3
{
public:
    Vector3 pos = Vector3::zero();
    Quaternion ori = Quaternion::identity();

    Pose3() = default;
    forceinline Pose3(const Pose3& r) : pos(r.pos), ori(r.ori) {}
    forceinline Pose3(const Vector3& pos_, const Quaternion& ori_) : pos(pos_), ori(ori_) {}
    static forceinline Pose3 makeWithPose2(const Pose2& pose2)
    {
        return Pose3(Vector3(pose2.pos.x, pose2.pos.y, 0), Quaternion::makeWithRPY(0, 0, pose2.ori));
    }

    forceinline Pose3 inverse() const { return Pose3(-this->pos, this->ori.inverse()); }

    forceinline Pose2 toPose2() const { return Pose2(this->pos.x, this->pos.y, this->ori.toRPY().yaw); }
};

forceinline bool operator==(const Pose3& l, const Pose3& r)
{
    return l.pos == r.pos && l.ori == r.ori;
}

forceinline bool operator!=(const Pose3& l, const Pose3& r)
{
    return l.pos != r.pos || l.ori != r.ori;
}

forceinline Pose3 operator*(const Pose3& l, const Pose3& r)
{
    return Pose3(l.pos + l.ori * r.pos, l.ori * r.ori);
}

} // namespace nc
