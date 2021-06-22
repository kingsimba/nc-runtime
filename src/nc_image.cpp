#include "stdafx_nc_runtime.h"
#include "nc_runtime/nc_image.h"
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include "nc_runtime/nc_string.h"

class NcImageImple : public NcImage
{
  public:
    bool initWithSize(Size size)
    {
        m_size = size;
        m_pixels = (Rgba8*)malloc((size_t)size.width * size.height * sizeof(Rgba8));
        return true;
    }

    bool initWithFileName(const char* fileName)
    {
        int w, h, channelsInFile;
        Rgba8* buffer = (Rgba8*)stbi_load(fileName, &w, &h, &channelsInFile, 4);
        if (buffer != NULL)
        {
            m_pixels = buffer;
            m_size = Size_make(w, h);
            return true;
        }

        return false;
    }

    ~NcImageImple() { free(m_pixels); }

    bool saveAs(NcString* fileName) override
    {
        int bytesWritten =
            stbi_write_png(fileName->cstr(), m_size.width, m_size.height, 4, m_pixels, sizeof(Rgba8) * m_size.width);
        return bytesWritten != 0;
    }
};

sp<NcImage> NcImage::allocWithSize(Size size)
{
    sp<NcImageImple> o = new NcImageImple();
    if (!o->initWithSize(size))
        o = NULL;
    return o;
}

sp<NcImage> NcImage::allocWithFileName(const char* fileName)
{
    sp<NcImageImple> o = new NcImageImple();
    if (!o->initWithFileName(fileName))
        o = NULL;
    return o;
}

void NcImage::clear(Rgba8 color)
{
    if (color.value == 0)
    {
        memset(m_pixels, 0, sizeof(color) * pixelCount());
    }
    else
    {
        Rgba8* p = m_pixels;
        Rgba8* pend = p + pixelCount();
        for (; p != pend; p++)
        {
            *p = color;
        }
    }
}

class AxImageU8Imple : public NcImageU8
{
  public:
    bool initWithSize(Size size)
    {
        m_size = size;
        m_pixels = (u8*)malloc((size_t)size.width * size.height * sizeof(u8));
        return true;
    }

    bool initWithFileName(const char* fileName)
    {
        int w, h, channelsInFile;
        u8* buffer = (u8*)stbi_load(fileName, &w, &h, &channelsInFile, 1);
        if (buffer != NULL)
        {
            m_pixels = buffer;
            m_size = Size_make(w, h);
            return true;
        }

        return false;
    }

    ~AxImageU8Imple() { free(m_pixels); }

    bool saveAs(NcString* fileName) override
    {
        int bytesWritten =
            stbi_write_png(fileName->cstr(), m_size.width, m_size.height, 1, m_pixels, sizeof(u8) * m_size.width);
        return bytesWritten != 0;
    }
};

sp<NcImageU8> NcImageU8::allocWithSize(Size size)
{
    sp<AxImageU8Imple> o = new AxImageU8Imple();
    if (!o->initWithSize(size))
        o.reset();
    return o;
}

sp<NcImageU8> NcImageU8::allocWithFileName(const char* fileName)
{
    sp<AxImageU8Imple> o = new AxImageU8Imple();
    if (!o->initWithFileName(fileName))
        o.reset();
    return o;
}

sp<NcImageU8> NcImageU8::allocByCoping(NcImageU8* r)
{
    sp<NcImageU8> o = NcImageU8::allocWithSize(r->size());
    o->setOrigin(r->origin());
    memcpy(o->mutablePixels(), r->pixels(), r->pixelCount());
    return o;
}

void NcImageU8::clear(u8 color)
{
    if (color == 0)
    {
        memset(m_pixels, 0, sizeof(color) * pixelCount());
    }
    else
    {
        u8* p = m_pixels;
        u8* pend = p + pixelCount();
        for (; p != pend; p++)
        {
            *p = color;
        }
    }
}
