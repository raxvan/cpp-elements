#pragma once

#define CPPE_ENABLE_ASSERT

#define CPPE_ENABLE_NOALIAS_DECL

//#define USE_CUSTOM_FAST_STDSTR
//#define USE_CUSTOM_STDSTR

//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

#if defined(CPPE_TESTING)

#	include <ttf.h>
#	define CPPE_ASSERT TTF_ASSERT

#elif defined(CPPE_ENABLE_ASSERT)
//--------------------------------------------------------------------------------------------------------------------------------
#	if defined(CPPE_DEV_PLATFORM)

#		include <devtiny.h>
#		define CPPE_ASSERT DEV_ASSERT

#	else

//--------------------------------------------------------------------------------------------------------------------------------

namespace cllio
{
	extern "C++" void cppe_assert_failed(const char* file, const int line, const char* cond);
}

#		define CPPE_ASSERT(_COND)                                  \
			do                                                      \
			{                                                       \
				if (!(_COND))                                       \
					cppe_assert_failed(__FILE__, __LINE__, #_COND); \
			} while (false)
#		define CPPE_ASSERT_FALSE(CSTR_MSG)                       \
			do                                                    \
			{                                                     \
				cppe_assert_failed(__FILE__, __LINE__, CSTR_MSG); \
			} while (false)
#	endif
//--------------------------------------------------------------------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

#ifndef CPPE_ASSERT

#	define CPPE_ASSERT(...) \
		do                   \
		{                    \
		} while (false)
#endif

#ifndef CPPE_ASSERT_FALSE
#	define CPPE_ASSERT_FALSE(...) \
		do                         \
		{                          \
			CPPE_ASSERT(false);    \
		} while (false)

#endif

//--------------------------------------------------------------------------------------------------------------------------------

#if _MSC_VER

#	define cppedecl_finline __forceinline

#	ifdef CPPE_ENABLE_NOALIAS_DECL
#		define cppedecl_noalias __declspec(noalias)
#	endif

#endif

//--------------------------------------------------------------------------------------------------------------------------------

#ifndef cppedecl_finline
#	define cppedecl_finline inline
#endif
#ifndef cppedecl_noalias
#	define cppedecl_noalias /*no decl*/
#endif

//--------------------------------------------------------------------------------------------------------------------------------
#include "cppelements_types.h"
#include <vector>
