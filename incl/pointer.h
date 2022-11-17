#pragma once

#include "config/cppelements_config.h"

namespace cppe
{
	template <class T>
	struct basic_raw_ptr
	{
	public:
		using class_t = basic_raw_ptr<T>;

		basic_raw_ptr() = default;
		basic_raw_ptr(const class_t&) = default;
		basic_raw_ptr& operator=(const class_t&) = default;

	public:
		cppedecl_finline basic_raw_ptr(T* p)
			: m_px(p)
		{
		}

	public:
		cppedecl_finline T* c_ptr() const
		{
			return m_px;
		}

		template <typename TYPE>
		cppedecl_finline const TYPE* get() const
		{
			CPPE_ASSERT(m_px != nullptr);
			return static_cast<const TYPE*>(m_px);
		}

		template <typename TYPE>
		cppedecl_finline const TYPE* get(const std::size_t index) const
		{
			return get<TYPE>() + index;
		}

		template <typename TYPE>
		cppedecl_finline const TYPE& getr() const
		{
			return *get<TYPE>();
		}

		cppedecl_finline void reset(T* px)
		{
			m_px = px;
		}
		cppedecl_finline void reset(const std::nullptr_t)
		{
			m_px = nullptr;
		}
		cppedecl_finline void reset()
		{
			m_px = nullptr;
		}
		void swap(class_t& other)
		{
			std::swap(m_px, other.m_px);
		}

	protected:
		T* m_px = nullptr;
	};

	//--------------------------------------------------------------------------------------------------------------------------------
	struct raw_ptr : public basic_raw_ptr<void>
	{
	public:
		using base_t = basic_raw_ptr<void>;

	public:
		raw_ptr(void* const);
		raw_ptr(const std::nullptr_t);

		using basic_raw_ptr<void>::operator=;
		raw_ptr&				   operator=(void* const);
		raw_ptr&				   operator=(const std::nullptr_t);

	public:
		raw_ptr() = default;
		raw_ptr(const raw_ptr& p) = default;
		raw_ptr& operator=(const raw_ptr& p) = default;

	public:
		using basic_raw_ptr<void>::reset;

		template <typename TYPE>
		void reset(TYPE* const tp);

	public:
		cppedecl_finline bool operator==(const std::nullptr_t) const
		{
			return m_px == nullptr;
		}
		cppedecl_finline bool operator!=(const std::nullptr_t) const
		{
			return m_px != nullptr;
		}
	};
	//--------------------------------------------------------------------------------------------------------------------------------
	struct const_raw_ptr : public basic_raw_ptr<const void>
	{
	public:
		using base_t = basic_raw_ptr<const void>;

	public:
		const_raw_ptr(void* const);
		const_raw_ptr(const std::nullptr_t);

		using basic_raw_ptr<const void>::operator=;

		const_raw_ptr& operator=(void* const);
		const_raw_ptr& operator=(const std::nullptr_t);

	public:
		const_raw_ptr() = default;
		const_raw_ptr(const const_raw_ptr& p) = default;
		const_raw_ptr& operator=(const const_raw_ptr& p) = default;

	public:
		using basic_raw_ptr<const void>::reset;

		template <typename TYPE>
		void reset(const TYPE* const tp);

	public:
		cppedecl_finline bool operator==(const std::nullptr_t) const
		{
			return m_px == nullptr;
		}
		cppedecl_finline bool operator!=(const std::nullptr_t) const
		{
			return m_px != nullptr;
		}
	};
	//--------------------------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------------------------
	cppedecl_finline raw_ptr::raw_ptr(void* const p)
		: base_t(p)
	{
	}
	cppedecl_finline raw_ptr::raw_ptr(const std::nullptr_t)
	{
	}
	cppedecl_finline raw_ptr& raw_ptr::operator=(void* const p)
	{
		m_px = p;
		return (*this);
	}
	cppedecl_finline raw_ptr& raw_ptr::operator=(const std::nullptr_t)
	{
		m_px = nullptr;
		return (*this);
	}
	template <typename TYPE>
	cppedecl_finline void raw_ptr::reset(TYPE* const tp)
	{
		m_px = static_cast<void*>(tp);
	}
	//--------------------------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------------------------
	cppedecl_finline const_raw_ptr::const_raw_ptr(void* const p)
		: base_t(p)
	{
	}
	cppedecl_finline const_raw_ptr::const_raw_ptr(const std::nullptr_t)
	{
	}
	cppedecl_finline const_raw_ptr& const_raw_ptr::operator=(void* const p)
	{
		m_px = p;
		return (*this);
	}
	cppedecl_finline const_raw_ptr& const_raw_ptr::operator=(const std::nullptr_t)
	{
		m_px = nullptr;
		return (*this);
	}
	template <typename TYPE>
	cppedecl_finline void const_raw_ptr::reset(const TYPE* const tp)
	{
		m_px = static_cast<const void*>(tp);
	}
	//--------------------------------------------------------------------------------------------------------------------------------
}
