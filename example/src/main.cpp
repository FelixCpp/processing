#include <processing/processing.hpp>
using namespace processing;

struct DemoApp : Sketch
{
    std::shared_ptr<Graphics> graphics = createGraphics(2, 2);

    virtual void setup() override
    {
        setWindowSize(600, 400);

        pushGraphics(graphics);
        {
            background(255, 0, 0);
        }
        popGraphics();
    }

    virtual void draw() override
    {
        background(21);
        fill(255, 0, 0);
    }

    virtual void destroy() override
    {
    }
};

std::unique_ptr<Sketch> processing::createSketch()
{
    return std::make_unique<DemoApp>();
}
