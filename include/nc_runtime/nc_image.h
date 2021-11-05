#pragma once

#include "vector2.h"
#include "nc_object.h"

class NcImageBase : public NcObject
{
public:
    typedef nc::Vector2 Vector2;

    inline Size size() { return m_size; }
    inline int width() { return m_size.width; }
    inline int height() { return m_size.height; }
    inline Rect area() { return Rect{0, 0, m_size.width, m_size.height}; }
    inline int pixelCount() { return m_size.width * m_size.height; }

    inline Vector2 origin() { return m_origin; }
    inline void setOrigin(Vector2 origin) { m_origin = origin; }

    virtual bool saveAs(NcString* fileName) = 0;

protected:
    NcImageBase() = default;
    ~NcImageBase() = default;

protected:
    Size m_size = {0, 0};
    Vector2 m_origin = {0, 0};
};

// RGBA8 image
class NcImage : public NcImageBase
{
public:
    NcImage() = default;
    ~NcImage();

    static sp<NcImage> allocWithSize(Size size);
    static sp<NcImage> allocWithBytesNoCopy(Rgba8* bytes, Size size);
    static sp<NcImage> allocWithFileName(const char* fileName);

    Rgba8* mutablePixels() { return m_pixels; }
    const Rgba8* pixels() { return m_pixels; }

    void clear(Rgba8 color);

    bool saveAs(NcString* fileName) override;

protected:
    bool initWithSize(Size size);
    bool initWithFileName(const char* fileName);

protected:
    bool m_shouldFreePixels = false;
    Rgba8* m_pixels = NULL;
};

// Grayscale image
class NcImageU8 : public NcImageBase
{
public:
    NcImageU8() = default;
    ~NcImageU8();

    static sp<NcImageU8> allocWithSize(Size size);
    static sp<NcImageU8> allocWithFileName(const char* fileName);

    // The image doesn't own the memory. The user must keep it valid.
    static sp<NcImageU8> allocWithBytesNoCopy(u8* bytes, Size size);
    static sp<NcImageU8> allocByCoping(NcImageU8* r);

    inline u8* mutablePixels() { return m_pixels; }
    inline const u8* pixels() { return m_pixels; }
    inline u8 pixelAt(int x, int y) { return m_pixels[y * m_size.width + x]; }

    void clear(u8 color);

    bool saveAs(NcString* fileName) override;

protected:
    bool initWithSize(Size size);
    bool initWithFileName(const char* fileName);

protected:
    bool m_shouldFreePixels = false;
    u8* m_pixels = NULL;
};
