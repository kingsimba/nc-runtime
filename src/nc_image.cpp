#include "stdafx_nc_runtime.h"
#include "nc_runtime/nc_image.h"
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include "nc_runtime/nc_string.h"

bool NcImage::initWithSize(Size size)
{
    m_size = size;
    m_shouldFreePixels = true;
    m_pixels = (Rgba8*)malloc((size_t)size.width * size.height * sizeof(Rgba8));
    return true;
}

bool NcImage::initWithFileName(const char* fileName)
{
    int w, h, channelsInFile;
    Rgba8* buffer = (Rgba8*)stbi_load(fileName, &w, &h, &channelsInFile, 4);
    if (buffer != NULL)
    {
        m_shouldFreePixels = true;
        m_pixels = buffer;
        m_size = Size_make(w, h);
        return true;
    }

    return false;
}

NcImage::~NcImage()
{
    if (m_shouldFreePixels)
        free(m_pixels);
}

bool NcImage::saveAs(NcString* fileName)
{
    int bytesWritten =
        stbi_write_png(fileName->cstr(), m_size.width, m_size.height, 4, m_pixels, sizeof(Rgba8) * m_size.width);
    return bytesWritten != 0;
}

sp<NcImage> NcImage::allocWithSize(Size size)
{
    sp<NcImage> o = alloc<NcImage>();
    if (!o->initWithSize(size))
        o = NULL;
    return o;
}

sp<NcImage> NcImage::allocWithBytesNoCopy(Rgba8* bytes, Size size)
{
    sp<NcImage> o = alloc<NcImage>();
    o->m_size = size;
    o->m_pixels = bytes;
    return o;
}

sp<NcImage> NcImage::allocWithFileName(const char* fileName)
{
    sp<NcImage> o = alloc<NcImage>();
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

//////////////////////////////////////////////////////////////////////////

bool NcImageU8::initWithSize(Size size)
{
    m_size = size;
    m_shouldFreePixels = true;
    m_pixels = (u8*)malloc((size_t)size.width * size.height * sizeof(u8));
    return true;
}

bool NcImageU8::initWithFileName(const char* fileName)
{
    int w, h, channelsInFile;
    u8* buffer = (u8*)stbi_load(fileName, &w, &h, &channelsInFile, 1);
    if (buffer != NULL)
    {
        m_shouldFreePixels = true;
        m_pixels = buffer;
        m_size = Size_make(w, h);
        return true;
    }

    return false;
}

NcImageU8::~NcImageU8()
{
    if (m_shouldFreePixels)
        free(m_pixels);
}

bool NcImageU8::saveAs(NcString* fileName)
{
    int bytesWritten =
        stbi_write_png(fileName->cstr(), m_size.width, m_size.height, 1, m_pixels, sizeof(u8) * m_size.width);
    return bytesWritten != 0;
}

sp<NcImageU8> NcImageU8::allocWithSize(Size size)
{
    sp<NcImageU8> o = alloc<NcImageU8>();
    if (!o->initWithSize(size))
        o.reset();
    return o;
}

sp<NcImageU8> NcImageU8::allocWithBytesNoCopy(u8* bytes, Size size)
{
    sp<NcImageU8> o = alloc<NcImageU8>();
    o->m_size = size;
    o->m_pixels = bytes;
    return o;
}

sp<NcImageU8> NcImageU8::allocWithFileName(const char* fileName)
{
    sp<NcImageU8> o = alloc<NcImageU8>();
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

//////////////////////////////////////////////////////////////////////////

bool NcImageU16::initWithSize(Size size)
{
    m_size = size;
    m_shouldFreePixels = true;
    m_pixels = (u16*)malloc((size_t)size.width * size.height * sizeof(u16));
    return true;
}

bool NcImageU16::initWithFileName(const char* fileName)
{
    int w, h, channelsInFile;
    u16* buffer = (u16*)stbi_load(fileName, &w, &h, &channelsInFile, 1);
    if (buffer != NULL)
    {
        m_shouldFreePixels = true;
        m_pixels = buffer;
        m_size = Size_make(w, h);
        return true;
    }

    return false;
}

NcImageU16::~NcImageU16()
{
    if (m_shouldFreePixels)
        free(m_pixels);
}

bool NcImageU16::saveAs(NcString* fileName)
{
    int bytesWritten =
        stbi_write_png(fileName->cstr(), m_size.width, m_size.height, 1, m_pixels, sizeof(u16) * m_size.width);
    return bytesWritten != 0;
}

sp<NcImageU16> NcImageU16::allocWithSize(Size size)
{
    sp<NcImageU16> o = alloc<NcImageU16>();
    if (!o->initWithSize(size))
        o.reset();
    return o;
}

sp<NcImageU16> NcImageU16::allocWithBytesNoCopy(u16* bytes, Size size)
{
    sp<NcImageU16> o = alloc<NcImageU16>();
    o->m_size = size;
    o->m_pixels = bytes;
    return o;
}

sp<NcImageU16> NcImageU16::allocWithFileName(const char* fileName)
{
    sp<NcImageU16> o = alloc<NcImageU16>();
    if (!o->initWithFileName(fileName))
        o.reset();
    return o;
}

sp<NcImageU16> NcImageU16::allocByCoping(NcImageU16* r)
{
    sp<NcImageU16> o = NcImageU16::allocWithSize(r->size());
    o->setOrigin(r->origin());
    memcpy(o->mutablePixels(), r->pixels(), r->pixelCount());
    return o;
}

void NcImageU16::clear(u16 color)
{
    if (color == 0)
    {
        memset(m_pixels, 0, sizeof(color) * pixelCount());
    }
    else
    {
        u16* p = m_pixels;
        u16* pend = p + pixelCount();
        for (; p != pend; p++)
        {
            *p = color;
        }
    }
}
