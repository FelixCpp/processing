#include <processing/processing.hpp>
using namespace processing;

constexpr auto VS_SOURCE = R"(
    #version 330 core

    layout (location = 0) in vec3 a_Position;
    layout (location = 1) in vec2 a_TexCoord;
    layout (location = 2) in vec4 a_Color;

    out vec2 v_TexCoord;
    out vec4 v_Color;

    uniform mat4 u_ProjectionMatrix;

    void main()
    {
        gl_Position = u_ProjectionMatrix * vec4(a_Position, 1.0);
        v_TexCoord = a_TexCoord;
        v_Color = a_Color;
    }
)";

constexpr auto LENSE_FS = R"(
#version 330 core

layout (location = 0) out vec4 o_FragColor;

in vec2 v_TexCoord;
in vec4 v_Color;

uniform sampler2D u_Texture;
uniform vec2 u_MousePos;
uniform vec2 u_Resolution;
uniform float u_Radius;
uniform float u_OutlineWidth;

void main()
{
    vec4 texColor = texture(u_Texture, v_TexCoord);
    vec2 pixelPos = v_TexCoord * u_Resolution;
    float dist = distance(pixelPos, u_MousePos);
    float grayFactor = smoothstep(u_Radius - 10.0, u_Radius, dist);
    float gray = dot(texColor.rgb, vec3(0.299, 0.587, 0.114));
    vec3 grayColor = vec3(gray);
    vec3 finalColor = mix(texColor.rgb, grayColor, grayFactor);
    float outerEdge = u_Radius + u_OutlineWidth * 0.5;
    float innerEdge = u_Radius - u_OutlineWidth * 0.5;
    float outlineMask = smoothstep(innerEdge - 1.0, innerEdge, dist) *
                        (1.0 - smoothstep(outerEdge, outerEdge + 1.0, dist));
    vec3 outlineColor = vec3(1.0, 1.0, 1.0);
    finalColor = mix(finalColor, outlineColor, outlineMask);
    o_FragColor = vec4(finalColor, texColor.a) * v_Color;
}
)";

struct ViewportTestApp : Sketch
{
    Shader lense = loadShader(VS_SOURCE, LENSE_FS);
    Texture wallpaper = loadTexture("images/wallpaper2.jpg");

    float radius = 150.0f;

    void setup() override
    {
        setWindowSize(1920, 1080);
    }

    void event(const Event& event) override
    {
        if (event.type == Event::mouse_wheel_scrolled)
        {
            radius += event.mouse_wheel.verticalDelta * 2.0f;
        }
    }

    void draw() override
    {
        lense.uploadUniform("u_MousePos", getMousePosition().x, getViewport().height - getMousePosition().y);
        lense.uploadUniform("u_Resolution", getViewport().width, getViewport().height);
        lense.uploadUniform("u_Radius", radius);
        lense.uploadUniform("u_OutlineWidth", 15.0f);

        shader(lense);
        background(wallpaper);
        noShader();
    }

    void destroy() override
    {
    }
};

std::unique_ptr<Sketch> processing::createSketch()
{
    return std::make_unique<ViewportTestApp>();
}
