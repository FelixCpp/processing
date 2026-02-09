#include <processing/processing.hpp>

namespace processing
{
    Renderbuffer::Renderbuffer()
        : m_impl(nullptr)
    {
    }

    Renderbuffer::Renderbuffer(std::shared_ptr<PlatformRenderbuffer> impl)
        : m_impl{std::move(impl)}
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
        return m_impl->getAssetId();
    }
} // namespace processing
