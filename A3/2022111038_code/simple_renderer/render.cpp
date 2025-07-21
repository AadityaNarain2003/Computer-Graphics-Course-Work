#include "render.h"
#include <omp.h>
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
            Vector3f result(0, 0, 0);

            if (si.didIntersect)
            {
                Vector3f radiance;
                LightSample ls;
                for (Light &light : this->scene.lights)
                {
                    std::tie(radiance, ls) = light.sample(&si);

                    Ray shadowRay(si.p + 1e-3f * si.n, ls.wo);
                    Interaction siShadow = this->scene.rayIntersect(shadowRay);

                    if (!siShadow.didIntersect || siShadow.t > ls.d)
                    {
                        //result += si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance * std::abs(Dot(si.n, ls.wo));
                    }
                }
            }
            Interaction rayemitter = this->scene.rayEmitterIntersect(cameraRay);
            result += rayemitter.emissiveColor;
            this->outputImage.writePixelColor(result, x, y);
        }
    }
    auto finishTime = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
}

Vector3f UniformSampleHemisphere(const Vector2f &u)
{
    float z = u[0];
    float r = std::sqrt(std::max((float)0, (float)1. - z * z));
    float phi = 2 * M_PI * u[1];
    return Vector3f(r * std::cos(phi), r * std::sin(phi), z);
}

Vector3f UniformSampleHemisphereCaller()
{
    float a = next_float();
    float b = next_float();
    Vector2f u = Vector2f(a, b);
    return UniformSampleHemisphere(u);
}

Vector3f CosineWeightedSampleHemisphere(const Vector2f &u)
{
    float z = std::sqrt(u[0]);

    float phi = 2 * M_PI * u[1];

    float theta = std::acos(z);
    float x = std::sin(theta) * std::cos(phi);
    float y = std::sin(theta) * std::sin(phi);

    return Vector3f(x, y, std::cos(theta));
}

Vector3f CosineWeightedSampleHemisphereCaller()
{
    float a = next_float();
    float b = next_float();
    Vector2f u = Vector2f(a, b);
    return CosineWeightedSampleHemisphere(u);
}

std::vector<Vector3f> getONB(Vector3f normal)
{
    std::vector<Vector3f> onb(3);
    Vector3f n = Normalize(normal);
    onb[2] = n; // Set the third basis vector to the normalized input normal vector

    // Find the smallest component index of the normal vector
    int small = 0;
    if (std::fabs(n[1]) < std::fabs(n[small]))
        small = 1;
    if (std::fabs(n[2]) < std::fabs(n[small]))
        small = 2;

    // Set the first component of the second basis vector to 0
    onb[1][small] = 0;

    // Swap the remaining components to obtain an orthogonal vector
    int first = (small + 1) % 3;
    int last = (small + 2) % 3;
    onb[1][last] = -n[first];
    onb[1][first] = n[last];

    // Normalize the second basis vector
    onb[1] = Normalize(onb[1]);

    // Compute the first basis vector using cross product
    onb[0] = Cross(onb[1], onb[2]);

    return onb;
}
Vector3f toLocalCord(std::vector<Vector3f> onb, Vector3f p)
{
    Vector3f localCord;
    localCord[0] = onb[0][0] * p.x + onb[0][1] * p.y + onb[0][2] * p.z;
    localCord[1] = onb[1][0] * p.x + onb[1][1] * p.y + onb[1][2] * p.z;
    localCord[2] = onb[2][0] * p.x + onb[2][1] * p.y + onb[2][2] * p.z;

    return localCord;
}

Vector3f toWorldCord(const std::vector<Vector3f> onb, const Vector3f p)
{ 
    Vector3f worldCoord;
    worldCoord[0] = onb[0][0] * p.x + onb[1][0] * p.y + onb[2][0] * p.z;
    worldCoord[1] = onb[0][1] * p.x + onb[1][1] * p.y + onb[2][1] * p.z;
    worldCoord[2] = onb[0][2] * p.x + onb[1][2] * p.y + onb[2][2] * p.z;
    return worldCoord;
}

long long Integrator::MonteCarloRender_Uniform(int spp,int mul)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int x = 0; x < this->scene.imageResolution.x; x++) // this->scene.imageResolution.x
    {
        for (int y = 0; y < this->scene.imageResolution.y; y++)
        {
            Vector3f overallResult(0, 0, 0);
            for (int z = 0; z < spp; z++)
            {
                // printf("%d %d\n", x, y);
                Ray cameraRay = this->scene.camera.generateRay_Anti_aliasing(x, y);
                Interaction si = this->scene.rayIntersect(cameraRay);
                Vector3f result(0, 0, 0);

                if (si.didIntersect)
                {
                    // now onto doing montecarlo
                    std::vector<Vector3f> onb = getONB(si.n);
                    Vector3f result3(0, 0, 0);
                    for (int i = 0; i < mul; i++)
                    {
                        Vector3f pointOnHemisphere = UniformSampleHemisphereCaller();
                        // pointOnHemisphere = toLocalCord(onb, pointOnHemisphere);
                        Vector3f directionGlobal = toWorldCord(onb, pointOnHemisphere);

                        if (Dot(si.n, directionGlobal) < 0)
                        {
                            printf("HIIII\n");
                        }
                        // initializing a ray
                        Ray fromPoint;
                        fromPoint.d = directionGlobal;
                        fromPoint.o = si.p;
                        fromPoint.tmax = 1e30f;
                        fromPoint.t = 1e30f;
                        // now i need to check if the intersection has been hitting the light or an object before
                        Interaction emitterInteraction = this->scene.rayEmitterIntersect(fromPoint);
                        if (emitterInteraction.didIntersect)
                        {
                            Ray shadowRay;
                            shadowRay.d = (directionGlobal);
                            shadowRay.o = si.p + 0.0001f * shadowRay.d;
                            shadowRay.tmax = 1e30f;
                            shadowRay.t = shadowRay.tmax;
                            Interaction siShadow = this->scene.rayIntersect(shadowRay);
                            Vector3f radiance = emitterInteraction.emissiveColor;
                            // printf("%f %f\n",siShadow.t,emitterInteraction.t);
                            if (!siShadow.didIntersect || siShadow.t >= emitterInteraction.t)
                            {
                                // printf("HERE\n");
                                result3 += si.bsdf->eval(&si,  toLocalCord(onb,directionGlobal)) * emitterInteraction.emissiveColor * std::abs(Dot(si.n, shadowRay.d));
                            }
                        }
                    }
                    // printf("%f %f %f\n", result3[0], result3[1], result3[2]);
                    result3 /= mul;
                    result += result3 * 2 * M_PI; //*2;
                }

                Interaction rayemitter = this->scene.rayEmitterIntersect(cameraRay);
                result += rayemitter.emissiveColor;
                overallResult += result;
            }
            overallResult = overallResult / spp;

            this->outputImage.writePixelColor(overallResult, x, y);
        }
    }
    auto finishTime = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
}

long long Integrator::MonteCarloRender_Cosine(int spp,int mul)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int x = 0; x < this->scene.imageResolution.x; x++) // this->scene.imageResolution.x
    {
        printf("%d\n", x);
        for (int y = 0; y < this->scene.imageResolution.y; y++)
        {
            Vector3f overallResult(0, 0, 0);
            for (int z = 0; z < spp; z++)
            {
                 
                Ray cameraRay = this->scene.camera.generateRay_Anti_aliasing(x, y);
                Interaction si = this->scene.rayIntersect(cameraRay);
                Vector3f result(0, 0, 0);

                if (si.didIntersect)
                {
                    // now onto doing montecarlo
                    std::vector<Vector3f> onb = getONB(si.n);
                    Vector3f result3(0, 0, 0);
                    for (int i = 0; i < mul; i++)
                    {
                        Vector3f pointOnHemisphere = CosineWeightedSampleHemisphereCaller();
                        // pointOnHemisphere = toLocalCord(onb, pointOnHemisphere);
                        Vector3f directionGlobal = toWorldCord(onb, pointOnHemisphere);

                        if (Dot(si.n, directionGlobal) < 0)
                        {
                            printf("HIIII\n");
                        }
                        // initializing a ray
                        Ray fromPoint;
                        fromPoint.d = directionGlobal;
                        fromPoint.o = si.p;
                        fromPoint.tmax = 1e30f;
                        fromPoint.t = 1e30f;
                        // now i need to check if the intersection has been hitting the light or an object before
                        Interaction emitterInteraction = this->scene.rayEmitterIntersect(fromPoint);
                        if (emitterInteraction.didIntersect)
                        {
                            Ray shadowRay;
                            shadowRay.d = (directionGlobal);
                            shadowRay.o = si.p + 0.0001f * shadowRay.d;
                            shadowRay.tmax = 1e30f;
                            shadowRay.t = shadowRay.tmax;
                            Interaction siShadow = this->scene.rayIntersect(shadowRay);
                            Vector3f radiance = emitterInteraction.emissiveColor;
                            // printf("%f %f\n",siShadow.t,emitterInteraction.t);
                            if (!siShadow.didIntersect || siShadow.t >= emitterInteraction.t)
                            {
                                // printf("HERE\n");
                                result3 += si.bsdf->eval(&si, toLocalCord(onb,directionGlobal)) * emitterInteraction.emissiveColor; // * std::abs(Dot(si.n, shadowRay.d));
                            }
                        }
                    }
                    // printf("%f %f %f\n", result3[0], result3[1], result3[2]);
                    result3 /= mul;
                    result += result3 * M_PI; //*2;
                }

                Interaction rayemitter = this->scene.rayEmitterIntersect(cameraRay);
                result += rayemitter.emissiveColor;
                overallResult += result;
            }
            overallResult = overallResult / spp;

            this->outputImage.writePixelColor(overallResult, x, y);
        }
    }
    auto finishTime = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
}

long long Integrator::render_Anti_aliasing(int samples)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int x = 0; x < this->scene.imageResolution.x; x++)
    {
        for (int y = 0; y < this->scene.imageResolution.y; y++)
        {
            Vector3f overall_result(0, 0, 0);
            for (int z = 0; z < samples; z++)
            {
                Ray cameraRay = this->scene.camera.generateRay_Anti_aliasing(x, y);
                Interaction si = this->scene.rayIntersect(cameraRay);
                Vector3f result(0, 0, 0);
                // build the onb vector

                if (si.didIntersect)
                {
                    Vector3f radiance;
                    LightSample ls;
                    for (Light &light : this->scene.lights)
                    {
                        std::tie(radiance, ls) = light.sample(&si);

                        Ray shadowRay(si.p + 1e-3f * si.n, ls.wo);
                        Interaction siShadow = this->scene.rayIntersect(shadowRay);

                        if (!siShadow.didIntersect || siShadow.t > ls.d)
                        {
                            result += si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance * std::abs(Dot(si.n, ls.wo));
                        }
                    }
                }
                Interaction rayemitter = this->scene.rayEmitterIntersect(cameraRay);
                overall_result += (result + rayemitter.emissiveColor);
            }
            overall_result /= samples;

            this->outputImage.writePixelColor(overall_result, x, y);
        }
    }
    auto finishTime = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
}

long long Integrator::render_sampler(int spp,int mul)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    // first we sample the area light
    std::vector<Light> l;
    for (Light &light : this->scene.lights)
    {
        if (light.type == AREA_LIGHT)
        {
            l.push_back(light);
        }
    }
    int len = l.size();
    for (int x = 0; x < this->scene.imageResolution.x; x++)
    {
        auto tempTime = std::chrono::high_resolution_clock::now();
        auto total_time=tempTime-startTime;
        auto ren=std::chrono::duration_cast<std::chrono::milliseconds>(tempTime-startTime).count();
        int t=x;
        if(x==0)
        {
            t=t+1;
        }
        auto time_taken_per_row=ren/(t);
        auto expected_finish_time=time_taken_per_row*(1024-x);
        
        std::cout << x << ": " ;
        std::cout << (expected_finish_time/1000 ) << " seconds\n";
        for (int y = 0; y < this->scene.imageResolution.y; y++)
        {
            Vector3f overallResult(0, 0, 0);
            for (int z = 0; z < spp; z++)
            {
                Ray cameraRay = this->scene.camera.generateRay_Anti_aliasing(x, y);
                Interaction si = this->scene.rayIntersect(cameraRay);
                Vector3f result(0, 0, 0);

                if (si.didIntersect)
                {

                    Vector3f result3(0, 0, 0);
                    for (int i = 0; i < mul; i++)
                    {
                        Vector3f radiance;
                        LightSample ls;
                        Light &light = l[(int)(len * next_float())];
                        std::tie(radiance, ls) = light.sample(&si);
                        float h = light.vx.Length();
                        float w = light.vy.Length();
                        float A = 4* h * w;
                        if (Dot(light.normal, ls.wo) <= 0)
                        {
                            Ray shadowRay(si.p + 1e-3f * si.n, ls.wo);
                            Interaction siShadow = this->scene.rayIntersect(shadowRay);

                            if (!siShadow.didIntersect || siShadow.t > ls.d)
                            {
                                result3 += A * si.bsdf->eval(&si,ls.wo) * radiance * std::abs(Dot(si.n, ls.wo)) * std::abs(Dot(light.normal, ls.wo));
                            }
                        }
                    }
                    result3 /= mul;
                    result += (result3)*len;
                }
                Interaction rayemitter = this->scene.rayEmitterIntersect(cameraRay);
                if (rayemitter.didIntersect)
                {
                    result = rayemitter.emissiveColor;
                }
                overallResult += result;
            }
            overallResult = overallResult / spp;
            // printf("%f %f %f\n", overallResult.x, overallResult.y, overallResult.z);
            this->outputImage.writePixelColor(overallResult, x, y);
        }
    }
    auto finishTime = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
}
/*
int main(int argc, char **argv)
{
    if (argc != 5)
    {
        std::cerr << "Usage: ./render <scene_config> <out_path> <num_samples> <sampling_strategy>";
        return 1;
    }
    Scene scene(argv[1]);
    Integrator rayTracer(scene);
    int spp = atoi(argv[3]);
    int strategy = atoi(argv[4]);
    if (strategy == 0)
    {
        auto renderTime = rayTracer.MonteCarloRender_Uniform(spp);
        std::cout << "Render Time: " << std::to_string(renderTime / 1000.f) << " ms" << std::endl;
        rayTracer.outputImage.save(argv[2]);
    }
    else if (strategy == 1)
    {
        auto renderTime = rayTracer.MonteCarloRender_Cosine(spp);
        std::cout << "Render Time: " << std::to_string(renderTime / 1000.f) << " ms" << std::endl;
        rayTracer.outputImage.save(argv[2]);
    }
    else if (strategy == 2)
    {
        auto renderTime = rayTracer.render_sampler(spp);
        std::cout << "Render Time: " << std::to_string(renderTime / 1000.f) << " ms" << std::endl;
        rayTracer.outputImage.save(argv[2]);
    }
    else
    {
        auto renderTime = rayTracer.render();
        std::cout << "Render Time: " << std::to_string(renderTime / 1000.f) << " ms" << std::endl;
        rayTracer.outputImage.save(argv[2]);
    }

    return 0;
}
*/
int main(int argc, char **argv) {
    if (argc != 6) {
        std::cerr << "Usage: ./render <scene_config> <out_path> <num_samples> <mul2> <sampling_strategy> ";
        return 1;
    }

    Scene scene(argv[1]);
    Integrator rayTracer(scene);
    int spp = atoi(argv[3]);
    int strategy = atoi(argv[5]);
    int mul=atoi(argv[4]);
    auto start = std::chrono::steady_clock::now(); // Start timing

    #pragma omp parallel num_threads(omp_get_num_procs())
    {
        #pragma omp single nowait
        {
            if (strategy == 0) {
                rayTracer.MonteCarloRender_Uniform(spp,mul);
            } else if (strategy == 1) {
                rayTracer.MonteCarloRender_Cosine(spp,mul);
            } else if (strategy == 2) {
                rayTracer.render_sampler(spp,mul);
            } else {
                rayTracer.render();
            }
        }
    }

    auto end = std::chrono::steady_clock::now(); // End timing
    auto renderTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Render Time: " << renderTime  << " milliseconds" << std::endl;
    rayTracer.outputImage.save(argv[2]);

    return 0;
}