#ifndef _PROCESSING_INCLUDE_GRAPHICS_HPP_
#define _PROCESSING_INCLUDE_GRAPHICS_HPP_

#include <processing/processing.hpp>
#include <processing/render_targets.hpp>
#include <processing/render_style.hpp>
#include <processing/render_style_stack.hpp>

namespace processing
{
    struct FrameSpecification
    {
        uint2 windowSize;
        uint2 framebufferSize;
    };

    class Graphics
    {
    public:
        explicit Graphics(uint2 size);

        void beginDraw(const FrameSpecification& specification);
        void endDraw();

        rect2f getViewport();

        void strokeJoin(StrokeJoin strokeJoin);
        void strokeCap(StrokeCap strokeCap);

        void pushState();
        void popState();
        RenderStyle& peekState();

        void blendMode(const BlendMode& blendMode);

        void background(int red, int green, int blue, int alpha = 255);
        void background(int grey, int alpha = 255);
        void background(color_t color);

        void fill(int red, int green, int blue, int alpha = 255);
        void fill(int grey, int alpha = 255);
        void fill(color_t color);
        void noFill();

        void stroke(int red, int green, int blue, int alpha = 255);
        void stroke(int grey, int alpha = 255);
        void stroke(color_t color);
        void noStroke();

        void imageMode(RectMode imageMode);
        void imageSourceMode(ImageSourceMode imageSourceMode);
        void imageTint(int red, int green, int blue, int alpha = 255);
        void imageTint(int grey, int alpha = 255);
        void imageTint(color_t color);

        void strokeWeight(float strokeWeight);
        void rectMode(RectMode rectMode);
        void ellipseMode(EllipseMode ellipseMode);

        void rect(float left, float top, float width, float height);
        void square(float left, float top, float size);
        void ellipse(float centerX, float centerY, float radiusX, float radiusY);
        void circle(float centerX, float centerY, float radius);
        void line(float x1, float y1, float x2, float y2);
        void triangle(float x1, float y1, float x2, float y2, float x3, float y3);
        void point(float x, float y);
        void image(const Texture& texture, float x1, float y1);
        void image(const Texture& texture, float x1, float y1, float x2, float y2);
        void image(const Texture& texture, float x1, float y1, float x2, float y2, float sx1, float sy1, float sx2, float sy2);

    private:
        float getNextDepth();

        std::unique_ptr<MainRenderTarget> m_renderTarget;
        std::unique_ptr<Renderer> m_renderer;
        RenderStyleStack m_renderStyles;
        float m_currentDepth;

        uint2 m_windowSize;
    };
} // namespace processing

#endif // _PROCESSING_INCLUDE_GRAPHICS_HPP_
