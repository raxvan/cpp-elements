
#include "fixed_string.h"
#include "string_utils.h"
#include <locale>

namespace cppe
{

	uint32_t strutil::base16charToUnsigned32(const char c)
	{
		if (c >= '0' && c <= '9')
			return uint32_t(c - '0');
		else if (c >= 'a' && c <= 'f')
			return uint32_t(c - 'a') + 10;
		else if (c >= 'A' && c <= 'F')
			return uint32_t(c - 'A') + 10;
		else
		{
			CPPE_ASSERT(false);
			return 0;
		}
	}

	strutil::utf8_t strutil::unicodeToUtf8(const uint32_t utfValue)
	{
		// http://en.wikipedia.org/wiki/UTF-8

		strutil::utf8_t result;
		if (utfValue < 0x80)
		{
			result.first[0] = static_cast<char>(utfValue);
			result.second = 1;
		}
		else if (utfValue < 0x800)
		{
			result.first[0] = static_cast<char>(0xC0 | (0x1f & (utfValue >> 6)));
			result.first[1] = static_cast<char>(0x80 | (0x3f & utfValue));
			result.second = 2;
		}
		else if (utfValue < 0x10000)
		{
			result.first[0] = static_cast<char>(0xE0 | (0xf & (utfValue >> 12)));
			result.first[1] = static_cast<char>(0x80 | (0x3f & (utfValue >> 6)));
			result.first[2] = static_cast<char>(0x80 | (0x3f & utfValue));
			result.second = 3;
		}
		else if (utfValue < 0x110000)
		{
			result.first[0] = static_cast<char>(0xF0 | (0x7 & (utfValue >> 18)));
			result.first[1] = static_cast<char>(0x80 | (0x3f & (utfValue >> 12)));
			result.first[2] = static_cast<char>(0x80 | (0x3f & (utfValue >> 6)));
			result.first[3] = static_cast<char>(0x80 | (0x3f & utfValue));
			result.second = 4;
		}
		return result;
	}

	bool strutil::unicodeFromBase16char(const std::array<char, 4> c, uint32_t& out)
	{
		out = base16char4ToUnsigned32(c);
		return (out >= 0xD800 && out <= 0xDBFF);
	}
	void strutil::unicodeSurrogateBase16char(const std::array<char, 4> c, uint32_t& original)
	{
		CPPE_ASSERT(original >= 0xD800 && original <= 0xDBFF);
		uint32_t sp = base16char4ToUnsigned32(c);
		original = 0x10000 + ((original & 0x3FF) << 10) + (sp & 0x3FF);
	}

	uint32_t strutil::base16char4ToUnsigned32(const std::array<char, 4> c)
	{

		uint32_t value = 0;
		value = value * 16 + base16charToUnsigned32(c[0]);
		value = value * 16 + base16charToUnsigned32(c[1]);
		value = value * 16 + base16charToUnsigned32(c[2]);
		value = value * 16 + base16charToUnsigned32(c[3]);
		return value;
	}
	bool strutil::decodeEscapeChar(const char c, char& out)
	{

		if (c != 'u')
		{
			out = decodeEscapeChar(c);
			return true;
		}
		return false;
	}
	char strutil::decodeEscapeChar(const char c)
	{

		if (c == 'n')
			return '\n';
		else if (c == 't')
			return '\t';
		else if (c == '0')
			return '\0';
		else if (c == 'r')
			return '\r';
		else if (c == 'f')
			return '\f';
		else if (c == 'b')
			return '\b';
		else
			return c;
	}

	std::size_t strutil::find_end(char* buffer, const std::size_t size)
	{
		if (size == 0)
			return 0;
		std::size_t s = size;
		while (s > 0)
		{
			if (is_readable_ascii(buffer[s - 1]))
				break;
			s--;
		}

		return s;
	}

	std::size_t strutil::length(const char* s)
	{
		CPPE_ASSERT(s != nullptr);
#ifdef USE_CUSTOM_FAST_STDSTR
		std::size_t len = 0;
		for (;;)
		{
			uint32_t x = *reinterpret_cast<const uint32_t*>(s);
			if ((x & 0xFF) == 0)
				return len;
			CPPE_ASSERT(is_readable_ascii(x & 0xFF));
			if ((x & 0xFF00) == 0)
				return len + 1;
			CPPE_ASSERT(is_readable_ascii((x & 0xFF00) >> 8));
			if ((x & 0xFF0000) == 0)
				return len + 2;
			CPPE_ASSERT(is_readable_ascii((x & 0xFF0000) >> 16));
			if ((x & 0xFF000000) == 0)
				return len + 3;
			CPPE_ASSERT(is_readable_ascii((x & 0xFF000000) >> 24));
			s += 4, len += 4;
		}
		CPPE_ASSERT(false, "unreachable code");
		return len; // unreachable code
#elif defined(USE_CUSTOM_STDSTR)
		std::size_t len = 0;
		while (*s)
		{
			CPPE_ASSERT(is_readable_ascii(*s));
			s++, len++;
		}
		return len;
#else
		return std::strlen(s);
#endif
	}

	std::size_t strutil::length(const char* _start, const char* _end)
	{
		CPPE_ASSERT(_start <= _end);
		return std::size_t(_end - _start);
	}

	cppedecl_noalias bool strutil::equals_lower(const char* x, const char* y)
	{
		CPPE_ASSERT(x != nullptr && y != nullptr);

		std::locale loc;
		while (*x)
		{
			CPPE_ASSERT(is_readable_ascii(*x));
			if (std::tolower(*x++, loc) != std::tolower(*y++, loc))
				return false;
		}
		return (*y == 0);
	}
	cppedecl_noalias bool strutil::equals_lower(const char* x, const char* y, const std::size_t sz)
	{
		bool		is_equal = true;
		std::locale loc;
		for (std::size_t i = 0; is_equal && i < sz; i++)
		{
			CPPE_ASSERT(is_readable_ascii(x[i]) && is_readable_ascii(y[i]));
			is_equal = std::tolower(x[i], loc) == std::tolower(y[i], loc);
		}
		return is_equal;
	}
	inline std::size_t lower4(const std::size_t size)
	{
		// returns the largest multiple of 4 smaller than size
		return (size & ~3);
	}
	cppedecl_noalias bool strutil::equals(const char* x, const char* y, const std::size_t sz)
	{
		CPPE_ASSERT(x != nullptr && y != nullptr);
#ifdef USE_CUSTOM_STR_COMPARE
		const uint32_t* a = reinterpret_cast<const uint32_t*>(x);
		const uint32_t* b = reinterpret_cast<const uint32_t*>(y);
		std::size_t		l4 = lower4(sz);
		std::size_t		s4 = l4 / 4;
		bool			is_equal = true;
		for (std::size_t i = 0; i < s4 && is_equal; i++)
		{
			is_equal = (a[i] == b[i]);
		}
		for (std::size_t i = l4; is_equal && i < sz; i++)
		{
			CPPE_ASSERT(is_readable_ascii(x[i]) && is_readable_ascii(y[i]));
			is_equal = (x[i] == y[i]);
		}
		return is_equal;
#else
		return std::char_traits<char>::compare(x, y, sz) == 0;
#endif
	}
	cppedecl_noalias bool strutil::less(const char* _left, const char* _right, const std::size_t sz)
	{
		CPPE_ASSERT(_left != nullptr && _right != nullptr && _left != _right);
#ifdef USE_CUSTOM_STR_COMPARE
		const uint32_t* _l = reinterpret_cast<const uint32_t*>(_left);
		const uint32_t* _r = reinterpret_cast<const uint32_t*>(_right);

		std::size_t l4 = lower4(sz);
		std::size_t s4 = l4 / 4;

		for (std::size_t i = 0; i < s4; i++)
			if (_l[i] < _r[i])
				return true;
			else if (_l[i] != _r[i])
				return false;
		for (std::size_t i = l4; i < sz; i++)
		{
			if (_left[i] < _right[i])
				return true;
			else if (_left[i] != _right[i])
				return false;
		}
		return false;
#else
		return std::char_traits<char>::compare(_left, _right, sz) < 0;
#endif
	}

	cppedecl_noalias bool strutil::equals(const char* x, const char* y)
	{
		CPPE_ASSERT(x != nullptr && y != nullptr);
#ifdef USE_CUSTOM_FAST_STDSTR
		uint32_t a, b;
		for (;;)
		{
			a = *reinterpret_cast<const uint32_t*>(x);
			b = *reinterpret_cast<const uint32_t*>(y);

			if ((a & 0xFF) == (b & 0xFF))
			{
				if ((a & 0xFF) == '\0')
					return true;

				if ((a & 0xFF00) == (b & 0xFF00))
				{
					if ((a & 0xFF00) == '\0')
						return true;

					if ((a & 0xFF0000) == (b & 0xFF0000))
					{
						if ((a & 0xFF0000) == '\0')
							return true;

						if ((a & 0xFF000000) == (b & 0xFF000000))
						{
							if ((a & 0xFF000000) == '\0')
								return true;
						}
						else
							break;
					}
					else
						break;
				}
				else
					break;
			}
			else
				break;

			x += sizeof(const uint32_t);
			y += sizeof(const uint32_t);
		}

		return false;
#elif defined(USE_CUSTOM_STDSTR)
		while (*x)
		{
			CPPE_ASSERT(is_readable_ascii(*x));
			if (*x++ != *y++)
				return false;
		}
		return (*y == '\0');
#else
		return std::strcmp(x, y) == 0;
#endif
	}

	cmp_result_t strutil::compare(const char* Source1, const char* Source2)
	{
		CPPE_ASSERT(Source1 != nullptr && Source2 != nullptr);
		while (*Source1 != '\0')
		{
			CPPE_ASSERT(is_readable_ascii(*Source1));
			if (*Source1 != *Source2)
			{
				if (*Source1 < *Source2)
					return cmp_result_t::le;
				return cmp_result_t::gt;
			}
			Source1++;
			Source2++;
		}
		return (*Source2 == '\0') ? cmp_result_t::eq : cmp_result_t::le; /// equal?
	}
	const char* strutil::find_next_whitespace(const char* buffer)
	{
		CPPE_ASSERT(buffer != nullptr);
		while (*buffer)
		{
			char c = *buffer;
			CPPE_ASSERT(is_readable_ascii(c));
			if (c == ' ' || c == '\t' || c == '\n')
			{
				return buffer;
			}
			buffer++;
		}
		return nullptr;
	}
	const char* strutil::find(const char* str, const char* buffer)
	{
		CPPE_ASSERT(str != nullptr && buffer != nullptr);
		return std::strstr(buffer, str);
		/*
		const char* start = str;
		const char* retv = buffer - 1;//wtf did i just do ?
		while ( *buffer )
		{
			CPPE_ASSERT(is_readable_ascii(*buffer) && is_readable_ascii(*str));
			if ( *str == *buffer )
			{
				str++;
				if ( *str == '\0' )
					return retv + 1;
			}
			else
			{
				str = start;
				retv = buffer;
				if ( *str == *buffer )
				{
					str++;
					if ( *str == '\0' )
						return buffer;
				}
			}
			buffer++;
		}
		return nullptr;
		*/
	}

	const char* strutil::iterate_whitespace(const char* buffer)
	{
		CPPE_ASSERT(buffer != nullptr);
		while (is_whitespace(*buffer))
		{
			if (*(++buffer) == '\0')
				break;
		}
		return buffer;
	}
	const char* strutil::iterate_line_whitespace(const char* buffer)
	{
		CPPE_ASSERT(buffer != nullptr);
		while (*buffer != '\n' && *buffer == '\0' && is_whitespace(*buffer))
		{
			buffer++;
		}
		return buffer;
	}
	//--------------------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------------------
	uint32_t strutil::hash32(const char* source, const std::size_t sz)
	{
		CPPE_ASSERT(source != nullptr);
		/* magic numbers from http://www.isthe.com/chongo/tech/comp/fnv/ */
		/* Fowler / Noll / Vo (FNV) Hash */
		uint32_t	hash = 2166136261u;
		std::size_t i = 0;
		while (i++ < sz && *source)
		{
			CPPE_ASSERT(is_readable_ascii(*source));
			hash = hash ^ uint32_t(*source++); /* xor  the low 8 bits */
			hash = hash * 16777619;			   /* multiply by the magic number */
		}
		return hash;
	}
	uint64_t strutil::hash64(const char* source, const std::size_t sz)
	{
		CPPE_ASSERT(source != nullptr);
		/* magic numbers from http://www.isthe.com/chongo/tech/comp/fnv/ */
		/* Fowler / Noll / Vo (FNV) Hash */
		uint64_t	hash = 14695981039346656037ull;
		std::size_t i = 0;
		while (i++ < sz && *source)
		{
			CPPE_ASSERT(is_readable_ascii(*source));
			hash = hash ^ uint64_t(*source++); /* xor  the low 8 bits */
			hash = hash * 1099511628211ull;	   /* multiply by the magic number */
		}
		return hash;
	}
	//--------------------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------------------
	template <typename T>
	T parse_int(const char* source, bool* error_flag, const std::size_t max_length)
	{
		CPPE_ASSERT(source != nullptr);
		T	 r, sign;
		char c;

		r = T(0);
		sign = T(1);

		std::size_t sz = 0;

		while (*source && sz++ < max_length)
		{
			c = *source++;
			CPPE_ASSERT(strutil::is_readable_ascii(c));

			if (c == '-')
			{
				if (sign == -1) // multiple minus
				{
					if (error_flag != nullptr)
						*error_flag = true;
					return -r;
				}
				sign = -1;
				continue;
			}
			if (!(c >= '0' && c <= '9'))
			{
				if (error_flag != nullptr)
					*error_flag = true;
				return r * sign;
			}
			r = r * 10 + T(c - '0');
		}
		return r * sign;
	}
	int32_t strutil::parse_int32_t(const char* source, bool* error_flag, const std::size_t max_length)
	{
		return parse_int<int32_t>(source, error_flag, max_length);
	}
	int64_t strutil::pare_int64_t(const char* source, bool* error_flag, const std::size_t max_length)
	{
		return parse_int<int64_t>(source, error_flag, max_length);
	}
	uint32_t strutil::parse_uint32_t(const char* source, bool* error_flag, const std::size_t max_length)
	{
		CPPE_ASSERT(source != nullptr);
		uint32_t r;
		char	 c;
		r = 0;
		std::size_t sz = 0;
		while (*source && sz++ < max_length)
		{
			c = *source++;
			CPPE_ASSERT(is_readable_ascii(c));

			if (!(c >= '0' && c <= '9'))
			{
				if (error_flag != nullptr)
					*error_flag = true;
				return r;
			}
			r = r * 10 + uint32_t(c - '0');
		}
		return r;
	}
	template <typename T>
	T parse_real_number(const char* source, bool* error_flag, const std::size_t max_length)
	{
		CPPE_ASSERT(source != nullptr);
		T		re, im, sign;
		char	c;
		const T ten = T(10);
		im = re = T(0);
		sign = T(1);
		std::size_t sz = 0;
		while (*source && sz++ < max_length)
		{
			c = *source++;
			CPPE_ASSERT(strutil::is_readable_ascii(c));
			if (c == '.')
				break;
			if (c == '-')
			{
				if (sign < T(0))
				{
					if (error_flag != nullptr)
						*error_flag = true;
					return -re;
				}
				sign = -T(1);
				continue;
			}
			if (!(c >= '0' && c <= '9'))
			{
				if (error_flag != nullptr)
					*error_flag = true;
				return re * sign;
			}
			re = re * ten + T(c - '0');
		}
		CPPE_ASSERT(strutil::is_readable_ascii(*(source - 1)));
		if (*(source - 1) == '.')
		{
			while (('0' <= *source) && (*source <= '9') && sz++ < max_length)
			{
				CPPE_ASSERT(strutil::is_readable_ascii(*source));
				source++;
			}
			while (*--source != '.')
			{
				CPPE_ASSERT(strutil::is_readable_ascii(*source));
				im = (im + (T(*source - '0'))) / ten;
			}
		}
		return (re + im) * sign;
	}
	float strutil::parse_float(const char* source, bool* error_flag, const std::size_t max_length)
	{
		return parse_real_number<float>(source, error_flag, max_length);
	}
	double strutil::parse_double(const char* source, bool* error_flag, const std::size_t max_length)
	{
		return parse_real_number<double>(source, error_flag, max_length);
	}

	//--------------------------------------------------------------------------------------------------------------------------
	void strutil::lower(const char* source, char* dest)
	{
		CPPE_ASSERT(source != nullptr && dest != nullptr);
		std::locale loc;
		while (*source)
		{
			CPPE_ASSERT(is_readable_ascii(*source));
			*dest = std::tolower(*source, loc);

			source++;
			dest++;
		}
	}
	void strutil::upper(const char* source, char* dest)
	{
		CPPE_ASSERT(source != nullptr && dest != nullptr);
		std::locale loc;
		while (*source)
		{
			CPPE_ASSERT(is_readable_ascii(*source));
			*dest = std::toupper(*source, loc);

			source++;
			dest++;
		}
	}
}
