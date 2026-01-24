#include <processing/processing.hpp>
using namespace processing;

struct ViewportTestApp : Sketch
{
    RenderBuffer buffer = createRenderBuffer(100, 100);

    void setup() override
    {
    }

    void event(const Event& event) override
    {
    }

    void draw() override
    {
        background(51);

        renderBuffer(buffer);
        stroke(255);
        strokeWeight(45.0f);
        strokeCap(StrokeCap::round);
        line(100.0f, 100.0f, getViewport().width * 0.5f, 100.0f);
        noRenderBuffer();
    }
    void destroy() override
    {
    }
};

std::unique_ptr<Sketch> processing::createSketch()
{
    return std::make_unique<ViewportTestApp>();
}
