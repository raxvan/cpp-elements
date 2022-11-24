
#pragma once

#include "../config/cppelements_config.h"

namespace cppe
{

	//--------------------------------------------------------------------------------------------------------------------------------

	class AbstractPoolEntry //avoid multiple inheritance problem
	{
	protected:
		friend class AbstractPoolListImpl;
		template <class ALLOCATOR>
		friend class AbstractPool;

	protected:
		AbstractPoolEntry* left = nullptr;
		AbstractPoolEntry* right = nullptr;

	public:
		inline const void* pool_entry_ptr() const
		{
			//hack to solve multiple inheritance problem
			return this;
		}
		inline void set_pool_entry_ptr(const void*)
		{
			//empty
		}
	public:
		virtual ~AbstractPoolEntry();
	};

	//--------------------------------------------------------------------------------------------------------------------------------

	class AbstractPoolEntryEx : public AbstractPoolEntry
	{
	public:
		inline const void* pool_entry_ptr() const
		{
			return m_entry_ptr;
		}
		inline void set_pool_entry_ptr(const void* ep)
		{
			m_entry_ptr = ep;
		}
	private:
		const void* m_entry_ptr;
	};

	//--------------------------------------------------------------------------------------------------------------------------------


	class AbstractPoolListImpl
	{
	protected:
		AbstractPoolEntry* first = nullptr;
		AbstractPoolEntry* last = nullptr;

		void link(AbstractPoolEntry* entry);
		void unlink(AbstractPoolEntry* entry);
	};

	template <class ALLOCATOR>
	class AbstractPool : public ALLOCATOR, public AbstractPoolListImpl
	{
	public:
		template <class T>
		T* create()
		{
			constexpr std::size_t sz = alloc_size<T>();
			void* m = ALLOCATOR::alloc(sz);
			CPPE_ASSERT(m != nullptr);
			T* r = new (m) T();
			r->set_pool_entry_ptr(m);
			link(r);
			return r;
		}

		template <class T>
		void release(T * e)
		{
			CPPE_ASSERT(e != nullptr);
			unlink(e);
			e->~T();
			ALLOCATOR::free(e->pool_entry_ptr());
		}

	public:
		void clear()
		{
			AbstractPoolEntry* itr = first;
			while(itr != nullptr)
			{
				auto* next = itr->right;
				itr->~AbstractPoolEntry();
				itr = next;	
			}
			first = last = nullptr;
			ALLOCATOR::clear();
		}
		
	protected:
		template <class T>
		constexpr static std::size_t alloc_size()
		{
			const std::size_t align = 8;
			std::size_t r = sizeof(T);
			return r & ~(align - 1); 
		}
	};

}
