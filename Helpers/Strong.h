#pragma once

namespace Helpers
{
	// based on https://www.fluentcpp.com/2016/12/08/strong-types-for-strong-interfaces/
	template <typename T, typename Parameter>
	class Strong
	{
	public:
		explicit Strong(T const& value) : value_(value) {}
		explicit Strong(T&& value) : value_(std::move(value)) {}
		operator T& () { return value_; }
		operator T const& () const { return value_; }
	private:
		T value_;
	};
}
