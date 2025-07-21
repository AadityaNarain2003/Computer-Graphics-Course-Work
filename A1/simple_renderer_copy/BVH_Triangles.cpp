#include "BVH_Triangles.h"

BVH_Triangles *buildBVHBottomUp_Triangles(std::vector<AABB_Triangles> &aabbList)
{
    std::vector<BVH_Triangles *> nodes;

    // this is to make each aabb in computed_AABB into a BVH
    for (const auto &aabb : aabbList)
    {
        BVH_Triangles *leafNode = new BVH_Triangles{aabb, nullptr, nullptr};
        nodes.push_back(leafNode);
    }

    while (nodes.size() > 1)
    {
        std::vector<BVH_Triangles *> nextLevel;

        for (size_t i = 0; i < nodes.size(); i += 2)
        {
            BVH_Triangles *mergedNode = new BVH_Triangles;
            mergedNode->left = nodes[i];
            mergedNode->right = (i + 1 < nodes.size()) ? nodes[i + 1] : nullptr;

            mergedNode->bounds = mergeAABB(nodes[i]->bounds, (i + 1 < nodes.size()) ? nodes[i + 1]->bounds : AABB_Triangles{});
            nextLevel.push_back(mergedNode);
        }
        nodes = std::move(nextLevel);
    }

    return (nodes.empty()) ? nullptr : nodes[0];
}

AABB_Triangles mergeAABB(const AABB_Triangles &aabb1, const AABB_Triangles &aabb2)
{
    AABB_Triangles mergedAABB;

    mergedAABB.min_point.x = std::min(aabb1.min_point.x, aabb2.min_point.x);
    mergedAABB.min_point.y = std::min(aabb1.min_point.y, aabb2.min_point.y);
    mergedAABB.min_point.z = std::min(aabb1.min_point.z, aabb2.min_point.z);

    mergedAABB.max_point.x = std::max(aabb1.max_point.x, aabb2.max_point.x);
    mergedAABB.max_point.y = std::max(aabb1.max_point.y, aabb2.max_point.y);
    mergedAABB.max_point.z = std::max(aabb1.max_point.z, aabb2.max_point.z);

    return mergedAABB;
}

void printBVHTriangles(const BVH_Triangles *node, int level)
{
    if (node == nullptr)
    {
        return;
    }
    std::cout << "Level " << level << ": ";
    std::cout << "Min: (" << node->bounds.min_point.x << ", " << node->bounds.min_point.y << ", " << node->bounds.min_point.z << ") ";
    std::cout << "Max: (" << node->bounds.max_point.x << ", " << node->bounds.max_point.y << ", " << node->bounds.max_point.z << ") ";
    std::cout << "Indices: (" << node->bounds.indices.x << ", " << node->bounds.indices.y << ", " << node->bounds.indices.z << ")\n";

    printBVHTriangles(node->left, level + 1);
    printBVHTriangles(node->right, level + 1);
}