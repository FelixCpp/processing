#ifndef _PROCESSING_INCLUDE_SHAPE_BUILDER_HPP_
#define _PROCESSING_INCLUDE_SHAPE_BUILDER_HPP_

#include <processing/processing.hpp>

#include <vector>
#include <cstdint>

namespace processing
{
    struct Contour
    {
        std::vector<float2> positions;
        std::vector<float2> texcoords;
        std::vector<uint32_t> indices;
    };

    struct Radius
    {
        float x;
        float y;

        inline static constexpr Radius circular(float radius)
        {
            return {.x = radius, .y = radius};
        }

        inline static constexpr Radius elliptical(float x, float y)
        {
            return {.x = x, .y = y};
        }
    };

} // namespace processing

namespace processing
{
    struct Shape
    {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
    };

    Shape shape_from_contour(const Contour& contour, const matrix4x4& transform, color_t color, float depth);
} // namespace processing

namespace processing
{
    struct RectPath
    {
        rect2f boundary;
        std::vector<float2> points;
    };

    RectPath path_rect(const rect2f& boundary);
    Contour contour_rect_fill(const RectPath& path);
    Contour contour_rect_stroke(const RectPath& path, float strokeWeight, StrokeJoin strokeJoin);
} // namespace processing

namespace processing
{
    struct EllipseSpecification
    {
        float2 center;
        Radius radius;
        size_t segments;
    };

    struct EllipsePath
    {
        EllipseSpecification specification;
        std::vector<float2> points;
    };

    EllipsePath path_ellipse(const EllipseSpecification& specification);
    Contour contour_ellipse_fill(const EllipsePath& path);
    Contour contour_ellipse_stroke(const EllipsePath& path, float strokeWeight, StrokeJoin strokeJoin);
} // namespace processing

namespace processing
{
    struct TriangleSpecification
    {
        float2 a;
        float2 b;
        float2 c;
    };

    struct TrianglePath
    {
        TriangleSpecification specification;
        std::vector<float2> points;
    };

    TrianglePath path_triangle(const TriangleSpecification& specification);
    Contour contour_triangle_fill(const TrianglePath& path);
    Contour contour_triangle_stroke(const TrianglePath& path, float strokeWeight, StrokeJoin strokeJoin);
} // namespace processing

namespace processing
{
    struct RoundedRectSpecification
    {
        rect2f boundary;
        Radius topLeft;
        Radius topRight;
        Radius bottomRight;
        Radius bottomLeft;
    };

    struct RoundedRectPath
    {
        RoundedRectSpecification specification;
        std::vector<float2> points;
    };

    RoundedRectPath path_rounded_rect(const RoundedRectSpecification& roundedRect);
    Contour contour_rounded_rect_fill(const RoundedRectPath& path);
    Contour contour_rounded_rect_stroke(const RoundedRectPath& path, float strokeWeight, StrokeJoin strokeJoin);

    Contour contour_quad_fill(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
    Contour contour_quad_stroke(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float strokeWeight, StrokeJoin strokeJoin);

    Contour contour_line(float x1, float y1, float x2, float y2, float strokeWeight, StrokeCap strokeCap);
    Contour contour_image(float left, float top, float width, float height, float sourceLeft, float sourceTop, float sourceWidth, float sourceHeight);
} // namespace processing

#endif // _PROCESSING_INCLUDE_SHAPE_BUILDER_HPP_
