

#include <allocators/base_allocator.h>

namespace cppe
{
	overflow_allocator::~overflow_allocator()
	{
		for (const auto& kv : m_allocations)
			delete[](kv.ptr);
	}

	void overflow_allocator::clear()
	{
		for (const auto& kv : m_allocations)
			delete[](kv.ptr);

		m_allocations.clear();
	}

	void* overflow_allocator::alloc(const std::size_t sz)
	{
		detail::byte_t* r = new detail::byte_t[sz];
		m_allocations.insert({ r, sz });
		return r;
	}

	void overflow_allocator::free(const void* p)
	{
		const detail::byte_t* bp = static_cast<const detail::byte_t*>(p);
		auto				  itr = m_allocations.find({ bp, 0 });

		CPPE_ASSERT(itr != m_allocations.end());

		m_allocations.erase(itr);
	}

	bool overflow_allocator::try_free(const void* p)
	{
		const detail::byte_t* bp = static_cast<const detail::byte_t*>(p);
		auto				  itr = m_allocations.find({ bp, 0 });
		if (itr != m_allocations.end())
		{
			m_allocations.erase(itr);
			return true;
		}
		else
		{
			return false;
		}
	}

	bool overflow_allocator::owns(const void* p) const
	{
		for (const auto& kv : m_allocations)
		{
			if (kv.ptr <= p && p < (kv.ptr + kv.size))
				return true;
		}
		return false;
	}

	std::size_t overflow_allocator::size() const
	{
		std::size_t r = 0;
		for (const auto& kv : m_allocations)
			r += kv.size;
		return r;
	}

	void overflow_allocator::swap(overflow_allocator& other)
	{
		m_allocations.swap(other.m_allocations);
	}

	//-------------------------------------------------------------------------------------------------------------

	threaded_overflow_allocator::~threaded_overflow_allocator()
	{
		overflow_allocator tmp;
		{
			std::lock_guard<std::mutex> _(m_lock);
			tmp.swap(m_base);
		}
	}
	void threaded_overflow_allocator::clear()
	{
		std::lock_guard<std::mutex> _(m_lock);
		m_base.clear();
	}

	void* threaded_overflow_allocator::alloc(const std::size_t sz)
	{
		std::lock_guard<std::mutex> _(m_lock);
		return m_base.alloc(sz);
	}
	void threaded_overflow_allocator::free(const void* p)
	{
		std::lock_guard<std::mutex> _(m_lock);
		m_base.free(p);
	}
	bool threaded_overflow_allocator::try_free(const void* p)
	{
		std::lock_guard<std::mutex> _(m_lock);
		return m_base.try_free(p);
	}
	std::size_t threaded_overflow_allocator::size()
	{
		std::lock_guard<std::mutex> _(m_lock);
		return m_base.size();
	}
	bool threaded_overflow_allocator::owns(const void* p)
	{
		std::lock_guard<std::mutex> _(m_lock);
		return m_base.owns(p);
	}
	//-------------------------------------------------------------------------------------------------------------

}
