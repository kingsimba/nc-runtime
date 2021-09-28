#pragma once

#include "vector2.h"

namespace nc
{

class Pose2
{
public:
    Vector2 pos;
    float ori;

    Pose2() = default;
    Pose2(float x, float y, float ori_) : pos(x, y), ori(ori_) {}
};

} // namespace nc
