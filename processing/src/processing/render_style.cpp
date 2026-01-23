#include "processing/shader.hpp"
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
            .imageMode = rect_mode_ltwh(),
            .imageSourceMode = image_source_mode_ltwh_coordinates(),
            .imageTint = color(255, 255, 255),
            .blendMode = BlendMode::alpha,
            .shaderHandle = INVALID_SHADER_HANDLE,
        };
    }
} // namespace processing
