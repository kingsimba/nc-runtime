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
 * @brief  Raytrace a line and apply some action at each step
 * @param  at The action to take... a functor
 * @param  x0 The starting x coordinate
 * @param  y0 The starting y coordinate
 * @param  x1 The ending x coordinate
 * @param  y1 The ending y coordinate
 * @param  maxLength The maximum desired length of the segment... allows you to not go all the way to the
 * endpoint
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
