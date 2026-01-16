#include "processing/processing.hpp"
#include "processing/render_style_stack.hpp"
#include <processing/graphics.hpp>
#include <processing/shape_builder.hpp>
#include <processing/batch_renderer.hpp>
#include <stdexcept>

namespace processing
{
    inline static constexpr float MIN_DEPTH = -1.0f;
    inline static constexpr float MAX_DEPTH = 1.0f;
    inline static constexpr float DEPTH_INCREMENT = (MAX_DEPTH - MIN_DEPTH) / 20'000.0f;

    Graphics::Graphics(const uint2 size) : m_renderTarget(std::make_unique<MainRenderTarget>(rect2u{0, 0, size.x, size.y})), m_renderer(BatchRenderer::create()), m_renderStyles(render_style_stack_create()), m_windowSize(size)
    {
    }

    void Graphics::beginDraw(const FrameSpecification& specification)
    {
        m_windowSize = specification.windowSize;

        m_renderTarget->setViewport(rect2u{0, 0, specification.framebufferSize.x, specification.framebufferSize.y});
        m_renderTarget->activate();
        m_renderer->beginDraw({
            .projectionMatrix = matrix4x4_orthographic(0.0f, 0.0f, static_cast<float>(specification.windowSize.x), static_cast<float>(specification.windowSize.y), MIN_DEPTH, MAX_DEPTH),
            .viewMatrix = matrix4x4_identity(),
        });
        render_style_stack_reset(m_renderStyles);
        m_currentDepth = MIN_DEPTH;
    }

    void Graphics::endDraw()
    {
        m_renderer->endDraw();
    }

    rect2f Graphics::getViewport()
    {
        return rect2f{0.0f, 0.0f, static_cast<float>(m_windowSize.x), static_cast<float>(m_windowSize.y)};
    }

    void Graphics::strokeJoin(const StrokeJoin lineJoin)
    {
        RenderStyle& style = render_style_stack_peek(m_renderStyles);
        style.strokeJoin = lineJoin;
    }

    void Graphics::strokeCap(const StrokeCap strokeCap)
    {
        RenderStyle& style = render_style_stack_peek(m_renderStyles);
        style.strokeCap = strokeCap;
    }

    void Graphics::pushState()
    {
        render_style_stack_push(m_renderStyles, peekState());
    }

    void Graphics::popState()
    {
        render_style_stack_pop(m_renderStyles);
    }

    RenderStyle& Graphics::peekState()
    {
        return render_style_stack_peek(m_renderStyles);
    }

    void Graphics::background(int red, int green, int blue, int alpha)
    {
        background(color(red, green, blue, alpha));
    }

    void Graphics::background(int grey, int alpha)
    {
        background(color(grey, grey, grey, alpha));
    }

    void Graphics::background(color_t color)
    {
        const rect2f viewport = getViewport();
        const Contour rect_contour = contour_rect_fill(viewport.left, viewport.top, viewport.width, viewport.height);
        const Shape shape = shape_from_contour(rect_contour, color, getNextDepth());

        m_renderer->submit({
            .vertices = shape.vertices,
            .indices = shape.indices,
        });
    }

    void Graphics::fill(int red, int green, int blue, int alpha)
    {
        fill(color(red, green, blue, alpha));
    }

    void Graphics::fill(int grey, int alpha)
    {
        fill(color(grey, alpha));
    }

    void Graphics::fill(color_t color)
    {
        RenderStyle& style = peekState();
        style.fillColor = color;
        style.isFillEnabled = true;
    }

    void Graphics::noFill()
    {
        RenderStyle& style = peekState();
        style.isFillEnabled = false;
    }

    void Graphics::stroke(int red, int green, int blue, int alpha)
    {
        stroke(color(red, green, blue, alpha));
    }

    void Graphics::stroke(int grey, int alpha)
    {
        stroke(color(grey, alpha));
    }

    void Graphics::stroke(color_t color)
    {
        RenderStyle& style = peekState();
        style.strokeColor = color;
        style.isStrokeEnabled = true;
    }

    void Graphics::noStroke()
    {
        RenderStyle& style = peekState();
        style.isStrokeEnabled = false;
    }

    void Graphics::strokeWeight(float strokeWeight)
    {
        RenderStyle& style = peekState();
        style.strokeWeight = strokeWeight;
    }

    void Graphics::rectMode(RectMode rectMode)
    {
        RenderStyle& style = peekState();
        style.rectMode = rectMode;
    }

    void Graphics::rect(float left, float top, float width, float height)
    {
        const RenderStyle& style = render_style_stack_peek(m_renderStyles);
        const rect2f rectMode = style.rectMode(left, top, width, height);

        if (style.isFillEnabled)
        {
            const Contour contour = contour_rect_fill(left, top, width, height);
            const Shape shape = shape_from_contour(contour, style.fillColor, getNextDepth());

            m_renderer->submit({
                .vertices = shape.vertices,
                .indices = shape.indices,
            });
        }

        if (style.isStrokeEnabled)
        {
            const Contour contour = contour_rect_stroke(left, top, width, height, style.strokeWeight, style.strokeJoin);
            const Shape shape = shape_from_contour(contour, style.strokeColor, getNextDepth());

            m_renderer->submit({
                .vertices = shape.vertices,
                .indices = shape.indices,
            });
        }
    }

    void Graphics::square(float left, float top, float size)
    {
        rect(left, top, size, size);
    }

    void Graphics::ellipse(float centerX, float centerY, float radiusX, float radiusY)
    {
        const RenderStyle& style = render_style_stack_peek(m_renderStyles);

        if (style.isFillEnabled)
        {
            const Contour contour = contour_ellipse_fill(centerX, centerY, radiusX, radiusY, 32);
            const Shape shape = shape_from_contour(contour, style.fillColor, getNextDepth());

            m_renderer->submit({
                .vertices = shape.vertices,
                .indices = shape.indices,
            });
        }

        if (style.isStrokeEnabled)
        {
            const Contour contour = contour_ellipse_stroke(centerX, centerY, radiusX, radiusY, style.strokeWeight, 32, style.strokeJoin);
            const Shape shape = shape_from_contour(contour, style.strokeColor, getNextDepth());

            m_renderer->submit({
                .vertices = shape.vertices,
                .indices = shape.indices,
            });
        }
    }

    void Graphics::circle(float centerX, float centerY, float radius)
    {
        ellipse(centerX, centerY, radius, radius);
    }

    void Graphics::line(float x1, float y1, float x2, float y2)
    {
        const RenderStyle& style = render_style_stack_peek(m_renderStyles);
        const Contour contour = contour_line(x1, y1, x2, y2, style.strokeWeight, style.strokeCap);
        const Shape shape = shape_from_contour(contour, style.strokeColor, getNextDepth());

        m_renderer->submit({
            .vertices = shape.vertices,
            .indices = shape.indices,
        });
    }

    void Graphics::triangle(float x1, float y1, float x2, float y2, float x3, float y3)
    {
        const RenderStyle& style = render_style_stack_peek(m_renderStyles);

        if (style.isFillEnabled)
        {
            const Contour contour = contour_triangle_fill(x1, y1, x2, y2, x3, y3);
            const Shape shape = shape_from_contour(contour, style.fillColor, getNextDepth());

            m_renderer->submit({
                .vertices = shape.vertices,
                .indices = shape.indices,
            });
        }

        if (style.isStrokeEnabled)
        {
            const Contour contour = contour_triangle_stroke(x1, y1, x2, y2, x3, y3, style.strokeWeight, style.strokeJoin);
            const Shape shape = shape_from_contour(contour, style.strokeColor, getNextDepth());

            m_renderer->submit({
                .vertices = shape.vertices,
                .indices = shape.indices,
            });
        }
    }

    void Graphics::point(float x, float y)
    {
        const RenderStyle& style = render_style_stack_peek(m_renderStyles);
        const Contour contour = contour_ellipse_fill(x, y, style.strokeWeight, style.strokeWeight, 16);
        const Shape shape = shape_from_contour(contour, style.strokeColor, getNextDepth());

        m_renderer->submit({
            .vertices = shape.vertices,
            .indices = shape.indices,
        });
    }

    float Graphics::getNextDepth()
    {
        float value = m_currentDepth;
        m_currentDepth += DEPTH_INCREMENT;
        return value;
    }
} // namespace processing
