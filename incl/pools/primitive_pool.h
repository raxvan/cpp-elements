#pragma once

#include "../config/cppelements_config.h"
#ifdef CPPE_POOL_VALIDATION
#	include <algorithm>
#endif

namespace cppe
{

	template <class T>
	struct primitive_pool
	{
	public:
		primitive_pool(const std::size_t sz)
		{
			m_free_indices.resize(sz);
			for (uint_fast32_t i = 0; i < sz; i++)
				m_free_indices[i] = uint_fast32_t(sz - i - 1);

			m_data.resize(sz);
		}

		~primitive_pool()
		{
#ifdef CPPE_POOL_VALIDATION
			// make sure everything is deallocated
			std::sort(m_free_indices.begin(), m_free_indices.end());
			for (uint_fast32_t i = 0; i < m_free_indices.size(); i++)
			{
				CPPE_ASSERT(i == m_free_indices[i]);
			}
#endif
		}

	public:
		cppedecl_finline T* create()
		{
			if (m_free_indices.size() > 0)
			{
				uint_fast32_t i = m_free_indices.back();
				m_free_indices.pop_back();
				return &m_data[i];
			}
			return nullptr;
		}

		cppedecl_finline void release(const T* px)
		{
			auto d = start_end_distance(&m_data[0], px);
			CPPE_ASSERT(d < m_data.size());
			auto& element = m_data[uint_fast32_t(d)];
			{
				// rebuild
				element.~T();
				new (&element) T;
			}
			m_free_indices.push_back(uint_fast32_t(d));
		}

	public:
		template <class F>
		void visit_objects(const F& _func)
		{
			std::sort(m_free_indices.begin(), m_free_indices.end(), std::greater<uint_fast32_t>());

			uint_fast32_t itr = 0;
			for (std::size_t i = m_free_indices.size(); i > 0; i--)
			{
				const auto ind = m_free_indices[i - 1];
				while (itr < ind)
					_func(m_data[itr++]);
				itr++;
			}
		}
		void clear()
		{
			m_free_indices.resize(m_data.size());
			uint_fast32_t s = uint_fast32_t(m_data.size());
			for (uint_fast32_t i = s; i > 0; i++)
			{
				m_free_indices[s - i] = s - i - 1;
			}
		}

		void rebuild(const std::size_t sz)
		{
			visit_objects([&](T& e) { e = T(); });

			m_free_indices.resize(sz);
			for (uint_fast32_t i = 0; i < sz; i++)
				m_free_indices[i] = uint_fast32_t(sz - i - 1);

			m_data.resize(sz);
		}

	public:
		std::vector<uint_fast32_t> m_free_indices;
		std::vector<T>			   m_data;
	};

}
