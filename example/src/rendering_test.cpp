#include <processing/processing.hpp>
using namespace processing;

#include <chrono>

float measure(auto&& function)
{
    const auto start = std::chrono::system_clock::now();
    function();
    const auto end = std::chrono::system_clock::now();
    const auto diff = std::chrono::duration<float>(end - start);
    return diff.count();
}

struct RendererTest : Sketch
{
    void setup() override
    {
    }

    void draw(const f32 deltaTime) override
    {
        background(20, 20, 40);
        noFill();
        strokeWeight(25.0f);
        strokeJoin(StrokeJoin::round);
        stroke(60, 60, 90);
        triangle(100.0f, 100.0f, getMousePosition().x, getMousePosition().y, 400.0f, 400.0f);

        // float time = measure(
        //     []()
        //     {
        //         for (int i = 0; i < 10'000; ++i)
        //         {
        //             float x = random(900);
        //             float y = random(900);
        //             float u = random(900);
        //             float v = random(900);
        //
        //             line(x, y, u, v);
        //         }
        //     }
        // );
        //
        // fprintf(stdout, "Time: %.2f\n", 1.0f / time);
        // fflush(stdout);
    }

    void destroy() override
    {
    }
};

std::unique_ptr<Sketch> processing::createSketch()
{
    return std::make_unique<RendererTest>();
}
