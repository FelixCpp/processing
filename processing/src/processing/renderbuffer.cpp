#include <processing/processing.hpp>

namespace processing
{
    Renderbuffer::Renderbuffer()
        : m_assetId(AssetId{.value = 0}),
          m_impl(nullptr)
    {
    }

    Renderbuffer::Renderbuffer(const AssetId assetId, std::shared_ptr<PlatformRenderbuffer> impl)
        : m_assetId{assetId},
          m_impl{std::move(impl)}
    {
    }

    Image& Renderbuffer::getImage()
    {
        return m_impl->getImage();
    }

    uint2 Renderbuffer::getSize() const
    {
        return m_impl->getSize();
    }

    AssetId Renderbuffer::getAssetId() const
    {
        return m_assetId;
    }

} // namespace processing
