#pragma once

#include "nc_vector.h"
#include "nc_object.h"

class NcImageBase : public NcObject
{
  public:
    inline Size size() { return m_size; }
    inline int width() { return m_size.width; }
    inline int height() { return m_size.height; }
    inline Rect area() { return Rect{0, 0, m_size.width, m_size.height}; }
    inline int pixelCount() { return m_size.width * m_size.height; }

    inline NcVector2 origin() { return m_origin; }
    inline void setOrigin(NcVector2 origin) { m_origin = origin; }

    virtual bool saveAs(NcString* fileName) = 0;

  protected:
    NcImageBase() {}
    ~NcImageBase() {}

  protected:
    Size m_size = {0, 0};
    NcVector2 m_origin = {0, 0};
};

class NcImage : public NcImageBase
{
  public:
    static sp<NcImage> allocWithSize(Size size);
    static sp<NcImage> allocWithFileName(const char* fileName);

    Rgba8* mutablePixels() { return m_pixels; }
    const Rgba8* pixels() { return m_pixels; }

    void clear(Rgba8 color);

  protected:
    Rgba8* m_pixels;
};

class NcImageU8 : public NcImageBase
{
  public:
    static sp<NcImageU8> allocWithSize(Size size);
    static sp<NcImageU8> allocWithFileName(const char* fileName);
    static sp<NcImageU8> allocByCoping(NcImageU8* r);

    inline u8* mutablePixels() { return m_pixels; }
    inline const u8* pixels() { return m_pixels; }
    inline u8 pixelAt(int x, int y) { return m_pixels[y * m_size.width + x]; }

    void clear(u8 color);

  protected:
    u8* m_pixels;
};
