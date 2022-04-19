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
    CostmapInflatorImple(const CostmapInflatorParams& params, CustomInflationFunc func);
    ~CostmapInflatorImple();

    void setSeed(CostValue seed) { m_seed = seed; }

    sp<NcImageU8> inflate(NcImageU8* inputImg, Rect region);
    void inflateInplace(NcImageU8* img, Rect region);

    CostValue computeCost(float distanceInPixel);

private:
    void _init();
    void _cleanup();
    CostValue _computeCost(float distance);
    CustomInflationFunc m_customInflationFunc;

    std::map<float, std::vector<CellData>> m_inflationCells;

    CostValue m_seed;
    CostmapInflatorParams m_params;
    int m_cellInflationRadius;
    int m_cachedInflationRadius;

    size_t m_cacheLineWidth;
    CostValue* m_cachedCosts; // m_cacheLineWidth * m_cacheLineWidth 2D array
    float* m_cachedDistances; // m_cacheLineWidth * m_cacheLineWidth 2D array
};
