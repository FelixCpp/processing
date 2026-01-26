#ifndef _PROCESSING_INCLUDE_RENDER_TARGETS_HPP_
#define _PROCESSING_INCLUDE_RENDER_TARGETS_HPP_

#include <processing/processing.hpp>

#include <glad/gl.h>

namespace processing
{
    class MainRenderTarget : public RenderTarget
    {
    public:
        explicit MainRenderTarget(uint32_t width, uint32_t height);

        void activate() override;

        void setSize(uint32_t width, uint32_t height);
        uint2 getSize() const override;
        uint32_t getFramebufferId() const override;

    private:
        uint2 m_viewportSize;
    };
} // namespace processing

#endif // _PROCESSING_INCLUDE_RENDER_TARGETS_HPP_
