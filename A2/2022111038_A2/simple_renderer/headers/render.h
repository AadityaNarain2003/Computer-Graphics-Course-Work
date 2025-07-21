#pragma once

#include "scene.h"

struct Integrator {
    Integrator(Scene& scene);

    long long render(int type);

    Scene scene;
    Texture outputImage;
};