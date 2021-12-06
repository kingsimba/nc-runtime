#include "stdafx_nc_runtime.h"
#include "nc_runtime/base64.h"
#include "./turbo_base64/turbob64.h"

size_t Base64_getEncodeLength(size_t inputLength)
{
    return tb64enclen(inputLength);
}
size_t Base64_getDecodeLength(const char* in, size_t inputLength)
{
    return tb64declen((const u8*)in, inputLength);
}

sp<NcString> Base64_encode(const char* in, size_t len)
{
    size_t outLen = tb64enclen(len);
    char* out = (char*)malloc(outLen + 1);
    if (out == NULL)
        return nullptr;

    outLen = tb64xenc((const u8*)in, len, (u8*)out);
    out[outLen] = 0;
    return NcString::allocByTakingBytes(out, outLen);
}

sp<NcString> Base64_decode(const char* in, size_t len)
{
    size_t outLen = tb64declen((const u8*)in, len);
    char* out = (char*)malloc(outLen + 1);
    if (out == NULL)
        return nullptr;

    outLen = tb64xdec((const u8*)in, len, (u8*)out);
    out[outLen] = 0;
    return NcString::allocByTakingBytes(out, outLen);
}
