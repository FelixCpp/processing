#ifndef _PROCESSING_INCLUDE_RENDER_TARGETS_HPP_
#define _PROCESSING_INCLUDE_RENDER_TARGETS_HPP_

#include <glad/gl.h>

#include <processing/processing.hpp>

namespace processing
{
    class MainRenderTarget : public RenderTarget
    {
    public:
        explicit MainRenderTarget(uint2 size);
        void resize(uint2 size);

        void beginDraw() override;
        void endDraw() override;
        uint2 getSize() override;

    private:
        uint2 m_size;
    };

    class OffscreenRenderTarget : public RenderTarget
    {
    public:
        static std::unique_ptr<OffscreenRenderTarget> create(uint2 size);

        ~OffscreenRenderTarget() override;

        void beginDraw() override;
        void endDraw() override;
        uint2 getSize() override;

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
