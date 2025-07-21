#include "render.h"
#include "BVH_AABB.h"
// user defined function starts
void printValues(const Surface *s, const BVH_Triangles *node)
{
    // Extract vertices and normals
    Vector3f p1 = s->vertices[node->bounds.indices.x];
    Vector3f p2 = s->vertices[node->bounds.indices.y];
    Vector3f p3 = s->vertices[node->bounds.indices.z];

    Vector3f n1 = s->normals[node->bounds.indices.x];
    Vector3f n2 = s->normals[node->bounds.indices.y];
    Vector3f n3 = s->normals[node->bounds.indices.z];

    // Calculate the normalized normal
    Vector3f n = Normalize(n1 + n2 + n3);

    // Print values
    std::cout << "p1: (" << p1.x << ", " << p1.y << ", " << p1.z << ")\n";
    std::cout << "p2: (" << p2.x << ", " << p2.y << ", " << p2.z << ")\n";
    std::cout << "p3: (" << p3.x << ", " << p3.y << ", " << p3.z << ")\n";

    std::cout << "n1: (" << n1.x << ", " << n1.y << ", " << n1.z << ")\n";
    std::cout << "n2: (" << n2.x << ", " << n2.y << ", " << n2.z << ")\n";
    std::cout << "n3: (" << n3.x << ", " << n3.y << ", " << n3.z << ")\n";

    std::cout << "Normalized n: (" << n.x << ", " << n.y << ", " << n.z << ")\n";
}

void traverseBVH_triangle(Surface *s, BVH_Triangles *node, const Ray &ray, double &len2, Interaction &mini2, bool &intersect2)
{
    if (node == nullptr)
    {
        return;
    }
    if (node->left == nullptr && node->right == nullptr)
    {
        
        Vector3f p1 = s->vertices[node->bounds.indices.x];
        Vector3f p2 = s->vertices[node->bounds.indices.y];
        Vector3f p3 = s->vertices[node->bounds.indices.z];

        Vector3f n1 = s->normals[node->bounds.indices.x];
        Vector3f n2 = s->normals[node->bounds.indices.y];
        Vector3f n3 = s->normals[node->bounds.indices.z];
        Vector3f n = Normalize(n1 + n2 + n3);
        //printValues(s,node);
        Interaction si = s->rayTriangleIntersect(ray, p1, p2, p3, n);
        /*
        Vector3f n = Normalize(node->bounds.n1 +node->bounds.n2 + node->bounds.n3);
        // printValues(s,node);
        Interaction si = s->rayTriangleIntersect(ray, node->bounds.v1, node->bounds.v2, node->bounds.v3, n);
        */
        if (si.t <= len2 && si.didIntersect)
        {
            mini2 = si;
            len2 = si.t;
            intersect2 = true;
        }

        return;
    }
    if (node->left != nullptr)
    {
        bool intersectLeft = rayIntersectsAABB_Triangles(ray, node->left->bounds);
        if (intersectLeft == true)
        {
            traverseBVH_triangle(s, node->left, ray, len2, mini2, intersect2);
        }
    }
    if (node->right != nullptr)
    {
        bool intersectRight = rayIntersectsAABB_Triangles(ray, node->right->bounds);
        if (intersectRight == true)
        {
            traverseBVH_triangle(s, node->right, ray, len2, mini2, intersect2);
        }
    }
}

void traverseBVH_2(BVH_AABB *node, const Ray &ray, double &len, Interaction &mini, bool &intersect)
{
    if (node == nullptr)
    {
        return;
    }
    if (node->left == nullptr && node->right == nullptr)
    {
        Interaction mini2;
        double len2 = ray.t;
        bool intersect2 = false;
        Surface *surf = &(node->bounds.surface);
        traverseBVH_triangle(surf, node->triangles, ray, len2, mini2, intersect2);
        // if(intersect2)
        //{
        //     std::cout<< "TRUE "<< len2 << " "<< len << "\n";
        // }
        if (intersect2 == true && len2 <= len)
        {
            mini = mini2;
            len = len2;
            intersect = true;
        }
        return;
    }
    if (node->left != nullptr)
    {
        bool intersectLeft = rayIntersectsAABB(ray, node->left->bounds);
        if (intersectLeft == true)
        {
            traverseBVH_2(node->left, ray, len, mini, intersect);
        }
    }
    if (node->right != nullptr)
    {
        bool intersectRight = rayIntersectsAABB(ray, node->right->bounds);
        if (intersectRight == true)
        {
            traverseBVH_2(node->right, ray, len, mini, intersect);
        }
    }
}

long long Integrator::renderBVH_AABB_Triangles()
{
    auto startTime = std::chrono::high_resolution_clock::now();

    this->scene.computed_AABBs = computeAABBs(this->scene.surfaces);
    std::sort(this->scene.computed_AABBs.begin(), this->scene.computed_AABBs.end(), compareAABB);
    BVH_AABB *root = buildBVHBottomUp_2(this->scene.computed_AABBs);
    for (int x = 0; x < this->scene.imageResolution.x; x++)
    {
        for (int y = 0; y < this->scene.imageResolution.y; y++)
        {
            Ray cameraRay = this->scene.camera.generateRay(x, y);
            Interaction mini;
            double len = __DBL_MAX__;
            bool intersect = false;
            traverseBVH_2(root, cameraRay, len, mini, intersect);

            if (intersect == false)
            {
                this->outputImage.writePixelColor(Vector3f(0.f, 0.f, 0.f), x, y);
            }
            else
            {
                this->outputImage.writePixelColor(0.5f * (mini.n + Vector3f(1.f, 1.f, 1.f)), x, y);
            }
            // std::cout << x << " "<< y << "\n";
        }
    }

    auto finishTime = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
}

long long Integrator::renderBVH_AABB()
{
    auto startTime = std::chrono::high_resolution_clock::now();
    this->scene.computed_AABBs = computeAABBs(this->scene.surfaces);
    std::sort(this->scene.computed_AABBs.begin(), this->scene.computed_AABBs.end(), compareAABB);
    BVH_AABB *root = buildBVHBottomUp(this->scene.computed_AABBs);
    // printBVH(root, 0);
    std::cout << this->scene.imageResolution.x << "\n";
    std::cout << this->scene.imageResolution.y << "\n";
    for (int x = 0; x < this->scene.imageResolution.x; x++)
    {
        for (int y = 0; y < this->scene.imageResolution.y; y++)
        {
            Ray cameraRay = this->scene.camera.generateRay(x, y);
            // now that I have generated a Ray, I need to traverse down all the possible paths of BVH
            // where the GENERATED RAY CAN INTERSECT WITH THE
            Interaction mini;
            double len = __DBL_MAX__;
            bool intersect = false;
            traverseBVH(root, cameraRay, len, mini, intersect);
            if (intersect == false)
            {
                this->outputImage.writePixelColor(Vector3f(0.f, 0.f, 0.f), x, y);
            }
            else
            {
                this->outputImage.writePixelColor(0.5f * (mini.n + Vector3f(1.f, 1.f, 1.f)), x, y);
            }
            // std::cout << x << " "<< y << "\n";
        }
    }
    auto finishTime = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
}
long long Integrator::renderAABB()
{
    auto startTime = std::chrono::high_resolution_clock::now();
    this->scene.computed_AABBs = computeAABBs(this->scene.surfaces);

    int count_of_surfaces = this->scene.computed_AABBs.size();
    std::cout << count_of_surfaces << " is count\n";
    std::cout << this->scene.imageResolution.x << "\n";
    std::cout << this->scene.imageResolution.y << "\n";
    for (int x = 0; x < this->scene.imageResolution.x; x++)
    {
        for (int y = 0; y < this->scene.imageResolution.y; y++)
        {
            Ray cameraRay = this->scene.camera.generateRay(x, y);
            bool intersect = false;
            Interaction mini;
            double len = __DBL_MAX__;
            for (int i = 0; i < count_of_surfaces; i++)
            {

                bool doesIntersect = rayIntersectsAABB(cameraRay, this->scene.computed_AABBs[i]);
                if (doesIntersect)
                {
                    Interaction si = this->scene.computed_AABBs[i].surface.rayIntersect(cameraRay);
                    if (si.didIntersect)
                    {
                        double vx = si.p.x - cameraRay.o.x;
                        double vy = si.p.y - cameraRay.o.y;
                        double vz = si.p.z - cameraRay.o.z;
                        double l = vx * vx + vy * vy + vz * vz;
                        if (l < len)
                        {
                            mini = si;
                            len = l;
                        }

                        intersect = true;
                    }
                }
            }
            if (intersect == false)
            {
                this->outputImage.writePixelColor(Vector3f(0.f, 0.f, 0.f), x, y);
            }
            else
            {
                this->outputImage.writePixelColor(0.5f * (mini.n + Vector3f(1.f, 1.f, 1.f)), x, y);
            }
            // std::cout << x << " "<< y << "\n";
        }
    }
    auto finishTime = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
}

// user defined function ends

Integrator::Integrator(Scene &scene)
{
    this->scene = scene;
    this->outputImage.allocate(TextureType::UNSIGNED_INTEGER_ALPHA, this->scene.imageResolution);
}

long long Integrator::render()
{
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int x = 0; x < this->scene.imageResolution.x; x++)
    {
        for (int y = 0; y < this->scene.imageResolution.y; y++)
        {
            Ray cameraRay = this->scene.camera.generateRay(x, y);
            Interaction si = this->scene.rayIntersect(cameraRay);

            if (si.didIntersect)
                this->outputImage.writePixelColor(0.5f * (si.n + Vector3f(1.f, 1.f, 1.f)), x, y);
            else
                this->outputImage.writePixelColor(Vector3f(0.f, 0.f, 0.f), x, y);
        }
    }
    auto finishTime = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
}

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        std::cerr << "Usage: ./render <scene_config> <out_path> <num>\n";
        return 1;
    }
    Scene scene(argv[1]);
    Integrator rayTracer(scene);

    int num = std::stoi(argv[3]);
    if (num == 0)
    {
        auto renderTime = rayTracer.render();
        std::cout << "Render Time: " << std::to_string(renderTime / 1000.f) << " ms" << std::endl;
        rayTracer.outputImage.save(argv[2]);
    }
    else if (num == 1)
    {
        auto renderTime = rayTracer.renderAABB();
        std::cout << "Render Time: " << std::to_string(renderTime / 1000.f) << " ms" << std::endl;
        rayTracer.outputImage.save(argv[2]);
    }
    else if (num == 2)
    {
        auto renderTime = rayTracer.renderBVH_AABB();
        std::cout << "Render Time: " << std::to_string(renderTime / 1000.f) << " ms" << std::endl;
        rayTracer.outputImage.save(argv[2]);
    }
    else if (num == 3)
    {
        auto renderTime = rayTracer.renderBVH_AABB_Triangles();
        std::cout << "Render Time: " << std::to_string(renderTime / 1000.f) << " ms" << std::endl;
        rayTracer.outputImage.save(argv[2]);
    }
    else
    {
        std::cout << "wrong input\n";
    }

    return 0;
}
