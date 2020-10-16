#include "Block.h"

bool Block::operator==(const Block& other) const
{
	bool same = this->left == other.left
		&& this->top == other.top
		&& this->right == other.right
		&& this->bottom == other.bottom
		&& this->imageWidth == other.imageWidth
		&& this->imageHeight == other.imageHeight;

	return same;
}
