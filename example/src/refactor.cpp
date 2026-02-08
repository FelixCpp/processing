#include <processing/processing.hpp>
using namespace processing;

struct DemoApp : Sketch
{
    Renderbuffer buffer;
    float angle = 0.0f;

    void setup() override
    {
        buffer = createRenderbuffer(900, 900);
    }

    void draw(f32 deltaTime) override
    {
        pushRenderbuffer(buffer);
        {
            background(20, 20, 40);
            pushMatrix();
            translate(400, 300);
            // Äußerer Ring aus Rechtecken
            for (int i = 0; i < 12; i++)
            {
                pushMatrix();
                rotate(angle + i * TAU / 12);
                translate(150, 0);
                rotate(angle * 2);

                fill(100 + i * 12, 150, 255 - i * 12);
                noStroke();
                rect(-20, -20, 40, 40);
                popMatrix();
            }

            // Innerer Ring aus Kreisen
            for (int i = 0; i < 8; i++)
            {
                pushMatrix();
                rotate(-angle * 1.5 + i * TAU / 8);
                translate(80, 0);

                fill(255, 100 + i * 15, 150);
                circle(0, 0, 30);
                popMatrix();
            }

            // Zentrale pulsierende Ellipse
            float pulse = sin(angle * 3) * 15 + 40;
            fill(255, 200, 100, 200);
            ellipse(0, 0, pulse, pulse);

            popMatrix();

            // Partikel-System
            for (int i = 0; i < 20; i++)
            {
                float x = 400 + cos(angle * 0.5 + i * 2) * (200 + sin(angle + i) * 50);
                float y = 300 + sin(angle * 0.5 + i * 2) * (200 + cos(angle + i) * 50);
                float size = sin(angle * 2 + i) * 5 + 10;

                fill(150 + i * 5, 255 - i * 8, 200, 150);
                noStroke();
                circle(x, y, size);
            }
        }
        popRenderbuffer();

        background(255);
        image(buffer.getImage(), 100.0f, 100.0f, 600.0f, 600.0f);

        background(0);
        image(buffer.getImage(), 0, 0);

        // Halbtransparente Kopie leicht versetzt (Ghosting-Effekt)
        tint(255, 150);
        image(buffer.getImage(), 5, 5);
        //
        // Weitere versetzte Kopie für Trail-Effekt
        tint(255, 80);
        image(buffer.getImage(), -5, -5);

        tint(255, 255); // Reset tint

        // Kleine Vorschau in der Ecke (Picture-in-Picture)
        pushMatrix();
        translate(650, 450);
        scale(0.2, 0.2);
        tint(255, 200);
        image(buffer.getImage(), 0, 0);
        tint(255, 255);
        popMatrix();

        // Umrandung für die Vorschau
        noFill();
        stroke(255);
        strokeWeight(2);
        rect(650, 450, 160, 120);

        angle += 0.02;
    }

    void destroy() override
    {
    }
};

std::unique_ptr<Sketch> processing::createSketch()
{
    return std::make_unique<DemoApp>();
}
