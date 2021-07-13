#pragma once

#include "nc_types.h"

// wrap a angle between [0, NC_TWO_PI)
float Math_wrapTwoPi(float angle);

// calculate the nearest turn angle between two directions.
float Math_turnAngle(float from, float to);
