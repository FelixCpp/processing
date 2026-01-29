#include <processing/processing.hpp>
using namespace processing;

struct RenderbufferTest : Sketch
{
    Renderbuffer offscreen = createRenderbuffer(200, 200);

    void setup() override
    {
        renderbuffer(offscreen);
        background(255, 0, 0);
        rect(100.0f, 100.0f, 50.0f, 50.0f);
        noRenderbuffer();

        for (int i = 0; i < 2; ++i)
        {
            renderbuffer(offscreen);
            image(offscreen.getTexture(), 100.0f, 100.0f, 50.0f, 50.0f);
            noRenderbuffer();
        }
    }

    void event(const Event& event) override
    {
    }

    void draw() override
    {
        background(21);
        image(offscreen.getTexture(), 0.0f, 0.0f);
    }

    void destroy() override
    {
    }
};

std::unique_ptr<Sketch> processing::createSketch()
{
    return std::make_unique<RenderbufferTest>();
}
