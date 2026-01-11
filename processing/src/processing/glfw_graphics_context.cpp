#include <processing/glfw_graphics_context.hpp>
#include <processing/glfw_window.hpp>
#include <format>

#include <GLFW/glfw3.h>
#include <glad/gl.h>

namespace processing
{
    std::unique_ptr<GraphicsContextImplGLFW> GraphicsContextImplGLFW::create(WindowImplGLFW& window)
    {
        GLFWwindow* handle = window.getPlatformHandle();
        glfwMakeContextCurrent(handle);
        glfwSwapInterval(1);

        if (not gladLoadGL(&glfwGetProcAddress))
        {
            error("Failed to initialize glad.");
            return nullptr;
        }

        info(std::format("OpenGL-Version: {}", std::bit_cast<const char*>(glGetString(GL_VERSION))));
        info(std::format("OpenGL-Vendor: {}", std::bit_cast<const char*>(glGetString(GL_VENDOR))));
        info(std::format("GLSL Language Version: {}", std::bit_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION))));

        return std::unique_ptr<GraphicsContextImplGLFW>(new GraphicsContextImplGLFW(handle, true));
    }

    void GraphicsContextImplGLFW::setVerticalSyncEnabled(bool enabled)
    {
        glfwSwapInterval(enabled ? 1 : 0);
        m_isVerticalSyncEnabled = enabled;
    }

    bool GraphicsContextImplGLFW::isVerticalSyncEnabled() const
    {
        return m_isVerticalSyncEnabled;
    }

    void GraphicsContextImplGLFW::flush()
    {
        glfwSwapBuffers(m_window);
    }

    GraphicsContextImplGLFW::GraphicsContextImplGLFW(GLFWwindow* window, bool isVerticalSyncEnabled)
        : m_window(window), m_isVerticalSyncEnabled(isVerticalSyncEnabled)
    {
    }
} // namespace processing
