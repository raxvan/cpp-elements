
#include "string_view.h"
#include <charconv>

namespace cppe
{
	string_view::string_view()
		: m_px("")
		, m_size(0)
	{
	}

	inline string_view _make_string_view(const char* strdata, const std::size_t sz)
	{
		CPPE_ASSERT(strdata != nullptr);
		if (sz == 0)
			return string_view {};

		if (strdata[sz] == '\0')
			return string_view::make_null_terminated(strdata, sz);

		return string_view::make_subview(strdata, sz);

		/*std::size_t delta = 0;
		for (auto itr = vec_data.rbegin(), end = vec_data.rend(); itr != end; itr++)
		{
			if (*itr == '\0')
				delta++;
			else
				break;
		}
		if (delta > 0)
		{
			if (delta < vec_data.size())
				return string_view::make_null_terminated(vec_data.data(), vec_data.size() - delta);
		}
		else if (vec_data.size() > 0)
		{
			return string_view::make_subview(vec_data.data(), vec_data.size());
		}
		return string_view {};
		*/
	}

	string_view::string_view(const char* s, const std::size_t sz)
		: string_view(_make_string_view(s, sz))
	{
	}
	

	std::string string_view::std_string() const
	{
		return std::string(c_str(), size());
	}
	std::string_view string_view::std_string_view() const
	{
		return std::string_view(c_str(), size());
	}
	/*
	template <class R>
	inline R parse_string_view_t(const string_view& sv, R default_value, const char* format, bool* error_flag)
	{
		if (sv.size() == 0)
			return default_value;

		R	 rv = default_value;
		bool is_error = false;
		if (sv.is_terminated())
		{
			is_error = std::sscanf(sv.c_str(), format, &rv) <= 0;
		}
		else if (sv.size() < 127)
		{
			char buffer[128];
			strutil::copy(&buffer[0], sv.data(), sv.size());
			is_error = std::sscanf(sv.data(), format, &rv) <= 0;
		}
		else
		{
			std_vector<char> buffer;
			buffer.insert(buffer.end(), sv.data(), sv.data() + sv.size());
			buffer.push_back(0);
			is_error = std::sscanf(buffer.data(), format, &rv) <= 0;
		}
		if (error_flag != nullptr && is_error)
			*error_flag = is_error;
		return rv;
	}
	*/
	int32_t string_view::std_parse_int32(const int32_t default_value, bool& error_flag) const
	{
		int32_t result;
		if (std::from_chars(data(), data() + size(), result).ec == std::errc())
			return result;
		error_flag = true;
		return default_value;
	}
	uint32_t string_view::std_parse_unsigned32(const uint32_t default_value, bool& error_flag) const
	{
		uint32_t result;
		if (std::from_chars(data(), data() + size(), result).ec == std::errc())
			return result;
		error_flag = true;
		return default_value;
	}
	float string_view::std_parse_float(const float default_value, bool& error_flag) const
	{
		float result;
		if (std::from_chars(data(), data() + size(), result).ec == std::errc())
			return result;
		error_flag = true;
		return default_value;
	}
	bool string_view::std_parse_int32(int32_t& out) const
	{
		return std::from_chars(data(), data() + size(), out).ec == std::errc();
	}
	bool string_view::std_parse_unsigned32(uint32_t& out) const
	{
		return std::from_chars(data(), data() + size(), out).ec == std::errc();
	}
	bool string_view::std_parse_float(float& out) const
	{
		return std::from_chars(data(), data() + size(), out).ec == std::errc();
	}

}
