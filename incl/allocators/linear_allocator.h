#pragma once

#include "base_allocator.h"

namespace cppe
{
	
	struct linear_allocator
	{
	public:
		linear_allocator(const linear_allocator&) = delete;
		linear_allocator& operator=(const linear_allocator&) = delete;
		linear_allocator() = default;
		~linear_allocator() = default;

	public:
		void		set_capacity(const std::size_t size);
		std::size_t capacity() const;
	public:

		void  clear();
		void  clear_and_resize_extra(const std::size_t sz);
		void* alloc(const std::size_t sz); // does what you expect
		bool  owns(const void* mem) const; // returns true if memory is owned directly

	public:
		inline void* operator()(const std::size_t sz)
		{
			return alloc(sz);
		}
		inline void free(const void*)
		{
			//empty
		}
		inline std::size_t size() const
		{
			return m_itr;
		}
	protected:
		// main buffer data:
		std::size_t					m_itr{ 0 };
		std::vector<detail::byte_t> m_storage;
	};

	//--------------------------------------------------------------------------------------------------------------------------------


	// buffer of fixed size which allows concurrent allocation on the same buffer.
	// there is no "free" operation just clear everything
	struct threaded_linear_allocator
	{
	public:
		threaded_linear_allocator(const threaded_linear_allocator&) = delete;
		threaded_linear_allocator& operator=(const threaded_linear_allocator&) = delete;
		threaded_linear_allocator() = default;
		~threaded_linear_allocator() = default;

	public:
		void		set_capacity(const std::size_t size);
		std::size_t capacity() const;

	public:
		void  clear();
		void  clear_and_resize_extra(const std::size_t sz);
		void* alloc(const std::size_t sz); // does what you expect
		bool  owns(const void* mem) const; // returns true if memory is owned directly
	public:
		inline void* operator()(const std::size_t sz)
		{
			return alloc(sz);
		}
		inline void free(const void*)
		{
			//empty
		}
		inline std::size_t size() const
		{
			return m_itr.load();
		}
	protected:
		// main buffer data:
		std::atomic<std::size_t>	m_itr { 0 };
		std::vector<detail::byte_t> m_storage;
	};

	//--------------------------------------------------------------------------------------------------------------------------------

	template <class LALLOC, class FALLOC>
	// T -> need to look like overflow_allocator
	struct safe_linear_allocator : public LALLOC
	{
		// if allocations overflow, then it defaults to T allocator
	public:
		using class_t = safe_linear_allocator<LALLOC, FALLOC>;
		safe_linear_allocator(const class_t&) = delete;
		class_t& operator=(const class_t&) = delete;
		~safe_linear_allocator() = default;

	public:
		safe_linear_allocator() = default;

	public:
		void clear();			  // locked, resets allocators to "free" state.
		void reserve_and_clear(); // if reserve_overflow is true overflow is allocated into

		void* alloc(const std::size_t sz); // does what you expect

		bool owns(const void* mem); // returns true if memory is owned directly

	public:
		inline void free(const void* mem)
		{
			m_overflow_fallback.try_free(mem);
		}
		inline void* operator()(const std::size_t sz)
		{
			return alloc(sz);
		}

		inline std::size_t size() const
		{
			return LALLOC::size() + m_overflow_fallback.size();
		}

	public:
		// danger zone:
		inline safe_linear_allocator(const FALLOC& alc)
			: m_overflow_fallback(alc)
		{
		}
		template <class F>
		// void F(overflow_allocator&)
		inline void intrusive_visit(const F& _func)
		{
			m_overflow_fallback.intrusive_visit(_func);
		}

	protected:
		FALLOC m_overflow_fallback;
	};

	//--------------------------------------------------------------------------------------------------------------------------------

	template <class LALLOC, class FALLOC>
	inline void safe_linear_allocator<LALLOC, FALLOC>::clear()
	{
		LALLOC::clear();
		m_overflow_fallback.intrusive_visit([&](auto& alc) {
			alc.clear();
		});
	}
	template <class LALLOC, class FALLOC>
	inline void safe_linear_allocator<LALLOC, FALLOC>::reserve_and_clear()
	{
		m_overflow_fallback.intrusive_visit([&](auto& alc) {
			LALLOC::clear_and_resize_extra(alc.size());
			alc.clear();
		});
	}

	template <class LALLOC, class FALLOC>
	inline void* safe_linear_allocator<LALLOC, FALLOC>::alloc(const std::size_t sz)
	{
		void* r = this->LALLOC::alloc(sz);
		if(r == nullptr)
			r = m_overflow_fallback.alloc(sz);
		return r;
	}
	template <class LALLOC, class FALLOC>
	inline bool safe_linear_allocator<LALLOC, FALLOC>::owns(const void* mem)
	{
		if (LALLOC::owns(mem))
			return true;
		bool r = false;
		m_overflow_fallback.intrusive_visit([&](auto& alc) { r = alc.owns(mem); });
		return r;
	}
}
