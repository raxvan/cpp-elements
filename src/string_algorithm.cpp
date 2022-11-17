
#include "string_helpers.h"
#include "string_view.h"
#include <sstream>

namespace cppe
{
	void strhelpers::split(const char* _begin, const char* model, std::function<void(string_view)> _func, const bool ignore_null_strings)
	{
		CPPE_ASSERT(_begin != nullptr && model != nullptr);
		std::size_t model_len = strutil::length(model);

		while (*_begin != '\0')
		{
			CPPE_ASSERT(strutil::is_readable_ascii(*_begin));

			const char* fpos = strutil::find(model, _begin);
			if (fpos != nullptr)
			{
				auto len = std::size_t(std::distance(_begin, fpos));
				if (!(ignore_null_strings && len == 0))
					_func(string_view(_begin, fpos));
				_begin += len + model_len;
			}
			else
			{
				string_view s(_begin);
				if (!(ignore_null_strings && s.size() == 0))
					_func(s);
				break;
			}
		}
	}
	void strhelpers::split_white(const char* source, std::function<void(string_view)> _func)
	{
		CPPE_ASSERT(source != nullptr);
		while (*source)
		{
			CPPE_ASSERT(strutil::is_readable_ascii(*source));
			const char* fpos = strutil::find_next_whitespace(source);
			if (fpos != nullptr)
			{
				_func(string_view(source, fpos));
				source = strutil::iterate_whitespace(fpos);
			}
			else
			{
				_func(string_view(source));
				break;
			}
		}
	}

	std::string strhelpers::std_trim(const std::string& str, const std::string& whitespace)
	{
		const auto strBegin = str.find_first_not_of(whitespace);
		if (strBegin == std::string::npos)
			return ""; // no content

		const auto strEnd = str.find_last_not_of(whitespace);
		const auto strRange = strEnd - strBegin + 1;

		return str.substr(strBegin, strRange);
	}

	std::vector<std::string>& strhelpers::std_split(const std::string& s, char delim, std::vector<std::string>& elems)
	{

		std::stringstream ss(s);
		std::string		  item;
		while (std::getline(ss, item, delim))
		{
			elems.push_back(item);
		}
		return elems;
	}
}
