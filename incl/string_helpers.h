#pragma once

#include "string_utils.h"
#include "string_view.h"
#include <functional>
#include <algorithm>

namespace cppe
{
	struct strhelpers
	{
	public:
		static std::string				 std_trim(const std::string& str, const std::string& whitespace = " \t\n");
		static std::vector<std::string>& std_split(const std::string& s, char delim, std::vector<std::string>& elems);

	public:
		static void split(const char* source, const char* model, std::function<void(string_view)> _func, const bool ignore_null_strings = true);
		static void split_white(const char* source, std::function<void(string_view)> _func);

	public:
		// TODO: simplify all other function
		template <std::size_t N, class F>
		static const char* tokenize(const char* _begin, const char* _end, const char (&split)[N], char end_char, const F& _func)
		{
			CPPE_ASSERT(_begin != nullptr && _end != nullptr && _begin < _end);
			std::array<char, N> s;
			// TODO: constexpr
			for (std::size_t i = 0; i < N; i++)
				s[i] = split[i];
			std::sort(s.begin(), s.end());

			auto skip = [&]() {
				char ch = *_begin;
				auto itr = std::lower_bound(s.begin(), s.end(), ch);
				if (itr != s.end() && *itr == ch)
					return true;
				return false;
			};

			const char* _start = _begin;
			bool		skip_state = true;
			while (_begin < _end && strutil::is_readable_ascii(*_begin) && *_begin != end_char)
			{
				bool skipped = skip();
				if (skipped != skip_state)
				{
					skip_state = skipped;

					if (skipped == true)
						_func(cppe::string_view(_start, _begin));
					else
						_start = _begin;
				}
				_begin++;
			}
			if (skip_state == false)
				_func(cppe::string_view(_start, _begin));
			if (_begin < _end && *_begin == end_char)
				_begin++;
			return _begin;
		}

	public:
		template <typename F>
		static cppedecl_finline const char* parse_line(const char* _begin, const char* _end, F&& _functor, const char& break_ch = '\n')
		{
			CPPE_ASSERT(_begin != nullptr && strutil::is_readable_ascii(break_ch));
			while (*_begin && _begin != _end)
			{
				CPPE_ASSERT(strutil::is_readable_ascii(*_begin));
				if (*_begin == break_ch)
					break;
				else
				{
					_functor(*_begin);
				}

				_begin++;
			}
			return _begin + 1;
		}

		template <typename C>
		static cppedecl_finline const char* readline(const char* _begin, const char* _end, C& _pb_container, const char& break_ch = '\n')
		{
			return parse_line(
				_begin, _end, [&](const char& ch) { _pb_container.push_back(ch); }, break_ch);
		}
		template <typename C>
		static cppedecl_finline const char* readline(const char* _begin, C& _pb_container, const char& break_ch = '\n')
		{
			return parse_line(
				_begin, nullptr, [&](const char& ch) { _pb_container.push_back(ch); }, break_ch);
		}

		template <typename C>
		static const char* readline_trimmed(const char* _begin, C& _pb_container, const char& break_ch = '\n')
		{
			const char* no_white = strutil::iterate_line_whitespace(_begin);
			const char* rv = readline(no_white, _pb_container, break_ch);
			while (_pb_container.size() > 0 && strutil::is_whitespace(_pb_container.back()))
				_pb_container.pop_back();
			return rv;
		}
		template <typename C>
		static const char* readline_trimmed(const char* _begin, const char* _end, C& _pb_container, const char& break_ch = '\n')
		{
			const char* no_white = strutil::iterate_line_whitespace(_begin);
			if (no_white >= _end)
				return _end;
			const char* rv = readline(no_white, _end, _pb_container, break_ch);
			while (_pb_container.size() > 0 && strutil::is_whitespace(_pb_container.back()))
				_pb_container.pop_back();
			return rv;
		}

	public:
		// unicode:
		//\uXXXX
		/*bool decodeUnicodeEscapeSequence(const char * start, const char * end, uint32_t& unicode)
		{

			unicode = 0;
			for (std::size_t index = 0; index < 4; ++index)
			{
				char c = *current++;
				unicode *= 16;
				if (c >= '0' && c <= '9')
					unicode += c - '0';
				else if (c >= 'a' && c <= 'f')
					unicode += c - 'a' + 10;
				else if (c >= 'A' && c <= 'F')
					unicode += c - 'A' + 10;
				else
					return false;
			}
			return true;
		}*/

		//----------------------------------------------------------------------------------------------------------------------------------------------------
	};
}
