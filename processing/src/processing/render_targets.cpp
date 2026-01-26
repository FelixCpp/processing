#include <processing/render_targets.hpp>

namespace processing
{
    MainRenderTarget::MainRenderTarget(const uint32_t width, const uint32_t height)
        : m_viewportSize(width, height)
    {
    }

    void MainRenderTarget::setSize(const uint32_t width, const uint32_t height)
    {
        m_viewportSize = {width, height};
    }

    uint2 MainRenderTarget::getSize() const
    {
        return m_viewportSize;
    }

    uint32_t MainRenderTarget::getFramebufferId() const
    {
        return 0;
    }

    void MainRenderTarget::activate()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);
    }
} // namespace processing
