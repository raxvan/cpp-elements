
#pragma once

#include <vector>
#include <array>
#ifndef CPPE_DEV_PLATFORM
#	include <utility>
#endif

namespace cppe
{
	enum class cmp_result_t : int
	{
		le = -1,
		eq = 0,
		gt = 1,
	};

	template <typename F>
	struct deferred_call
	{
	public:
		deferred_call() = delete;
		deferred_call(const deferred_call&) = delete;
		deferred_call& operator=(const deferred_call&) = delete;

	public:
		inline deferred_call(F&& f)
			: _func(std::forward<F>(f))
		{
		}

		inline deferred_call(deferred_call&& other)
			: _func(std::move(other._func))
		{
		}

		inline ~deferred_call()
		{
			_func();
		}

	private:
		F _func;
	};

	template <typename F>
	inline deferred_call<F> defer(F&& _f)
	{
		return deferred_call<F>(std::forward<F>(_f));
	}

	template <class T>
	cppedecl_finline uint_fast32_t start_end_distance(const T* _start, const T* _end)
	{
		CPPE_ASSERT(_start <= _end);
		return uint_fast32_t(_end - _start);
	}

}
