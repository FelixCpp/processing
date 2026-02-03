#ifndef _PROCESSING_INCLUDE_PROCESSING_HPP_
#define _PROCESSING_INCLUDE_PROCESSING_HPP_

#include <cstdint>
#include <array>
#include <numbers>
#include <memory>
#include <span>
#include <stack>

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
        alpha,
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
        corners,
        centerRadius,
        centerDiameter,
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
        std::span<const Vertex> vertices;
        std::span<const uint32_t> indices;
    };
} // namespace processing

namespace processing
{
    struct RenderStyle
    {
        Color fillColor;
        Color strokeColor;

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

        RenderStyle();
    };
} // namespace processing

namespace processing
{
    class Graphics
    {
    public:
        void beginDraw();
        void endDraw();

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

        void background(i32 red, i32 green, i32 blue, i32 alpha = 255);
        void background(i32 grey, i32 alpha = 255);
        void background(Color color);

        void beginShape();
        void endShape();
        void vertex(f32 x, f32 y);
        void vertex(f32 x, f32 y, f32 u, f32 v);
        void bezierVertex(f32 x2, f32 y2, f32 x3, f32 y3);
        void quadraticVertex(f32 cx, f32 cy, f32 x3, f32 y3);
        void curveVertex(f32 x, f32 y);

        void square(f32 x1, f32 y1, f32 xy2);
        void ellipse(f32 x1, f32 y1, f32 x2, f32 y2);
        void circle(f32 x1, f32 y1, f32 xy2);
        void triangle(f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3);
        void point(f32 x, f32 y);
        void line(f32 x1, f32 y1, f32 x2, f32 y2);

    private:
        std::stack<RenderStyle> m_renderStyles;
        std::stack<matrix4x4> m_metrics;
    };
} // namespace processing

namespace processing
{
    void quit();
    void quit(i32 exitCode);
    void restart();
    void setExitCode(i32 exitCode);
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

#endif // _PROCESSING_INCLUDE_PROCESSING_INL_
