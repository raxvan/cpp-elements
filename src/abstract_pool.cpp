
#include <pools/abstract_pool.h>

namespace cppe
{

	AbstractPoolEntry::~AbstractPoolEntry()
	{
		//empty
	}

	//--------------------------------------------------------------------------------------------------------------------------------

	void AbstractPoolListImpl::link(AbstractPoolEntry* entry)
	{
		CPPE_ASSERT(entry != nullptr);
		if(first == nullptr)
		{
			CPPE_ASSERT(last == nullptr);
			first = entry;
			last = entry;
		}
		else
		{
			last->right = entry;
			entry->left = last;
			last = entry;
		}

	}
	void AbstractPoolListImpl::unlink(AbstractPoolEntry* entry)
	{
		CPPE_ASSERT(entry != nullptr);
		if(first == last)
		{
			CPPE_ASSERT(first == entry);
			CPPE_ASSERT(entry->left == nullptr && entry->right == nullptr);

			first = nullptr;
			last = nullptr;
		}
		else if(entry == first)
		{
			first = entry->right;
			entry->right = nullptr;
			CPPE_ASSERT(first != nullptr);
			first->left = nullptr;
		}
		else if(entry == last)
		{
			last = entry->left;
			entry->left = nullptr;
			CPPE_ASSERT(last != nullptr);
			last->right = nullptr;
		}
		else
		{
			auto* l = entry->left;
			auto* r = entry->right;
			CPPE_ASSERT(l != nullptr && r != nullptr);
			l->right = r;
			r->left = l;
			entry->left = nullptr;
			entry->right = nullptr;
		}
	}
}