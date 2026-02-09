#include <processing/processing.hpp>
using namespace processing;

struct RendererTest : Sketch
{
    void setup() override
    {
    }

    void draw(const f32 deltaTime) override
    {
        background(20, 20, 40);
    }

    void destroy() override
    {
    }
};

std::unique_ptr<Sketch> processing::createSketch()
{
    return std::make_unique<RendererTest>();
}
