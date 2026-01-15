#ifndef _PROCESSING_INCLUDE_GRAPHICS_STACK_HPP_
#define _PROCESSING_INCLUDE_GRAPHICS_STACK_HPP_

#include <processing/graphics.hpp>

#include <stack>

namespace processing
{
    struct GraphicsStack
    {
        std::stack<std::shared_ptr<OffscreenGraphics>> remoteRenderTargets;
        std::shared_ptr<MainGraphics> defaultRenderTarget;
    };

    GraphicsStack graphics_stack_create(std::shared_ptr<MainGraphics> mainRenderTarget);
    void graphics_stack_push(GraphicsStack& stack, std::shared_ptr<OffscreenGraphics> renderTarget);
    void graphics_stack_pop(GraphicsStack& stack);
    Graphics& graphics_stack_peek(GraphicsStack& stack);
} // namespace processing

#endif // _PROCESSING_INCLUDE_GRAPHICS_STACK_HPP_
