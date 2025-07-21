#include "light.h"

Light::Light(LightType type, nlohmann::json config)
{
    switch (type)
    {
    case LightType::POINT_LIGHT:
        this->position = Vector3f(config["location"][0], config["location"][1], config["location"][2]);
        break;
    case LightType::DIRECTIONAL_LIGHT:
        this->direction = Vector3f(config["direction"][0], config["direction"][1], config["direction"][2]);
        break;
    case LightType::AREA_LIGHT:
        // TODO: is done
        this->center = Vector3f(config["center"][0], config["center"][1], config["center"][2]);
        this->vx = Vector3f(config["vx"][0], config["vx"][1], config["vx"][2]);
        this->vy = Vector3f(config["vy"][0], config["vy"][1], config["vy"][2]);
        this->normal = Vector3f(config["normal"][0], config["normal"][1], config["normal"][2]);
        break;
    default:
        std::cout << "WARNING: Invalid light type detected";
        break;
    }

    this->radiance = Vector3f(config["radiance"][0], config["radiance"][1], config["radiance"][2]);
    this->type = type;
}

std::pair<Vector3f, LightSample> Light::sample(Interaction *si)
{
    LightSample ls;
    memset(&ls, 0, sizeof(ls));

    Vector3f radiance;
    switch (type)
    {
    case LightType::POINT_LIGHT:
        ls.wo = (position - si->p);
        ls.d = ls.wo.Length();
        ls.wo = Normalize(ls.wo);
        radiance = (1.f / (ls.d * ls.d)) * this->radiance;
        break;
    case LightType::DIRECTIONAL_LIGHT:
        ls.wo = Normalize(direction);
        ls.d = 1e10;
        radiance = this->radiance;
        break;
    case LightType::AREA_LIGHT:
        // TODO: Implement this
        // first i unifromly sample a light source amongst the ones i have(pre this)
        // then i uniformly sample a point on that light source
        // then i use it as a point light source
        // lets go
        float u = 2 * (next_float() - 0.5);
        float v = 2 * (next_float() - 0.5);
        Vector3f pos = center + u * vx + v * vy;
        ls.wo = (pos - si->p);
        ls.d = ls.wo.Length();
        ls.wo = Normalize(ls.wo);
        radiance = (1.f / (ls.d * ls.d)) * this->radiance;
        break;
    }
    return {radiance, ls};
}

Interaction rayPlaneIntersect(Ray ray, Vector3f p, Vector3f n)
{
    Interaction si;

    float dDotN = Dot(ray.d, n);
    if (dDotN != 0.f)
    {
        float t = -Dot((ray.o - p), n) / dDotN;

        if (t >= 0.f)
        {
            si.didIntersect = true;
            si.t = t;
            si.n = n;
            si.p = ray.o + ray.d * si.t;
        }
    }

    return si;
}

float magnitude(Vector3f v)
{
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

Interaction Light::intersectLight(Ray *ray)
{
    Interaction sid;
    memset(&sid, 0, sizeof(sid));
    sid.didIntersect = false;
    sid.emissiveColor = Vector3f(0, 0, 0);

    if (type == LightType::AREA_LIGHT)
    {
        // TODO: Implement this
        Interaction si = rayPlaneIntersect(*ray, center, normal);
        if (si.didIntersect)
        {
            Vector3f poi = si.p;

            Vector3f intersectionToCenter = poi - center;
            float projectionX = Dot(intersectionToCenter, vx);
            float projectionY = Dot(intersectionToCenter, vy);

            float width = vx.LengthSquared();
            float height = vy.LengthSquared();

            if ((-width <= projectionX && projectionX <= width) && (-height <= projectionY && projectionY <= height))
            {
                float checkangle = Dot(ray->d, normal);
                if (checkangle <= 0)
                {
                    sid.didIntersect = true;
                    sid.emissiveColor = radiance;
                    sid.t = si.t;
                    sid.p = si.p;
                    sid.n = si.n;
                }
            }
        }
    }

    return sid;
}