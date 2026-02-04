#ifndef _PROCESSING_INCLUDE_IMAGE_HPP_
#define _PROCESSING_INCLUDE_IMAGE_HPP_

#include <processing/processing.hpp>

namespace processing
{
    class ImageAssetHandler
    {
    public:
        Image createImage(u32 width, u32 height, FilterMode filterMode, ExtendMode extendMode);
        Image loadImage(const std::filesystem::path& filepath, FilterMode filterMode, ExtendMode extendMode);
        Image loadAsset(AssetId assetId);

    private:
        std::vector<std::shared_ptr<PlatformImage>> m_assets;
    };
} // namespace processing

#endif // _PROCESSING_INCLUDE_IMAGE_HPP_
