#ifndef _PROCESSING_INCLUDE_LIBRARY_HPP_
#define _PROCESSING_INCLUDE_LIBRARY_HPP_

#include <cstdint>
#include <string>
#include <memory>
#include <array>

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
        T x, y;
    };

    using int2 = value2<int32_t>;
    using uint2 = value2<uint32_t>;
    using float2 = value2<float>;

    template <typename T>
    struct value3
    {
        T x, y, z;
    };

    using int3 = value3<int32_t>;
    using uint3 = value3<uint32_t>;
    using float3 = value3<float>;

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
    struct Event
    {
        enum Type
        {
            closed,
            resized,
        } type;

        struct SizeEvent
        {
            uint32_t width;
            uint32_t height;
        };

        union
        {
            SizeEvent size;
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
    using color_t = struct
    {
        uint32_t value;
    };

    color_t color(int32_t red, int32_t green, int32_t blue, int32_t alpha = 255);
    color_t color(int32_t grey, int32_t alpha = 255);
    int32_t red(color_t color);
    int32_t green(color_t color);
    int32_t blue(color_t color);
    int32_t alpha(color_t color);

    struct RenderTarget
    {
        virtual ~RenderTarget() = default;
        virtual void beginDraw() = 0;
        virtual void endDraw() = 0;
        virtual uint2 getSize() = 0;
    };

    class Renderer;

    class Graphics
    {
    private:
        struct RenderStyle
        {
            color_t fillColor;
            color_t strokeColor;
            float strokeWeight;
            bool isFillEnabled;
            bool isStrokeEnabled;

            RenderStyle();

            static const RenderStyle Default;
        };

        struct RenderStyleStack
        {
        public:
            RenderStyleStack();

            void pushStyle();
            void popStyle();
            RenderStyle& peekStyle();

            void reset();

        private:
            std::array<RenderStyle, 64> m_renderStyles;
            size_t m_currentStyle;
        };

    public:
        void beginDraw();
        void endDraw();
        uint2 getSize();

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

        void strokeWeight(float strokeWeight);

        void rect(float left, float top, float width, float height);
        void square(float left, float top, float size);
        void ellipse(float centerX, float centerY, float radiusX, float radiusY);
        void circle(float centerX, float centerY, float radius);
        void line(float x1, float y1, float x2, float y2);
        void triangle(float x1, float y1, float x2, float y2, float x3, float y3);
        void point(float x, float y);

    private:
        explicit Graphics(std::shared_ptr<RenderTarget> renderTarget, std::shared_ptr<Renderer> renderer);

        std::shared_ptr<RenderTarget> m_renderTarget;
        std::shared_ptr<Renderer> m_renderer;
        RenderStyleStack m_renderStyles;

    private:
        friend std::unique_ptr<Graphics> createGraphics(std::shared_ptr<RenderTarget>);
    };

    std::unique_ptr<Graphics> createGraphics(std::shared_ptr<RenderTarget> renderTarget);
    std::unique_ptr<Graphics> createGraphics(uint32_t width, uint32_t height);

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

    void strokeWeight(float strokeWeight);

    void rect(float left, float top, float width, float height);
    void square(float left, float top, float size);
    void ellipse(float centerX, float centerY, float radiusX, float radiusY);
    void circle(float centerX, float centerY, float radius);
    void line(float x1, float y1, float x2, float y2);
    void triangle(float x1, float y1, float x2, float y2, float x3, float y3);
    void point(float x, float y);
} // namespace processing

#endif // _PROCESSING_INCLUDE_LIBRARY_HPP_
