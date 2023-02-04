#include "string_pool.h"
#include <fstream>

namespace cppe
{

	string_pool_handle string_pool_handle::get_next() const
	{
		CPPE_ASSERT(m_strbuf != nullptr);
		return m_strbuf->get_next(*this);
	}
	string_pool_handle::string_pool_handle(const string_info& info, const string_pool& str_buffer)
		: m_strbuf(&str_buffer)
		, m_offset(info.offset())
		, m_size(info.size())
		, m_cache(str_buffer.at(info.offset()))
	{
	}
	void string_pool_handle::update()
	{
		if (m_strbuf != nullptr)
			m_cache = m_strbuf->at(m_offset);
	}
	//---------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------
	string_pool::~string_pool()
	{
	}
	string_pool::string_pool()
		: m_count(0)
	{
	}
	string_pool::string_pool(std::vector<char>&& v)
		: m_content(std::move(v))
		, m_count(1)
	{
	}
	string_pool::string_pool(const std::vector<char>& v)
		: m_content(v)
		, m_count(1)
	{
	}
	string_pool_handle string_pool::begin_append()
	{
		std::size_t ind = m_content.size();
		m_content.push_back('\0');
		m_count++;
		return string_pool_handle(ind, 0, this);
	}
	void string_pool::append(string_t& s, const char* c, const std::size_t char_count)
	{
		CPPE_ASSERT(m_content.size() > 0 && m_content.back() == '\0');
		m_content.pop_back();
		CPPE_ASSERT((s.size() + s.m_offset) == m_content.size());
		s.m_size += char_count;
		m_content.insert(m_content.end(), c, c + char_count);
		m_content.push_back('\0');
	}
	void string_pool::append(string_t& s, const char c)
	{
		if (m_content.size() > 0)
		{
			CPPE_ASSERT(m_content.back() == '\0');
			m_content.pop_back();
		}
		CPPE_ASSERT((s.size() + s.m_offset) == m_content.size());
		s.m_size++;
		m_content.push_back(c);
		m_content.push_back('\0');
	}
	string_pool_handle string_pool::insert(const string_view* s, const std::size_t count)
	{
		CPPE_ASSERT(s != nullptr && count > 0);
		m_count++;
		std::size_t ind = m_content.size();
		std::size_t total_size = 0;
		for (std::size_t i = 0; i < count; i++)
		{
			total_size += s[i].size();
			m_content.insert(m_content.end(), s[i].begin(), s[i].end());
		}
		m_content.push_back('\0'); // terminating null character;
		return string_pool_handle(ind, total_size, this);
	}
	string_pool_handle string_pool::insert(const char* x, const std::size_t size)
	{
		CPPE_ASSERT(x != nullptr && size > 0);
		m_count++;
		std::size_t ind = m_content.size();
		m_content.insert(m_content.end(), x, x + size);
		m_content.push_back('\0'); // terminating null character;

		return string_pool_handle(ind, size, this);
	}
	string_pool_handle string_pool::insert_file(const char* abs_file_path)
	{
		if (abs_file_path == nullptr || abs_file_path[0] == '\0')
			return {};
		std::ifstream t(abs_file_path);
		if (t.is_open() == false)
			return {};

		std::size_t ind = m_content.size();
		m_content.insert(m_content.end(), (std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
		std::size_t size = m_content.size() - ind;
		m_content.push_back('\0');
		return string_pool_handle(ind, size, this);
	}
	void string_pool::swap(string_pool& o)
	{
		m_content.swap(o.m_content);
		std::swap(m_count, o.m_count);
	}
	void string_pool::clear()
	{

		m_content.clear();
		m_count = 0;
	}
	string_pool::string_t string_pool::get_all()
	{
		CPPE_ASSERT(m_content.size() > 0 && m_content.back() == '\0');
		return string_pool::string_t(0, m_content.size() - 1, this);
	}
	string_pool::string_t string_pool::get_first() const
	{
		if (m_content.size() > 0)
			return string_pool::string_t(0, cppe::strutil::length(m_content.data()), this, m_content.data());

		return string_pool::string_t {};
	}
	string_pool::string_t string_pool::get_next(const string_t& s) const
	{

		std::size_t next_offset = s.m_offset + s.size() + 1;
		if (next_offset < m_content.size())
		{
			const char* str_start = at(next_offset);
			return string_pool::string_t(next_offset, cppe::strutil::length(str_start), this, str_start);
		}

		return string_pool::string_t();
	}

}
