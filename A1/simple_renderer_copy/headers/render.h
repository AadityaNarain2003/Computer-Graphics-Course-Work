#pragma once

#include "scene.h"
#include "BVH_AABB.h"

struct Integrator {
    Integrator(Scene& scene);

    long long render();
    //user defined function
    long long renderAABB();
    long long renderBVH_AABB();
    long long renderBVH_AABB_Triangles();

    Scene scene;
    Texture outputImage;
};

