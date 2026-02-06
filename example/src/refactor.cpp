#include <processing/processing.hpp>
using namespace processing;

struct DemoApp : Sketch
{
    Image wallpaper = loadImage("images/wallpaper.jpg");
    Graphics buff = createGraphics(200, 200);

    void setup() override
    {
        buff.beginDraw();
        buff.blendMode(BlendMode::additive);
        buff.image(wallpaper, 0.0f, 0.0f, 200, 200);
        buff.background(51, 200);
        buff.endDraw();
    }

    void draw(f32 deltaTime) override
    {
        getGfx().blendMode(BlendMode::alpha);
        getGfx().background(0);
        getGfx().fill(255, 0, 0, getMousePosition().x / 800.0f * 255.0f);
        getGfx().noStroke();
        getGfx().rect(100.0f, 100.0f, 300.0f, 300.0f);
        // getGfx().tint(255, 0, 0, getMousePosition().x / 800.0f);
        // getGfx().image(buff.getImage(), 0.0f, 0.0f, 400.0f, 400.0f);
    }

    void destroy() override
    {
    }
};

std::unique_ptr<Sketch> processing::createSketch()
{
    return std::make_unique<DemoApp>();
}
