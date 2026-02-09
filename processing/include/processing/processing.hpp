#ifndef _PROCESSING_INCLUDE_PROCESSING_HPP_
#define _PROCESSING_INCLUDE_PROCESSING_HPP_

#include <cstdint>
#include <cmath>

#include <array>
#include <numbers>
#include <memory>
#include <vector>
#include <filesystem>
#include <string_view>
#include <optional>

namespace processing
{
    using i8 = int8_t;
    using u8 = uint8_t;
    using i16 = int16_t;
    using u16 = uint16_t;
    using i32 = int32_t;
    using u32 = uint32_t;
    using i64 = int64_t;
    using u64 = uint64_t;

    using f32 = float;
    using f64 = double;

    using usize = size_t;
} // namespace processing

namespace processing
{
    struct Sketch
    {
        virtual ~Sketch() = default;
        virtual void setup() = 0;
        virtual void draw(f32 deltaTime) = 0;
        virtual void destroy() = 0;
    };

    extern std::unique_ptr<Sketch> createSketch();
} // namespace processing

namespace processing
{
    template <typename T>
    struct value2
    {
        value2();
        value2(T x, T y);
        explicit value2(T scalar);

        template <typename U>
        explicit value2(const value2<U>& other);

        T length() const;
        T lengthSquared() const;
        T dot(const value2& other) const;

        value2 perpendicular_cw() const;
        value2 perpendicular_ccw() const;

        value2 normalized() const;

        bool operator==(const value2<T>& rhs) const = default;
        bool operator!=(const value2<T>& rhs) const = default;

        value2<T> operator+(const value2<T>& rhs) const;
        value2<T> operator-(const value2<T>& rhs) const;
        value2<T> operator*(const value2<T>& rhs) const;
        value2<T> operator/(const value2<T>& rhs) const;

        value2<T> operator+(T rhs) const;
        value2<T> operator-(T rhs) const;
        value2<T> operator*(T rhs) const;
        value2<T> operator/(T rhs) const;

        T x, y;
    };

    using double2 = value2<f64>;
    using float2 = value2<f32>;
    using uint2 = value2<u32>;
    using int2 = value2<i32>;
} // namespace processing

namespace processing
{
    template <typename T>
    struct value3
    {
        value3();
        value3(T x, T y, T z);
        explicit value3(T scalar);

        template <typename U>
        explicit value3(const value3<U>& other);

        explicit value3(const value2<T>& xy, T z);
        explicit value3(T x, const value2<T>& yz);

        T length() const;
        T lengthSquared() const;

        bool operator==(const value3& other) const = default;
        bool operator!=(const value3& other) const = default;

        value3<T> operator+(const value3& rhs) const;
        value3<T> operator-(const value3& rhs) const;
        value3<T> operator*(const value3& rhs) const;
        value3<T> operator/(const value3& rhs) const;

        value3<T> operator+(T rhs) const;
        value3<T> operator-(T rhs) const;
        value3<T> operator*(T rhs) const;
        value3<T> operator/(T rhs) const;

        T x, y, z;
    };

    using double3 = value3<f64>;
    using float3 = value3<f32>;
    using uint3 = value3<u32>;
    using int3 = value3<i32>;
} // namespace processing

namespace processing
{
    template <typename T>
    struct value4
    {
        value4();
        value4(T x, T y, T z, T w);
        explicit value4(T scalar);

        template <typename U>
        explicit value4(const value4<U>& other);

        T length() const;
        T lengthSquared() const;

        bool operator==(const value4<T>& other) const = default;
        bool operator!=(const value4<T>& other) const = default;

        value4<T> operator+(const value4<T>& rhs) const;
        value4<T> operator-(const value4<T>& rhs) const;
        value4<T> operator*(const value4<T>& rhs) const;
        value4<T> operator/(const value4<T>& rhs) const;

        value4<T> operator+(T rhs) const;
        value4<T> operator-(T rhs) const;
        value4<T> operator*(T rhs) const;
        value4<T> operator/(T rhs) const;

        T x, y, z, w;
    };

    using double4 = value4<f64>;
    using float4 = value4<f32>;
    using uint4 = value4<u32>;
    using int4 = value4<i32>;
} // namespace processing

namespace processing
{
    template <typename T>
    struct rect2
    {
        rect2();
        rect2(T left, T top, T width, T height);
        rect2(const value2<T>& position, const value2<T>& size);

        T right() const;
        T bottom() const;

        value2<T> center() const;

        bool operator==(const rect2& other) const = default;
        bool operator!=(const rect2& other) const = default;

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

    using rect2f = rect2<f32>;
    using rect2d = rect2<f64>;
    using rect2u = rect2<u32>;
    using rect2i = rect2<i32>;
} // namespace processing

namespace processing
{
    struct matrix4x4
    {
        matrix4x4();

        matrix4x4(
            float m00, float m01, float m02, float m03,
            float m10, float m11, float m12, float m13,
            float m20, float m21, float m22, float m23,
            float m30, float m31, float m32, float m33
        );

        static matrix4x4 orthographic(f32 left, f32 top, f32 width, f32 height, f32 near, f32 far);
        static matrix4x4 translation(f32 x, f32 y);
        static matrix4x4 scaling(f32 x, f32 y);
        static matrix4x4 rotation(f32 angle);

        matrix4x4 combined(const matrix4x4& other) const;

        float2 transformPoint(const float2& point) const;
        float3 transformPoint(const float3& point) const;

        static const matrix4x4 identity;

        std::array<f32, 16> data;
    };
} // namespace processing

namespace processing
{
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
} // namespace processing

namespace processing
{
    enum class StrokeJoin
    {
        miter,
        bevel,
        round,
    };

    enum class BlendMode
    {
        opaque,        // kein Blending
        alpha,         // klassisches Alpha-Blending
        premultiplied, // premultiplied alpha
        additive,      // Glow / Partikel
        multiply,      // Schatten / Darken
        screen,        // Aufhellen
        subtract,      // Spezialeffekte
    };

    enum class AngleMode
    {
        degrees,
        radians,
    };

    enum class RectMode
    {
        cornerSize,
        corners,
        centerSize,
    };

    enum class EllipseMode
    {
        cornerDiameter,
        cornerRadius,
        corners,
        centerRadius,
        centerDiameter,
    };

    enum class ImageSourceMode
    {
        normal,
        size,
    };

    enum class ShapeMode
    {
        points,
        linesStrip,
        lineLoop,
        triangles,
        triangleStrip,
        triangleFan,
        quads,
        quadStrip,
    };
} // namespace processing

namespace processing
{
    struct Color
    {
        Color();
        Color(i32 red, i32 green, i32 blue, i32 alpha = 255);
        Color(i32 grey, i32 alpha = 255);

        i32 brightness() const;

        u8 r, g, b, a;
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

    enum class VertexMode
    {
        points,
        lines,
        lineStrip,
        lineLoop,
        triangles,
        triangleStrip,
        triangleFan,
    };

    struct Vertices
    {
        VertexMode mode;
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
    };
} // namespace processing

namespace processing
{
    struct ResourceId
    {
        u32 value;

        inline constexpr bool operator==(const ResourceId& other) const = default;
        inline constexpr bool operator!=(const ResourceId& other) const = default;
    };

    struct AssetId
    {
        usize value;

        inline constexpr bool operator==(const AssetId& other) const = default;
        inline constexpr bool operator!=(const AssetId& other) const = default;
    };
} // namespace processing

namespace processing
{
    enum class FilterModeType
    {
        linear,
        nearest,
    };

    struct FilterMode
    {
        FilterModeType min;
        FilterModeType mag;

        bool operator==(const FilterMode& other) const = default;
        bool operator!=(const FilterMode& other) const = default;

        static const FilterMode linear;
        static const FilterMode nearest;
    };
} // namespace processing

namespace processing
{
    enum class ExtendModeType
    {
        clamp,
        repeat,
        mirroredRepeat,
    };

    struct ExtendMode
    {
        ExtendModeType horizontal;
        ExtendModeType vertical;

        bool operator==(const ExtendMode& other) const = default;
        bool operator!=(const ExtendMode& other) const = default;

        static const ExtendMode clamp;
        static const ExtendMode repeat;
        static const ExtendMode mirroredRepeat;
    };
} // namespace processing

namespace processing
{
    class PlatformImage;
    class Pixels
    {
    public:
        explicit Pixels(u32 width, u32 height, PlatformImage* parent, const std::vector<u8>& data);
        void set(u32 x, u32 y, Color color);
        Color get(u32 x, u32 y) const;

        void commit();

    private:
        u32 m_width;
        u32 m_height;
        PlatformImage* m_parent;
        std::vector<u8> m_data;
    };

    struct PlatformImage
    {
        virtual ~PlatformImage() = default;

        virtual void setFilterMode(FilterMode mode) = 0;
        virtual FilterMode getFilterMode() const = 0;

        virtual void setExtendMode(ExtendMode mode) = 0;
        virtual ExtendMode getExtendMode() const = 0;

        virtual uint2 getSize() const = 0;
        virtual Pixels loadPixels() = 0;

        virtual ResourceId getResourceId() const = 0;
    };

    class Image
    {
    public:
        Image();
        explicit Image(AssetId assetId, std::shared_ptr<PlatformImage> image);

        void setFilterMode(FilterMode mode);
        FilterMode getFilterMode() const;

        void setExtendMode(ExtendMode mode);
        ExtendMode getExtendMode() const;

        uint2 getSize() const;
        Pixels loadPixels();

        ResourceId getResourceId() const;
        AssetId getAssetId() const;

    private:
        AssetId m_assetId;
        std::shared_ptr<PlatformImage> m_impl;
    };

    Image createImage(u32 width, u32 height, const u8* data = nullptr, FilterMode filterMode = FilterMode::linear, ExtendMode extendMode = ExtendMode::clamp);
    Image loadImage(const std::filesystem::path& filepath, FilterMode filterMode = FilterMode::linear, ExtendMode extendMode = ExtendMode::clamp);
} // namespace processing

namespace processing
{
    struct PlatformRenderbuffer
    {
        virtual ~PlatformRenderbuffer() = default;
        virtual Image& getImage() = 0;
        virtual uint2 getSize() const = 0;
        virtual AssetId getAssetId() const = 0;
    };

    class Renderbuffer
    {
    public:
        Renderbuffer();
        explicit Renderbuffer(std::shared_ptr<PlatformRenderbuffer> impl);

        Image& getImage();
        uint2 getSize() const;
        AssetId getAssetId() const;

    private:
        std::shared_ptr<PlatformRenderbuffer> m_impl;
    };

    Renderbuffer createRenderbuffer(u32 width, u32 height, FilterMode filterMode = FilterMode::linear, ExtendMode extendMode = ExtendMode::clamp);
} // namespace processing

namespace processing
{
    struct PlatformShader
    {
        virtual ~PlatformShader() = default;
        virtual ResourceId getResourceId() const = 0;
    };

    class Shader
    {
    public:
        explicit Shader(AssetId assetId, std::shared_ptr<PlatformShader> impl);

        ResourceId getResourceId() const;
        AssetId getAssetId() const;

    private:
        AssetId m_assetId;
        std::shared_ptr<PlatformShader> m_impl;
    };

    Shader createShader(std::string_view vertexShaderSource, std::string_view fragmentShaderSource);
} // namespace processing

namespace processing
{
    struct RenderStyle
    {
        Color fillColor;
        Color strokeColor;
        Color tintColor;

        bool isFillEnabled;
        bool isStrokeEnabled;

        f32 strokeWeight;
        StrokeCap strokeCap;
        StrokeJoin strokeJoin;

        BlendMode blendMode;
        AngleMode angleMode;
        RectMode rectMode;
        EllipseMode ellipseMode;
        RectMode imageMode;
        ImageSourceMode imageSourceMode;

        std::optional<Shader> shader;

        RenderStyle();
    };
} // namespace processing

namespace processing
{
    void quit();
    void quit(i32 exitCode);
    void restart();
    void setExitCode(i32 exitCode);

    void loop();
    void noLoop();
    void redraw();
} // namespace processing

namespace processing
{
    void randomSeed(u64 seed);
    f32 random(f32 max);
    f32 random(f32 min, f32 max);
    f32 map(f32 value, f32 istart, f32 istop, f32 ostart, f32 ostop);
} // namespace processing

namespace processing
{
    int2 getMousePosition();
} // namespace processing

namespace processing
{
    void pushRenderbuffer(const Renderbuffer& renderbuffer);
    void popRenderbuffer();

    void push();
    void pop();

    void pushStyle(bool extendPreviousStyle = true);
    void popStyle();
    RenderStyle& peekStyle();

    void pushMatrix(bool extendPreviousMatrix = true);
    void popMatrix();
    void resetMatrix();
    void resetMatrix(const matrix4x4& matrix);
    matrix4x4& peekMatrix();
    void translate(f32 x, f32 y);
    void scale(f32 x, f32 y);
    void rotate(f32 rotation);

    void blendMode(BlendMode mode);
    void angleMode(AngleMode mode);
    void rectMode(RectMode mode);
    void ellipseMode(EllipseMode mode);
    void imageMode(RectMode mode);
    void imageSourceMode(ImageSourceMode mode);

    void shader(const Shader& shader);
    void noShader();

    void fill(i32 red, i32 green, i32 blue, i32 alpha = 255);
    void fill(i32 grey, i32 alpha = 255);
    void fill(Color color);
    void noFill();

    void stroke(i32 red, i32 green, i32 blue, i32 alpha = 255);
    void stroke(i32 grey, i32 alpha = 255);
    void stroke(Color color);
    void noStroke();

    void strokeWeight(f32 strokeWeight);
    void strokeCap(StrokeCap strokeCap);
    void strokeJoin(StrokeJoin strokeJoin);

    void tint(i32 red, i32 green, i32 blue, i32 alpha = 255);
    void tint(i32 grey, i32 alpha = 255);
    void tint(Color color);

    void background(i32 red, i32 green, i32 blue, i32 alpha = 255);
    void background(i32 grey, i32 alpha = 255);
    void background(Color color);

    void beginShape(ShapeMode mode);
    void endShape(bool closed = true);
    void vertex(f32 x, f32 y);
    void vertex(f32 x, f32 y, f32 u, f32 v);
    void bezierVertex(f32 x2, f32 y2, f32 x3, f32 y3, f32 x4, f32 y4);
    void quadraticVertex(f32 cx, f32 cy, f32 x3, f32 y3);
    void curveVertex(f32 x, f32 y);

    void rect(f32 x1, f32 y1, f32 x2, f32 y2);
    void square(f32 x1, f32 y1, f32 xy2);
    void ellipse(f32 x1, f32 y1, f32 x2, f32 y2);
    void circle(f32 x1, f32 y1, f32 xy2);
    void triangle(f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3);
    void point(f32 x, f32 y);
    void line(f32 x1, f32 y1, f32 x2, f32 y2);
    void image(const Image& img, f32 x1, f32 y1);
    void image(const Image& img, f32 x1, f32 y1, f32 x2, f32 y2);
    void image(const Image& img, f32 x1, f32 y1, f32 x2, f32 y2, f32 sx1, f32 sy1, f32 sx2, f32 sy2);
} // namespace processing

#endif // _PROCESSING_INCLUDE_PROCESSING_HPP_

#ifndef _PROCESSING_INCLUDE_PROCESSING_INL_
#define _PROCESSING_INCLUDE_PROCESSING_INL_

#include <numbers>

namespace processing
{
    inline constexpr f32 PI = std::numbers::pi_v<f32>;
    inline constexpr f32 TAU = 2.0f * PI;
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
    template <typename T> value2<T> value2<T>::normalized() const { const T len = length(); if (len != static_cast<T>(0.0)) { return { x / len, y / len }; } return *this; }
    template <typename T> value2<T> value2<T>::operator+(const value2<T>& rhs) const { return { x + rhs.x, y + rhs.y }; }
    template <typename T> value2<T> value2<T>::operator-(const value2<T>& rhs) const { return { x - rhs.x, y - rhs.y }; }
    template <typename T> value2<T> value2<T>::operator*(const value2<T>& rhs) const { return { x * rhs.x, y * rhs.y }; }
    template <typename T> value2<T> value2<T>::operator/(const value2<T>& rhs) const { return { x / rhs.x, y / rhs.y }; }
    template <typename T> value2<T> value2<T>::operator+(T rhs) const { return { x + rhs, y + rhs }; }
    template <typename T> value2<T> value2<T>::operator-(T rhs) const { return { x - rhs, y - rhs }; }
    template <typename T> value2<T> value2<T>::operator*(T rhs) const { return { x * rhs, y * rhs }; }
    template <typename T> value2<T> value2<T>::operator/(T rhs) const { return { x / rhs, y / rhs }; }
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
    inline constexpr StrokeCap StrokeCap::butt = {.start = StrokeCapStyle::butt, .end = StrokeCapStyle::butt};
    inline constexpr StrokeCap StrokeCap::square = {.start = StrokeCapStyle::square, .end = StrokeCapStyle::square};
    inline constexpr StrokeCap StrokeCap::round = {.start = StrokeCapStyle::round, .end = StrokeCapStyle::round};
} // namespace processing

namespace processing
{
    inline constexpr FilterMode FilterMode::linear = {.min = FilterModeType::linear, .mag = FilterModeType::linear};
    inline constexpr FilterMode FilterMode::nearest = {.min = FilterModeType::nearest, .mag = FilterModeType::nearest};
} // namespace processing

namespace processing
{
    inline constexpr ExtendMode ExtendMode::clamp = {.horizontal = ExtendModeType::clamp, .vertical = ExtendModeType::clamp};
    inline constexpr ExtendMode ExtendMode::repeat = {.horizontal = ExtendModeType::repeat, .vertical = ExtendModeType::repeat};
    inline constexpr ExtendMode ExtendMode::mirroredRepeat = {.horizontal = ExtendModeType::mirroredRepeat, .vertical = ExtendModeType::mirroredRepeat};
} // namespace processing

#endif // _PROCESSING_INCLUDE_PROCESSING_INL_
