#pragma once

#include "nc_runtime/costmap_inflator.h"
#include <map>

class CostmapInflatorImple
{
    struct CellData
    {
        // // The index of the cell in the cost map
        int index;
        // The x coordinate of the cell in the cost map
        int x;
        // The y coordinate of the cell in the cost map
        int y;
        // The x coordinate of the closest obstacle cell in the costmap
        int srcX;
        // The y coordinate of the closest obstacle cell in the costmap
        int srcY;
    };

  public:
    CostmapInflatorImple(const CostmapInflatorParams& params);
    ~CostmapInflatorImple();

    sp<NcImageU8> inflate(NcImageU8* inputImg, Rect region);

  private:
    void init();
    CostValue computeCost(float distance);

    std::map<float, std::vector<CellData>> m_inflationCells;

    CostmapInflatorParams m_params;
    int m_cellInflationRadius;
    int m_cachedInflationRadius;

    size_t m_cacheLineWidth;
    const static size_t MAX_CACHE_SIZE = 32 * 32;
    CostValue m_cachedCosts[MAX_CACHE_SIZE]; // m_cacheLineWidth * m_cacheLineWidth 2D array
    float m_cachedDistances[MAX_CACHE_SIZE]; // m_cacheLineWidth * m_cacheLineWidth 2D array
};
