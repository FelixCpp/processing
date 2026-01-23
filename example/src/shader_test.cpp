#include <processing/processing.hpp>
using namespace processing;

inline static constexpr const char* VERTEX_SHADER = R"(
    #version 330 core

    layout (location = 0) in vec3 a_Position;
    layout (location = 1) in vec2 a_TexCoord;
    layout (location = 2) in vec4 a_Color;

    uniform mat4 u_ProjectionMatrix;
    // uniform mat4 u_ViewMatrix;

    void main()
    {
        gl_Position = u_ProjectionMatrix * vec4(a_Position, 1.0);
    }
)";

inline static constexpr const char* FRAGMENT_SHADER = R"(
    #version 330 core

    layout (location = 0) out vec4 o_Color;

    void main()
    {
        o_Color = vec4(0.3, 0.4, 0.8, 1.0);
    }
)";

struct ShaderTest : Sketch
{
    Shader customShader = loadShader(VERTEX_SHADER, FRAGMENT_SHADER);

    void setup() override
    {
    }

    void event(const Event& event) override
    {
    }

    void draw() override
    {
        background(21, 21, 21);
        shader(customShader);
        rect(100.0f, 100.0f, 300.0f, 300.0f);
        noShader();
        rect(400.0f, 100.0f, 300.0f, 300.0f);
        noLoop();
    }

    virtual void destroy() override
    {
    }
};

std::unique_ptr<Sketch> processing::createSketch()
{
    return std::make_unique<ShaderTest>();
}
