#include <processing/processing.hpp>
using namespace processing;

struct DemoApp : Sketch
{

    virtual void setup() override
    {
    }

    virtual void draw() override
    {
        const int2 mouse = getMousePosition();

        background(51, 51, 51);
        fill(255, 0, 0);
        strokeWeight(35.0f);
        strokeJoin(StrokeJoin::bevel);
        stroke(0, 255, 0);
        ellipseMode(ellipse_mode_ltrb());
        ellipse(100.0f, 100.0f, mouse.x, mouse.y);
    }

    virtual void destroy() override
    {
    }
};

std::unique_ptr<Sketch> processing::createSketch()
{
    return std::make_unique<DemoApp>();
}
