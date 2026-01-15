#ifndef _PROCESSING_INCLUDE_RECURSIVE_RENDERER_HPP_
#define _PROCESSING_INCLUDE_RECURSIVE_RENDERER_HPP_

#include <processing/processing.hpp>

namespace processing
{

    class RecursiveRenderer : public Renderer
    {
    public:
        explicit RecursiveRenderer(std::shared_ptr<Renderer> m_renderer);

        rect2f getViewport() override;

        void beginDraw(const ProjectionDetails& details) override;
        void endDraw() override;
        void submit(const RenderingSubmission& submission) override;
        void flush() override;

    private:
        std::shared_ptr<Renderer> m_renderer;
        bool m_isDrawing;
    };

} // namespace processing

#endif // _PROCESSING_INCLUDE_RECURSIVE_RENDERER_HPP_
