#include <processing/processing.hpp>
using namespace processing;

struct DemoApp : Sketch
{
    virtual void setup() override
    {
    }

    virtual void draw() override
    {
        background(255, 0, 0);
        fill(255, 100, 200);
        strokeWeight(4.0f);
        stroke(255, 0, 0);
        rect(10.0f, 10.0f, 50.0f, 50.0f);
    }

    virtual void destroy() override
    {
    }
};

std::unique_ptr<Sketch> processing::createSketch()
{
    return std::make_unique<DemoApp>();
}
