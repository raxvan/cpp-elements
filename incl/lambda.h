#pragma once

#include "lambda_utils.h"
#include "allocators/base_allocator.h"

namespace cppe
{
	template <std::size_t CAPTURE_SIZE, class T>
	struct lambda;

	template <std::size_t CAPTURE_SIZE, class R, class... ARGS>
	struct lambda<CAPTURE_SIZE, R(ARGS...)>
	{
	public:
		using class_t = lambda<CAPTURE_SIZE, R(ARGS...)>;
		using storage_t = std::array<detail::byte_t, CAPTURE_SIZE>;

	public:
		template <class RF>
		using func_type = typename std::remove_const<typename std::remove_reference<RF>::type>::type;

	public:
		lambda()
		{
			new (get_raw()) base_impl();
		}
		lambda(const class_t& c)
		{
			c.get_impl()->copy_to(get_storage());
		}

		lambda(class_t&& c)
		{
			c.get_impl()->move_to(get_storage());
		}

		class_t& operator=(const class_t& c)
		{
			get_impl()->~base_impl();
			c.get_impl()->copy_to(get_storage());
			return (*this);
		}

		class_t& operator=(class_t&& c)
		{
			get_impl()->~base_impl();
			c.get_impl()->move_to(get_storage());
			return (*this);
		}

		~lambda()
		{
			get_impl()->~base_impl();
		}

	public:
		void reset()
		{
			get_impl()->~base_impl();
			new (get_raw()) base_impl();
		}
		operator bool() const
		{
			return get_impl()->isValid();
		}

		template <class FF>
		class_t& operator=(FF&& _func)
		{
			// assign
			get_impl()->~base_impl();
			new (this) class_t(std::forward<FF>(_func));
			return (*this);
		}

		template <class FF>
		lambda(FF&& _func)
		{
			using lambda_type_t = func_type<FF>;
			static_assert(sizeof(storage_t) >= sizeof(lambda_type_t), "lambda storage size when created is invalid"); //!
			new (get_raw()) func_impl<lambda_type_t>(std::forward<FF>(_func));
		}

		R operator()(ARGS... args) const
		{
			return get_impl()->call(lambda_args_forward<ARGS>::fwd(args)...);
		}
		void swap(class_t& other)
		{
			storage_t tmp;
			tmp = m_data;
			m_data = other.m_data;
			other.m_data = tmp;
		}

	protected:
	protected:
		struct base_impl
		{
			virtual ~base_impl()
			{
			}
			virtual R call(ARGS...) const
			{
				return R();
			}
			virtual void copy_to(storage_t& _where) const
			{
				new (_where.data()) base_impl(); //!
			}
			virtual void move_to(storage_t& _where)
			{
				new (_where.data()) base_impl(); //!
			}
			virtual bool isValid() const
			{
				return false;
			}
		};
		template <class FUNC>
		struct func_impl : public base_impl
		{
			template <class TFUNC>
			func_impl(TFUNC&& f)
				: m_func(std::forward<TFUNC>(f))
			{
			}

			virtual ~func_impl() override final
			{
			}
			virtual R call(ARGS... args) const override final
			{
				return m_func(lambda_args_forward<ARGS>::fwd(args)...);
			}
			virtual void copy_to(storage_t& _where) const override final
			{
				static_assert(sizeof(_where) >= sizeof(func_impl<FUNC>), "lambda storage size at copy is invalid"); //!
				new (_where.data()) func_impl<FUNC>(m_func);														//!
			}
			virtual void move_to(storage_t& _where) override final
			{
				static_assert(sizeof(_where) >= sizeof(func_impl<FUNC>), "lambda storage size at move is invalid"); //!
				new (_where.data()) func_impl<FUNC>(std::move(m_func));												//!
			}
			virtual bool isValid() const override final
			{
				return true;
			}

		public:
			FUNC m_func;
		};

	public:
		const base_impl* get_impl() const
		{
			return static_cast<const base_impl*>(get_raw());
		}
		base_impl* get_impl()
		{
			return static_cast<base_impl*>(get_raw());
		}

	protected:
		const void* get_raw() const
		{
			return m_data.data();
		}
		void* get_raw()
		{
			return m_data.data();
		}
		const storage_t& get_storage() const
		{
			return m_data;
		}
		storage_t& get_storage()
		{
			return m_data;
		}

	protected:
		storage_t m_data;
	};

	template <class T>
	using lambda32 = lambda<32, T>;

	template <class T>
	using lambda64 = lambda<64, T>;

	template <class T>
	using lambda128 = lambda<128, T>;

	template <class T>
	using lambda256 = lambda<256, T>;

}
