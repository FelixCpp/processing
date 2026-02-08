#include <processing/processing.hpp>
#include <processing/graphics.hpp>
#include <processing/image.hpp>
#include <processing/renderbuffer.hpp>
#include <processing/renderer.hpp>
#include <processing/shader.hpp>

#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include <algorithm>

namespace processing
{
    struct LibraryData
    {
        bool closeRequested;
        bool shouldRestart;
        bool isLoopPaused;
        bool isRedrawRequested;
        i32 exitCode;
        u64 frameCount;

        GLFWwindow* window;

        ImageAssetHandler images;
        RenderbufferAssetHandler renderbuffers;
        ShaderAssetHandler shaders;

        std::unique_ptr<Sketch> sketch;
        // std::shared_ptr<Renderer> renderer;
        // std::unique_ptr<Graphics> graphics;
    };

    inline static LibraryData s_data;
} // namespace processing

namespace processing
{
    matrix4x4::matrix4x4()
        : data{
              1.0f, 0.0f, 0.0f, 0.0f,
              0.0f, 1.0f, 0.0f, 0.0f,
              0.0f, 0.0f, 1.0f, 0.0f,
              0.0f, 0.0f, 0.0f, 1.0f
          }
    {
    }

    matrix4x4::matrix4x4(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33
    )
        : data{
              m00, m01, m02, m03,
              m10, m11, m12, m13,
              m20, m21, m22, m23,
              m30, m31, m32, m33
          }
    {
    }

    matrix4x4 matrix4x4::combined(const matrix4x4& other) const
    {
        matrix4x4 result;

        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                result.data[i * 4 + j] =
                    data[i * 4 + 0] * other.data[0 * 4 + j] +
                    data[i * 4 + 1] * other.data[1 * 4 + j] +
                    data[i * 4 + 2] * other.data[2 * 4 + j] +
                    data[i * 4 + 3] * other.data[3 * 4 + j];
            }
        }

        return result;
    }

    matrix4x4 matrix4x4::orthographic(f32 left, f32 top, f32 width, f32 height, f32 near, f32 far)
    {
        f32 right = left + width;
        f32 bottom = top + height;

        f32 rl = right - left;
        f32 tb = top - bottom;
        f32 fn = far - near;

        return matrix4x4{
            2.0f / rl, 0.0f, 0.0f, 0.0f,
            0.0f, 2.0f / tb, 0.0f, 0.0f,
            0.0f, 0.0f, -2.0f / fn, 0.0f,
            -(right + left) / rl, -(top + bottom) / tb, -(far + near) / fn, 1.0f
        };
    }

    matrix4x4 matrix4x4::translation(const f32 x, const f32 y)
    {
        return matrix4x4{
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            x, y, 0.0f, 1.0f
        };
    }

    matrix4x4 matrix4x4::scaling(const f32 x, const f32 y)
    {
        return matrix4x4{
            x, 0.0f, 0.0f, 0.0f,
            0.0f, y, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
    }

    matrix4x4 matrix4x4::rotation(const f32 angle)
    {
        const f32 cos = std::cos(angle);
        const f32 sin = std::sin(angle);

        return matrix4x4{
            cos, sin, 0.0f, 0.0f,
            -sin, cos, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
    }

    float2 matrix4x4::transformPoint(const float2& point) const
    {
        return point;
    }

    float3 matrix4x4::transformPoint(const float3& point) const
    {
        return point;
    }

    const matrix4x4 matrix4x4::identity;
} // namespace processing

namespace processing
{
    Color::Color()
        : Color(255)
    {
    }
    Color::Color(const i32 red, const i32 green, const i32 blue, const i32 alpha)
        : r{static_cast<u8>(std::clamp(red, 0, 255))},
          g{static_cast<u8>(std::clamp(green, 0, 255))},
          b{static_cast<u8>(std::clamp(blue, 0, 255))},
          a{static_cast<u8>(std::clamp(alpha, 0, 255))}
    {
    }

    Color::Color(const i32 grey, const i32 alpha)
        : Color(grey, grey, grey, alpha)
    {
    }

    i32 Color::brightness() const
    {
        const f32 r = 0.2126f * static_cast<f32>(r);
        const f32 g = 0.7152f * static_cast<f32>(g);
        const f32 b = 0.0722f * static_cast<f32>(b);
        const f32 luminance = r + g + b;

        return static_cast<i32>(luminance);
    }
} // namespace processing

namespace processing
{
    RenderStyle::RenderStyle()
        : fillColor{255, 255, 255},
          strokeColor{255, 255, 255},
          tintColor{255, 255, 255},
          isFillEnabled{true},
          isStrokeEnabled{true},
          strokeWeight{1.0f},
          strokeCap{StrokeCap::round},
          strokeJoin{StrokeJoin::miter},
          blendMode{BlendMode::alpha},
          angleMode{AngleMode::degrees},
          rectMode{RectMode::cornerSize},
          ellipseMode{EllipseMode::centerDiameter},
          imageMode{RectMode::cornerSize},
          shader(std::nullopt)
    {
    }
} // namespace processing

namespace processing
{
    // clang-format off
    void quit() { s_data.closeRequested = true; }
    void quit(const i32 exitCode) { setExitCode(exitCode); quit(); }
    void restart() { quit(); s_data.shouldRestart = true; }
    void setExitCode(const i32 exitCode) { s_data.exitCode = exitCode; }
    void loop() { s_data.isLoopPaused = false; }
    void noLoop() { s_data.isLoopPaused = true; }
    // clang-format on
} // namespace processing

namespace processing
{
    int2 getMousePosition()
    {
        double mx, my;
        glfwGetCursorPos(s_data.window, &mx, &my);
        return int2{static_cast<i32>(mx), static_cast<i32>(my)};
    }
} // namespace processing

namespace processing
{
    Image createImage(u32 width, u32 height, const u8* data, FilterMode filterMode, ExtendMode extendMode)
    {
        return s_data.images.createImage(width, height, data, filterMode, extendMode);
    }

    Image loadImage(const std::filesystem::path& filepath, FilterMode filterMode, ExtendMode extendMode)
    {
        return s_data.images.loadImage(filepath, filterMode, extendMode);
    }
} // namespace processing

namespace processing
{
    Renderbuffer createRenderbuffer(const u32 width, const u32 height, const FilterMode filterMode, const ExtendMode extendMode)
    {
        return s_data.renderbuffers.create(width, height, filterMode, extendMode);
    }
} // namespace processing

namespace processing
{
    Shader createShader(std::string_view vertexShaderSource, std::string_view fragmentShaderSource)
    {
        return s_data.shaders.create(vertexShaderSource, fragmentShaderSource);
    }
} // namespace processing

namespace processing
{
    void launch()
    {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
        glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

        // glfwWindowHint(GLFW_SAMPLES, 4);
        // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        s_data.window = glfwCreateWindow(800, 800, "Processing App", nullptr, nullptr);
        glfwMakeContextCurrent(s_data.window);
        glfwSwapInterval(1);

        glfwSetWindowCloseCallback(
            s_data.window, [](GLFWwindow*)
            {
                s_data.closeRequested = true;
            }
        );

        int w, h;
        glfwGetFramebufferSize(s_data.window, &w, &h);

        gladLoadGL(&glfwGetProcAddress);

        glEnable(GL_BLEND);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        // glEnable(GL_MULTISAMPLE);

        initGraphics(800, 800);

        s_data.sketch = createSketch();
        beginDraw();
        s_data.sketch->setup();
        endDraw(w, h);

        while (not s_data.closeRequested)
        {
            ++s_data.frameCount;

            if (not s_data.isLoopPaused or s_data.isRedrawRequested or s_data.frameCount == 1)
            {
                glfwGetFramebufferSize(s_data.window, &w, &h);

                beginDraw();
                s_data.sketch->draw(0.0f);
                endDraw(w, h);
                glfwSwapBuffers(s_data.window);

                s_data.isRedrawRequested = false;
            }

            glfwPollEvents();
        }

        s_data.sketch->destroy();
        glfwTerminate();
    }
} // namespace processing

int main()
{
    using namespace processing;

    do
    {
        s_data = {};
        launch();
    } while (s_data.shouldRestart);

    return s_data.exitCode;
}
