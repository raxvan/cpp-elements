#pragma once

#include "string_view.h"

namespace cppe
{
	struct string_pool;

	//-----------------------------------------------------------------------
	//-----------------------------------------------------------------------
	struct string_info
	{
	public:
		std::size_t offset() const
		{
			return m_offset;
		}
		std::size_t size() const
		{
			return m_size;
		}

	private:
		uint32_t m_offset;
		uint32_t m_size;

		friend struct string_pool_handle;
	};
	//-----------------------------------------------------------------------
	//-----------------------------------------------------------------------
	struct string_pool_handle
	{
	public:
		string_pool_handle(const char* c, const std::size_t sz)
			: m_strbuf(nullptr)
			, m_offset(0)
			, m_size(sz)
			, m_cache(const_cast<char*>(c))
		{
		}
		string_pool_handle(const string_view& as)
			: string_pool_handle(as.c_str(), as.size())
		{
		}
		string_pool_handle(char* const c)
			: string_pool_handle(c, cppe::strutil::length(c))
		{
		}
		template <std::size_t N>
		string_pool_handle(const char (&c)[N])
			: string_pool_handle(&c[0], N - 1)
		{
		}

		string_pool_handle(const string_pool_handle&) = default;

	public:
		string_pool_handle()
			: m_strbuf(nullptr)
			, m_offset(0)
			, m_size(0)
			, m_cache("")
		{
		}
		string_pool_handle(const string_info& info, const string_pool& str_buffer);

	public:
		const char* get() const;

		string_pool_handle get_next() const;

		string_view to_string_view() const;
		std::size_t size() const;
		string_info info() const;

		string_pool_handle& operator=(const string_pool_handle&) = default;

		void update(); // resets cache with get() result

	public:
		bool operator<(const string_pool_handle& other) const
		{
			if (size() == other.size())
				return cppe::strutil::less(get(), other.get(), other.size());
			return size() < other.size();
		}
		bool operator!=(const string_pool_handle& other) const
		{
			if (size() == other.size())
				return !cppe::strutil::equals(get(), other.get(), other.size());
			return true;
		}
		bool operator==(const string_pool_handle& other) const
		{
			if (size() == other.size())
				return cppe::strutil::equals(get(), other.get(), other.size());
			return false;
		}

	public:
		bool operator<(const string_view& s) const
		{
			if (size() == s.size())
				return cppe::strutil::less(get(), s.c_str(), s.size());
			return size() < s.size();
		}
		bool operator==(const string_view& s) const
		{
			if (size() == s.size())
				return cppe::strutil::equals(get(), s.c_str(), s.size());
			return false;
		}
		bool operator!=(const string_view& s) const
		{
			if (size() == s.size())
				return !cppe::strutil::equals(get(), s.c_str(), s.size());
			return true;
		}

	protected:
		friend struct string_pool;

		explicit string_pool_handle(const std::size_t _ind, const std::size_t _size, const string_pool* _strbp, const char* _cache)
			: m_strbuf(_strbp)
			, m_offset(_ind)
			, m_size(_size)
			, m_cache(_cache)
		{
		}
		explicit string_pool_handle(const std::size_t _ind, const std::size_t _size, const string_pool* _strbp)
			: m_strbuf(_strbp)
			, m_offset(_ind)
			, m_size(_size)
		{
		}

	protected:
		const string_pool* m_strbuf = nullptr;
		std::size_t		   m_offset = 0;
		std::size_t		   m_size = 0;
		const char*		   m_cache = nullptr;
	};
	//------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	struct string_pool
	{
	public:
		using string_t = string_pool_handle;

	public:
		string_pool();
		string_pool(std::vector<char>&&);
		string_pool(const std::vector<char>&);
		~string_pool();

		string_t insert(const std::string& s);
		string_t insert(const std::vector<char>& s);

		string_t begin_append();
		void	 append(string_t& s, const char c);
		void	 append(string_t& s, const char* c, const std::size_t char_count);

		string_t insert(const string_view& s);
		string_t insert(const string_view* s, const std::size_t count);
		string_t insert(const char* x);
		string_t insert(const char* x, const std::size_t size);

		string_t get_all();
		string_t get_first() const;
		string_t get_next(const string_t& s) const;

		const char* at(const std::size_t ind) const;

		const char* get(const string_t& buf) const;

		std::size_t count() const; // returns the number of strings
		std::size_t size() const;  // returns the size of the buffer in bytes
		void		clear();	   // clears all content
		void		swap(string_pool&);

	public:
		const char* data() const
		{
			return m_content.data();
		}
		char* data()
		{
			return m_content.data();
		}
		const std::vector<char>& contents() const
		{
			return m_content;
		}

	protected:
		std::vector<char> m_content;
		std::size_t		  m_count;
	};

	//------------------------------------------------------------------------------------------

	inline const char* string_pool_handle::get() const
	{
		if (m_strbuf != nullptr)
			return m_strbuf->at(m_offset);
		return m_cache;
	}
	inline std::size_t string_pool_handle::size() const
	{
		return m_size;
	}
	inline string_info string_pool_handle::info() const
	{
		CPPE_ASSERT(m_offset < std::numeric_limits<uint32_t>::max() && m_size < std::numeric_limits<uint32_t>::max());
		string_info rvalue;
		rvalue.m_offset = uint32_t(m_offset);
		rvalue.m_size = uint32_t(m_size);
		return rvalue;
	}
	inline string_view string_pool_handle::to_string_view() const
	{
		return string_view::make_null_terminated(get(), size());
	}
	//------------------------------------------------------------------------------------------
	inline string_pool::string_t string_pool::insert(const std::string& s)
	{
		return insert(s.c_str(), s.size());
	}
	inline string_pool::string_t string_pool::insert(const std::vector<char>& s)
	{
		return insert(s.data(), s.size());
	}
	inline string_pool::string_t string_pool::insert(const string_view& s)
	{
		return insert(s.c_str(), s.size());
	}
	inline string_pool::string_t string_pool::insert(const char* x)
	{
		CPPE_ASSERT(x != nullptr);
		return insert(x, cppe::strutil::length(x));
	}
	inline const char* string_pool::at(const std::size_t ind) const
	{
		return &m_content[ind];
	}
	inline const char* string_pool::get(const string_pool::string_t& buf) const
	{
		return at(buf.m_offset);
	}
	inline std::size_t string_pool::count() const
	{
		return m_count;
	}
	inline std::size_t string_pool::size() const
	{
		return m_content.size();
	}
	//------------------------------------------------------------------------------------------
}
