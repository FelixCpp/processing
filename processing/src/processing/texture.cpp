#include <processing/texture.hpp>

#include <glad/gl.h>
#include <stb/stb_image.h>

#include <format>
#include <stdexcept>

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

        ~TextureAsset()
        {
            glDeleteTextures(1, &m_resourceId);
        }

        void setExtendMode(const ExtendMode mode)
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

        ExtendMode getExtendMode() const
        {
            return m_extendMode;
        }

        void setFilterMode(const FilterMode mode)
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

        FilterMode getFilterMode() const
        {
            return m_filterMode;
        }

        ResourceId getResourceId() const
        {
            return ResourceId{.value = m_resourceId};
        }

        uint2 getSize() const
        {
            return m_size;
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
