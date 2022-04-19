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

typedef CostValue (*CustomInflationFunc)(float distance, const CostmapInflatorParams& params);

class CostmapInflatorImple;

class CostmapInflator : public NcObject
{
public:
    CostmapInflator(const CostmapInflatorParams& params, CustomInflationFunc func = nullptr);
    ~CostmapInflator();

    sp<NcImageU8> inflate(NcImageU8* inputImg, Rect region);

    void inflateInplace(NcImageU8* img, Rect region);

    /** Default value:: CostValue::obstacle */
    void setSeed(CostValue seed);

    CostValue computeCost(float distanceInPixel);

private:
    std::unique_ptr<CostmapInflatorImple> m_imple;
};
