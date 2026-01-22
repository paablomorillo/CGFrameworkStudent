#pragma once
#include "image.h"

class ParticleSystem
{
    // Max particles (fixed array for speed)
    static const int MAX_PARTICLES = 200;

    struct Particle
    {
        Vector2 position = Vector2();   // Pixel position (float for smooth motion)
        Vector2 velocity = Vector2();   // Pixels per second
        Color color = Color::WHITE;

        float ttl = 0.0f;               // Time to live (seconds)
        bool inactive = true;           // Skip update/render if true
    };

    Particle particles[MAX_PARTICLES];

public:
    void Init(int width, int height);
    void Update(float dt, int width, int height); // dt in seconds
    void Render(Image* framebuffer);
};
