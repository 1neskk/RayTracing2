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
	float radius = 0.0f;
	int id = 0;
};

struct Cube
{
	glm::vec3 position{ 0.0f };
	float size = 0.0f;
	int id = 0;
};

struct Material
{
	glm::vec3 albedo{ 1.0f };
	float diffuse{ 0.0f };
	float specular{ 0.0f };

	glm::vec3 emissionColor{ 0.0f };
	float emissionIntensity = 0.0f;

	int id = 0;

	glm::vec3 getEmission() const { return emissionColor * emissionIntensity; }
};

struct Settings
{
	bool accumulation = true;
	bool skyLight = false;
};

struct Scene
{
	std::vector<Sphere> spheres;
	std::vector<Cube> cubes;
	std::vector<Material> materials;

	// overload -> operator to access light and sphere data
	//Scene operator->() { return *this; }
};

