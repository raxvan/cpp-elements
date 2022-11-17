#pragma once

#include "array_view.h"
#include "string_pool.h"
#include "vecmap.h"

namespace cppe
{

	struct property_map
	{
	public:
		void LoadArray(const array_view<const char* const>& elements);
		void LoadIni(std::istream& is);
		bool LoadIni(const char* fname);
		void SaveIni(const char* fname) const;

	public:
		int32_t			   getInt32(const string_view& s, const int32_t default_value) const;
		uint32_t		   getUnsigned32(const string_view& s, const uint32_t default_value) const;
		bool			   getBool(const string_view& s, const bool default_value) const;
		float			   getFloat(const string_view& s, const float default_value) const;
		string_view		   getString(const string_view& s, const string_view& default_value) const;
		string_pool_handle get(const string_view& s) const;

	public:
		void			   setInt32(const string_view& s, const int32_t value);
		void			   setUnsigned32(const string_view& s, const uint32_t value);
		void			   setBool(const string_view& s, const bool value);
		void			   setFloat(const string_view& s, const float value);
		string_pool_handle set(const string_view& s, const string_view& value);

	public:
		static int32_t	parseInt32(const string_view& s, const int32_t default_value);
		static uint32_t parseUnsigned32(const string_view& s, const uint32_t default_value);
		static bool		parseBool(const string_view& s, const bool default_value);
		static float	parseFloat(const string_view& s, const float default_value);

	protected:
		void load_expr_internal(const std::string& s);

	protected:
		vecmap<string_pool_handle, string_pool_handle> m_data;
		string_pool									   m_buf;
	};

}
