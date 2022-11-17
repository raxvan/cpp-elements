#pragma once

#include "string_utils.h"
#include <string_view>

namespace cppe
{
	struct string_view;
	//------------------------------------------------------------------------------------------
	template <const std::size_t SIZE>
	struct fixed_string
	{
	public:
		using class_t = fixed_string<SIZE>;

		cppedecl_finline fixed_string()
			: m_size(0)
		{
			m_data[0] = 0;
		}
		fixed_string(const char* c)
			: class_t(c, cppe::strutil::length(c))
		{
		}
		fixed_string(const char* s, const std::size_t _size)
			: m_size(_size)
		{
			CPPE_ASSERT(s != nullptr);
			m_size = std::min(m_size, SIZE - 1);
			std::memcpy(m_data, s, m_size);
			m_data[m_size] = '\0';
		}
		fixed_string(const class_t& s)
			: class_t(s.c_str(), s.size())
		{
		}
		template <std::size_t N>
		fixed_string(const char (&cs)[N])
			: class_t(&cs[0], N - 1)
		{
		}
		fixed_string(const std::string_view& s);
		fixed_string(const cppe::string_view& s);

		class_t& operator=(const class_t& other)
		{
			m_size = std::min(other.size(), SIZE - 1);
			std::memcpy(m_data, other.c_str(), m_size);
			m_data[m_size] = '\0';
			return (*this);
		}

	public:
		template <typename... ARGS>
		class_t& format(const char* f, const ARGS&... a)
		{
			CPPE_ASSERT(f != nullptr);
			std::size_t sz = std::snprintf(&m_data[m_size], SIZE - m_size, f, a...);
			m_size = std::min(m_size + sz, SIZE - 1);
			return (*this);
		}

		class_t& operator=(const std::string_view& as);
		class_t& operator=(const char* c)
		{
			return *new (this) class_t(c);
		}
		template <unsigned int N>
		class_t& operator=(const char (&c)[N])
		{
			return *new (this) class_t(&c[0], N);
		}

		// for specific de-serialization only
		std::string_view create(const std::size_t len);
		std::string_view to_string_view() const;

	public:
		bool operator<(const class_t& s) const
		{
			if (m_size == s.m_size)
				return cppe::strutil::less(get(), s.get(), size());
			else
				return (m_size < s.m_size);
		}

		bool operator==(const class_t& s) const
		{

			if (m_size == s.m_size)
				return (cppe::strutil::equals(&m_data[0], &s.m_data[0], size()));
			return false;
		}
		bool operator!=(const class_t& s) const
		{

			if (m_size == s.m_size)
				return (!cppe::strutil::equals(&m_data[0], &s.m_data[0], size()));
			return true;
		}

	public:
		bool operator==(const char* s) const
		{
			return cppe::strutil::equals(c_str(), s);
		}
		bool operator!=(const char* s) const
		{
			return !cppe::strutil::equals(c_str(), s);
		}

	public:
		void assign(const char* t, const std::size_t _size)
		{
			CPPE_ASSERT(t != nullptr);
			new (this) fixed_string<SIZE>(t, _size);
		}
		template <const std::size_t OS>
		void swap(fixed_string<OS>& s)
		{
			char buffer[SIZE + 1];

			std::memcpy(buffer, get(), size()); // tmp = this

			std::memcpy(get(), s.get(), std::min(SIZE - 1, s.size())); // this = other

			std::memcpy(s.get(), buffer, std::min(OS - 1, size())); // other = tmp

			std::swap(m_size, s.m_size);
		}

	public:
		void clear()
		{
			m_size = 0;
			m_data[0] = '\0';
		}
		operator char*()
		{
			return data();
		}
		operator const char*() const
		{
			return c_str();
		}
		char* data()
		{
			return (const_cast<char*>(&m_data[0]));
		}
		const char* c_str() const
		{
			return (&m_data[0]);
		}
		const char* get() const
		{
			return (&m_data[0]);
		}
		bool isEmpty() const
		{
			return (m_size == 0);
		}
		char back() const
		{
			CPPE_ASSERT(m_size > 0);
			return m_data[m_size - 1];
		}
		std::size_t size() const
		{
			return m_size;
		}
		cppedecl_finline std::size_t maxsize() const
		{
			return SIZE;
		}

	public:
		void push_back(const char c)
		{
			CPPE_ASSERT(m_size < (SIZE - 2));
			m_data[m_size++] = c;
			m_data[m_size] = '\0';
		}
		void pop_back()
		{
			CPPE_ASSERT(m_size > 0);
			m_data[m_size--] = '\0';
		}
		void trim(const std::size_t n_chars)
		{
			CPPE_ASSERT(n_chars <= m_size);
			m_size -= n_chars;
			m_data[m_size] = '\0';
		}

	private:
		std::size_t m_size = 0;
		char		m_data[SIZE];
	};

	//--------------------------------------------------------------------------------------------------------------------------------

	template <std::size_t N>
	cppedecl_finline fixed_string<N>::fixed_string(const std::string_view& s)
		: fixed_string<N>(s.data(), s.size())
	{
	}

	template <std::size_t N>
	cppedecl_finline fixed_string<N>& fixed_string<N>::operator=(const std::string_view& as)
	{
		return *new (this) class_t(as);
	}
	template <std::size_t N>
	cppedecl_finline std::string_view fixed_string<N>::create(const std::size_t len)
	{
		m_size = std::min(len, N - 1);
		m_data[m_size] = 0;
		return std::string_view(get(), m_size);
	}
	template <std::size_t N>
	cppedecl_finline std::string_view fixed_string<N>::to_string_view() const
	{
		return std::string_view(get(), size());
	}
}
