#include <processing/processing.hpp>
using namespace processing;

struct RendererTest : Sketch
{
    void setup() override
    {
    }

    void event(const Event& event) override
    {
    }

    void draw() override
    {
        imageMode(rect_mode_ltwh());

        background(0);
        noFill();
        strokeWeight(15.0f);
        rect(100.0f, 100.0f, 300.0f, 300.0f, 60.0f);
    }

    void destroy() override
    {
    }
};

std::unique_ptr<Sketch> processing::createSketch()
{
    return std::make_unique<RendererTest>();
}
