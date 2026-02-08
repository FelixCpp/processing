#include <processing/framebuffer.hpp>

#include <glad/gl.h>

namespace processing
{
    class OpenGLFramebuffer : public PlatformFramebuffer
    {
    public:
        static std::unique_ptr<OpenGLFramebuffer> create(u32 width, u32 height, FilterMode filterMode, ExtendMode extendMode)
        {
            ResourceId framebufferId = {.value = 0};
            glGenFramebuffers(1, &framebufferId.value);
            glBindFramebuffer(GL_FRAMEBUFFER, framebufferId.value);

            Image image = createImage(width, height, nullptr, filterMode, extendMode);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, image.getResourceId().value, 0);

            ResourceId renderbufferId = {.value = 0};
            glGenRenderbuffers(1, &renderbufferId.value);
            glBindRenderbuffer(GL_RENDERBUFFER, renderbufferId.value);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbufferId.value);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            return std::unique_ptr<OpenGLFramebuffer>(new OpenGLFramebuffer(uint2{width, height}, renderbufferId, framebufferId, image));
        }

        ~OpenGLFramebuffer() override
        {
            glDeleteFramebuffers(1, &m_framebufferId.value);
            glDeleteRenderbuffers(1, &m_renderbufferId.value);
        }

        Image& getImage() override
        {
            return m_image;
        }

        uint2 getSize() const override
        {
            return m_size;
        }

        ResourceId getResourceId() const override
        {
            return m_framebufferId;
        }

    private:
        explicit OpenGLFramebuffer(const uint2 size, ResourceId renderbufferId, ResourceId framebufferId, Image image)
            : m_size(size),
              m_renderbufferId(renderbufferId),
              m_framebufferId(framebufferId),
              m_image(std::move(image))
        {
        }

        uint2 m_size;
        ResourceId m_renderbufferId;
        ResourceId m_framebufferId;
        Image m_image;
    };
} // namespace processing

namespace processing
{
    Framebuffer::Framebuffer(std::shared_ptr<PlatformFramebuffer> impl)
        : m_impl{std::move(impl)}
    {
    }

    Image& Framebuffer::getImage()
    {
        return m_impl->getImage();
    }

    uint2 Framebuffer::getSize() const
    {
        return m_impl->getSize();
    }

    ResourceId Framebuffer::getResourceId() const
    {
        return m_impl->getResourceId();
    }
} // namespace processing

namespace processing
{
    Framebuffer createFramebuffer(const u32 width, const u32 height, const FilterMode filterMode, const ExtendMode extendMode)
    {
        return Framebuffer{OpenGLFramebuffer::create(width, height, filterMode, extendMode)};
    }
} // namespace processing
