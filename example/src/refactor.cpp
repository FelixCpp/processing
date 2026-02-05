#include <processing/processing.hpp>
using namespace processing;

struct DemoApp : Sketch
{
    void setup() override
    {
    }

    void draw(f32 deltaTime) override
    {
        getGfx().background(51);
        getGfx().fill(255, 0, 0);
        getGfx().stroke(0, 255, 0);
        getGfx().strokeWeight(35.0f);
        getGfx().strokeJoin(StrokeJoin::round);
        getGfx().rect(100.0f, 100.0f, 300.0f, 300.0f);
    }

    void destroy() override
    {
    }
};

std::unique_ptr<Sketch> processing::createSketch()
{
    return std::make_unique<DemoApp>();
}
