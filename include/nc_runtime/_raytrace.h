#pragma once

/**
 * @brief A 2D implementation of Bresenham's raytracing algorithm... applies an action at each step
 */
template <class ActionType>
inline void bresenham2D(ActionType at, int absDa, int absDb, int errorB, int offsetA, int offsetB, int offset)
{
    for (int i = 0; i < absDa; ++i)
    {
        at(offset);
        offset += offsetA;
        errorB += absDb;
        if (errorB >= absDa)
        {
            offset += offsetB;
            errorB -= absDa;
        }
    }
    at(offset);
}

static inline int sign(int x)
{
    return x > 0 ? 1 : -1;
}

/**
 *  @brief  Raytrace a line and apply some action at each step
 *  @param [in] at
 *      The action to take... a functor
 *  @param [in] x0
 *      The starting x coordinate
 *  @param [in] y0
 *      The starting y coordinate
 *  @param [in] x1
 *      The ending x coordinate
 *  @param [in] y1
 *      The ending y coordinate
 */
template <class ActionType>
inline void raytraceLine(ActionType at, int width, int x0, int y0, int x1, int y1)
{
    int dx = x1 - x0;
    int dy = y1 - y0;

    int absDx = std::abs(dx);
    int absDy = std::abs(dy);

    int offsetDx = sign(dx);
    int offsetDy = sign(dy) * width;

    int offset = y0 * width + x0;

    // if x is dominant
    if (absDx >= absDy)
    {
        int errorY = absDx / 2;
        bresenham2D(at, absDx, absDy, errorY, offsetDx, offsetDy, offset);
        return;
    }

    // otherwise y is dominant
    int errorX = absDy / 2;
    bresenham2D(at, absDy, absDx, errorX, offsetDy, offsetDx, offset);
}

template <class PixelType>
class Marker
{
public:
    Marker(PixelType* pixels, PixelType color) : m_pixels(pixels), m_color(color) {}
    inline void operator()(int offset) { m_pixels[offset] = m_color; }

private:
    PixelType* m_pixels;
    PixelType m_color;
};
