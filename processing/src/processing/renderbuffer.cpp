#include <processing/renderbuffer.hpp>
#include <glad/gl.h>

namespace processing
{
    class OpenGLRenderbuffer : public PlatformRenderbuffer
    {
    public:
        static std::unique_ptr<OpenGLRenderbuffer> create(u32 width, u32 height, FilterMode filterMode, ExtendMode extendMode)
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
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, renderbufferId.value, renderbufferId.value);

            return std::unique_ptr<OpenGLRenderbuffer>(new OpenGLRenderbuffer(uint2{width, height}, renderbufferId, framebufferId, image));
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
    Renderbuffer RenderbufferAssetHandler::create(u32 width, u32 height, FilterMode filterMode, ExtendMode extendMode)
    {
        if (auto image = OpenGLRenderbuffer::create(width, height, filterMode, extendMode))
        {
            std::shared_ptr<PlatformRenderbuffer>& ptr = m_assets.emplace_back(std::move(image));
            AssetId assetId = {.value = m_assets.size()};

            return Renderbuffer(assetId, ptr);
        }

        return Renderbuffer(AssetId{.value = 0}, nullptr);
    }
} // namespace processing

namespace processing
{
    Renderbuffer::Renderbuffer(const AssetId assetId, std::shared_ptr<PlatformRenderbuffer> impl)
        : m_assetId{assetId},
          m_impl{std::move(impl)}
    {
    }

    Image& Renderbuffer::getImage()
    {
        return m_impl->getImage();
    }

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
