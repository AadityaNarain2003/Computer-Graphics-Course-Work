#include "AABB_Triangles.h"

AABB_Triangles getTriangleAABB(const Vector3f &v1, const Vector3f &v2, const Vector3f &v3)
{
    AABB_Triangles aabb;
    aabb.min_point.x = std::min({v1.x, v2.x, v3.x});
    aabb.min_point.y = std::min({v1.y, v2.y, v3.y});
    aabb.min_point.z = std::min({v1.z, v2.z, v3.z});

    aabb.max_point.x = std::max({v1.x, v2.x, v3.x});
    aabb.max_point.y = std::max({v1.y, v2.y, v3.y});
    aabb.max_point.z = std::max({v1.z, v2.z, v3.z});

    return aabb;
}

void printSurfaceData(const Surface& surface) {
    std::cout << "Vertices:\n";
    for (const auto& vertex : surface.vertices) {
        std::cout << "(" << vertex.x << ", " << vertex.y << ", " << vertex.z << ")\n";
    }

    std::cout << "\nNormals:\n";
    for (const auto& normal : surface.normals) {
        std::cout << "(" << normal.x << ", " << normal.y << ", " << normal.z << ")\n";
    }

    std::cout << "\nUVs:\n";
    for (const auto& uv : surface.uvs) {
        std::cout << "(" << uv.x << ", " << uv.y << ")\n";
    }

    std::cout << "\nIndices:\n";
    for (const auto& index : surface.indices) {
        std::cout << "(" << index.x << ", " << index.y << ", " << index.z << ")\n";
    }

    std::cout << "\nOther Data:\n";
    std::cout << "isLight: " << surface.isLight << "\n";
    std::cout << "shapeIdx: " << surface.shapeIdx << "\n";
    std::cout << "Diffuse: (" << surface.diffuse.x << ", " << surface.diffuse.y << ", " << surface.diffuse.z << ")\n";
    std::cout << "Alpha: " << surface.alpha << "\n";

    // Print other members as needed
}

std::vector<AABB_Triangles> computeTriangleAABBs(const Surface &surface)
{
    printSurfaceData(surface);
    std::vector<AABB_Triangles> triangleAABBs;
    const std::vector<Vector3i> &indices = surface.indices;
    const std::vector<Vector3f> &vertices = surface.vertices;

    for (size_t i = 0; i < indices.size(); ++i)
    {
        const Vector3i &faceIndices = indices[i];

        // Extract vertices of the triangle
        const Vector3f &v1 = vertices[faceIndices.x ]; // Adjust for 1-based indexing in OBJ
        const Vector3f &v2 = vertices[faceIndices.y ];
        const Vector3f &v3 = vertices[faceIndices.z ];

        // Create AABB for the current triangle
        AABB_Triangles triangleAABB = getTriangleAABB(v1, v2, v3);
        //--------------------------------------------------------------
        /*
        triangleAABB.v1=v1;
        triangleAABB.v2=v2;
        triangleAABB.v3=v3;
        triangleAABB.n1=surface.normals[faceIndices.x-1];
        triangleAABB.n2=surface.normals[faceIndices.y-1];
        triangleAABB.n3=surface.normals[faceIndices.z-1];
        */
        //--------------------------------------------------------------
        triangleAABB.indices=faceIndices;
        // Add the AABB to the list
        triangleAABBs.push_back(triangleAABB);
    }
    return triangleAABBs;
}

void printAABBs_triangles(const std::vector<AABB_Triangles> &aabbs)
{
    std::cout << "AABBs:\n";
    for (size_t i = 0; i < aabbs.size(); ++i)
    {
        const AABB_Triangles &aabb = aabbs[i];
        std::cout << "Triangle " << (i + 1) << " AABB:\n";
        std::cout << "  Min: " << aabb.min_point.x << ", " << aabb.min_point.y << ", " << aabb.min_point.z << "\n";
        std::cout << "  Max: " << aabb.max_point.x << ", " << aabb.max_point.y << ", " << aabb.max_point.z << "\n";
        std::cout << "  Ind: " << aabb.indices.x << ", " << aabb.indices.y << ", " << aabb.indices.z << "\n";
    }
}

bool rayIntersectsAABB_Triangles( const Ray& ray, const AABB_Triangles &aabb )
{
    float tx1 = (aabb.min_point.x - ray.o.x) / ray.d.x, tx2 = (aabb.max_point.x - ray.o.x) / ray.d.x;
    float tmin = std::min( tx1, tx2 ), tmax = std::max( tx1, tx2 );
    float ty1 = (aabb.min_point.y - ray.o.y) / ray.d.y, ty2 = (aabb.max_point.y - ray.o.y) / ray.d.y;
    tmin = std::max( tmin, std::min( ty1, ty2 ) ), tmax = std::min( tmax, std::max( ty1, ty2 ) );
    float tz1 = (aabb.min_point.z - ray.o.z) / ray.d.z, tz2 = (aabb.max_point.z - ray.o.z) / ray.d.z;
    tmin = std::max( tmin, std::min( tz1, tz2 ) ), tmax = std::min( tmax, std::max( tz1, tz2 ) );
    return tmax >= tmin && tmin < ray.t && tmax > 0;
}