#include "nc_types.h"
#include <iostream>

std::ostream& operator<<(std::ostream& os, const Size& c) {
  char buffer[64];
  sprintf(buffer, "Size(%d, %d)", c.width, c.height);
  return os << buffer;
}

std::ostream& operator<<(std::ostream& os, const Rgba8& c) { return os << c.r; }
