#include "processing/render_style.hpp"
#include <processing/render_style_stack.hpp>

namespace processing
{

    RenderStyleStack render_style_stack_create()
    {
        std::array<RenderStyle, 64> renderStyles;
        renderStyles.fill(render_style_default());

        return {
            .renderStyles = std::move(renderStyles),
            .currentStyleIndex = 0,
        };
    }

    void render_style_stack_push(RenderStyleStack& stack, const RenderStyle& style)
    {
        if (stack.currentStyleIndex < stack.renderStyles.size() - 1)
        {
            stack.currentStyleIndex++;
            stack.renderStyles[stack.currentStyleIndex] = style;
        }
    }

    void render_style_stack_pop(RenderStyleStack& stack)
    {
        if (stack.currentStyleIndex > 0)
        {
            // stack.renderStyles[stack.currentStyleIndex] = defaultRenderStyle();
            stack.currentStyleIndex--;
        }
    }

    void render_style_stack_reset(RenderStyleStack& stack)
    {
        // stack.renderStyles[0] = render_style_default(); // TODO(Felix): Do we really want to do this?
        stack.currentStyleIndex = 0;
    }

    RenderStyle& render_style_stack_peek(RenderStyleStack& stack)
    {
        return stack.renderStyles[stack.currentStyleIndex];
    }
} // namespace processing
