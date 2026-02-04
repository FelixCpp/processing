#include <processing/processing.hpp>
using namespace processing;

struct DemoApp : Sketch
{
    Image buff = createImage(200, 200);

    void setup() override
    {
    }

    void draw(f32 deltaTime) override
    {
        Pixels p = buff.loadPixels();
        for (u32 y = 0; y < buff.height; ++y)
        {
            for (u32 x = 0; x < buff.width; ++x)
            {
                Color c = p.get(x, y);
                p.set(x, y, c);
            }
        }
        buff.updatePixels(p);
    }

    void destroy() override
    {
    }
};

std::unique_ptr<Sketch> processing::createSketch()
{
    return std::make_unique<DemoApp>();
}
