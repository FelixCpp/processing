#include <processing/processing.hpp>
using namespace processing;

struct DemoApp : Sketch
{

    virtual void setup() override
    {
    }

    virtual void draw() override
    {
        background(51, 51, 51);
        triangle(100.0f, 100.0f, 300.0f, 100.0f, 200.f, 300.0f);
        fill(255, 0, 0);
        circle(200.0f, 175.0f, 56.0f);
    }

    virtual void destroy() override
    {
    }
};

std::unique_ptr<Sketch> processing::createSketch()
{
    return std::make_unique<DemoApp>();
}
