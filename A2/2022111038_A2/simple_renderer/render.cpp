#include "render.h"

Integrator::Integrator(Scene &scene)
{
    this->scene = scene;
    this->outputImage.allocate(TextureType::UNSIGNED_INTEGER_ALPHA, this->scene.imageResolution);
}

std::vector<float> shade(const Light &light, Vector3f textureColour)
{
    std::vector<float> resultColor(3, 0.0f);

    for (int i = 0; i < 3; ++i)
    {
        if (light.type == DIRECTIONAL_LIGHT)
        {
            resultColor[i] += (light.direction.radiance[i] * (1.0f / M_PI));
        }
        else
        {
            resultColor[i] += (light.point.radiance[i]) * (1.0f / M_PI);
        }
    }
    return resultColor;
}
Vector3f Barycentric(Vector3f a, Vector3f b, Vector3f c, Vector3f p)
{
    Vector3f v0 = b - a, v1 = c - a, v2 = p - a;
    float d00 = Dot(v0, v0);
    float d01 = Dot(v0, v1);
    float d11 = Dot(v1, v1);
    float d20 = Dot(v2, v0);
    float d21 = Dot(v2, v1);
    float denom = d00 * d11 - d01 * d01;
    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;

    return Vector3f(u, v, w);
}

long long Integrator::render(int type)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int x = 0; x < this->scene.imageResolution.x; x++)
    {
        for (int y = 0; y < this->scene.imageResolution.y; y++)
        {
            Ray cameraRay = this->scene.camera.generateRay(x, y);
            Interaction si = this->scene.rayIntersect(cameraRay);

            float color[3] = {0, 0, 0};
            float r = 1;
            Vector3f textureColour;
            textureColour[0] = 1;
            textureColour[1] = 1;
            textureColour[2] = 1;
            if (si.didIntersect)
            {
                // this is for texture

                Surface *s = si.sur;
                if (s->hasDiffuseTexture())
                {
                    Vector3f bc = Barycentric(si.v1, si.v2, si.v3, si.p);
                    float U = bc[0] * si.uv1[0] + bc[1] * si.uv2[0] + bc[2] * si.uv3[0];
                    float V = bc[0] * si.uv1[1] + bc[1] * si.uv2[1] + bc[2] * si.uv3[1];
                    float textureCordx = U;
                    float textureCordy = V;
                    if (type == 0)
                    {
                        textureColour = s->diffuseTexture.nearestNeighbourFetch(Vector2f(textureCordx, textureCordy));
                    }
                    else
                    {
                        textureColour = s->diffuseTexture.bilinearFetch(Vector2f(textureCordx, textureCordy));
                    }
                    // std::cout << textureColour[0] << " " << textureColour[1] << " " << textureColour[2] << "\n";
                    // for (int i = 0; i < 3; ++i)
                    //{
                    //    color[i] += textureColour[i];
                    //}
                }

                // this is for lighting

                for (auto light : this->scene.lights)
                {
                    Vector3f shadowRayDir;
                    if (light.type == DIRECTIONAL_LIGHT)
                    {
                        // this is just for directional light
                        shadowRayDir = Vector3f(light.direction.direction[0], light.direction.direction[1], light.direction.direction[2]);
                        r = shadowRayDir.Length();
                        shadowRayDir = Normalize(shadowRayDir);
                    }
                    else
                    {
                        // this is for point light
                        shadowRayDir = Vector3f(light.point.location[0] - si.p[0], light.point.location[1] - si.p[1], light.point.location[2] - si.p[2]);
                        r = shadowRayDir.Length();
                        shadowRayDir = Normalize(shadowRayDir);
                    }

                    Ray shadowRay(si.p + 1e-3f * si.n, shadowRayDir);
                    Interaction shadowIntersect;

                    shadowIntersect = this->scene.rayIntersect(shadowRay);

                    std::vector<float> shading = shade(light, textureColour);
                    float cosineFactor = Dot(si.n, shadowRayDir);
                    cosineFactor = std::max(0.0f, cosineFactor);

                    if (!shadowIntersect.didIntersect && light.type == DIRECTIONAL_LIGHT)
                    {
                        float tempcolor[3] = {0, 0, 0};

                        for (int i = 0; i < 3; ++i)
                        {
                            tempcolor[i] += (shading[i]) * cosineFactor;
                        }

                        for (int i = 0; i < 3; ++i)
                        {
                            color[i] += ((tempcolor[i]));
                        }
                    }

                    else if ((!shadowIntersect.didIntersect || shadowIntersect.t > r) && light.type == POINT_LIGHT)
                    {
                        float tempcolor[3] = {0, 0, 0};

                        for (int i = 0; i < 3; ++i)
                        {
                            tempcolor[i] += (shading[i]) * cosineFactor;
                        }

                        for (int i = 0; i < 3; ++i)
                        {
                            color[i] += ((tempcolor[i]) / (r * r));
                        }
                    }
                }
                for (int i = 0; i < 3; ++i)
                {
                    color[i] = color[i] * textureColour[i];
                }
                Vector3f X = (Vector3f(color[0], color[1], color[2]));
                // std::cout << X[0] << " " << X[1] << " " << X[2] << " \n";
                this->outputImage.writePixelColor(X, x, y);
            }
            else
            {
                Vector3f X = (Vector3f(color[0], color[1], color[2]));
                this->outputImage.writePixelColor(X, x, y);
            }
        }
    }
    auto finishTime = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
}

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        std::cerr << "Usage: ./render <scene_config> <out_path> <type>";
        return 1;
    }

    Scene scene(argv[1]);

    Integrator rayTracer(scene);

    auto renderTime = rayTracer.render( std::stoi(argv[3]));

    std::cout << "Render Time: " << std::to_string(renderTime / 1000.f) << " ms" << std::endl;
    rayTracer.outputImage.save(argv[2]);

    return 0;
}
