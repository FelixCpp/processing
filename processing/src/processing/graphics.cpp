#include <processing/graphics.hpp>
#include <processing/shape_builder.hpp>
#include <processing/default_renderer.hpp>

namespace processing
{
    constexpr StrokeProperties get_stroke_properties(const RenderStyle& style)
    {
        return {
            .strokeJoin = style.strokeJoin,
            .strokeWeight = style.strokeWeight,
            .miterLimit = 4.0f,
        };
    }
} // namespace processing

namespace processing
{
    inline static constexpr float MIN_DEPTH = -1.0f;
    inline static constexpr float MAX_DEPTH = 1.0f;
    inline static constexpr float DEPTH_INCREMENT = (MAX_DEPTH - MIN_DEPTH) / 20'000.0f;

    Graphics::Graphics(const uint2 size, ShaderAssetManager& shaderAssetManager, RenderbufferManager& renderTargetManager, TextureAssetManager& textureAssetManager)
        : m_renderer(DefaultRenderer::create(shaderAssetManager, textureAssetManager)),
          m_depthProvider(MIN_DEPTH, MAX_DEPTH, DEPTH_INCREMENT),
          m_renderTargetManager(&renderTargetManager),
          m_textureAssetManager(&textureAssetManager),
          m_renderStyles(render_style_stack_create()),
          m_mainRenderbuffer(MainRenderbuffer(rect2u{0, 0, size.x, size.y})),
          m_tmpDepthProvider(DepthProvider(MIN_DEPTH, MAX_DEPTH, DEPTH_INCREMENT)),
          m_windowSize(size)
    {
    }

    void Graphics::event(const Event& event)
    {
        if (event.type == Event::framebuffer_resized)
        {
            m_mainRenderbuffer.setViewport(rect2u{0, 0, event.size.width, event.size.height});
        }

        if (event.type == Event::window_resized)
        {
            m_windowSize = {event.size.width, event.size.height};
        }
    }

    void Graphics::beginDraw()
    {
        m_tmpRenderbuffer.reset();
        m_depthProvider.reset();
        m_renderer->beginDraw(RenderingDetails{
            .renderbufferResourceId = m_mainRenderbuffer.getResourceId(),
            .renderbufferViewport = m_mainRenderbuffer.getViewport(),
            .projectionMatrix = matrix4x4_orthographic(0.0f, 0.0f, static_cast<float>(m_windowSize.x), static_cast<float>(m_windowSize.y), MIN_DEPTH, MAX_DEPTH),
        });

        render_style_stack_reset(m_renderStyles);
    }

    void Graphics::endDraw()
    {
        m_renderer->endDraw();
    }

    rect2f Graphics::getViewport() const
    {
        if (m_tmpRenderbuffer.has_value())
        {
            return rect2f{m_tmpRenderbuffer->getViewport()};
        }

        return rect2f{0.0f, 0.0f, static_cast<float>(m_windowSize.x), static_cast<float>(m_windowSize.y)};
    }

    void Graphics::renderbuffer(Renderbuffer renderbuffer)
    {
        m_tmpDepthProvider.reset();
        m_tmpRenderbuffer = renderbuffer;

        const rect2u& viewport = renderbuffer.getViewport();

        m_renderer->activate(RenderingDetails{
            .renderbufferResourceId = renderbuffer.getResourceId(),
            .renderbufferViewport = renderbuffer.getViewport(),
            .projectionMatrix = matrix4x4_orthographic(static_cast<float>(viewport.left), static_cast<float>(viewport.top), static_cast<float>(viewport.width), static_cast<float>(viewport.height), MIN_DEPTH, MAX_DEPTH),
        });
    }

    void Graphics::noRenderbuffer()
    {
        if (m_tmpRenderbuffer.has_value())
        {
            m_renderer->flush(); // Flush the offscreen layer
            m_renderer->beginDraw(RenderingDetails{
                .renderbufferResourceId = m_mainRenderbuffer.getResourceId(),
                .renderbufferViewport = m_mainRenderbuffer.getViewport(),
                .projectionMatrix = matrix4x4_orthographic(0.0f, 0.0f, static_cast<float>(m_windowSize.x), static_cast<float>(m_windowSize.y), MIN_DEPTH, MAX_DEPTH),
            });
        }
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
        RenderStyle& style = peekState();
        matrix_stack_push(style.matrixStack, peekMatrix());
    }

    void Graphics::popMatrix()
    {
        RenderStyle& style = peekState();
        matrix_stack_pop(style.matrixStack);
    }

    void Graphics::resetMatrix()
    {
        RenderStyle& style = peekState();
        matrix_stack_reset(style.matrixStack);
    }

    matrix4x4& Graphics::peekMatrix()
    {
        return matrix_stack_peek(peekState().matrixStack);
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

    void Graphics::shader(const Shader& shaderProgram)
    {
        RenderStyle& style = peekState();
        style.shaderResourceId = shaderProgram.getResourceId();
    }

    void Graphics::noShader()
    {
        RenderStyle& style = peekState();
        style.shaderResourceId = std::nullopt;
    }

    void Graphics::background(const Texture& texture)
    {
        const RenderStyle& style = peekState();
        const rect2f viewport = getViewport();
        const Contour rect_contour = contour_image(
            viewport.left, viewport.top, viewport.width, viewport.height,
            0.0f, 0.0f, 1.0f, 1.0f
        );

        const Shape shape = shape_from_contour(rect_contour, matrix4x4_identity(), style.imageTint, getNextDepth());

        submit({
            .vertices = shape.vertices,
            .indices = shape.indices,
            .shaderResourceId = style.shaderResourceId,
            .textureResourceId = texture.getResourceId(),
            .blendMode = BlendMode::alpha,
        });
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
        const RenderStyle& style = peekState();
        const rect2f viewport = getViewport();
        const Contour rect_contour = contour_quad_fill(
            viewport.left, viewport.top,
            viewport.left + viewport.width, viewport.top,
            viewport.left + viewport.width, viewport.top + viewport.height,
            viewport.left, viewport.top + viewport.height
        );

        const Shape shape = shape_from_contour(rect_contour, matrix4x4_identity(), color, getNextDepth());

        submit({
            .vertices = shape.vertices,
            .indices = shape.indices,
            .shaderResourceId = style.shaderResourceId,
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

        const RectPath path = path_rect(boundary);

        if (style.isFillEnabled)
        {
            const Contour contour = contour_rect_fill(path);
            const Shape shape = shape_from_contour(contour, matrix, style.fillColor, getNextDepth());

            submit({
                .vertices = shape.vertices,
                .indices = shape.indices,
                .shaderResourceId = style.shaderResourceId,
                .blendMode = style.blendMode,
            });
        }

        if (style.isStrokeEnabled)
        {
            const Contour contour = contour_rect_stroke(path, get_stroke_properties(style));
            const Shape shape = shape_from_contour(contour, matrix, style.strokeColor, getNextDepth());

            submit({
                .vertices = shape.vertices,
                .indices = shape.indices,
                .shaderResourceId = style.shaderResourceId,
                .blendMode = style.blendMode,
            });
        }
    }

    void Graphics::rect(float left, float top, float width, float height, float cornerRadius)
    {
        rect(left, top, width, height, cornerRadius, cornerRadius, cornerRadius, cornerRadius);
    }

    void Graphics::rect(float left, float top, float width, float height, float cornerRadiusTopLeft, float cornerRadiusTopRight, float cornerRadiusBottomRight, float cornerRadiusBottomLeft)
    {
        rect(left, top, width, height, cornerRadiusTopLeft, cornerRadiusTopLeft, cornerRadiusTopRight, cornerRadiusTopRight, cornerRadiusBottomRight, cornerRadiusBottomRight, cornerRadiusBottomLeft, cornerRadiusBottomLeft);
    }

    void Graphics::rect(float left, float top, float width, float height, float cornerRadiusTopLeftX, float cornerRadiusTopLeftY, float cornerRadiusTopRightX, float cornerRadiusTopRightY, float cornerRadiusBottomRightX, float cornerRadiusBottomRightY, float cornerRadiusBottomLeftX, float cornerRadiusBottomLeftY)
    {
        const RenderStyle& style = peekState();
        const matrix4x4& matrix = peekMatrix();
        const rect2f boundary = style.rectMode(left, top, width, height);
        const RoundedRectPath path = path_rounded_rect({
            .boundary = {left, top, width, height},
            .topLeft = {cornerRadiusTopLeftX, cornerRadiusTopLeftY},
            .topRight = {cornerRadiusTopRightX, cornerRadiusTopRightY},
            .bottomRight = {cornerRadiusBottomRightX, cornerRadiusBottomRightY},
            .bottomLeft = {cornerRadiusBottomLeftX, cornerRadiusBottomLeftY},
        });

        if (style.isFillEnabled)
        {
            const Contour contour = contour_rounded_rect_fill(path);
            const Shape shape = shape_from_contour(contour, matrix, style.fillColor, getNextDepth());

            submit({
                .vertices = shape.vertices,
                .indices = shape.indices,
                .shaderResourceId = style.shaderResourceId,
                .blendMode = style.blendMode,
            });
        }

        if (style.isStrokeEnabled)
        {
            const Contour contour = contour_rounded_rect_stroke(path, get_stroke_properties(style));
            const Shape shape = shape_from_contour(contour, matrix, style.strokeColor, getNextDepth());

            submit({
                .vertices = shape.vertices,
                .indices = shape.indices,
                .shaderResourceId = style.shaderResourceId,
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

        const EllipsePath path = path_ellipse({
            .center = boundary.center(),
            .radius = Radius::elliptical(boundary.width * 0.5f, boundary.height * 0.5f),
            .segments = 32,
        });

        if (style.isFillEnabled)
        {
            const Contour contour = contour_ellipse_fill(path);
            const Shape shape = shape_from_contour(contour, matrix, style.fillColor, getNextDepth());

            submit({
                .vertices = shape.vertices,
                .indices = shape.indices,
                .shaderResourceId = style.shaderResourceId,
                .blendMode = style.blendMode,
            });
        }

        if (style.isStrokeEnabled)
        {
            const Contour contour = contour_ellipse_stroke(path, get_stroke_properties(style));
            const Shape shape = shape_from_contour(contour, matrix, style.strokeColor, getNextDepth());

            submit({
                .vertices = shape.vertices,
                .indices = shape.indices,
                .shaderResourceId = style.shaderResourceId,
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

        submit({
            .vertices = shape.vertices,
            .indices = shape.indices,
            .shaderResourceId = style.shaderResourceId,
            .blendMode = style.blendMode,
        });
    }

    void Graphics::triangle(float x1, float y1, float x2, float y2, float x3, float y3)
    {
        const RenderStyle& style = peekState();
        const matrix4x4& matrix = peekMatrix();

        const TrianglePath path = path_triangle({
            .a = float2{x1, y1},
            .b = float2{x2, y2},
            .c = float2{x3, y3},
        });

        if (style.isFillEnabled)
        {
            const Contour contour = contour_triangle_fill(path);
            const Shape shape = shape_from_contour(contour, matrix, style.fillColor, getNextDepth());

            submit({
                .vertices = shape.vertices,
                .indices = shape.indices,
                .shaderResourceId = style.shaderResourceId,
                .blendMode = style.blendMode,
            });
        }

        if (style.isStrokeEnabled)
        {
            const Contour contour = contour_triangle_stroke(path, get_stroke_properties(style));
            const Shape shape = shape_from_contour(contour, matrix, style.strokeColor, getNextDepth());

            submit({
                .vertices = shape.vertices,
                .indices = shape.indices,
                .shaderResourceId = style.shaderResourceId,
                .blendMode = style.blendMode,
            });
        }
    }

    void Graphics::point(float x, float y)
    {
        const RenderStyle& style = peekState();
        const matrix4x4& matrix = peekMatrix();
        const EllipsePath path = path_ellipse({
            .center = float2(x, y),
            .radius = Radius::circular(style.strokeWeight),
            .segments = 16,
        });

        const Contour contour = contour_ellipse_fill(path);
        const Shape shape = shape_from_contour(contour, matrix, style.strokeColor, getNextDepth());

        submit({
            .vertices = shape.vertices,
            .indices = shape.indices,
            .shaderResourceId = style.shaderResourceId,
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

        submit({
            .vertices = shape.vertices,
            .indices = shape.indices,
            .shaderResourceId = style.shaderResourceId,
            .textureResourceId = texture.getResourceId(),
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

        submit({
            .vertices = shape.vertices,
            .indices = shape.indices,
            .shaderResourceId = style.shaderResourceId,
            .textureResourceId = texture.getResourceId(),
            .blendMode = style.blendMode,
        });
    }

    float Graphics::getNextDepth()
    {
        return m_depthProvider.getNextDepth();
    }

    void Graphics::submit(const RenderingSubmission& submission)
    {
        m_renderer->submit(submission);
    }
} // namespace processing
