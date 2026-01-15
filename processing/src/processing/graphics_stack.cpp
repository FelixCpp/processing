#include <processing/graphics_stack.hpp>

namespace processing
{
    GraphicsStack graphics_stack_create(std::shared_ptr<MainGraphics> mainRenderTarget)
    {
        GraphicsStack stack;
        stack.defaultRenderTarget = mainRenderTarget;
        return stack;
    }

    void graphics_stack_push(GraphicsStack& stack, std::shared_ptr<OffscreenGraphics> renderTarget)
    {
        if (stack.remoteRenderTargets.empty())
        {
            stack.defaultRenderTarget->pause();
        }
        else
        {
            stack.remoteRenderTargets.top()->pause();
        }

        stack.remoteRenderTargets.emplace(std::move(renderTarget))->beginDraw();
    }

    void graphics_stack_pop(GraphicsStack& stack)
    {
        const size_t remoteRenderTargetCount = stack.remoteRenderTargets.size();

        if (remoteRenderTargetCount > 0) //< Must not be empty
        {
            stack.remoteRenderTargets.top()->endDraw();
            stack.remoteRenderTargets.pop();

            if (remoteRenderTargetCount > 1) // There's at least one more below the recently popped one
            {
                stack.defaultRenderTarget->resume();
            }
            else
            {
                stack.remoteRenderTargets.top()->resume();
            }
        }
    }

    Graphics& graphics_stack_peek(GraphicsStack& stack)
    {
        if (not stack.remoteRenderTargets.empty())
        {
            return *stack.remoteRenderTargets.top();
        }

        return *stack.defaultRenderTarget;
    }
} // namespace processing
