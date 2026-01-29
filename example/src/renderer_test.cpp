#include <processing/processing.hpp>
using namespace processing;

struct RendererTest : Sketch
{
    const int cellWidth = 100;
    const int cellHeight = 100;

    void setup() override
    {
    }

    void event(const Event& event) override
    {
    }

    void draw() override
    {
        const float2 mouse = float2{getMousePosition()} / getViewport().size;
        const int2 cell = int2{mouse * float2(cellWidth, cellHeight)};

        background(51);
        stroke(255);
        fill(255, 50);
        rect(cell.x, cell.y, cellWidth, cellHeight);
    }

    void destroy() override
    {
    }
};

std::unique_ptr<Sketch> processing::createSketch()
{
    return std::make_unique<RendererTest>();
}
