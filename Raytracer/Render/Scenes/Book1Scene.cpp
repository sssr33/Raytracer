#include "Book1Scene.h"
#include "Render/Hitable/HitableList.h"
#include "Render/Hitable/Sphere.h"
#include "Render/Hitable/Triangle.h"
#include "Render/Material/Lambertian.h"
#include "Render/Material/Metal.h"
#include "Render/Material/Dielectric.h"

std::unique_ptr<IHitable> Book1Scene::operator()(const std::shared_ptr<IRandomInUnitSphere>& randomInUnitSphere)
{
	auto Rand = []()
	{
		return std::fmod(static_cast<float>(rand() % 10000) / 10000.f, 1.f);
	};

	std::unique_ptr<HitableList> list = std::make_unique<HitableList>();

	list->objects.emplace_back(std::make_unique<Sphere>(
		vec3<float>{0.f, -1000.f, 0.f},
		1000.f,
		std::make_unique<Lambertian>(vec3<float>(0.5f, 0.5f, 0.5f), randomInUnitSphere)
		)
	);

	constexpr float GridSize = 0.f;

	for (float a = -GridSize; a < GridSize; a++)
	{
		for (float b = -GridSize; b < GridSize; b++)
		{
			float chooseMat = Rand();
			vec3<float> center(a + 0.9f * Rand(), 0.2f, b + 0.9f * Rand());

			if ((center - vec3<float>(4.f, 0.2f, 0.f)).length() > 0.9f)
			{
				std::unique_ptr<IMaterial> material;

				if (chooseMat < 0.8f)
				{
					vec3<float> color(Rand() * Rand(), Rand() * Rand(), Rand() * Rand());
					material = std::make_unique<Lambertian>(color, randomInUnitSphere);
				}
				else if (chooseMat < 0.95f)
				{
					vec3<float> color(0.5f * (1.f + Rand()), 0.5f * (1.f + Rand()), 0.5f * (1.f + Rand()));
					material = std::make_unique<Metal>(color, 0.5f * Rand(), randomInUnitSphere);
				}
				else
				{
					material = std::make_unique<Dielectric>(1.5f, randomInUnitSphere);
				}

				std::unique_ptr<Sphere> sphere = std::make_unique<Sphere>(center, 0.2f, std::move(material));
				list->objects.push_back(std::move(sphere));
			}
		}
	}

	list->objects.emplace_back(std::make_unique<Sphere>(
		vec3<float>{0.f, 1.f, 0.f},
		1.f,
		std::make_unique<Dielectric>(1.5f, randomInUnitSphere)
		)
	);

	list->objects.emplace_back(std::make_unique<Sphere>(
		vec3<float>{-4.f, 1.f, 0.f},
		1.f,
		std::make_unique<Lambertian>(vec3<float>(0.4f, 0.2f, 0.1f), randomInUnitSphere)
		)
	);

	list->objects.emplace_back(std::make_unique<Sphere>(
		vec3<float>{4.f, 1.f, 0.f},
		1.f,
		std::make_unique<Metal>(vec3<float>(0.7f, 0.6f, 0.5f), 0.f, randomInUnitSphere)
		)
	);

	return list;
}