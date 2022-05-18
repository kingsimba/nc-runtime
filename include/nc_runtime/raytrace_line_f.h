#pragma once

#include "nc_runtime/vector2.h"

/**
 *  @brief Raytrace a line and apply some action at each cell.
 *  @param [in] action
 *      The template parameter ActionCellType should define operator()(nc::Vector2i cell) to receive cells along the
 * line.
 *  @param [in] p0
 *      Start point of the line, in cell coordinate.
 *  @param [in] p1
 *      End point of the line, in cell coordinate.
 */
template <class ActionCellType>
forceinline void raytraceLineF(ActionCellType action, nc::Vector2 p0, nc::Vector2 p1)
{
    int sx = (int)floorf(p0.x);
    int sy = (int)floorf(p0.y);
    int ex = (int)floorf(p1.x);
    int ey = (int)floorf(p1.y);

    int n = abs(ex - sx) + abs(ey - sy) - 1;
    nc::Vector2i cell(sx, sy);
    action(cell);
    if (n > 0)
    {
        float rayX = p1.x - p0.x;
        float rayY = p1.y - p0.y;

        int stepX = p1.x > p0.x ? 1 : -1;
        int stepY = p1.y > p0.y ? 1 : -1;

        float tMaxX = rayX != 0 ? (sx + (p1.x > p0.x ? 1 : 0) - p0.x) / rayX : FLT_MAX;
        float tMaxY = rayY != 0 ? (sy + (p1.y > p0.y ? 1 : 0) - p0.y) / rayY : FLT_MAX;

        float tDeltaX = rayX != 0 ? stepX / rayX : FLT_MAX;
        float tDeltaY = rayY != 0 ? stepY / rayY : FLT_MAX;

        for (; n > 0; n--)
        {
            if (tMaxX < tMaxY)
            {
                tMaxX += tDeltaX;
                cell.x += stepX;
            }
            else
            {
                tMaxY += tDeltaY;
                cell.y += stepY;
            }
            action(cell);
        }
    }
    if (n == 0) // make sure the last pixel is (ex, ey)
        action({ex, ey});
}
