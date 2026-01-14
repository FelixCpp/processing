#include <processing/graphics_context.hpp>
#include <GLFW/glfw3.h>
#include <processing/processing.hpp>
#include <processing/glfw_window.hpp>
#include <format>

namespace processing
{
    static uint32_t s_windowCount = 0;
    static bool s_errorCallbackSet = false;

    void errorCallback(const int errorCode, const char* errorMessage)
    {
        error(std::format("GLFW-Error ({}): {}", errorCode, errorMessage));
    }

    std::unique_ptr<WindowImplGLFW> WindowImplGLFW::create(uint32_t width, uint32_t height, const std::string_view title)
    {
        if (not s_errorCallbackSet)
        {
            glfwSetErrorCallback(&errorCallback);
            s_errorCallbackSet = true;
        }

        if (s_windowCount == 0)
        {
            if (not glfwInit())
            {
                error("Could not initialize GLFW");
                return nullptr;
            }
        }

        GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
        if (primaryMonitor == nullptr)
        {
            warning("Could not determine primary monitor");

            glfwWindowHint(GLFW_POSITION_X, GLFW_ANY_POSITION);
            glfwWindowHint(GLFW_POSITION_Y, GLFW_ANY_POSITION);
        }
        else
        {
            int monitorLeft, monitorTop, monitorWidth, monitorHeight;
            glfwGetMonitorWorkarea(primaryMonitor, &monitorLeft, &monitorTop, &monitorWidth, &monitorHeight);
            const int windowLeft = monitorLeft + (monitorWidth - width) / 2;
            const int windowTop = monitorTop + (monitorHeight - height) / 2;
            glfwWindowHint(GLFW_POSITION_X, windowLeft);
            glfwWindowHint(GLFW_POSITION_Y, windowTop);
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
        glfwWindowHint(GLFW_CONTEXT_DEBUG, GLFW_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);

        GLFWwindow* window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
        if (window == nullptr)
        {
            error("Could not create GLFW window");
            return nullptr;
        }

        auto result = std::unique_ptr<WindowImplGLFW>(new WindowImplGLFW(window));
        glfwSetWindowUserPointer(window, result.get());

        glfwSetWindowCloseCallback(
            window,
            [](GLFWwindow* window)
            {
                auto* parent = static_cast<WindowImplGLFW*>(glfwGetWindowUserPointer(window));

                Event event;
                event.type = Event::closed;
                parent->m_events.push(event);
            }
        );

        glfwSetFramebufferSizeCallback(
            window, [](GLFWwindow* window, int width, int height)
            {
                auto* parent = static_cast<WindowImplGLFW*>(glfwGetWindowUserPointer(window));

                Event event;
                event.type = Event::framebuffer_resized;
                event.size.width = static_cast<uint32_t>(width);
                event.size.height = static_cast<uint32_t>(height);
                parent->m_events.push(event);
            }
        );

        glfwSetWindowSizeCallback(
            window, [](GLFWwindow* window, int width, int height)
            {
                auto* parent = static_cast<WindowImplGLFW*>(glfwGetWindowUserPointer(window));

                Event event;
                event.type = Event::window_resized;
                event.size.width = static_cast<uint32_t>(width);
                event.size.height = static_cast<uint32_t>(height);
                parent->m_events.push(event);
            }
        );

        glfwSetCursorPosCallback(
            window, [](GLFWwindow* window, double mx, double my)
            {
                auto* parent = static_cast<WindowImplGLFW*>(glfwGetWindowUserPointer(window));

                Event event;
                event.type = Event::mouse_moved;
                event.mouse_move.x = static_cast<uint32_t>(mx);
                event.mouse_move.y = static_cast<uint32_t>(my);
                parent->m_events.push(event);
            }
        );

        {
            int fbWidth, fbHeight;
            glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
            Event event;
            event.type = Event::framebuffer_resized;
            event.size.width = static_cast<uint32_t>(fbWidth);
            event.size.height = static_cast<uint32_t>(fbHeight);
            result->m_events.push(event);
        }

        return result;
    }

    WindowImplGLFW::~WindowImplGLFW()
    {
        glfwDestroyWindow(m_window);
        --s_windowCount;

        if (s_windowCount == 0)
        {
            glfwTerminate();
        }
    }

    void WindowImplGLFW::setSize(uint32_t width, uint32_t height)
    {
        glfwSetWindowSize(m_window, static_cast<int>(width), static_cast<int>(height));
    }

    uint2 WindowImplGLFW::getSize()
    {
        int width, height;
        printf("Here: %p", m_window);
        glfwGetWindowSize(m_window, &width, &height);
        return uint2{static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
    }

    void WindowImplGLFW::setTitle(const std::string_view title)
    {
        glfwSetWindowTitle(m_window, title.data());
    }

    std::string WindowImplGLFW::getTitle()
    {
        return glfwGetWindowTitle(m_window);
    }

    int2 WindowImplGLFW::getMousePosition()
    {
        double mx, my;
        glfwGetCursorPos(m_window, &mx, &my);
        return {static_cast<int32_t>(mx), static_cast<int32_t>(my)};
    }
    std::optional<Event> WindowImplGLFW::pollEvent()
    {
        if (m_events.empty())
        {
            glfwPollEvents();

            if (m_events.empty())
            {
                return std::nullopt;
            }
        }

        Event event = std::move(m_events.front());
        m_events.pop();
        return event;
    }

    GLFWwindow* WindowImplGLFW::getPlatformHandle()
    {
        return m_window;
    }

    WindowImplGLFW::WindowImplGLFW(GLFWwindow* window) : m_window(window)
    {
    }
} // namespace processing
