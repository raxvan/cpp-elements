#pragma once

#include "memory.h"

namespace cppe
{
	template <typename T>
	struct realloc_copy
	{
		static void copy(T* from, T* to, const std::size_t count)
		{
			std::copy(from, from + count, to);
		}
	};
	template <typename T>
	struct realloc_move
	{
		static void copy(T* from, T* to, const std::size_t count)
		{
			/*
			stdext::checked_array_iterator<T> checked_iterator(to, count);
			std::move(from,from+count,to);
			/*/
			T* end = from + count;
			while (from != end)
			{
				*to++ = std::move(*from++);
			}
			/**/
		}
	};
	//----------------------------------------------------------------------------------------------------------
	template <typename T, typename DEALLOCATOR = std::default_delete<T>>
	struct default_allocator : public DEALLOCATOR
	{
	public:
		using element_type = typename std::remove_all_extents<T>::type;
		using pointer_t = element_type*;
		using class_type = default_allocator<T, DEALLOCATOR>;

	public:
		default_allocator() = default;
		default_allocator(const default_allocator<T, DEALLOCATOR>&) = default;
		default_allocator& operator=(const default_allocator<T, DEALLOCATOR>&) = default;

	public:
		pointer_t alloc_n(const std::size_t n) const
		{
			return new element_type[n];
		}
		template <typename... ARGS>
		pointer_t alloc_c(ARGS&&... args) const
		{
			return new element_type(std::forward<ARGS>(args)...);
		}
		pointer_t alloc() const
		{
			return new element_type();
		}

		cppedecl_finline void dealloc(pointer_t px) const
		{
			(*this)(px);
		}

	public:
		cppedecl_finline void swap(class_type&)
		{
			// empty
		}

	public:
		static void move_elements(pointer_t _from, pointer_t _to, const std::size_t& sz)
		{
			using copy_func = typename std::conditional<std::is_trivially_move_assignable<element_type>::value, realloc_copy<element_type>, realloc_move<element_type>>::type;
			copy_func::copy(_from, _to, sz);
		}
	};
	//----------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------
	// TODO F,T -> variadic
	template <typename F, typename T>
	struct ChainBlockPtr
	{
	private:
		F* m_first;
		T* m_second;

	public:
		using class_t = ChainBlockPtr<F, T>;
		using chain_allocator_t = default_allocator<detail::byte_t[]>;

		ChainBlockPtr()
			: m_first(nullptr)
			, m_second(nullptr)
		{
		}
		ChainBlockPtr(F* const _f, T* const _t)
			: m_first(_f)
			, m_second(_t)
		{
		}
		ChainBlockPtr(const std::nullptr_t, T* const _t, const std::size_t first_count = 1)
			: m_first(reinterpret_cast<F* const>(reinterpret_cast<detail::byte_t* const>(_t) - sizeof(F) * first_count))
			, m_second(_t)
		{
		}
		ChainBlockPtr(F* const _f, const std::nullptr_t, const std::size_t first_count = 1)
			: m_first(_f)
			, m_second(reinterpret_cast<T* const>(reinterpret_cast<detail::byte_t* const>(_f) + sizeof(F) * first_count))
		{
		}
		ChainBlockPtr(const std::size_t fc, const std::size_t tc)
		{

			alloc(fc, tc);
		}
		ChainBlockPtr(const class_t&) = default;
		class_t& operator=(const class_t&) = default;

	public:
		F* first() const
		{

			return m_first;
		}
		T* second() const
		{

			return m_second;
		}

	public:
		cppedecl_finline static std::size_t alloc_size_first(const std::size_t count)
		{
			return sizeof(F) * count;
		}
		cppedecl_finline static std::size_t alloc_size_second(const std::size_t count)
		{
			return sizeof(T) * count;
		}
		void destruct_first(const std::size_t _count)
		{

			F* o = m_first;
			for (std::size_t i = 0; i < _count; i++, o++)
				o->~F();
		}
		void destruct_second(const std::size_t _count)
		{

			T* o = m_second;
			for (std::size_t i = 0; i < _count; i++, o++)
				o->~T();
		}
		void destruct(const std::size_t _count_first, const std::size_t _count_second)
		{

			destruct_first(_count_first);
			destruct_second(_count_second);
		}

	public:
		template <class _AL>
		void* alloc(const std::size_t fc, const std::size_t tc, const _AL& alc = chain_allocator_t {})
		{
			auto size_first = alloc_size_first(fc);
			auto size_sec = alloc_size_second(tc);

			detail::byte_t* mm = alc.alloc_n(size_first + size_sec);

			m_first = reinterpret_cast<F*>(mm);
			m_second = reinterpret_cast<T*>(mm + size_first);
			new (m_first) F[fc];
			new (m_second) T[tc];
			return static_cast<void*>(mm);
		}

		template <class _AL>
		cppedecl_finline void dealloc(detail::byte_t* mm, const _AL& alc = chain_allocator_t {})
		{
			alc.dealloc(mm);
		}
		template <class _AL>
		cppedecl_finline void dealloc(const _AL& alc = chain_allocator_t {})
		{
			dealloc(static_cast<detail::byte_t*>(m_first), alc);
		}
	};
	//----------------------------------------------------------------------------------------------------------
}
