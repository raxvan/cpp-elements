#pragma once

#include "config/cppelements_config.h"
#include <cstring>
#include <array>

namespace cppe
{
	struct strutil
	{
	public:
		static char		decodeEscapeChar(const char c);
		static bool		decodeEscapeChar(const char c, char& out); // return false if it's unicode escape sequence '\uXXXX'
		static uint32_t base16charToUnsigned32(const char c);	   //->0..15
		static uint32_t base16char4ToUnsigned32(const std::array<char, 4> c);

		static bool unicodeFromBase16char(const std::array<char, 4> c, uint32_t& out); // returns true if the sequence requires a surrogate
		static void unicodeSurrogateBase16char(const std::array<char, 4> c, uint32_t& original);

	public:
		using utf8_t = std::pair<std::array<char, 4>, std::size_t>;
		static utf8_t unicodeToUtf8(const uint32_t utfValue);

	public:
		static bool is_whitespace(const char c);

		static bool is_readable_ascii(const char c);
		template <typename OTH>
		static bool is_readable_ascii(const OTH& c);
		template <typename OTH>
		static bool is_readable_ascii(const OTH* c);

		static const char* iterate_whitespace(const char* buffer);		// jumpes over white characters; returns first non whitespace character or end of string
		static const char* iterate_line_whitespace(const char* buffer); // jumpes over white characters; returns first non whitespace character or end of string
	public:
		static cppedecl_noalias bool less(const char* _left, const char* _right, const std::size_t sz);
		static cppedecl_noalias bool equals(const char* a, const char* b, const std::size_t sz);

		static cppedecl_noalias bool equals(const char* a, const char* b);
		static cppedecl_noalias bool equals_lower(const char* a, const char* b);
		static cppedecl_noalias bool equals_lower(const char* a, const char* b, const std::size_t sz);

	public:
		static std::size_t length(const char* s);

		static cppe::cmp_result_t compare(const char* Source1, const char* Source2);
		static const char*		  find_next_whitespace(const char* buffer);
		static const char*		  find(const char* model, const char* buffer);

		//-----------------------------------------------------------------------

		static std::size_t find_end(char* buffer, const std::size_t size);
		//-----------------------------------------------------------------------
		static void copy(char* dest, const char* source, const std::size_t size);
		//-----------------------------------------------------------------------
		//-----------------------------------------------------------------------
		static void lower(const char* source, char* dest);
		static void upper(const char* source, char* dest);

	public:
		static int32_t	parse_int32_t(const char* source, bool* error_flag = nullptr, const std::size_t max_length = std::numeric_limits<std::size_t>::max());
		static uint32_t parse_uint32_t(const char* source, bool* error_flag = nullptr, const std::size_t max_length = std::numeric_limits<std::size_t>::max());
		static float	parse_float(const char* source, bool* error_flag = nullptr, const std::size_t max_length = std::numeric_limits<std::size_t>::max());
		static int64_t	pare_int64_t(const char* source, bool* error_flag = nullptr, const std::size_t max_length = std::numeric_limits<std::size_t>::max());
		static double	parse_double(const char* source, bool* error_flag = nullptr, const std::size_t max_length = std::numeric_limits<std::size_t>::max());

	public:
		static uint32_t hash32(const char* source);
		static uint64_t hash64(const char* source);
		static uint32_t hash32(const char* source, const std::size_t sz);
		static uint64_t hash64(const char* source, const std::size_t sz);

		template <typename T>
		static T hash(const char* t);
		template <typename T>
		static T hash(const char* t, const std::size_t sz);

		template <typename R, std::size_t N>
		static R		parseHexadecimal(const char* buffer);
		static uint32_t base16ToBase10(const char c);
	};
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	cppedecl_finline bool strutil::is_readable_ascii(const char c)
	{
		return is_readable_ascii<char>(c);
	}
	template <typename OTH>
	cppedecl_finline bool strutil::is_readable_ascii(const OTH* c)
	{
		CPPE_ASSERT(c != nullptr);
		return is_readable_ascii(*c);
	}
	template <typename OTH>
	cppedecl_finline bool strutil::is_readable_ascii(const OTH& c)
	{
		return (c >= 32 && c < 127) || c == '\n' || c == '\t' || c == '\r' || c == ' ';
	}
	cppedecl_finline bool strutil::is_whitespace(const char c)
	{
		CPPE_ASSERT(is_readable_ascii(c));
		return c == '\t' || c == ' ' || c == '\n';
	}

	cppedecl_finline void strutil::copy(char* dest, const char* source, const std::size_t size)
	{
		std::memcpy(dest, source, size);
		dest[size] = '\0';
	}

	cppedecl_finline uint32_t strutil::hash32(const char* source)
	{
		return hash32(source, std::numeric_limits<std::size_t>::max());
	}
	cppedecl_finline uint64_t strutil::hash64(const char* source)
	{
		return hash64(source, std::numeric_limits<std::size_t>::max());
	}

	template <typename R, std::size_t N>
	cppedecl_finline R strutil::parseHexadecimal(const char* buffer)
	{
		CPPE_ASSERT(buffer != nullptr);
		R value = 0;
		for (std::size_t i = 0; i < N; i++)
			value = (value << 4) + number_cast<R>(base16ToBase10(buffer[i]));
		return value;
	}
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	template <typename T>
	struct hash_helper;
	template <>
	struct hash_helper<uint32_t>
	{
		cppedecl_finline static uint32_t hash(const char* t)
		{
			return strutil::hash32(t);
		}
		cppedecl_finline static uint32_t hash(const char* t, const std::size_t sz)
		{
			return strutil::hash32(t, sz);
		}
	};
	template <>
	struct hash_helper<uint64_t>
	{
		cppedecl_finline static uint64_t hash(const char* t)
		{
			return strutil::hash64(t);
		}
		cppedecl_finline static uint64_t hash(const char* t, const std::size_t sz)
		{
			return strutil::hash64(t, sz);
		}
	};

	template <typename T>
	cppedecl_finline T strutil::hash(const char* t)
	{

		return hash_helper<T>::hash(t);
	}
	template <typename T>
	cppedecl_finline T strutil::hash(const char* t, const std::size_t sz)
	{

		return hash_helper<T>::hash(t, sz);
	}
}

/*
namespace il
{
	cppedecl_finline bool values_equal(const char* a, const char* b)
	{
		return cppe::strutil::equals(a, b);
	}
}
*/
