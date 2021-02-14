#pragma once

#include "basic_types.h"

class NcObject {
public:
  virtual sp<NcString> toString() = 0;
};
