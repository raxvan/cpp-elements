#pragma once

#include "config/cppelements_config.h"

namespace cppe
{

	template <typename T>
	struct index
	{
	public:
		static_assert(std::is_unsigned<T>::value == true, "flags class requires unsigned type");

		using class_t = index<T>;

	public:
		inline T value() const
		{
			CPPE_ASSERT(data != std::numeric_limits<T>::max());
			return data;
		}
		inline void set(const std::size_t v)
		{
			CPPE_ASSERT(v < std::numeric_limits<T>::max());
			data = T(v);
		}
		inline bool valid() const
		{
			return data != std::numeric_limits<T>::max();
		}
	public:
		T data = std::numeric_limits<T>::max();
	};


	using index8_t = index<uint8_t>;
	using index16_t = index<uint16_t>;
	using index32_t = index<uint32_t>;
	using index64_t = index<uint64_t>;

}
