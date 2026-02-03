#include <processing/texture.hpp>

#include <glad/gl.h>
#include <stb/stb_image.h>

#include <format>
#include <stdexcept>

namespace processing
{
    PixelBuffer::PixelBuffer(const uint32_t width, const uint32_t height, uint8_t* data)
        : m_size{width, height},
          m_data({data, width * height * 4})
    {
    }

    void PixelBuffer::set(const uint32_t x, const uint32_t y, const color_t color)
    {
        const size_t n = (y * m_size.x + x) * 4;

#ifndef NDEBUG
        if (n >= m_data.size())
        {
            return;
        }
#endif

        m_data[n + 0] = red(color);
        m_data[n + 1] = green(color);
        m_data[n + 2] = blue(color);
        m_data[n + 3] = alpha(color);
    }

    color_t PixelBuffer::get(const uint32_t x, const uint32_t y) const
    {
        const size_t n = (y * m_size.x + x) * 4;

#ifndef NDEBUG
        if (n >= m_data.size())
        {
            return color(0);
        }
#endif

        return color(m_data[n + 0], m_data[n + 1], m_data[n + 2], m_data[n + 3]);
    }

    const uint2& PixelBuffer::getSize() const
    {
        return m_size;
    }
} // namespace processing

namespace processing
{
    Texture::Texture()
    {
    }

    Texture::Texture(AssetId assetId, std::weak_ptr<TextureImpl> impl)
        : m_assetId(assetId),
          m_impl(impl)
    {
    }

    void Texture::setFilterMode(FilterMode mode)
    {
        m_impl.lock()->setFilterMode(mode);
    }

    FilterMode Texture::getFilterMode() const
    {
        return m_impl.lock()->getFilterMode();
    }

    void Texture::setExtendMode(ExtendMode mode)
    {
        m_impl.lock()->setExtendMode(mode);
    }

    ExtendMode Texture::getExtendMode() const
    {
        return m_impl.lock()->getExtendMode();
    }

    void Texture::modifyPixels(const std::function<void(PixelBuffer&)>& callback)
    {
        m_impl.lock()->modifyPixels(callback);
    }

    void Texture::readPixels(const std::function<void(const PixelBuffer&)>& callback)
    {
        m_impl.lock()->readPixels(callback);
    }

    uint2 Texture::getSize() const
    {
        return m_impl.lock()->getSize();
    }

    ResourceId Texture::getResourceId() const
    {
        return m_impl.lock()->getResourceId();
    }

    AssetId Texture::getAssetId() const
    {
        return m_assetId;
    }
} // namespace processing

namespace processing
{
    class TextureAsset : public TextureImpl
    {
    public:
        static std::unique_ptr<TextureAsset> create(const uint32_t width, const uint32_t height, const uint8_t* data)
        {
            uint32_t id;
            glGenTextures(1, &id);
            glBindTexture(GL_TEXTURE_2D, id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glBindTexture(GL_TEXTURE_2D, 0);

            return std::unique_ptr<TextureAsset>(new TextureAsset(id, uint2{width, height}, FilterMode::linear, ExtendMode::clamp));
        }

        ~TextureAsset() override
        {
            glDeleteTextures(1, &m_resourceId);
        }

        void setExtendMode(const ExtendMode mode) override
        {
            if (m_extendMode != mode)
            {
                const auto convert = [](ExtendModeType type)
                {
                    switch (type)
                    {
                        case ExtendModeType::clamp:
                            return GL_CLAMP_TO_EDGE;
                        case ExtendModeType::repeat:
                            return GL_REPEAT;
                        case ExtendModeType::mirror:
                            return GL_MIRRORED_REPEAT;
                    }
                };

                glBindTexture(GL_TEXTURE_2D, m_resourceId);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, convert(mode.s));
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, convert(mode.t));
                m_extendMode = mode;
            }
        }

        ExtendMode getExtendMode() const override
        {
            return m_extendMode;
        }

        void setFilterMode(const FilterMode mode) override
        {
            if (m_filterMode != mode)
            {
                const auto convert = [](FilterModeType type)
                {
                    switch (type)
                    {
                        case FilterModeType::nearest:
                            return GL_NEAREST;
                        case FilterModeType::linear:
                            return GL_LINEAR;
                    }
                };

                glBindTexture(GL_TEXTURE_2D, m_resourceId);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, convert(mode.mag));
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, convert(mode.min));
                m_filterMode = mode;
            }
        }

        FilterMode getFilterMode() const override
        {
            return m_filterMode;
        }

        ResourceId getResourceId() const override
        {
            return ResourceId{.value = m_resourceId};
        }

        uint2 getSize() const override
        {
            return m_size;
        }

        std::unique_ptr<TextureImpl> copy(uint32_t left, uint32_t top, uint32_t width, uint32_t height) override
        {
            GLuint newTex = 0;
            glGenTextures(1, &newTex);
            glBindTexture(GL_TEXTURE_2D, newTex);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_filterMode.min == FilterModeType::linear ? GL_LINEAR : GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_filterMode.mag == FilterModeType::linear ? GL_LINEAR : GL_NEAREST);

            const auto convertExtend = [](ExtendModeType type)
            {
                switch (type)
                {
                    case ExtendModeType::clamp:
                        return GL_CLAMP_TO_EDGE;
                    case ExtendModeType::repeat:
                        return GL_REPEAT;
                    case ExtendModeType::mirror:
                        return GL_MIRRORED_REPEAT;
                }
                return GL_CLAMP_TO_EDGE;
            };

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, convertExtend(m_extendMode.s));
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, convertExtend(m_extendMode.t));
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

            GLuint fbo = 0;
            glGenFramebuffers(1, &fbo);
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_resourceId, 0);
            glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, left, m_size.y - top - height, width, height);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDeleteFramebuffers(1, &fbo);
            glBindTexture(GL_TEXTURE_2D, 0);

            return std::unique_ptr<TextureImpl>(new TextureAsset(newTex, uint2{width, height}, m_filterMode, m_extendMode));
        }

        void modifyPixels(const std::function<void(PixelBuffer&)>& callback) override
        {
            const auto data = std::make_unique<uint8_t[]>(m_size.x * m_size.y * 4);

            glBindTexture(GL_TEXTURE_2D, m_resourceId);
            glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.get());

            auto buffer = PixelBuffer{m_size.x, m_size.y, data.get()};
            callback(buffer);

            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_size.x, m_size.y, GL_RGBA, GL_UNSIGNED_BYTE, data.get());
        }

        void readPixels(const std::function<void(const PixelBuffer&)>& callback) override
        {
            const auto data = std::make_unique<uint8_t[]>(m_size.x * m_size.y * 4);

            glBindTexture(GL_TEXTURE_2D, m_resourceId);
            glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.get());

            const auto buffer = PixelBuffer{m_size.x, m_size.y, data.get()};
            callback(buffer);
        }

    private:
        TextureAsset(const uint32_t resourceId, const uint2 size, const FilterMode filterMode, const ExtendMode extendMode)
            : m_resourceId(resourceId),
              m_size(size),
              m_filterMode(filterMode),
              m_extendMode(extendMode)
        {
        }

        uint32_t m_resourceId;
        uint2 m_size;
        FilterMode m_filterMode;
        ExtendMode m_extendMode;
    };

} // namespace processing

namespace processing
{
    TextureAssetManager::TextureAssetManager()
        : m_nextAssetId(1)
    {
    }

    Texture TextureAssetManager::load(const std::filesystem::path& filepath)
    {
        const std::string path = filepath.string();

        stbi_set_flip_vertically_on_load(1);

        int width, height, channels;
        std::unique_ptr<uint8_t, decltype(&stbi_image_free)> data(stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha), &stbi_image_free);
        if (data == nullptr)
        {
            error(std::format("Failed to load image \"{}\"", path));
        }

        return create(static_cast<uint32_t>(width), static_cast<uint32_t>(height), data.get());
    }

    Texture TextureAssetManager::create(const uint32_t width, const uint32_t height, const uint8_t* data)
    {
        const auto insertion = m_assets.insert(std::make_pair(m_nextAssetId++, TextureAsset::create(width, height, data)));
        const auto assetId = insertion.first->first;

        return Texture(AssetId{assetId}, insertion.first->second);
    }

    Texture TextureAssetManager::copy(const Texture& source, const uint32_t left, const uint32_t top, const uint32_t width, const uint32_t height)
    {
        TextureImpl& impl = getAsset(source.getAssetId());
        auto newTexture = impl.copy(left, top, width, height);
        const auto insertion = m_assets.insert(std::make_pair(m_nextAssetId++, std::move(newTexture)));
        const auto assetId = insertion.first->first;

        return Texture(AssetId{assetId}, insertion.first->second);
    }

    TextureImpl& TextureAssetManager::getAsset(AssetId assetId)
    {
        const auto itr = m_assets.find(assetId.value);
        if (itr != m_assets.end())
        {
            return *itr->second;
        }

        throw std::runtime_error("Could not find texture with given id");
    }
} // namespace processing
