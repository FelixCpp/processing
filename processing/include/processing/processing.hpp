#ifndef _PROCESSING_INCLUDE_LIBRARY_HPP_
#define _PROCESSING_INCLUDE_LIBRARY_HPP_

#include <cstdint>
#include <cmath>
#include <string>
#include <memory>
#include <array>
#include <span>
#include <optional>
#include <string_view>
#include <filesystem>

namespace processing
{
    void trace(const std::string& message);
    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
} // namespace processing

namespace processing
{
    void close();
    void close(int exitCode);
    void restart();

    void setExitCode(int exitCode);
    int getExitCode();

    void loop();
    void noLoop();
    void redraw();
} // namespace processing

namespace processing
{
    template <typename T>
    struct value2
    {
        constexpr value2();
        constexpr value2(T x, T y);
        constexpr explicit value2(T scalar);

        template <typename U>
        constexpr explicit value2(const value2<U>& other);

        T x, y;
    };

    // clang-format off
    template <typename T> T value2_length(const value2<T>& value);
    template <typename T> constexpr T value2_length_squared(const value2<T>& value);
    template <typename T> constexpr T value2_dot(const value2<T>& lhs, const value2<T>& rhs);
    template <typename T> constexpr value2<T> value2_perpendicular(const value2<T>& value);
    template <typename T> value2<T> value2_normalized(const value2<T>& value);
    template <typename T> void value2_normalize(value2<T>& value);

    template <typename T> constexpr value2<T> operator+(const value2<T>& lhs, const value2<T>& rhs);
    template <typename T> constexpr value2<T> operator-(const value2<T>& lhs, const value2<T>& rhs);
    template <typename T> constexpr value2<T> operator*(const value2<T>& lhs, const value2<T>& rhs);
    template <typename T> constexpr value2<T> operator/(const value2<T>& lhs, const value2<T>& rhs);

    template <typename T> constexpr value2<T> operator+(const value2<T>& lhs, T rhs);
    template <typename T> constexpr value2<T> operator-(const value2<T>& lhs, T rhs);
    template <typename T> constexpr value2<T> operator*(const value2<T>& lhs, T rhs);
    template <typename T> constexpr value2<T> operator/(const value2<T>& lhs, T rhs);
    // clang-format on

    using int2 = value2<int32_t>;
    using uint2 = value2<uint32_t>;
    using float2 = value2<float>;
} // namespace processing

namespace processing
{
    template <typename T>
    struct value3
    {
        constexpr value3();
        constexpr value3(T x, T y, T z);
        constexpr explicit value3(T scalar);

        constexpr explicit value3(const value2<T>& xy, T z);
        constexpr explicit value3(T x, const value2<T>& yz);

        T x, y, z;
    };

    using int3 = value3<int32_t>;
    using uint3 = value3<uint32_t>;
    using float3 = value3<float>;
} // namespace processing

namespace processing
{
    template <typename T>
    struct value4
    {
        T x, y, z, w;
    };

    using int4 = value4<int32_t>;
    using uint4 = value4<uint32_t>;
    using float4 = value4<float>;
} // namespace processing

namespace processing
{
    struct matrix4x4
    {
        std::array<float, 16> data;
    };

    constexpr matrix4x4 matrix4x4_create(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33
    );

    constexpr matrix4x4 matrix4x4_identity();

    constexpr matrix4x4 matrix4x4_translate(float x, float y, float z);
    constexpr matrix4x4 matrix4x4_scale(float x, float y, float z);
    constexpr matrix4x4 matrix4x4_orthographic(float left, float top, float width, float height, float near, float far);

    constexpr float3 transformPoint(const matrix4x4& matrix, const float3& point);
    constexpr float2 transformVector(const matrix4x4& matrix, const float2& vector);
} // namespace processing

namespace processing
{
    template <typename T>
    struct rect2
    {
        constexpr rect2();
        constexpr rect2(T left, T top, T width, T height);

        template <typename U>
        constexpr rect2(const rect2<U>& other);

        constexpr value2<T> center() const;

        constexpr T right() const;
        constexpr T bottom() const;

        union
        {
            struct
            {
                T left, top;
            };

            value2<T> position;
        };

        union
        {
            struct
            {
                T width, height;
            };

            value2<T> size;
        };
    };

    // clang-format off
    template <typename T> constexpr bool operator == (const rect2<T>& lhs, const rect2<T>& rhs);
    template <typename T> constexpr bool operator != (const rect2<T>& lhs, const rect2<T>& rhs);
    // clang-format on

    using rect2i = rect2<int32_t>;
    using rect2u = rect2<uint32_t>;
    using rect2f = rect2<float>;
} // namespace processing

namespace processing
{
    struct Event
    {
        enum Type
        {
            closed,
            window_resized,
            framebuffer_resized,
            mouse_moved,
        } type;

        struct SizeEvent
        {
            uint32_t width;
            uint32_t height;
        };

        struct MouseMoveEvent
        {
            int32_t x;
            int32_t y;
        };

        union
        {
            SizeEvent size;
            MouseMoveEvent mouse_move;
        };
    };

    struct Sketch
    {
        virtual ~Sketch() = default;
        virtual void setup() = 0;
        virtual void draw() = 0;
        virtual void destroy() = 0;
    };

    extern std::unique_ptr<Sketch> createSketch();
} // namespace processing

namespace processing
{
    void setWindowSize(uint32_t width, uint32_t height);
    uint2 getWindowSize();
    void setWindowTitle(std::string_view title);
    std::string getWindowTitle();
    int2 getMousePosition();
} // namespace processing

namespace processing
{
    using color_t = struct
    {
        uint32_t value;
    };

    constexpr color_t color(int32_t red, int32_t green, int32_t blue, int32_t alpha = 255);
    constexpr color_t color(int32_t grey, int32_t alpha = 255);
    constexpr int32_t red(color_t color);
    constexpr int32_t green(color_t color);
    constexpr int32_t blue(color_t color);
    constexpr int32_t alpha(color_t color);
} // namespace processing

namespace processing
{
    struct RenderTarget
    {
        virtual ~RenderTarget() = default;
        virtual void activate() = 0;
    };
} // namespace processing

namespace processing
{
    struct Vertex
    {
        float3 position;
        float2 texcoord;
        float4 color;
    };
} // namespace processing

namespace processing
{
    using ShaderProgramId = struct
    {
        uint32_t value;
    };

    constexpr bool operator==(const ShaderProgramId& lhs, const ShaderProgramId& rhs);
    constexpr bool operator!=(const ShaderProgramId& lhs, const ShaderProgramId& rhs);

    struct ShaderProgram
    {
        virtual ~ShaderProgram() = default;
        virtual ShaderProgramId getResourceId() const = 0;
    };
} // namespace processing

namespace processing
{
    using TextureId = struct
    {
        uint32_t value;
    };

    constexpr bool operator==(const TextureId& lhs, const TextureId& rhs);
    constexpr bool operator!=(const TextureId& lhs, const TextureId& rhs);

    struct TextureImpl
    {
        virtual ~TextureImpl() = default;
        virtual uint2 getSize() const = 0;
        virtual TextureId getResourceId() const = 0;
    };

    class Texture
    {
    public:
        Texture() = default;
        explicit Texture(std::unique_ptr<TextureImpl> impl);

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;
        Texture(Texture&&) = default;
        Texture& operator=(Texture&&) = default;

        uint2 getSize() const;
        TextureId getResourceId() const;

    private:
        std::unique_ptr<TextureImpl> m_impl;
    };

    Texture loadTexture(const std::filesystem::path& filepath);
} // namespace processing

namespace processing
{
    struct RenderingSubmission
    {
        std::span<const Vertex> vertices;
        std::span<const uint32_t> indices;
        std::optional<ShaderProgramId> shaderProgramId;
        std::optional<TextureId> textureId;
    };

    struct ProjectionDetails
    {
        matrix4x4 projectionMatrix;
        matrix4x4 viewMatrix;
    };

    struct Renderer
    {
        virtual ~Renderer() = default;
        virtual void beginDraw(const ProjectionDetails& details) = 0;
        virtual void endDraw() = 0;
        virtual void submit(const RenderingSubmission& submission) = 0;
        virtual void flush() = 0;
    };
} // namespace processing

namespace processing
{
    enum class StrokeJoin
    {
        miter,
        bevel,
        round,
    };

    enum class StrokeCapStyle
    {
        butt,
        square,
        round,
    };

    struct StrokeCap
    {
        StrokeCapStyle start;
        StrokeCapStyle end;

        static const StrokeCap butt;
        static const StrokeCap square;
        static const StrokeCap round;
    };

    using RectMode = rect2f (*)(float, float, float, float);
    RectMode rect_mode_ltwh();
    RectMode rect_mode_ltrb();
    RectMode rect_mode_center_size();

    using EllipseMode = rect2f (*)(float, float, float, float);
    EllipseMode ellipse_mode_ltwh();
    EllipseMode ellipse_mode_ltrb();
    EllipseMode ellipse_mode_center_radius();
    EllipseMode ellipse_mode_center_diameter();

    using ImageSourceMode = rect2f (*)(uint2, float, float, float, float);
    ImageSourceMode image_source_mode_ltwh_normalized();
    ImageSourceMode image_source_mode_ltwh_coordinates();
} // namespace processing

namespace processing
{
    void pushState();
    void popState();

    rect2f getViewport();

    void strokeJoin(StrokeJoin strokeJoin);
    void strokeCap(StrokeCap strokeCap);

    void background(int red, int green, int blue, int alpha = 255);
    void background(int gmey, int alpha = 255);
    void background(color_t color);

    void fill(int red, int green, int blue, int alpha = 255);
    void fill(int grey, int alpha = 255);
    void fill(color_t color);
    void noFill();

    void stroke(int red, int green, int blue, int alpha = 255);
    void stroke(int grey, int alpha = 255);
    void stroke(color_t color);
    void noStroke();

    void imageMode(RectMode imageMode);
    void imageSourceMode(ImageSourceMode imageSourceMode);
    void imageTint(int red, int green, int blue, int alpha = 255);
    void imageTint(int grey, int alpha = 255);
    void imageTint(color_t color);

    void strokeWeight(float strokeWeight);
    void rectMode(RectMode rectMode);
    void ellipseMode(EllipseMode ellipseMode);

    void rect(float left, float top, float width, float height);
    void square(float left, float top, float size);
    void ellipse(float centerX, float centerY, float radiusX, float radiusY);
    void circle(float centerX, float centerY, float radius);
    void line(float x1, float y1, float x2, float y2);
    void triangle(float x1, float y1, float x2, float y2, float x3, float y3);
    void point(float x, float y);
    void image(const Texture& texture, float x1, float y1);
    void image(const Texture& texture, float x1, float y1, float x2, float y2);
    void image(const Texture& texture, float x1, float y1, float x2, float y2, float sx1, float sy1, float sx2, float sy2);
} // namespace processing

#include <processing/processing.inl>

#endif // _PROCESSING_INCLUDE_LIBRARY_HPP_
