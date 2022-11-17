
#pragma once

#include "../config/cppelements_config.h"
#include <vector>
#ifdef CPPE_POOL_VALIDATION
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

	template <class T, std::size_t BUCKET_SKIP_COUNT = 0>
	struct primitive_bucket_pool : public bucket_helper
	{
	public:
		struct handle
		{
		public:
			T* ptr = nullptr;

		private:
			uint_fast32_t index;
			friend struct primitive_bucket_pool<T, BUCKET_SKIP_COUNT>;

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
#ifdef CPPE_TESTING
			inline uint_fast32_t get_debug_index() const
			{
				//used for validation only
				return index - ((uint_fast32_t(1) << uint_fast32_t(BUCKET_SKIP_COUNT)) - 1);
			}
#endif
		};

		using class_t = primitive_bucket_pool<T>;
		using handle_t = handle;

	public:
		primitive_bucket_pool() = default;
		primitive_bucket_pool(const class_t&) = delete;
		class_t& operator=(const class_t&) = delete;

		~primitive_bucket_pool()
		{
#ifdef CPPE_POOL_VALIDATION
			validate_empty();
#endif
			for (auto b : m_buckets)
				delete[] b.buffer;
		}

#ifdef CPPE_POOL_VALIDATION
		void validate_empty()
		{
			// make sure everything is deallocated
			std::sort(m_free_indices.begin(), m_free_indices.end());
			uint_fast32_t start = bucket_index_to_element_index(uint_fast32_t(BUCKET_SKIP_COUNT));
			for (std::size_t i = 0; i < m_free_indices.size(); i++)
			{
				CPPE_ASSERT(start == m_free_indices[i]);
				start++;
			}
		}
#endif

		cppedecl_finline void swap(class_t& other)
		{
			m_free_indices.swap(other.m_free_indices);
			m_buckets.swap(other.m_buckets);
		}

	public:
		handle create()
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

				auto loc = index_to_valid_location(h.index);
				h.ptr = &m_buckets[loc.bucket_index].buffer[loc.data_index];
			}
			return h;
		}
		cppedecl_finline void release(const handle& handle)
		{
			m_free_indices.push_back(handle.index);
		}
		
		void release(const T* pv)
		{
			for(std::size_t i = 0, s = m_buckets.size();i < s;i++)
			{
				const auto& b = m_buckets[i];
				if(b.buffer <= pv && pv < (b.buffer + b.size))
				{
					auto element_id = bucket_index_to_element_index(uint_fast32_t(i + BUCKET_SKIP_COUNT));
					m_free_indices.push_back(element_id + start_end_distance(b.buffer, pv));
					return;
				}
			}
			CPPE_ASSERT(false);//object is not part of this pool
		}

		void clear()
		{
			m_free_indices.clear();
			for(std::size_t b = m_buckets.size();b > 0; b--)
			{
				auto bucket_size = bucket_index_to_bucket_size(uint_fast32_t(b + BUCKET_SKIP_COUNT - 1));
				auto element_id = bucket_index_to_element_index(uint_fast32_t(b + BUCKET_SKIP_COUNT - 1));
				m_free_indices.reserve(m_free_indices.size() + bucket_size - 1);
				for (uint_fast32_t i = 0; i < bucket_size; i++)
					m_free_indices.push_back(element_id + bucket_size - i - 1);
			}
		}

		template <class F>
		void visit_objects(const F& _func)
		{
			std::sort(m_free_indices.begin(), m_free_indices.end(), std::greater<uint_fast32_t>());

			uint_fast32_t itr = bucket_index_to_element_index(uint_fast32_t(BUCKET_SKIP_COUNT));
			for (std::size_t i = m_free_indices.size(); i > 0 ; i--)
			{
				const auto ind = m_free_indices[i - 1];
				while (itr < ind)
				{
					auto loc = index_to_valid_location(itr++);
					_func(m_buckets[loc.bucket_index].buffer[loc.data_index]);
				}
				itr++;
			}
		}
	protected:
		uint_fast32_t append_bucket()
		{
			std::size_t bucket_index = m_buckets.size();
			m_buckets.resize(bucket_index + 1);

			auto bucket_size = bucket_index_to_bucket_size(uint_fast32_t(bucket_index + BUCKET_SKIP_COUNT));
			auto element_id = bucket_index_to_element_index(uint_fast32_t(bucket_index + BUCKET_SKIP_COUNT));

			m_buckets[bucket_index].buffer = new T[bucket_size];
			m_buckets[bucket_index].size = bucket_size;

			m_free_indices.resize(bucket_size - 1);
			for (uint_fast32_t i = 1; i < bucket_size; i++)
				m_free_indices[i - 1] = element_id + bucket_size - i;

			return element_id;
		}

		cppedecl_finline bucket_helper::bucket_info index_to_valid_location(uint_fast32_t ind) const
		{
			auto loc = info_from_index(ind);
			CPPE_ASSERT(BUCKET_SKIP_COUNT <= loc.bucket_index);
			loc.bucket_index = loc.bucket_index - uint_fast32_t(BUCKET_SKIP_COUNT);
			CPPE_ASSERT(loc.bucket_index < m_buckets.size() && loc.data_index < m_buckets[loc.bucket_index].size);
			return loc;
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
