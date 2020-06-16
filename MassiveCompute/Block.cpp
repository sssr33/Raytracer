#include "Block.h"

bool Block::operator==(const Block& other) const
{
	bool same = this->image == other.image
		&& this->SameParameters(other);

	return same;
}

bool Block::SameParameters(const Block& other) const
{
	bool same = this->left == other.left
		&& this->top == other.top
		&& this->right == other.right
		&& this->bottom == other.bottom
		&& this->imageWidth == other.imageWidth
		&& this->imageHeight == other.imageHeight;

	return same;
}
