#include <processing/depth_provider.hpp>
#include <cmath>

namespace processing
{

    DepthProvider::DepthProvider(const float minDepth, const float maxDepth, const float increment)
        : m_min(minDepth),
          m_max(maxDepth),
          m_increment(increment),
          m_current(minDepth)
    {
    }

    float DepthProvider::getMinDepth() const
    {
        return m_min;
    }

    float DepthProvider::getMaxDepth() const
    {
        return m_max;
    }

    float DepthProvider::getNextDepth()
    {
        float depth = m_current;
        m_current = fminf(m_current + m_increment, m_max);
        return depth;
    }

    void DepthProvider::reset()
    {
        m_current = m_min;
    }

} // namespace processing
