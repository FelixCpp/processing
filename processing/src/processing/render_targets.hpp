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

    class OffscreenRenderTarget : public RenderTarget
    {
    public:
        static std::unique_ptr<OffscreenRenderTarget> create(uint32_t width, uint32_t height);

        ~OffscreenRenderTarget() override;

        void activate() override;
        uint2 getSize() const override;

        uint32_t getFramebufferId() const override;

    private:
        explicit OffscreenRenderTarget(
            GLuint framebufferId,
            GLuint renderbufferId,
            GLuint renderTextureId,
            uint2 size
        );

        GLuint m_framebufferId;
        GLuint m_renderbufferId;
        GLuint m_renderTextureId;
        uint2 m_size;
    };
} // namespace processing

#endif // _PROCESSING_INCLUDE_RENDER_TARGETS_HPP_
