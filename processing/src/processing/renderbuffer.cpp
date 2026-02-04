#include <processing/processing.hpp>
#include <glad/gl.h>

namespace processing
{

    class OpenGLRenderbuffer : PlatformRenderbuffer
    {
    public:
    private:
    };

} // namespace processing

namespace processing
{
    uint2 Renderbuffer::getSize() const
    {
        return m_impl->getSize();
    }

    ResourceId Renderbuffer::getResourceId() const
    {
        return m_impl->getResourceId();
    }

    AssetId Renderbuffer::getAssetId() const
    {
        return m_assetId;
    }

} // namespace processing
