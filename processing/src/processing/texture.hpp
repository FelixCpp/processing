#ifndef _PROCESSING_INCLUDE_TEXTURE_HPP_
#define _PROCESSING_INCLUDE_TEXTURE_HPP_

#include <processing/processing.hpp>

#include <unordered_map>
#include <filesystem>

namespace processing
{

    class TextureAsset : public TextureImpl
    {
    public:
        static std::unique_ptr<TextureAsset> create(uint32_t width, uint32_t height, const uint8_t* data);
        ~TextureAsset();

        ResourceId getResourceId() const override;
        uint2 getSize() const override;

    private:
        explicit TextureAsset(uint32_t resourceId, uint2 size);
        uint32_t m_resourceId;
        uint2 m_size;
    };

    class TextureAssetManager
    {
    public:
        TextureAssetManager();
        Texture load(const std::filesystem::path& filepath);
        Texture create(uint32_t width, uint32_t height, const uint8_t* data);

        TextureImpl& getAsset(AssetId assetId);

    private:
        std::unordered_map<size_t, std::shared_ptr<TextureImpl>> m_assets;
        size_t m_nextAssetId;
    };

} // namespace processing

#endif // _PROCESSING_INCLUDE_TEXTURE_HPP_
