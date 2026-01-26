#include "processing/batch_renderer.hpp"
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
    std::unique_ptr<TextureAsset> TextureAsset::create(const uint32_t width, const uint32_t height, const uint8_t* data)
    {
        uint32_t id;
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glBindTexture(GL_TEXTURE_2D, 0);

        return std::unique_ptr<TextureAsset>(new TextureAsset(id, uint2{width, height}));
    }

    TextureAsset::~TextureAsset()
    {
        glDeleteTextures(1, &m_resourceId);
    }

    ResourceId TextureAsset::getResourceId() const
    {
        return ResourceId{.value = m_resourceId};
    }

    uint2 TextureAsset::getSize() const
    {
        return m_size;
    }

    TextureAsset::TextureAsset(const uint32_t resourceId, const uint2 size)
        : m_resourceId(resourceId),
          m_size(size)
    {
    }
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
