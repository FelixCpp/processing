#include <processing/render_targets.hpp>

namespace processing
{
    MainRenderTarget::MainRenderTarget(const rect2u viewport) : m_viewport(viewport)
    {
    }

    void MainRenderTarget::setViewport(const rect2u viewport)
    {
        m_viewport = viewport;
    }

    void MainRenderTarget::activate()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(m_viewport.left, m_viewport.top, m_viewport.width, m_viewport.height);
    }
} // namespace processing
