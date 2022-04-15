#pragma once

namespace nc
{

namespace ClusterId
{
enum
{
    unclassified = -1,
    noise = -2
};
} // namespace ClusterId

/**

    @brief DBSCAN clustering algorithm
    @sa
        https://github.com/james-yoo/DBSCAN
        https://en.wikipedia.org/wiki/DBSCAN

    @details

    The template parameter PointSpec should define:
    * a type: `PointType`
    * a static member function:
        static float distance(const PointType& l, const PointType& r);

    Example:
    @code
struct ClusterPoint
{
    float x, y, z;
    void* userData;
};

class ClusterPointSpec
{
public:
    typedef ClusterPoint PointType;

    static float distance(const PointType& l, const PointType& r)
    {
        return sqrt((r.x - l.x) * (r.x - l.x) + (r.y - l.y) * (r.y - l.y) + (r.z - l.z) * (r.z - l.z));
    }
};
    @endcode
 */
template <typename PointSpec>
class Dbscan
{
public:
    typedef typename PointSpec::PointType PointType;

    /**
     *  @param [in] minPts
     *  @param [in] eps
     */
    Dbscan(int minPts, float eps) : m_minPoints(minPts), m_epsilon(eps) {}
    ~Dbscan() {}

    /**
     *  @param [in] points
     *      The point data to cluster.
     *  @param [out] clusterIdsOut
     *      Cluster ID for each point in @p points
     *      Positive values are valid.
     *      May contain negative values: ClusterId::noise(-2).
     */
    void run(const std::vector<PointType>& points, std::vector<int>* clusterIdsOut)
    {
        m_points = points.data();
        m_pointCount = (int)points.size();

        int clusterId = 1;
        clusterIdsOut->clear();
        clusterIdsOut->resize(points.size(), ClusterId::unclassified);
        m_clusterIds = clusterIdsOut->data();

        for (int idx = 0; idx < m_pointCount; ++idx)
        {
            if (m_clusterIds[idx] == ClusterId::unclassified)
            {
                if (_expandCluster(idx, clusterId))
                    clusterId += 1;
            }
        }
    }

private:
    bool _expandCluster(int idx, int clusterId)
    {
        _calculateCluster(m_points[idx], &m_clusterSeeds);
        if ((int)m_clusterSeeds.size() < m_minPoints)
        {
            m_clusterIds[idx] = ClusterId::noise;
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
            _calculateCluster(m_points[m_clusterSeeds[seedIdx]], &m_clusterNeighors);

            if ((int)m_clusterNeighors.size() >= m_minPoints)
            {
                for (size_t nbIdx = 0; nbIdx < m_clusterNeighors.size(); ++nbIdx)
                {
                    int& id = m_clusterIds[m_clusterNeighors[nbIdx]];

                    if (id == ClusterId::unclassified || id == ClusterId::noise)
                    {
                        if (id == ClusterId::unclassified)
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

    void _calculateCluster(const PointType& point, std::vector<int>* clusterIndicesOut)
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
};

} // namespace nc