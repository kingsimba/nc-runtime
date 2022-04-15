#pragma once

namespace nc
{

struct ClusterPointExample
{
    float x, y, z;
    void* userData;
};

class ClulsterPointSpecExample
{
public:
    static float distance(const ClusterPointExample& l, const ClusterPointExample& r)
    {
        return sqrt((r.x - l.x) * (r.x - l.x) + (r.y - l.y) * (r.y - l.y) + (r.z - l.z) * (r.z - l.z));
    }
};

template <typename PointType, typename PointSpec>
class Dbscan
{
public:
    Dbscan(int minPts, float eps) : m_minPoints(minPts), m_epsilon(eps) {}
    ~Dbscan() {}

    void run(const std::vector<PointType>& points, std::vector<int>* clusterIdsOut)
    {
        m_points = points.data();
        m_pointCount = (int)points.size();

        int clusterId = 1;
        clusterIdsOut->clear();
        clusterIdsOut->resize(points.size(), UNCLASSIFIED);
        m_clusterIds = clusterIdsOut->data();

        for (int idx = 0; idx < m_pointCount; ++idx)
        {
            if (m_clusterIds[idx] == UNCLASSIFIED)
            {
                if (expandCluster(idx, clusterId))
                    clusterId += 1;
            }
        }
    }

private:
    bool expandCluster(int idx, int clusterId)
    {
        calculateCluster(m_points[idx], &m_clusterSeeds);
        if (m_clusterSeeds.size() < m_minPoints)
        {
            m_clusterIds[idx] = NOISE;
            return false;
        }

        size_t indexCorePoint = 0;
        for (size_t seedIdx = 0; seedIdx < m_clusterSeeds.size(); ++seedIdx)
        {
            int seed = m_clusterSeeds[seedIdx];
            m_clusterIds[seed] = clusterId;
            if (seed == idx)
            {
                indexCorePoint = seedIdx;
            }
        }
        m_clusterSeeds.erase(m_clusterSeeds.begin() + indexCorePoint);

        for (size_t seedIdx = 0; seedIdx < m_clusterSeeds.size(); ++seedIdx)
        {
            calculateCluster(m_points[m_clusterSeeds[seedIdx]], &m_clusterNeighors);

            if (m_clusterNeighors.size() >= m_minPoints)
            {
                for (size_t nbIdx = 0; nbIdx < m_clusterNeighors.size(); ++nbIdx)
                {
                    int& id = m_clusterIds[m_clusterNeighors[nbIdx]];

                    if (id == UNCLASSIFIED || id == NOISE)
                    {
                        if (id == UNCLASSIFIED)
                        {
                            m_clusterSeeds.push_back(m_clusterNeighors[nbIdx]);
                        }
                        id = clusterId;
                    }
                }
            }
        }

        return true;
    }

    void calculateCluster(const PointType& point, std::vector<int>* clusterIndicesOut)
    {
        clusterIndicesOut->clear();
        for (int index = 0; index < m_pointCount; ++index)
        {
            if (PointSpec::distance(point, m_points[index]) <= m_epsilon)
                clusterIndicesOut->push_back(index);
        }
    }

private:
    int m_minPoints;
    float m_epsilon;

    int m_pointCount = 0;
    const PointType* m_points = nullptr;
    int* m_clusterIds = nullptr;

    std::vector<int> m_clusterSeeds;
    std::vector<int> m_clusterNeighors;

    static constexpr int UNCLASSIFIED = -1;
    static constexpr int NOISE = -2;
};

} // namespace nc