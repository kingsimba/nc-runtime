#pragma once

#include "nc_runtime/nc_types.h"

namespace nc
{

class Vector2
{
public:
    float x, y;
};

static forceinline Vector2 vec2(float x, float y)
{
    return Vector2{x, y};
}

} // namespace nc
