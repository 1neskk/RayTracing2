#pragma once
#include <cstdint>
#include <memory>

#include "Image.h"
#include "Camera.h"

struct Ray
{
	glm::vec3 origin{ 0.0f };
	glm::vec3 direction{ 0.0f };
};

struct Sphere
{
	glm::vec3 position{ 0.0f };
	float radius;
	int id;
};

struct Cube
{
	glm::vec3 position{ 0.0f };
	float size;
	int id;
};

struct Material
{
	glm::vec3 albedo{ 1.0f };
	float roughness;
    float metallic;
	float reflectivity;
	int id;
};

struct Settings
{
	bool accumulation = true;
};

struct Scene
{
	std::vector<Sphere> spheres;
	std::vector<Cube> cubes;
	std::vector<Material> materials;

	// overload -> operator to access light and sphere data
	//Scene operator->() { return *this; }
};

