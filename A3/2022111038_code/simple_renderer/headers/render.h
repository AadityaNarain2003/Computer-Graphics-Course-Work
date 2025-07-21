#pragma once

#include "scene.h"

struct Integrator {
    Integrator(Scene& scene);

    long long render();
    long long render_Anti_aliasing(int samples);
    long long MonteCarloRender_Uniform(int spp,int mul);
    long long MonteCarloRender_Cosine(int spp,int mul);
    long long render_sampler(int spp,int mul);
    Scene scene;
    Texture outputImage;
};