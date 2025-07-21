#include "BVH_AABB.h"

BVH_AABB *buildBVHBottomUp(std::vector<AABB> &aabbList)
{
    std::vector<BVH_AABB *> nodes;

    // this is to make each aabb in computed_AABB into a BVH
    for (const auto &aabb : aabbList)
    {
        BVH_AABB *leafNode = new BVH_AABB{aabb, nullptr, nullptr,nullptr};
        nodes.push_back(leafNode);
    }

    while (nodes.size() > 1)
    {
        std::vector<BVH_AABB *> nextLevel;

        for (size_t i = 0; i < nodes.size(); i += 2)
        {
            BVH_AABB *mergedNode = new BVH_AABB;
            mergedNode->left = nodes[i];
            mergedNode->right = (i + 1 < nodes.size()) ? nodes[i + 1] : nullptr;

            mergedNode->bounds = mergeAABB(nodes[i]->bounds, (i + 1 < nodes.size()) ? nodes[i + 1]->bounds : AABB{});
            nextLevel.push_back(mergedNode);
        }
        nodes = std::move(nextLevel);
    }

    return (nodes.empty()) ? nullptr : nodes[0];
}

BVH_AABB *buildBVHBottomUp_2(std::vector<AABB> &aabbList)
{
    std::vector<BVH_AABB *> nodes;

    // this is to make each aabb in computed_AABB into a BVH
    for (const auto &aabb : aabbList)
    {
        std::vector<AABB_Triangles> temp = computeTriangleAABBs(aabb.surface);
        //printAABBs_triangles(temp);
        BVH_Triangles* bvh_t=buildBVHBottomUp_Triangles(temp);
        printBVHTriangles(bvh_t,0);
        BVH_AABB *leafNode = new BVH_AABB{aabb, nullptr, nullptr,bvh_t};
        // here i will build the aabbs of each of the smaller surfaces and do

        nodes.push_back(leafNode);
    }
    //printBVHTriangles(nodes[0]->triangles,0);

    while (nodes.size() > 1)
    {
        std::vector<BVH_AABB *> nextLevel;

        for (size_t i = 0; i < nodes.size(); i += 2)
        {
            BVH_AABB *mergedNode = new BVH_AABB;
            mergedNode->left = nodes[i];
            mergedNode->right = (i + 1 < nodes.size()) ? nodes[i + 1] : nullptr;

            mergedNode->bounds = mergeAABB(nodes[i]->bounds, (i + 1 < nodes.size()) ? nodes[i + 1]->bounds : AABB{});
            nextLevel.push_back(mergedNode);
        }
        nodes = std::move(nextLevel);
    }

    return (nodes.empty()) ? nullptr : nodes[0];
}

AABB mergeAABB(const AABB &aabb1, const AABB &aabb2)
{
    AABB mergedAABB;

    mergedAABB.min_point.x = std::min(aabb1.min_point.x, aabb2.min_point.x);
    mergedAABB.min_point.y = std::min(aabb1.min_point.y, aabb2.min_point.y);
    mergedAABB.min_point.z = std::min(aabb1.min_point.z, aabb2.min_point.z);

    mergedAABB.max_point.x = std::max(aabb1.max_point.x, aabb2.max_point.x);
    mergedAABB.max_point.y = std::max(aabb1.max_point.y, aabb2.max_point.y);
    mergedAABB.max_point.z = std::max(aabb1.max_point.z, aabb2.max_point.z);

    return mergedAABB;
}

void traverseBVH(BVH_AABB *node, const Ray &ray, double &len, Interaction &mini, bool &intersect)
{
    if (node == nullptr)
    {
        return;
    }
    if (node->left == nullptr && node->right == nullptr)
    {
        Interaction si = node->bounds.surface.rayIntersect(ray);
        if (si.didIntersect)
        {
            double vx = si.p.x - ray.o.x;
            double vy = si.p.y - ray.o.y;
            double vz = si.p.z - ray.o.z;
            double l = vx * vx + vy * vy + vz * vz;
            if (l < len)
            {
                mini = si;
                len = l;
            }
            intersect = true;
        }

        return;
    }
    if (node->left != nullptr)
    {
        bool intersectLeft = rayIntersectsAABB(ray, node->left->bounds);
        if (intersectLeft == true)
        {
            traverseBVH(node->left, ray, len, mini, intersect);
        }
    }
    if (node->right != nullptr)
    {
        bool intersectRight = rayIntersectsAABB(ray, node->right->bounds);
        if (intersectRight == true)
        {
            traverseBVH(node->right, ray, len, mini, intersect);
        }
    }
}
