#ifndef BVH_TRIANGLES_H
#define BVH_TRIANGLES_H

#include "common.h"
#include "AABB.h"
#include "AABB_Triangles.h"

struct BVH_Triangles
{
    AABB_Triangles bounds;
    BVH_Triangles* left;
    BVH_Triangles* right;
};

BVH_Triangles *buildBVHBottomUp_Triangles(std::vector<AABB_Triangles> &aabbList);
AABB_Triangles mergeAABB(const AABB_Triangles &aabb1, const AABB_Triangles &aabb2);
void printBVHTriangles(const BVH_Triangles *node, int level);

#endif