#include <processing/processing.hpp>
using namespace processing;

struct ViewportTestApp : Sketch
{
    RenderBuffer rb = createRenderBuffer(100, 100);

    void setup() override
    {
        renderBuffer(rb);
        background(0);
        rect(0.0f, 0.0f, getViewport().width / 2, getViewport().height / 2);
        noRenderBuffer();
    }

    void event(const Event& event) override
    {
    }

    void draw() override
    {
        background(51);
        image(rb.getTexture(), 0.0f, 0.0f, 100.0f, 100.0f);
    }

    void destroy() override
    {
    }
};

std::unique_ptr<Sketch> processing::createSketch()
{
    return std::make_unique<ViewportTestApp>();
}
