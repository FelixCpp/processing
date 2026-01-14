#include <algorithm>
#include <processing/processing.hpp>
using namespace processing;

struct DemoApp : Sketch
{
    float2 ballPosition = float2{};
    float2 ballVelocity = float2{};
    float ballRadius = 50.0f;

    virtual void setup() override
    {
        const auto [width, height] = getViewport().size;

        ballPosition.x = static_cast<float>(width) / 2.0f;
        ballPosition.y = static_cast<float>(height) / 2.0f;

        ballVelocity.x = 1.0f;
        ballVelocity.y = 2.0f;

        noStroke();
    }

    virtual void draw() override
    {
        const auto [width, height] = getViewport().size;

        const float minX = ballRadius;
        const float maxX = width - ballRadius;

        ballPosition.x += ballVelocity.x;
        if (ballPosition.x < minX or ballPosition.x > maxX)
        {
            ballPosition.x = std::clamp(ballPosition.x, minX, maxX);
            ballVelocity.x *= -1.0f;
        }

        const float minY = ballRadius;
        const float maxY = height - ballRadius;
        ballPosition.y += ballVelocity.y;
        if (ballPosition.y < minY or ballPosition.y > maxY)
        {
            ballPosition.y = std::clamp(ballPosition.y, minY, maxY);
            ballVelocity.y *= -1.0f;
        }

        background(51);
        fill(255, 0, 0);
        circle(getMousePosition().x, getMousePosition().y, 50.0f);
    }

    virtual void destroy() override
    {
    }
};

std::unique_ptr<Sketch> processing::createSketch()
{
    return std::make_unique<DemoApp>();
}
