#include "stdafx_nc_runtime.h"
#include "nc_runtime/nc_file.h"

sp<NcFile> NcFile::alloc(const StringSlice& fileName, FileOpenFlag flags) {
  char mode[5];
  mode[0] = 0;
  if (flags & FileOpenFlag::read) {
    strcat(mode, "r");
  }
  if (flags & FileOpenFlag::write) {
    strcat(mode, "w");
  }
  strcat(mode, "b");

  char cstr[NC_MAX_PATH];
  fileName.toCString(cstr, NC_MAX_PATH);

  FILE* fp = fopen(cstr, mode);
  if (fp == NULL) return NULL;

  sp<NcFile> o = new NcFile();
  o->m_fp = fp;
  return o;
}

void NcFile::close() {
  if (m_fp != NULL) {
    fclose(m_fp);
    m_fp = NULL;
  }
}

i64 NcFile::tell() {
  if (m_fp == NULL) return 0;

  return ftell(m_fp);
}

i64 NcFile::length() {
  i64 pos = tell();
  seek(FileSeekOrigin::end, 0);
  i64 len = tell();
  seek(FileSeekOrigin::begin, pos);
  return len;
}

bool NcFile::seek(FileSeekOrigin origin, i64 offset) {
  if (m_fp == NULL) return false;

  int seek;
  switch (origin) {
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

int NcFile::read(void* bufferOut, int bytesToRead) {
  if (m_fp == NULL) return 0;
  int rtn = (int)fread(bufferOut, 1, bytesToRead, m_fp);
  return rtn;
}

int NcFile::write(const void* buffer, int bytesToWrite) {
  if (m_fp == NULL) return 0;
  int rtn = (int)fwrite(buffer, 1, bytesToWrite, m_fp);
  return rtn;
}
