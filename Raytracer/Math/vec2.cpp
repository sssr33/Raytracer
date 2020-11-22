#include "vec2.h"

vec2::vec2(float v)
    : x(v), y(v)
{}

vec2::vec2(float x, float y)
    : x(x), y(y)
{}

float vec2::getX() const
{
    return x;
}

float vec2::getY() const
{
    return y;
}

void vec2::setX(float v)
{
    x = v;
}

void vec2::setY(float v)
{
    y = v;
}
