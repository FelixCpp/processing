#ifndef _PROCESSING_INCLUDE_GRAPHICS_HPP_
#define _PROCESSING_INCLUDE_GRAPHICS_HPP_

#include <processing/processing.hpp>
#include <processing/render_targets.hpp>
#include <processing/render_style.hpp>
#include <processing/render_style_stack.hpp>

namespace processing
{
    class GraphicsImpl : public Graphics
    {
    public:
        explicit GraphicsImpl(std::shared_ptr<RenderTarget> rendertarget, std::shared_ptr<Renderer> renderer);

        void beginDraw() override;
        void endDraw() override;
        uint2 getSize() override;

        void strokeJoin(StrokeJoin strokeJoin) override;
        void strokeCap(StrokeCap strokeCap) override;

        void pushState() override;
        void popState() override;
        RenderStyle& peekState();

        void background(int red, int green, int blue, int alpha = 255) override;
        void background(int grey, int alpha = 255) override;
        void background(color_t color) override;

        void fill(int red, int green, int blue, int alpha = 255) override;
        void fill(int grey, int alpha = 255) override;
        void fill(color_t color) override;
        void noFill() override;

        void stroke(int red, int green, int blue, int alpha = 255) override;
        void stroke(int grey, int alpha = 255) override;
        void stroke(color_t color) override;
        void noStroke() override;

        void strokeWeight(float strokeWeight) override;

        void rect(float left, float top, float width, float height) override;
        void square(float left, float top, float size) override;
        void ellipse(float centerX, float centerY, float radiusX, float radiusY) override;
        void circle(float centerX, float centerY, float radius) override;
        void line(float x1, float y1, float x2, float y2) override;
        void triangle(float x1, float y1, float x2, float y2, float x3, float y3) override;
        void point(float x, float y) override;

    private:
        float getNextDepth();

        std::shared_ptr<RenderTarget> m_renderTarget;
        std::shared_ptr<Renderer> m_renderer;
        RenderStyleStack m_renderStyles;
        float m_currentDepth;
    };
} // namespace processing

#endif // _PROCESSING_INCLUDE_GRAPHICS_HPP_
