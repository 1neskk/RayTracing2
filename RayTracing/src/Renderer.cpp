#include "Renderer.h"

#include <algorithm>
#include <execution>

#include "Random.h"

void Renderer::onResize(uint32_t width, uint32_t height)
{
	if(m_image)
	{
		if (m_image->getWidth() == width && m_image->getHeight() == height)
		{
			return;
		}
		m_image->resize(width, height);
	}
	else
		m_image = std::make_shared<Image>(width, height, ImageType::RGBA);

	delete[] m_imageData;
	m_imageData = new uint32_t[width * height];

	delete[] m_accumulation;
	m_accumulation = new glm::vec4[width * height];

	m_imageWidth.resize(width);
	m_imageHeight.resize(height);
	for (uint32_t i = 0; i < width; i++)
		m_imageWidth[i] = i;
	for (uint32_t i = 0; i < height; i++)
		m_imageHeight[i] = i;
}

void Renderer::Render(const Camera& camera, const Scene& scene)
{
	m_scene = &scene;
	m_camera = &camera;

	if (m_frameIndex == 1)
		memset(m_accumulation, 0, m_image->getWidth() * m_image->getHeight() * sizeof(glm::vec4));

	if (!m_image)
	{
		return;
	}

#define MT 1
#if MT
	std::for_each(std::execution::par, m_imageHeight.begin(), m_imageHeight.end(), [this](uint32_t y)
	{
		std::for_each(std::execution::par, m_imageWidth.begin(), m_imageWidth.end(), [this, y](uint32_t x)
		{
			const glm::vec4 color = traceRay(x, y);
			m_accumulation[x + y * m_image->getWidth()] += color;
			glm::vec4 finalColor = m_accumulation[x + y * m_image->getWidth()] / static_cast<float>(m_frameIndex);
			finalColor = glm::clamp(finalColor, 0.0f, 1.0f);
			m_imageData[x + y * m_image->getWidth()] = colorUtils::vec4ToRGBA(finalColor);
		});
	});
#else
	for (uint32_t y = 0; y < m_image->getHeight(); y++)
	{
		for (uint32_t x = 0; x < m_image->getWidth(); x++)
		{
			const glm::vec4 color = traceRay(x, y);
			m_accumulation[x + y * m_image->getWidth()] += color;
			glm::vec4 finalColor = m_accumulation[x + y * m_image->getWidth()] / static_cast<float>(m_frameIndex);
			finalColor = glm::clamp(finalColor, 0.0f, 1.0f);
			m_imageData[x + y * m_image->getWidth()] = colorUtils::vec4ToRGBA(finalColor);
		}
	}
#endif
	m_image->setData(m_imageData);

	if (m_settings.accumulation)
		m_frameIndex++;
	else
		m_frameIndex = 1;
}

Renderer::HitRecord Renderer::perPixel(const Ray& ray)
{
	int closestSphere = -1;
	float closestT = std::numeric_limits<float>::max();
	const int n = m_scene->spheres.size();

	for (size_t i = 0; i < n; i++)
	{
		const Sphere& sphere = m_scene->spheres[i];
		glm::vec3 origin = ray.origin - sphere.position;

		const float a = glm::dot(ray.direction, ray.direction);
		const float b = 2.0f * glm::dot(ray.direction, origin);
		const float c = glm::dot(origin, origin) - sphere.radius * sphere.radius;
		const float discriminant = b * b - 4.0f * a * c;

		if (discriminant < 0.0f)
			continue;

		float t1 = (-b + sqrt(discriminant)) / (2.0f * a);
		float t2 = (-b - sqrt(discriminant)) / (2.0f * a);
		const float t = std::min(t1, t2);

		if (t > 0.0f && t < closestT)
		{
			closestT = t;
			closestSphere = static_cast<int>(i);
		}
	}

	if (closestSphere < 0)
		return rayMiss(ray);

	return rayHit(ray, closestT, closestSphere);
}

glm::vec4 Renderer::traceRay(uint32_t x, uint32_t y)
{
	Ray ray;
	ray.origin = m_camera->getPosition();
	ray.direction = m_camera->getRayDirection()[x + y * m_image->getWidth()];

	glm::vec3 color(0.0f);
	float m = 1.0f;
	int b = 5; // bounces per pixel

	for (int i = 0; i < b; i++)
	{
		HitRecord ht = perPixel(ray);
		if (ht.t < 0.0f)
		{
			//glm::vec3 bgColor = (1.0f - 0.5f) * glm::vec3(1.0f, 1.0f, 1.0f) + 0.5f * glm::vec3(0.5f, 0.7f, 1.0f);
			glm::vec3 bgColor(0.6f, 0.7f, 0.9f);
			color += bgColor * m;
			break;
		}

		glm::vec3 lightDirection = glm::normalize(glm::vec3(-1, -1, -1));
		float light = glm::dot(-lightDirection, ht.normal);
		if (light < 0.0f)
			light = 0.0f;

		const Sphere& sphere = m_scene->spheres[ht.objID];
		//const Cube& cube = m_scene->cubes[ht.objID];
		const Material& material = m_scene->materials[sphere.id];

		glm::vec3 sphereColor = material.albedo * light;
		color += sphereColor * m;

		m *= 0.5f;

		ray.origin = ht.worldPos + ht.normal * 0.0001f;
		ray.direction = glm::reflect(ray.direction, ht.normal + material.roughness * Random::Random::Vec3(-0.5f, 0.5f));
	}
	return { color , 1.0f };
}

Renderer::HitRecord Renderer::rayMiss(const Ray& ray)
{
	HitRecord ht;
	ht.t = -1.0f;
	return ht;
}

Renderer::HitRecord Renderer::rayHit(const Ray& ray, float closestT, int index) const
{
	HitRecord ht;
	ht.t = closestT;
	ht.objID = index;

	const glm::vec3 origin = ray.origin - m_scene->spheres[index].position;
	ht.worldPos = origin + ray.direction * closestT;
	ht.normal = glm::normalize(ht.worldPos);

	ht.worldPos += m_scene->spheres[index].position;

	return ht;
}

Renderer::HitRecord Renderer::cubePerPixel(const Ray& ray)
{
	int closestCube = -1;
	float tfar = std::numeric_limits<float>::max();

	size_t n = m_scene->cubes.size();

	// R(t) = ray.origin + (cube.position - ray.origin) * t
	for (size_t i = 0; i < n; i++)
	{
		const Cube& cube = m_scene->cubes[i];
		glm::vec3 origin = ray.origin - cube.position;

		float t1 = (cube.size - origin.x) / ray.direction.x;
		float t2 = (-cube.size - origin.x) / ray.direction.x;

		float t3 = (cube.size - origin.y) / ray.direction.y;
		float t4 = (-cube.size - origin.y) / ray.direction.y;

		float t5 = (cube.size - origin.z) / ray.direction.z;
		float t6 = (-cube.size - origin.z) / ray.direction.z;

		float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
		float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

		if (tmax < 0.0f)
			continue;

		if (tmin > tmax)
			continue;

		if (tmin < 0.0f)
			tmin = tmax;

		if (tmin < tfar)
		{
			tfar = tmin;
			closestCube = static_cast<int>(i);
		}
	}
	if (closestCube < 0)
		return rayMiss(ray);

	return rayHit(ray, tfar, closestCube);
}
