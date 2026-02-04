#include <processing/processing.hpp>
#include <processing/image.hpp>
#include <processing/renderbuffer.hpp>
#include <processing/renderer.hpp>

#include <GLFW/glfw3.h>
#include <cmath>

namespace processing
{
    struct LibraryData
    {
        bool closeRequested;
        bool shouldRestart;
        i32 exitCode;

        ImageAssetHandler images;
        RenderbufferAssetHandler renderbuffers;

        std::unique_ptr<Sketch> sketch;
        std::unique_ptr<Graphics> graphics;
    };

    inline static LibraryData s_data;
} // namespace processing

namespace processing
{
    // clang-format off
    template <typename T> value2<T>::value2() : x(T{}), y(T{}) {}
    template <typename T> value2<T>::value2(T x, T y) : x(x), y(y) {}
    template <typename T> value2<T>::value2(const T scalar) : x(scalar), y(scalar) {}
    template <typename T> template <typename U> value2<T>::value2(const value2<U>& other) : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)) {}
    template <typename T> T value2<T>::length() const { return std::hypot(x, y); }
    template <typename T> T value2<T>::lengthSquared() const { return x * x + y * y; }
    template <typename T> T value2<T>::dot(const value2<T>& other) const { return x * other.x + y * other.y; }
    template <typename T> value2<T> value2<T>::perpendicular_cw() const { return { y, -x }; }
    template <typename T> value2<T> value2<T>::perpendicular_ccw() const { return { -y, x }; }
    template <typename T> value2<T> value2<T>::normalized() const { const T len = length(); if (len != static_cast<T>(0.0)) { return { x / len, y / len }; } return this; }
    template <typename T> value2<T> value2<T>::operator+(const value2<T>& rhs) const { return { x + rhs.x, y + rhs.y }; }
    template <typename T> value2<T> value2<T>::operator-(const value2<T>& rhs) const { return { x - rhs.x, y - rhs.y }; }
    template <typename T> value2<T> value2<T>::operator*(const value2<T>& rhs) const { return { x * rhs.x, y * rhs.y }; }
    template <typename T> value2<T> value2<T>::operator/(const value2<T>& rhs) const { return { x / rhs.x, y / rhs.y }; }
    template <typename T> value2<T> value2<T>::operator+(T rhs) const { return { x + rhs.x, y + rhs.y }; }
    template <typename T> value2<T> value2<T>::operator-(T rhs) const { return { x - rhs.x, y - rhs.y }; }
    template <typename T> value2<T> value2<T>::operator*(T rhs) const { return { x * rhs.x, y * rhs.y }; }
    template <typename T> value2<T> value2<T>::operator/(T rhs) const { return { x / rhs.x, y / rhs.y }; }
    // clang-format on
} // namespace processing

namespace processing
{
    // clang-format off
    template <typename T> value3<T>::value3() : x(T{}), y(T{}), z(T{}) {}
    template <typename T> value3<T>::value3(T x, T y, T z) : x(x), y(y), z(z) {}
    template <typename T> value3<T>::value3(T scalar) : x(scalar), y(scalar), z(scalar) {}
    template <typename T> template <typename U> value3<T>::value3(const value3<U>& other) : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)), z(static_cast<T>(other.z)) {}
    template <typename T> value3<T>::value3(const value2<T>& xy, T z) : x(xy.x), y(xy.y), z(z) {}
    template <typename T> value3<T>::value3(T x, const value2<T>& yz) : x(x), y(yz.x), z(yz.y) {}
    template <typename T> T value3<T>::length() const { return std::sqrt(lengthSquared()); }
    template <typename T> T value3<T>::lengthSquared() const { return x * x + y * y + z * z; }
    template <typename T> value3<T> value3<T>::operator+(const value3& rhs) const { return { x + rhs.x, y + rhs.y, z + rhs.z }; }
    template <typename T> value3<T> value3<T>::operator-(const value3& rhs) const { return { x - rhs.x, y - rhs.y, z - rhs.z }; }
    template <typename T> value3<T> value3<T>::operator*(const value3& rhs) const { return { x * rhs.x, y * rhs.y, z * rhs.z }; }
    template <typename T> value3<T> value3<T>::operator/(const value3& rhs) const { return { x / rhs.x, y / rhs.y, z / rhs.z }; }
    template <typename T> value3<T> value3<T>::operator+(T rhs) const { return { x + rhs, y + rhs, z + rhs }; }
    template <typename T> value3<T> value3<T>::operator-(T rhs) const { return { x - rhs, y - rhs, z - rhs }; }
    template <typename T> value3<T> value3<T>::operator*(T rhs) const { return { x * rhs, y * rhs, z * rhs }; }
    template <typename T> value3<T> value3<T>::operator/(T rhs) const { return { x / rhs, y / rhs, z / rhs }; }
    // clang-format on
} // namespace processing

namespace processing
{
    // clang-format off
    template <typename T> value4<T>::value4() : x(T{}), y(T{}), z(T{}), w(T{}) {}
    template <typename T> value4<T>::value4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
    template <typename T> value4<T>::value4(T scalar) : x(scalar), y(scalar), z(scalar), w(scalar) {}
    template <typename T> template <typename U> value4<T>::value4(const value4<U>& other) : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)), z(static_cast<T>(other.z)), w(static_cast<T>(other.w)) {}
    template <typename T> T value4<T>::length() const { return std::sqrt(lengthSquared()); }
    template <typename T> T value4<T>::lengthSquared() const { return x * x + y * y + z * z + w * w; }
    template <typename T> value4<T> value4<T>::operator+(const value4& rhs) const { return { x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w }; }
    template <typename T> value4<T> value4<T>::operator-(const value4& rhs) const { return { x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w }; }
    template <typename T> value4<T> value4<T>::operator*(const value4& rhs) const { return { x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w }; }
    template <typename T> value4<T> value4<T>::operator/(const value4& rhs) const { return { x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w }; }
    template <typename T> value4<T> value4<T>::operator+(T rhs) const { return { x + rhs, y + rhs, z + rhs, w + rhs }; }
    template <typename T> value4<T> value4<T>::operator-(T rhs) const { return { x - rhs, y - rhs, z - rhs, w - rhs }; }
    template <typename T> value4<T> value4<T>::operator*(T rhs) const { return { x * rhs, y * rhs, z * rhs, w * rhs }; }
    template <typename T> value4<T> value4<T>::operator/(T rhs) const { return { x / rhs, y / rhs, z / rhs, w / rhs }; }
    // clang-format on
} // namespace processing

namespace processing
{
    // clang-format off
    template <typename T> rect2<T>::rect2(): left(static_cast<T>(0)), top(static_cast<T>(0)), width(static_cast<T>(0)), height(static_cast<T>(0)) {}
    template <typename T> rect2<T>::rect2(T left, T top, T width, T height): left(left), top(top), width(width), height(height) {}
    template <typename T> rect2<T>::rect2(const value2<T>& position, const value2<T>& size): position(position), size(size) {}
    template <typename T> T rect2<T>::right() const { return left + width; }
    template <typename T> T rect2<T>::bottom() const { return top + height; }
    template <typename T> value2<T> rect2<T>::center() const { return position + size / 2; }
    // clang-format on
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

    float2 matrix4x4::transformPoint(const float2& point) const
    {
        return point;
    }

    float3 matrix4x4::transformPoint(const float3& point) const
    {
        return point;
    }
} // namespace processing

namespace processing
{
    // clang-format off
    void quit() { s_data.closeRequested = true; }
    void quit(const i32 exitCode) { setExitCode(exitCode); quit(); }
    void restart() { quit(); s_data.shouldRestart = true; }
    void setExitCode(const i32 exitCode) { s_data.exitCode = exitCode; }
    // clang-format on
} // namespace processing

namespace processing
{
    Image createImage(u32 width, u32 height, FilterMode filterMode, ExtendMode extendMode)
    {
        return s_data.images.createImage(width, height, filterMode, extendMode);
    }

    Image loadImage(const std::filesystem::path& filepath, FilterMode filterMode, ExtendMode extendMode)
    {
        return s_data.images.loadImage(filepath, filterMode, extendMode);
    }

    Renderbuffer createRenderbuffer(u32 width, u32 height)
    {
        return s_data.renderbuffers.create(width, height);
    }
} // namespace processing

namespace processing
{
    void launch()
    {
        glfwInit();
        GLFWwindow* window = glfwCreateWindow(800, 800, "Processing App", nullptr, nullptr);
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        std::shared_ptr<Renderer> renderer = DefaultRenderer::create();
        Renderbuffer buffer = createRenderbuffer(800, 800);
        s_data.graphics = std::make_unique<Graphics>(renderer, buffer);

        s_data.sketch = createSketch();
        s_data.sketch->setup();

        while (not s_data.closeRequested)
        {
            glfwPollEvents();
            s_data.graphics->beginDraw();
            s_data.sketch->draw(0.0f);
            s_data.graphics->endDraw();
            glfwSwapBuffers(window);
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
