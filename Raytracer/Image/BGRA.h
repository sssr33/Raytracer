#pragma once

template<class ComponentT>
struct BGRA
{
	ComponentT b, g, r, a;

	bool operator==(const BGRA& other) const
	{
		bool equ = this->b == other.b
			&& this->g == other.g
			&& this->r == other.r
			&& this->a == other.a;

		return equ;
	}

	bool operator!=(const BGRA& other) const
	{
		return !this->operator==(other);
	}
};
