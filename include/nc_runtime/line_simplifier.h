#pragma once

#include <vector>
#include "vector2.h"

/** @brief 折线段抽稀器 */
class LineSimplifier
{
public:
    /**
     *  @brief 使用 Douglas-Peucker 算法抽稀指定的折线段
     *  @param [in, out] points
     *      指定折线段上的点，抽稀结果也保存在这里。
     *  @param [in] count
     *      指定折线段上的点的数目。
     *  @param [in] tolerance
     *      距离允许偏差。
     *  @param [in] maxResultCount
     *      抽稀结果中最多允许的点数。如不限制，可传入 INT_MAX。
     *  @returns
     *      抽稀结果中的点数。
     *      抽稀结果也存储在 @p points 中。
     *  @sa
     *      https://martinfleischmann.net/line-simplification-algorithms/
     */
    int douglasPeucker(nc::Vector2* points, int count, float tolerance, int maxResultCount);

private:
    int _compactBySelection(nc::Vector2* points, int count);

    std::vector<bool> m_selection;
};
