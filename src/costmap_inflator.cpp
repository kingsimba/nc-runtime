#include "stdafx_nc_runtime.h"
#include "nc_runtime/costmap_inflator.h"
#include <map>
#include <math.h>

#define ARRAY_AT(arr, x, y) arr[m_cacheLineWidth * (y) + (x)]

namespace
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

} // namespace

CostmapInflator::CostmapInflator(const CostmapInflatorParams& params)
{
    m_params = params;
    init();
}

CostmapInflator::~CostmapInflator()
{
}

sp<NcImageU8> CostmapInflator::inflate(NcImageU8* inputImg, Rect region)
{
    Size size = inputImg->size();

    // copy image from input
    sp<NcImageU8> outputImg = NcImageU8::allocByCoping(inputImg);

    // expand region according to inflation cells
    region.expand(m_cellInflationRadius);
    region.intersectWith(inputImg->area());

    // Start with lethal obstacles: by definition distance is 0.0
    std::map<float, std::vector<CellData>> inflationCells;
    auto& obsBin = inflationCells[0.0];
    for (int y = region.top; y < region.bottom; y++)
    {
        for (int x = region.left; x < region.right; x++)
        {
            if (outputImg->pixelAt(x, y) == (u8)CostValue::obstacle)
            {
                obsBin.push_back(CellData{y * size.width + x, x, y, x, y});
            }
        }
    }

    // Process cells by increasing distance; new cells are appended to the corresponding distance bin, so they
    // can overtake previously inserted but farther away cells

    bool* seen = (bool*)calloc(size.width * size.height, 1);
    StandardFreer seenFreer(seen);

    auto enqueue = [&inflationCells, seen, size, this](const CellData& cell) {
        if (seen[cell.index])
        {
            return;
        }

        float distance = ARRAY_AT(m_cachedDistances, abs(cell.y - cell.srcY), abs(cell.x - cell.srcX));
        if (distance > m_cellInflationRadius)
        {
            return;
        }
        inflationCells[distance].push_back(cell);
    };

    u8* outputPixels = outputImg->mutablePixels();
    for (auto bin = inflationCells.begin(); bin != inflationCells.end(); ++bin)
    {
        for (const auto& cell : bin->second)
        {
            // ignore if already visited
            if (seen[cell.index])
            {
                continue;
            }
            seen[cell.index] = true;
            // assign the cost associated with the distance from an obstacle to the cell
            int dx = abs(cell.x - cell.srcX);
            int dy = abs(cell.y - cell.srcY);
            CostValue cost = ARRAY_AT(m_cachedCosts, dx, dy);
            CostValue oldCost = (CostValue)outputImg->pixelAt(cell.x, cell.y);
            if (oldCost == CostValue::noInfo && (cost >= CostValue::inflatedObstacle))
                outputPixels[cell.index] = (u8)cost;
            else
                outputPixels[cell.index] = (u8)std::max(oldCost, cost);

            // attempt to put the neighbors of the current cell onto the inflation list
            if (cell.x > 0)
                enqueue(CellData{cell.index - 1, cell.x - 1, cell.y, cell.srcX, cell.srcY});
            if (cell.y > 0)
                enqueue(CellData{cell.index - size.width, cell.x, cell.y - 1, cell.srcX, cell.srcY});
            if (cell.x < size.width - 1)
                enqueue(CellData{cell.index + 1, cell.x + 1, cell.y, cell.srcX, cell.srcY});
            if (cell.y < size.height - 1)
                enqueue(CellData{cell.index + size.width, cell.x, cell.y + 1, cell.srcX, cell.srcY});
        }
    }

    return outputImg;
}

void CostmapInflator::init()
{
    m_cellInflationRadius = static_cast<int>(max(0.0f, m_params.inflationRadius / m_params.resolution));

    // computeCaches;
    m_cachedInflationRadius = m_cellInflationRadius;
    m_cacheLineWidth = m_cellInflationRadius + 2;
    NC_ASSERT(m_cacheLineWidth * m_cacheLineWidth <= MAX_CACHE_SIZE);

    for (unsigned int y = 0; y <= m_cellInflationRadius + 1; ++y)
    {
        for (unsigned int x = 0; x <= m_cellInflationRadius + 1; ++x)
        {
            float distance = hypot(x, y);
            ARRAY_AT(m_cachedDistances, x, y) = distance;
            ARRAY_AT(m_cachedCosts, x, y) = computeCost(distance);
        }
    }
}

CostValue CostmapInflator::computeCost(float distance)
{
    CostValue cost = CostValue::freeSpace;
    if (distance == 0)
        cost = CostValue::obstacle;
    else if (distance * m_params.resolution <= m_params.inscribedRadius)
        cost = CostValue::inflatedObstacle;
    else
    {
        // make sure cost falls off by Euclidean distance
        float euclideanDistance = distance * m_params.resolution;
        float factor = exp(-1.0 * m_params.costScalingFactor * (euclideanDistance - m_params.inscribedRadius));
        cost = (CostValue)(((u8)CostValue::inflatedObstacle - 1) * factor);
    }
    return cost;
}
