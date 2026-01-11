#include <processing/window.hpp>

#include <processing/glfw_window.hpp>

namespace processing
{
    std::unique_ptr<Window> createWindow(uint32_t width, uint32_t height, std::string_view title)
    {
        return WindowImplGLFW::create(width, height, title);
    }
} // namespace processing
