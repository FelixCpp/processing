#include <processing/shape_builder.hpp>
#include <cassert>
#include <cmath>
#include <numbers>

namespace processing
{
    inline static constexpr float PI = std::numbers::pi_v<float>;

    // Generiert Stroke-Geometrie aus einer geschlossenen Kontur
    static Contour contour_stroke_from_path(const std::vector<float2>& path, float strokeWeight, StrokeJoin strokeJoin)
    {
        std::vector<float2> positions;
        std::vector<float2> texcoords;
        std::vector<uint32_t> indices;

        const float half_stroke_weight = strokeWeight * 0.5f;

        for (size_t i = 0; i < path.size(); ++i)
        {
            const size_t next_index = (i + 1) % path.size();
            const float2 p1 = path[i];
            const float2 p2 = path[next_index];
            const float2 diff = p2 - p1;
            // const float length = value2_length(diff);
        }

        return {
            .positions = std::move(positions),
            .texcoords = std::move(texcoords),
            .indices = std::move(indices),
        };
    }
} // namespace processing

namespace processing
{
    static constexpr float4 float4_from_color(color_t color)
    {
        return float4{
            .x = static_cast<float>(red(color)) / 255.0f,
            .y = static_cast<float>(green(color)) / 255.0f,
            .z = static_cast<float>(blue(color)) / 255.0f,
            .w = static_cast<float>(alpha(color)) / 255.0f,
        };
    }

    Shape shape_from_contour(const Contour& contour, color_t color, float depth)
    {
        assert(contour.positions.size() == contour.texcoords.size() and "Contour must contain the same number of positions as texcoords");

        Shape shape;
        shape.vertices.reserve(contour.positions.size());
        shape.indices.append_range(contour.indices);

        for (size_t i = 0; i < contour.positions.size(); ++i)
        {
            shape.vertices.push_back(Vertex{
                .position = float3(contour.positions[i], depth),
                .texcoord = contour.texcoords[i],
                .color = float4_from_color(color),
            });
        }

        return shape;
    }

    Contour contour_rect_fill(float left, float top, float width, float height)
    {
        const float right = left + width;
        const float bottom = top + height;

        return {
            .positions = {
                {left, top},
                {right, top},
                {right, bottom},
                {left, bottom},
            },
            .texcoords = {
                {0.0f, 0.0f},
                {1.0f, 0.0f},
                {1.0f, 1.0f},
                {0.0f, 1.0f},
            },
            .indices = {0, 1, 2, 2, 3, 0}
        };
    }

    Contour contour_rect_stroke(float left, float top, float width, float height, float strokeWeight, StrokeJoin strokeJoin)
    {
        const float right = left + width;
        const float bottom = top + height;

        const std::vector<float2> positions = {
            {left, top},
            {right, top},
            {right, bottom},
            {left, bottom},
        };

        return contour_stroke_from_path(positions, strokeWeight, strokeJoin);
    }

    Contour contour_ellipse_fill(float centerX, float centerY, float radiusX, float radiusY, size_t segments)
    {
        Contour c;

        c.positions.push_back({centerX, centerY});
        c.texcoords.push_back({0.5f, 0.5f});

        for (size_t i = 0; i <= segments; ++i)
        {
            const float angle = 2.0f * PI * i / segments;
            const float x = centerX + std::cos(angle) * radiusX;
            const float y = centerY + std::sin(angle) * radiusY;

            c.positions.push_back({x, y});
            c.texcoords.push_back({0.5f + std::cos(angle) * 0.5f, 0.5f + std::sin(angle) * 0.5f});
        }

        for (size_t i = 1; i <= segments; ++i)
        {
            c.indices.push_back(0);
            c.indices.push_back(i);
            c.indices.push_back(i + 1);
        }

        return c;
    }

    Contour contour_ellipse_stroke(float centerX, float centerY, float radiusX, float radiusY, float strokeWeight, size_t segments, StrokeJoin strokeJoin)
    {
        std::vector<float2> path;
        path.reserve(segments);

        for (size_t i = 0; i < segments; ++i)
        {
            float angle = 2.0f * PI * i / segments;
            path.push_back({centerX + std::cos(angle) * radiusX, centerY + std::sin(angle) * radiusY});
        }

        return contour_stroke_from_path(path, strokeWeight, strokeJoin);
    }

    Contour contour_triangle_fill(float x1, float y1, float x2, float y2, float x3, float y3)
    {
        Contour c;

        c.positions = {
            {x1, y1},
            {x2, y2},
            {x3, y3}
        };

        c.texcoords = {
            {0.0f, 0.0f},
            {1.0f, 0.0f},
            {0.5f, 1.0f}
        };

        c.indices = {0, 1, 2};

        return c;
    }

    Contour contour_triangle_stroke(float x1, float y1, float x2, float y2, float x3, float y3, float strokeWeight, StrokeJoin strokeJoin)
    {
        std::vector<float2> path = {
            {x1, y1},
            {x2, y2},
            {x3, y3}
        };

        return contour_stroke_from_path(path, strokeWeight, strokeJoin);
    }

    Contour contour_line(float x1, float y1, float x2, float y2, float strokeWeight, StrokeCap strokeCap)
    {
        return Contour{};
    }
} // namespace processing
