#include <processing/processing.hpp>
using namespace processing;

struct DemoApp : Sketch
{
    void setup() override
    {
    }

    void draw(f32 deltaTime) override
    {
    }

    void destroy() override
    {
    }
};

std::unique_ptr<Sketch> processing::createSketch()
{
    return std::make_unique<DemoApp>();
}
