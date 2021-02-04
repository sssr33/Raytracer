#include "RayTraceWindowHandler.h"
#include "Image/BGRA.h"
#include "Image/ImageView.h"
#include "Render/Functor/TestGradientFunctor.h"
#include "Render/Functor/CopyImageFunctor.h"
#include "Render/Functor/RayTraceFunctor.h"
#include "Render/PerlinNoise/PerlinNoiseTextureSampler.h"
#include "Render/Sampler/TextureRaySampler.h"
#include "Render/Sampler/TextureSamplerWithOffset.h"
#include "Render/Random/TextureRaySamplerRandomInUnitSphere.h"
#include "Render/Camera/Camera.h"
#include "Render/Hitable/HitableList.h"
#include "Render/Hitable/Sphere.h"
#include "Render/Hitable/Triangle.h"
#include "Render/Material/Lambertian.h"
#include "Render/Material/Metal.h"
#include "Render/Material/Dielectric.h"
#include "Render/Scenes/Book1Scene.h"

#include <Helpers/is_ready.h>
#include <MassiveCompute/Schedulers/StealingBlockScheduler.h>
#include <MassiveCompute/Schedulers/ConstantBlockScheduler.h>
#include <MassiveCompute/Schedulers/EqualBlockScheduler.h>

RayTraceWindowHandler::RayTraceWindowHandler()
	: random(std::random_device()())
	, perlinNoise(std::make_shared<PerlinNoiseTextureSampler>(4096))
{
	// 1 image for raytracing task
	this->renderQueue.emplace();
}

RayTraceWindowHandler::~RayTraceWindowHandler()
{
	if (this->rayTraceTask.valid())
	{
		this->rayTraceTaskCancel = true;
		this->rayTraceTask.wait();
	}
}

void RayTraceWindowHandler::GameLoop(ISystemBackBuffer& backBuffer)
{
	this->OnRepaint(backBuffer);
}

void RayTraceWindowHandler::OnResize(const Helpers::Size2D<uint32_t>& newSize)
{
	this->currentSize = newSize;
}

void RayTraceWindowHandler::OnRepaint(ISystemBackBuffer& backBuffer)
{
	this->TryStartRayTraceTask();
	this->TryFinishRayTraceTask();

	SystemBackBufferLock bufLock(backBuffer);
	ISystemBackBuffer::LockedData& data = bufLock.GetData();

	if (!data.data)
	{
		return;
	}

	BGRA<uint8_t>* pixels = reinterpret_cast<BGRA<uint8_t>*>(data.data);
	ImageView<BGRA<uint8_t>> imageView(data.size.width, data.size.height, pixels);

	const size_t width = std::min(this->currentlyPresentingImage.GetWidth(), imageView.GetWidth());
	const size_t height = std::min(this->currentlyPresentingImage.GetHeight(), imageView.GetHeight());

	MassiveCompute::StealingBlockScheduler stealingScheduler;

	stealingScheduler(imageView, MakeCopyImageFunctor(imageView, this->currentlyPresentingImage.MakeView(), width, height), imageView.GetWidth(), 1);
}

void RayTraceWindowHandler::OnMouseLeftPress(const Helpers::Point2D<float>& pt)
{

}

void RayTraceWindowHandler::OnMouseLeftRelease(const Helpers::Point2D<float>& pt)
{
	cameraX -= 0.01f;
}

void RayTraceWindowHandler::OnMouseRightPress(const Helpers::Point2D<float>& pt)
{

}

void RayTraceWindowHandler::OnMouseRightRelease(const Helpers::Point2D<float>& pt)
{
	cameraX += 0.01f;
}

void RayTraceWindowHandler::OnMouseMiddlePress(const Helpers::Point2D<float>& pt)
{

}

void RayTraceWindowHandler::OnMouseMiddleRelease(const Helpers::Point2D<float>& pt)
{

}

void RayTraceWindowHandler::OnMouseMove(const Helpers::Point2D<float>& pt)
{

}

void RayTraceWindowHandler::OnMouseWheel(float delta)
{

}

void RayTraceWindowHandler::TryStartRayTraceTask()
{
	if (this->rayTraceTask.valid() || this->renderQueue.empty() || this->currentSize.Empty())
	{
		return;
	}

	Image<BGRA<uint8_t>> image = std::move(this->renderQueue.front());
	this->renderQueue.pop();

	if (image.GetWidth() != this->currentSize.width || image.GetHeight() != this->currentSize.height)
	{
		image = Image<BGRA<uint8_t>>::Resize(std::move(image), this->currentSize.width, this->currentSize.height);
	}

	RayTraceFunctorParams rayTraceParams = this->MakeDefaultScene();
	//RayTraceFunctorParams rayTraceParams = this->MakeBook1Scene();

	this->rayTraceTask = std::async(
		std::launch::async,
		&RayTraceWindowHandler::RayTraceMain,
		std::move(rayTraceParams),
		std::move(image),
		std::ref(this->rayTraceTaskCancel)
	);
}

void RayTraceWindowHandler::TryFinishRayTraceTask()
{
	if (!is_ready(this->rayTraceTask))
	{
		return;
	}

	this->renderQueue.push(std::move(this->currentlyPresentingImage));
	this->currentlyPresentingImage = this->rayTraceTask.get();
}

float RayTraceWindowHandler::GetRandomFloat()
{
	constexpr uint32_t RandomResolution = 10000;
	float rnd = static_cast<float>(this->random() % RandomResolution) / static_cast<float>(RandomResolution);

	return rnd;
}

RayTraceFunctorParams RayTraceWindowHandler::MakeDefaultScene()
{
	auto randomInUnitSphere =
		std::make_shared<TextureRaySamplerRandomInUnitSphere>(
			std::make_shared<TextureRaySampler<float>>(
				std::make_shared<TextureSamplerWithOffset<float>>(
					this->perlinNoise, vec2<float>(this->GetRandomFloat(), this->GetRandomFloat())
					)
				)
			);
	const vec2<float> imageSize(static_cast<float>(this->currentSize.width), static_cast<float>(this->currentSize.height));

	CameraFovSettings camSettings;

	/*camSettings.lookFrom = { -2.f + this->cameraX, 2.f, 1.f };
	camSettings.lookAt = { 0.f, 0.f, -1.f };
	camSettings.fov = 45.f;*/
	/*camSettings.lookFrom = { 3.f, 3.f, 2.f };
	camSettings.lookAt = { 0.f, 0.f, -1.f };
	camSettings.fov = 20.f;
	camSettings.aperture = 0.5f;*/
	camSettings.aspectRatio = imageSize.x / imageSize.y;
	camSettings.focusDist = (camSettings.lookFrom - camSettings.lookAt).length();

	std::shared_ptr<Camera> camera = std::make_shared<Camera>(camSettings);

	std::shared_ptr<HitableList> hitableList = std::make_shared<HitableList>();

	hitableList->objects.emplace_back(std::make_unique<Sphere>(
	    vec3<float>{0.f, 0.f, -1.f},
	    0.5f,
	    std::make_unique<Lambertian>(vec3<float>(0.8f, 0.3f, 0.3f), randomInUnitSphere)
	    )
	);

	hitableList->objects.emplace_back(std::make_unique<Sphere>(
	    vec3<float>{0.f, -100.5f, -1.f},
	    100.f,
	    std::make_unique<Lambertian>(vec3<float>(0.8f, 0.8f, 0.8f), randomInUnitSphere)
	    )
	);

	hitableList->objects.emplace_back(std::make_unique<Sphere>(
	    vec3<float>{1.f, 0.f, -1.f},
	    0.5f,
	    std::make_unique<Metal>(vec3<float>(0.8f, 0.6f, 0.2f), 0.025f, randomInUnitSphere)
	    )
	);

	hitableList->objects.emplace_back(std::make_unique<Sphere>(
	    vec3<float>{-1.f, 0.f, -1.f},
	    0.5f,
	    std::make_unique<Dielectric>(1.5f, randomInUnitSphere)
	    )
	);

	vec3<float> center = { 0.f, 0.f, -1.f };
	float width = 2.f;
	float height = 2.f;

	float emission = 2.f;

	hitableList->objects.emplace_back(
	    std::make_unique<Triangle>(
	        center + vec3<float>(-width * 0.5f, -height * 0.5f, 0.f),
	        center + vec3<float>(width * 0.1f, height * 0.5f, 0.f),
	        center + vec3<float>(-width * 0.5f, height * 0.5f, 0.f),
	        vec2<float>(0.f, 1.f),
	        vec2<float>(1.f, 0.f),
	        vec2<float>(0.f, 0.f),
	        std::make_unique<Lambertian>(vec3<float>(0.0f, emission * (38.f / 255.f), emission * 1.0f), randomInUnitSphere)
	        )
	);

	hitableList->objects.emplace_back(
	    std::make_unique<Triangle>(
	        center + vec3<float>(-width * 0.5f, -height * 0.5f, 0.f),
	        center + vec3<float>(width * 0.5f, -height * 0.5f, 0.f),
	        center + vec3<float>(width * 0.5f, height * 0.5f, 0.f),
	        vec2<float>(0.f, 1.f),
	        vec2<float>(1.f, 1.f),
	        vec2<float>(1.f, 0.f),
	        std::make_unique<Lambertian>(vec3<float>(emission * 1.0f, emission * (30.f / 255.f), 0.0f), randomInUnitSphere)
	        )
	);

	hitableList->objects.emplace_back(
	    std::make_unique<Triangle>(
	        vec3<float>(-width, -height, 1.f),
	        vec3<float>(width, -height, 1.f),
	        vec3<float>(0.f, height, 1.f),
	        vec2<float>(0.f, 1.f),
	        vec2<float>(1.f, 1.f),
	        vec2<float>(1.f, 0.f),
	        std::make_unique<Metal>(vec3<float>(emission * (100.f / 255.f), emission * 1.0f, 0.f), 0.f, randomInUnitSphere)
	        )
	);

	RayTraceFunctorParams params;

	params.camera = std::move(camera);
	params.scene = std::move(hitableList);

	return params;
}

RayTraceFunctorParams RayTraceWindowHandler::MakeBook1Scene()
{
	auto randomInUnitSphere =
		std::make_shared<TextureRaySamplerRandomInUnitSphere>(
			std::make_shared<TextureRaySampler<float>>(
				std::make_shared<TextureSamplerWithOffset<float>>(
					this->perlinNoise, vec2<float>(this->GetRandomFloat(), this->GetRandomFloat())
					)
				)
			);

	if (!this->book1Scene)
	{
		// scene is generated randomly so create it 1 time to have constant object position between frames
		this->book1Scene = Book1Scene()(randomInUnitSphere);
	}

	const vec2<float> imageSize(static_cast<float>(this->currentSize.width), static_cast<float>(this->currentSize.height));

	CameraFovSettings camSettings;

	camSettings.lookFrom = { 17.f, 2.5f, 4.f };
	camSettings.lookAt = { 0.f, 1.f, 0.f };
	camSettings.fov = 20.f;
	camSettings.aspectRatio = imageSize.x / imageSize.y;
	camSettings.aperture = 0.3f;
	camSettings.focusDist = (camSettings.lookFrom - camSettings.lookAt).length();

	std::shared_ptr<Camera> camera = std::make_shared<Camera>(camSettings);

	RayTraceFunctorParams params;

	params.camera = std::move(camera);
	params.scene = this->book1Scene;

	return params;
}

Image<BGRA<uint8_t>> RayTraceWindowHandler::RayTraceMain(
	RayTraceFunctorParams rayTraceParams,
	Image<BGRA<uint8_t>> resultImage,
	std::atomic<bool>& /*cancel*/
)
{
	ImageView<BGRA<uint8_t>> imageView(resultImage.GetWidth(), resultImage.GetHeight(), resultImage.GetData());
	MassiveCompute::StealingBlockScheduler stealingScheduler;

	//MassiveCompute::ConstantBlockScheduler cbs;

	//MassiveCompute::EqualBlockScheduler ebs;

	//ebs(imageView, RayTraceFunctor(imageView, std::move(rayTraceParams)));

	//cbs(imageView, RayTraceFunctor(imageView, std::move(rayTraceParams)), imageView.GetWidth(), 1);

	stealingScheduler(imageView, RayTraceFunctor(imageView, std::move(rayTraceParams)), imageView.GetWidth(), 1);

	// single thread, for test
	/*MassiveCompute::Block block;
	block.left = 0;
	block.top = 0;
	block.right = block.imageWidth = imageView.GetWidth();
	block.bottom = block.imageHeight = imageView.GetHeight();
	(TestGradientFunctor(imageView))(block);*/

	return resultImage;
}
