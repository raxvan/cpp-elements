

#include <allocators/scoped_allocator.h>

namespace cppe
{
	stack_allocator_buffer::~stack_allocator_buffer()
	{
		CPPE_ASSERT(m_head == nullptr); // can't destroy this container while allocation scopes are using it
	}
	std::size_t stack_allocator_buffer::capacity() const
	{
		return m_storage.size();
	}
	void stack_allocator_buffer::resize(const std::size_t size)
	{
		CPPE_ASSERT(m_head == nullptr); // make sure we don't have scopes
		m_storage.resize(size);
	}
	//-----------------------------------------------------------------------------------------------------------

	stack_allocator::stack_allocator(stack_allocator_buffer& alc)
		: m_buffer(alc)
		, m_parent(alc.m_head)
	{
		alc.m_head = this;
		if (m_parent != nullptr)
		{
			m_start = m_parent->m_start + m_parent->m_itr;
			m_cap = m_parent->available_size();

			m_parent->m_cap = m_parent->m_itr;
		}
		else
		{
			m_start = alc.m_storage.data();
			m_cap = alc.m_storage.size();
		}
	}
	stack_allocator::stack_allocator(stack_allocator_buffer& alc, const std::size_t extra_parent_space)
		: m_buffer(alc)
		, m_parent(alc.m_head)
	{
		alc.m_head = this;
		if (m_parent != nullptr)
		{
			std::size_t total_remaining_size = m_parent->available_size();
			std::size_t offset = 0;

			if (extra_parent_space < total_remaining_size)
			{
				offset = extra_parent_space;
				total_remaining_size -= extra_parent_space;
			}
			else
			{
				offset = total_remaining_size;
				total_remaining_size = 0;
			}

			m_start = m_parent->m_start + m_parent->m_itr + offset;
			m_cap = total_remaining_size;

			m_parent->m_cap = m_itr + offset;
		}
		else
		{
			m_start = alc.m_storage.data();
			m_cap = alc.m_storage.size();
		}
	}
	stack_allocator::~stack_allocator()
	{
		CPPE_ASSERT(m_buffer.m_head == this);
		m_buffer.m_head = m_parent;
		if (m_parent != nullptr)
		{
			m_parent->m_cap += m_cap;
		}
	}

	void* stack_allocator::alloc_unique(const std::size_t sz)
	{
		std::size_t next_itr = sz;
		if (next_itr <= m_cap)
		{
			m_itr = next_itr;
			return m_start;
		}
		return nullptr;
	}

	void* stack_allocator::alloc_linear(const std::size_t sz)
	{
		std::size_t next_itr = m_itr + sz;
		if (next_itr <= m_cap)
		{
			void* result = m_start + m_itr;
			m_itr = next_itr;
			return result;
		}
		return nullptr;
	}
	void stack_allocator::clear()
	{
		m_itr = 0;
	}

}
