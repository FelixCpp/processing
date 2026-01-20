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

    struct Shape
    {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
    };

    Shape shape_from_contour(const Contour& contour, color_t color, float depth);

    Contour contour_rect_fill(float left, float top, float width, float height);
    Contour contour_rect_stroke(float left, float top, float width, float height, float strokeWeight, StrokeJoin strokeJoin);

    Contour contour_ellipse_fill(float centerX, float centerY, float radiusX, float radiusY, size_t segments);
    Contour contour_ellipse_stroke(float centerX, float centerY, float radiusX, float radiusY, float strokeWeight, size_t segments, StrokeJoin strokeJoin);

    Contour contour_triangle_fill(float x1, float y1, float x2, float y2, float x3, float y3);
    Contour contour_triangle_stroke(float x1, float y1, float x2, float y2, float x3, float y3, float strokeWeight, StrokeJoin strokeJoin);

    Contour contour_line(float x1, float y1, float x2, float y2, float strokeWeight, StrokeCap strokeCap);
    Contour contour_image(float left, float top, float width, float height, float sourceLeft, float sourceTop, float sourceWidth, float sourceHeight);
} // namespace processing

#endif // _PROCESSING_INCLUDE_SHAPE_BUILDER_HPP_
