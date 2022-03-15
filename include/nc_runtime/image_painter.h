#pragma once

#include "_raytrace.h"

template <typename ImageType>
class ImagePainter
{
public:
    ImagePainter(ImageType* image) : m_image(image) {}

    typedef typename ImageType::PixelType PixelType;

    void line(nc::Vector2i from, nc::Vector2i to, PixelType color)
    {
        Marker<PixelType> m(m_image->mutablePixels(), color);
        raytraceLine(m, m_image->width(), from.x, from.y, to.x, to.y);
    }

private:
    ImageType* m_image;
};
