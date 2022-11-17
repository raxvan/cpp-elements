#pragma once

#include "config/cppelements_config.h"

namespace cppe
{
	template <class T>
	struct auto_ptr_factory
	{
	public:
		using refptr_t = std::size_t*;

		refptr_t make_refcount(T*)
		{
			return new std::size_t { 0 };
		}

	public:
		static bool remove_ref(refptr_t r)
		{
			return (--(*r)) == 0;
		}
		static void destroy(T* px, const refptr_t& r)
		{
			delete (px);
			delete (r);
		}

	public:
		void add_ref(refptr_t r)
		{
			(++(*r));
		}
		void swap(auto_ptr_factory<T>&)
		{
			// ownership tracker
		}
	};

	//--------------------------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------------------------

	template <typename T, typename FACTORY = auto_ptr_factory<T>>
	struct auto_ptr : public FACTORY
	{
	public:
		using class_t = auto_ptr<T, FACTORY>;
		using factory_t = FACTORY;
		using refptr_t = typename factory_t::refptr_t;

	public:
		auto_ptr()
			: m_px(nullptr)
			, m_refcount(nullptr)
		{
		}
		auto_ptr(const std::nullptr_t)
			: class_t {}
		{
		}
		auto_ptr(T* px, const std::nullptr_t)
			: m_px(px)
			, m_refcount(nullptr)
		{
		}

	public:
		auto_ptr(T* px, const refptr_t& refcounter)
			: m_px(px)
			, m_refcount(refcounter)
		{
			if (m_refcount != nullptr)
			{
				factory_t::add_ref(m_refcount);
				// track ownership
			}
		}
		auto_ptr(T* px)
			: m_px(px)
		{
			m_refcount = factory_t::make_refcount(px);
			if (m_refcount != nullptr)
			{
				factory_t::add_ref(m_refcount);
				// track ownership
			}
		}
		auto_ptr(const class_t& other)
			: m_px(other.m_px)
			, m_refcount(other.m_refcount)
		{
			if (m_refcount != nullptr)
			{
				factory_t::add_ref(m_refcount);
				// track ownership
			}
		}

	public:
		auto_ptr(class_t&& other)
			: class_t()
		{
			other.swap(*this);
			// track ownership
		}

	public:
		~auto_ptr()
		{
			if (m_refcount != nullptr)
			{
				if (factory_t::remove_ref(m_refcount))
					factory_t::destroy(m_px, m_refcount);
			}
		}

	public:
		class_t& operator=(T* rhs)
		{
			class_t(rhs).swap(*this);
			return *this;
		}
		class_t& operator=(const class_t& rhs)
		{
			class_t(rhs).swap(*this);
			return *this;
		}
		class_t& operator=(class_t&& rhs)
		{
			class_t(std::move(rhs)).swap(*this);
			return *this;
		}

	public:
		inline bool operator!=(const std::nullptr_t) const
		{
			return m_px != nullptr;
		}
		inline bool operator==(const std::nullptr_t) const
		{
			return m_px == nullptr;
		}
		template <typename O, typename FO>
		inline bool operator!=(const auto_ptr<O, FO>& rhs) const
		{
			return get() != rhs.get();
		}
		template <typename O, typename FO>
		inline bool operator==(const auto_ptr<O, FO>& rhs) const
		{
			return get() == rhs.get();
		}
		template <typename O, typename FO>
		inline bool operator<(const auto_ptr<O, FO>& rhs) const
		{
			return get() < rhs.get();
		}
		inline bool operator!=(const T* rhs) const
		{
			return get() != rhs;
		}
		inline bool operator==(const T* rhs) const
		{
			return get() == rhs;
		}
		inline bool operator<(const T* rhs) const
		{
			return get() < rhs;
		}

	public:
		void reset()
		{
			class_t().swap(*this);
		}
		void reset(T* rhs)
		{
			class_t(rhs).swap(*this);
		}
		T& operator*() const
		{
			CPPE_ASSERT(m_px != nullptr);
			return *m_px;
		}
		T* operator->() const
		{
			CPPE_ASSERT(m_px != nullptr);
			return m_px;
		}
		T* get() const
		{
			return m_px;
		}

		refptr_t shared_refcount() const
		{
			return m_refcount;
		}

	public:
		void swap(class_t& other)
		{
			std::swap(m_px, other.m_px);
			std::swap(m_refcount, other.m_refcount);
			factory_t::swap(static_cast<factory_t&>(other));
		}

	protected:
		void detach()
		{
			m_refcount = nullptr;
		}

	protected:
		T*		 m_px;
		refptr_t m_refcount;
	};
}

#if 0
struct autoptr_tester
{
	std::string name;

	autoptr_tester(const std::string & wat)
		:name(wat)
	{
		std::cout << "+" << name << std::endl;
	}
	~autoptr_tester()
	{
		std::cout << "-" << name << std::endl;
	}
};

//--------------------------------------------------------------------------------------------------------------------------------

void test_compile_errors()
{
	{
		auto_ptr<autoptr_tester> a = new autoptr_tester("a");
	}
	{
		auto_ptr<autoptr_tester> b;
		b.reset(new autoptr_tester("b"));
	}
	{
		auto_ptr<autoptr_tester> c = auto_ptr<autoptr_tester>(new autoptr_tester("c"));
	}
	{
		auto_ptr<autoptr_tester> d;
		d = new autoptr_tester("d");
	}
	{
		auto_ptr<autoptr_tester> e;
		e = auto_ptr<autoptr_tester>(new autoptr_tester("e"));
	}
	{
		auto_ptr<autoptr_tester> f0;
		auto_ptr<autoptr_tester> f1 = new autoptr_tester("f");
		f0 = f1;
		f0.reset();
		f0 = auto_ptr<autoptr_tester>(f1);
		f0.swap(f1);
	}

	{
		auto_ptr<autoptr_tester> a = nullptr;
	}
	{
		auto_ptr<autoptr_tester> b;
		b.reset(nullptr);
	}
	{
		auto_ptr<autoptr_tester> c = auto_ptr<autoptr_tester>(nullptr);
	}
	{
		auto_ptr<autoptr_tester> d;
		d = nullptr;
	}
	{
		auto_ptr<autoptr_tester> e;
		e = auto_ptr<autoptr_tester>(nullptr);
	}
	{
		auto_ptr<autoptr_tester> a = new autoptr_tester("m");
		auto_ptr<autoptr_tester> b = std::move(a);
		auto_ptr<autoptr_tester> c;
		c = std::move(b);
		c->name = "mx";
	}
}
#endif
