#pragma once

#include "config/cppelements_config.h"

namespace cppe
{
	// TODO
	template <class T>
	struct array_view
	{
	public:
		using class_t = array_view<T>;

	public:
		inline T* begin() const
		{
			CPPE_ASSERT(m_data != nullptr);
			return m_data;
		}
		inline T* end() const
		{
			return begin() + m_size;
		}
		/*inline T* begin()
		{
			CPPE_ASSERT(m_data != nullptr);
			return m_data;
		}
		inline T* end()
		{
			return begin() + m_size;
		}*/
		inline const T* data() const
		{
			return m_data;
		}
		inline std::size_t size() const
		{
			return m_size;
		}
		inline const T& operator[](const std::size_t index) const
		{
			CPPE_ASSERT(m_data != nullptr && index < size());
			return m_data[index];
		}
		inline T& operator[](const std::size_t index)
		{
			CPPE_ASSERT(m_data != nullptr && index < size());
			return m_data[index];
		}
		inline array_view<T> subview(const std::size_t _start, const std::size_t _end) const
		{
			CPPE_ASSERT(m_data != nullptr && _start <= size() && _end <= size() && _start <= _end);
			return array_view<T>(m_data + _start, _end - _start);
		}

	public:
		inline array_view(T* _begin, const std::size_t _size)
			: m_data(_begin)
			, m_size(_size)
		{
		}
		template <class AL>
		inline array_view(std::vector<T, AL>& v)
			: m_data(v.data())
			, m_size(v.size())
		{
		}
		template <std::size_t N>
		inline array_view(std::array<T, N>& v)
			: m_data(v.data())
			, m_size(N)
		{
		}
		template <class AL>
		inline array_view(const std::vector<T, AL>& v)
			: m_data(v.data())
			, m_size(v.size())
		{
		}

	public:
		array_view() = default;
		array_view(const class_t&) = default;
		class_t& operator=(const class_t&) = default;

	protected:
		T*			m_data = nullptr;
		std::size_t m_size = 0;
	};

}
