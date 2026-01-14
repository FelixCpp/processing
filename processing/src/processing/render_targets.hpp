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

    class OffscreenRenderTarget : public RenderTarget
    {
    public:
        static std::unique_ptr<OffscreenRenderTarget> create(uint2 size);

        ~OffscreenRenderTarget() override;

        void activate() override;

        GLuint getTextureId();

    private:
        explicit OffscreenRenderTarget(GLuint framebufferId, GLuint renderbufferId, GLuint textureId, uint2 size);

        GLuint m_framebufferId;
        GLuint m_renderbufferId;
        GLuint m_textureId;

        uint2 m_size;
    };
} // namespace processing

#endif // _PROCESSING_INCLUDE_RENDER_TARGETS_HPP_
