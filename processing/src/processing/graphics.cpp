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

    struct RenderbufferImpl : PlatformRenderbuffer
    {
        explicit RenderbufferImpl(const u32 width, const u32 height, const FilterMode filterMode, const ExtendMode extendMode)
            : currentDepth{MIN_DEPTH},
              renderStyles{RenderStyle()},
              metrics{matrix4x4::identity},
              framebuffer{createFramebuffer(width, height, filterMode, extendMode)}
        {
        }

        virtual Image& getImage() override
        {
            return framebuffer.getImage();
        }

        virtual uint2 getSize() const override
        {
            return framebuffer.getSize();
        }

        f32 currentDepth;
        NeverEmptyStack<RenderStyle> renderStyles;
        NeverEmptyStack<matrix4x4> metrics;
        Framebuffer framebuffer;
    };

    class RenderbufferRegistry
    {
    public:
        explicit RenderbufferRegistry()
            : m_nextAssetId(1)
        {
        }

        AssetId insert(std::shared_ptr<RenderbufferImpl> impl)
        {
            const AssetId insertionId = AssetId{m_nextAssetId};
            m_assets.emplace(std::make_pair(insertionId.value, std::move(impl)));
            ++m_nextAssetId;
            return insertionId;
        }

        RenderbufferImpl& get(const AssetId& assetId)
        {
            return *m_assets.at(assetId.value);
        }

    private:
        std::unordered_map<usize, std::shared_ptr<RenderbufferImpl>> m_assets;
        usize m_nextAssetId;
    };

    struct GraphicsData
    {
        RenderbufferRegistry renderbuffers;
        NeverEmptyStack<usize> renderbufferIds;
        std::shared_ptr<DefaultRenderer> renderer;
    };

    inline static std::unique_ptr<GraphicsData> s_graphics;

    inline RenderbufferImpl& peekRenderbuffer()
    {
        return s_graphics->renderbuffers.get(AssetId{s_graphics->renderbufferIds.peek()});
    }
} // namespace processing

namespace processing
{
    void initGraphics(const u32 width, const u32 height)
    {
        auto registry = RenderbufferRegistry{};
        AssetId initialBufferId = registry.insert(std::make_shared<RenderbufferImpl>(width, height, FilterMode::linear, ExtendMode::clamp));

        s_graphics = std::unique_ptr<GraphicsData>{
            new GraphicsData{
                .renderbuffers = std::move(registry),
                .renderbufferIds = NeverEmptyStack<usize>{initialBufferId.value},
                .renderer = DefaultRenderer::create(),
            },
        };
    }

    void beginDraw()
    {
        RenderbufferImpl& renderbuffer = peekRenderbuffer();
        renderbuffer.currentDepth = MIN_DEPTH;
        s_graphics->renderer->beginDraw(renderbuffer.framebuffer);
    }

    void endDraw(u32 width, u32 height)
    {
        // TODO(Felix): Maybe detect some unreleased resources (push without pop for styles, matrics shaders or renderbuffers)?
        s_graphics->renderer->endDraw();
        blit(width, height, peekRenderbuffer().framebuffer);
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
    f32 getNextDepth(RenderbufferImpl& data)
    {
        const f32 depth = data.currentDepth;
        data.currentDepth += 1.0f / 20'000.0f;
        return depth;
    }

    RenderState getRenderState(const RenderStyle& style, const RenderbufferImpl& renderbuffer, const std::optional<Image>& image = std::nullopt)
    {
        return RenderState{
            .blendMode = style.blendMode,
            .shader = style.shader,
            .image = image,
            .transform = matrix4x4::orthographic(0.0f, 0.0f, renderbuffer.framebuffer.getSize().x, renderbuffer.framebuffer.getSize().y, -1.0f, 1.0f),
        };
    }
} // namespace processing

namespace processing
{
    Renderbuffer createRenderbuffer(const u32 width, const u32 height, const FilterMode filterMode, const ExtendMode extendMode)
    {
        auto impl = std::make_shared<RenderbufferImpl>(width, height, filterMode, extendMode);
        auto assetId = s_graphics->renderbuffers.insert(impl);
        return Renderbuffer{assetId, impl};
    }
} // namespace processing

namespace processing
{
    void pushRenderbuffer(const Renderbuffer& renderbufferId)
    {
        // Flush the rendering state
        s_graphics->renderer->endDraw();
        s_graphics->renderbufferIds.push(renderbufferId.getAssetId().value);

        // Activate the new render buffer
        {
            const RenderbufferImpl& renderbuffer = peekRenderbuffer();
            s_graphics->renderer->beginDraw(renderbuffer.framebuffer);
        }
    }

    void popRenderbuffer()
    {
        // Flush the rendering state & pop the current renderbuffer from the stack.
        s_graphics->renderer->endDraw();
        s_graphics->renderbufferIds.pop();

        // Reactivate Graphicsrenderbuffer below the recently popped one.
        {
            const RenderbufferImpl& renderbuffer = peekRenderbuffer();
            s_graphics->renderer->beginDraw(renderbuffer.framebuffer);
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
            peekRenderbuffer().renderStyles.push(peekStyle());
        }
        else
        {
            peekRenderbuffer().renderStyles.push(RenderStyle());
        }
    }

    void popStyle()
    {
        peekRenderbuffer().renderStyles.pop();
    }

    RenderStyle& peekStyle()
    {
        return peekRenderbuffer().renderStyles.peek();
    }

    void pushMatrix(const bool extendPreviousMatrix)
    {
        if (extendPreviousMatrix)
        {
            peekRenderbuffer().metrics.push(peekMatrix());
        }
        else
        {
            peekRenderbuffer().metrics.push(matrix4x4::identity);
        }
    }

    void popMatrix()
    {
        peekRenderbuffer().metrics.pop();
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
        return peekRenderbuffer().metrics.peek();
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
        RenderbufferImpl& renderbuffer = peekRenderbuffer();
        const float2 size = float2{renderbuffer.framebuffer.getSize()};

        const RenderStyle& style = peekStyle();
        const RectPath path = path_rect(rect2f{0.0f, 0.0f, size.x, size.y});
        const Contour contour = contour_rect_fill(path);
        const Vertices vertices = vertices_from_contour(contour, matrix4x4::identity, color, getNextDepth(renderbuffer));

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        s_graphics->renderer->render(vertices, getRenderState(style, renderbuffer));
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
        RenderbufferImpl& renderbuffer = peekRenderbuffer();
        const RenderStyle& style = peekStyle();
        const matrix4x4& matrix = peekMatrix();
        const rect2f boundary = convert_to_rect(style.rectMode, x1, y1, x2, y2);
        const RectPath path = path_rect(boundary);

        if (style.isFillEnabled)
        {
            const Contour contour = contour_rect_fill(path);
            const Vertices vertices = vertices_from_contour(contour, matrix, style.fillColor, getNextDepth(renderbuffer));
            s_graphics->renderer->render(vertices, getRenderState(style, renderbuffer));
        }

        if (style.isStrokeEnabled)
        {
            const Contour contour = contour_rect_stroke(path, get_stroke_properties(style));
            const Vertices vertices = vertices_from_contour(contour, matrix, style.strokeColor, getNextDepth(renderbuffer));
            s_graphics->renderer->render(vertices, getRenderState(style, renderbuffer));
        }
    }

    void square(f32 x1, f32 y1, f32 xy2)
    {
        rect(x1, y1, xy2, xy2);
    }

    void ellipse(f32 x1, f32 y1, f32 x2, f32 y2)
    {
        RenderbufferImpl& renderbuffer = peekRenderbuffer();
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
            const Vertices vertices = vertices_from_contour(contour, matrix, style.fillColor, getNextDepth(renderbuffer));
            s_graphics->renderer->render(vertices, getRenderState(style, renderbuffer));
        }

        if (style.isStrokeEnabled)
        {
            const Contour contour = contour_ellipse_stroke(path, get_stroke_properties(style));
            const Vertices vertices = vertices_from_contour(contour, matrix, style.strokeColor, getNextDepth(renderbuffer));
            s_graphics->renderer->render(vertices, getRenderState(style, renderbuffer));
        }
    }

    void circle(f32 x1, f32 y1, f32 xy2)
    {
        ellipse(x1, y1, xy2, xy2);
    }

    void triangle(f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3)
    {
        RenderbufferImpl& renderbuffer = peekRenderbuffer();
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
            const Vertices vertices = vertices_from_contour(contour, matrix, style.fillColor, getNextDepth(renderbuffer));
            s_graphics->renderer->render(vertices, getRenderState(style, renderbuffer));
        }

        if (style.isStrokeEnabled)
        {
            const Contour contour = contour_triangle_stroke(path, get_stroke_properties(style));
            const Vertices vertices = vertices_from_contour(contour, matrix, style.strokeColor, getNextDepth(renderbuffer));
            s_graphics->renderer->render(vertices, getRenderState(style, renderbuffer));
        }
    }

    void point(f32 x, f32 y)
    {
        RenderbufferImpl& renderbuffer = peekRenderbuffer();
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
        const Vertices vertices = vertices_from_contour(contour, matrix, style.strokeColor, getNextDepth(renderbuffer));
        s_graphics->renderer->render(vertices, getRenderState(style, renderbuffer));
    }

    void line(f32 x1, f32 y1, f32 x2, f32 y2)
    {
        RenderbufferImpl& renderbuffer = peekRenderbuffer();
        const RenderStyle& style = peekStyle();
        const matrix4x4& matrix = peekMatrix();
        const rect2f boundary = ellipse_to_rect(style.ellipseMode, x1, y1, x2, y2);

        const Contour contour = contour_line(x1, y1, x2, y2, style.strokeWeight, style.strokeCap);
        const Vertices vertices = vertices_from_contour(contour, matrix, style.strokeColor, getNextDepth(renderbuffer));
        s_graphics->renderer->render(vertices, getRenderState(style, renderbuffer));
    }

    void image(const Image& img, f32 x1, f32 y1)
    {
        const auto [imgWidth, imgHeight] = img.getSize();
        image(img, x1, y1, static_cast<f32>(imgWidth), static_cast<f32>(imgHeight));
    }

    void image(const Image& img, f32 x1, f32 y1, f32 x2, f32 y2)
    {
        RenderbufferImpl& renderbuffer = peekRenderbuffer();
        const RenderStyle& style = peekStyle();
        const matrix4x4& matrix = peekMatrix();
        const rect2f boundary = convert_to_rect(style.imageMode, x1, y1, x2, y2);

        const Contour contour = contour_image(boundary.left, boundary.top, boundary.width, boundary.height, 0.0f, 0.0f, 1.0f, 1.0f);
        const Vertices vertices = vertices_from_contour(contour, matrix, style.tintColor, getNextDepth(renderbuffer));
        s_graphics->renderer->render(vertices, getRenderState(style, renderbuffer, img));
    }

    void image(const Image& img, f32 x1, f32 y1, f32 x2, f32 y2, f32 sx1, f32 sy1, f32 sx2, f32 sy2)
    {
        RenderbufferImpl& renderbuffer = peekRenderbuffer();
        const auto [imgWidth, imgHeight] = img.getSize();
        const RenderStyle& style = peekStyle();
        const matrix4x4& matrix = peekMatrix();
        const rect2f boundary = convert_to_rect(style.imageMode, x1, y1, x2, y2);
        const rect2f source = image_source_to_rect(style.imageSourceMode, static_cast<f32>(imgWidth), static_cast<f32>(imgHeight), sx1, sy1, sx2, sy2);

        const Contour contour = contour_image(boundary.left, boundary.top, boundary.width, boundary.height, source.left, source.top, source.width, source.height);
        const Vertices vertices = vertices_from_contour(contour, matrix, style.tintColor, getNextDepth(renderbuffer));
        s_graphics->renderer->render(vertices, getRenderState(style, renderbuffer, img));
    }
} // namespace processing
