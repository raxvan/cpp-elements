#pragma once

#include "../config/core_base_include.h"

namespace cppe
{

	template <class T>
	struct primitive_pool
	{
	public:
		primitive_pool(const std::size_t sz)
		{
			m_free_indices.resize(sz);
			for (std::size_t i = 0; i < sz; i++)
				m_free_indices[i] = uint32_t(sz - i - 1);

			m_data.resize(sz);
		}

		T* create()
		{
			if (m_free_indices.size() > 0)
			{
				uint32_t i = m_free_indices.back();
				m_free_indices.pop_back();
				return &m_data[i];
			}
			return nullptr;
		}

		void free(const T* px)
		{
			auto d = il::ptr_distance(&m_data[0], px);
			CPPE_ASSERT(d >= 0 && d < m_data.size());
			auto& element = m_data[uint32_t(d)];
			{
				// rebuild
				element.~T();
				new (&element) T;
			}
			m_free_indices.push_back(uint32_t(d));
		}

		template <class F>
		void visit_allocations(const F& _func)
		{
			std::sort(m_free_indices.begin(), m_free_indices.end());

			uint32_t itr = 0;
			for (const auto ind : m_free_indices)
			{
				while (itr < ind)
					_func(m_data[itr++]);
				itr++;
			}
		}

		void rebuild(const std::size_t sz)
		{
			visit_allocations([&](T& e) { e = T(); });

			m_free_indices.resize(sz);
			for (std::size_t i = 0; i < sz; i++)
				m_free_indices[i] = uint32_t(sz - i - 1);

			m_data.resize(sz);
		}

	public:
		std_vector<uint32_t> m_free_indices;
		std_vector<T>		 m_data;
	};

}
