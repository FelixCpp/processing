#include <processing/batch_renderer.hpp>
#include <processing/processing.hpp>
#include <processing/processing_data.hpp>
#include <processing/render_targets.hpp>
#include <processing/graphics.hpp>

#include <iostream>

using namespace processing;

namespace processing
{
    ProcessingData s_data;
}

namespace processing
{
    // clang-format off
    void close() { s_data.shouldAppClose = true; s_data.shouldRestart = false; }
    void close(int exitCode) { setExitCode(exitCode); close(); }
    void restart() { s_data.shouldAppClose = true; s_data.shouldRestart = true; }
    void setExitCode(int exitCode) { s_data.exitCode = exitCode; }
    int getExitCode() { return s_data.exitCode; }
    void loop() { s_data.isMainLoopPaused = false; }
    void noLoop() { s_data.isMainLoopPaused = true; }
    void redraw() { s_data.userRequestedRedraw = true; }
    // clang-format on
} // namespace processing

namespace processing
{
    // clang-format off
    void trace(const std::string& message) { std::cout << message.c_str() << std::endl; }
    void debug(const std::string& message) { std::cout << message.c_str() << std::endl; }
    void info(const std::string& message) { std::cout << message.c_str() << std::endl; }
    void warning(const std::string& message) { std::cout << message.c_str() << std::endl; }
    void error(const std::string& message) { std::cout << message.c_str() << std::endl; }
    // clang-format on
} // namespace processing

namespace processing
{
    // clang-format off
    void setWindowSize(uint32_t width, uint32_t height) { s_data.window->setSize(width, height); }
    uint2 getWindowSize() { return s_data.window->getSize(); }
    void setWindowTitle(std::string_view title) { s_data.window->setTitle(title); }
    std::string getWindowTitle() { return s_data.window->getTitle(); }
    int2 getMousePosition() { return s_data.window->getMousePosition(); }
    // clang-format on
} // namespace processing

namespace processing
{
    RectMode rect_mode_ltwh()
    {
        return [](float x1, float y1, float x2, float y2)
        {
            return rect2f(x1, y1, x2, y2);
        };
    }

    RectMode rect_mode_ltrb()
    {
        return [](float x1, float y1, float x2, float y2)
        {
            return rect2f(x1, y1, x2 - x1, y2 - y1);
        };
    }

    RectMode rect_mode_center_size()
    {
        return [](float x1, float y1, float x2, float y2)
        {
            return rect2f(x1 - x2 * 0.5f, y1 - y2 * 0.5f, x2, y2);
        };
    }

    EllipseMode ellipse_mode_ltwh()
    {
        return [](float x1, float y1, float x2, float y2)
        {
            return rect2f(x1, y1, x2, y2);
        };
    }

    EllipseMode ellipse_mode_ltrb()
    {
        return [](float x1, float y1, float x2, float y2)
        {
            return rect2f(x1, y1, x2 - x1, y2 - y1);
        };
    }

    EllipseMode ellipse_mode_center_radius()
    {
        return [](float x1, float y1, float x2, float y2)
        {
            return rect2f(x1 - x2, y1 - y2, x2 * 2.0f, y2 * 2.0f);
        };
    }

    EllipseMode ellipse_mode_center_diameter()
    {
        return [](float x1, float y1, float x2, float y2)
        {
            return rect2f(x1 - x2 * 0.5f, y1 - y2 * 0.5f, x2, y2);
        };
    }
} // namespace processing

namespace processing
{
    ImageSourceMode image_source_mode_ltwh_normalized()
    {
        return [](uint2, float x1, float y1, float x2, float y2)
        {
            return rect2f(x1, y1, x2, y2);
        };
    }

    ImageSourceMode image_source_mode_ltwh_coordinates()
    {
        return [](uint2 size, float x1, float y1, float x2, float y2)
        {
            const float sourceLeft = x1 / static_cast<float>(size.x);
            const float sourceTop = y1 / static_cast<float>(size.y);
            const float sourceWidth = x2 / static_cast<float>(size.x);
            const float sourceHeight = y2 / static_cast<float>(size.y);
            return rect2f(sourceLeft, sourceTop, sourceWidth, sourceHeight);
        };
    }
} // namespace processing

namespace processing
{
    // clang-format off
    void pushState() { s_data.graphics->pushState(); }
    void popState() { s_data.graphics->popState(); }

    rect2f getViewport() { return s_data.graphics->getViewport(); }

    void strokeJoin(StrokeJoin strokeJoin) { s_data.graphics->strokeJoin(strokeJoin); }
    void strokeCap(StrokeCap strokeCap) { s_data.graphics->strokeCap(strokeCap); }

    void pushMatrix() { s_data.graphics->pushMatrix(); }
    void popMatrix() { s_data.graphics->popMatrix(); }
    void resetMatrix() { s_data.graphics->resetMatrix(); }
    matrix4x4& peekMatrix() { return s_data.graphics->peekMatrix(); }
    void translate(float x, float y) { s_data.graphics->translate(x, y); }
    void scale(float x, float y) { s_data.graphics->scale(x, y); }
    void rotate(float angle) { s_data.graphics->rotate(angle); }

    void blendMode(const BlendMode& blendMode) { s_data.graphics->blendMode(blendMode); }
    void shader(ShaderProgram *shaderProgram) { s_data.graphics->shader(shaderProgram); }

    void background(int red, int green, int blue, int alpha) { s_data.graphics->background(red, green, blue, alpha); }
    void background(int grey, int alpha) { s_data.graphics->background(grey, alpha); }
    void background(color_t color) { s_data.graphics->background(color); }

    void fill(int red, int green, int blue, int alpha) { s_data.graphics->fill(red, green, blue, alpha); }
    void fill(int grey, int alpha) { s_data.graphics->fill(grey, alpha); }
    void fill(color_t color) { s_data.graphics->fill(color); }
    void noFill() { s_data.graphics->noFill(); }

    void stroke(int red, int green, int blue, int alpha) { s_data.graphics->stroke(red, green, blue, alpha); }
    void stroke(int grey, int alpha) { s_data.graphics->stroke(grey, alpha); }
    void strkoe(color_t color) { s_data.graphics->stroke(color); }
    void noStroke() { s_data.graphics->noStroke(); }

    void imageMode(RectMode imageMode) { s_data.graphics->imageMode(imageMode); }
    void imageSourceMode(ImageSourceMode imageSourceMode) { s_data.graphics->imageSourceMode(imageSourceMode); }
    void imageTint(int red, int green, int blue, int alpha) { s_data.graphics->imageTint(red, green, blue, alpha); }
    void imageTint(int grey, int alpha) { s_data.graphics->imageTint(grey, alpha); }
    void imageTint(color_t color) { s_data.graphics->imageTint(color); }

    void strokeWeight(float strokeWeight) { s_data.graphics->strokeWeight(strokeWeight); }
    void rectMode(RectMode rectMode) { s_data.graphics->rectMode(rectMode);}
    void ellipseMode(EllipseMode ellipseMode) { s_data.graphics->ellipseMode(ellipseMode);}

    void rect(float left, float top, float width, float height) { s_data.graphics->rect(left, top, width, height); }
    void square(float left, float top, float size) { s_data.graphics->square(left, top, size); }
    void ellipse(float centerX, float centerY, float radiusX, float radiusY) { s_data.graphics->ellipse(centerX, centerY, radiusX, radiusY); }
    void circle(float centerX, float centerY, float radius) { s_data.graphics->circle(centerX, centerY, radius); }
    void line(float x1, float y1, float x2, float y2) { s_data.graphics->line(x1, y1, x2, y2); }
    void triangle(float x1, float y1, float x2, float y2, float x3, float y3) { s_data.graphics->triangle(x1, y1, x2, y2, x3, y3); }
    void point(float x, float y) { s_data.graphics->point(x, y); }
    void image(const Texture &texture, float x1, float y1) { s_data.graphics->image(texture, x1, y1); }
    void image(const Texture &texture, float x1, float y1, float x2, float y2) { s_data.graphics->image(texture, x1, y1, x2, y2); }
    void image(const Texture &texture, float x1, float y1, float x2, float y2, float sx1, float sy1, float sx2, float sy2) { s_data.graphics->image(texture, x1, y1, x2, y2, sx1, sy1, sx2, sy2); }
    // clang-format on
} // namespace processing

void launch()
{
    s_data.window = createWindow(1280, 720, "Processing");
    s_data.context = createContext(*s_data.window);
    s_data.graphics = std::make_unique<Graphics>(uint2{1280, 720});
    s_data.sketch = createSketch();

    s_data.sketch->setup();

    while (not s_data.shouldAppClose)
    {
        if (const auto event = s_data.window->pollEvent())
        {
            if (event->type == Event::closed)
            {
                close();
            }

            s_data.sketch->event(*event);
        }

        if (not s_data.isMainLoopPaused or s_data.userRequestedRedraw)
        {
            const uint2 framebufferSize = s_data.window->getFramebufferSize();
            const uint2 windowSize = s_data.window->getSize();

            s_data.graphics->beginDraw({
                .windowSize = windowSize,
                .framebufferSize = framebufferSize,
            });
            s_data.sketch->draw();
            s_data.graphics->endDraw();

            s_data.context->flush();

            s_data.userRequestedRedraw = false;
        }
    }

    s_data.sketch->destroy();
    s_data = {};
}

int main()
{
    do
    {
        std::memset((void*)&s_data, 0, sizeof(ProcessingData));
        launch();
    } while (s_data.shouldRestart);

    return s_data.exitCode;
}
