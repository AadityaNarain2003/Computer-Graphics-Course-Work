#pragma once

#include "common.h"

struct Camera {
    Vector3f from, to, up;
    double fieldOfView;
    Vector2i imageResolution;

    double focusDistance = 1;
    double aspect;

    Vector3f u, v, w;
    Vector3f pixelDeltaU, pixelDeltaV;
    Vector3f upperLeft;

    Camera() {};
    Camera(Vector3f from, Vector3f to, Vector3f up, double fieldOfView, Vector2i imageResolution);

    Ray generateRay(int x, int y);
    
};