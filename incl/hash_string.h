#pragma once

#include "config/cppelements_config.h"

namespace cppe
{

	template <typename H, typename BASE_IMPL>
	struct hash_string_impl
	{
	private:
		H		  m_hash;
		BASE_IMPL m_str;

	public:
		using class_t = hash_string_impl<H, BASE_IMPL>;
		using str_impl_t = BASE_IMPL;

	public:
		const char* get() const
		{
			return m_str.get();
		}
		H hash() const
		{
			return m_hash;
		}
		std::size_t size() const
		{
			return m_str.size();
		}
		const str_impl_t& str() const
		{
			return m_str;
		}
		str_impl_t& str()
		{
			return m_str;
		}

	public:
		hash_string_impl(const H& _hash, const char* t, const std::size_t sz) // if precomputed hash
			: m_hash(_hash)
			, m_str(t, sz)
		{
			CPPE_ASSERT(strutil::hash<H>(t, sz) == _hash);
		}
		hash_string_impl(const H& _hash, const str_impl_t& impl) // if precomputed hash
			: m_hash(_hash)
			, m_str(impl)
		{
			CPPE_ASSERT(strutil::hash<H>(m_str.get(), m_str.size()) == _hash);
		}
		hash_string_impl(const H& _hash, str_impl_t&& impl) // if precomputed hash
			: m_hash(_hash)
			, m_str(std::forward<str_impl_t>(impl))
		{
			CPPE_ASSERT(strutil::hash<H>(m_str.get(), m_str.size()) == _hash);
		}

	public:
		hash_string_impl(const str_impl_t& impl) // if precomputed hash
			: m_hash(strutil::hash<H>(impl.get(), impl.size()))
			, m_str(impl)
		{
		}
		hash_string_impl(str_impl_t&& impl) // if precomputed hash
			: m_hash(strutil::hash<H>(impl.get(), impl.size()))
			, m_str(std::forward<str_impl_t>(impl))
		{
		}

	public:
		template <std::size_t N>
		hash_string_impl(const char (&c)[N])
			: class_t(strutil::hash<H>(&c[0], N), &c[0], N - 1)
		{
		}
		template <std::size_t N>
		hash_string_impl(const char (&c)[N], const H& _hash)
			: class_t(_hash, &c[0], N - 1, _hash)
		{
		}
		hash_string_impl(const char* t)
			: class_t(cppe::strutil::hash<H>(t), t, strutil::length(t))
		{
		}
		hash_string_impl()
			: class_t("")
		{
		}

	public:
		cppedecl_noalias bool operator<(const class_t& h) const
		{
			if (m_hash != h.m_hash)
				return m_hash < h.m_hash;
			return m_str < h.m_str;
		}
		cppedecl_noalias bool operator==(const class_t& h) const
		{
			if (m_hash != h.m_hash)
				return false;
			return m_str == h.m_str;
		}
		cppedecl_noalias bool operator!=(const class_t& h) const
		{
			if (m_hash != h.m_hash)
				return true;
			return m_str != h.m_str;
		}
		cppedecl_noalias bool operator==(const std::nullptr_t&) const
		{
			return m_str == nullptr;
		}
		cppedecl_noalias bool operator!=(const std::nullptr_t&) const
		{
			return m_str != nullptr;
		}
	};

	//-----------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------

	// using hstring32 = hash_string_impl<uint32_t,std::string_view>;
}
