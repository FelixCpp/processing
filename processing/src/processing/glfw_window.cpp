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

    static std::optional<KeyCode> glfwKeyCodeToProcessingKeyCode(int key)
    {
        // clang-format off
        switch (key)
        {
            case GLFW_KEY_A: return KeyCode::a;
            case GLFW_KEY_B: return KeyCode::b;
            case GLFW_KEY_C: return KeyCode::c;
            case GLFW_KEY_D: return KeyCode::d;
            case GLFW_KEY_E: return KeyCode::e;
            case GLFW_KEY_F: return KeyCode::f;
            case GLFW_KEY_G: return KeyCode::g;
            case GLFW_KEY_H: return KeyCode::h;
            case GLFW_KEY_I: return KeyCode::i;
            case GLFW_KEY_J: return KeyCode::j;
            case GLFW_KEY_K: return KeyCode::k;
            case GLFW_KEY_L: return KeyCode::l;
            case GLFW_KEY_M: return KeyCode::m;
            case GLFW_KEY_N: return KeyCode::n;
            case GLFW_KEY_O: return KeyCode::o;
            case GLFW_KEY_P: return KeyCode::p;
            case GLFW_KEY_Q: return KeyCode::q;
            case GLFW_KEY_R: return KeyCode::r;
            case GLFW_KEY_S: return KeyCode::s;
            case GLFW_KEY_T: return KeyCode::t;
            case GLFW_KEY_U: return KeyCode::u;
            case GLFW_KEY_V: return KeyCode::v;
            case GLFW_KEY_W: return KeyCode::w;
            case GLFW_KEY_X: return KeyCode::x;
            case GLFW_KEY_Y: return KeyCode::y;
            case GLFW_KEY_Z: return KeyCode::z;
            case GLFW_KEY_1: return KeyCode::num1;
            case GLFW_KEY_2: return KeyCode::num2;
            case GLFW_KEY_3: return KeyCode::num3;
            case GLFW_KEY_4: return KeyCode::num4;
            case GLFW_KEY_5: return KeyCode::num5;
            case GLFW_KEY_6: return KeyCode::num6;
            case GLFW_KEY_7: return KeyCode::num7;
            case GLFW_KEY_8: return KeyCode::num8;
            case GLFW_KEY_9: return KeyCode::num9;
            case GLFW_KEY_0: return KeyCode::num0;
            case GLFW_KEY_SPACE: return KeyCode::space;
            case GLFW_KEY_MINUS: return KeyCode::minus;
            case GLFW_KEY_EQUAL: return KeyCode::equal;
            case GLFW_KEY_LEFT_BRACKET: return KeyCode::leftBracket;
            case GLFW_KEY_RIGHT_BRACKET: return KeyCode::rightBracket;
            case GLFW_KEY_BACKSLASH: return KeyCode::backslash;
            case GLFW_KEY_SEMICOLON: return KeyCode::semicolon;
            case GLFW_KEY_APOSTROPHE: return KeyCode::apostrophe;
            case GLFW_KEY_GRAVE_ACCENT: return KeyCode::graveAccent;
            case GLFW_KEY_COMMA: return KeyCode::comma;
            case GLFW_KEY_PERIOD: return KeyCode::period;
            case GLFW_KEY_SLASH: return KeyCode::slash;
            case GLFW_KEY_WORLD_1: return KeyCode::world1;
            case GLFW_KEY_WORLD_2: return KeyCode::world2;
            case GLFW_KEY_ESCAPE: return KeyCode::escape;
            case GLFW_KEY_F1: return KeyCode::f1;
            case GLFW_KEY_F2: return KeyCode::f2;
            case GLFW_KEY_F3: return KeyCode::f3;
            case GLFW_KEY_F4: return KeyCode::f4;
            case GLFW_KEY_F5: return KeyCode::f5;
            case GLFW_KEY_F6: return KeyCode::f6;
            case GLFW_KEY_F7: return KeyCode::f7;
            case GLFW_KEY_F8: return KeyCode::f8;
            case GLFW_KEY_F9: return KeyCode::f9;
            case GLFW_KEY_F10: return KeyCode::f10;
            case GLFW_KEY_F11: return KeyCode::f11;
            case GLFW_KEY_F12: return KeyCode::f12;
            case GLFW_KEY_F13: return KeyCode::f13;
            case GLFW_KEY_F14: return KeyCode::f14;
            case GLFW_KEY_F15: return KeyCode::f15;
            case GLFW_KEY_F16: return KeyCode::f16;
            case GLFW_KEY_F17: return KeyCode::f17;
            case GLFW_KEY_F18: return KeyCode::f18;
            case GLFW_KEY_F19: return KeyCode::f19;
            case GLFW_KEY_F20: return KeyCode::f20;
            case GLFW_KEY_F21: return KeyCode::f21;
            case GLFW_KEY_F22: return KeyCode::f22;
            case GLFW_KEY_F23: return KeyCode::f23;
            case GLFW_KEY_F24: return KeyCode::f24;
            case GLFW_KEY_F25: return KeyCode::f25;
            case GLFW_KEY_UP: return KeyCode::up;
            case GLFW_KEY_DOWN: return KeyCode::down;
            case GLFW_KEY_LEFT: return KeyCode::left;
            case GLFW_KEY_RIGHT: return KeyCode::right;
            case GLFW_KEY_LEFT_SHIFT: return KeyCode::leftShift;
            case GLFW_KEY_RIGHT_SHIFT: return KeyCode::rightShift;
            case GLFW_KEY_LEFT_CONTROL: return KeyCode::leftControl;
            case GLFW_KEY_RIGHT_CONTROL: return KeyCode::rightControl;
            case GLFW_KEY_LEFT_ALT: return KeyCode::leftAlt;
            case GLFW_KEY_RIGHT_ALT: return KeyCode::rightAlt;
            case GLFW_KEY_TAB: return KeyCode::tab;
            case GLFW_KEY_ENTER: return KeyCode::enter;
            case GLFW_KEY_BACKSPACE: return KeyCode::backspace;
            case GLFW_KEY_INSERT: return KeyCode::insert;
            case GLFW_KEY_DELETE: return KeyCode::deleteKey;
            case GLFW_KEY_PAGE_UP: return KeyCode::pageUp;
            case GLFW_KEY_PAGE_DOWN: return KeyCode::pageDown;
            case GLFW_KEY_HOME: return KeyCode::home;
            case GLFW_KEY_END: return KeyCode::end;
            case GLFW_KEY_KP_0: return KeyCode::kp0;
            case GLFW_KEY_KP_1: return KeyCode::kp1;
            case GLFW_KEY_KP_2: return KeyCode::kp2;
            case GLFW_KEY_KP_3: return KeyCode::kp3;
            case GLFW_KEY_KP_4: return KeyCode::kp4;
            case GLFW_KEY_KP_5: return KeyCode::kp5;
            case GLFW_KEY_KP_6: return KeyCode::kp6;
            case GLFW_KEY_KP_7: return KeyCode::kp7;
            case GLFW_KEY_KP_8: return KeyCode::kp8;
            case GLFW_KEY_KP_9: return KeyCode::kp9;
            case GLFW_KEY_KP_DIVIDE: return KeyCode::kpDivide;
            case GLFW_KEY_KP_MULTIPLY: return KeyCode::kpMultiply;
            case GLFW_KEY_KP_SUBTRACT: return KeyCode::kpSubtract;
            case GLFW_KEY_KP_ADD: return KeyCode::kpAdd;
            case GLFW_KEY_KP_DECIMAL: return KeyCode::kpDecimal;
            case GLFW_KEY_KP_EQUAL: return KeyCode::kpEqual;
            case GLFW_KEY_KP_ENTER: return KeyCode::kpEnter;
            case GLFW_KEY_PRINT_SCREEN: return KeyCode::printScreen;
            case GLFW_KEY_NUM_LOCK: return KeyCode::numLock;
            case GLFW_KEY_CAPS_LOCK: return KeyCode::capsLock;
            case GLFW_KEY_SCROLL_LOCK: return KeyCode::scrollLock;
            case GLFW_KEY_PAUSE: return KeyCode::pause;
            case GLFW_KEY_LEFT_SUPER: return KeyCode::leftSuper;
            case GLFW_KEY_RIGHT_SUPER: return KeyCode::rightSuper;
            case GLFW_KEY_MENU: return KeyCode::menu;
            default:  return std::nullopt;
        };
        // clang-format on
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
        glfwWindowHint(GLFW_SAMPLES, 4);

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

        glfwSetKeyCallback(
            window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
            {
                auto* parent = static_cast<WindowImplGLFW*>(glfwGetWindowUserPointer(window));

                if (const auto code = glfwKeyCodeToProcessingKeyCode(key))
                {
                    Event event;
                    event.type = action == GLFW_PRESS ? Event::key_pressed : action == GLFW_REPEAT ? Event::key_repeated
                                                                                                   : Event::key_released;
                    event.key.code = *code;
                    parent->m_events.push(event);
                }
            }
        );

        glfwSetScrollCallback(
            window, [](GLFWwindow* window, double xoffset, double yoffset)
            {
                auto* parent = static_cast<WindowImplGLFW*>(glfwGetWindowUserPointer(window));

                Event event;
                event.type = Event::mouse_wheel_scrolled;
                event.mouse_wheel.horizontalDelta = xoffset;
                event.mouse_wheel.verticalDelta = yoffset;
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

    uint2 WindowImplGLFW::getFramebufferSize()
    {
        int width, height;
        glfwGetFramebufferSize(m_window, &width, &height);
        return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
    }

    float2 WindowImplGLFW::getContentScale()
    {
        float scaleX, scaleY;
        glfwGetWindowContentScale(m_window, &scaleX, &scaleY);
        return {scaleX, scaleY};
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

    WindowImplGLFW::WindowImplGLFW(GLFWwindow* window)
        : m_window(window)
    {
    }
} // namespace processing
