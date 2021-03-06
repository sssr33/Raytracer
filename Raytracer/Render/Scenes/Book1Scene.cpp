#include "Book1Scene.h"
#include "Render/Hitable/HitableList.h"
#include "Render/Hitable/Sphere.h"
#include "Render/Hitable/MovingSphere.h"
#include "Render/Hitable/Triangle.h"
#include "Render/Material/Lambertian.h"
#include "Render/Material/Metal.h"
#include "Render/Material/Dielectric.h"
#include "Render/Random/RandomFunctions.h"

std::unique_ptr<IHitable> Book1Scene::operator()()
{
	std::unique_ptr<HitableList> list = std::make_unique<HitableList>();

	list->objects.emplace_back(std::make_unique<Sphere>(
		vec3<float>{0.f, -1000.f, 0.f},
		1000.f,
		std::make_unique<Lambertian>(vec3<float>(0.5f, 0.5f, 0.5f))
		)
	);

	constexpr float GridSize = 11.f;

	for (float a = -GridSize; a < GridSize; a++)
	{
		for (float b = -GridSize; b < GridSize; b++)
		{
			float chooseMat = RandomFloat();
			vec3<float> center(a + 0.9f * RandomFloat(), 0.2f, b + 0.9f * RandomFloat());

			if ((center - vec3<float>(4.f, 0.2f, 0.f)).length() > 0.9f)
			{
				std::unique_ptr<IHitable> hitable;

				if (chooseMat < 0.8f)
				{
					vec3<float> color(RandomFloat() * RandomFloat(), RandomFloat() * RandomFloat(), RandomFloat() * RandomFloat());
					auto material = std::make_unique<Lambertian>(color);

					hitable = std::make_unique<MovingSphere>(center, center + vec3<float>(0.f, 0.5f * RandomFloat(), 0.f), 0.f, 1.f, 0.2f, std::move(material));
				}
				else if (chooseMat < 0.95f)
				{
					vec3<float> color(0.5f * (1.f + RandomFloat()), 0.5f * (1.f + RandomFloat()), 0.5f * (1.f + RandomFloat()));
					auto material = std::make_unique<Metal>(color, 0.5f * RandomFloat());

					hitable = std::make_unique<Sphere>(center, 0.2f, std::move(material));
				}
				else
				{
					auto material = std::make_unique<Dielectric>(1.5f);

					hitable = std::make_unique<Sphere>(center, 0.2f, std::move(material));
				}

				list->objects.push_back(std::move(hitable));
			}
		}
	}

	list->objects.emplace_back(std::make_unique<Sphere>(
		vec3<float>{0.f, 1.f, 0.f},
		1.f,
		std::make_unique<Dielectric>(1.5f)
		)
	);

	list->objects.emplace_back(std::make_unique<Sphere>(
		vec3<float>{-4.f, 1.f, 0.f},
		1.f,
		std::make_unique<Lambertian>(vec3<float>(0.4f, 0.2f, 0.1f))
		)
	);

	list->objects.emplace_back(std::make_unique<Sphere>(
		vec3<float>{4.f, 1.f, 0.f},
		1.f,
		std::make_unique<Metal>(vec3<float>(0.7f, 0.6f, 0.5f), 0.f)
		)
	);

	return list;
}
