#include <processing/render_targets.hpp>

namespace processing
{
    MainRenderTarget::MainRenderTarget(const uint32_t width, const uint32_t height)
        : m_viewportSize(width, height)
    {
    }

    void MainRenderTarget::setSize(const uint32_t width, const uint32_t height)
    {
        m_viewportSize = {width, height};
    }

    uint2 MainRenderTarget::getSize() const
    {
        return m_viewportSize;
    }

    uint32_t MainRenderTarget::getFramebufferId() const
    {
        return 0;
    }

    void MainRenderTarget::activate()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);
    }
} // namespace processing

namespace processing
{

    std::unique_ptr<OffscreenRenderTarget> OffscreenRenderTarget::create(uint32_t width, uint32_t height)
    {
        GLuint framebufferId = 0;
        glGenFramebuffers(1, &framebufferId);
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);

        GLuint renderTextureId = 0;
        glGenTextures(1, &renderTextureId);
        glBindTexture(GL_TEXTURE_2D, renderTextureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glBindTexture(GL_TEXTURE_2D, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTextureId, 0);

        GLuint renderbufferId = 0;
        glGenRenderbuffers(1, &renderbufferId);
        glBindRenderbuffer(GL_RENDERBUFFER, renderbufferId);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbufferId);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            error("Framebuffer is not yet complete.");
            glDeleteFramebuffers(1, &framebufferId);
            glDeleteRenderbuffers(1, &renderbufferId);
            glDeleteTextures(1, &renderTextureId);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            return nullptr;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return std::unique_ptr<OffscreenRenderTarget>(new OffscreenRenderTarget(framebufferId, renderTextureId, renderTextureId, uint2{width, height}));
    }

    OffscreenRenderTarget::~OffscreenRenderTarget()
    {
        glDeleteFramebuffers(1, &m_framebufferId);
        glDeleteRenderbuffers(1, &m_renderbufferId);
        glDeleteTextures(1, &m_renderTextureId);
    }

    void OffscreenRenderTarget::activate()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferId);
        glViewport(0, 0, m_size.x, m_size.y);
    }

    uint2 OffscreenRenderTarget::getSize() const
    {
        return m_size;
    }

    uint32_t OffscreenRenderTarget::getFramebufferId() const
    {
        return m_framebufferId;
    }

    OffscreenRenderTarget::OffscreenRenderTarget(const GLuint framebufferId, const GLuint renderbufferId, const GLuint renderTextureId, const uint2 size)
        : m_framebufferId(framebufferId),
          m_renderbufferId(renderbufferId),
          m_renderTextureId(renderTextureId),
          m_size(size)
    {
    }

} // namespace processing
