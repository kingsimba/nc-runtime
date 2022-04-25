#pragma once

#include <vector>
#include "vector2.h"

class LineSimplifier
{
public:
    /**
     *  @brief Simplify specified polyline with Douglas-Peucker algorithm
     *  @param [in, out] points
     *      Points of the specified polyline. The simplification result is also stored here.
     *  @param [in] count
     *      Count of points in the specified polyline.
     *  @param [in] tolerance
     *      Distance tolerance.
     *  @param [in] maxResultCount
     *      Maximum count of points in the simplification result.
     *      Passing INT_MAX to ignore this parameter.
     *  @returns
     *      Count of points in the simplification result.
     *      The result points is stored in @p points.
     *  @note
     *      If @p maxResultCount conflicts @p tolerance, @p maxResultCount takes precedence.
     *  @sa
     *      https://martinfleischmann.net/line-simplification-algorithms/
     */
    int douglasPeucker(nc::Vector2* points, int count, float tolerance, int maxResultCount);

private:
    int _compactBySelection(nc::Vector2* points, int count);

    std::vector<bool> m_selection;
};
