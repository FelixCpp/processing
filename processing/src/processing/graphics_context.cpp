#include <processing/graphics_context.hpp>
#include <processing/glfw_graphics_context.hpp>
#include <processing/glfw_window.hpp>

namespace processing
{
    std::unique_ptr<GraphicsContext> createContext(Window& window)
    {
        if (WindowImplGLFW* impl = dynamic_cast<WindowImplGLFW*>(&window))
        {
            return GraphicsContextImplGLFW::create(*impl);
        }

        return nullptr;
    }
} // namespace processing
