#pragma once

#include "common.h"
#

enum LightType
{
	POINT_LIGHT = 0,
	DIRECTIONAL_LIGHT = 1,
	NUM_LIGHT_TYPES
};

struct Light
{
	LightType type;
	union //this based on light type helps us decide the struct to use	
	{
		// Directional light
		struct
		{
			Vector3f direction;
			Vector3f radiance;
		} direction;

		// Point light
		struct
		{
			Vector3f location;
			Vector3f radiance;
		} point;
	};
	Light() {};
};

std::vector<Light> getLights(nlohmann::json sceneConfig);
void printLights(std::vector<Light> &lights);