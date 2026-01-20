#include <processing/processing.hpp>
using namespace processing;

struct DemoApp : Sketch
{
    Texture texture = loadTexture("images/test3.png");

    virtual void setup() override
    {
    }

    virtual void draw() override
    {
        background(255);
        // imageTint(255, 0, 0);
        image(texture, 100.0f, 100.0f, 300.0f, 300.0f, 0.0f, 0.0f, texture.getSize().x, texture.getSize().y);
    }

    virtual void destroy() override
    {
    }
};

std::unique_ptr<Sketch> processing::createSketch()
{
    return std::make_unique<DemoApp>();
}
