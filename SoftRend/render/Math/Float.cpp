#include "Float.h"

Float4::operator Float2() const {
    return Float2(this->x, this->y);
}

Float4 Float4::operator*(const Float4& other) const {
    Float4 res;

    res.x = this->x * other.x;
    res.y = this->y * other.y;
    res.z = this->z * other.z;
    res.w = this->w * other.w;

    return res;
}

Float4 Float4::operator+(const Float4& other) const {
    Float4 res;

    res.x = this->x + other.x;
    res.y = this->y + other.y;
    res.z = this->z + other.z;
    res.w = this->w + other.w;

    return res;
}




Float2 Float2::operator-(const Float2& other) const {
    Float2 res;

    res.x = this->x - other.x;
    res.y = this->y - other.y;

    return res;
}

float Float2::Cross(const Float2& other) const {
    float cross = (this->x * other.y) - (this->y * other.x);
    return cross;
}
