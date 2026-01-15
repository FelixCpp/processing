#ifndef _PROCESSING_INCLUDE_GRAPHICS_HPP_
#define _PROCESSING_INCLUDE_GRAPHICS_HPP_

#include <processing/processing.hpp>
#include <processing/render_targets.hpp>
#include <processing/render_style.hpp>
#include <processing/render_style_stack.hpp>

namespace processing
{
    struct Shape;

    class BaseGraphics : public Graphics
    {
    public:
        explicit BaseGraphics();

        rect2f getViewport() override;

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
        void rectMode(RectMode rectMode) override;

        void rect(float left, float top, float width, float height) override;
        void square(float left, float top, float size) override;
        void ellipse(float centerX, float centerY, float radiusX, float radiusY) override;
        void circle(float centerX, float centerY, float radius) override;
        void line(float x1, float y1, float x2, float y2) override;
        void triangle(float x1, float y1, float x2, float y2, float x3, float y3) override;
        void point(float x, float y) override;
        void image(const Texture& texture, float left, float top, float width, float height) override;

    protected:
        virtual Renderer& getRenderer() = 0;

        void submit(const Shape& shape);

    private:
        float getNextDepth();

        RenderStyleStack m_renderStyles;
        float m_currentDepth;

        matrix4x4 m_projectionMatrix;
    };

    class MainGraphics : public BaseGraphics
    {
    public:
        explicit MainGraphics(const rect2u& windowViewport, std::shared_ptr<Renderer> renderer);

        void handle(const Event& event);

        void pause();
        void resume();

        void beginDraw();
        void endDraw();

    protected:
        Renderer& getRenderer() override;

    private:
        MainRenderTarget m_renderTarget;
        std::shared_ptr<Renderer> m_renderer;
        rect2u m_windowViewport;
        rect2u m_framebufferViewport;
    };

    class OffscreenGraphics : public BaseGraphics, virtual ClientGraphics
    {
    public:
        explicit OffscreenGraphics(const uint2& size, std::shared_ptr<Renderer> renderer);

        void pause();
        void resume();

        void beginDraw();
        void endDraw();

        const Texture& getTexture() const override;

    private:
        Renderer& getRenderer();

        std::unique_ptr<OffscreenRenderTarget> m_renderTarget;
        std::shared_ptr<Renderer> m_renderer;
        rect2u m_windowViewport;
    };
} // namespace processing

#endif // _PROCESSING_INCLUDE_GRAPHICS_HPP_
