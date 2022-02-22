#pragma once

#include "vector2.h"
#include "nc_object.h"
#include "string_slice.h"

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

    virtual bool saveAs(const StringSlice& fileName) = 0;

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
    static sp<NcImage> allocWithFileName(const StringSlice& fileName);

    Rgba8* mutablePixels() { return m_pixels; }
    const Rgba8* pixels() { return m_pixels; }

    void clear(Rgba8 color);

    bool saveAs(const StringSlice& fileName) override;

protected:
    bool initWithSize(Size size);
    bool initWithFileName(const StringSlice& fileName);

protected:
    bool m_shouldFreePixels = false;
    Rgba8* m_pixels = NULL;
};

class NcImageU8 : public NcImageBase
// Grayscale image
{
public:
    NcImageU8() = default;
    ~NcImageU8();

    typedef u8 PixelType;

    static sp<NcImageU8> allocWithSize(Size size);
    static sp<NcImageU8> allocWithFileName(const StringSlice& fileName);
    static sp<NcImageU8> allocWithData(const void* data, int lengthInBytes);

    // The image doesn't own the memory. The user must keep it valid.
    static sp<NcImageU8> allocWithBytesNoCopy(u8* bytes, Size size);
    static sp<NcImageU8> allocByCoping(NcImageU8* r);

    inline u8* mutablePixels() { return m_pixels; }
    inline const u8* pixels() { return m_pixels; }
    inline u8 pixelAt(int x, int y) { return m_pixels[y * m_size.width + x]; }

    void clear(u8 color);

    bool saveAs(const StringSlice& fileName) override;

protected:
    bool initWithSize(Size size);
    bool initWithFileName(const StringSlice& fileName);
    bool initWithData(const void* data, int lengthInBytes);

protected:
    bool m_shouldFreePixels = false;
    u8* m_pixels = NULL;
};

// Grayscale image
class NcImageU16 : public NcImageBase
{
public:
    NcImageU16() = default;
    ~NcImageU16();

    static sp<NcImageU16> allocWithSize(Size size);
    static sp<NcImageU16> allocWithFileName(const char* fileName);

    // The image doesn't own the memory. The user must keep it valid.
    static sp<NcImageU16> allocWithBytesNoCopy(u16* bytes, Size size);
    static sp<NcImageU16> allocByCoping(NcImageU16* r);

    inline u16* mutablePixels() { return m_pixels; }
    inline const u16* pixels() { return m_pixels; }
    inline u16 pixelAt(int x, int y) { return m_pixels[y * m_size.width + x]; }

    void clear(u16 color);

    bool saveAs(const StringSlice& fileName) override;

    void normalize();

protected:
    bool initWithSize(Size size);
    bool initWithFileName(const char* fileName);

protected:
    bool m_shouldFreePixels = false;
    u16* m_pixels = NULL;
};
