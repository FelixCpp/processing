#include <processing/render_targets.hpp>

namespace processing
{
    MainRenderTarget::MainRenderTarget(const rect2u viewport) : m_viewport(viewport)
    {
    }

    void MainRenderTarget::setViewport(const rect2u viewport)
    {
        m_viewport = viewport;
    }

    void MainRenderTarget::activate()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(m_viewport.left, m_viewport.top, m_viewport.width, m_viewport.height);
    }
} // namespace processing

namespace processing
{
    std::unique_ptr<OffscreenRenderTarget> OffscreenRenderTarget::create(uint2 size)
    {
        GLuint textureId = 0;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

        GLuint renderbufferId = 0;
        glGenRenderbuffers(1, &renderbufferId);
        glBindRenderbuffer(GL_RENDERBUFFER, renderbufferId);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x, size.y);

        GLuint framebufferId = 0;
        glGenFramebuffers(1, &framebufferId);
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbufferId);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            error("Framebuffer is incomplete");
            return nullptr;
        }

        return std::unique_ptr<OffscreenRenderTarget>(new OffscreenRenderTarget(framebufferId, renderbufferId, textureId, size));
    }

    OffscreenRenderTarget::~OffscreenRenderTarget()
    {
        glDeleteTextures(1, &m_textureId);
        glDeleteRenderbuffers(1, &m_renderbufferId);
        glDeleteFramebuffers(1, &m_framebufferId);
    }

    void OffscreenRenderTarget::activate()
    {
        glViewport(0, 0, m_size.x, m_size.y);
        glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferId);
    }

    GLuint OffscreenRenderTarget::getTextureId()
    {
        return m_textureId;
    }

    OffscreenRenderTarget::OffscreenRenderTarget(GLuint framebufferId, GLuint renderbufferId, GLuint textureId, uint2 size)
        : m_framebufferId(framebufferId), m_renderbufferId(renderbufferId), m_textureId(textureId), m_size(size)
    {
    }
} // namespace processing
