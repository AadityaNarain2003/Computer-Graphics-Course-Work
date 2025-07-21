#ifndef BVH_AABB_H
#define BVH_AABB_H

#include "common.h"
#include "AABB.h"
#include "BVH_Triangles.h"

struct BVH_AABB
{
    AABB bounds;
    BVH_AABB* left;
    BVH_AABB* right;
    BVH_Triangles* triangles;
};

BVH_AABB* buildBVHBottomUp(std::vector<AABB>& aabbList);
BVH_AABB* buildBVHBottomUp_2(std::vector<AABB>& aabbList);
AABB mergeAABB(const AABB& aabb1, const AABB& aabb2);
void traverseBVH(BVH_AABB *node, const Ray &ray, double &len, Interaction &mini, bool &intersect);

#endif