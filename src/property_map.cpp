
#include "property_map.h"
#include "string_helpers.h"
#include <fstream>
#include <string>

namespace cppe
{

	int property_map::getInt32(const string_view& s, const int default_value) const
	{
		string_pool_handle bs = get(s);
		if (bs.size() == 0)
			return default_value;
		return property_map::parseInt32(bs.string_view(), default_value);
	}
	uint32_t property_map::getUnsigned32(const string_view& s, const uint32_t default_value) const
	{
		string_pool_handle bs = get(s);
		if (bs.size() == 0)
			return default_value;
		return property_map::parseUnsigned32(bs.string_view(), default_value);
	}
	bool property_map::getBool(const string_view& s, const bool default_value) const
	{
		string_pool_handle bs = get(s);
		if (bs.size() == 0)
			return default_value;
		return property_map::parseBool(bs.string_view(), default_value);
	}
	float property_map::getFloat(const string_view& s, const float default_value) const
	{
		string_pool_handle bs = get(s);
		if (bs.size() == 0)
			return default_value;
		return property_map::parseFloat(bs.string_view(), default_value);
	}
	string_view property_map::getString(const string_view& s, const string_view& default_value) const
	{
		string_pool_handle bs = get(s);
		if (bs.size() == 0)
			return default_value;
		return bs.string_view();
	}
	//----------------------------------------------------------------------------------------------------------
	void property_map::setInt32(const string_view& s, const int value)
	{
		fixed_string<64> ms;
		set(s, ms.format("%d", value));
	}
	void property_map::setUnsigned32(const string_view& s, const uint32_t value)
	{
		fixed_string<64> ms;
		set(s, ms.format("%u", value));
	}
	void property_map::setBool(const string_view& s, const bool value)
	{
		fixed_string<8> ms;
		set(s, ms.format("%s", value ? "true" : "false"));
	}
	void property_map::setFloat(const string_view& s, const float value)
	{
		fixed_string<64> ms;
		set(s, ms.format("%f", value));
	}
	//----------------------------------------------------------------------------------------------------------
	string_pool_handle property_map::get(const string_view& s) const
	{
		const string_pool_handle* rv = m_data.find(string_pool_handle(s));
		if (rv != nullptr)
			return *rv;
		return string_pool_handle();
	}
	string_pool_handle property_map::set(const string_view& s, const string_view& value)
	{
		string_pool_handle	ss(s);
		string_pool_handle* rv = m_data.find(ss);
		if (rv != nullptr)
		{
			if ((*rv) != ss)
				(*rv) = m_buf.insert(value);
			return (*rv);
		}
		return m_data.insert(std::pair<string_pool_handle, string_pool_handle>(m_buf.insert(s), m_buf.insert(value)));
	}
	//----------------------------------------------------------------------------------------------------------
	int32_t property_map::parseInt32(const string_view& s, const int32_t default_value)
	{
		int32_t r = default_value;
		s.std_parse_int32(r);
		return r;
	}
	uint32_t property_map::parseUnsigned32(const string_view& s, const uint32_t default_value)
	{
		uint32_t r = default_value;
		s.std_parse_unsigned32(r);
		return r;
	}
	bool property_map::parseBool(const string_view& s, const bool default_value)
	{
		if (strutil::equals_lower(s.c_str(), "true"))
			return true;
		if (strutil::equals_lower(s.c_str(), "false"))
			return false;

		int32_t r = default_value ? 1 : 0;
		s.std_parse_int32(r);
		return r != 0;
	}
	float property_map::parseFloat(const string_view& s, const float default_value)
	{
		float r = default_value;
		s.std_parse_float(r);
		return r;
	}
	//----------------------------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------------------------

	void property_map::load_expr_internal(const std::string& s)
	{
		const auto strBegin = s.find_first_of("=");
		if (strBegin == std::string::npos)
			return;

		std::string left = strhelpers::std_trim(s.substr(0, strBegin));
		std::string right = strhelpers::std_trim(s.substr(strBegin + 1, s.size()));

		if (left.size() > 0 && right.size() > 0)
		{
			m_data.push_back(m_buf.insert(string_view(left)), m_buf.insert(string_view(right)));
		}
	}

	void property_map::LoadArray(const array_view<const char* const>& elements)
	{
		for (auto e : elements)
		{
			load_expr_internal(e);
		}
		m_data.sort();
	}
	void property_map::LoadIni(std::istream& is)
	{
		std::vector<std::string> lms;
		std::string				 line;
		while (std::getline(is, line))
		{
			lms.clear();
			strhelpers::std_split(line, ';', lms);
			for (auto& s : lms)
			{
				load_expr_internal(s);
			}
		}
		m_data.sort();
#ifdef _DEBUG
		for (std::size_t i = 0; i < m_data.size(); i++)
		{
			auto& kv = m_data.at(i);
			kv.first.update();
			kv.second.update();
		}
#endif

	}
	bool property_map::LoadIni(const char* fname)
	{
		std::ifstream fin(fname);
		if (fin)
		{
			LoadIni(fin);
			return true;
		}
		return false;
	}
	void property_map::SaveIni(const char* fname) const
	{
		std::ofstream fout(fname);

		for (const auto& i : m_data)
		{
			const auto& key = i.first;
			const auto& value = i.second;
			fout << key.get() << "=" << value.get() << ";" << std::endl;
		}
	}

}
