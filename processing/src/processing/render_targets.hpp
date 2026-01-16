#ifndef _PROCESSING_INCLUDE_RENDER_TARGETS_HPP_
#define _PROCESSING_INCLUDE_RENDER_TARGETS_HPP_

#include <glad/gl.h>

#include <processing/processing.hpp>

namespace processing
{
    class MainRenderTarget : public RenderTarget
    {
    public:
        explicit MainRenderTarget(rect2u viewport);
        void setViewport(rect2u viewport);

        void activate() override;

    private:
        rect2u m_viewport;
    };
} // namespace processing

#endif // _PROCESSING_INCLUDE_RENDER_TARGETS_HPP_
