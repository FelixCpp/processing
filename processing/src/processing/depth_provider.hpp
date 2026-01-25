#ifndef _PROCESSING_INCLUDE_DEPTH_PROVIDER_HPP_
#define _PROCESSING_INCLUDE_DEPTH_PROVIDER_HPP_

namespace processing
{
    class DepthProvider
    {
    public:
        explicit DepthProvider(float min, float max, float increment);

        float getMinDepth() const;
        float getMaxDepth() const;
        float getNextDepth();
        void reset();

    private:
        float m_min;
        float m_max;
        float m_increment;
        float m_current;
    };
} // namespace processing

#endif // _PROCESSING_INCLUDE_DEPTH_PROVIDER_HPP_
