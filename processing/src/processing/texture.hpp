#ifndef _PROCESSING_INCLUDE_TEXTURE_HPP_
#define _PROCESSING_INCLUDE_TEXTURE_HPP_

#include <processing/processing.hpp>

#include <unordered_map>
#include <filesystem>

namespace processing
{

    class TextureAssetManager
    {
    public:
        TextureAssetManager();
        Texture load(const std::filesystem::path& filepath);
        Texture create(uint32_t width, uint32_t height, const uint8_t* data);
        Texture copy(const Texture& source, uint32_t left, uint32_t top, uint32_t width, uint32_t height);

        TextureImpl& getAsset(AssetId assetId);

    private:
        std::unordered_map<size_t, std::shared_ptr<TextureImpl>> m_assets;
        size_t m_nextAssetId;
    };

} // namespace processing

#endif // _PROCESSING_INCLUDE_TEXTURE_HPP_
