#pragma once

#include "nc_string.h"

enum class FileOpenFlag
{
    none = 0,
    read = 1,
    write = 2,
    openExisting = 4,
    createAlways = 8
};

forceinline FileOpenFlag operator|(FileOpenFlag l, FileOpenFlag r)
{
    return FileOpenFlag((int)l | (int)r);
}
forceinline bool operator&(FileOpenFlag l, FileOpenFlag r)
{
    return ((int)l & (int)r) != 0;
}

enum class FileSeekOrigin
{
    none = 0,
    current,
    begin,
    end
};

class NcFile : public NcObject
{
public:
    NcFile() = default;
    ~NcFile() { close(); }

    static sp<NcFile> alloc(const StringSlice& slice, FileOpenFlag flags);

    void close();

    i64 length();

    bool seek(FileSeekOrigin origin, i64 offset);
    i64 tell();
    int read(void* bufferOut, int bytesToRead);      // return number of bytes read
    int write(const void* buffer, int bytesToWrite); // return number of bytes written

private:
    FILE* m_fp = NULL;
};
