#ifndef AABB_TRIANGLES_H
#define AABB_TRIANGLES_H

#include "common.h"
#include "surface.h"

struct AABB_Triangles 
{
    Vector3f min_point;
    Vector3f max_point;  
    Vector3i indices; 


    //some test stuff
    Vector3f v1;
    Vector3f v2;
    Vector3f v3;
    Vector3f n1;
    Vector3f n2;
    Vector3f n3;
};

AABB_Triangles getTriangleAABB(const Vector3f &v1, const Vector3f &v2, const Vector3f &v3);
std::vector<AABB_Triangles> computeTriangleAABBs(const Surface &surface);
void printAABBs_triangles(const std::vector<AABB_Triangles> &aabbs);
bool rayIntersectsAABB_Triangles( const Ray& ray, const AABB_Triangles &aabb );

#endif