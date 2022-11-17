
#pragma once

#include "../config/cppelements_config.h"
#include <vector>
#ifdef CPPE_TESTING
#	include <algorithm>
#endif

namespace cppe
{

	//--------------------------------------------------------------------------------------------------------------------------------

	struct bucket_helper
	{
	public:
		struct bucket_info
		{
			uint_fast32_t bucket_index;
			uint_fast32_t data_index;
		};

	public:
		static bucket_info	 info_from_index(const uint_fast32_t index);
		static uint_fast32_t bucket_index_to_bucket_size(const uint_fast32_t index);
		static uint_fast32_t bucket_index_to_element_index(const uint_fast32_t index);
	};

	//--------------------------------------------------------------------------------------------------------------------------------

	template <class T>
	struct primitive_bucket_pool : public bucket_helper
	{
	public:
		struct handle
		{
		public:
			T* ptr = nullptr;

		private:
			uint_fast32_t index;
			friend struct primitive_bucket_pool<T>;

		public:
			inline void reset()
			{
				(*this) = handle {};
			}
			inline bool operator==(const std::nullptr_t) const
			{
				return ptr == nullptr;
			}
			inline bool operator!=(const std::nullptr_t) const
			{
				return ptr != nullptr;
			}
			inline uint_fast32_t get_index() const
			{
				return index;
			}
		};

		using class_t = primitive_bucket_pool<T>;
		using handle_t = handle;

	public:
		primitive_bucket_pool() = default;
		primitive_bucket_pool(const class_t&) = delete;
		class_t& operator=(const class_t&) = delete;

		~primitive_bucket_pool()
		{
#ifdef CPPE_TESTING
			// make sure everything is deallocated
			std::sort(m_free_indices.begin(), m_free_indices.end());
			for (std::size_t i = 0; i < m_free_indices.size(); i++)
			{
				CPPE_ASSERT(i == m_free_indices[i]);
			}
#endif
			for (auto b : m_buckets)
				delete[] b.buffer;
		}

		cppedecl_finline void swap(class_t& other)
		{
			m_free_indices.swap(other.m_free_indices);
			m_buckets.swap(other.m_buckets);
		}

	public:
		handle alloc()
		{
			handle h;

			if (m_free_indices.size() == 0)
			{
				h.index = append_bucket();
				h.ptr = m_buckets.back().buffer;
			}
			else
			{
				h.index = m_free_indices.back();
				m_free_indices.pop_back();
				auto loc = info_from_index(h.index);
				CPPE_ASSERT(loc.bucket_index < m_buckets.size());
				CPPE_ASSERT(loc.data_index < m_buckets[loc.bucket_index].size);
				h.ptr = &m_buckets[loc.bucket_index].buffer[loc.data_index];
			}
			return h;
		}
		cppedecl_finline void free(const handle& handle)
		{
			m_free_indices.push_back(handle.index);
		}

	protected:
		uint_fast32_t append_bucket()
		{
			std::size_t bucket_index = m_buckets.size();
			std::size_t bucket_count = bucket_index + 1;
			m_buckets.resize(bucket_count);

			auto bucket_size = bucket_index_to_bucket_size(uint_fast32_t(bucket_index));
			auto element_id = bucket_index_to_element_index(uint_fast32_t(bucket_index));

			m_buckets[bucket_index].buffer = new T[bucket_size];
			m_buckets[bucket_index].size = bucket_size;

			m_free_indices.resize(bucket_size - 1);
			for (uint_fast32_t i = 1; i < bucket_size; i++)
				m_free_indices[i - 1] = element_id + bucket_size - i;

			return element_id;
		}

	protected:
		struct bucket_info
		{
			T*			buffer;
			std::size_t size;
		};
		std::vector<uint_fast32_t> m_free_indices;
		std::vector<bucket_info>   m_buckets;
	};

	//--------------------------------------------------------------------------------------------------------------------------------

}
