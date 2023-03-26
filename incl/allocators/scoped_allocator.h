#pragma once

#include "base_allocator.h"

namespace cppe
{
	struct stack_allocator;

	// buffer storage for allocators designed to used in a call tree context.
	struct stack_allocator_buffer
	{
	public:
		stack_allocator_buffer(const stack_allocator_buffer&) = delete;
		stack_allocator_buffer& operator=(const stack_allocator_buffer&) = delete;

		stack_allocator_buffer() = default;
		stack_allocator_buffer(const std::size_t base_capacity);

	public:
		~stack_allocator_buffer();

		std::size_t capacity() const;
		void		resize(const std::size_t sz);

	protected:
		stack_allocator* m_head = nullptr;

		std::vector<detail::byte_t> m_storage;

		friend struct stack_allocator;
	};

	//-----------------------------------------------------------------------------------------------------------
	struct stack_allocator
	{
	public:
		~stack_allocator();
		stack_allocator(stack_allocator_buffer& alc);
		stack_allocator(stack_allocator_buffer& alc, const std::size_t extra_parent_space);
		// extra_parent_space is the amount of memory left to parent lock in case it wants to allocate some more
	public:
		stack_allocator() = delete;
		stack_allocator(const stack_allocator&) = delete;
		stack_allocator& operator=(const stack_allocator&) = delete;

	public:
		void* alloc_linear(const std::size_t sz);
		//^ allocates multiple elements right after another in the same space; like a push_back()

		void* alloc_unique(const std::size_t sz);
		//^ assume only one allocation the can be resized; like a resize();

		void clear();

	public:
		inline void* operator()(const std::size_t sz)
		{
			return alloc_linear(sz);
		}
		inline std::size_t size() const
		{
			return m_itr;
		}
		inline std::size_t capacity() const
		{
			return m_cap;
		}
		inline std::size_t available_size() const
		{
			return m_cap - m_itr;
		}
		inline void* data() const
		{
			return m_start;
		}

	protected:
		stack_allocator_buffer& m_buffer;
		stack_allocator*		m_parent = nullptr;
		detail::byte_t*			m_start = nullptr;
		std::size_t				m_itr = 0;
		std::size_t				m_cap = 0;
	};

	//-----------------------------------------------------------------------------------------------------------

	template <class T>
	struct scoped_vector : protected stack_allocator
	{
	public:
		inline scoped_vector(stack_allocator_buffer& alc)
			: stack_allocator(alc)
		{
		}
		inline scoped_vector(stack_allocator_buffer& alc, const std::size_t parent_size_offset)
			: stack_allocator(alc, parent_size_offset)
		{
		}

		static inline const T* get_element_ptr(const void* base_ptr, const std::size_t index)
		{
			return reinterpret_cast<const T*>(base_ptr) + index;
		}
		static inline T* get_element_ptr(void* base_ptr, const std::size_t index)
		{
			return reinterpret_cast<T*>(base_ptr) + index;
		}
		static inline std::size_t get_buffer_size(const std::size_t element_count)
		{
			return sizeof(T) * element_count;
		}

	public:
		~scoped_vector()
		{
			for (std::size_t i = 0, s = m_size; i < s; i++)
			{
				get_element_ptr(data(), i)->~T();
			}
		}

	public:
		inline std::size_t size() const
		{
			return m_size;
		}

		inline T& operator [](const std::size_t index)
		{
			CPPE_ASSERT(index < m_size);
			auto* e = get_element_ptr(data(), index);
			CPPE_ASSERT(e != nullptr);
			return *e;
		}
		inline const T& operator [](const std::size_t index) const
		{
			CPPE_ASSERT(index < m_size);
			auto* e = get_element_ptr(data(), index);
			CPPE_ASSERT(e != nullptr);
			return *e;
		}
		inline T& back()
		{
			CPPE_ASSERT(m_size > 0);
			return (*this)[m_size - 1];
		}
		inline const T& back() const
		{
			CPPE_ASSERT(m_size > 0);
			return (*this)[m_size - 1];
		}
	public:

		void push_back(const T& value)
		{
			std::size_t		end = m_size++;
			auto* p = stack_allocator::alloc_unique(get_buffer_size(m_size));
			CPPE_ASSERT(p != nullptr);
			new (get_element_ptr(p, end)) T(value);
		}
		template <class FT>
		void push_back(FT&& value)
		{
			std::size_t		end = m_size++;
			auto* p = stack_allocator::alloc_unique(get_buffer_size(m_size));
			CPPE_ASSERT(p != nullptr);
			new (get_element_ptr(p, end)) T(std::forward<FT>(value));
		}
		void pop_back()
		{
			CPPE_ASSERT(m_size > 0);
			std::size_t new_sz = --m_size;
			get_element_ptr(data(), new_sz)->~T();
			auto* p = stack_allocator::alloc_unique(get_buffer_size(new_sz));
			CPPE_ASSERT(p != nullptr);
		}



	protected:
		std::size_t m_size = 0;
	};

}
