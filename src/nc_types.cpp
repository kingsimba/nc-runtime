#include "stdafx_nc_runtime.h"
#include "nc_runtime/nc_types.h"
#include "nc_runtime/vector3.h"
#include <iostream>

std::ostream& operator<<(std::ostream& os, const Size& c)
{
    char buffer[64];
    sprintf(buffer, "Size(%d, %d)", c.width, c.height);
    return os << buffer;
}

std::ostream& operator<<(std::ostream& os, const Rgba8& c)
{
    char buffer[64];
    sprintf(buffer, "Rgba8(%d, %d, %d, %d)", c.r, c.g, c.b, c.a);
    return os << buffer;
}

std::ostream& operator<<(std::ostream& os, const Range& c)
{
    char buffer[64];
    sprintf(buffer, "Range(%d, %d)", c.location, c.length);
    return os << buffer;
}

std::ostream& operator<<(std::ostream& os, const nc::Vector3& c)
{
    char buffer[128];
    sprintf(buffer, "vec3(%f, %f, %f)", c.x, c.y, c.z);
    return os << buffer;
}
