#include <processing/render_buffer.hpp>
#include <stdexcept>

namespace processing
{
    RenderBuffer::RenderBuffer()
    {
    }

    RenderBuffer::RenderBuffer(const AssetId assetId, std::weak_ptr<RenderBufferImpl> impl)
        : m_assetId(assetId),
          m_impl(impl)
    {
    }

    AssetId RenderBuffer::getAssetId() const
    {
        return m_assetId;
    }

    ResourceId RenderBuffer::getResourceId() const
    {
        return m_impl.lock()->getResourceId();
    }

    const Texture& RenderBuffer::getTexture() const
    {
        return m_impl.lock()->getTexture();
    }

    void RenderBuffer::activate()
    {
        return m_impl.lock()->activate();
    }

    uint2 RenderBuffer::getSize() const
    {
        return m_impl.lock()->getSize();
    }
} // namespace processing

namespace processing
{
    class RenderBufferAsset : public RenderBufferImpl
    {
    public:
        static std::unique_ptr<RenderBufferAsset> create(uint32_t width, uint32_t height, TextureAssetManager& textureAssetManager)
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

            return std::unique_ptr<RenderBufferAsset>(new RenderBufferAsset(framebufferId, std::move(renderTexture), renderbufferId, uint2{width, height}));
        }

        ResourceId getResourceId() const override
        {
            return ResourceId{.value = m_framebufferId};
        }

        const Texture& getTexture() const override
        {
            return m_renderTexture;
        }

        uint2 getSize() const override
        {
            return m_size;
        }

        void activate() override
        {
            glViewport(0, 0, m_size.x, m_size.y);
            glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferId);
        }

    private:
        explicit RenderBufferAsset(GLuint framebufferId, Texture renderTexture, GLuint renderbufferId, uint2 size)
            : m_framebufferId(framebufferId),
              m_renderbufferId(renderbufferId),
              m_renderTexture(std::move(renderTexture)),
              m_size(size)
        {
        }

        GLuint m_framebufferId;
        GLuint m_renderbufferId;
        Texture m_renderTexture;
        uint2 m_size;
    };
} // namespace processing

namespace processing
{
    RenderTargetManager::RenderTargetManager()
        : m_nextAssetId(1)
    {
    }

    RenderBuffer RenderTargetManager::create(const uint32_t width, const uint32_t height, TextureAssetManager& textureAssetManager)
    {
        if (auto renderbuffer = RenderBufferAsset::create(width, height, textureAssetManager))
        {
            const auto insertion = m_assets.insert(std::make_pair(m_nextAssetId++, std::move(renderbuffer)));
            return RenderBuffer(AssetId{.value = insertion.first->first}, insertion.first->second);
        }

        error("Failed to create offscreen render buffer resource");
        return RenderBuffer();
    }

    RenderBufferImpl& RenderTargetManager::getAsset(AssetId assetId)
    {
        if (const auto itr = m_assets.find(assetId.value); itr != m_assets.end())
        {
            return *itr->second;
        }

        throw std::runtime_error("Unknown AssetId");
    }
} // namespace processing
