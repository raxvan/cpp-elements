#pragma once

#include "base_allocator.h"

namespace cppe
{

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

	protected:
		// main buffer data:
		std::atomic<std::size_t> m_itr { 0 };
		std::vector<detail::byte_t>		 m_storage;
	};

	//--------------------------------------------------------------------------------------------------------------------------------

	template <class T>
	// T -> need to look like overflow_allocator
	struct threaded_linear_safe_allocator : public threaded_linear_allocator
	{
		// if allocations overflow, then it defaults to T allocator
	public:
		using class_t = threaded_linear_safe_allocator<T>;
		threaded_linear_safe_allocator(const class_t&) = delete;
		class_t& operator=(const class_t&) = delete;
		~threaded_linear_safe_allocator() = default;

	public:
		threaded_linear_safe_allocator() = default;

	public:
		void clear(); // locked, resets allocators to "free" state.
		void reserve_and_clear();//if reserve_overflow is true overflow is allocated into

		void* alloc(const std::size_t sz); // does what you expect

		bool  owns(const void* mem); // returns true if memory is owned directly
	public:
		inline void* operator()(const std::size_t sz)
		{
			return alloc(sz);
		}

	public:
		// danger zone:
		inline threaded_linear_safe_allocator(const T& alc)
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
		T m_overflow_fallback;
	};

	/*template <class T>
	inline void threaded_linear_safe_allocator<T>::clear(const bool reserve_overflow)
	{
		m_overflow_fallback.intrusive_visit([&](overflow_allocator& alc) {
			threaded_linear_allocator::clear();
			std::size_t max_overflow = alc.max_size();
			alc.clear();

			if (reserve_overflow)
				m_storage.resize(m_storage.size() + max_overflow);
		});
	}
	*/

	template <class T>
	inline void threaded_linear_safe_allocator<T>::clear()
	{
		m_overflow_fallback.intrusive_visit([&](overflow_allocator& alc) {
			threaded_linear_allocator::clear();
			alc.clear();
		});
	}
	template <class T>
	inline void threaded_linear_safe_allocator<T>::reserve_and_clear()
	{
		m_overflow_fallback.intrusive_visit([&](overflow_allocator& alc) {
			this->threaded_linear_allocator::clear_and_resize_extra(alc.size());
			alc.clear();
		});
	}

	template <class T>
	inline void* threaded_linear_safe_allocator<T>::alloc(const std::size_t sz)
	{
		std::size_t itr = m_itr.fetch_add(sz);
		if ((itr + sz) <= m_storage.size())
			return &m_storage[itr];
		else
			return m_overflow_fallback.alloc(sz);
	}
	template <class T>
	inline bool threaded_linear_safe_allocator<T>::owns(const void* mem)
	{
		if (threaded_linear_allocator::owns(mem))
			return true;
		bool r = false;
		m_overflow_fallback.intrusive_visit([&](overflow_allocator& alc) {
			r = alc.owns(mem);
		});
		return r;
	}
}
