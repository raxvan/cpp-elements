#pragma once

#include "config/cppelements_config.h"

namespace cppe
{

	template <typename T>
	struct flags
	{
	public:
		static_assert(std::is_unsigned<T>::value == true, "flags class requires unsigned type");

		using flags_t = T;
		using class_t = flags<T>;

		cppedecl_finline T get() const
		{
			return m_flags;
		}
		cppedecl_finline T get(const T& d) const
		{
			return m_flags & d;
		}

		cppedecl_finline flags()
			: m_flags(0)
		{
		}
		cppedecl_finline flags(const T& f)
			: m_flags(f)
		{
		}

		flags(const class_t& o) // can't make this default for some reason
			: m_flags(o.m_flags)
		{
		}
		class_t& operator=(const class_t& o)
		{
			m_flags = o.m_flags;
			return (*this);
		}

	public:
		cppedecl_finline bool test(const class_t& cf) const
		{
			return test(cf.get());
		}
		cppedecl_finline bool test(const T& d) const
		{
			return get(d) != T(0);
		}
		cppedecl_finline bool test_all(const class_t& cf) const
		{
			return test_all(cf.get());
		}
		cppedecl_finline bool test_all(const T& d) const
		{
			return get(d) == d;
		}
		cppedecl_finline bool is(const T& d) const
		{
			return get() == d;
		}

	public:
		cppedecl_finline operator bool() const
		{
			return !isEmpty();
		}
		cppedecl_finline class_t& operator+=(const T& flag)
		{
			add(flag);
			return (*this);
		}
		cppedecl_finline class_t& operator-=(const T& flag)
		{
			remove(flag);
			return (*this);
		}

	public:
		cppedecl_finline class_t& remove(const class_t& other)
		{
			m_flags = m_flags & ~other.get();
			return (*this);
		}

		cppedecl_finline class_t& add(const class_t& other)
		{
			m_flags = m_flags | other.get();
			return (*this);
		}
		cppedecl_finline bool add_and_test(const class_t& d)
		{
			bool r = test(d);
			add(d);
			return !r;
		}
		// returns true if setting/resetting the flag changed anything
		cppedecl_finline bool reset_and_test(const T& d, const bool condition)
		{
			bool r = test(d);
			reset(d, condition);
			return r != condition;
		}
		cppedecl_finline class_t& reset(const T& d, const bool condition)
		{
			if (condition)
				add(d);
			else
				remove(d);
			return (*this);
		}

	public:
		cppedecl_finline bool isFull() const
		{
			return m_flags == ~(T(0));
		}
		cppedecl_finline bool isEmpty() const
		{
			return m_flags == T(0);
		}
		cppedecl_finline T get_first_free_bit() const
		{
			CPPE_ASSERT(m_flags != ~T(0));
			T x = ~m_flags;
			return (x & ~(x - 1));
		}

	protected:
		T m_flags;
	};
	//------------------------------------------------------------------------------------------
	template <typename TYPE, typename OT>
	cppedecl_finline bool isset(const TYPE v, const OT b)
	{
		return (v & b) != 0;
	}
	template <typename TYPE, typename OT>
	cppedecl_finline void fset(TYPE& v, const OT fs)
	{
		v = v | fs;
	}
	template <typename TYPE, typename OT>
	cppedecl_finline void fclear(TYPE& v, const OT fs)
	{
		v &= ~number_cast<TYPE>(fs);
	}
	template <typename TYPE, typename OT>
	cppedecl_finline void fset(TYPE& v, const OT fs, const bool isset_)
	{
		if (isset_)
			fset(v, fs);
		else
			fclear(v, fs);
	}
	//------------------------------------------------------------------------------------------
	using flags32_t = flags<uint32_t>;
}
