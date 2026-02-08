#include <processing/processing.hpp>
using namespace processing;

struct DemoApp : Sketch
{
    Renderbuffer rb = createRenderbuffer(400, 400);

    void setup() override
    {
        pushRenderbuffer(rb);
        strokeWeight(40.0f);
        popRenderbuffer();
    }

    void draw(f32 deltaTime) override
    {
        pushRenderbuffer(rb);
        background(255, 0, 0);
        line(100.0f, 100.0f, 200.0f, 200.0f);
        popRenderbuffer();

        image(rb.getImage(), 0.0f, 0.0f);
    }

    void destroy() override
    {
    }
};

std::unique_ptr<Sketch> processing::createSketch()
{
    return std::make_unique<DemoApp>();
}
