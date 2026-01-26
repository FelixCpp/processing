#ifndef _PROCESSING_INCLUDE_GRAPHICS_HPP_
#define _PROCESSING_INCLUDE_GRAPHICS_HPP_

#include <processing/processing.hpp>
#include <processing/render_targets.hpp>
#include <processing/render_style.hpp>
#include <processing/render_style_stack.hpp>
#include <processing/matrix_stack.hpp>
#include <processing/depth_provider.hpp>
#include <processing/render_buffer.hpp>
#include <processing/texture.hpp>
#include <processing/shader.hpp>

namespace processing
{
    class Graphics
    {
    public:
        explicit Graphics(uint2 size, ShaderAssetManager& shaderAssetManager, RenderTargetManager& renderTargetManager, TextureAssetManager& textureAssetManager);

        void event(const Event& event);

        void beginDraw();
        void endDraw();
        rect2f getViewport() const;

        void renderBuffer(RenderBuffer renderBuffer);
        void noRenderBuffer();

        void strokeJoin(StrokeJoin strokeJoin);
        void strokeCap(StrokeCap strokeCap);

        void pushState();
        void popState();
        RenderStyle& peekState();

        void pushMatrix();
        void popMatrix();
        void resetMatrix();
        matrix4x4& peekMatrix();

        void translate(float x, float y);
        void scale(float x, float y);
        void rotate(float angle);

        void blendMode(const BlendMode& blendMode);
        void shader(const Shader& shader);
        void noShader();

        void background(const Texture& texture);
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
        struct RenderingLayer
        {
            DepthProvider depthProvider;
            RenderBuffer renderBuffer;
        };

        float getNextDepth();
        void submit(const RenderingSubmission& submission);

        DepthProvider& getActiveDepthProvider();

        std::unique_ptr<Renderer> m_renderer;
        RenderTargetManager* m_renderTargetManager;
        TextureAssetManager* m_textureAssetManager;
        RenderStyleStack m_renderStyles;

        // Core-Layer
        DepthProvider m_depthProvider;

        std::unique_ptr<RenderingLayer> m_offscreenLayer;

        uint2 m_windowSize;
        uint2 m_framebufferSize;
    };
} // namespace processing

#endif // _PROCESSING_INCLUDE_GRAPHICS_HPP_
