#include "stdafx_nc_runtime.h"
#include "costmap_inflator_imple.h"
#include <math.h>

#define ARRAY_AT(arr, x, y) arr[m_cacheLineWidth * (y) + (x)]

CostmapInflatorImple::CostmapInflatorImple(const CostmapInflatorParams& params, CustomInflationFunc func)
{
    m_seed = CostValue::obstacle;
    m_params = params;
    m_customInflationFunc = func;
    init();
}

CostmapInflatorImple::~CostmapInflatorImple()
{
    cleanup();
}

sp<NcImageU8> CostmapInflatorImple::inflate(NcImageU8* inputImg, Rect region)
{
    // copy image from input
    sp<NcImageU8> outputImg = NcImageU8::allocByCoping(inputImg);

    inflateInplace(outputImg.get(), region);

    return outputImg;
}

void CostmapInflatorImple::inflateInplace(NcImageU8* img, Rect region)
{
    m_inflationCells.clear();

    Size size = img->size();

    // expand region according to inflation cells
    region.expand(m_cellInflationRadius);
    region.intersectWith(img->area());

    // Start with lethal obstacles: by definition distance is 0.0

    auto& obsBin = m_inflationCells[0.0];
    auto pixels = img->pixels();
    for (int y = region.top; y < region.bottom; y++)
    {
        for (int x = region.left; x < region.right; x++)
        {
            if (pixels[y * size.width + x] == (u8)m_seed)
            {
                obsBin.emplace_back(CellData{y * size.width + x, x, y, x, y});
            }
        }
    }

    // Process cells by increasing distance; new cells are appended to the corresponding distance bin, so they
    // can overtake previously inserted but farther away cells

    bool* seen = (bool*)calloc(size.width * size.height, 1);
    StandardFreer seenFreer(seen);

    float cachedDistance = -1.0f;
    std::vector<CellData>* cachedCells = NULL;
    auto enqueue = [&cachedDistance, &cachedCells, seen, this](const CellData& cell) {
        if (seen[cell.index])
        {
            return;
        }

        float distance = ARRAY_AT(m_cachedDistances, abs(cell.y - cell.srcY), abs(cell.x - cell.srcX));
        if (distance > m_cellInflationRadius)
        {
            return;
        }
        if (distance != cachedDistance)
        {
            cachedDistance = distance;
            cachedCells = &m_inflationCells[distance];
        }
        cachedCells->emplace_back(cell);
    };

    u8* outputPixels = img->mutablePixels();
    for (auto bin = m_inflationCells.begin(); bin != m_inflationCells.end(); ++bin)
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
            CostValue oldCost = (CostValue)pixels[cell.y * size.width + cell.x];
            if (m_customInflationFunc != nullptr
                || (oldCost == CostValue::noInfo && (cost >= CostValue::inflatedObstacle)))
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
}

void CostmapInflatorImple::init()
{
    m_cellInflationRadius = static_cast<int>(max(0.0f, m_params.inflationRadius / m_params.resolution));

    // computeCaches;
    m_cachedInflationRadius = m_cellInflationRadius;
    m_cacheLineWidth = m_cellInflationRadius + 2;

    m_cachedCosts = new CostValue[m_cacheLineWidth * m_cacheLineWidth];
    m_cachedDistances = new float[m_cacheLineWidth * m_cacheLineWidth];

    if (m_customInflationFunc == nullptr)
    {
        for (int y = 0; y <= m_cellInflationRadius + 1; ++y)
        {
            for (int x = 0; x <= m_cellInflationRadius + 1; ++x)
            {
                float distance = hypotf((float)x, (float)y);
                ARRAY_AT(m_cachedDistances, x, y) = distance;
                ARRAY_AT(m_cachedCosts, x, y) = computeCost(distance);
            }
        }
    }
    else
    {
        for (int y = 0; y <= m_cellInflationRadius + 1; ++y)
        {
            for (int x = 0; x <= m_cellInflationRadius + 1; ++x)
            {
                float distance = hypotf((float)x, (float)y);
                ARRAY_AT(m_cachedDistances, x, y) = distance;
                ARRAY_AT(m_cachedCosts, x, y) = (*m_customInflationFunc)(distance, m_params);
            }
        }
    }
}

void CostmapInflatorImple::cleanup()
{
    delete[] m_cachedDistances;
    m_cachedDistances = nullptr;
    delete[] m_cachedCosts;
    m_cachedCosts = nullptr;
}

CostValue CostmapInflatorImple::computeCost(float distance)
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
        float factor = expf(-1.0f * m_params.costScalingFactor * (euclideanDistance - m_params.inscribedRadius));
        cost = (CostValue)(((u8)CostValue::inflatedObstacle - 1) * factor);
    }
    return cost;
}

//////////////////////////////////////////////////////////////////////////

CostmapInflator::CostmapInflator(const CostmapInflatorParams& params, CustomInflationFunc func)
{
    m_imple = std::make_unique<CostmapInflatorImple>(params, func);
}

CostmapInflator::~CostmapInflator()
{
}

sp<NcImageU8> CostmapInflator::inflate(NcImageU8* inputImg, Rect region)
{
    return m_imple->inflate(inputImg, region);
}

void CostmapInflator::inflateInplace(NcImageU8* img, Rect region)
{
    m_imple->inflateInplace(img, region);
}

void CostmapInflator::setSeed(CostValue seed)
{
    m_imple->setSeed(seed);
}
