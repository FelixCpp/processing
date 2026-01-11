#ifndef _PROCESSING_INCLUDE_LIBRARY_HPP_
#define _PROCESSING_INCLUDE_LIBRARY_HPP_

#include <cstdint>
#include <string>
#include <memory>

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
    using color = struct
    {
        uint32_t value;
    };

    struct Graphics
    {
        virtual ~Graphics() = default;
        virtual void fill(int red, int green, int blue, int alpha = 255) = 0;
        virtual void fill(int grey, int alpha = 255) = 0;
        virtual void fill(color color) = 0;
        virtual void noFill() = 0;
        virtual void stroke(int red, int green, int blue, int alpha = 255) = 0;
        virtual void stroke(int grey, int alpha = 255) = 0;
        virtual void stroke(color color) = 0;
        virtual void noStroke() = 0;
        virtual void rect(float left, float top, float width, float height) = 0;
        virtual void square(float left, float top, float size) = 0;
        virtual void ellipse(float centerX, float centerY, float radiusX, float radiusY) = 0;
        virtual void circle(float centerX, float centerY, float radius) = 0;
        virtual void line(float x1, float y1, float x2, float y2) = 0;
        virtual void triangle(float x1, float y1, float x2, float y2, float x3, float y3) = 0;
        virtual void point(float x, float y) = 0;
    };

    std::unique_ptr<Graphics> createGraphics(uint32_t width, uint32_t height);
} // namespace processing

#endif // _PROCESSING_INCLUDE_LIBRARY_HPP_
