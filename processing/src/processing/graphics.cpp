#include <processing/graphics.hpp>
#include <processing/shape_builder.hpp>

#include <glad/gl.h>

#include <unordered_map>

namespace processing
{
    void blit(u32 width, u32 height, const Framebuffer& fb)
    {
        glViewport(0, 0, width, height);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fb.getResourceId().value);
        glBlitFramebuffer(0, 0, fb.getSize().x, fb.getSize().y, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    }
} // namespace processing

namespace processing
{
    inline static constexpr f32 MIN_DEPTH = -1.0f;
    inline static constexpr f32 MAX_DEPTH = 1.0f;
    inline static constexpr f32 DEPTH_INCREMENT = 1.0f / 20'000.0f;
    inline static constexpr f32 MITER_LIMIT = 4.0f;

    struct ShapeBuilderPoint
    {
        float2 position;
        float2 texcoord;
        Color fillColor;
        Color strokeColor;
    };

    struct GraphicsData
    {
        std::unordered_map<usize, f32> depths;
        std::unordered_map<usize, NeverEmptyStack<RenderStyle>> renderStyles;
        std::unordered_map<usize, NeverEmptyStack<matrix4x4>> metrics;
        std::unordered_map<usize, Framebuffer> framebuffers;

        NeverEmptyStack<usize> assetIds;
        usize nextAssetId;

        ShapeMode shapeMode;
        bool shapeStarted;
        std::vector<ShapeBuilderPoint> points;
        std::vector<float2> curvePoints;

        std::shared_ptr<BatchRenderer> renderer;
    };

    inline static std::unique_ptr<GraphicsData> s_graphics;

    // clang-format off
    AssetId peekAssetId() { return AssetId{ .value = s_graphics->assetIds.peek() }; }
    f32& peekDepth() { return s_graphics->depths.at(peekAssetId().value); }
    NeverEmptyStack<RenderStyle>& peekRenderStyles() { return s_graphics->renderStyles.at(peekAssetId().value); }
    NeverEmptyStack<matrix4x4>& peekMetrics() { return s_graphics->metrics.at(peekAssetId().value); }
    Framebuffer& peekFramebuffer() { return s_graphics->framebuffers.at(peekAssetId().value); }
    // clang-format on
} // namespace processing

namespace processing
{
    class RenderbufferImpl : public PlatformRenderbuffer
    {
    public:
        explicit RenderbufferImpl(const AssetId assetId)
            : m_assetId{assetId}
        {
        }

        virtual Image& getImage() override
        {
            return s_graphics->framebuffers.at(m_assetId.value).getImage();
        }

        virtual uint2 getSize() const override
        {
            return s_graphics->framebuffers.at(m_assetId.value).getSize();
        }

        virtual AssetId getAssetId() const override
        {
            return m_assetId;
        }

    private:
        AssetId m_assetId;
    };
} // namespace processing

namespace processing
{
    void warnMemoryLeaks()
    {
#ifndef NDEBUG
        if (s_graphics->metrics.at(peekAssetId().value).size() > 1)
        {
            fprintf(stdout, "Metrics have not been released properly");
            fflush(stdout);
        }

        if (s_graphics->renderStyles.at(peekAssetId().value).size() > 1)
        {
            fprintf(stdout, "RenderStyles have not been released properly");
            fflush(stdout);
        }

        if (s_graphics->shapeStarted)
        {
            fprintf(stdout, "Shape builder is in building stage");
            fflush(stdout);
        }
#endif
    }
} // namespace processing

namespace processing
{
    void initGraphics(const u32 width, const u32 height)
    {
        s_graphics = std::unique_ptr<GraphicsData>{
            new GraphicsData{
                .depths = {{0, MIN_DEPTH}},
                .renderStyles = {{0, NeverEmptyStack<RenderStyle>{RenderStyle()}}},
                .metrics = {{0, NeverEmptyStack<matrix4x4>{matrix4x4::identity}}},
                .framebuffers = {{0, createFramebuffer(width, height, FilterMode::linear, ExtendMode::clamp)}},
                .assetIds = NeverEmptyStack<usize>{0},
                .nextAssetId = 1,
                .shapeMode = ShapeMode::points,
                .shapeStarted = false,
                .points = {},
                .curvePoints = {},
                .renderer = BatchRenderer::create(),
            },
        };
    }

    void beginDraw()
    {
        s_graphics->renderer->beginDraw(peekFramebuffer());
    }

    void endDraw(u32 width, u32 height)
    {
        s_graphics->renderer->endDraw();
        blit(width, height, peekFramebuffer());

        warnMemoryLeaks();

        peekDepth() = MIN_DEPTH;
        peekRenderStyles().reset(RenderStyle());
        peekMetrics().reset(matrix4x4::identity);
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

    Vertices vertices_from_polygon_contour(const PolygonContour& contour, const matrix4x4& transform, f32 depth)
    {
        Vertices shape;
        shape.mode = VertexMode::triangles;
        shape.vertices.reserve(contour.points.size());
        shape.indices.append_range(contour.indices);

        for (size_t i = 0; i < contour.points.size(); ++i)
        {
            shape.vertices.push_back(Vertex{
                .position = float3{transform.transformPoint(contour.points[i]), depth},
                .texcoord = contour.texcoords[i],
                .color = float4_from_color(contour.colors[i]),
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
            .miterLimit = MITER_LIMIT,
        };
    }
} // namespace processing

namespace processing
{
    f32 getNextDepth()
    {
        const f32 depth = peekDepth();
        peekDepth() += 1.0f / 20'000.0f;
        return depth;
    }

    RenderState getRenderState(const std::optional<Image>& image = std::nullopt)
    {
        const RenderStyle& style = peekStyle();
        const Framebuffer& framebuffer = peekFramebuffer();

        return RenderState{
            .blendMode = style.blendMode,
            .shader = style.shader,
            .image = image,
            .transform = matrix4x4::orthographic(0.0f, 0.0f, framebuffer.getSize().x, framebuffer.getSize().y, -1.0f, 1.0f),
        };
    }
} // namespace processing

namespace processing
{
    Renderbuffer createRenderbuffer(const u32 width, const u32 height, const FilterMode filterMode, const ExtendMode extendMode)
    {
        const AssetId newAssetId = {.value = s_graphics->nextAssetId};
        s_graphics->depths.emplace(std::make_pair(newAssetId.value, MIN_DEPTH));
        s_graphics->renderStyles.emplace(std::make_pair(newAssetId.value, NeverEmptyStack<RenderStyle>{RenderStyle()}));
        s_graphics->metrics.emplace(std::make_pair(newAssetId.value, NeverEmptyStack<matrix4x4>{matrix4x4::identity}));
        s_graphics->framebuffers.emplace(std::make_pair(newAssetId.value, createFramebuffer(width, height, filterMode, extendMode)));

        ++s_graphics->nextAssetId;
        return Renderbuffer{std::make_shared<RenderbufferImpl>(newAssetId)};
    }
} // namespace processing

namespace processing
{
    void pushRenderbuffer(const Renderbuffer& renderbufferId)
    {
        // Flush the rendering state
        s_graphics->renderer->endDraw();
        s_graphics->assetIds.push(renderbufferId.getAssetId().value);
        // s_graphics->renderbufferIds.push(renderbufferId.getAssetId().value);

        // Activate the new render buffer
        {
            peekDepth() = MIN_DEPTH;
            s_graphics->renderer->beginDraw(peekFramebuffer());
        }
    }

    void popRenderbuffer()
    {
        warnMemoryLeaks();

        // Flush the rendering state & pop the current renderbuffer from the stack.
        s_graphics->renderer->endDraw();
        s_graphics->assetIds.pop();

        // Reactivate Graphicsrenderbuffer below the recently popped one.
        {
            s_graphics->renderer->beginDraw(peekFramebuffer());
        }
    }

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
            peekRenderStyles().push(peekStyle());
        }
        else
        {
            peekRenderStyles().push(RenderStyle());
        }
    }

    void popStyle()
    {
        peekRenderStyles().pop();
    }

    RenderStyle& peekStyle()
    {
        return peekRenderStyles().peek();
    }

    void pushMatrix(const bool extendPreviousMatrix)
    {
        if (extendPreviousMatrix)
        {
            peekMetrics().push(peekMatrix());
        }
        else
        {
            peekMetrics().push(matrix4x4::identity);
        }
    }

    void popMatrix()
    {
        peekMetrics().pop();
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
        return peekMetrics().peek();
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

    void fill(const Color color, const i32 alpha)
    {
        fill(Color(color.r, color.g, color.b, alpha));
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
        const float2 size = float2{peekFramebuffer().getSize()};
        const std::array points = {
            float2{0.0f, 0.0f},
            float2{size.x, 0.0f},
            float2{size.x, size.y},
            float2{0.0f, size.y},
        };

        const std::array colors = {color, color, color, color};

        const RenderStyle& style = peekStyle();
        const PolygonContour contour = contour_polygon_quad_fill(points, colors);
        const Vertices vertices = vertices_from_polygon_contour(contour, matrix4x4::identity, getNextDepth());

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        s_graphics->renderer->render(vertices, getRenderState());
    }

    void beginShape(const ShapeMode mode)
    {
        s_graphics->points.clear();
        s_graphics->curvePoints.clear();
        s_graphics->shapeMode = mode;
        s_graphics->shapeStarted = true;
    }

    void render(const Vertices& vertices, const RenderState& state)
    {
        s_graphics->renderer->render(vertices, state);
    }

    void renderPoints(const std::span<const ShapeBuilderPoint>& points, const RenderStyle& style, const matrix4x4& transform, const RenderState& renderState)
    {
        for (size_t i = 0; i < points.size(); ++i)
        {
            const ShapeBuilderPoint& point = points[i];
            const EllipsePath path = path_ellipse({
                .center = point.position,
                .radius = Radius::circular(style.strokeWeight),
            });

            const PolygonContour contour = contour_polygon_ellipse_fill(path, point.strokeColor);
            const Vertices vertices = vertices_from_polygon_contour(contour, transform, getNextDepth());
            render(vertices, renderState);
        }
    }

    void renderLines(const std::span<const ShapeBuilderPoint>& points, const RenderStyle& style, const matrix4x4& transform, const RenderState& renderState)
    {
        for (usize i = 0; i + 1 < points.size(); i += 2)
        {
            const ShapeBuilderPoint& a = points[i + 0];
            const ShapeBuilderPoint& b = points[i + 1];
            const PolygonContour contour = contour_polygon_line(
                std::array{a.position, b.position},
                std::array{a.strokeColor, b.strokeColor},
                style.strokeWeight, style.strokeCap
            );

            const Vertices vertices = vertices_from_polygon_contour(contour, transform, getNextDepth());
            render(vertices, renderState);
        }
    }

    void renderLineStrip(const std::span<const ShapeBuilderPoint>& points, const RenderStyle& style, const matrix4x4& transform, const RenderState& renderState)
    {
        for (size_t i = 0; i + 1 < points.size(); i++)
        {
            const ShapeBuilderPoint& a = points[i + 0];
            const ShapeBuilderPoint& b = points[i + 1];
            const PolygonContour contour = contour_polygon_line(
                std::array{a.position, b.position},
                std::array{a.strokeColor, b.strokeColor},
                style.strokeWeight, style.strokeCap
            );

            const Vertices vertices = vertices_from_polygon_contour(contour, transform, getNextDepth());
            render(vertices, renderState);
        }
    }

    void renderLineLoop(const std::span<const ShapeBuilderPoint>& points, const RenderStyle& style, const matrix4x4& transform, const RenderState& renderState)
    {
        for (size_t i = 0; i < points.size(); i++)
        {
            const ShapeBuilderPoint& a = points[i + 0];
            const ShapeBuilderPoint& b = points[(i + 1) % points.size()];
            const PolygonContour contour = contour_polygon_line(
                std::array{a.position, b.position},
                std::array{a.strokeColor, b.strokeColor},
                style.strokeWeight, style.strokeCap
            );

            const Vertices vertices = vertices_from_polygon_contour(contour, transform, getNextDepth());
            render(vertices, renderState);
        }
    }

    void renderTriangles(const std::span<const ShapeBuilderPoint>& points, const RenderStyle& style, const matrix4x4& transform, const RenderState& renderState)
    {
        for (size_t i = 0; i + 2 < points.size(); i += 3)
        {
            const ShapeBuilderPoint& a = points[i + 0];
            const ShapeBuilderPoint& b = points[i + 1];
            const ShapeBuilderPoint& c = points[i + 2];

            if (style.isFillEnabled)
            {
                const PolygonContour contour = contour_polygon_triangle_fill(
                    std::array{a.position, b.position, c.position},
                    std::array{a.fillColor, b.fillColor, c.fillColor}
                );

                const Vertices vertices = vertices_from_polygon_contour(contour, transform, getNextDepth());
                render(vertices, renderState);
            }

            if (style.isStrokeEnabled)
            {
                const PolygonContour contour = contour_polygon_triangle_stroke(
                    std::array{a.position, b.position, c.position},
                    std::array{a.strokeColor, b.strokeColor, c.strokeColor},
                    get_stroke_properties(style)
                );

                const Vertices vertices = vertices_from_polygon_contour(contour, transform, getNextDepth());
                render(vertices, renderState);
            }
        }
    }

    void renderTriangleStrip(const std::span<const ShapeBuilderPoint>& points, const RenderStyle& style, const matrix4x4& transform, const RenderState& renderState)
    {
        for (size_t i = 0; i + 2 < s_graphics->points.size(); i++)
        {
            const ShapeBuilderPoint& a = points[i + 0];
            const ShapeBuilderPoint& b = points[i + 1];
            const ShapeBuilderPoint& c = points[i + 2];

            if (style.isFillEnabled)
            {
                const PolygonContour contour = contour_polygon_triangle_fill(
                    std::array{a.position, b.position, c.position},
                    std::array{a.fillColor, b.fillColor, c.fillColor}
                );

                const Vertices vertices = vertices_from_polygon_contour(contour, transform, getNextDepth());
                render(vertices, renderState);
            }

            if (style.isStrokeEnabled)
            {
                const PolygonContour contour = contour_polygon_triangle_stroke(
                    std::array{a.position, b.position, c.position},
                    std::array{a.fillColor, b.fillColor, c.fillColor},
                    get_stroke_properties(style)
                );

                const Vertices vertices = vertices_from_polygon_contour(contour, transform, getNextDepth());
                render(vertices, renderState);
            }
        }
    }

    void renderTriangleFan(const std::span<const ShapeBuilderPoint>& points, const RenderStyle& style, const matrix4x4& transform, const RenderState& renderState)
    {
        if (points.size() < 3) return;

        const ShapeBuilderPoint& a = points[0];

        for (size_t i = 1; i + 1 < s_graphics->points.size(); i++)
        {
            const ShapeBuilderPoint& b = points[i + 1];
            const ShapeBuilderPoint& c = points[i + 2];

            if (style.isFillEnabled)
            {
                const PolygonContour contour = contour_polygon_triangle_fill(
                    std::array{a.position, b.position, c.position},
                    std::array{a.fillColor, b.fillColor, c.fillColor}
                );

                const Vertices vertices = vertices_from_polygon_contour(contour, transform, getNextDepth());
                render(vertices, renderState);
            }

            if (style.isStrokeEnabled)
            {
                const PolygonContour contour = contour_polygon_triangle_stroke(
                    std::array{a.position, b.position, c.position},
                    std::array{a.fillColor, b.fillColor, c.fillColor},
                    get_stroke_properties(style)
                );

                const Vertices vertices = vertices_from_polygon_contour(contour, transform, getNextDepth());
                render(vertices, renderState);
            }
        }
    }

    void renderQuads(const std::span<const ShapeBuilderPoint>& points, const RenderStyle& style, const matrix4x4& transform, const RenderState& renderState)
    {
        for (size_t i = 0; i + 3 < points.size(); i += 4)
        {
            const ShapeBuilderPoint& a = points[i + 0];
            const ShapeBuilderPoint& b = points[i + 1];
            const ShapeBuilderPoint& c = points[i + 2];
            const ShapeBuilderPoint& d = points[i + 3];

            if (style.isFillEnabled)
            {
                const PolygonContour contour = contour_polygon_quad_fill(
                    std::array{a.position, b.position, c.position, d.position},
                    std::array{a.fillColor, b.fillColor, c.fillColor, d.fillColor}
                );

                const Vertices vertices = vertices_from_polygon_contour(contour, transform, getNextDepth());
                render(vertices, renderState);
            }

            if (style.isStrokeEnabled)
            {
                const PolygonContour contour = contour_polygon_quad_stroke(
                    std::array{a.position, b.position, c.position, d.position},
                    std::array{a.strokeColor, b.strokeColor, c.strokeColor, d.strokeColor},
                    get_stroke_properties(style)
                );

                const Vertices vertices = vertices_from_polygon_contour(contour, transform, getNextDepth());
                render(vertices, renderState);
            }
        }
    }

    void renderQuadStrip(const std::span<const ShapeBuilderPoint>& points, const RenderStyle& style, const matrix4x4& transform, const RenderState& renderState)
    {
        for (size_t i = 0; i + 3 < points.size(); i += 2)
        {
            const ShapeBuilderPoint& a = points[i + 0];
            const ShapeBuilderPoint& b = points[i + 1];
            const ShapeBuilderPoint& c = points[i + 2];
            const ShapeBuilderPoint& d = points[i + 3];

            if (style.isFillEnabled)
            {
                const PolygonContour contour = contour_polygon_quad_fill(
                    std::array{a.position, b.position, c.position, d.position},
                    std::array{a.fillColor, b.fillColor, c.fillColor, d.fillColor}
                );

                const Vertices vertices = vertices_from_polygon_contour(contour, transform, getNextDepth());
                render(vertices, renderState);
            }

            if (style.isStrokeEnabled)
            {
                const PolygonContour contour = contour_polygon_quad_stroke(
                    std::array{a.position, b.position, c.position, d.position},
                    std::array{a.strokeColor, b.strokeColor, c.strokeColor, d.strokeColor},
                    get_stroke_properties(style)
                );

                const Vertices vertices = vertices_from_polygon_contour(contour, transform, getNextDepth());
                render(vertices, renderState);
            }
        }
    }

    void renderPolygon(const std::span<const ShapeBuilderPoint>& points, const RenderStyle& style, const matrix4x4& transform, const RenderState& renderState)
    {
        for (size_t i = 0; i + 3 < points.size(); i += 2)
        {
            const ShapeBuilderPoint& a = points[i + 0];
            const ShapeBuilderPoint& b = points[i + 1];
            const ShapeBuilderPoint& c = points[i + 2];
            const ShapeBuilderPoint& d = points[i + 3];

            if (style.isFillEnabled)
            {
                const PolygonContour contour = contour_polygon_polygon_fill(
                    std::array{a.position, b.position, c.position, d.position},
                    std::array{a.texcoord, b.texcoord, c.texcoord, d.texcoord},
                    std::array{a.fillColor, b.fillColor, c.fillColor, d.fillColor}
                );

                const Vertices vertices = vertices_from_polygon_contour(contour, transform, getNextDepth());
                render(vertices, renderState);
            }

            if (style.isStrokeEnabled)
            {
                const PolygonContour contour = contour_polygon_polygon_stroke(
                    std::array{a.position, b.position, c.position, d.position},
                    std::array{a.strokeColor, b.strokeColor, c.strokeColor, d.strokeColor},
                    get_stroke_properties(style)
                );

                const Vertices vertices = vertices_from_polygon_contour(contour, transform, getNextDepth());
                render(vertices, renderState);
            }
        }
    }

    void endShape(const bool closed)
    {
        if (not s_graphics->shapeStarted) return;

        const RenderStyle& style = peekStyle();
        const matrix4x4& transform = peekMatrix();
        const RenderState renderState = getRenderState();

        switch (s_graphics->shapeMode)
        {
                // clang-format off
            case ShapeMode::points: { renderPoints(s_graphics->points, style, transform, renderState); } break;
            case ShapeMode::lines: { renderLines(s_graphics->points, style, transform, renderState); } break;
            case ShapeMode::lineStrip: { renderLineStrip(s_graphics->points, style, transform, renderState); } break;
            case ShapeMode::lineLoop: { renderLineLoop(s_graphics->points, style, transform, renderState); } break;
            case ShapeMode::triangles: { renderTriangles(s_graphics->points, style, transform, renderState); } break;
            case ShapeMode::triangleStrip: { renderTriangleStrip(s_graphics->points, style, transform, renderState); } break;
            case ShapeMode::triangleFan: { renderTriangleFan(s_graphics->points, style, transform, renderState); } break;
            case ShapeMode::quads: { renderQuads(s_graphics->points, style, transform, renderState); } break;
            case ShapeMode::quadStrip: { renderQuadStrip(s_graphics->points, style, transform, renderState); } break;
            case ShapeMode::polygon: { renderPolygon(s_graphics->points, style, transform, renderState); } break;
                // clang-format on
        }

        s_graphics->shapeStarted = false;
    }

    void vertex(f32 x, f32 y)
    {
        vertex(x, y, 0.0f, 0.0f);
    }

    void vertex(f32 x, f32 y, f32 u, f32 v)
    {
        if (not s_graphics->shapeStarted) return;

        const RenderStyle& style = peekStyle();

        ShapeBuilderPoint point = {
            .position = float2{x, y},
            .texcoord = {u, v},
            .fillColor = style.fillColor,
            .strokeColor = style.strokeColor,
        };

        s_graphics->points.emplace_back(std::move(point));
    }

    void bezierVertex(f32 x2, f32 y2, f32 x3, f32 y3, f32 x4, f32 y4)
    {
        if (!s_graphics->shapeStarted || s_graphics->points.empty()) return;

        // Letzter Punkt ist der Start der Bezier-Kurve
        float2 p1 = s_graphics->points.back().position;
        float2 p2(x2, y2);
        float2 p3(x3, y3);
        float2 p4(x4, y4);

        // Generiere Punkte entlang der kubischen Bezier-Kurve
        int segments = 20;
        for (int i = 1; i <= segments; i++)
        {
            float t = i / (float)segments;
            float t2 = t * t;
            float t3 = t2 * t;
            float mt = 1.0f - t;
            float mt2 = mt * mt;
            float mt3 = mt2 * mt;

            // Kubische Bezier-Formel: B(t) = (1-t)³P1 + 3(1-t)²tP2 + 3(1-t)t²P3 + t³P4
            float x = mt3 * p1.x + 3 * mt2 * t * p2.x + 3 * mt * t2 * p3.x + t3 * p4.x;
            float y = mt3 * p1.y + 3 * mt2 * t * p2.y + 3 * mt * t2 * p3.y + t3 * p4.y;

            vertex(x, y);
        }
    }

    void quadraticVertex(f32 cx, f32 cy, f32 x3, f32 y3)
    {
        if (!s_graphics->shapeStarted || s_graphics->points.empty()) return;

        // Letzter Punkt ist der Start
        float2 p1 = s_graphics->points.back().position;
        float2 p2(cx, cy); // Kontrollpunkt
        float2 p3(x3, y3); // Endpunkt

        // Generiere Punkte entlang der quadratischen Bezier-Kurve
        int segments = 20;
        for (int i = 1; i <= segments; i++)
        {
            float t = i / (float)segments;
            float t2 = t * t;
            float mt = 1.0f - t;
            float mt2 = mt * mt;

            // Quadratische Bezier-Formel: B(t) = (1-t)²P1 + 2(1-t)tP2 + t²P3
            float x = mt2 * p1.x + 2 * mt * t * p2.x + t2 * p3.x;
            float y = mt2 * p1.y + 2 * mt * t * p2.y + t2 * p3.y;

            vertex(x, y);
        }
    }

    void curveVertex(f32 x, f32 y)
    {
        if (!s_graphics->shapeStarted) return;

        // Sammle Punkte für Catmull-Rom Spline
        s_graphics->curvePoints.push_back(float2(x, y));

        // Brauchen mindestens 4 Punkte für Catmull-Rom
        if (s_graphics->curvePoints.size() >= 4)
        {
            usize n = s_graphics->curvePoints.size();
            float2 p0 = s_graphics->curvePoints[n - 4];
            float2 p1 = s_graphics->curvePoints[n - 3];
            float2 p2 = s_graphics->curvePoints[n - 2];
            float2 p3 = s_graphics->curvePoints[n - 1];

            // Generiere Kurve zwischen p1 und p2
            int segments = 20;
            for (int i = 0; i <= segments; i++)
            {
                float t = i / (float)segments;
                float t2 = t * t;
                float t3 = t2 * t;

                // Catmull-Rom Spline Matrix
                float x = 0.5f * ((2 * p1.x) +
                                  (-p0.x + p2.x) * t +
                                  (2 * p0.x - 5 * p1.x + 4 * p2.x - p3.x) * t2 +
                                  (-p0.x + 3 * p1.x - 3 * p2.x + p3.x) * t3);

                float y = 0.5f * ((2 * p1.y) +
                                  (-p0.y + p2.y) * t +
                                  (2 * p0.y - 5 * p1.y + 4 * p2.y - p3.y) * t2 +
                                  (-p0.y + 3 * p1.y - 3 * p2.y + p3.y) * t3);

                vertex(x, y);
            }
        }
    }

    void bezier(f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3, f32 x4, f32 y4)
    {
        beginShape(ShapeMode::lineStrip);
        vertex(x1, y1);
        bezierVertex(x2, y2, x3, y3, x4, y4);
        endShape();
    }

    void curve(f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3, f32 x4, f32 y4)
    {
        beginShape(ShapeMode::lineStrip);
        vertex(x1, y1);
        curveVertex(x2, y2);
        curveVertex(x3, y3);
        curveVertex(x4, y4);
        endShape();
    }

    void rect(f32 x1, f32 y1, f32 x2, f32 y2)
    {
        const RenderStyle& style = peekStyle();
        const matrix4x4& matrix = peekMatrix();
        const rect2f boundary = convert_to_rect(style.rectMode, x1, y1, x2, y2);
        const std::array points = {
            float2{boundary.left, boundary.top},
            float2{boundary.right(), boundary.top},
            float2{boundary.right(), boundary.bottom()},
            float2{boundary.left, boundary.bottom()},
        };

        if (style.isFillEnabled)
        {
            const std::array<Color, 4> colors = {style.fillColor, style.fillColor, style.fillColor, style.fillColor};
            const PolygonContour contour = contour_polygon_quad_fill(points, colors);
            const Vertices vertices = vertices_from_polygon_contour(contour, matrix, getNextDepth());
            render(vertices, getRenderState());
        }

        if (style.isStrokeEnabled)
        {
            const std::array<Color, 4> colors = {style.strokeColor, style.strokeColor, style.strokeColor, style.strokeColor};
            const PolygonContour contour = contour_polygon_quad_stroke(points, colors, get_stroke_properties(style));
            const Vertices vertices = vertices_from_polygon_contour(contour, matrix, getNextDepth());
            render(vertices, getRenderState());
        }
    }

    void square(f32 x1, f32 y1, f32 xy2)
    {
        rect(x1, y1, xy2, xy2);
    }

    void ellipse(f32 x1, f32 y1, f32 x2, f32 y2)
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
        });

        if (style.isFillEnabled)
        {
            const PolygonContour contour = contour_polygon_ellipse_fill(path, style.fillColor);
            const Vertices vertices = vertices_from_polygon_contour(contour, matrix, getNextDepth());
            render(vertices, getRenderState());
        }

        if (style.isStrokeEnabled)
        {
            const PolygonContour contour = contour_polygon_ellipse_stroke(path, style.strokeColor, get_stroke_properties(style));
            const Vertices vertices = vertices_from_polygon_contour(contour, matrix, getNextDepth());
            render(vertices, getRenderState());
        }
    }

    void circle(f32 x1, f32 y1, f32 xy2)
    {
        ellipse(x1, y1, xy2, xy2);
    }

    void triangle(f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3)
    {
        const RenderStyle& style = peekStyle();
        const matrix4x4& matrix = peekMatrix();
        const rect2f boundary = ellipse_to_rect(style.ellipseMode, x1, y1, x2, y2);
        const std::array path = {
            float2{x1, y1},
            float2{x2, y2},
            float2{x3, y3},
        };

        if (style.isFillEnabled)
        {
            const std::array colors = {style.fillColor, style.fillColor, style.fillColor};
            const PolygonContour contour = contour_polygon_triangle_fill(path, colors);
            const Vertices vertices = vertices_from_polygon_contour(contour, matrix, getNextDepth());
            render(vertices, getRenderState());
        }

        if (style.isStrokeEnabled)
        {
            const std::array colors = {style.strokeColor, style.strokeColor, style.strokeColor};
            const PolygonContour contour = contour_polygon_triangle_stroke(path, colors, get_stroke_properties(style));
            const Vertices vertices = vertices_from_polygon_contour(contour, matrix, getNextDepth());
            render(vertices, getRenderState());
        }
    }

    void point(f32 x, f32 y)
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
        });

        const PolygonContour contour = contour_polygon_ellipse_fill(path, style.strokeColor);
        const Vertices vertices = vertices_from_polygon_contour(contour, matrix, getNextDepth());
        render(vertices, getRenderState());
    }

    void line(f32 x1, f32 y1, f32 x2, f32 y2)
    {
        const RenderStyle& style = peekStyle();
        const matrix4x4& matrix = peekMatrix();
        const std::array points = {float2{x1, y1}, float2{x2, y2}};
        const std::array colors = {style.strokeColor, style.strokeColor};

        const PolygonContour contour = contour_polygon_line(points, colors, style.strokeWeight, style.strokeCap);
        const Vertices vertices = vertices_from_polygon_contour(contour, matrix, getNextDepth());
        render(vertices, getRenderState());
    }

    void image(const Image& img, f32 x1, f32 y1)
    {
        const auto [imgWidth, imgHeight] = img.getSize();
        image(img, x1, y1, static_cast<f32>(imgWidth), static_cast<f32>(imgHeight));
    }

    void image(const Image& img, f32 x1, f32 y1, f32 x2, f32 y2)
    {
        const RenderStyle& style = peekStyle();
        const matrix4x4& matrix = peekMatrix();
        const rect2f boundary = convert_to_rect(style.imageMode, x1, y1, x2, y2);
        const std::array<float2, 4> points = {
            float2{boundary.top, boundary.left},
            float2{boundary.right(), boundary.top},
            float2{boundary.right(), boundary.bottom()},
            float2{boundary.left, boundary.bottom()},
        };

        const std::array colors = {style.tintColor, style.tintColor, style.tintColor, style.tintColor};
        const PolygonContour contour = contour_polygon_image(points, colors, rect2f{0.0f, 0.0f, 1.0f, 1.0f});
        const Vertices vertices = vertices_from_polygon_contour(contour, matrix, getNextDepth());
        s_graphics->renderer->render(vertices, getRenderState(img));
    }

    void image(const Image& img, f32 x1, f32 y1, f32 x2, f32 y2, f32 sx1, f32 sy1, f32 sx2, f32 sy2)
    {
        const auto [imgWidth, imgHeight] = img.getSize();
        const RenderStyle& style = peekStyle();
        const matrix4x4& matrix = peekMatrix();
        const rect2f boundary = convert_to_rect(style.imageMode, x1, y1, x2, y2);
        const rect2f source = image_source_to_rect(style.imageSourceMode, static_cast<f32>(imgWidth), static_cast<f32>(imgHeight), sx1, sy1, sx2, sy2);

        const std::array<float2, 4> points = {
            float2{boundary.top, boundary.left},
            float2{boundary.right(), boundary.top},
            float2{boundary.right(), boundary.bottom()},
            float2{boundary.left, boundary.bottom()},
        };

        const std::array colors = {style.tintColor, style.tintColor, style.tintColor, style.tintColor};
        const PolygonContour contour = contour_polygon_image(points, colors, source);
        const Vertices vertices = vertices_from_polygon_contour(contour, matrix, getNextDepth());
        s_graphics->renderer->render(vertices, getRenderState(img));
    }
} // namespace processing
