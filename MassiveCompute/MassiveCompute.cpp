// MassiveCompute.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Image.h"
#include "BaseFunctor.h"
#include "Schedulers/EqualBlockScheduler.h"
#include "Schedulers/ConstantBlockScheduler.h"
#include "Schedulers/ConstantBlockSchedulerWithThreads.h"
#include "Helpers.h"

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

struct Functor
{
    void operator()(const Block& block)
    {
        for (size_t y = block.top; y < block.bottom; y++)
        {
            float* dst = block.image + y * block.imageWidth;

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
};

Image GetExpected(size_t width, size_t height, BaseFunctor functor);

int main()
{
    const size_t ImgHeight = 1080;
    const size_t ImgWidth = (ImgHeight * 16) / 9;

    size_t constantWidth = ImgWidth;
    size_t constantHeight = 1;

    Image expectedImg = GetExpected(ImgWidth, ImgHeight, Functor());

    EqualBlockScheduler equalScheduler;
    ConstantBlockScheduler constanceScheduler;
    ConstantBlockSchedulerWithThreads constanceSchedulerWithThreads;

    while (true)
    {
        Image equalImg(ImgWidth, ImgHeight);
        Image constantImg(ImgWidth, ImgHeight);
        Image constantWThreadsImg(ImgWidth, ImgHeight);

        equalScheduler(equalImg, Functor());
        constanceScheduler(constantImg, Functor(), constantWidth, constantHeight);
        constanceSchedulerWithThreads(constantWThreadsImg, Functor(), constantWidth, constantHeight);

        bool equalSame = equalImg == expectedImg;
        bool constantSame = constantImg == expectedImg;
        bool constantWThreadsSame = constantWThreadsImg == expectedImg;

        int stop = 324;
    }
}

Image GetExpected(size_t width, size_t height, BaseFunctor functor)
{
    if (!width || !height)
    {
        return Image();
    }

    auto img = Image(width, height);

    functor(*img.GetBlock(0, 0, width, height));

    return img;
}