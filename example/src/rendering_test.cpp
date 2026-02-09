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
        strokeWeight(15.0f);
        strokeJoin(StrokeJoin::round);

        beginShape(ShapeMode::quads);
        stroke(255, 0, 0);
        vertex(getMousePosition().x, getMousePosition().y);
        stroke(0, 255, 0);
        vertex(300.0f, 100.0f);
        stroke(0, 0, 255);
        vertex(300.0f, 300.0f);
        vertex(100.0f, 300.0f);
        endShape(true);

        // rectMode(RectMode::centerSize);
        // rect(getMousePosition().x, getMousePosition().y, 300.0f, 300.0f);
    }

    void destroy() override
    {
    }
};

std::unique_ptr<Sketch> processing::createSketch()
{
    return std::make_unique<RendererTest>();
}
