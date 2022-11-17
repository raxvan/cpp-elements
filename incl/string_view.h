#pragma once

#include "fixed_string.h"
#include "string_utils.h"

namespace cppe
{
	struct string_view_traits
	{
		using length_storage_t = int32_t;

		static int32_t size_from_size_t(const std::size_t);

		template <class InputIt>
		static std::size_t size_t_distance(InputIt first, InputIt last);
	};

	//------------------------------------------------------------------------------------------
	// NOTE: There are some advantages of using this over a standard std::string_view.
	//      cppe::string_view knows if it's terminated or not.
	struct string_view
	{
	public:
		using string_view_size_t = string_view_traits::length_storage_t;

	public:
		inline ~string_view()
		{
		}

	public:
		inline string_view(std::nullptr_t)
			: string_view()
		{
		}
		string_view(const string_view&) = default;
		string_view& operator=(const string_view&) = default;

	public:
		string_view();
		string_view(const char* s, const std::size_t sz);

		string_view(const char* s)
			: string_view(make_null_terminated(s, strutil::length(s)))
		{
		}

	public:
		template <std::size_t N>
		string_view(const char (&s)[N])
			: string_view(make_null_terminated(s, N - 1))
		{
		}

		template <std::size_t SIZE>
		string_view(const fixed_string<SIZE>& s)
			: string_view(make_null_terminated(s.c_str(), s.size()))
		{
		}
		string_view(const std::string& s)
			: string_view(make_null_terminated(s.c_str(), s.size()))
		{
		}

		template <class Allocator>
		string_view(const std::vector<char, Allocator>& vec_data)
			: string_view(vec_data.data(), vec_data.size())
		{
		}

		template <class TItr>
		string_view(const TItr& _begin, const TItr& _end)
			: string_view(make_subview(&(*_begin), string_view_traits::size_t_distance(_begin, _end)))
		{
		}

	public:
		static cppedecl_finline string_view make_null_terminated(const char* s, const std::size_t sz)
		{
			return string_view(s, sz, nullptr);
		}
		static cppedecl_finline string_view make_subview(const char* s, const std::size_t sz)
		{
			CPPE_ASSERT(sz > 0);
			return string_view(std::pair<const char*, std::size_t> { s, sz });
		}

	private:
		cppedecl_finline string_view(const char* s, const std::size_t sz, std::nullptr_t)
			: m_px(s)
			, m_size(string_view_size_t(sz))
		{
			CPPE_ASSERT(s != nullptr);
		}
		cppedecl_finline string_view(const std::pair<const char*, std::size_t>& s)
			: m_px(s.first)
			, m_size(-string_view_traits::size_from_size_t(s.second))
		{
			CPPE_ASSERT(s.first != nullptr);
		}

	public:
		std::string		 std_string() const;
		std::string_view std_string_view() const;

	public:
		template <std::size_t N>
		string_view& operator=(const char (&s)[N])
		{
			return *new (this) string_view(s);
		}
		string_view& operator=(const char* s)
		{
			return *new (this) string_view(s);
		}
		void clear()
		{
			new (this) string_view {};
		}
		bool empty() const
		{
			return (size() == 0);
		}
		bool is_terminated() const
		{
			CPPE_ASSERT(m_px != nullptr);
			return m_size >= 0;
		}

	public:
		cppedecl_finline char operator[](const std::size_t index) const
		{
			CPPE_ASSERT(m_px != nullptr && index < size());
			return m_px[index];
		}
		cppedecl_finline std::size_t size() const
		{
			return std::size_t(std::abs(m_size));
		}
		cppedecl_finline const char* c_str() const
		{
			CPPE_ASSERT(is_terminated());
			return m_px;
		}
		cppedecl_finline const char* data() const
		{
			CPPE_ASSERT(m_px != nullptr);
			return m_px;
		}
		char back() const
		{
			CPPE_ASSERT(m_px != nullptr && size() > 0);
			return data()[size() - 1];
		}
		const char* begin() const
		{
			return data();
		}
		const char* end() const
		{
			return data() + size();
		}

	public:
		int32_t	 std_parse_int32(const int32_t default_value, bool& error_flag) const;
		uint32_t std_parse_unsigned32(const uint32_t default_value, bool& error_flag) const;
		float	 std_parse_float(const float default_value, bool& error_flag) const;

		bool std_parse_int32(int32_t& out) const;
		bool std_parse_unsigned32(uint32_t& out) const;
		bool std_parse_float(float& out) const;

	public:
		bool operator==(const string_view& as) const
		{
			if (size() == as.size())
				return strutil::equals(c_str(), as.c_str(), size());
			return false;
		}
		bool operator==(const char* as) const
		{
			return (*this) == string_view(as);
		}
		bool operator<(const string_view& as) const
		{
			if (size() == as.size())
				return strutil::less(c_str(), as.c_str(), size());
			return size() < as.size();
		}
		bool operator!=(const string_view& as) const
		{
			if (size() == as.size())
				return strutil::equals(c_str(), as.c_str(), size());
			return true;
		}

	protected:
		const char*		   m_px = nullptr;
		string_view_size_t m_size = 0;
	};

	template <std::size_t N>
	cppedecl_finline fixed_string<N>::fixed_string(const cppe::string_view& s)
		: fixed_string<N>(s.data(), s.size())
	{
	}

	template <class InputIt>
	cppedecl_finline std::size_t string_view_traits::size_t_distance(InputIt first, InputIt last)
	{
		auto r = std::distance(first, last);
		CPPE_ASSERT(r >= 0);
		return static_cast<std::size_t>(r);
	}

}
