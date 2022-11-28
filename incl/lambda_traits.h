#pragma once

#include "lambda_utils.h"

namespace cppe
{
	//----------------------------------------------------------------------------------------------------------------

	template <class R, class... ARGS>
	struct lambda_traits<R(ARGS...)>
	{
	public:
		// using func_sig_t = typename std::remove_const<typename std::remove_reference<F>::type>::type;

		using func_prototype = R(ARGS...);
		using func_t = func_prototype*;

		using call_operator_func = R(const lambda_storage_instance*, ARGS...);
		using call_and_destroy_func = R(lambda_storage_instance*, ARGS...);
		using destructor_func = void(lambda_storage_instance*);
		template <class A>
		using copy_constructor_func = lambda_storage_instance*(const lambda_storage_instance*, A&);
		template <class A>
		using move_constructor_func = lambda_storage_instance*(lambda_storage_instance*, A&);
		using copy_func = void(lambda_storage_instance*, const lambda_storage_instance*);
		using move_func = void(lambda_storage_instance*, lambda_storage_instance*);

		template <class F>
		using clean_func_type = typename std::remove_const<typename std::remove_reference<F>::type>::type;
	public:
		struct lambda_ptr
		{
			lambda_storage_instance* capture = nullptr;
			call_and_destroy_func*	 func = nullptr;

			inline R operator()(ARGS... args)
			{
				return (*func)(capture, args...);
			}
		};

	public:
		template <class ALLOCATOR, class FUNCTOR>
		static lambda_ptr build_unique_call(ALLOCATOR& allocator, FUNCTOR&& _func)
		{
			using functor_t = clean_func_type<FUNCTOR>;
			using storage_t = lambda_storage<functor_t>;
			lambda_ptr result;

			result.capture = new (allocator(sizeof(storage_t))) storage_t { std::forward<FUNCTOR>(_func) };
			result.func = &storage_t::template invoke_and_destroy_vfunc<R, ARGS...>;

			return result;
		}

		template <class BASE, class FUNCTOR>
		static virtual_lambda<BASE, clean_func_type<FUNCTOR>, R(ARGS...)> make_virtual_lambda(FUNCTOR&& _func)
		{
			return virtual_lambda<BASE, clean_func_type<FUNCTOR>, R(ARGS...)>{_func};
		}
	};

}
