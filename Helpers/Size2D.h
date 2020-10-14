#pragma once

namespace Helpers
{
	template<class T>
	struct Size2D
	{
		T width = T();
		T height = T();

		bool operator==(const Size2D& other)
		{
			return this->width == other.width && this->height == other.height;
		}

		bool operator!=(const Size2D& other)
		{
			return !this->operator==(other);
		}

		bool Empty() const
		{
			return this->width == T() || this->height == T();
		}
	};
}
