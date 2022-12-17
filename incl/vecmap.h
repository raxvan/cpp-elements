#pragma once

#include "config/cppelements_config.h"
#include <algorithm>

namespace cppe
{
	//--------------------------------------------------------------------------
	//--------------------------------------------------------------------------
	template <typename KEY, typename VALUE>
	struct vecmap
	{
	public:
		using class_type = vecmap<KEY, VALUE>;
		using key_t = KEY;
		using value_t = VALUE;
		using key_value_t = std::pair<KEY, VALUE>;
		using vector_t = std::vector<key_value_t>;
		using iterator_t = typename std::vector<key_value_t>::iterator;

		vecmap()
		{
		}
		~vecmap()
		{
		}

	public:
		value_t& insert(const key_value_t& e)
		{
			CPPE_ASSERT(find(e.first) == nullptr);
			return push_lower_bound(m_elements, e).second;
		}
		value_t& insert(key_value_t&& e)
		{
			CPPE_ASSERT(find(e.first) == nullptr);
			auto i = std::lower_bound(m_elements.begin(), m_elements.end(), e, [](const key_value_t& left, const key_value_t& right) { return left.first < right.first; });
			return m_elements.insert(i, std::forward<key_value_t>(e))->second;
		}
		template <typename K, typename V, typename = perfect_forward_t<K, key_t>, typename = perfect_forward_t<V, value_t>>
		value_t& insert(K&& k, V&& t)
		{
			return insert(key_value_t(std::forward<K>(k), std::forward<V>(t)));
		}

	public:
		template< std::convertible_to<KEY> FK,std::convertible_to<VALUE> FV>
		void push_back(FK&& k, FV&& v)
		{
			m_elements.push_back({std::forward<FK>(k),std::forward<FV>(v)});
		}
		void sort()
		{
			std::sort(m_elements.begin(), m_elements.end(), [](const auto& a, const auto& b){
				return a.first < b.first;
			});
		}
	public:
		const value_t* find(const key_t& k) const
		{
			auto i = std::lower_bound(m_elements.begin(), m_elements.end(), k, [](const key_value_t& ii, const key_t& k) { return ii.first < k; });
			if (i != m_elements.end() && i->first == k)
				return &(i->second);
			return nullptr;
		}
		value_t* find(const key_t& k)
		{
			auto i = std::lower_bound(m_elements.begin(), m_elements.end(), k, [](const key_value_t& ii, const key_t& k) { return ii.first < k; });
			if (i != m_elements.end() && i->first == k)
				return &(i->second);
			return nullptr;
		}
		const value_t& find(const key_t& k, const value_t& _default_value) const
		{
			auto i = std::lower_bound(m_elements.begin(), m_elements.end(), k, [](const key_value_t& ii, const key_t& k) { return ii.first < k; });
			if (i != m_elements.end() && i->first == k)
				return (i->second);
			return _default_value;
		}
		template <typename K, typename = perfect_forward_t<K, key_t>>
		value_t& operator[](K&& k)
		{

			return setfunc(k, [&]() { return key_value_t(std::forward<K>(k), value_t()); });
		}

	public:
		const value_t& get(const key_t& k, const value_t& v) const
		{
			auto i = std::lower_bound(m_elements.begin(), m_elements.end(), k, [](const key_value_t& ii, const key_t& k) { return ii.first < k; });
			if (i == m_elements.end() || i->first != k)
				return v;
			return i->second;
		}
		value_t& get(const key_t& k, value_t& v)
		{

			auto i = std::lower_bound(m_elements.begin(), m_elements.end(), k, [](const key_value_t& ii, const key_t& k) { return ii.first < k; });
			if (i == m_elements.end() || i->first != k)
				return v;
			return i->second;
		}

		template <typename FUNCTOR>
		value_t& setfunc(const key_t& k, const FUNCTOR& _func_default_key_value)
		{

			auto i = std::lower_bound(m_elements.begin(), m_elements.end(), k, [](const key_value_t& ii, const key_t& k) { return ii.first < k; });
			if (i == m_elements.end() || i->first != k)
				i = m_elements.insert(i, _func_default_key_value());
			return i->second;
		}
		template <typename OT, typename = perfect_forward_t<OT, value_t>>
		value_t& setdefault(const key_t& k, OT&& v)
		{

			auto i = std::lower_bound(m_elements.begin(), m_elements.end(), k, [](const key_value_t& ii, const key_t& k) { return ii.first < k; });
			if (i == m_elements.end() || i->first != k)
			{
				i = m_elements.insert(i, key_value_t(k, std::forward<OT>(v)));
			}
			else
			{
				i->second = std::forward<OT>(v);
			}
			return i->second;
		}

	public:
		bool remove(const key_t& k) // returns true if key was found
		{

			auto i = std::lower_bound(m_elements.begin(), m_elements.end(), k, [](const key_value_t& ii, const key_t& k) { return ii.first < k; });
			if (!(i != m_elements.end() && key(i) == k))
			{
				m_elements.erase(i);
				return true;
			}
			return false;
		}
		template <typename F>
		bool remove_if(const key_t& k, const F& _func)
		{

			auto i = std::lower_bound(m_elements.begin(), m_elements.end(), k, [](const key_value_t& ii, const key_t& k) { return ii.first < k; });
			if (!(i != m_elements.end() && key(i) == k) && _func(i))
			{
				m_elements.erase(i);
				return true;
			}
			return false;
		}

	public:
		typename std::vector<key_value_t>::const_iterator begin() const
		{

			return m_elements.begin();
		}
		typename std::vector<key_value_t>::const_iterator end() const
		{

			return m_elements.end();
		}
		typename std::vector<key_value_t>::iterator begin()
		{

			return m_elements.begin();
		}
		typename std::vector<key_value_t>::iterator end()
		{

			return m_elements.end();
		}

	public:
		const key_value_t& at(const std::size_t& index) const
		{
			return m_elements[index];
		}
		key_value_t& at(const std::size_t& index)
		{
			return m_elements[index];
		}
		static const key_t& key(const typename std::vector<key_value_t>::const_iterator& index)
		{
			return index->first;
		}
		static const value_t& value(const typename std::vector<key_value_t>::const_iterator& index)
		{
			return index->second;
		}
		static value_t& value(const typename std::vector<key_value_t>::iterator& index)
		{
			return index->second;
		}

		typename std::vector<key_value_t>::iterator erase(const typename std::vector<key_value_t>::iterator& i)
		{
			return m_elements.erase(i);
		}

	public:
		const key_t& key(const std::size_t idx) const
		{
			return m_elements[idx].first;
		}

	public:
		void clear()
		{

			m_elements.clear();
		}
		void swap(class_type& tp)
		{

			m_elements.swap(tp);
		}
		std::size_t size() const
		{
			return m_elements.size();
		}

	protected:
		std::vector<key_value_t> m_elements;
	};
	//--------------------------------------------------------------------------
	//--------------------------------------------------------------------------
	//--------------------------------------------------------------------------
}
