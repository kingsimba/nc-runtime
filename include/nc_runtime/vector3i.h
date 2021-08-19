#pragma once

#include "nc_runtime/nc_types.h"

namespace nc
{

class Vector3i
{
public:
    int x, y, z;

    int length() const { return (int)sqrtf((float)(x * x + y * y + z * z)); }
    int lengthSquared() const { return x * x + y * y + z * z; }
};

forceinline Vector3i vec3i(int x, int y, int z)
{
    return Vector3i{x, y, z};
}

forceinline bool operator==(Vector3i l, Vector3i r)
{
    return l.x == r.x && l.y == r.y && l.z == r.z;
}

forceinline bool operator!=(Vector3i l, Vector3i r)
{
    return l.x != r.x || l.y != r.y || l.z != r.z;
}

forceinline Vector3i operator-(Vector3i l, Vector3i r)
{
    return Vector3i{l.x - r.x, l.y - r.y, l.z - r.z};
}

forceinline Vector3i operator+(Vector3i l, Vector3i r)
{
    return Vector3i{l.x + r.x, l.y + r.y, l.z + r.z};
}

forceinline Vector3i operator*(Vector3i l, int scaler)
{
    return Vector3i{l.x * scaler, l.y * scaler, l.z * scaler};
}

forceinline Vector3i operator/(Vector3i l, int scaler)
{
    return Vector3i{l.x / scaler, l.y / scaler, l.z / scaler};
}

} // namespace nc
