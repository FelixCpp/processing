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

        noFill();

        stroke(255, 102, 0);
        line(340, 80, 40, 40);
        line(360, 360, 60, 320);
        stroke(255);
        bezier(340, 80, 40, 40, 360, 360, 60, 320);
        // beginShape(ShapeMode::lineStrip);
        // vertex(340, 80);
        // bezierVertex(40, 40, 360, 360, 60, 320);
        // endShape();
    }

    void destroy() override
    {
    }
};

std::unique_ptr<Sketch> processing::createSketch()
{
    return std::make_unique<RendererTest>();
}
