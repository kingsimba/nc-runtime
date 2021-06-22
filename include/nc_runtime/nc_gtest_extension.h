#pragma once

#include <ostream>
#include "nc_types.h"

std::ostream& operator<<(std::ostream& os, const Size& c);
std::ostream& operator<<(std::ostream& os, const Rgba8& c);
std::ostream& operator<<(std::ostream& os, const Range& c);
