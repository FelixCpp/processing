#include <processing/graphics.hpp>
#include <processing/shape_builder.hpp>

#include <glad/gl.h>

namespace processing
{
    inline static std::unique_ptr<GraphicsData> s_graphics;
    inline static constexpr f32 MIN_DEPTH = -1.0f;
    inline static constexpr f32 MAX_DEPTH = 1.0f;
    inline static constexpr f32 DEPTH_INCREMENT = 1.0f / 20'000.0f;

    void blit(u32 width, u32 height, const Renderbuffer& rb)
    {
        glViewport(0, 0, width, height);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, rb.getResourceId().value);
        glBlitFramebuffer(0, 0, rb.getSize().x, rb.getSize().y, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    }

    GraphicsLayer createGraphicsLayer(const Renderbuffer& renderbuffer)
    {
        return GraphicsLayer{
            .currentDepth = MIN_DEPTH,
            .renderStyles = NeverEmptyStack<RenderStyle>{RenderStyle()},
            .metrics = NeverEmptyStack<matrix4x4>{matrix4x4::identity},
            .renderbuffer = renderbuffer,
        };
    }

    inline GraphicsLayer& peekLayer()
    {
        return s_graphics->layers.peek();
    }
} // namespace processing

namespace processing
{
    void initGraphics(const u32 width, const u32 height)
    {
        s_graphics = std::unique_ptr<GraphicsData>{
            new GraphicsData{
                .layers = NeverEmptyStack<GraphicsLayer>{createGraphicsLayer(createRenderbuffer(width, height))},
                .renderer = DefaultRenderer::create(),
            },
        };
    }

    void beginDraw()
    {
        // TODO(Felix): Maybe detect some unreleased resources (push without pop for styles, matrics shaders or renderbuffers)?
        GraphicsLayer& layer = peekLayer();
        layer.currentDepth = MIN_DEPTH;
        s_graphics->renderer->beginDraw(layer.renderbuffer);
    }

    void endDraw(u32 width, u32 height)
    {
        s_graphics->renderer->endDraw();
        blit(width, height, s_graphics->layers.peek().renderbuffer);
    }
} // namespace processing

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
            case ImageSourceMode::normal:
            {
                const f32 u = x1;
                const f32 v = 1.0f - (y1 + y2);
                const f32 w = x2;
                const f32 h = y2;
                return rect2f{ u, v, w, h };
            }
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
    f32 getNextDepth(GraphicsLayer& layer)
    {
        const f32 depth = layer.currentDepth;
        layer.currentDepth += 1.0f / 20'000.0f;
        return depth;
    }

    RenderState getRenderState(const RenderStyle& style, const GraphicsLayer& layer, const std::optional<Image>& image = std::nullopt)
    {
        return RenderState{
            .blendMode = style.blendMode,
            .shader = style.shader,
            .image = image,
            .transform = matrix4x4::orthographic(0.0f, 0.0f, layer.renderbuffer.getSize().x, layer.renderbuffer.getSize().y, -1.0f, 1.0f),
        };
    }
} // namespace processing

namespace processing
{
    void push()
    {
        pushStyle();
        pushMatrix();
    }

    void pop()
    {
        popStyle();
        popMatrix();
    }

    void pushStyle(const bool extendPreviousStyle)
    {
        if (extendPreviousStyle)
        {
            peekLayer().renderStyles.push(peekStyle());
        }
        else
        {
            peekLayer().renderStyles.push(RenderStyle());
        }
    }

    void popStyle()
    {
        peekLayer().renderStyles.pop();
    }

    RenderStyle& peekStyle()
    {
        return peekLayer().renderStyles.peek();
    }

    void pushMatrix(const bool extendPreviousMatrix)
    {
        if (extendPreviousMatrix)
        {
            peekLayer().metrics.push(peekMatrix());
        }
        else
        {
            peekLayer().metrics.push(matrix4x4::identity);
        }
    }

    void popMatrix()
    {
        peekLayer().metrics.pop();
    }

    void resetMatrix()
    {
        peekMatrix() = matrix4x4::identity;
    }

    void resetMatrix(const matrix4x4& matrix)
    {
        peekMatrix() = matrix;
    }

    matrix4x4& peekMatrix()
    {
        return peekLayer().metrics.peek();
    }

    void translate(const f32 x, const f32 y)
    {
        resetMatrix(matrix4x4::translation(x, y).combined(peekMatrix()));
    }

    void scale(const f32 x, const f32 y)
    {
        resetMatrix(matrix4x4::scaling(x, y).combined(peekMatrix()));
    }

    void rotate(const f32 angle)
    {
        resetMatrix(matrix4x4::rotation(angle).combined(peekMatrix()));
    }

    void blendMode(const BlendMode mode)
    {
        peekStyle().blendMode = mode;
    }

    void angleMode(const AngleMode mode)
    {
        peekStyle().angleMode = mode;
    }

    void rectMode(const RectMode mode)
    {
        peekStyle().rectMode = mode;
    }

    void ellipseMode(const EllipseMode mode)
    {
        peekStyle().ellipseMode = mode;
    }

    void imageMode(const RectMode mode)
    {
        peekStyle().imageMode = mode;
    }

    void imageSourceMode(ImageSourceMode mode)
    {
        peekStyle().imageSourceMode = mode;
    }

    void renderbuffer(const Renderbuffer& renderbuffer)
    {
        // Flush the current render state
        s_graphics->renderer->endDraw();

        // Push a new graphics layer onto the stack and activate it
        s_graphics->layers.push(createGraphicsLayer(renderbuffer));
        s_graphics->renderer->beginDraw(renderbuffer);
    }

    void noRenderbuffer()
    {
        // Flush the rendering state & pop the current layer from the stack.
        s_graphics->renderer->endDraw();
        s_graphics->layers.pop();

        // Reactivate Graphicslayer below the recently popped one.
        {
            GraphicsLayer& layer = peekLayer();
            s_graphics->renderer->beginDraw(layer.renderbuffer);
        }
    }

    void shader(const Shader& shader)
    {
        peekStyle().shader = shader;
    }

    void noShader()
    {
        peekStyle().shader.reset();
    }

    void fill(const i32 red, const i32 green, const i32 blue, const i32 alpha)
    {
        fill(Color(red, green, blue, alpha));
    }

    void fill(const i32 grey, const i32 alpha)
    {
        fill(Color(grey, alpha));
    }

    void fill(const Color color)
    {
        RenderStyle& style = peekStyle();
        style.fillColor = color;
        style.isFillEnabled = true;
    }

    void noFill()
    {
        peekStyle().isFillEnabled = false;
    }

    void stroke(const i32 red, const i32 green, const i32 blue, const i32 alpha)
    {
        stroke(Color(red, green, blue, alpha));
    }

    void stroke(const i32 grey, const i32 alpha)
    {
        stroke(Color(grey, alpha));
    }

    void stroke(const Color color)
    {
        RenderStyle& style = peekStyle();
        style.strokeColor = color;
        style.isStrokeEnabled = true;
    }

    void noStroke()
    {
        peekStyle().isStrokeEnabled = false;
    }

    void strokeWeight(const f32 strokeWeight)
    {
        peekStyle().strokeWeight = strokeWeight;
    }

    void strokeCap(const StrokeCap strokeCap)
    {
        peekStyle().strokeCap = strokeCap;
    }

    void strokeJoin(const StrokeJoin strokeJoin)
    {
        peekStyle().strokeJoin = strokeJoin;
    }

    void tint(const i32 red, const i32 green, const i32 blue, const i32 alpha)
    {
        tint(Color(red, green, blue, alpha));
    }

    void tint(const i32 grey, const i32 alpha)
    {
        tint(Color(grey, alpha));
    }

    void tint(const Color color)
    {
        peekStyle().tintColor = color;
    }

    void background(i32 red, i32 green, i32 blue, i32 alpha)
    {
        background(Color(red, green, blue, alpha));
    }

    void background(i32 grey, i32 alpha)
    {
        background(Color(grey, alpha));
    }

    void background(Color color)
    {
        GraphicsLayer& layer = peekLayer();
        const float2 size = float2{layer.renderbuffer.getSize()};

        const RenderStyle& style = peekStyle();
        const RectPath path = path_rect(rect2f{0.0f, 0.0f, size.x, size.y});
        const Contour contour = contour_rect_fill(path);
        const Vertices vertices = vertices_from_contour(contour, matrix4x4::identity, color, getNextDepth(layer));

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        s_graphics->renderer->render(vertices, getRenderState(style, layer));
    }

    void beginShape()
    {
    }

    void endShape()
    {
    }

    void vertex(f32 x, f32 y)
    {
    }

    void vertex(f32 x, f32 y, f32 u, f32 v)
    {
    }

    void bezierVertex(f32 x2, f32 y2, f32 x3, f32 y3)
    {
    }

    void quadraticVertex(f32 cx, f32 cy, f32 x3, f32 y3)
    {
    }

    void curveVertex(f32 x, f32 y)
    {
    }

    void rect(f32 x1, f32 y1, f32 x2, f32 y2)
    {
        GraphicsLayer& layer = peekLayer();
        const RenderStyle& style = peekStyle();
        const matrix4x4& matrix = peekMatrix();
        const rect2f boundary = convert_to_rect(style.rectMode, x1, y1, x2, y2);
        const RectPath path = path_rect(boundary);

        if (style.isFillEnabled)
        {
            const Contour contour = contour_rect_fill(path);
            const Vertices vertices = vertices_from_contour(contour, matrix, style.fillColor, getNextDepth(layer));
            s_graphics->renderer->render(vertices, getRenderState(style, layer));
        }

        if (style.isStrokeEnabled)
        {
            const Contour contour = contour_rect_stroke(path, get_stroke_properties(style));
            const Vertices vertices = vertices_from_contour(contour, matrix, style.strokeColor, getNextDepth(layer));
            s_graphics->renderer->render(vertices, getRenderState(style, layer));
        }
    }

    void square(f32 x1, f32 y1, f32 xy2)
    {
        rect(x1, y1, xy2, xy2);
    }

    void ellipse(f32 x1, f32 y1, f32 x2, f32 y2)
    {
        GraphicsLayer& layer = peekLayer();
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
            const Vertices vertices = vertices_from_contour(contour, matrix, style.fillColor, getNextDepth(layer));
            s_graphics->renderer->render(vertices, getRenderState(style, layer));
        }

        if (style.isStrokeEnabled)
        {
            const Contour contour = contour_ellipse_stroke(path, get_stroke_properties(style));
            const Vertices vertices = vertices_from_contour(contour, matrix, style.strokeColor, getNextDepth(layer));
            s_graphics->renderer->render(vertices, getRenderState(style, layer));
        }
    }

    void circle(f32 x1, f32 y1, f32 xy2)
    {
        ellipse(x1, y1, xy2, xy2);
    }

    void triangle(f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3)
    {
        GraphicsLayer& layer = peekLayer();
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
            const Vertices vertices = vertices_from_contour(contour, matrix, style.fillColor, getNextDepth(layer));
            s_graphics->renderer->render(vertices, getRenderState(style, layer));
        }

        if (style.isStrokeEnabled)
        {
            const Contour contour = contour_triangle_stroke(path, get_stroke_properties(style));
            const Vertices vertices = vertices_from_contour(contour, matrix, style.strokeColor, getNextDepth(layer));
            s_graphics->renderer->render(vertices, getRenderState(style, layer));
        }
    }

    void point(f32 x, f32 y)
    {
        GraphicsLayer& layer = peekLayer();
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
        const Vertices vertices = vertices_from_contour(contour, matrix, style.strokeColor, getNextDepth(layer));
        s_graphics->renderer->render(vertices, getRenderState(style, layer));
    }

    void line(f32 x1, f32 y1, f32 x2, f32 y2)
    {
        GraphicsLayer& layer = peekLayer();
        const RenderStyle& style = peekStyle();
        const matrix4x4& matrix = peekMatrix();
        const rect2f boundary = ellipse_to_rect(style.ellipseMode, x1, y1, x2, y2);

        const Contour contour = contour_line(x1, y1, x2, y2, style.strokeWeight, style.strokeCap);
        const Vertices vertices = vertices_from_contour(contour, matrix, style.strokeColor, getNextDepth(layer));
        s_graphics->renderer->render(vertices, getRenderState(style, layer));
    }

    void image(const Image& img, f32 x1, f32 y1)
    {
        const auto [imgWidth, imgHeight] = img.getSize();
        image(img, x1, y1, static_cast<f32>(imgWidth), static_cast<f32>(imgHeight));
    }

    void image(const Image& img, f32 x1, f32 y1, f32 x2, f32 y2)
    {
        GraphicsLayer& layer = peekLayer();
        const RenderStyle& style = peekStyle();
        const matrix4x4& matrix = peekMatrix();
        const rect2f boundary = convert_to_rect(style.imageMode, x1, y1, x2, y2);

        const Contour contour = contour_image(boundary.left, boundary.top, boundary.width, boundary.height, 0.0f, 0.0f, 1.0f, 1.0f);
        const Vertices vertices = vertices_from_contour(contour, matrix, style.tintColor, getNextDepth(layer));
        s_graphics->renderer->render(vertices, getRenderState(style, layer, img));
    }

    void image(const Image& img, f32 x1, f32 y1, f32 x2, f32 y2, f32 sx1, f32 sy1, f32 sx2, f32 sy2)
    {
        GraphicsLayer& layer = peekLayer();
        const auto [imgWidth, imgHeight] = img.getSize();
        const RenderStyle& style = peekStyle();
        const matrix4x4& matrix = peekMatrix();
        const rect2f boundary = convert_to_rect(style.imageMode, x1, y1, x2, y2);
        const rect2f source = image_source_to_rect(style.imageSourceMode, static_cast<f32>(imgWidth), static_cast<f32>(imgHeight), sx1, sy1, sx2, sy2);

        const Contour contour = contour_image(boundary.left, boundary.top, boundary.width, boundary.height, source.left, source.top, source.width, source.height);
        const Vertices vertices = vertices_from_contour(contour, matrix, style.tintColor, getNextDepth(layer));
        s_graphics->renderer->render(vertices, getRenderState(style, layer, img));
    }
} // namespace processing
