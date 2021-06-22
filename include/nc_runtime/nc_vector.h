#pragma once

#include "nc_runtime/nc_types.h"

struct NcVector2
{
    float x, y;
};

static forceinline NcVector2 vec2(float x, float y)
{
    return NcVector2{x, y};
}
