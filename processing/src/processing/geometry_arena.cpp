#include <processing/geometry_arena.hpp>

namespace processing
{
    static GeometryArena* s_context = nullptr;

    void geometry_begin_frame(GeometryArena& arena)
    {
        s_context = &arena;
    }

    void geometry_end_frame()
    {
        s_context = nullptr;
    }

    void geometry_add_quad_fill(const std::span<const float2, 4>& points, const std::span<const Color, 4>& colors)
    {
        const usize base = s_context->vertexCursor;
        for (usize i = 0; i < 4; ++i)
        {
            s_context->points[base + i] = points[i];
            s_context->colors[base + i] = colors[i];
        }

        s_context->vertexCursor += 4;

        s_context->indices[s_context->indexCursor + 0] = base + 0;
        s_context->indices[s_context->indexCursor + 1] = base + 1;
        s_context->indices[s_context->indexCursor + 2] = base + 2;
        s_context->indices[s_context->indexCursor + 3] = base + 2;
        s_context->indices[s_context->indexCursor + 4] = base + 3;
        s_context->indices[s_context->indexCursor + 5] = base + 0;
        s_context->indexCursor += 6;
    }

    void geometry_add_quad_stroke(const std::span<const float2, 4>& points, const std::span<const Color, 4>& colors, const StrokeProperties& properties)
    {
    }

} // namespace processing
