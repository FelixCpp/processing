#ifndef _PROCESSING_INCLUDE_GEOMETRY_ARENA_HPP_
#define _PROCESSING_INCLUDE_GEOMETRY_ARENA_HPP_

#include <processing/processing.hpp>
#include <processing/shape_builder.hpp>

#include <span>

namespace processing
{
    struct GeometryArena
    {
        std::unique_ptr<float2[]> points;
        std::unique_ptr<float2[]> texcoords;
        std::unique_ptr<Color[]> colors;
        std::unique_ptr<u32[]> indices;

        size_t vertexCursor;
        size_t indexCursor;
    };

    // struct StrokeProperties
    // {
    //     StrokeJoin strokeJoin;
    //     f32 strokeWeight;
    //     float miterLimit;
    // };

    void geometry_begin_frame(GeometryArena& arena);
    void geometry_end_frame();

    void geometry_add_quad_fill(const std::span<const float2, 4>& points, const std::span<const Color, 4>& colors);
    void geometry_add_quad_stroke(const std::span<const float2, 4>& points, const std::span<const Color, 4>& colors, const StrokeProperties& properties);
} // namespace processing

#endif // _PROCESSING_INCLUDE_GEOMETRY_ARENA_HPP_
