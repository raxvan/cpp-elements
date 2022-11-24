
#pragma once

#include "../config/cppelements_config.h"

namespace cppe
{

	class AbstractPoolEntry
	{
	protected:
		friend class AbstractPoolListImpl;
		AbstractPoolEntry* left = nullptr;
		AbstractPoolEntry* right = nullptr;
	public:
		virtual ~AbstractPoolEntry();
	};


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

		}

		
	public:
		
	};

}
