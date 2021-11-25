#include "stdafx_nc_runtime.h"
#include "nc_runtime/nc_gtest_extension.h"

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

std::ostream& operator<<(std::ostream& os, const nc::Vector2& c)
{
    char buffer[128];
    sprintf(buffer, "Vector2(%f, %f)", c.x, c.y);
    return os << buffer;
}

std::ostream& operator<<(std::ostream& os, const nc::Vector2i& c)
{
    char buffer[128];
    sprintf(buffer, "Vector2(%d, %d)", c.x, c.y);
    return os << buffer;
}

std::ostream& operator<<(std::ostream& os, const nc::Vector3& c)
{
    char buffer[128];
    sprintf(buffer, "Vector3(%f, %f, %f)", c.x, c.y, c.z);
    return os << buffer;
}

std::ostream& operator<<(std::ostream& os, const nc::Vector3i& c)
{
    char buffer[128];
    sprintf(buffer, "Vector3i(%d, %d, %d)", c.x, c.y, c.z);
    return os << buffer;
}

std::ostream& operator<<(std::ostream& os, const nc::Quaternion& c)
{
    char buffer[128];
    sprintf(buffer, "Quaternion(%f, %f, %f, %f)", c.x, c.y, c.z, c.w);
    return os << buffer;
}

std::ostream& operator<<(std::ostream& os, const nc::Pose2& c)
{
    char buffer[128];
    sprintf(buffer, "Pose2(%f, %f, %f)", c.pos.x, c.pos.y, c.ori);
    return os << buffer;
}

std::ostream& operator<<(std::ostream& os, const StringSlice& c)
{
    return os << std::string(c);
}
