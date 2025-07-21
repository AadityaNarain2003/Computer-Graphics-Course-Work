#ifndef AABB_H
#define AABB_H

#include "common.h"
#include "surface.h"

struct AABB {
    Vector3f min_point;
    Vector3f max_point;  
    Surface surface; 
};

std::vector<AABB> computeAABBs(const std::vector<Surface>& surfaces);

bool rayIntersectsAABB(const Ray &ray, const AABB &aabb);
void printAABB(const AABB& aabb);
void printAllAABB(const std::vector<AABB>& aabb);

bool compareAABB(const AABB& aabb1, const AABB& aabb2);

#endif