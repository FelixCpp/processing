#include <processing/render_style.hpp>

namespace processing
{
    RenderStyle render_style_default()
    {
        return {
            .fillColor = color(255, 255, 255),
            .strokeColor = color(255, 255, 255),
            .strokeWeight = 1.0f,
            .isFillEnabled = true,
            .isStrokeEnabled = true,
            .strokeJoin = StrokeJoin::miter,
            .strokeCap = StrokeCap::round,
            .rectMode = rect_mode_ltwh(),
            .ellipseMode = ellipse_mode_center_radius(),
        };
    }
} // namespace processing
