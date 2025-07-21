#include "AABB.h"



std::vector<AABB> computeAABBs(const std::vector<Surface> &surfaces)
{
    std::vector<AABB> aabbs;

    for (const auto &surf : surfaces)
    {
        AABB aabb;
        //replace this with surf.vertics
        aabb.min_point = Vector3f(surf.vertices[0].x, surf.vertices[0].y, surf.vertices[0].z);
        aabb.max_point = Vector3f(surf.vertices[0].x, surf.vertices[0].y, surf.vertices[0].z);
        aabb.surface=surf;
        for (const auto &vertex : surf.vertices)
        {
            for (int i = 0; i < 3; ++i)
            {
                aabb.min_point[i] = std::min(aabb.min_point[i], vertex[i]);
                aabb.max_point[i] = std::max(aabb.max_point[i], vertex[i]);
            }
        }
        // these are just print statements to test whether they work or not
        //std::cout << aabb.max_point[0] << " " << aabb.max_point[1] << " " << aabb.max_point[2] << " " << "\n";
        //std::cout << aabb.min_point[0] << " " << aabb.min_point[1] << " " << aabb.min_point[2] << " " << "\n";
        aabbs.push_back(aabb);
    }

    return aabbs;
}
void printAABB(const AABB &aabb)
{
    std::cout << "AABB: ";
    std::cout << "Min(" << aabb.min_point.x << ", " << aabb.min_point.y << ", " << aabb.min_point.z << ") ";
    std::cout << "Max(" << aabb.max_point.x << ", " << aabb.max_point.y << ", " << aabb.max_point.z << ")\n";
}


bool rayIntersectsAABB( const Ray& ray, const AABB &aabb )
{
    float tx1 = (aabb.min_point.x - ray.o.x) / ray.d.x, tx2 = (aabb.max_point.x - ray.o.x) / ray.d.x;
    float tmin = std::min( tx1, tx2 ), tmax = std::max( tx1, tx2 );
    float ty1 = (aabb.min_point.y - ray.o.y) / ray.d.y, ty2 = (aabb.max_point.y - ray.o.y) / ray.d.y;
    tmin = std::max( tmin, std::min( ty1, ty2 ) ), tmax = std::min( tmax, std::max( ty1, ty2 ) );
    float tz1 = (aabb.min_point.z - ray.o.z) / ray.d.z, tz2 = (aabb.max_point.z - ray.o.z) / ray.d.z;
    tmin = std::max( tmin, std::min( tz1, tz2 ) ), tmax = std::min( tmax, std::max( tz1, tz2 ) );
    return tmax >= tmin && tmin < ray.t && tmax > 0;
}

bool compareAABB(const AABB& aabb1, const AABB& aabb2)
{
    double a=(aabb1.min_point.y+aabb1.max_point.y)/2;
    double b=(aabb2.min_point.y+aabb2.max_point.y)/2;
    return a<b;
}

void printAllAABB(const std::vector<AABB>& aabb)
{
    for( auto a:aabb)
    {
        printAABB(a);
    }
}