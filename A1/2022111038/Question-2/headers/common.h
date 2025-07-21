#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include <cmath>

#include "vec.h"

#include "json/include/nlohmann/json.hpp"

#define M_PI 3.14159263

struct Ray {
    Vector3f o, d;
    double t = 1e308;
    double tmax = 1e308;


    Ray(Vector3f origin, Vector3f direction, double t = 1e308, double tmax = 1e308)
        : o(origin), d(direction), t(t), tmax(tmax) {};
};

struct Interaction {
    Vector3f p, n;
    double t = 1e30;
    bool didIntersect = false;
};