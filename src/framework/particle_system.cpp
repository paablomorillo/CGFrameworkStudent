#include "particle_system.h"
#include <cstdlib>   // rand
#include <algorithm> // std::min/std::max

static float frand01()
{
    // Random float in [0,1)
    return (rand() % 10000) / 10000.0f;
}

void ParticleSystem::Init(int width, int height)
{
    // Init starfield particles
    for (int i = 0; i < MAX_PARTICLES; ++i)
    {
        particles[i].inactive = false;

        particles[i].ttl = 5.0f + frand01() * 5.0f;

        float vx = (frand01() * 2.0f - 1.0f) * 30.0f; // drift
        float vy = 50.0f + frand01() * 150.0f;        // down speed
        particles[i].velocity = Vector2(vx, vy);

        unsigned char v = (unsigned char)(180 + frand01() * 75);
        particles[i].color = Color(v, v, v);

        particles[i].ttl = 5.0f + frand01() * 5.0f;
    }
}

void ParticleSystem::Update(float dt, int width, int height)
{
    // Update using dt (seconds)
    for (int i = 0; i < MAX_PARTICLES; ++i)
    {
        if (particles[i].inactive)
            continue;

        particles[i].ttl -= dt;
        particles[i].position = particles[i].position + particles[i].velocity * dt;

        // Respawn if dead/out of screen
        if (particles[i].ttl <= 0.0f ||
            particles[i].position.y >= height ||
            particles[i].position.x < 0 || particles[i].position.x >= width)
        {
            particles[i].position = Vector2(frand01() * width, 0.0f);

            float vx = (frand01() * 2.0f - 1.0f) * 30.0f;
            float vy = 50.0f + frand01() * 150.0f;
            particles[i].velocity = Vector2(vx, vy);

            particles[i].ttl = 5.0f + frand01() * 5.0f;
        }
    }
}

void ParticleSystem::Render(Image* framebuffer)
{
    // Draw each particle as a pixel (safe write)
    for (int i = 0; i < MAX_PARTICLES; ++i)
    {
        if (particles[i].inactive)
            continue;

        int x = (int)particles[i].position.x;
        int y = (int)particles[i].position.y;

        framebuffer->SetPixelSafeInt(x, y, particles[i].color);
    }
}
