#ifndef _PROCESSING_INCLUDE_RENDER_STYLE_HPP_
#define _PROCESSING_INCLUDE_RENDER_STYLE_HPP_

#include <processing/processing.hpp>

namespace processing
{
    struct RenderStyle
    {
        color_t fillColor;
        color_t strokeColor;
        float strokeWeight;
        bool isFillEnabled;
        bool isStrokeEnabled;
        StrokeJoin strokeJoin;
        StrokeCap strokeCap;
        RectMode rectMode;
        EllipseMode ellipseMode;
    };

    RenderStyle render_style_default();
} // namespace processing

#endif // _PROCESSING_INCLUDE_RENDER_STYLE_HPP_
