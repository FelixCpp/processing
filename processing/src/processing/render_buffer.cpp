#include <processing/render_buffer.hpp>
#include <stdexcept>

namespace processing
{
    MainRenderbuffer::MainRenderbuffer(const rect2u& viewport)
        : m_viewport(viewport)
    {
    }

    void MainRenderbuffer::setViewport(const rect2u& viewport)
    {
        m_viewport = viewport;
    }

    const rect2u& MainRenderbuffer::getViewport() const
    {
        return m_viewport;
    }

    ResourceId MainRenderbuffer::getResourceId() const
    {
        return ResourceId{.value = 0};
    }
} // namespace processing

namespace processing
{
    Renderbuffer::Renderbuffer()
    {
    }

    Renderbuffer::Renderbuffer(const AssetId assetId, std::weak_ptr<RenderbufferImpl> impl)
        : m_assetId(assetId),
          m_impl(impl)
    {
    }

    AssetId Renderbuffer::getAssetId() const
    {
        return m_assetId;
    }

    ResourceId Renderbuffer::getResourceId() const
    {
        return m_impl.lock()->getResourceId();
    }

    const Texture& Renderbuffer::getTexture() const
    {
        return m_impl.lock()->getTexture();
    }

    const rect2u& Renderbuffer::getViewport() const
    {
        return m_impl.lock()->getViewport();
    }
} // namespace processing

namespace processing
{
    class RenderbufferAsset : public RenderbufferImpl
    {
    public:
        static std::unique_ptr<RenderbufferAsset> create(uint32_t width, uint32_t height, TextureAssetManager& textureAssetManager)
        {
            GLuint framebufferId = 0;
            glGenFramebuffers(1, &framebufferId);
            glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);

            Texture renderTexture = textureAssetManager.create(width, height, nullptr);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture.getResourceId().value, 0);

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
                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                return nullptr;
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            return std::unique_ptr<RenderbufferAsset>(new RenderbufferAsset(framebufferId, std::move(renderTexture), renderbufferId, uint2{width, height}));
        }

        ~RenderbufferAsset() override
        {
            glDeleteFramebuffers(1, &m_framebufferId);
            glDeleteRenderbuffers(1, &m_renderbufferId);
        }

        ResourceId getResourceId() const override
        {
            return ResourceId{.value = m_framebufferId};
        }

        const rect2u& getViewport() const override
        {
            return m_viewport;
        }

        const Texture& getTexture() const override
        {
            return m_renderTexture;
        }

    private:
        explicit RenderbufferAsset(GLuint framebufferId, Texture renderTexture, GLuint renderbufferId, uint2 size)
            : m_framebufferId(framebufferId),
              m_renderbufferId(renderbufferId),
              m_renderTexture(std::move(renderTexture)),
              m_viewport(0, 0, size.x, size.y)
        {
        }

        GLuint m_framebufferId;
        GLuint m_renderbufferId;
        Texture m_renderTexture;
        rect2u m_viewport;
    };
} // namespace processing

namespace processing
{
    RenderbufferManager::RenderbufferManager()
        : m_nextAssetId(1)
    {
    }

    Renderbuffer RenderbufferManager::create(const uint32_t width, const uint32_t height, TextureAssetManager& textureAssetManager)
    {
        if (auto renderbuffer = RenderbufferAsset::create(width, height, textureAssetManager))
        {
            const auto insertion = m_assets.insert(std::make_pair(m_nextAssetId++, std::move(renderbuffer)));
            return Renderbuffer(AssetId{.value = insertion.first->first}, insertion.first->second);
        }

        error("Failed to create offscreen render buffer resource");
        return Renderbuffer();
    }

    RenderbufferImpl& RenderbufferManager::getAsset(AssetId assetId)
    {
        if (const auto itr = m_assets.find(assetId.value); itr != m_assets.end())
        {
            return *itr->second;
        }

        throw std::runtime_error("Unknown AssetId");
    }
} // namespace processing
