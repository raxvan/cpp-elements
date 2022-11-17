
#pragma once

#include "../config/cppelements_config.h"
#include <vector>

namespace cppe
{

	struct bucket_helper
	{
	public:
		struct bucket_info
		{
			std::size_t bucket_index;
			std::size_t data_index;
		};

	public:
		static bucket_info info_from_index(const std::size_t index);
		static std::size_t bucket_index_to_bucket_size(const std::size_t index);
		static std::size_t bucket_index_to_element_index(const std::size_t index);
	};

	template <class T>
	struct bucket_allocator : public bucket_helper
	{
	public:
		struct handle
		{
		public:
			T* ptr = nullptr;
		private:
			std::size_t index;
			friend struct bucket_allocator<T>;
		public:
			inline void reset()
			{
				(*this) = handle{};
			}
			inline bool operator == (const std::nullptr_t) const
			{
				return ptr == nullptr;
			}
			inline bool operator != (const std::nullptr_t) const
			{
				return ptr != nullptr;
			}
			inline std::size_t get_index() const
			{
				return index;
			}
		};
	public:
		using class_t = bucket_allocator<T>;
		bucket_allocator()
		{
		}
		~bucket_allocator()
		{
#ifdef VA_TESTING
			//make sure everything is deallocated
			std::sort(m_free_elements.begin(), m_free_elements.end());
			for(std::size_t i = 0; i < m_free_elements.size();i++)
			{
				VA_ASSERT(i == m_free_elements[i]);
			}
#endif
			for (auto b : m_buckets)
				delete[] b.buffer;
		}
		bucket_allocator(const class_t&) = delete;
		class_t& operator = (const class_t&) = delete;

		void swap(class_t& other)
		{
			m_free_elements.swap(other.m_free_elements);
			m_buckets.swap(other.m_buckets);
		}
	public:
		handle alloc()
		{
			handle h;

			if (m_free_elements.size() == 0)
			{
				h.index = append_bucker();
				h.ptr = m_buckets.back().buffer;
			}
			else
			{
				h.index = m_free_elements.back();
				m_free_elements.pop_back();
				auto loc = info_from_index(h.index);
				VA_ASSERT(loc.bucket_index < m_buckets.size());
				VA_ASSERT(loc.data_index < m_buckets[loc.bucket_index].size);
				h.ptr = &m_buckets[loc.bucket_index].buffer[loc.data_index];
			}
			return h;

		}
		void free(const handle& handle)
		{
			m_free_elements.push_back(handle.index);
		}
	protected:
		std::size_t append_bucker()
		{
			std::size_t bucket_index = m_buckets.size();
			std::size_t bucket_count = bucket_index + 1;
			m_buckets.resize(bucket_count);

			std::size_t bucket_size = bucket_index_to_bucket_size(bucket_index);
			std::size_t element_id = bucket_index_to_element_index(bucket_index);

			m_buckets[bucket_index].buffer = new T[bucket_size];
			m_buckets[bucket_index].size = bucket_size;

			m_free_elements.resize(bucket_size - 1);
			for (std::size_t i = 1; i < bucket_size; i++)
			{
				m_free_elements[i - 1] = element_id + bucket_size - i;
			}

			return element_id;
		}
	protected:
		struct bucket_info
		{
			T* buffer;
			std::size_t size;
		};
		std::vector<std::size_t> 	m_free_elements;
		std::vector<bucket_info>	m_buckets;


	};

}
