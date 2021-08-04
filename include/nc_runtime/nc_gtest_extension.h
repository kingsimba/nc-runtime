#pragma once

#include <ostream>
#include "nc_types.h"
#include "vector3.h"

std::ostream& operator<<(std::ostream& os, const Size& c);
std::ostream& operator<<(std::ostream& os, const Rgba8& c);
std::ostream& operator<<(std::ostream& os, const Range& c);
std::ostream& operator<<(std::ostream& os, const Range& c);
std::ostream& operator<<(std::ostream& os, const nc::Vector3& c);
