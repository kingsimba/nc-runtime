#pragma once

#include "nc_runtime/nc_types.h"

namespace nc
{

class Vector3
{
public:
    float x, y, z;

    float length() const { return sqrtf(x * x + y * y + z * z); }
    float lengthSquared() const { return x * x + y * y + z * z; }
};

forceinline Vector3 vec3(float x, float y, float z)
{
    return Vector3{x, y, z};
}

forceinline bool operator==(Vector3 l, Vector3 r)
{
    return l.x == r.x && l.y == r.y && l.z == r.z;
}

forceinline bool operator!=(Vector3 l, Vector3 r)
{
    return l.x != r.x || l.y != r.y || l.z != r.z;
}

forceinline Vector3 operator-(Vector3 l, Vector3 r)
{
    return Vector3{l.x - r.x, l.y - r.y, l.z - r.z};
}

forceinline Vector3 operator+(Vector3 l, Vector3 r)
{
    return Vector3{l.x + r.x, l.y + r.y, l.z + r.z};
}

forceinline Vector3 operator*(Vector3 l, float scaler)
{
    return Vector3{l.x * scaler, l.y * scaler, l.z * scaler};
}

forceinline Vector3 operator/(Vector3 l, float scaler)
{
    return Vector3{l.x / scaler, l.y / scaler, l.z / scaler};
}

} // namespace nc
