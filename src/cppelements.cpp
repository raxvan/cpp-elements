
#include <config/cppelements_config.h>

#ifdef CPPE_ASSERT_ENABLED
#	include <iostream>
#	include <cassert>
#endif

namespace cppe
{

#ifdef CPPE_ASSERT_ENABLED
	void cppe_assert_failed(const char* file, const int line, const char* cond)
	{
		std::cerr << "CPPE_ASSERT failed in " << file << "(" << line << ")> " << cond << std::endl;
		assert(false);
	}
#endif

}
