#include "processing/graphics_stack.hpp"
#include <memory>
#include <processing/batch_renderer.hpp>
#include <processing/processing.hpp>
#include <processing/processing_data.hpp>
#include <processing/render_targets.hpp>
#include <processing/graphics.hpp>
#include <processing/recursive_renderer.hpp>

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
            const float width = x2 - x1;
            const float height = y2 - y1;
            return rect2f(x1 - width * 0.5f, y1 - height * 0.5f, width, height);
        };
    }
} // namespace processing

namespace processing
{
    // clang-format off
    Graphics& getGraphics() { return graphics_stack_peek(s_data.graphicsStack); }
    std::unique_ptr<ClientGraphics> createGraphics(uint32_t width, uint32_t height) { return std::make_unique<OffscreenGraphics>(uint2{ width, height }, s_data.renderer); }

    void pushGraphics(std::shared_ptr<Graphics> graphics) { graphics_stack_push(s_data.graphicsStack, std::dynamic_pointer_cast<OffscreenGraphics>(graphics)); }
    void popGraphics() { graphics_stack_pop(s_data.graphicsStack); }

    void pushState() { getGraphics().pushState(); }
    void popState() { getGraphics().popState(); }

    rect2f getViewport() { return getGraphics().getViewport(); }

    void strokeJoin(StrokeJoin strokeJoin) { getGraphics().strokeJoin(strokeJoin); }
    void strokeCap(StrokeCap strokeCap) { getGraphics().strokeCap(strokeCap); }

    void background(int red, int green, int blue, int alpha) { getGraphics().background(red, green, blue, alpha); }
    void background(int grey, int alpha) { getGraphics().background(grey, alpha); }
    void background(color_t color) { getGraphics().background(color); }

    void fill(int red, int green, int blue, int alpha) { getGraphics().fill(red, green, blue, alpha); }
    void fill(int grey, int alpha) { getGraphics().fill(grey, alpha); }
    void fill(color_t color) { getGraphics().fill(color); }
    void noFill() { getGraphics().noFill(); }

    void stroke(int red, int green, int blue, int alpha) { getGraphics().stroke(red, green, blue, alpha); }
    void stroke(int grey, int alpha) { getGraphics().stroke(grey, alpha); }
    void strkoe(color_t color) { getGraphics().stroke(color); }
    void noStroke() { getGraphics().noStroke(); }

    void strokeWeight(float strokeWeight) { getGraphics().strokeWeight(strokeWeight); }
    void rectMode(RectMode rectMode) { getGraphics().rectMode(rectMode);}

    void rect(float left, float top, float width, float height) { getGraphics().rect(left, top, width, height); }
    void square(float left, float top, float size) { getGraphics().square(left, top, size); }
    void ellipse(float centerX, float centerY, float radiusX, float radiusY) { getGraphics().ellipse(centerX, centerY, radiusX, radiusY); }
    void circle(float centerX, float centerY, float radius) { getGraphics().circle(centerX, centerY, radius); }
    void line(float x1, float y1, float x2, float y2) { getGraphics().line(x1, y1, x2, y2); }
    void triangle(float x1, float y1, float x2, float y2, float x3, float y3) { getGraphics().triangle(x1, y1, x2, y2, x3, y3); }
    void point(float x, float y) { getGraphics().point(x, y); }
    // clang-format on
} // namespace processing

void launch()
{
    const rect2u initialViewport = rect2u{0, 0, 1280, 720};
    s_data.window = createWindow(initialViewport.width, initialViewport.height, "Processing");
    s_data.context = createContext(*s_data.window);
    s_data.renderer = std::make_unique<RecursiveRenderer>(BatchRenderer::create());
    s_data.mainGraphics = std::make_shared<MainGraphics>(initialViewport, s_data.renderer);
    s_data.graphicsStack = graphics_stack_create(s_data.mainGraphics);
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

            s_data.mainGraphics->handle(*event);
        }

        if (not s_data.isMainLoopPaused or s_data.userRequestedRedraw)
        {
            s_data.mainGraphics->beginDraw();
            s_data.sketch->draw();
            s_data.mainGraphics->endDraw();

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
