#ifndef _PROCESSING_INCLUDE_GRAPHICS_HPP_
#define _PROCESSING_INCLUDE_GRAPHICS_HPP_

#include <processing/processing.hpp>

namespace processing
{
    class MainGraphics : public Graphics
    {
    public:
        virtual ~MainGraphics() override = default;

        void handleEvent(const Event& event);

        virtual void fill(int red, int green, int blue, int alpha = 255) override;
        virtual void fill(int grey, int alpha = 255) override;
        virtual void fill(color color) override;
        virtual void noFill() override;
        virtual void stroke(int red, int green, int blue, int alpha = 255) override;
        virtual void stroke(int grey, int alpha = 255) override;
        virtual void stroke(color color) override;
        virtual void noStroke() override;
        virtual void rect(float left, float top, float width, float height) override;
        virtual void square(float left, float top, float size) override;
        virtual void ellipse(float centerX, float centerY, float radiusX, float radiusY) override;
        virtual void circle(float centerX, float centerY, float radius) override;
        virtual void line(float x1, float y1, float x2, float y2) override;
        virtual void triangle(float x1, float y1, float x2, float y2, float x3, float y3) override;
        virtual void point(float x, float y) override;

    private:
    };

    std::unique_ptr<MainGraphics> createMainGraphics();
} // namespace processing

#endif // _PROCESSING_INCLUDE_GRAPHICS_HPP_
