#include "processing/renderer.hpp"
#include <processing/processing.hpp>
#include <processing/processing_data.hpp>
#include <processing/render_targets.hpp>

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
    std::unique_ptr<Graphics> createGraphics(std::shared_ptr<RenderTarget> renderTarget) { return std::unique_ptr<Graphics>(new Graphics(renderTarget, s_data.renderer)); }
    std::unique_ptr<Graphics> createGraphics(uint32_t width, uint32_t height) { return createGraphics(OffscreenRenderTarget::create(uint2{width, height})); }

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

    void strokeWeight(float strokeWeight) { s_data.graphics->strokeWeight(strokeWeight); }

    void rect(float left, float top, float width, float height) { s_data.graphics->rect(left, top, width, height); }
    void square(float left, float top, float size) { s_data.graphics->square(left, top, size); }
    void ellipse(float centerX, float centerY, float radiusX, float radiusY) { s_data.graphics->ellipse(centerX, centerY, radiusX, radiusY); }
    void circle(float centerX, float centerY, float radius) { s_data.graphics->circle(centerX, centerY, radius); }
    void line(float x1, float y1, float x2, float y2) { s_data.graphics->line(x1, y1, x2, y2); }
    void triangle(float x1, float y1, float x2, float y2, float x3, float y3) { s_data.graphics->triangle(x1, y1, x2, y2, x3, y3); }
    void point(float x, float y) { s_data.graphics->point(x, y); }
    // clang-format on
} // namespace processing

void launch()
{
    s_data.window = createWindow(1280, 720, "Processing");
    s_data.context = createContext(*s_data.window);
    s_data.mainRenderTarget = std::make_shared<MainRenderTarget>(uint2{1280, 720});
    s_data.renderer = Renderer::create();
    s_data.graphics = createGraphics(s_data.mainRenderTarget);
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

            if (event->type == Event::resized)
            {
                s_data.mainRenderTarget->resize(uint2{event->size.width, event->size.height});
            }
        }

        if (not s_data.isMainLoopPaused or s_data.userRequestedRedraw)
        {
            s_data.graphics->beginDraw();
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
