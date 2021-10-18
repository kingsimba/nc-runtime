#pragma once

#include <ostream>
#include "nc_types.h"
#include "vector2.h"
#include "vector2i.h"
#include "vector3.h"
#include "vector3i.h"
#include "quaternion.h"
#include "pose2.h"

std::ostream& operator<<(std::ostream& os, const Size& c);
std::ostream& operator<<(std::ostream& os, const Rgba8& c);
std::ostream& operator<<(std::ostream& os, const Range& c);
std::ostream& operator<<(std::ostream& os, const Range& c);
std::ostream& operator<<(std::ostream& os, const nc::Vector2& c);
std::ostream& operator<<(std::ostream& os, const nc::Vector2i& c);
std::ostream& operator<<(std::ostream& os, const nc::Vector3& c);
std::ostream& operator<<(std::ostream& os, const nc::Vector3i& c);
std::ostream& operator<<(std::ostream& os, const nc::Quaternion& c);
std::ostream& operator<<(std::ostream& os, const nc::Pose2& c);
