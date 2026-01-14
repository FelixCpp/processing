#ifndef _PROCESSING_INCLUDE_RENDER_STYLE_STACK_HPP_
#define _PROCESSING_INCLUDE_RENDER_STYLE_STACK_HPP_

#include <processing/render_style.hpp>

#include <array>

namespace processing
{
    struct RenderStyleStack
    {
        std::array<RenderStyle, 64> renderStyles;
        size_t currentStyleIndex;
    };

    RenderStyleStack render_style_stack_create();
    void render_style_stack_push(RenderStyleStack& stack, const RenderStyle& style);
    void render_style_stack_pop(RenderStyleStack& stack);
    void render_style_stack_reset(RenderStyleStack& stack);
    RenderStyle& render_style_stack_peek(RenderStyleStack& stack);
} // namespace processing

#endif // _PROCESSING_INCLUDE_RENDER_STYLE_STACK_HPP_
