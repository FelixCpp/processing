#include <processing/recursive_renderer.hpp>

namespace processing
{

    RecursiveRenderer::RecursiveRenderer(std::shared_ptr<Renderer> renderer) : m_renderer(std::move(renderer)), m_isDrawing(false)
    {
    }

    rect2f RecursiveRenderer::getViewport()
    {
        return m_renderer->getViewport();
    }

    void RecursiveRenderer::beginDraw(const ProjectionDetails& details)
    {
        if (m_isDrawing)
        {
            flush();
        }

        m_renderer->beginDraw(details);
        m_isDrawing = true;
    }

    void RecursiveRenderer::endDraw()
    {
        m_renderer->endDraw();
        m_isDrawing = false;
    }

    void RecursiveRenderer::submit(const RenderingSubmission& submission)
    {
        m_renderer->submit(submission);
    }

    void RecursiveRenderer::flush()
    {
        m_renderer->flush();
    }

} // namespace processing
