#include <processing/processing.hpp>
using namespace processing;

struct MatrixTest : Sketch
{
    void setup() override
    {
    }

    void event(const Event& event) override
    {
    }

    void draw() override
    {
        const float2 size = getViewport().size;

        strokeCap(StrokeCap::butt);
        strokeWeight(3.0f);
        stroke(48, 34, 24);

        background(51);
        translate(size.x * 0.5f, size.y);
        branch(100.0f);
    }

    void branch(float length)
    {
        if (length < 12.0f)
        {
            pushState();
            blendMode(BlendMode::opaque);
            noStroke();
            stroke(80, 180, 60, 140);
            strokeWeight(3.0f);
            point(0.0f, 0.0f);
            popState();
            return;
        }

        line(0.0f, 0.0f, 0.0f, -length);

        const float rotation = (float)getMousePosition().x / getViewport().size.x * 40.0f + 10.0f;

        pushMatrix();
        translate(0.0f, -length);
        rotate(rotation);
        branch(length * 0.75f);
        popMatrix();

        pushMatrix();
        translate(0.0f, -length);
        rotate(-rotation);
        branch(length * 0.77f);
        popMatrix();
    }

    virtual void destroy() override
    {
    }
};

std::unique_ptr<Sketch> processing::createSketch()
{
    return std::make_unique<MatrixTest>();
}
