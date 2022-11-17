#pragma once

#include "config/cppelements_config.h"

namespace cppe
{

	//--------------------------------------------------------------------------------------------------------------------------------

	template <class V>
	struct lambda_args_forward
	{
		static V fwd(V val)
		{
			return val;
		}
	};
	template <class V>
	struct lambda_args_forward<const V&>
	{
		static const V& fwd(const V& val)
		{
			return val;
		}
	};
	template <class V>
	struct lambda_args_forward<V&>
	{
		static V& fwd(V& val)
		{
			return val;
		}
	};
	template <class V>
	struct lambda_args_forward<V&&>
	{
		static V&& fwd(V&& val)
		{
			return std::forward<V>(val);
		}
	};

	//--------------------------------------------------------------------------------------------------------------------------------

	struct lambda_storage_instance
	{
	};

	//--------------------------------------------------------------------------------------------------------------------------------

	template <class T>
	struct lambda_storage : public lambda_storage_instance
	{
		using class_t = lambda_storage<T>;

		T instance;

	public:
		lambda_storage() = default;

	public:
		template <class TT>
		lambda_storage(const TT& _value)
			: instance(_value)
		{
		}
		template <class TT>
		lambda_storage(TT&& _value)
			: instance(std::forward<T>(_value))
		{
		}

	public:
		lambda_storage(class_t&& other)
			: instance(std::move(other.instance))
		{
		}
		class_t& operator=(class_t&& other)
		{
			instance = std::move(other.instance);
			return (*this);
		}

	protected:
		template <class FB>
		friend struct lambda_traits;

		template <class R, class... ARGS>
		static R invoke_vfunc(const lambda_storage_instance* inst, ARGS... args)
		{
			CPPE_ASSERT(inst != nullptr);
			return cast_ptr<const class_t>(inst)->instance(lambda_args_forward<ARGS>::fwd(args)...);
		}
		template <class R, class... ARGS>
		static R invoke_and_destroy_vfunc(lambda_storage_instance* inst, ARGS... args)
		{
			CPPE_ASSERT(inst != nullptr);
			class_t* ci = static_cast<class_t*>(inst);
			auto	 _ = defer([=]() { ci->~lambda_storage(); });
			return ci->instance(lambda_args_forward<ARGS>::fwd(args)...);
		}
		static void destructor_vfunc(lambda_storage_instance* inst)
		{
			CPPE_ASSERT(inst != nullptr);
			cast_ptr<class_t>(inst)->~lambda_storage();
		}
		template <class A>
		static lambda_storage_instance* copy_constructor_vfunc(const lambda_storage_instance* inst, A& allocator)
		{
			CPPE_ASSERT(inst != nullptr);
			return new (allocator(sizeof(class_t))) class_t(*cast_ptr<const class_t>(inst));
		}
		template <class A>
		static lambda_storage_instance* move_constructor_vfunc(lambda_storage_instance* inst, A& allocator)
		{
			CPPE_ASSERT(inst != nullptr);
			return new (allocator(sizeof(class_t))) class_t(std::move(*cast_ptr<class_t>(inst)));
		}
		static void copy_vfunc(lambda_storage_instance* lhs, const lambda_storage_instance* rhs)
		{
			CPPE_ASSERT(lhs != nullptr && rhs != nullptr);
			*cast_ptr<class_t>(lhs) = *cast_ptr<const class_t>(rhs);
		}
		static void move_vfunc(lambda_storage_instance* lhs, lambda_storage_instance* rhs)
		{
			CPPE_ASSERT(lhs != nullptr && rhs != nullptr);
			*cast_ptr<class_t>(lhs) = std::move(*cast_ptr<class_t>(rhs));
		}
	};

}
