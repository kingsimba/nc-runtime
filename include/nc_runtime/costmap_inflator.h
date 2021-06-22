#pragma once

#include "nc_image.h"

enum class CostValue : u8
{
    freeSpace = 0,
    noInfo = 255,
    obstacle = 254,        // The center of the robot shall never cross
    inflatedObstacle = 253 // Inflated with the inscribed radius of the robot.
                           // The robot may cross at some angles.
};

// some params can be obtained from layered_costmap_, inflation_layer.h
struct CostmapInflatorParams
{
    float resolution = 0;
    float inflationRadius = 2;
    float inscribedRadius = 0;
    float costScalingFactor = 2;
};

class CostmapInflator
{
  public:
    CostmapInflator(const CostmapInflatorParams& params);
    ~CostmapInflator();

    sp<NcImageU8> inflate(NcImageU8* inputImg, Rect region);

  private:
    void init();
    CostValue computeCost(float distance);

    CostmapInflatorParams m_params;
    int m_cellInflationRadius;
    int m_cachedInflationRadius;

    size_t m_cacheLineWidth;
    const static size_t MAX_CACHE_SIZE = 32 * 32;
    CostValue m_cachedCosts[MAX_CACHE_SIZE]; // m_cacheLineWidth * m_cacheLineWidth 2D array
    float m_cachedDistances[MAX_CACHE_SIZE]; // m_cacheLineWidth * m_cacheLineWidth 2D array
};
