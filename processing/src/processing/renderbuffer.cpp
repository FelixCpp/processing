#include <processing/renderbuffer.hpp>
#include <glad/gl.h>

namespace processing
{
    MainRenderbuffer::MainRenderbuffer(const u32 width, const u32 height)
        : m_size{width, height}
    {
    }

    void MainRenderbuffer::resize(const u32 width, const u32 height)
    {
        m_size = {width, height};
    }

    uint2 MainRenderbuffer::getSize() const
    {
        return m_size;
    }

    ResourceId MainRenderbuffer::getResourceId() const
    {
        return ResourceId{.value = 0};
    }
} // namespace processing

namespace processing
{
    class OpenGLRenderbuffer : public PlatformRenderbuffer
    {
    public:
        static std::unique_ptr<OpenGLRenderbuffer> create(u32 width, u32 height)
        {
            ResourceId framebufferId = {.value = 0};
            glGenFramebuffers(1, &framebufferId.value);
            glBindFramebuffer(GL_FRAMEBUFFER, framebufferId.value);

            Image image = createImage(width, height, FilterMode::linear, ExtendMode::clamp);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, image.getResourceId().value, 0);

            ResourceId renderbufferId = {.value = 0};
            glGenRenderbuffers(1, &renderbufferId.value);
            glBindRenderbuffer(GL_RENDERBUFFER, renderbufferId.value);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, renderbufferId.value, renderbufferId.value);

            return std::unique_ptr<OpenGLRenderbuffer>(new OpenGLRenderbuffer(uint2{width, height}, renderbufferId, framebufferId, image));
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
        explicit OpenGLRenderbuffer(const uint2 size, ResourceId renderbufferId, ResourceId framebufferId, Image image)
            : m_size(size),
              m_renderbufferId(renderbufferId),
              m_framebufferId(framebufferId),
              m_image(image)
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
    uint2 Renderbuffer::getSize() const
    {
        return m_impl->getSize();
    }

    ResourceId Renderbuffer::getResourceId() const
    {
        return m_impl->getResourceId();
    }

    AssetId Renderbuffer::getAssetId() const
    {
        return m_assetId;
    }
} // namespace processing
