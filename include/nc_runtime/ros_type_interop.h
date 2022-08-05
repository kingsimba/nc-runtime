#pragma once

#include "pose3.h"
#include <geometry_msgs/Vector3.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/Quaternion.h>

namespace nc
{
forceinline geometry_msgs::Vector3 Vector3_toRos(const Vector3& o)
{
    geometry_msgs::Vector3 v;
    v.x = o.x, v.y = o.y, v.z = o.z;
    return v;
}

forceinline Vector3 Vector3_fromRos(const geometry_msgs::Vector3& msg)
{
    return Vector3(msg.x, msg.y, msg.z);
}

forceinline geometry_msgs::Quaternion Quaternion_toRos(const Quaternion& o)
{
    geometry_msgs::Quaternion v;
    v.x = o.x, v.y = o.y, v.z = o.z, v.w = o.w;
    return v;
}

forceinline Quaternion Quaternion_fromRos(const geometry_msgs::Quaternion& msg)
{
    return Quaternion(msg.x, msg.y, msg.z, msg.w);
}

forceinline geometry_msgs::Pose Pose3_toRos(const Pose3& o)
{
    geometry_msgs::Pose pose;
    pose.position.x = o.pos.x;
    pose.position.y = o.pos.y;
    pose.position.z = o.pos.z;
    pose.orientation.x = o.ori.x;
    pose.orientation.y = o.ori.y;
    pose.orientation.z = o.ori.z;
    pose.orientation.w = o.ori.w;
    return pose;
}

forceinline Pose3 Pose3_fromRos(const geometry_msgs::Pose& msg)
{
    return Pose3(Vector3(msg.position.x, msg.position.y, msg.position.z),
                 Quaternion(msg.orientation.x, msg.orientation.y, msg.orientation.z, msg.orientation.w));
}

} // namespace nc