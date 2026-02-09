#ifndef _PROCESSING_INCLUDE_SHAPE_BUILDER_HPP_
#define _PROCESSING_INCLUDE_SHAPE_BUILDER_HPP_

#include <processing/processing.hpp>

#include <vector>
#include <span>
#include <cstdint>

namespace processing
{
    struct Contour
    {
        std::vector<float2> positions;
        std::vector<float2> texcoords;
        std::vector<uint32_t> indices;
    };

    struct PolygonContour
    {
        std::vector<float2> points;
        std::vector<float2> texcoords;
        std::vector<Color> colors;
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
    struct StrokeProperties
    {
        StrokeJoin strokeJoin;
        float strokeWeight;
        float miterLimit;
    };

    struct EllipseProperties
    {
        float2 center;
        Radius radius;
        usize segments;
    };

    struct EllipsePath
    {
        EllipseProperties properties;
        std::vector<float2> points;
    };
} // namespace processing

namespace processing
{
    EllipsePath path_ellipse(const EllipseProperties& properties);
    PolygonContour contour_polygon_ellipse_fill(const EllipsePath& path, Color color);
    PolygonContour contour_polygon_ellipse_stroke(const EllipsePath& path, Color color, const StrokeProperties& properties);

    PolygonContour contour_polygon_quad_fill(const std::span<const float2, 4>& points, const std::span<const Color, 4>& colors);
    PolygonContour contour_polygon_quad_stroke(const std::span<const float2, 4>& points, const std::span<const Color, 4>& colors, const StrokeProperties& properties);

    PolygonContour contour_polygon_triangle_fill(const std::span<const float2, 3>& points, const std::span<const Color, 3>& colors);
    PolygonContour contour_polygon_triangle_stroke(const std::span<const float2, 3>& points, const std::span<const Color, 3>& colors, const StrokeProperties& properties);

    PolygonContour contour_polygon_line(const std::span<const float2, 2>& points, const std::span<const Color, 2>& colors, float strokeWeight, StrokeCap strokeCap);
    PolygonContour contour_polygon_image(const std::span<const float2, 4>& points, const std::span<const Color>& colors, const rect2f& sourceRect);

    PolygonContour contour_polygon_polygon_fill(const std::span<const float2>& points, const std::span<const float2>& texcoords, const std::span<const Color>& colors);
    PolygonContour contour_polygon_polygon_stroke(const std::span<const float2>& points, const std::span<const Color>& colors, const StrokeProperties& strokeProperties);
} // namespace processing

#endif // _PROCESSING_INCLUDE_SHAPE_BUILDER_HPP_
