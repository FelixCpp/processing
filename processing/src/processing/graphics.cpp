#include "processing/processing.hpp"
#include "processing/render_style_stack.hpp"
#include <processing/graphics.hpp>
#include <processing/shape_builder.hpp>

namespace processing
{
    inline static constexpr float MIN_DEPTH = -1.0f;
    inline static constexpr float MAX_DEPTH = 1.0f;
    inline static constexpr float DEPTH_INCREMENT = (MAX_DEPTH - MIN_DEPTH) / 20'000.0f;

    GraphicsImpl::GraphicsImpl(std::unique_ptr<RenderTarget> renderTarget, std::shared_ptr<Renderer> renderer) : m_renderTarget(std::move(renderTarget)), m_renderer(std::move(renderer))
    {
    }

    void GraphicsImpl::resize(const uint2 size)
    {
        m_projectionMatrix = matrix4x4_orthographic(0.0f, 0.0f, static_cast<float>(size.x), static_cast<float>(size.y), MIN_DEPTH, MAX_DEPTH);
    }

    void GraphicsImpl::beginDraw()
    {
        const ProjectionDetails details = {
            .projectionMatrix = m_projectionMatrix,
            .viewMatrix = matrix4x4_identity(),
        };

        m_renderer->beginDraw(details);
        m_renderTarget->beginDraw();
        render_style_stack_reset(m_renderStyles);
        m_currentDepth = MIN_DEPTH;
    }

    void GraphicsImpl::endDraw()
    {
        m_renderTarget->endDraw();
        m_renderer->endDraw();
    }

    rect2f GraphicsImpl::getViewport()
    {
        return m_renderTarget->getViewport();
    }

    void GraphicsImpl::strokeJoin(const StrokeJoin lineJoin)
    {
        RenderStyle& style = render_style_stack_peek(m_renderStyles);
        style.strokeJoin = lineJoin;
    }

    void GraphicsImpl::strokeCap(const StrokeCap strokeCap)
    {
        RenderStyle& style = render_style_stack_peek(m_renderStyles);
        style.strokeCap = strokeCap;
    }

    void GraphicsImpl::pushState()
    {
        render_style_stack_push(m_renderStyles, peekState());
    }

    void GraphicsImpl::popState()
    {
        render_style_stack_pop(m_renderStyles);
    }

    RenderStyle& GraphicsImpl::peekState()
    {
        return render_style_stack_peek(m_renderStyles);
    }

    void GraphicsImpl::background(int red, int green, int blue, int alpha)
    {
        background(color(red, green, blue, alpha));
    }

    void GraphicsImpl::background(int grey, int alpha)
    {
        background(color(grey, grey, grey, alpha));
    }

    void GraphicsImpl::background(color_t color)
    {
        const rect2f viewport = getViewport();
        const Contour rect_contour = contour_rect_fill(viewport.left, viewport.top, viewport.width, viewport.height);
        const Shape shape = shape_from_contour(rect_contour, color, getNextDepth());

        m_renderer->submit({
            .vertices = shape.vertices,
            .indices = shape.indices,
        });
    }

    void GraphicsImpl::fill(int red, int green, int blue, int alpha)
    {
        fill(color(red, green, blue, alpha));
    }

    void GraphicsImpl::fill(int grey, int alpha)
    {
        fill(color(grey, alpha));
    }

    void GraphicsImpl::fill(color_t color)
    {
        RenderStyle& style = peekState();
        style.fillColor = color;
        style.isFillEnabled = true;
    }

    void GraphicsImpl::noFill()
    {
        RenderStyle& style = peekState();
        style.isFillEnabled = false;
    }

    void GraphicsImpl::stroke(int red, int green, int blue, int alpha)
    {
        stroke(color(red, green, blue, alpha));
    }

    void GraphicsImpl::stroke(int grey, int alpha)
    {
        stroke(color(grey, alpha));
    }

    void GraphicsImpl::stroke(color_t color)
    {
        RenderStyle& style = peekState();
        style.strokeColor = color;
        style.isStrokeEnabled = true;
    }

    void GraphicsImpl::noStroke()
    {
        RenderStyle& style = peekState();
        style.isStrokeEnabled = false;
    }

    void GraphicsImpl::strokeWeight(float strokeWeight)
    {
        RenderStyle& style = peekState();
        style.strokeWeight = strokeWeight;
    }

    void GraphicsImpl::rectMode(RectMode rectMode)
    {
        RenderStyle& style = peekState();
        style.rectMode = rectMode;
    }

    void GraphicsImpl::rect(float left, float top, float width, float height)
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

    void GraphicsImpl::square(float left, float top, float size)
    {
        rect(left, top, size, size);
    }

    void GraphicsImpl::ellipse(float centerX, float centerY, float radiusX, float radiusY)
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

    void GraphicsImpl::circle(float centerX, float centerY, float radius)
    {
        ellipse(centerX, centerY, radius, radius);
    }

    void GraphicsImpl::line(float x1, float y1, float x2, float y2)
    {
        const RenderStyle& style = render_style_stack_peek(m_renderStyles);
        const Contour contour = contour_line(x1, y1, x2, y2, style.strokeWeight, style.strokeCap);
        const Shape shape = shape_from_contour(contour, style.strokeColor, getNextDepth());

        m_renderer->submit({
            .vertices = shape.vertices,
            .indices = shape.indices,
        });
    }

    void GraphicsImpl::triangle(float x1, float y1, float x2, float y2, float x3, float y3)
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

    void GraphicsImpl::point(float x, float y)
    {
        const RenderStyle& style = render_style_stack_peek(m_renderStyles);
        const Contour contour = contour_ellipse_fill(x, y, style.strokeWeight, style.strokeWeight, 16);
        const Shape shape = shape_from_contour(contour, style.strokeColor, getNextDepth());

        m_renderer->submit({
            .vertices = shape.vertices,
            .indices = shape.indices,
        });
    }

    float GraphicsImpl::getNextDepth()
    {
        float value = m_currentDepth;
        m_currentDepth += DEPTH_INCREMENT;
        return value;
    }
} // namespace processing
