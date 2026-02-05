#ifndef _PROCESSING_INCLUDE_RENDERBUFFER_HPP_
#define _PROCESSING_INCLUDE_RENDERBUFFER_HPP_

#include <processing/processing.hpp>

#include <vector>
#include <memory>

namespace processing
{
    class MainRenderbuffer : public PlatformRenderbuffer
    {
    public:
        explicit MainRenderbuffer(u32 width, u32 height);

        void resize(u32 width, u32 height);

        uint2 getSize() const override;
        ResourceId getResourceId() const override;

        void blit();

    private:
        uint2 m_size;
    };

    class RenderbufferAssetHandler
    {
    public:
        Renderbuffer create(u32 width, u32 height, FilterMode filterMode, ExtendMode extendMode);
        Renderbuffer getAsset();

    private:
        std::vector<std::shared_ptr<PlatformRenderbuffer>> m_assets;
    };
} // namespace processing

#endif // _PROCESSING_INCLUDE_RENDERBUFFER_HPP_
