#ifndef _PROCESSING_INCLUDE_GLFW_GRAPHICSCONTEXT_HPP_
#define _PROCESSING_INCLUDE_GLFW_GRAPHICSCONTEXT_HPP_

#include <processing/graphics_context.hpp>
#include <GLFW/glfw3.h>

#include <memory>

namespace processing
{
    class WindowImplGLFW;

    class GraphicsContextImplGLFW : public GraphicsContext
    {
    public:
        static std::unique_ptr<GraphicsContextImplGLFW> create(WindowImplGLFW& window);

        ~GraphicsContextImplGLFW() override = default;

        void setVerticalSyncEnabled(bool enabled) override;
        bool isVerticalSyncEnabled() const override;
        void flush() override;

    private:
        explicit GraphicsContextImplGLFW(GLFWwindow* window, bool isVerticalSyncEnabled);

        GLFWwindow* m_window;
        bool m_isVerticalSyncEnabled;
    };
} // namespace processing

#endif // _PROCESSING_INCLUDE_GLFW_GRAPHICSCONTEXT_HPP_
