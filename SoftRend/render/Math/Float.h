#pragma once

struct Float4 {
    float x, y, z, w;

    Float4() = default;
    Float4(float x, float y, float z, float w)
        : x(x), y(y), z(z), w(w)
    {}
};

struct Float3 {
    float x, y, z;

    Float3() = default;
    Float3(float x, float y, float z)
        : x(x), y(y), z(z)
    {}
};

struct Float2 {
    float x, y;

    Float2() = default;
    Float2(float x, float y)
        : x(x), y(y)
    {}
};