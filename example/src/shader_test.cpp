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

    uniform float u_Time;

    void main()
    {
        float red = sin(u_Time) * 0.5f + 0.5f;
        o_Color = vec4(red, 0.4, 0.8, 1.0);
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

    float time = 0.0f;

    void withShader(Shader id, auto lambda)
    {
        pushState();
        shader(id);
        lambda();
        popState();
    }

    void draw() override
    {
        time += 0.1f;

        background(21, 21, 21);
        withShader(
            customShader, [this]()
            {
                shaderUniform("u_Time", time);
                ellipse(getMousePosition().x, getMousePosition().y, 30.0f, 30.0f);
            }
        );
    }

    virtual void destroy() override
    {
    }
};

std::unique_ptr<Sketch> processing::createSketch()
{
    return std::make_unique<ShaderTest>();
}
