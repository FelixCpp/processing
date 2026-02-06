#include <processing/processing.hpp>
#include <processing/shape_builder.hpp>

namespace processing
{
    static constexpr float4 float4_from_color(Color color)
    {
        return float4{
            static_cast<float>(color.r) / 255.0f,
            static_cast<float>(color.g) / 255.0f,
            static_cast<float>(color.b) / 255.0f,
            static_cast<float>(color.a) / 255.0f,
        };
    }

    Vertices vertices_from_contour(const Contour& contour, const matrix4x4& transform, Color color, float depth)
    {
        Vertices shape;
        shape.mode = VertexMode::triangles;
        shape.vertices.reserve(contour.positions.size());
        shape.indices.append_range(contour.indices);

        const float4 col = float4_from_color(color);

        for (size_t i = 0; i < contour.positions.size(); ++i)
        {
            shape.vertices.push_back(Vertex{
                .position = transform.transformPoint(float3{contour.positions[i], depth}),
                .texcoord = contour.texcoords[i],
                .color = col,
            });
        }

        return shape;
    }
} // namespace processing

namespace processing
{
    inline static constexpr rect2f convert_to_rect(const RectMode mode, f32 x1, f32 y1, f32 x2, f32 y2)
    {
        switch (mode)
        {
                // clang-format off
            case RectMode::cornerSize: return rect2f{ x1, y1, x2, y2 };
            case RectMode::corners: return rect2f{ x1, y1, x2 - x1, y2 - y1 };
            case RectMode::centerSize: return rect2f{ x1 - x2 * 0.5f, y1 - y2 * 0.5f, x2, y2 };
                // clang-format on
        }
    }

    inline static constexpr rect2f image_source_to_rect(const ImageSourceMode mode, f32 imgWidth, f32 imgHeight, f32 x1, f32 y1, f32 x2, f32 y2)
    {
        switch (mode)
        {
                // clang-format off
            case ImageSourceMode::normal: return rect2f{ x1, y1, x2, y2 };
            case ImageSourceMode::size:
            {
                const f32 u = x1 / imgWidth;
                const f32 v = 1.0f - (y1 + y2) / imgHeight;
                const f32 w = x2 / imgWidth;
                const f32 h = y2 / imgHeight;

                return rect2f{u, v, w, h};
            } break;
                // clang-format on
        }
    }

    inline static constexpr rect2f ellipse_to_rect(const EllipseMode mode, const f32 x1, const f32 y1, const f32 x2, const f32 y2)
    {
        switch (mode)
        {
                // clang-format off
            case EllipseMode::cornerDiameter: return rect2f{ x1, y1, x2, y2 };
            case EllipseMode::cornerRadius: return rect2f{ x1, y1, x2 * 2.0f, y2 * 2.0f };
            case EllipseMode::corners: return rect2f{ x1, y1, x2 - x1, y2 - y1 };
            case EllipseMode::centerRadius: return rect2f{ x1 - x2, y1 - y2, x2 * 2.0f, y2 * 2.0f };
            case EllipseMode::centerDiameter: return rect2f{ x1 - x2 * 0.5f, y1 - y2 * 0.5f, x2, y2 };
                // clang-format on
        }
    }

    inline static constexpr StrokeProperties get_stroke_properties(const RenderStyle& style)
    {
        return StrokeProperties{
            .strokeJoin = style.strokeJoin,
            .strokeWeight = style.strokeWeight,
            .miterLimit = 4.0f,
        };
    }
} // namespace processing

namespace processing
{
    Graphics::Graphics(std::shared_ptr<Renderer> renderer, Renderbuffer renderbuffer)
        : m_renderer(std::move(renderer)),
          m_renderbuffer(std::move(renderbuffer)),
          m_renderStyles({RenderStyle()}),
          m_metrics({matrix4x4::identity}),
          m_currentDepth{-1.0f}
    {
    }

    void Graphics::beginDraw()
    {
        m_currentDepth = -1.0f;
    }

    void Graphics::endDraw()
    {
    }

    void Graphics::push()
    {
        pushStyle();
        pushMatrix();
    }

    void Graphics::pop()
    {
        popStyle();
        popMatrix();
    }

    void Graphics::pushStyle(const bool extendPreviousStyle)
    {
        if (extendPreviousStyle)
        {
            m_renderStyles.push(peekStyle());
        }
        else
        {
            m_renderStyles.push(RenderStyle());
        }
    }

    void Graphics::popStyle()
    {
        if (m_renderStyles.size() > 1)
        {
            m_renderStyles.pop();
        }
    }

    RenderStyle& Graphics::peekStyle()
    {
        return m_renderStyles.top();
    }

    void Graphics::pushMatrix(const bool extendPreviousMatrix)
    {
        if (extendPreviousMatrix)
        {
            m_metrics.push(peekMatrix());
        }
        else
        {
            m_metrics.push(matrix4x4::identity);
        }
    }

    void Graphics::popMatrix()
    {
        if (m_metrics.size() > 1)
        {
            m_metrics.pop();
        }
    }

    void Graphics::resetMatrix()
    {
        peekMatrix() = matrix4x4::identity;
    }

    void Graphics::resetMatrix(const matrix4x4& matrix)
    {
        peekMatrix() = matrix;
    }

    matrix4x4& Graphics::peekMatrix()
    {
        return m_metrics.top();
    }

    void Graphics::translate(const f32 x, const f32 y)
    {
        resetMatrix(matrix4x4::translation(x, y).combined(peekMatrix()));
    }

    void Graphics::scale(const f32 x, const f32 y)
    {
        resetMatrix(matrix4x4::scaling(x, y).combined(peekMatrix()));
    }

    void Graphics::rotate(const f32 angle)
    {
        resetMatrix(matrix4x4::rotation(angle).combined(peekMatrix()));
    }

    void Graphics::blendMode(const BlendMode mode)
    {
        peekStyle().blendMode = mode;
    }

    void Graphics::angleMode(const AngleMode mode)
    {
        peekStyle().angleMode = mode;
    }

    void Graphics::rectMode(const RectMode mode)
    {
        peekStyle().rectMode = mode;
    }

    void Graphics::ellipseMode(const EllipseMode mode)
    {
        peekStyle().ellipseMode = mode;
    }

    void Graphics::imageMode(const RectMode mode)
    {
        peekStyle().imageMode = mode;
    }

    void Graphics::imageSourceMode(ImageSourceMode mode)
    {
        peekStyle().imageSourceMode = mode;
    }

    void Graphics::shader(const Shader& shader)
    {
        peekStyle().shader = shader;
    }

    void Graphics::noShader()
    {
        peekStyle().shader.reset();
    }

    void Graphics::fill(const i32 red, const i32 green, const i32 blue, const i32 alpha)
    {
        fill(Color(red, green, blue, alpha));
    }

    void Graphics::fill(const i32 grey, const i32 alpha)
    {
        fill(Color(grey, alpha));
    }

    void Graphics::fill(const Color color)
    {
        RenderStyle& style = peekStyle();
        style.fillColor = color;
        style.isFillEnabled = true;
    }

    void Graphics::noFill()
    {
        peekStyle().isFillEnabled = false;
    }

    void Graphics::stroke(const i32 red, const i32 green, const i32 blue, const i32 alpha)
    {
        stroke(Color(red, green, blue, alpha));
    }

    void Graphics::stroke(const i32 grey, const i32 alpha)
    {
        stroke(Color(grey, alpha));
    }

    void Graphics::stroke(const Color color)
    {
        RenderStyle& style = peekStyle();
        style.strokeColor = color;
        style.isStrokeEnabled = true;
    }

    void Graphics::noStroke()
    {
        peekStyle().isStrokeEnabled = false;
    }

    void Graphics::strokeWeight(const f32 strokeWeight)
    {
        peekStyle().strokeWeight = strokeWeight;
    }

    void Graphics::strokeCap(const StrokeCap strokeCap)
    {
        peekStyle().strokeCap = strokeCap;
    }

    void Graphics::strokeJoin(const StrokeJoin strokeJoin)
    {
        peekStyle().strokeJoin = strokeJoin;
    }

    void Graphics::tint(const i32 red, const i32 green, const i32 blue, const i32 alpha)
    {
        tint(Color(red, green, blue, alpha));
    }

    void Graphics::tint(const i32 grey, const i32 alpha)
    {
        tint(Color(grey, alpha));
    }

    void Graphics::tint(const Color color)
    {
        peekStyle().tintColor = color;
    }

    void Graphics::background(i32 red, i32 green, i32 blue, i32 alpha)
    {
        background(Color(red, green, blue, alpha));
    }

    void Graphics::background(i32 grey, i32 alpha)
    {
        background(Color(grey, alpha));
    }

    void Graphics::background(Color color)
    {
        const float2 size = float2{m_renderbuffer.getSize()};

        const RenderStyle& style = peekStyle();
        const RectPath path = path_rect(rect2f{0.0f, 0.0f, size.x, size.y});
        const Contour contour = contour_rect_fill(path);
        const Vertices vertices = vertices_from_contour(contour, matrix4x4::identity, color, getNextDepth());

        m_renderer->render(
            vertices,
            {
                .blendMode = style.blendMode,
                .renderbuffer = m_renderbuffer,
                .transform = matrix4x4::orthographic(0.0f, 0.0f, size.x, size.y, -1.0f, 1.0f),
            }
        );
    }

    void Graphics::beginShape()
    {
    }

    void Graphics::endShape()
    {
    }

    void Graphics::vertex(f32 x, f32 y)
    {
    }

    void Graphics::vertex(f32 x, f32 y, f32 u, f32 v)
    {
    }

    void Graphics::bezierVertex(f32 x2, f32 y2, f32 x3, f32 y3)
    {
    }

    void Graphics::quadraticVertex(f32 cx, f32 cy, f32 x3, f32 y3)
    {
    }

    void Graphics::curveVertex(f32 x, f32 y)
    {
    }

    void Graphics::rect(f32 x1, f32 y1, f32 x2, f32 y2)
    {
        const RenderStyle& style = peekStyle();
        const matrix4x4& matrix = peekMatrix();
        const rect2f boundary = convert_to_rect(style.rectMode, x1, y1, x2, y2);
        const RectPath path = path_rect(boundary);

        if (style.isFillEnabled)
        {
            const Contour contour = contour_rect_fill(path);
            const Vertices vertices = vertices_from_contour(contour, matrix, style.fillColor, getNextDepth());
            m_renderer->render(vertices, getRenderState(style));
        }

        // if (style.isStrokeEnabled)
        // {
        //     const Contour contour = contour_rect_stroke(path, get_stroke_properties(style));
        //     const Vertices vertices = vertices_from_contour(contour, matrix, style.strokeColor, getNextDepth());
        //     m_renderer->render(vertices, getRenderState(style));
        // }
    }

    void Graphics::square(f32 x1, f32 y1, f32 xy2)
    {
        rect(x1, y1, xy2, xy2);
    }

    void Graphics::ellipse(f32 x1, f32 y1, f32 x2, f32 y2)
    {
        const RenderStyle& style = peekStyle();
        const matrix4x4& matrix = peekMatrix();
        const rect2f boundary = ellipse_to_rect(style.ellipseMode, x1, y1, x2, y2);
        const EllipsePath path = path_ellipse({
            .center = boundary.center(),
            .radius = Radius{
                .x = boundary.width * 0.5f,
                .y = boundary.height * 0.5f,
            },
            .segments = 32,
        });

        if (style.isFillEnabled)
        {
            const Contour contour = contour_ellipse_fill(path);
            const Vertices vertices = vertices_from_contour(contour, matrix, style.fillColor, getNextDepth());
            m_renderer->render(vertices, getRenderState(style));
        }

        // if (style.isStrokeEnabled)
        // {
        //     const Contour contour = contour_ellipse_stroke(path, get_stroke_properties(style));
        //     const Vertices vertices = vertices_from_contour(contour, matrix, style.strokeColor, getNextDepth());
        //     m_renderer->render(vertices, getRenderState(style));
        // }
    }

    void Graphics::circle(f32 x1, f32 y1, f32 xy2)
    {
        ellipse(x1, y1, xy2, xy2);
    }

    void Graphics::triangle(f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3)
    {
        const RenderStyle& style = peekStyle();
        const matrix4x4& matrix = peekMatrix();
        const rect2f boundary = ellipse_to_rect(style.ellipseMode, x1, y1, x2, y2);
        const TrianglePath path = path_triangle({
            .a = {x1, y1},
            .b = {x2, y2},
            .c = {x3, y3},
        });

        if (style.isFillEnabled)
        {
            const Contour contour = contour_triangle_fill(path);
            const Vertices vertices = vertices_from_contour(contour, matrix, style.fillColor, getNextDepth());
            m_renderer->render(vertices, getRenderState(style));
        }

        if (style.isStrokeEnabled)
        {
            const Contour contour = contour_triangle_stroke(path, get_stroke_properties(style));
            const Vertices vertices = vertices_from_contour(contour, matrix, style.strokeColor, getNextDepth());
            m_renderer->render(vertices, getRenderState(style));
        }
    }

    void Graphics::point(f32 x, f32 y)
    {
        const RenderStyle& style = peekStyle();
        const matrix4x4& matrix = peekMatrix();
        const rect2f boundary = ellipse_to_rect(EllipseMode::centerDiameter, x, y, style.strokeWeight, style.strokeWeight);
        const EllipsePath path = path_ellipse({
            .center = boundary.center(),
            .radius = Radius{
                .x = boundary.width * 0.5f,
                .y = boundary.height * 0.5f,
            },
            .segments = 32,
        });

        const Contour contour = contour_ellipse_fill(path);
        const Vertices vertices = vertices_from_contour(contour, matrix, style.strokeColor, getNextDepth());
        m_renderer->render(vertices, getRenderState(style));
    }

    void Graphics::line(f32 x1, f32 y1, f32 x2, f32 y2)
    {
        const RenderStyle& style = peekStyle();
        const matrix4x4& matrix = peekMatrix();
        const rect2f boundary = ellipse_to_rect(style.ellipseMode, x1, y1, x2, y2);

        const Contour contour = contour_line(x1, y1, x2, y2, style.strokeWeight, style.strokeCap);
        const Vertices vertices = vertices_from_contour(contour, matrix, style.strokeColor, getNextDepth());
        m_renderer->render(vertices, getRenderState(style));
    }

    void Graphics::image(const Image& img, f32 x1, f32 y1)
    {
        const auto [imgWidth, imgHeight] = img.getSize();
        image(img, x1, y1, static_cast<f32>(imgWidth), static_cast<f32>(imgHeight));
    }

    void Graphics::image(const Image& img, f32 x1, f32 y1, f32 x2, f32 y2)
    {
        const RenderStyle& style = peekStyle();
        const matrix4x4& matrix = peekMatrix();
        const rect2f boundary = convert_to_rect(style.imageMode, x1, y1, x2, y2);

        const Contour contour = contour_image(boundary.left, boundary.top, boundary.width, boundary.height, 0.0f, 0.0f, 1.0f, 1.0f);
        const Vertices vertices = vertices_from_contour(contour, matrix, style.tintColor, getNextDepth());
        m_renderer->render(vertices, getRenderState(style, img));
    }

    void Graphics::image(const Image& img, f32 x1, f32 y1, f32 x2, f32 y2, f32 sx1, f32 sy1, f32 sx2, f32 sy2)
    {
        const auto [imgWidth, imgHeight] = img.getSize();
        const RenderStyle& style = peekStyle();
        const matrix4x4& matrix = peekMatrix();
        const rect2f boundary = convert_to_rect(style.imageMode, x1, y1, x2, y2);
        const rect2f source = image_source_to_rect(style.imageSourceMode, static_cast<f32>(imgWidth), static_cast<f32>(imgHeight), sx1, sy1, sx2, sy2);

        const Contour contour = contour_image(boundary.left, boundary.top, boundary.width, boundary.height, source.left, source.top, source.width, source.height);
        const Vertices vertices = vertices_from_contour(contour, matrix, style.tintColor, getNextDepth());
        m_renderer->render(vertices, getRenderState(style, img));
    }

    Image& Graphics::getImage()
    {
        return m_renderbuffer.getImage();
    }

    f32 Graphics::getNextDepth()
    {
        const f32 depth = m_currentDepth;
        m_currentDepth += 1.0f / 20'000.0f;
        return depth;
    }

    RenderState Graphics::getRenderState(const RenderStyle& style, const std::optional<Image>& image)
    {
        return RenderState{
            .blendMode = style.blendMode,
            .renderbuffer = m_renderbuffer,
            .shader = style.shader,
            .image = image,
            .transform = matrix4x4::orthographic(0.0f, 0.0f, m_renderbuffer.getSize().x, m_renderbuffer.getSize().y, -1.0f, 1.0f),
        };
    }

} // namespace processing
