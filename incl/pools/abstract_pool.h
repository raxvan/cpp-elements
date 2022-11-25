
#pragma once

#include "../config/cppelements_config.h"

namespace cppe
{

	//--------------------------------------------------------------------------------------------------------------------------------

	class AbstractPoolEntry //avoid multiple inheritance problem
	{
	protected:
		friend class AbstractEntryContainer;

		AbstractPoolEntry* left = nullptr;
		AbstractPoolEntry* right = nullptr;

	public:
		inline const void* get_entry_memory() const
		{
			//hack to solve multiple inheritance problem
			return this;
		}
		inline void set_entry_memory(const void*)
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
		inline const void* get_entry_memory() const
		{
			return m_entry_ptr;
		}
		inline void set_entry_memory(const void* ep)
		{
			m_entry_ptr = ep;
		}
	private:
		const void* m_entry_ptr;
	};

	//--------------------------------------------------------------------------------------------------------------------------------

	class AbstractEntryContainer
	{
	private:
		AbstractPoolEntry* first = nullptr;
		AbstractPoolEntry* last = nullptr;

	public:
		using base_entry_t = AbstractPoolEntry;

		void add_entry(AbstractPoolEntry* entry);
		void remove_entry(AbstractPoolEntry* entry);
		void clear();

	public:
		template <class F>
		void visit(const F& _func)
		{
			auto* itr = first;
			while(itr)
			{
				auto* next = itr->right;
				_func(itr);
				itr = next;
			}
		}
	public:
		template <class T>
		static T* construct(void * m)
		{
			auto* r = new (m) T{};
			r->set_entry_memory(m);
			return r;
		}
		template <class T>
		static const void* destruct_entry(T* eptr)
		{
			auto* r = eptr->get_entry_memory();
			eptr->~T();
			return r;
		}

	};

	//--------------------------------------------------------------------------------------------------------------------------------

	template <class ALLOCATOR, class CONTAINER = AbstractEntryContainer>
	class AbstractPool : public ALLOCATOR, public CONTAINER
	{
	public:
		using base_entry_t = CONTAINER::base_entry_t;
	public:
		template <class T>
		T* create()
		{
			constexpr std::size_t sz = alloc_size<T>();
			void* m = ALLOCATOR::alloc(sz);
			CPPE_ASSERT(m != nullptr);
			
			auto* r = CONTAINER::construct<T>(m);
			CONTAINER::add_entry(r);
			return r;
		}

		template <class T>
		void release(T * e)
		{
			CPPE_ASSERT(e != nullptr);
			
			CONTAINER::remove_entry(e);
			const void* m = CONTAINER::destruct_entry(e);
			ALLOCATOR::free(m);
		}

	public:
		void clear()
		{
			CONTAINER::visit([&](base_entry_t* e) {
				CPPE_ASSERT(e != nullptr);
				CONTAINER::destruct_entry(e);
			});
			CONTAINER::clear();
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
