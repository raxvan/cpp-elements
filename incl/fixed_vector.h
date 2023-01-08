#pragma once

#include "pointer.h"

namespace cppe
{
	template <typename TYPE, const std::size_t SIZE>
	struct fixed_vector : public basic_raw_ptr<TYPE>
	{
	public:
		using base_t = basic_raw_ptr<TYPE>;
		using class_t = fixed_vector<TYPE, SIZE>;
		using iterator_t = TYPE*;
		using const_iterator = const TYPE*;
		using value_type = TYPE;
		using pointer = TYPE*;

		static constexpr std::size_t max_size()
		{
			return SIZE;
		} 

	public:
		fixed_vector()
			: m_size(0)
		{
			base_t::reset(&m_data[0]);
		}
		fixed_vector(const TYPE* t, const std::size_t s)
			: m_size(s)
		{
			CPPE_ASSERT(s <= SIZE && t != nullptr);
			reset(&m_data[0]);
			std::memcpy(m_data, t, s);
			for (std::size_t i = 0; i < s; i++)
				m_data[i] = t[i];
		}

		template <std::size_t OS>
		fixed_vector(const fixed_vector<TYPE, OS>& other)
			: fixed_vector<TYPE, SIZE>(other.get(), other.size())
		{
		}
		template <std::size_t OS>
		class_t& operator=(const fixed_vector<TYPE, OS>& other)

		{
			m_size = other.size();
			CPPE_ASSERT(m_size <= SIZE);
			for (std::size_t i = 0; i < m_size; i++)
				m_data[i] = other[i];
			return (*this);
		}
		/*template <std::size_t OS>
		class_t& operator = (fixed_vector<TYPE, OS>&& other)
		{
			m_size = other.size();
			CPPE_ASSERT(m_size <= SIZE);
			for (std::size_t i = 0; i < m_size; i++)
				m_data[i] = std::move(other.c_ptr()[i]);
			other.m_size = 0;
			return (*this);
		}*/

	public:
		template <std::convertible_to<TYPE> FROM>
		void push_back(FROM&& from)
		{
			CPPE_ASSERT(m_size < SIZE);
			m_data[m_size++] = std::forward<FROM>(from);
		}
		std::size_t push_back_index()
		{
			CPPE_ASSERT(m_size < SIZE);
			return m_size++;
		}
		void push_backwards(const TYPE& t)
		{
			CPPE_ASSERT((m_size - 1) < SIZE);
			m_data[--m_size] = t;
		}
		TYPE& push_back()
		{
			CPPE_ASSERT(m_size < SIZE);
			return m_data[m_size++];
		}
		bool try_push_back(const TYPE& t)
		{
			if (m_size < SIZE)
			{
				push_back(t);
				return true;
			}
			return false;
		}

	public:
		cppedecl_finline void erase(const std::size_t index)
		{
			CPPE_ASSERT(index < m_size);
			std::size_t s = --m_size;
			for (std::size_t i = index; i < s; i++)
				m_data[i] = std::move(m_data[i + 1]);
		}
		cppedecl_finline TYPE pop_back(const std::size_t index)
		{
			CPPE_ASSERT(index < m_size);
			TYPE rv = std::move(m_data[index]);
			m_data[index] = std::move(pop_back());
			return rv;
		}
		cppedecl_finline TYPE pop_back()
		{
			CPPE_ASSERT(m_size > 0);
			return std::move(m_data[--m_size]);
		}

	public:
		bool isEmpty() const
		{
			return m_size == 0;
		}
		bool isFull() const
		{
			return (m_size >= SIZE);
		}

		void clear()
		{

			fill(TYPE {});
			m_size = 0;
		}
		void clear(const TYPE& v)
		{

			fill(v);
			m_size = 0;
		}
		void resize(const std::size_t sz, const TYPE& v)
		{
			resize(sz);
			fill(v);
		}
		void resize(const std::size_t sz)
		{
			CPPE_ASSERT(sz < SIZE);
			m_size = sz;
		}
		void fill(const TYPE& v)
		{
			for (std::size_t i = 0, s = size(); i < s; i++)
				(*this)[i] = v;
			// fot(std::size_t i = 0,s = size();i<s;i++)
			//	(*this)[i] = v;
		}
		std::size_t size() const
		{
			return m_size;
		}
		std::size_t size_right() const
		{
			return SIZE - size();
		}

		class_t& operator=(const class_t& t)
		{
			return *new (this) class_t(t.get(), t.size());
		}
		class_t& operator=(class_t&& t)
		{
			return *new (this) class_t(std::forward(t));
		}

		cppedecl_finline const TYPE* begin() const
		{
			return base_t::get();
		}
		cppedecl_finline const TYPE* end() const
		{
			return base_t::get() + size();
		}

		cppedecl_finline const TYPE& operator[](std::size_t index) const
		{
			CPPE_ASSERT(index < size());
			return m_data[index];
		}
		cppedecl_finline TYPE& operator[](std::size_t index)
		{
			CPPE_ASSERT(index < size());
			return m_data[index];
		}

	public:
		cppedecl_finline const std::array<TYPE, SIZE>& array() const
		{
			return m_data;
		}
		cppedecl_finline std::array<TYPE, SIZE>& array()
		{
			return m_data;
		}

	protected:
		std::size_t			   m_size = 0;
		std::array<TYPE, SIZE> m_data;
	};

	//------------------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------------------
	template <typename T>
	cppedecl_finline T* append(std::vector<T>& v, const std::size_t sz)
	{
		auto cs = v.size();
		v.resize(sz + cs);
		return v.data() + cs;
	}
	template <typename T>
	cppedecl_finline void pop_back(std::vector<T>& v, const std::size_t position)
	{
		if (position < (v.size() - 1))
			v[position] = std::move(v.back());
		v.pop_back();
	}
	//------------------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------------------
}
