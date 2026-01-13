#include <processing/processing.hpp>

struct DemoApp : processing::Sketch
{
    virtual void setup() override
    {
    }

    virtual void draw() override
    {
        processing::background(100);
    }

    virtual void destroy() override
    {
    }
};

std::unique_ptr<processing::Sketch> processing::createSketch()
{
    return std::make_unique<DemoApp>();
}
