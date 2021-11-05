#include "stdafx_nc_runtime.h"
#include "nc_runtime/nc_object.h"
#include "nc_runtime/nc_string.h"

NcObject* NcObject::allocRawObjectWithSize(size_t size, bool zero_memory)
{
    void* buffer;
    if (zero_memory)
        buffer = calloc(size + sizeof(ControlBlock), 1);
    else
        buffer = malloc(size + sizeof(ControlBlock));
    ControlBlock* ctrl = (ControlBlock*)buffer;
    ::new (ctrl) ControlBlock();
    return (NcObject*)(ctrl + 1);
}

sp<NcString> NcObject::toString()
{
    char buffer[64];
    snprintf(buffer, countof(buffer), "Object %zu", (size_t)this);
    return NcString::allocWithCString(buffer);
}
