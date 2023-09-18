#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <glm/glm.hpp>

#include "Application.h"

#include "Image.h"
#include "Camera.h"
#include "Scene.h"

class Renderer
{
	struct HitRecord
	{
		float t;
		glm::vec3 normal;
		glm::vec3 worldPos;
		Sphere sphere;
		int objID;
	};
public:
	Renderer() = default;

	void onResize(uint32_t width, uint32_t height);
	void Render(const Camera& camera, const Scene& scene);

	std::shared_ptr<Image> getImage() const { return m_image; }
	Settings& getSettings() { return m_settings; }

	void resetFrameIndex() { m_frameIndex = 1; }
private:
	HitRecord perPixel(const Ray& ray);
	HitRecord cubePerPixel(const Ray& ray);
	static HitRecord rayMiss(const Ray& ray);
	HitRecord rayHit(const Ray& ray, float closestT, int index) const;
	glm::vec4 traceRay(uint32_t x, uint32_t y);
private:
	const Scene* m_scene = nullptr;
	const Camera* m_camera = nullptr;

	Settings m_settings;

	glm::vec4* m_accumulation = nullptr;
	uint32_t m_frameIndex = 1;

	std::shared_ptr<Image> m_image;
	uint32_t* m_imageData = nullptr;

	std::vector<uint32_t> m_imageWidth, m_imageHeight;
};

namespace colorUtils
{
	inline uint32_t vec4ToRGBA(const glm::vec4& v)
	{
		const auto r = static_cast<uint8_t>(v.r * 255.0f);
		const auto g = static_cast<uint8_t>(v.g * 255.0f);
		const auto b = static_cast<uint8_t>(v.b * 255.0f);
		const auto a = static_cast<uint8_t>(v.a * 255.0f);

		return (a << 24) | (b << 16) | (g << 8) | r;
	}

	inline uint32_t vec3ToRGBA(const glm::vec3& v)
	{
		const auto r = static_cast<uint8_t>(v.r * 255.0f);
		const auto g = static_cast<uint8_t>(v.g * 255.0f);
		const auto b = static_cast<uint8_t>(v.b * 255.0f);

		return (255 << 24) | (b << 16) | (g << 8) | r;
	}
}
