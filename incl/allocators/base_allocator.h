#pragma once

#include "../config/cppelements_config.h"
#include <mutex>
#include <unordered_set>

namespace cppe
{
	namespace detail
	{
		using byte_t = unsigned char;
		using allocation_info_t = std::pair<byte_t*, std::size_t>;
	}

	template <class T, std::size_t ALIGN = 8>
	constexpr inline std::size_t alligned_alloc_size()
	{
		const std::size_t align = ALIGN;
		std::size_t r = sizeof(T);
		return (r + align - 1) & ~(align - 1); 
	}

	template <std::size_t ALIGN = 8>
	inline std::size_t alligned_alloc_size(const std::size_t sz)
	{
		const std::size_t align = ALIGN;
		std::size_t r = sz;
		return (r + align - 1) & ~(align - 1); 
	}

	struct overflow_allocator
	{
	public:
		overflow_allocator() = default;

		overflow_allocator& operator=(const overflow_allocator&) = delete;

		~overflow_allocator();

	public:
		void clear();

		void* alloc(const std::size_t sz);
		void  free(const void* p);
		bool  try_free(const void* p);

		void swap(overflow_allocator& other);

		std::size_t size() const;
		bool		owns(const void* p) const;

	public: // danger zone
		template <class F>
		// void F(overflow_allocator&)
		inline void intrusive_visit(const F& _func)
		{
			_func(*this);
		}
		inline overflow_allocator(const overflow_allocator&)
			: overflow_allocator()
		{
			// allow constructor but prevent copy because allocator contents should not be shared
		}

	protected:
		struct entry
		{
			const detail::byte_t* ptr;
			std::size_t			  size;
			inline bool			  operator<(const entry& other) const
			{
				return ptr < other.ptr;
			}
			inline bool operator==(const entry& other) const
			{
				return ptr == other.ptr;
			}
			inline bool operator!=(const entry& other) const
			{
				return ptr == other.ptr;
			}
		};
		struct entry_hash
		{
			std::size_t operator()(const entry& e) const
			{
				return std::hash<const detail::byte_t*> {}(e.ptr);
			}
		};
		std::unordered_set<entry, entry_hash> m_allocations;
	};

	//-------------------------------------------------------------------------------------------------------------

	struct threaded_overflow_allocator
	{
	public:
		threaded_overflow_allocator() = default;
		threaded_overflow_allocator& operator=(const threaded_overflow_allocator&) = delete;

		~threaded_overflow_allocator();

	public:
		void clear();

		void* alloc(const std::size_t sz);
		void  free(const void* p);
		bool  try_free(const void* p);

		std::size_t size();
		bool		owns(const void* p);

	public: // danger zone
		template <class F>
		// void F(overflow_allocator&)
		inline void intrusive_visit(const F& _func)
		{
			std::lock_guard<std::mutex> _(m_lock);
			_func(m_base);
		}
		inline threaded_overflow_allocator(const threaded_overflow_allocator&)
			: threaded_overflow_allocator()
		{
			// allow constructor but prevent copy because allocator contents should not be shared
		}

	protected:
		std::mutex		   m_lock;
		overflow_allocator m_base;
	};

	//-------------------------------------------------------------------------------------------------------------

	template <typename T>
	struct shared_overflow_allocator
	{
	public:
		struct internal_allocator_type : public T, public std::enable_shared_from_this<internal_allocator_type>
		{
		};
		using internal_allocator_ptr = std::shared_ptr<internal_allocator_type>;

	public:
		using class_t = shared_overflow_allocator<T>;

		shared_overflow_allocator(const class_t& other) = default;
		shared_overflow_allocator(class_t&& other) = default;
		class_t& operator=(const class_t&) = default;
		class_t& operator=(class_t&&) = default;

	public:
		inline shared_overflow_allocator()
			: m_allocator { std::make_shared<internal_allocator_type>() }
		{
		}

	public:
		inline void clear()
		{
			m_allocator->clear();
		}

		inline void* alloc(const std::size_t sz)
		{
			return m_allocator->alloc(sz);
		}
		inline void free(const void* p)
		{
			m_allocator->free(p);
		}
		inline bool try_free(const void* p)
		{
			return m_allocator->try_free(p);
		}
		inline void swap(class_t& other)
		{
			std::swap(m_allocator, other.m_allocator);
		}

	public: // danger zone
		template <class F>
		// void F(overflow_allocator&)
		inline void intrusive_visit(const F& _func)
		{
			m_allocator->intrusive_visit(_func);
		}

	protected:
		internal_allocator_ptr m_allocator;
	};

}
