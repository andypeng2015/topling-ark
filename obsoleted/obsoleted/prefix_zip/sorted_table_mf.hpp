/* vim: set tabstop=4 : */
// common member functions of ZStringTable and SortedTable
// ͬһ������������ const ���� non-const ����Щ����

	TERARK_M_iterator begin() TERARK_M_const { return m_index.begin();   }
	TERARK_M_iterator end()   TERARK_M_const { return m_index.end() - 1; }

	TERARK_M_reverse_iterator rbegin() TERARK_M_const { return TERARK_M_reverse_iterator(end()); }
	TERARK_M_reverse_iterator rend()   TERARK_M_const { return TERARK_M_reverse_iterator(begin()); }

	/**
	 @{
	 @brief �� iterator ת���� handle

	 @note ��Ǳ�Ҫ����Ҫʹ�øú���
	 */
	handle_t to_handle(TERARK_M_iterator iter) TERARK_M_const
	{
		assert(end() != iter);
		return handle_t(iter - this->begin());
	}
	//@}

	/**
	 @{
	 @brief �� handle ת���� iterator
	 @note ��Ǳ�Ҫ����Ҫʹ�øú���
	 */
	TERARK_M_iterator to_iterator(handle_t handle) TERARK_M_const
	{
		assert(handle.t < size());
		return begin() + handle.t;
	}
	//@}

	TERARK_M_const value_type& front() TERARK_M_const
	{
		assert(m_index.size() > 1);
		return (m_index.begin())->val();
	}
	TERARK_M_const value_type& back() TERARK_M_const
	{
		assert(m_index.size() > 1);
		return (m_index.end()-2)->val();
	}

	TERARK_M_const value_type& operator[](difference_type idx) TERARK_M_const
	{
		assert(0 <= idx && idx < m_index.size()-1);
		return m_index[idx].val();
	}

//////////////////////////////////////////////////////////////////////////
	template<class CompatibleCompare>
	TERARK_M_iterator
		lower_bound(TERARK_M_iterator first, TERARK_M_iterator last,
					const key_type& xkey, CompatibleCompare comp) TERARK_M_const
	{
		return TERARK_PZ_FUN(lower_bound)(TERARK_PZ_SEARCH_FUN_ARG_LIST_II);
	}
	template<class CompatibleCompare>
	TERARK_M_iterator
		upper_bound(TERARK_M_iterator first, TERARK_M_iterator last,
					const key_type& xkey, CompatibleCompare comp) TERARK_M_const
	{
		return TERARK_PZ_FUN(upper_bound)(TERARK_PZ_SEARCH_FUN_ARG_LIST_II);
	}
	template<class CompatibleCompare>
	std::pair<TERARK_M_iterator, TERARK_M_iterator>
		equal_range(TERARK_M_iterator first, TERARK_M_iterator last,
					const key_type& xkey, CompatibleCompare comp) TERARK_M_const
	{
		return TERARK_PZ_FUN(equal_range)(TERARK_PZ_SEARCH_FUN_ARG_LIST_II);
	}

//////////////////////////////////////////////////////////////////////////

	template<class CompatibleCompare>
	TERARK_M_iterator lower_bound(const key_type& xkey, CompatibleCompare comp) TERARK_M_const
	{
		return TERARK_PZ_FUN(lower_bound)(TERARK_PZ_SEARCH_FUN_ARG_LIST_BE_1);
	}
	template<class CompatibleCompare>
	TERARK_M_iterator upper_bound(const key_type& xkey, CompatibleCompare comp) TERARK_M_const
	{
		return TERARK_PZ_FUN(upper_bound)(TERARK_PZ_SEARCH_FUN_ARG_LIST_BE_1);
	}
	template<class CompatibleCompare>
	std::pair<TERARK_M_iterator, TERARK_M_iterator>
		equal_range(const key_type& xkey, CompatibleCompare comp) TERARK_M_const
	{
		return TERARK_PZ_FUN(equal_range)(TERARK_PZ_SEARCH_FUN_ARG_LIST_BE_1);
	}

	//////////////////////////////////////////////////////////////////////////

	TERARK_M_iterator lower_bound(const key_type& xkey) TERARK_M_const
	{
		return TERARK_PZ_FUN(lower_bound)(TERARK_PZ_SEARCH_FUN_ARG_LIST_BE_0);
	}
	TERARK_M_iterator upper_bound(const key_type& xkey) TERARK_M_const
	{
		return TERARK_PZ_FUN(upper_bound)(TERARK_PZ_SEARCH_FUN_ARG_LIST_BE_0);
	}
	std::pair<TERARK_M_iterator, TERARK_M_iterator>
		equal_range(const key_type& xkey) TERARK_M_const
	{
		return TERARK_PZ_FUN(equal_range)(TERARK_PZ_SEARCH_FUN_ARG_LIST_BE_0);
	}

	//////////////////////////////////////////////////////////////////////////

	/**
	 @brief �������� iter �ľ���

	  ���������(SortedTable/ZStringTable)��ֻ������
	  ���Ƕ��������࣬������Ҫ�ϸ��ӵļ���

     @note lesser �� larger ������ÿ�� (key, {values}) �ĵ�һ��
	 */
	std::ptrdiff_t distance(TERARK_M_iterator lesser, TERARK_M_iterator larger) TERARK_M_const
	{
		assert(lesser < larger);
		return larger - lesser;
	}

	//////////////////////////////////////////////////////////////////////////

	template<class ValueCompare>
	TERARK_M_iterator
		value_lower_bound(TERARK_M_iterator ilow, TERARK_M_iterator iupp,
						  const value_type& val, ValueCompare vcomp) TERARK_M_const
	{
		assert(ilow < iupp);
		return std::lower_bound(ilow, iupp, val, vcomp);
	}
	template<class ValueCompare>
	TERARK_M_iterator
		value_upper_bound(TERARK_M_iterator ilow, TERARK_M_iterator iupp,
						  const value_type& val, ValueCompare vcomp) TERARK_M_const
	{
		assert(ilow < iupp);
		return std::upper_bound(ilow, iupp, val, vcomp);
	}
	template<class ValueCompare>
	std::pair<TERARK_M_iterator, TERARK_M_iterator>
		value_equal_range(TERARK_M_iterator ilow, TERARK_M_iterator iupp,
						  const value_type& val, ValueCompare vcomp) TERARK_M_const
	{
		assert(ilow < iupp);
		return std::equal_range(ilow, iupp, val, vcomp);
	}

#ifndef TERARK_IS_IN_ZSTRING_TABLE
	/**
	 @brief return first_equal

	  first_equal is the iterator which key equal to the key iter point to
	 */
	TERARK_M_iterator first_equal(TERARK_M_iterator iter) TERARK_M_const
	{
		assert(this->end() != iter);
		return lower_bound_near(m_index.begin(), iter.m_iter, *iter.m_iter,
			CompareNodeKey<key_type, key_compare>(m_comp));
	}

	/**
	 @brief key(iter) �Ƿ��� key(iter+1) ���

	 @note ��� iter ����һ���� this->end()���򷵻� false
	 */
	bool equal_next(TERARK_M_iterator iter) TERARK_M_const
	{
		assert(iter.m_iter < m_index.end() - 1);
		if (iter.m_iter + 2 == m_index.end())
			return false;
		return !m_comp(iter.m_iter->key(), iter.m_iter[1].key());
	}

	size_t goto_next_larger(TERARK_M_iterator& iter) TERARK_M_const
	{
		typename index_t::TERARK_M_iterator iupper =
			upper_bound_near(iter.m_iter, m_index.end()-1, (*iter.m_iter).key(),
				CompareNodeKey<key_type, KeyCompareT>(m_comp));
		size_t count = iupper - iter.m_iter;
		iter.m_iter = iupper;
		return count;
	}

#endif //#ifndef TERARK_IS_IN_ZSTRING_TABLE

	TERARK_M_iterator
		find(TERARK_M_iterator first, TERARK_M_iterator last,
			 const key_type& xkey) TERARK_M_const
	{
		TERARK_M_iterator iter = this->lower_bound(first, last, xkey, m_comp);
		return iter != last && !m_comp(xkey, this->key(iter)) ? iter : end();
	}

	TERARK_M_iterator find(const key_type& xkey) TERARK_M_const
	{
		return find(begin(), end(), xkey);
	}

	/**
	 @{
	 @brief return next_larger iterator

	 next_larger iterator point to the first xkey-value which xkey is larger than the xkey iter point to
	 */
	TERARK_M_iterator next_larger(TERARK_M_iterator iter) TERARK_M_const
	{
		goto_next_larger(iter);
		return iter;
	}
	//@}

	template<class ResultContainer>
	void query_result(const TERARK_M_iterator& iter, ResultContainer& result) TERARK_M_const
	{
		assert(this->end() != iter);
		result.insert(result.end(), iter, next_larger(iter));
	}

#undef TERARK_M_reverse_iterator
#undef TERARK_M_iterator
#undef TERARK_M_const

