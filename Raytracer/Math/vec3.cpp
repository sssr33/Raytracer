#include "vec3.h"

vec3::vec3(float v)
    : x(v), y(v), z(v)
{}

vec3::vec3(float x, float y, float z)
    : x(x), y(y), z(z)
{}

float vec3::getX() const
{
    return x;
}

float vec3::getY() const
{
    return y;
}

float vec3::getZ() const
{
    return z;
}

void vec3::setX(float v)
{
    x = v;
}

void vec3::setY(float v)
{
    y = v;
}

void vec3::setZ(float v)
{
    z = v;
}
