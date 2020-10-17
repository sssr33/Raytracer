// MassiveCompute.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Image.h"
#include "BaseFunctor.h"
#include "Schedulers/EqualBlockScheduler.h"
#include "Schedulers/ConstantBlockScheduler.h"
#include "Schedulers/ConstantBlockSchedulerWithThreads.h"
#include "Schedulers/StealingBlockScheduler.h"

#include <iostream>
#include <cstdint>
#include <vector>
#include <functional>
#include <thread>
#include <future>
#include <atomic>
#include <mutex>
#include <cmath>
#include <cassert>
#include <optional>
#include <algorithm>
#include <Windows.h>

class ImageWithData : public Image
{
public:
    ImageWithData() = default;
    ImageWithData(size_t width, size_t height)
        : Image(width, height)
        , data(this->GetWidth() * this->GetHeight())
    {}

    bool operator==(const ImageWithData& other) const
    {
        bool equ = Image::operator==(other)
            && this->data == other.data;

        return equ;
    }

    bool operator!=(const ImageWithData& other) const
    {
        return !this->operator==(other);
    }

    const float* GetData() const
    {
        return this->data.data();
    }

    float* GetData()
    {
        return this->data.data();
    }

private:
    std::vector<float> data;
};

struct Functor
{
    Functor(ImageWithData& img)
        : img(img)
    {}

    void operator()(const Block& block)
    {
        for (size_t y = block.top; y < block.bottom; y++)
        {
            float* dst = img.get().GetData() + y * block.imageWidth;

            for (size_t x = block.left; x < block.right; x++)
            {
                //for (int i = 0; i < 1000; i++)
                {
                    float u = static_cast<float>(x) / static_cast<float>(block.imageWidth);
                    float v = static_cast<float>(y) / static_cast<float>(block.imageHeight);

                    float val = u * v;// std::cosf(u) + std::sinf(v) + std::tanf(u);

                    dst[x] += val;
                }
            }
        }
    }

private:
    std::reference_wrapper<ImageWithData> img;
};

struct FunctorFactory
{
    BaseFunctor operator()(ImageWithData& img)
    {
        return Functor(img);
    }
};

ImageWithData GetExpected(size_t width, size_t height, std::function<BaseFunctor(ImageWithData&)> functorFactory);

int MassiveComputeTest()
{
    const size_t ImgHeight = 1080;
    const size_t ImgWidth = (ImgHeight * 16) / 9;

    size_t constantWidth = ImgWidth;
    size_t constantHeight = 1;

    ImageWithData expectedImg = GetExpected(ImgWidth, ImgHeight, FunctorFactory());

    EqualBlockScheduler equalScheduler;
    ConstantBlockScheduler constanceScheduler;
    ConstantBlockSchedulerWithThreads constanceSchedulerWithThreads;
    StealingBlockScheduler stealingScheduler;

    while (true)
    {
        ImageWithData equalImg(ImgWidth, ImgHeight);
        ImageWithData constantImg(ImgWidth, ImgHeight);
        ImageWithData constantWThreadsImg(ImgWidth, ImgHeight);
        ImageWithData stealingImg(ImgWidth, ImgHeight);

        equalScheduler(equalImg, Functor(equalImg));
        constanceScheduler(constantImg, Functor(constantImg), constantWidth, constantHeight);
        constanceSchedulerWithThreads(constantWThreadsImg, Functor(constantWThreadsImg), constantWidth, constantHeight);
        stealingScheduler(stealingImg, Functor(stealingImg), constantWidth, constantHeight);

        bool equalSame = equalImg == expectedImg;
        bool constantSame = constantImg == expectedImg;
        bool constantWThreadsSame = constantWThreadsImg == expectedImg;
        bool stealingSame = stealingImg == expectedImg;

        assert(equalSame);
        assert(constantSame);
        assert(constantWThreadsSame);
        assert(stealingSame);

        int stop = 324;
    }
}

ImageWithData GetExpected(size_t width, size_t height, std::function<BaseFunctor(ImageWithData&)> functorFactory)
{
    if (!width || !height)
    {
        return ImageWithData();
    }

    ImageWithData img = ImageWithData(width, height);

    functorFactory(img)(*img.GetBlock(0, 0, width, height));

    return img;
}