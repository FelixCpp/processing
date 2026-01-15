#include "processing/processing.hpp"
#include "processing/render_style_stack.hpp"
#include <processing/graphics.hpp>
#include <processing/shape_builder.hpp>

namespace processing
{
    inline static constexpr float MIN_DEPTH = -1.0f;
    inline static constexpr float MAX_DEPTH = 1.0f;
    inline static constexpr float DEPTH_INCREMENT = (MAX_DEPTH - MIN_DEPTH) / 20'000.0f;

    BaseGraphics::BaseGraphics()
    {
    }

    rect2f BaseGraphics::getViewport()
    {
        return getRenderer().getViewport();
    }

    void BaseGraphics::strokeJoin(const StrokeJoin lineJoin)
    {
        RenderStyle& style = render_style_stack_peek(m_renderStyles);
        style.strokeJoin = lineJoin;
    }

    void BaseGraphics::strokeCap(const StrokeCap strokeCap)
    {
        RenderStyle& style = render_style_stack_peek(m_renderStyles);
        style.strokeCap = strokeCap;
    }

    void BaseGraphics::pushState()
    {
        render_style_stack_push(m_renderStyles, peekState());
    }

    void BaseGraphics::popState()
    {
        render_style_stack_pop(m_renderStyles);
    }

    RenderStyle& BaseGraphics::peekState()
    {
        return render_style_stack_peek(m_renderStyles);
    }

    void BaseGraphics::background(int red, int green, int blue, int alpha)
    {
        background(color(red, green, blue, alpha));
    }

    void BaseGraphics::background(int grey, int alpha)
    {
        background(color(grey, grey, grey, alpha));
    }

    void BaseGraphics::background(color_t color)
    {
        const rect2f viewport = getViewport();
        const Contour rect_contour = contour_rect_fill(viewport.left, viewport.top, viewport.width, viewport.height);
        const Shape shape = shape_from_contour(rect_contour, color, getNextDepth());

        submit({
            .vertices = shape.vertices,
            .indices = shape.indices,
        });
    }

    void BaseGraphics::fill(int red, int green, int blue, int alpha)
    {
        fill(color(red, green, blue, alpha));
    }

    void BaseGraphics::fill(int grey, int alpha)
    {
        fill(color(grey, alpha));
    }

    void BaseGraphics::fill(color_t color)
    {
        RenderStyle& style = peekState();
        style.fillColor = color;
        style.isFillEnabled = true;
    }

    void BaseGraphics::noFill()
    {
        RenderStyle& style = peekState();
        style.isFillEnabled = false;
    }

    void BaseGraphics::stroke(int red, int green, int blue, int alpha)
    {
        stroke(color(red, green, blue, alpha));
    }

    void BaseGraphics::stroke(int grey, int alpha)
    {
        stroke(color(grey, alpha));
    }

    void BaseGraphics::stroke(color_t color)
    {
        RenderStyle& style = peekState();
        style.strokeColor = color;
        style.isStrokeEnabled = true;
    }

    void BaseGraphics::noStroke()
    {
        RenderStyle& style = peekState();
        style.isStrokeEnabled = false;
    }

    void BaseGraphics::strokeWeight(float strokeWeight)
    {
        RenderStyle& style = peekState();
        style.strokeWeight = strokeWeight;
    }

    void BaseGraphics::rectMode(RectMode rectMode)
    {
        RenderStyle& style = peekState();
        style.rectMode = rectMode;
    }

    void BaseGraphics::rect(float left, float top, float width, float height)
    {
        const RenderStyle& style = render_style_stack_peek(m_renderStyles);
        const rect2f rectMode = style.rectMode(left, top, width, height);

        if (style.isFillEnabled)
        {
            const Contour contour = contour_rect_fill(left, top, width, height);
            const Shape shape = shape_from_contour(contour, style.fillColor, getNextDepth());

            submit(shape);
        }

        if (style.isStrokeEnabled)
        {
            const Contour contour = contour_rect_stroke(left, top, width, height, style.strokeWeight, style.strokeJoin);
            const Shape shape = shape_from_contour(contour, style.strokeColor, getNextDepth());

            submit(shape);
        }
    }

    void BaseGraphics::square(float left, float top, float size)
    {
        rect(left, top, size, size);
    }

    void BaseGraphics::ellipse(float centerX, float centerY, float radiusX, float radiusY)
    {
        const RenderStyle& style = render_style_stack_peek(m_renderStyles);

        if (style.isFillEnabled)
        {
            const Contour contour = contour_ellipse_fill(centerX, centerY, radiusX, radiusY, 32);
            const Shape shape = shape_from_contour(contour, style.fillColor, getNextDepth());

            submit(shape);
        }

        if (style.isStrokeEnabled)
        {
            const Contour contour = contour_ellipse_stroke(centerX, centerY, radiusX, radiusY, style.strokeWeight, 32, style.strokeJoin);
            const Shape shape = shape_from_contour(contour, style.strokeColor, getNextDepth());

            submit(shape);
        }
    }

    void BaseGraphics::circle(float centerX, float centerY, float radius)
    {
        ellipse(centerX, centerY, radius, radius);
    }

    void BaseGraphics::line(float x1, float y1, float x2, float y2)
    {
        const RenderStyle& style = render_style_stack_peek(m_renderStyles);
        const Contour contour = contour_line(x1, y1, x2, y2, style.strokeWeight, style.strokeCap);
        const Shape shape = shape_from_contour(contour, style.strokeColor, getNextDepth());

        submit(shape);
    }

    void BaseGraphics::triangle(float x1, float y1, float x2, float y2, float x3, float y3)
    {
        const RenderStyle& style = render_style_stack_peek(m_renderStyles);

        if (style.isFillEnabled)
        {
            const Contour contour = contour_triangle_fill(x1, y1, x2, y2, x3, y3);
            const Shape shape = shape_from_contour(contour, style.fillColor, getNextDepth());

            submit(shape);
        }

        if (style.isStrokeEnabled)
        {
            const Contour contour = contour_triangle_stroke(x1, y1, x2, y2, x3, y3, style.strokeWeight, style.strokeJoin);
            const Shape shape = shape_from_contour(contour, style.strokeColor, getNextDepth());

            submit(shape);
        }
    }

    void BaseGraphics::point(float x, float y)
    {
        const RenderStyle& style = render_style_stack_peek(m_renderStyles);
        const Contour contour = contour_ellipse_fill(x, y, style.strokeWeight, style.strokeWeight, 16);
        const Shape shape = shape_from_contour(contour, style.strokeColor, getNextDepth());

        submit(shape);
    }

    void BaseGraphics::image(const Texture& texture, float left, float top, float width, float height)
    {
        const RenderStyle& style = render_style_stack_peek(m_renderStyles);
        const rect2f rectMode = style.rectMode(left, top, width, height);

        const Contour contour = contour_rect_fill(left, top, width, height);
        const Shape shape = shape_from_contour(contour, style.fillColor, getNextDepth());

        getRenderer().submit({
            .vertices = shape.vertices,
            .indices = shape.indices,
            .textureId = texture.getResourceId(),
        });
    }

    void BaseGraphics::submit(const Shape& shape)
    {
        getRenderer().submit({
            .vertices = shape.vertices,
            .indices = shape.indices,
        });
    }

    float BaseGraphics::getNextDepth()
    {
        float value = m_currentDepth;
        m_currentDepth += DEPTH_INCREMENT;
        return value;
    }
} // namespace processing

namespace processing
{
    MainGraphics::MainGraphics(const rect2u& windowViewport, std::shared_ptr<Renderer> renderer) : m_renderTarget(windowViewport), m_renderer(std::move(renderer)), m_windowViewport(windowViewport)
    {
    }

    void MainGraphics::handle(const Event& event)
    {
        if (event.type == Event::framebuffer_resized)
        {
            // Update glViewport
            m_renderTarget.setViewport({0, 0, event.size.width, event.size.height});
        }

        if (event.type == Event::window_resized)
        {
            m_windowViewport = {0, 0, event.size.width, event.size.height};
        }
    }

    void MainGraphics::pause()
    {
        m_renderer->flush();
    }

    void MainGraphics::resume()
    {
        beginDraw();
    }

    void MainGraphics::beginDraw()
    {
        m_renderTarget.activate();

        const ProjectionDetails details = {
            .viewport = m_windowViewport,
            .projectionMatrix = matrix4x4_orthographic(m_windowViewport.left, m_windowViewport.top, m_windowViewport.width, m_windowViewport.height, MIN_DEPTH, MAX_DEPTH),
            .viewMatrix = matrix4x4_identity(),
        };

        m_renderer->beginDraw(details);
    }

    void MainGraphics::endDraw()
    {
        m_renderer->endDraw();
    }

    Renderer& MainGraphics::getRenderer()
    {
        return *m_renderer;
    }
} // namespace processing

namespace processing
{
    OffscreenGraphics::OffscreenGraphics(const uint2& size, std::shared_ptr<Renderer> renderer) : m_renderTarget(OffscreenRenderTarget::create(size)), m_renderer(std::move(renderer))
    {
    }

    void OffscreenGraphics::pause()
    {
    }

    void OffscreenGraphics::resume()
    {
        m_renderTarget->activate();
        const uint2 size = m_renderTarget->getSize();

        const ProjectionDetails details = {
            .viewport = rect2f{0.0f, 0.0f, static_cast<float>(size.x), static_cast<float>(size.y)},
            .projectionMatrix = matrix4x4_orthographic(0.0f, 0.0f, static_cast<float>(size.x), static_cast<float>(size.y), MIN_DEPTH, MAX_DEPTH),
            .viewMatrix = matrix4x4_identity(),
        };

        m_renderer->beginDraw(details);
    }

    void OffscreenGraphics::beginDraw()
    {
        m_renderTarget->activate();
    }

    void OffscreenGraphics::endDraw()
    {
        m_renderer->endDraw();
    }

    Renderer& OffscreenGraphics::getRenderer()
    {
        return *m_renderer;
    }
} // namespace processing
