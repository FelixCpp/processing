#include <processing/processing.hpp>
using namespace processing;

static constexpr auto vsSource = R"(
    #version 330 core

    layout (location = 0) vec3 a_Position;
    layout (location = 1) vec3 a_TexCoord;
    layout (location = 2) vec3 a_Color;

    uniform mat4 u_ProjectionMatrix;
    uniform mat4 u_ViewMatrix;

    void main() {
        gl_Position = u_ProjectionMatrix * u_ViewMatrix * vec4(a_Position, 1.0);
    }
)";

static constexpr auto fsSource = R"(
    #version 330 core

    layout (location = 0) out vec4 o_Color;

    void main() {
        o_Color = vec4(0.3, 0.4, 0.8, 1.0);
    }
)";

struct ShaderTest : Sketch
{
    ShaderProgram customShader;

    void setup() override
    {
        customShader = loadShaderFromMemory(vsSource, fsSource);
    }

    void event(const Event& event) override
    {
    }

    void draw() override
    {
        background(51);
        shader(&customShader);
        rect(100.0f, 100.0f, 300.0f, 300.0f);
    }

    virtual void destroy() override
    {
    }
};

std::unique_ptr<Sketch> processing::createSketch()
{
    return std::make_unique<ShaderTest>();
}
