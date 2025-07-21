#include "light.h"

void printLights(std::vector<Light> &lights)
{
    for( auto i : lights)
    {
        std::cout<< i.type<< "\n";
        if(i.type==DIRECTIONAL_LIGHT)
        {
            std::cout<< i.direction.direction[0] << " " << i.direction.direction[1] << " " << i.direction.direction[2] << " " << "\n";
            std::cout<< i.direction.radiance[0] << " " << i.direction.radiance[1] << " " << i.direction.radiance[2] << " " << "\n";
        }
        else
        {
            std::cout<< i.point.location[0] << " " << i.point.location[1] << " " << i.point.location[2] << " " << "\n";
            std::cout<< i.point.radiance[0] << " " << i.point.radiance[1] << " " << i.point.radiance[2] << " " << "\n";
        }
    }
}

std::vector<Light> getLights(nlohmann::json sceneConfig)
{
    std::vector<Light> lights;
    //to get the directional Light first
    try
    {
        auto dir=sceneConfig["directionalLights"];
        for( auto i : dir)
        {
            Light temp;
            temp.type=DIRECTIONAL_LIGHT;
            auto direction=i["direction"];
            auto radiance=i["radiance"];
            temp.direction.direction=Vector3f(direction[0],direction[1],direction[2]);
            temp.direction.radiance=Vector3f(radiance[0],radiance[1],radiance[2]);
            lights.push_back(temp);
            
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    try
    {
        auto dir=sceneConfig["pointLights"];
        for( auto i : dir)
        {
            Light temp;
            temp.type=POINT_LIGHT;
            auto location=i["location"];
            auto radiance=i["radiance"];
            temp.point.location=Vector3f(location[0],location[1],location[2]);
            temp.point.radiance=Vector3f(radiance[0],radiance[1],radiance[2]);
            lights.push_back(temp);
            
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return lights;
}