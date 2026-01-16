#ifndef _PROCESSING_INCLUDE_GLFW_WINDOW_HPP_
#define _PROCESSING_INCLUDE_GLFW_WINDOW_HPP_

#include <GLFW/glfw3.h>

#include <processing/window.hpp>

#include <memory>
#include <string_view>
#include <queue>

namespace processing
{
    class WindowImplGLFW : public Window
    {
    public:
        static std::unique_ptr<WindowImplGLFW> create(uint32_t width, uint32_t height, std::string_view title);

        ~WindowImplGLFW() override;

        void setSize(uint32_t width, uint32_t height) override;
        uint2 getSize() override;

        void setTitle(std::string_view title) override;
        std::string getTitle() override;

        int2 getMousePosition() override;
        uint2 getFramebufferSize() override;
        float2 getContentScale() override;

        std::optional<Event> pollEvent() override;

        GLFWwindow* getPlatformHandle();

    private:
        explicit WindowImplGLFW(GLFWwindow* window);

        GLFWwindow* m_window;
        std::queue<Event> m_events;
    };
} // namespace processing

#endif // _PROCESSING_INCLUDE_GLFW_WINDOW_HPP_
