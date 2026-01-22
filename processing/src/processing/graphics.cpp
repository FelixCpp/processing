#include "processing/matrix_stack.hpp"
#include <processing/graphics.hpp>
#include <processing/shape_builder.hpp>
#include <processing/batch_renderer.hpp>

namespace processing
{
    inline static constexpr float MIN_DEPTH = -1.0f;
    inline static constexpr float MAX_DEPTH = 1.0f;
    inline static constexpr float DEPTH_INCREMENT = (MAX_DEPTH - MIN_DEPTH) / 20'000.0f;

    Graphics::Graphics(const uint2 size) : m_renderTarget(std::make_unique<MainRenderTarget>(rect2u{0, 0, size.x, size.y})), m_renderer(BatchRenderer::create()), m_renderStyles(render_style_stack_create()), m_windowSize(size), m_metrics(matrix_stack_create())
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
        matrix_stack_reset(m_metrics);
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

    void Graphics::pushMatrix()
    {
        matrix_stack_push(m_metrics, peekMatrix());
    }

    void Graphics::popMatrix()
    {
        matrix_stack_pop(m_metrics);
    }

    void Graphics::resetMatrix()
    {
        matrix_stack_reset(m_metrics);
    }

    matrix4x4& Graphics::peekMatrix()
    {
        return matrix_stack_peek(m_metrics);
    }

    void Graphics::translate(const float x, const float y)
    {
        matrix4x4& matrix = peekMatrix();
        matrix = matrix4x4_multiply(matrix4x4_translate(x, y, 0.0f), matrix);
    }

    void Graphics::scale(const float x, const float y)
    {
        matrix4x4& matrix = peekMatrix();
        matrix = matrix4x4_multiply(matrix4x4_scale(x, y, 1.0f), matrix);
    }

    void Graphics::rotate(const float angle)
    {
        matrix4x4& matrix = peekMatrix();
        matrix = matrix4x4_multiply(matrix4x4_rotation_z(angle), matrix);
    }

    void Graphics::blendMode(const BlendMode& blendMode)
    {
        RenderStyle& style = peekState();
        style.blendMode = blendMode;
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
        const Contour rect_contour = contour_quad_fill(
            viewport.left, viewport.top,
            viewport.left + viewport.width, viewport.top,
            viewport.left + viewport.width, viewport.top + viewport.height,
            viewport.left, viewport.top + viewport.height
        );

        const Shape shape = shape_from_contour(rect_contour, matrix4x4_identity(), color, getNextDepth());

        m_renderer->submit({
            .vertices = shape.vertices,
            .indices = shape.indices,
            .blendMode = BlendMode::alpha,
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

    void Graphics::imageMode(RectMode imageMode)
    {
        RenderStyle& style = peekState();
        style.imageMode = imageMode;
    }

    void Graphics::imageSourceMode(ImageSourceMode imageSourceMode)
    {
        RenderStyle& style = peekState();
        style.imageSourceMode = imageSourceMode;
    }

    void Graphics::Graphics::imageTint(int red, int green, int blue, int alpha)
    {
        imageTint(color(red, green, blue, alpha));
    }

    void Graphics::Graphics::imageTint(int grey, int alpha)
    {
        imageTint(color(grey, alpha));
    }

    void Graphics::Graphics::imageTint(color_t color)
    {
        RenderStyle& style = peekState();
        style.imageTint = color;
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

    void Graphics::ellipseMode(EllipseMode ellipseMode)
    {
        RenderStyle& style = peekState();
        style.ellipseMode = ellipseMode;
    }

    void Graphics::rect(float x1, float y1, float x2, float y2)
    {
        const RenderStyle& style = peekState();
        const matrix4x4& matrix = peekMatrix();
        const rect2f boundary = style.rectMode(x1, y1, x2, y2);

        if (style.isFillEnabled)
        {
            const Contour contour = contour_rect_fill(boundary.left, boundary.top, boundary.width, boundary.height);
            const Shape shape = shape_from_contour(contour, matrix, style.fillColor, getNextDepth());

            m_renderer->submit({
                .vertices = shape.vertices,
                .indices = shape.indices,
                .blendMode = style.blendMode,
            });
        }

        if (style.isStrokeEnabled)
        {
            const Contour contour = contour_rect_stroke(boundary.left, boundary.top, boundary.width, boundary.height, style.strokeWeight, style.strokeJoin);
            const Shape shape = shape_from_contour(contour, matrix, style.strokeColor, getNextDepth());

            m_renderer->submit({
                .vertices = shape.vertices,
                .indices = shape.indices,
                .blendMode = style.blendMode,
            });
        }
    }

    void Graphics::square(float left, float top, float size)
    {
        rect(left, top, size, size);
    }

    void Graphics::ellipse(float x1, float y1, float x2, float y2)
    {
        const RenderStyle& style = peekState();
        const matrix4x4& matrix = peekMatrix();
        const rect2f boundary = style.ellipseMode(x1, y1, x2, y2);
        const float2 center = boundary.center();

        if (style.isFillEnabled)
        {
            const Contour contour = contour_ellipse_fill(center.x, center.y, boundary.width * 0.5f, boundary.height * 0.5f, 32);
            const Shape shape = shape_from_contour(contour, matrix, style.fillColor, getNextDepth());

            m_renderer->submit({
                .vertices = shape.vertices,
                .indices = shape.indices,
                .blendMode = style.blendMode,
            });
        }

        if (style.isStrokeEnabled)
        {
            const Contour contour = contour_ellipse_stroke(center.x, center.y, boundary.width * 0.5f, boundary.height * 0.5f, style.strokeWeight, 32, style.strokeJoin);
            const Shape shape = shape_from_contour(contour, matrix, style.strokeColor, getNextDepth());

            m_renderer->submit({
                .vertices = shape.vertices,
                .indices = shape.indices,
                .blendMode = style.blendMode,
            });
        }
    }

    void Graphics::circle(float centerX, float centerY, float radius)
    {
        ellipse(centerX, centerY, radius, radius);
    }

    void Graphics::line(float x1, float y1, float x2, float y2)
    {
        const RenderStyle& style = peekState();
        const matrix4x4& matrix = peekMatrix();
        const Contour contour = contour_line(x1, y1, x2, y2, style.strokeWeight, style.strokeCap);
        const Shape shape = shape_from_contour(contour, matrix, style.strokeColor, getNextDepth());

        m_renderer->submit({
            .vertices = shape.vertices,
            .indices = shape.indices,
            .blendMode = style.blendMode,
        });
    }

    void Graphics::triangle(float x1, float y1, float x2, float y2, float x3, float y3)
    {
        const RenderStyle& style = peekState();
        const matrix4x4& matrix = peekMatrix();

        if (style.isFillEnabled)
        {
            const Contour contour = contour_triangle_fill(x1, y1, x2, y2, x3, y3);
            const Shape shape = shape_from_contour(contour, matrix, style.fillColor, getNextDepth());

            m_renderer->submit({
                .vertices = shape.vertices,
                .indices = shape.indices,
                .blendMode = style.blendMode,
            });
        }

        if (style.isStrokeEnabled)
        {
            const Contour contour = contour_triangle_stroke(x1, y1, x2, y2, x3, y3, style.strokeWeight, style.strokeJoin);
            const Shape shape = shape_from_contour(contour, matrix, style.strokeColor, getNextDepth());

            m_renderer->submit({
                .vertices = shape.vertices,
                .indices = shape.indices,
                .blendMode = style.blendMode,
            });
        }
    }

    void Graphics::point(float x, float y)
    {
        const RenderStyle& style = peekState();
        const matrix4x4& matrix = peekMatrix();
        const Contour contour = contour_ellipse_fill(x, y, style.strokeWeight, style.strokeWeight, 16);
        const Shape shape = shape_from_contour(contour, matrix, style.strokeColor, getNextDepth());

        m_renderer->submit({
            .vertices = shape.vertices,
            .indices = shape.indices,
            .blendMode = style.blendMode,
        });
    }

    void Graphics::image(const Texture& texture, float x1, float y1)
    {
        const auto textureSize = float2{texture.getSize()};
        image(texture, x1, y1, textureSize.x, textureSize.y);
    }

    void Graphics::image(const Texture& texture, float x1, float y1, float x2, float y2)
    {
        const RenderStyle& style = peekState();
        const matrix4x4& matrix = peekMatrix();
        const rect2f boundary = style.imageMode(x1, y1, x2, y2);
        const Contour contour = contour_image(boundary.left, boundary.top, boundary.width, boundary.height, 0.0f, 0.0f, 1.0f, 1.0f);
        const Shape shape = shape_from_contour(contour, matrix, style.imageTint, getNextDepth());

        m_renderer->submit({
            .vertices = shape.vertices,
            .indices = shape.indices,
            .textureId = texture.getResourceId(),
            .blendMode = style.blendMode,
        });
    }

    void Graphics::image(const Texture& texture, float x1, float y1, float x2, float y2, float sx1, float sy1, float sx2, float sy2)
    {
        const RenderStyle& style = peekState();
        const matrix4x4& matrix = peekMatrix();
        const rect2f boundary = style.imageMode(x1, y1, x2, y2);
        const rect2f sourceRect = style.imageSourceMode(texture.getSize(), sx1, sy1, sx2, sy2);
        const Contour contour = contour_image(boundary.left, boundary.top, boundary.width, boundary.height, sourceRect.left, sourceRect.top, sourceRect.width, sourceRect.height);
        const Shape shape = shape_from_contour(contour, matrix, style.imageTint, getNextDepth());

        m_renderer->submit({
            .vertices = shape.vertices,
            .indices = shape.indices,
            .textureId = texture.getResourceId(),
            .blendMode = style.blendMode,
        });
    }

    float Graphics::getNextDepth()
    {
        float value = m_currentDepth;
        m_currentDepth += DEPTH_INCREMENT;
        return value;
    }
} // namespace processing
