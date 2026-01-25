#ifndef _PROCESSING_INCLUDE_RENDER_BUFFER_HPP_
#define _PROCESSING_INCLUDE_RENDER_BUFFER_HPP_

#include <processing/processing.hpp>
#include <processing/texture.hpp>
#include <processing/render_targets.hpp>

#include <unordered_map>
#include <memory>

#include <glad/gl.h>

namespace processing
{
    class RenderTargetManager
    {
    public:
        RenderTargetManager();

        RenderBuffer create(uint32_t width, uint32_t height, TextureAssetManager& textureAssetManager);
        RenderBufferImpl& getAsset(AssetId assetId);

    private:
        std::unordered_map<size_t, std::shared_ptr<RenderBufferImpl>> m_assets;
        size_t m_nextAssetId;
    };
} // namespace processing

#endif // _PROCESSING_INCLUDE_RENDER_BUFFER_HPP_
