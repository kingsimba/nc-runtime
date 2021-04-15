#include "stdafx_nc_runtime.h"
#include "nc_data.h"
#include "nc_file.h"

sp<NcData> NcData::allocWithBytes(const void* bytes, size_t len) {
  size_t totalLen = sizeof(NcData) + len;
  NcData* o = (NcData*)NcObject::allocRawObjectWithSize(totalLen, false);
  ::new (o) NcData();
  memcpy((void*)(o + 1), bytes, len);
  o->m_bytes = o + 1;
  o->m_length = len;

  return o;
}

sp<NcData> NcData::allocByTakingBytes(void* bytes, size_t len) {
  NcData* o = new NcData();
  o->m_bytes = bytes;
  o->m_length = len;
  o->m_shouldFree = false;
  return o;
}

sp<NcData> NcData::allocWithContentsOfFile(const StringSlice& fileName) {
  auto file = NcFile::alloc(fileName, FileOpenFlag::read);
  if (file == nullptr) {
    return nullptr;
  }

  i64 len = file->length();
  void* buffer = malloc(len);
  if (file->read(buffer, len) != len) {
    free(buffer);
    return nullptr;
  }

  return allocByTakingBytes(buffer, len);
}
