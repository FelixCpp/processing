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
#include <numbers>

namespace processing
{
    inline constexpr float PI = std::numbers::pi_v<float>;
    inline constexpr float TAU = 2.0f * PI;
    inline constexpr float E = std::numbers::e_v<float>;
} // namespace processing

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
    constexpr matrix4x4 matrix4x4_multiply(const matrix4x4& lhs, const matrix4x4& rhs);
    constexpr matrix4x4 matrix4x4_orthographic(float left, float top, float width, float height, float near, float far);

    constexpr float3 matrix4x4_transform_point(const matrix4x4& matrix, const float3& point);
    constexpr float2 matrix4x4_transform_vector(const matrix4x4& matrix, const float2& vector);

    matrix4x4 matrix4x4_rotation_z(float angle);
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
    enum class KeyCode
    {
        a,
        b,
        c,
        d,
        e,
        f,
        g,
        h,
        i,
        j,
        k,
        l,
        m,
        n,
        o,
        p,
        q,
        r,
        s,
        t,
        u,
        v,
        w,
        x,
        y,
        z,
        num1,
        num2,
        num3,
        num4,
        num5,
        num6,
        num7,
        num8,
        num9,
        num0,
        space,
        minus,
        equal,
        leftBracket,
        rightBracket,
        backslash,
        semicolon,
        apostrophe,
        graveAccent,
        comma,
        period,
        slash,
        world1,
        world2,
        escape,
        f1,
        f2,
        f3,
        f4,
        f5,
        f6,
        f7,
        f8,
        f9,
        f10,
        f11,
        f12,
        f13,
        f14,
        f15,
        f16,
        f17,
        f18,
        f19,
        f20,
        f21,
        f22,
        f23,
        f24,
        f25,
        up,
        down,
        left,
        right,
        leftShift,
        rightShift,
        leftControl,
        rightControl,
        leftAlt,
        rightAlt,
        tab,
        enter,
        backspace,
        insert,
        deleteKey,
        pageUp,
        pageDown,
        home,
        end,
        kp0,
        kp1,
        kp2,
        kp3,
        kp4,
        kp5,
        kp6,
        kp7,
        kp8,
        kp9,
        kpDivide,
        kpMultiply,
        kpSubtract,
        kpAdd,
        kpDecimal,
        kpEqual,
        kpEnter,
        printScreen,
        numLock,
        capsLock,
        scrollLock,
        pause,
        leftSuper,
        rightSuper,
        menu,
    };

    struct Event
    {
        enum Type
        {
            closed,
            window_resized,
            framebuffer_resized,
            mouse_moved,
            key_pressed,
            key_repeated,
            key_released,
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

        struct KeyEvent
        {
            KeyCode code;
        };

        union
        {
            SizeEvent size;
            MouseMoveEvent mouse_move;
            KeyEvent key;
        };
    };

    struct Sketch
    {
        virtual ~Sketch() = default;
        virtual void event(const Event& event) = 0;
        virtual void setup() = 0;
        virtual void draw() = 0;
        virtual void destroy() = 0;
    };

    extern std::unique_ptr<Sketch> createSketch();
} // namespace processing

namespace processing
{
    class DepthProvider
    {
    public:
        explicit DepthProvider(float min, float max, float increment);

        float getMinDepth() const;
        float getMaxDepth() const;
        float getNextDepth();
        void reset();

    private:
        float m_min;
        float m_max;
        float m_increment;
    };
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
    struct BlendMode
    {
        enum class Factor
        {
            zero,
            one,
            srcColor,
            oneMinusSrcColor,
            dstColor,
            oneMinusDstColor,
            srcAlpha,
            oneMinusSrcAlpha,
            dstAlpha,
            oneMinusDstAlpha
        };

        enum class Equation
        {
            add,
            subtract,
            reverseSubtract,
            min,
            max
        };

        constexpr BlendMode() = default;
        constexpr BlendMode(Factor sourceFactor, Factor destinationFactor, Equation blendEquation = Equation::add);
        constexpr BlendMode(Factor colorSourceFactor, Factor colorDestinationFactor, Equation colorBlendEquation, Factor alphaSourceFactor, Factor alphaDestinationFactor, Equation alphaBlendEquation);

        constexpr bool operator==(const BlendMode& rhs) const = default;
        constexpr bool operator!=(const BlendMode& rhs) const = default;

        static const BlendMode alpha;
        static const BlendMode add;
        static const BlendMode multiply;
        static const BlendMode opaque;
        static const BlendMode none;
        static const BlendMode additive;
        static const BlendMode screen;
        static const BlendMode subtract;
        static const BlendMode premultipliedAlpha;

        Factor colorSrcFactor{BlendMode::Factor::srcAlpha};
        Factor colorDstFactor{BlendMode::Factor::oneMinusSrcAlpha};
        Equation colorEquation{BlendMode::Equation::add};
        Factor alphaSrcFactor{BlendMode::Factor::one};
        Factor alphaDstFactor{BlendMode::Factor::oneMinusSrcAlpha};
        Equation alphaEquation{BlendMode::Equation::add};
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
    struct Shader
    {
        size_t id;

        constexpr bool operator==(const Shader& other) const = default;
        constexpr bool operator!=(const Shader& other) const = default;
    };

    struct ShaderHandleManager
    {
        virtual ~ShaderHandleManager() = default;
        virtual Shader loadShader(std::string_view vertexShaderSource, std::string_view fragmentShaderId) = 0;
        virtual uint32_t getResourceId(Shader id) const = 0;

        virtual void uploadUniform(Shader id, std::string_view name, float x) = 0;
        virtual void uploadUniform(Shader id, std::string_view name, float x, float y) = 0;
        virtual void uploadUniform(Shader id, std::string_view name, float x, float y, float z) = 0;
        virtual void uploadUniform(Shader id, std::string_view name, float x, float y, float z, float w) = 0;
    };

    Shader loadShader(std::string_view vertexShaderSource, std::string_view fragmentShaderId);
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
        std::optional<Shader> shaderProgramId;
        std::optional<TextureId> textureId;
        std::optional<BlendMode> blendMode;
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

    void pushMatrix();
    void popMatrix();
    void resetMatrix();
    matrix4x4& peekMatrix();
    void translate(float x, float y);
    void scale(float x, float y);
    void rotate(float angle);

    void blendMode(const BlendMode& blendMode);

    void shader(Shader shaderProgram);
    void noShader();
    void shaderUniform(std::string_view name, float x);
    void shaderUniform(std::string_view name, float x, float y);
    void shaderUniform(std::string_view name, float x, float y, float z);
    void shaderUniform(std::string_view name, float x, float y, float z, float w);

    void background(int red, int green, int blue, int alpha = 255);
    void background(int grey, int alpha = 255);
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

// #include <processing/processing.inl>

#endif // _PROCESSING_INCLUDE_LIBRARY_HPP_

#ifndef _PROCESSING_INCLUDE_PROCESSING_INL_
#define _PROCESSING_INCLUDE_PROCESSING_INL_

namespace processing
{
    // clang-format off
    template <typename T> constexpr value2<T>::value2() : x(T{}), y(T{}) { }
    template <typename T> constexpr value2<T>::value2(T x, T y) : x(x), y(y) { }
    template <typename T> constexpr value2<T>::value2(T scalar) : x(scalar), y(scalar) { }
    template <typename T> template <typename U> constexpr value2<T>::value2(const value2<U>& other) : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)) { }

    template <typename T>
    T value2_length(const value2<T>& value)
    {
        return std::hypot(value.x, value.y);
    }

    template <typename T>
    constexpr T value2_dot(const value2<T>& lhs, const value2<T>& rhs)
    {
        return lhs.x * rhs.x + lhs.y * rhs.y;
    }

    template <typename T>
    constexpr T value2_length_squared(const value2<T>& value)
    {
        return value.x * value.x + value.y * value.y;
    }

    template <typename T>
    constexpr value2<T> value2_perpendicular(const value2<T>& value)
    {
        return { value.y, -value.x };
    }

    template <typename T>
    value2<T> value2_normalized(const value2<T>& value)
    {
        const T length = value2_length(value);
        if (length != static_cast<T>(0)) {
            return { static_cast<T>(value.x / length), static_cast<T>(value.y / length) };
        }
        return value;
    }

    template <typename T>
    void value2_normalize(value2<T>& value)
    {
        return value = value2_normalized(value);
    }

    template <typename T> constexpr value2<T> operator+(const value2<T>& lhs, const value2<T>& rhs) { return { lhs.x + rhs.x, lhs.y + rhs.y }; }
    template <typename T> constexpr value2<T> operator-(const value2<T>& lhs, const value2<T>& rhs) { return { lhs.x - rhs.x, lhs.y - rhs.y }; }
    template <typename T> constexpr value2<T> operator*(const value2<T>& lhs, const value2<T>& rhs) { return { lhs.x * rhs.x, lhs.y * rhs.y }; }
    template <typename T> constexpr value2<T> operator/(const value2<T>& lhs, const value2<T>& rhs) { return { lhs.x / rhs.x, lhs.y / rhs.y }; }

    template <typename T> constexpr value2<T> operator+(const value2<T>& lhs, const T rhs) { return { lhs.x + rhs, lhs.y + rhs }; }
    template <typename T> constexpr value2<T> operator-(const value2<T>& lhs, const T rhs) { return { lhs.x - rhs, lhs.y - rhs }; }
    template <typename T> constexpr value2<T> operator*(const value2<T>& lhs, const T rhs) { return { lhs.x * rhs, lhs.y * rhs }; }
    template <typename T> constexpr value2<T> operator/(const value2<T>& lhs, const T rhs) { return { lhs.x / rhs, lhs.y / rhs }; }
    // clang-format on
} // namespace processing

namespace processing
{
    // clang-format off
    template <typename T> constexpr value3<T>::value3() : x(T{}), y(T{}), z(T{}) { }
    template <typename T> constexpr value3<T>::value3(const T x, const T y, const T z) : x(x), y(y), z(z) { }
    template <typename T> constexpr value3<T>::value3(T scalar) : x(scalar), y(scalar), z(scalar) { }
    template <typename T> constexpr value3<T>::value3(const value2<T>& xy, const T z) : x(xy.x), y(xy.y), z(z) {}
    template <typename T> constexpr value3<T>::value3(const T x, const value2<T>& yz) : x(x), y(yz.x), z(yz.y) {}
    // clang-format on
} // namespace processing

namespace processing
{
    // clang-format off
    constexpr matrix4x4 matrix4x4_create(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33
    )
    {
        return matrix4x4 {
            .data = {
                m00, m01, m02, m03,
                m10, m11, m12, m13,
                m20, m21, m22, m23,
                m30, m31, m32, m33,
            }
        };
    }

    constexpr matrix4x4 matrix4x4_identity()
    {
        return matrix4x4_create(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    constexpr matrix4x4 matrix4x4_translate(float x, float y, float z)
    {
        return matrix4x4_create(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            x, y, z, 1.0f
        );
    }

    constexpr matrix4x4 matrix4x4_scale(float x, float y, float z) {
        return matrix4x4_create(
            x, 0.0f, 0.0f, 0.0f,
            0.0f, y, 0.0f, 0.0f,
            0.0f, 0.0f, z, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    constexpr matrix4x4 matrix4x4_multiply(const matrix4x4 &lhs, const matrix4x4 &rhs)
    {
        matrix4x4 result;

        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result.data[i * 4 + j] =
                    lhs.data[i * 4 + 0] * rhs.data[0 * 4 + j] +
                    lhs.data[i * 4 + 1] * rhs.data[1 * 4 + j] +
                    lhs.data[i * 4 + 2] * rhs.data[2 * 4 + j] +
                    lhs.data[i * 4 + 3] * rhs.data[3 * 4 + j];
            }
        }

        return result;
    }

    constexpr matrix4x4 matrix4x4_orthographic(float left, float top, float width, float height, float near, float far) {
        const float right = left + width;
        const float bottom = top + height;

        return matrix4x4_create(
            2.0f / (right - left),  0.0f,                      0.0f,                      0.0f,
            0.0f,                   2.0f / (top - bottom),    0.0f,                      0.0f,
            0.0f,                   0.0f,                     -2.0f / (far - near),      0.0f,
           -(right + left) / (right - left),
           -(top + bottom) / (top - bottom),
           -(far + near) / (far - near),
            1.0f
        );
    }

    constexpr float3 matrix4x4_transform_point(const matrix4x4 &matrix, const float3 &point) {
        return {
            matrix.data[0] * point.x + matrix.data[4] * point.y + matrix.data[8] * point.z + matrix.data[12],
            matrix.data[1] * point.x + matrix.data[5] * point.y + matrix.data[9] * point.z + matrix.data[13],
            matrix.data[2] * point.x + matrix.data[6] * point.y + matrix.data[10] * point.z + matrix.data[14],
        };
    }

    constexpr float2 matrix4x4_transform_vector(const matrix4x4 &matrix, const float2 &vector) {
        return {
            matrix.data[0] * vector.x + matrix.data[4] * vector.y,
            matrix.data[1] * vector.x + matrix.data[5] * vector.y,
        };
    }

    inline matrix4x4 matrix4x4_rotation_z(float angle)
    {
        const float radians = angle * PI / 180.0f;
        const float cos = std::cos(radians);
        const float sin = std::sin(radians);

        return matrix4x4_create(
            cos, sin, 0.0f, 0.0f,
            -sin, cos, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }
    // clang-format on
} // namespace processing

namespace processing
{
    // clang-format off
    template <typename T> constexpr rect2<T>::rect2() : left(T{}), top(T{}), width(T{}), height(T{}) {}
    template <typename T> constexpr rect2<T>::rect2(T left, T top, T width, T height) : left(left), top(top), width(width), height(height) {}
    template <typename T> template <typename U> constexpr rect2<T>::rect2(const rect2<U>& other): left(static_cast<T>(other.left)), top(static_cast<T>(other.top)), width(static_cast<T>(other.width)), height(static_cast<T>(other.height)) {}
    template <typename T> constexpr value2<T> rect2<T>::center() const { return { left + width / 2, top + height / 2 }; }
    template <typename T> constexpr T rect2<T>::right() const { return left + width; }
    template <typename T> constexpr T rect2<T>::bottom() const { return top + height; }
    template <typename T> constexpr bool operator == (const rect2<T>& lhs, const rect2<T>& rhs) { return lhs.left == rhs.left and lhs.top == rhs.top and lhs.width == rhs.width and lhs.height == rhs.height; }
    template <typename T> constexpr bool operator != (const rect2<T>& lhs, const rect2<T>& rhs) { return lhs.left != rhs.left or lhs.top != rhs.top or lhs.width != rhs.width or lhs.height != rhs.height; }
    // clang-format on
} // namespace processing

namespace processing
{
    // clang-format off
    constexpr bool operator == (const TextureId& lhs, const TextureId& rhs) { return lhs.value == rhs.value; }
    constexpr bool operator != (const TextureId& lhs, const TextureId& rhs) { return lhs.value != rhs.value; }
    // clang-format on
} // namespace processing

namespace processing
{
    // clang-format off
    constexpr color_t color(int32_t red, int32_t green, int32_t blue, int32_t alpha) { return color_t{ .value = (uint32_t)(red << 24) | (uint32_t)(green << 16) | (uint32_t)(blue << 8) | (uint32_t)alpha }; }
    constexpr color_t color(int32_t grey, int32_t alpha) { return color(grey, grey, grey, alpha); }
    constexpr int32_t red(color_t color) { return (color.value & 0xFF000000) >> 24; }
    constexpr int32_t green(color_t color) { return (color.value & 0x00FF0000) >> 16; }
    constexpr int32_t blue(color_t color) { return (color.value & 0x0000FF00) >> 8; }
    constexpr int32_t alpha(color_t color) { return (color.value & 0x000000FF); }
    // clang-format on
} // namespace processing

namespace processing
{
    inline constexpr StrokeCap StrokeCap::butt = {.start = StrokeCapStyle::butt, .end = StrokeCapStyle::butt};
    inline constexpr StrokeCap StrokeCap::square = {.start = StrokeCapStyle::square, .end = StrokeCapStyle::square};
    inline constexpr StrokeCap StrokeCap::round = {.start = StrokeCapStyle::round, .end = StrokeCapStyle::round};
} // namespace processing

namespace processing
{
    // clang-format off
    inline constexpr Shader INVALID_SHADER_HANDLE = { .id = 0 };
    // clang-format on
} // namespace processing

namespace processing
{
    constexpr BlendMode::BlendMode(Factor sourceFactor, Factor destinationFactor, Equation blendEquation)
        : colorSrcFactor(sourceFactor),
          colorDstFactor(destinationFactor),
          colorEquation(blendEquation),
          alphaSrcFactor(sourceFactor),
          alphaDstFactor(destinationFactor),
          alphaEquation(blendEquation)
    {
    }

    constexpr BlendMode::BlendMode(Factor colorSourceFactor, Factor colorDestinationFactor, Equation colorBlendEquation, Factor alphaSourceFactor, Factor alphaDestinationFactor, Equation alphaBlendEquation)
        : colorSrcFactor(colorSourceFactor),
          colorDstFactor(colorDestinationFactor),
          colorEquation(colorBlendEquation),
          alphaSrcFactor(alphaSourceFactor),
          alphaDstFactor(alphaDestinationFactor),
          alphaEquation(alphaBlendEquation)
    {
    }

    inline constexpr BlendMode BlendMode::alpha{Factor::srcAlpha, Factor::oneMinusSrcAlpha};
    inline constexpr BlendMode BlendMode::add{Factor::srcAlpha, Factor::one};
    inline constexpr BlendMode BlendMode::multiply{Factor::dstColor, Factor::zero};
    inline constexpr BlendMode BlendMode::opaque{Factor::one, Factor::zero};
    inline constexpr BlendMode BlendMode::none{Factor::one, Factor::zero};
    inline constexpr BlendMode BlendMode::additive{Factor::one, Factor::one};
    inline constexpr BlendMode BlendMode::screen{Factor::one, Factor::oneMinusSrcColor};
    inline constexpr BlendMode BlendMode::subtract{Factor::srcAlpha, Factor::one, Equation::reverseSubtract};
    inline constexpr BlendMode BlendMode::premultipliedAlpha{Factor::one, Factor::oneMinusSrcAlpha};
} // namespace processing

#endif // _PROCESSING_INCLUDE_PROCESSING_INL_
