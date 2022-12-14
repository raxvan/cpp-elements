
#include <pools/primitive_bucket_pool.h>

namespace cppe
{

	uint_fast32_t log2_unsigned(uint64_t value)
	{
		constexpr uint_fast8_t tab64[64] = { 63, 0, 58, 1, 59, 47, 53, 2, 60, 39, 48, 27, 54, 33, 42, 3, 61, 51, 37, 40, 49, 18, 28, 20, 55, 30, 34, 11, 43, 14, 22, 4, 62, 57, 46, 52, 38, 26,
			32, 41, 50, 36, 17, 19, 29, 10, 13, 21, 56, 45, 25, 31, 35, 16, 9, 12, 44, 24, 15, 8, 23, 7, 6, 5 };
		value |= value >> 1;
		value |= value >> 2;
		value |= value >> 4;
		value |= value >> 8;
		value |= value >> 16;
		value |= value >> 32;
		auto t = ((uint64_t)((value - (value >> 1)) * 0x07EDD5E59A4E28C2)) >> 58;
		return tab64[t];
	}

	bucket_helper::bucket_info bucket_helper::info_from_index(const uint_fast32_t index)
	{
		if (index == 0)
			return { 0, 0 };
		uint_fast32_t bi = log2_unsigned(index + 1);
		uint_fast32_t delta = index - ((1 << bi) - 1);
		return { bi, delta };
	}
	uint_fast32_t bucket_helper::bucket_index_to_element_index(const uint_fast32_t index)
	{
		return (uint_fast32_t(1) << index) - 1;
	}
	uint_fast32_t bucket_helper::bucket_index_to_bucket_size(const uint_fast32_t index)
	{
		return uint_fast32_t(1) << index;
	}
}
