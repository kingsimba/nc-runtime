#include "stdafx_nc_runtime.h"
#include "nc_object.h"
#include "nc_string.h"

sp<NcString> NcObject::toString() {
  char buffer[64];
  snprintf(buffer, countof(buffer), "Object %zu", (size_t)this);
  return NcString::allocWithCString(buffer);
}
