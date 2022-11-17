

#include <allocators/threaded_linear_allocator.h>

namespace cppe
{
	void threaded_linear_allocator::set_capacity(const std::size_t size)
	{
		CPPE_ASSERT(m_itr.load() == 0);
		m_storage.resize(size);
	}
	std::size_t threaded_linear_allocator::capacity() const
	{
		return m_storage.size();
	}
	void threaded_linear_allocator::clear()
	{
		m_itr.store(0);
	}
	void threaded_linear_allocator::clear_and_resize_extra(const std::size_t sz)
	{
		m_itr.store(0);
		m_storage.resize(m_storage.size() + sz);
	}

	bool threaded_linear_allocator::owns(const void* mem) const
	{
		if (m_storage.size() == 0)
			return false;
		const detail::byte_t* bm = static_cast<const detail::byte_t*>(mem);
		return bm >= &m_storage[0] && bm <= &m_storage.back();
	}

	void* threaded_linear_allocator::alloc(const std::size_t sz)
	{
		std::size_t itr = m_itr.fetch_add(sz);
		if ((itr + sz) <= m_storage.size())
			return &m_storage[itr];
		return nullptr;
	}

	//--------------------------------------------------------------------------------------------------------------------------------

}
