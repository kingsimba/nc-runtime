#include "stdafx_nc_runtime.h"
#include "nc_runtime/nc_file.h"

sp<NcFile> NcFile::alloc(const StringSlice& fileName, FileOpenFlag flags)
{
    const char* mode = nullptr;
    bool openExisting = flags & FileOpenFlag::openExisting;
    bool createAlways = flags & FileOpenFlag::createAlways;

    if (openExisting && createAlways)
    {
        NC_ASSERT("openExisting & createAlways cannot coexist");
        return nullptr;
    }

    if (openExisting)
    {
        if (flags & FileOpenFlag::write)
        {
            mode = "r+b";
        }
        else if (flags & FileOpenFlag::read)
        {
            mode = "rb";
        }
    }
    else if (createAlways)
    {
        if (flags & FileOpenFlag::read)
        {
            mode = "w+b";
        }
        else if (flags & FileOpenFlag::write)
        {
            mode = "wb";
        }
    }
    else // OpenIfExists
    {
        if (flags & FileOpenFlag::write)
        {
            mode = "r+b";
        }
        else
        {
            mode = "rb";
        }
    }

    FILE* fp = fopen(fileName.cstr(), mode);
    if (fp == NULL)
        return NULL;

    if ((flags & FileOpenFlag::append) && (flags & FileOpenFlag::write))
    {
        fseek(fp, 0, SEEK_END);
    }
    else
    {
        fseek(fp, 0, SEEK_SET);
    }

    sp<NcFile> o = NcObject::alloc<NcFile>();
    o->m_fp = fp;
    return o;
}

void NcFile::close()
{
    if (m_fp != NULL)
    {
        fclose(m_fp);
        m_fp = NULL;
    }
}

i64 NcFile::tell()
{
    if (m_fp == NULL)
        return 0;

    return ftell(m_fp);
}

i64 NcFile::length()
{
    i64 pos = tell();
    seek(FileSeekOrigin::end, 0);
    i64 len = tell();
    seek(FileSeekOrigin::begin, pos);
    return len;
}

bool NcFile::seek(FileSeekOrigin origin, i64 offset)
{
    if (m_fp == NULL)
        return false;

    int seek;
    switch (origin)
    {
    case FileSeekOrigin::current:
        seek = SEEK_CUR;
        break;
    case FileSeekOrigin::end:
        seek = SEEK_END;
        break;
    default:
        seek = SEEK_SET;
        break;
    }

    return fseek(m_fp, (int)offset, seek) == 0;
}

int NcFile::read(void* bufferOut, int bytesToRead)
{
    if (m_fp == NULL)
        return 0;
    int rtn = (int)fread(bufferOut, 1, bytesToRead, m_fp);
    return rtn;
}

int NcFile::write(const void* buffer, int bytesToWrite)
{
    if (m_fp == NULL)
        return 0;
    int rtn = (int)fwrite(buffer, 1, bytesToWrite, m_fp);
    return rtn;
}
