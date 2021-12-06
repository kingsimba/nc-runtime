#pragma once

#include "nc_string.h"

size_t Base64_getEncodeLength(size_t inputLength);
size_t Base64_getDecodeLength(const char* in, size_t inputLength);

sp<NcString> Base64_encode(const char* in, size_t len);
sp<NcString> Base64_decode(const char* in,
                           size_t len); // the output is actually a buffer. Sometimes, it happens to be a valid string.
