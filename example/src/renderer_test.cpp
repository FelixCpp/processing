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

    float rotation = 0.0f;
    void draw() override
    {
        rectMode(rect_mode_center_size());

        background(0);
        resetMatrix();
        translate(getViewport().width / 2.0f, getViewport().height / 2.0f);
        // rotate(rotation += 0.1f);

        noFill();
        stroke(255);
        strokeWeight(35.0f);
        strokeJoin(StrokeJoin::miter);
        // rect(0.0f, 0.0f, 400.0f, 400.0f);
        circle(0.0f, 0.0f, 200.0f);
        // triangle(200.0f, 200.0f, getMousePosition().x, getMousePosition().y, 400.0f, 300.0f);
    }

    void destroy() override
    {
    }
};

std::unique_ptr<Sketch> processing::createSketch()
{
    return std::make_unique<RendererTest>();
}
