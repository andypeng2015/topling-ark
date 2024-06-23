/* vim: set tabstop=4 : */
#ifndef __terark_multi_way_table_h__
#define __terark_multi_way_table_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
# pragma warning(push)
# pragma warning(disable: 4018) // '<' : signed/unsigned mismatch
#endif

#include <boost/type_traits/remove_pointer.hpp>
#include <boost/ref.hpp>

#include "packed_table.hpp"
#include "sorted_table.hpp"
#include "biway_table.hpp"
#include "multi_sorted_table.hpp"
#include "serialize_common.hpp"

#include <terark/set_op.hpp>

namespace terark { namespace prefix_zip {

/**
 @defgroup prefix_zip MultiWayTable ʹ�ô洢���٣��ֿ��Զ�̬����

 @brief ����Ҫ��õ����� MultiWayTable ������鿴��ϸ���ݡ�

 @{
 */

/**
  @class MultiWayTable
  @brief ʹ�ô洢���٣��ֿ��Զ�̬������ַ���ӳ���

  -# ���� SortedTable ʹ�ô洢�����ٵ��ŵ㣬���� PackedTable ���Զ�̬����Ԫ�ص��ŵ�
  	 - һ����Խ�Լ 70\% ���ڴ棬�� bbs ���������У���Լ�� 73\% ���ڴ棬250����ʣ������� 35M �ڴ�(MultiWayTableռ�õ����ڴ�)

  -# �ڲ�ʵ�ֲ��ö�� SortedTable ��Ϊ�̶�������һ�� PackedTable ��Ϊ��������

  -# �� PackedTable Ҫ���ʱ���� PackedTable ѹ����һ�� SortedTable ����Ϊһ���µĹ̶�������\n
     Ȼ�� PackedTable ��գ��Ϳ��Լ���������Ԫ���ˡ�

  -# FlagLength ��ѹ���ַ����ı�־�ֽ�����Ŀǰ����ʹ�� 2, 3, 4. @see ZString

  -# ʹ�÷��� stl ��׼�Ľӿ������� MultiWayTable\n
	 MultiWayTable::iterator �Ǵ���󣬶��������� stl ����һ����������С����\n
	 MultiWayTable::iterator �� ForwardIterator��ֻ����ǰ���������

  -# ʹ�� iterator ���Բ��ҡ����롢���� MultiWayTable\n
	 ���� MultiWayTable ʱ��ʹ�ö�·�鲢�㷨�ڶ���̶�����������С��Ԫ�أ����Կ���ǰ��

  -# MultiWayTable Ŀǰ���ṩɾ��Ԫ�صĹ��ܣ�δ���İ汾���ܿ�������ɾ�����ܡ���ʵ���Ͽ���ֻ�Ǵ���ɾ����ǡ�

  -# Ŀǰ�İ汾�޷�������� reverse_iterator��δ���İ汾���ܿ������� reverse_iterator

  -# �������ڴ��� MultiWayTable ��Ϊ���ٸ��̶������������������ж���Ԫ�أ��洢ʱ���Ǵ�Ϊ\n
     һ����� SortedTable������ʱ��Ҳ����Ϊһ��ֻ��һ����Ĺ̶������� SortedTable��Ȼ���û����Բ�����Ԫ��

  -# ����ʱ�������ʹ�� find ��ȷ���ң�������ʹ�� equal_range/lower_bound/upper_bound �����ҷ��������ķ�Χ���������ͬǰ׺��

 @see BiWayTable, PackedTable, SortedTable, ZStringTable, MultiSortedTable, MultiZStringTable
 */
//@}

/**
 @name tags ������ʶ����������
 @{
 */

/**
 @brief (key, value) һһ��Ӧ
 */
struct UniqueKey {};

/**
 @brief һ�� key ���Զ�Ӧ��� value
 */
struct NonUniqueKey {};

/**
 @brief һ�� key ���Զ�Ӧ��� value

  ����һ�� key ��Ӧ�� value ���ϵĴ�С�����Ż�
  -# �� value ���Ͻ�Сʱ���� (key, value) ������ظ���ӳ����� key ӳ��Ϊ value
  -# �� value ���Ͻϴ��ǣ��� (key, {value}) ���벻�ظ���ӳ����� key ӳ��� value �ļ���({value})
 */
struct BiWayTableKey {};

/**
 @brief һ�� key ���Զ�Ӧ���� value

  -# ���ǽ� (key, value) ת���� (key, {value})
  -# ������ÿ�� key ��ӳ����� value �����
  -# ʱ�����ܱ� BiWayTableKey Ҫ��
  -# �ռ����ܱ� BiWayTableKey Ҫ��һЩ��һ������£�
 */
struct MultiIndexedKey {};

/**
 @brief һ�� key ���Զ�Ӧ���� value �����ҽ� value ���Ͻ���ѹ��

  -# һ������£���� value �� id ֮������ݣ����Ұ���С���򣬾Ϳ���ʹ�ò�ֱ������ѹ��
  -# ���⣬ValueZippedKey ��ζ�ſ��Խ� value �����ļ��У�ֻ�� key ��Ϊ���������ڴ���
 */
struct ValueZippedKey {};

//@}

/**
 @brief BiWayTableKey �ı���
 */
typedef BiWayTableKey LargeMultiKey;

//////////////////////////////////////////////////////////////////////////

template<class BaseClassT>
class MultiWayTable_Impl; // forward declaration

template<class BaseClassT>
class MultiWayTable_Impl_iterator; // forward declaration

template<class BaseClassT>
class MultiWayTable_Impl_const_iterator; // forward declaration

template<class BaseClassT>
class MultiWayTable_Impl_reverse_iterator; // forward declaration

template<class BaseClassT>
class MultiWayTable_Impl_const_reverse_iterator; // forward declaration

template<class StreamT, class DataInputT, class DataOutputT>
struct ValueZipped
{
	BOOST_STATIC_CONSTANT(bool, is_value_zipped = true); // for validation in MultiWayTable_Impl

	typedef StreamT     stream_t;
	typedef DataInputT  data_input_t;
	typedef DataOutputT data_output_t;
};

struct MultiWayTable_Impl_dump_header : public Table_dump_header
{
	uint32_t fixedIndexCount;
	uint32_t m_optimizeSegmentCount;

	MultiWayTable_Impl_dump_header()
	{
		fixedIndexCount = 0;
		m_optimizeSegmentCount = 10;
	}

	template<class SrcTable>
	explicit MultiWayTable_Impl_dump_header(const SrcTable& src)
		: Table_dump_header(src)
	{
		fixedIndexCount = src.fixed().size();
		m_optimizeSegmentCount = src.optimizeSegmentCount();
	}

	DATA_IO_REG_LOAD_SAVE_V(MultiWayTable_Impl_dump_header, 1,
		& vmg.template base<Table_dump_header>(this)
		& fixedIndexCount
		& m_optimizeSegmentCount
		)
};

#include "multi_way_table_iter.hpp"

template<class BaseClassT>
class MultiWayTable_Impl : public BaseClassT
{
	typedef typename BaseClassT::key_category key_category;
	BOOST_STATIC_ASSERT((
		boost::is_same<key_category,     UniqueKey>::value ||
		boost::is_same<key_category,  NonUniqueKey>::value ||
		boost::is_same<key_category, BiWayTableKey>::value ||
		boost::is_same<key_category,MultiIndexedKey>::value ||
		boost::is_same<key_category,ValueZippedKey>::value ||
	//	KeyCategory::is_value_zipped ||
		0));

public:
	BOOST_STATIC_CONSTANT(bool, IsUniqueKey = (boost::is_same<key_category, UniqueKey>::value));

	typedef	typename BaseClassT::increment_index increment_index;
	typedef typename BaseClassT::fixed_index	 fixed_index;
	typedef typename BaseClassT::compare_t		 compare_t;
	typedef typename BaseClassT::key_type		 key_type;
	typedef typename BaseClassT::value_type		 value_type;
	typedef typename BaseClassT::dump_header	 dump_header;

	typedef std::vector<fixed_index>		 fixed_index_vec;

	typedef MultiWayTable_Impl_iterator<BaseClassT>			    iterator;
	typedef MultiWayTable_Impl_const_iterator<BaseClassT> const_iterator;

	typedef MultiWayTable_Impl_reverse_iterator<BaseClassT>			    reverse_iterator;
	typedef MultiWayTable_Impl_const_reverse_iterator<BaseClassT> const_reverse_iterator;

	typedef ptrdiff_t difference_type;

	typedef typename fixed_index::size_type	size_type;
	typedef std::pair<iterator, iterator>	range_t;
	typedef std::pair<const_iterator, const_iterator> const_range_t;
	typedef std::pair<iterator, bool>		iter_bool_t;

	typedef boost::integral_constant<bool, IsUniqueKey> is_unique_key_t;
	typedef typename boost::mpl::if_c<IsUniqueKey, iter_bool_t, iterator>::type insert_return_t;

protected:
	typedef typename increment_index::iterator		   i_iter_t;
	typedef typename increment_index::const_iterator i_c_iter_t;
	typedef typename     fixed_index::iterator		   f_iter_t;
	typedef typename     fixed_index::const_iterator f_c_iter_t;

public:

//////////////////////////////////////////////////////////////////////////

	/**
	 @brief ���캯��

	 @param[in] maxPoolSize ���� StringPool �����ߴ硾������
	 @param[in] comp  �Ƚ���
	 @param[in] initialFixedIndexCapacity �̶������ĳ�ʼ����С����Ϊ����һ���̶������Ŀ����ǳ���\n
				���ֵӦ������㹻������̶�������������������󡾵��¼���Ŀ�������128 �������㹻����
	 */
	explicit MultiWayTable_Impl(uint32_t maxPoolSize,
						   compare_t comp = compare_t(),
						   size_type initialFixedIndexCapacity = 512)
		: m_maxPoolSize(maxPoolSize), m_inc(maxPoolSize, comp), m_comp(comp)
	{
		m_fixed.reserve(initialFixedIndexCapacity);
		m_key_count = 0;
		m_val_count = 0;
		m_incPos = 0;
//		m_startPos = 0;
		m_synchronized_segments = 0;
		m_optimizeSegmentCount = 10;
	}

	void setMaxPoolSize(uint32_t maxPoolSize)
	{
		m_maxPoolSize = m_maxPoolSize;
		m_inc.setMaxPoolSize(maxPoolSize);
	}

	void setOptimizeSegmentCount(uint32_t optimizeSegmentCount)
	{
		if (optimizeSegmentCount > 512)
			m_optimizeSegmentCount = 512;
		else
			m_optimizeSegmentCount = optimizeSegmentCount;
	}
	uint32_t optimizeSegmentCount() const { return m_optimizeSegmentCount; }

	compare_t comp() const { return m_comp; }

	/**
	 @brief ���ر��� value_type Ԫ�ص��������� val_count��

	 �� KeyCategory == UniqueKey ʱ��size() == key_count(), ����key_count == val_count
	 */
	size_type size() const
	{
		patch_count();
		return m_val_count;
	}
	size_type val_count() const { return size(); }

	/**
	 @brief ���ر��м����������� key_count��

	 �� KeyCategory == UniqueKey ʱ��size() == key_count(), ����key_count == val_count
	 */
	size_type key_count() const
	{
		patch_count();
		return m_key_count;
	}

	//! same as key_count
//	size_type distinct_size() const { return key_count(); }

	bool empty() const
	{
	//	patch_count(); //! һЩ���Ի���øú������ú�������Ҫ patch_count
		return 0 == m_val_count;
	}

	void clear()
	{
		m_fixed.clear();
		m_inc.clear();
		m_key_count = 0;
		m_val_count = 0;
		m_incPos = 0;
//		m_startPos = 0;
		m_synchronized_segments = 0;
	}

// 	/**
// 	 @brief add other table to this
//
// 	 @note this function will destroy 'other'
// 	 */
// 	void add(MultiWayTable_Impl& other)
// 	{
// 		other.zip_increment();
// 		typename fixed_index_vec::iterator j = other.m_fixed.begin();
// 		for (; j != other.m_fixed.end(); ++j)
// 		{
// 			m_fixed.push_back(fixed_index(0));
// 			m_fixed.back().swap(*j);
// 		}
// 	}

	/**
	 @brief ֻ���� key_type=std::string ʱ����
	 */
	void set_unzip_prefer(unzip_prefer uzp)
	{
		BaseClassT::set_unzip_prefer(m_fixed, uzp);
	}

	template<class CompareValueT>
	size_t sort_values_strict(const iterator& iter, CompareValueT compVal)
	{
		BOOST_STATIC_ASSERT(!is_unique_key_t::value); // unique key has no sort

		std::vector<value_type> vals; // sort vals for cache performance
		std::vector<boost::reference_wrapper<value_type> > refs;
		if (iter.m_is_in_fixed)
		{
			for (int i = 0; i < int(m_fvec.size()); ++i)
			{
				if (!comp()(xkey, m_fvec[i].xkey))
				{
					m_fixed[iter.m_fvec[i].indx].copy_values(iter.m_fvec[i].iter, vals);
					m_fixed[iter.m_fvec[i].indx].copy_values(iter.m_fvec[i].iter, refs);
				}
			}
		}
		if (!m_inc.empty())
		{
			if (m_inc.end() != iter.m_iinc && !comp()(xkey, m_inc.raw_key(m_inc)))
			{
				m_inc.copy_values(iter.m_iinc, vals);
				m_inc.copy_values(iter.m_iinc, refs);
			}
		}
		std::sort(vals.begin(), vals.end(), compVal); // sort vals for cache performance
		std::copy(vals.begin(), vals.end(), refs.begin());
	}

	/**
	 @name sort values
	 @{
	 @brief sort values
	 */
	template<class CompareValueT>
	size_t sort_values(const iterator& iter, CompareValueT compVal)
	{
		BOOST_STATIC_ASSERT(!is_unique_key_t::value); // unique key has no sort

		for (int i = 0; i != iter.m_fvec.size(); ++i)
		{
			m_fixed[iter.m_fixed[i].indx].sort_values(iter.m_fixed[i].iter, compVal);
		}
		m_inc.sort_values(iter.m_iinc, compVal);
	}

	template<class CompareValueT>
	void sort_values(CompareValueT compVal)
	{
		BOOST_STATIC_ASSERT(!is_unique_key_t::value); // unique key has no sort
		for (int i = 0; i != m_fixed.size(); ++i)
		{
			m_fixed[i].sort_values(compVal);
		}
		m_inc.sort_values(compVal);
	}
	//@}

#define TERARK_M_const const
#define TERARK_M_iterator const_iterator
#define TERARK_M_reverse_iterator const_reverse_iterator
#include "multi_way_table_mf.hpp"

#define TERARK_M_const
#define TERARK_M_iterator iterator
#define TERARK_M_reverse_iterator reverse_iterator
#include "multi_way_table_mf.hpp"

	/**
	 @name collection query functions
	 @{
	 �ɴ���һ����ѡ�� compare
	 ���� CompatibleCompare ʱ��ƥ��� xkey ֵ�������ṩ�� xkey ֵ�� m_comp �ϱȽ�Ϊ��ͬ
	 */
// 	template<class CompatibleCompare>
// 	std::vector<value_type>
// 	query_result(const key_type& xkey, CompatibleCompare& comp) const
// 	{
// 		std::vector<value_type> result;
// 		query_result(xkey, result, comp);
// 		return result;
// 	}

	template<class ResultContainer, class CompatibleCompare>
	void query_result(const key_type& xkey, ResultContainer& result, CompatibleCompare& comp) const
	{
		for (typename fixed_index_vec::const_iterator i = m_fixed.begin(); i != m_fixed.end(); ++i)
		{
			(*i).query_result(xkey, result, comp);
		}
		typename increment_index::const_range_t ii = m_inc.equal_range(xkey, comp);
		for (i_c_iter_t j = ii.first; j != ii.second; ++j)
		{
			result.push_back(*j);
		}
	}

	std::vector<value_type> query_result(const key_type& xkey) const
	{
		std::vector<value_type> result;
		query_result(xkey, result);
		return result;
	}

	template<class ResultContainer>
	void query_result(const key_type& xkey, ResultContainer& result) const
	{
		for (typename fixed_index_vec::const_iterator i = m_fixed.begin(); i != m_fixed.end(); ++i)
		{
			(*i).query_result(xkey, result);
		}
		typename increment_index::const_range_t ii = m_inc.equal_range(xkey, m_comp);
		result.insert(result.end(), ii.first, ii.second);
	}
	//@} query_result

	template<class ResultContainer>
	void query_result(const const_iterator& iter, ResultContainer& result) const
	{
		assert(this == iter.m_owner);
		iter.query_result(result);
	}

	//-------------------------------------------------------------------------------

	//! is last value of key
	bool is_last_vok(const const_iterator& iter) const
	{
		return false;
	}

	/**
	 * @brief ���� (key, defaultValue)�������� MultiWayTable_Impl �� defaultValue ������
	 *
	 * ��� key �Ѿ����ڣ�ֻ�򵥵ط������Ӧ�� value
	 * ������������󣬽���ʹ�øú��������� key-value
	 * �ú�����ִ���ٶȱ� insert ��
	 *
	 * for small value_type, operator[] is more efficient than insert
	 * but if you want to test the (key, val) is existed or not, use insert!
	 * it is more efficient than first find, and then use operator[] to insert (key, val)
	 * maybe insert element, could not const
	 */
	value_type& operator[](const key_type& xkey)
	{
		BOOST_STATIC_ASSERT(is_unique_key_t::value);
		bool successed;
		return insert_unique(xkey, successed);
	}

	/**
	 * @name ���� key-value
	 * @{
	 * @brief ���� key-value
	 *
	 * @return ��� xkey �����ڣ�retVal.second Ϊ true ��retVal.first �Ǹ�Ԫ�ص� iterator\n
	 * 		   ��� xkey �Ѵ��ڣ�retVal.second Ϊ false��retVal.first ���Ѵ��ڵ��Ǹ�Ԫ�ص� iterator
	 * ��Ϊ���� iterator Ҫ�ϴ�������������Ƚ������� operator[] �� fast_insert ������
	 */
	insert_return_t insert(const std::pair<key_type, value_type>& kv)
	{
		return insert_aux(kv.first, kv.second, is_unique_key_t());
	}

	insert_return_t insert(iterator pos, const std::pair<key_type, value_type>& kv)
	{
		return insert_aux(kv.first, kv.second, is_unique_key_t());
	}

	insert_return_t insert(const key_type& xkey, const value_type& val)
	{
		return insert_aux(xkey, val, is_unique_key_t());
	}

	template<class IterT>
	void insert(const key_type& xkey, const IterT& first, const IterT& last);
	//@}

	/**
	 * @brief ���ٲ���
	 *
	 * ��������޸� xkey ԭ�ȶ�Ӧ��ֵ������ֻ�뱣֤�� xkey ���뵽������
	 * ����ʹ����������������ٶ����
	 * @return void  ����Ϊ���ű�ʱ���������ǳɹ���
	 *				 ���ᵼ�� m_val_count �� m_key_count ��ͬ�������������ͬ�������ʵ���ʱ���Զ�����
	 *
	 * @return true  ����Ϊӳ���ʱ������ true ��ʾ����ɹ���
	 *		         ���� false ��ʾ xkey �Ѵ��ڣ�xkey ԭ�ȶ�Ӧ��ֵδ���޸�
	 */
	typename boost::mpl::if_c<IsUniqueKey, bool, void>::type
		fast_insert(const key_type& xkey, const value_type& val)
	{
		return fast_insert_aux(xkey, val, is_unique_key_t());
	}
	template<class IterT>
	void fast_insert(const key_type& xkey, const IterT& first, const IterT& last);

//////////////////////////////////////////////////////////////////////////
	key_type key(const_iterator iter) const { return iter.key(); }
	key_type key(iterator iter) const { return iter.key(); }

	/**
	 @name unusually used functions:
	 @{
	 */

	/// ����������ѹ����һ���̶�����
	void zip_increment();

	/// �̶�����������
	size_type fixed_index_count() const { return m_fixed.size(); }

	/// ����������Ԫ�ص�����
	size_type incr_index_size()   const { return m_inc.size(); }

	/// �̶������� string ��ѹ������ܳߴ�
	size_t zip_key_size() const
	{
		size_t size = 0;
		for (int i = 0; i < m_fixed.size(); ++i)
			size += m_fixed[i].total_pool_size();
		return size;
	}

	size_t used_pool_size() const { return zip_key_size() + m_inc.used_pool_size(); }

	//! �̶������� string ����ѹ����ܳߴ�
	size_t unzip_key_size() const
	{
		size_t size = 0;
		for (int i = 0; i != m_fixed.size(); ++i)
			size += m_fixed[i].unzip_key_size();
		return size;
	}

	size_t used_mem_size() const
	{
		size_t size = m_inc.used_mem_size();
		for (int i = 0; i != m_fixed.size(); ++i)
		{
			size += m_fixed[i].used_mem_size();
		}
		return size;
	}

	size_t total_mem_size() const
	{
		size_t size = m_inc.total_mem_size();
		for (int i = 0; i != m_fixed.size(); ++i)
		{
			size += m_fixed[i].total_mem_size();
		}
		return size;
	}

	//! ���������� key ���ܳߴ�
	size_t incr_key_size() const	{ return m_inc.used_pool_size(); }

	//! ���ع̶���������
	const fixed_index_vec& fixed() const { return m_fixed; }
	fixed_index_vec& fixed() { return m_fixed; }

	const increment_index& incr() const { return m_inc; }
	increment_index& incr() { return m_inc; }

	void merge()
	{
		if (m_fixed.size() == 1 && m_inc.empty())
			return; // not needed merge again
		else if (m_fixed.empty() && !m_inc.empty())
		{
			zip_increment();
			return;
		}
		zip_increment();

		// ֱ�ӵ��� val_count() ���ܻᵼ�����¼��� val_count
		// ���� optimize() �е��� merge() ʱ���������Ų� segments������ val_count ��׼ȷ
		// �� optimize() ����ʱ��val_count ��ָ���ȷֵ
		// �û���ʾ���øú���ʱ��val_count ������ȷ��
		size_t now_val_count = 0;
		for (int i = 0; i != m_fixed.size(); ++i)
			now_val_count += m_fixed[i].size();

		fixed_index one(0, m_comp);
		try {
			one.init(zip_key_size() + incr_key_size());
			one.reserve(now_val_count);
			BaseClassT::set_unzip_prefer(one);
			do_zipto_onefixed(one, is_unique_key_t());
		}
		catch (const std::bad_alloc&)
		{
			one.clear(); // free allocated memory, for dump_save_merged use...

			StatisticTime st("MultiWayTable_Impl::merge(), out of memory, use tmp file");

		//	std::string szTmpName(tmpnam(NULL));
		//	dump_save_merged_to_file(*this, szTmpName);
		//	this->clear(); // free memory, for 'one' to use
		//	dump_load_merged_from_file(one, szTmpName);

			FileStream file;
			file.attach(tmpfile());
			dump_save_merged_to_file(*this, file, "MultiWayTable_Impl.merge().tmpfile");
			this->clear(); // free memory, for 'one' to use
			file.seek(0);
			dump_load_merged_from_file(one, file, "MultiWayTable_Impl.merge().tmpfile");
		}
		m_fixed.clear();
		m_fixed.reserve(m_optimizeSegmentCount);
		m_fixed.push_back(fixed_index(0));
		m_fixed[0].swap(one);
		m_inc.clear();

		m_incPos = 0;
//		m_startPos = 0;
		m_synchronized_segments = 0;
	}

	void optimize(bool bZipInc = true)
	{
		//! make m_optimizeSegmentCount proper
		setOptimizeSegmentCount(m_optimizeSegmentCount);

		StatisticTime st("MultiWayTable_Impl::optimize");

		if (bZipInc)
			zip_increment();

		printf("m_fixed.size=%d, ", m_fixed.size()); fflush(stdout);

		if (m_fixed.size() < m_optimizeSegmentCount)
			return;

		size_type bigSize = m_fixed[0].size();
		int firstLittle = 0;
		for (; firstLittle != m_fixed.size(); ++firstLittle)
		{
			if (m_fixed[firstLittle].size() * 2 < bigSize)
				break;
		}
		if (firstLittle >= m_fixed.size() - 1)
			// reach end, or remain only 1 little segment
			firstLittle = 0; // merge all segment
		else
			; // merge little segment

		printf("firstLittle=%d, ", firstLittle); fflush(stdout);
		print_info(stdout, "before optimize");

		fixed_index_vec vtemp;
		vtemp.reserve(firstLittle);
		int i;
		// save [0, firstLittle) to vtemp
		for (i = 0; i != firstLittle; ++i)
		{
			vtemp.push_back(fixed_index(0));
			vtemp[i].swap(m_fixed[i]);
		}

		// erase [0, firstLittle), use swap for efficient
		int remain = m_fixed.size() - firstLittle;
		for (i = 0; i != remain; ++i)
			m_fixed[i].swap(m_fixed[firstLittle + i]);
		m_fixed.erase(m_fixed.begin() + remain, m_fixed.end());

		merge();

		// reserve space for saved old segment in vtemp
		for (i = 0; i != firstLittle; ++i)
			m_fixed.push_back(fixed_index(0));

		if (m_fixed.size() >= 2)
			// put merged segment to back
			m_fixed.front().swap(m_fixed.back());

		// restore old big segment
		for (i = 0; i != firstLittle; ++i)
		{
			m_fixed[i].swap(vtemp[i]);
		}
		print_info(stdout, "after optimize");
	}

	void zipto_onefixed(fixed_index& one) const
	{
		one.init(zip_key_size() + incr_key_size());
		do_zipto_onefixed(one, is_unique_key_t());
	}
	fixed_index zipto_onefixed() const
	{
		fixed_index one(zip_key_size() + incr_key_size(), m_comp);
		do_zipto_onefixed(one);
		return one;
	}
	//@}

protected:
	void do_zipto_onefixed(fixed_index& one, boost::mpl::true_ isUniqueKey) const
	{
		const_iterator iEnd = end();
		for (const_iterator iter = begin(); iter != iEnd; ++iter)
		{
			const key_type xkey = iter.key();
			one.push_back(xkey, *iter);
		}
	}
	void do_zipto_onefixed(fixed_index& one, boost::mpl::false_ isUniqueKey) const
	{
		const_iterator iEnd = end();
		for (const_iterator iter = begin(); iter != iEnd;)
		{
			const key_type xkey = iter.key();
			const_iterator ilow(iter);
			size_type equal_count = goto_next_larger(iter);
			one.push_back(xkey, ilow, iter, equal_count);
		}
	}
	value_type& insert_unique(const key_type& xkey, bool& successed);

	bool fast_insert_aux(const key_type& xkey, const value_type& val, boost:: true_type isUniqueKey);
	void fast_insert_aux(const key_type& xkey, const value_type& val, boost::mpl::false_ isUniqueKey);

	iter_bool_t insert_aux(const key_type& xkey, const value_type& val, boost:: true_type isUniqueKey);
	iterator    insert_aux(const key_type& xkey, const value_type& val, boost::mpl::false_ isUniqueKey);

	value_type& insert_multi(const key_type& xkey, const value_type& val);

	//! compute_val_count is fast enough
	size_type compute_val_count() const
	{
		size_type nSize = m_inc.size();
		for (typename fixed_index_vec::const_iterator iter = m_fixed.begin(); iter != m_fixed.end(); ++iter)
		{
			nSize += (*iter).size();
		}
		return nSize;
	}
	size_type compute_key_count() const { return compute_key_count_aux(is_unique_key_t()); }

	//! is very slow
	size_type compute_key_count_aux(boost::mpl::false_ isUniqueKey) const
	{
		size_type size = 0;
		const_iterator iend = this->end();
		for (const_iterator iter = this->begin(); iter != iend; )
		{
			this->goto_next_larger(iter);
			++size;
		}
		return size;
	}
	size_type compute_key_count_aux(boost::mpl::true_ isUniqueKey) const
	{
		return this->compute_val_count();
	}

	i_iter_t push_back_increment(i_iter_t iter, fixed_index& index, boost::mpl::false_ isUniqueKey)
	{
		try {
			for (; iter != m_inc.end();)
			{
				i_iter_t iupp = iter;
				size_type count = m_inc.goto_next_larger(iupp);
				index.push_back(m_inc.key(iter), iter, iupp, count);
				iter = iupp;
			}
		} catch (const OutOfSpaceException& exp) {
			printf("push_back_increment: %s\n", exp.what());
			throw;
		}
		return iter;
	}
	i_iter_t push_back_increment(i_iter_t iter, fixed_index& index, boost::mpl::true_ isUniqueKey)
	{
		try {
			for (; iter != m_inc.end(); ++iter)
			{
				index.push_back(m_inc.key(iter), *iter);
			}
		} catch (const OutOfSpaceException& exp) {
			printf("push_back_increment: %s\n", exp.what());
			throw;
		}
		return iter;
	}

	void patch_count() const
	{
		if (compute_val_count() != m_val_count) {
			fprintf(stdout, "error    count[key_count=%d, val_count=%d], fix it!\n", m_key_count, m_val_count);
			m_key_count = compute_key_count();
			m_val_count = compute_val_count();
			fprintf(stdout, "repaired count[key_count=%d, val_count=%d], fix it!\n", m_key_count, m_val_count);
		}
	}
	void patch_count(const dump_header& header) const
	{
		m_key_count = header.keyCount;
		m_val_count = header.valCount;
		patch_count();
	}

public:
	/**
	 @{
	 @brief ��ȡ/��ӡժҪ��Ϣ
	 */
	string_appender<>& get_info(string_appender<>& oss, const char* delim="\n") const
	{
		size_t memTotal = total_mem_size();
		size_t memUsed  = used_mem_size();
		size_t valCount = size();
		size_t keyCount = key_count();

		oss << "count[key=" << keyCount << ", val=" << valCount << "],"
			<< delim;
		oss << "mem[used=" << memUsed << ", total=" << memTotal << "],"
			<< delim;
		oss << "fixed[segments=" << m_fixed.size() << ", valCount=" << valCount - m_inc.size()
			<<   ", keySize[zip=" << zip_key_size() << ", unzip=" << unzip_key_size() << "]"
			<<   ", mem[used=" << memUsed - m_inc.used_mem_size()
			<<       ", total=" << memTotal - m_inc.total_mem_size() << "]]"
			<< delim;
		oss << "incr[pool=" << incr_key_size() << ", valCount=" << incr_index_size()
			<<    ", mem[used=" << m_inc.used_mem_size() << ", total=" << m_inc.total_mem_size()
			<< "]]";
		return oss;
	}
	std::string get_info(const char* delim="\n") const
	{
		string_appender<> oss;
		return get_info(oss, delim).str();
	}
	void print_info(FILE* fp, const char* szTitle, const char* delim="\n") const
	{
		fprintf(fp, "%s: [%s]\n", szTitle, get_info(delim).c_str());
	}
	//@}

protected:
	template<class DataInput> void do_dump_load_fixed(DataInput& input, dump_header& header)
	{
		m_fixed.clear();
		if (header.fixedIndexCount > m_fixed.capacity())
			m_fixed.reserve(header.fixedIndexCount);

		for (uint32_t i = 0; i < header.fixedIndexCount; ++i)
		{
			m_fixed.push_back(fixed_index(0));
			m_fixed.back().dump_load(input);
		}
		m_incPos = input.getStream()->tell();
		m_synchronized_segments = m_fixed.size();
	}
	template<class DataInput> void do_dump_load(DataInput& input, dump_header& header)
	{
		m_optimizeSegmentCount = header.m_optimizeSegmentCount;
		do_dump_load_fixed(input, header);
		m_inc.dump_load(input);
	//	printf("do_dump_load_fixed: count[key=%d, val=%d]\n", m_key_count, m_val_count);
		m_key_count = header.keyCount;
		m_val_count = header.valCount;
	//	printf("do_dump_load_fixed: count[key=%d, val=%d]\n", m_key_count, m_val_count);
		patch_count(header);
	}

public:
	/**
	 @{
	 @brief �������ٶȴ�ȡ����
	 */
	template<class DataInput> void dump_load(DataInput& input)
	{
		stream_position_t startPos = input.getStream()->tell();
		assert(0 == startPos);
//		m_startPos = input.getStream()->tell();
		dump_header header;
		input >> header;
		do_dump_load(input, header);

// 		if (header.version < dump_header::class_version)
// 		{
// 		// header version is not compatible, make all not synchronized
// 			m_incPos = 0;
// 		//	m_startPos = 0;
// 			m_synchronized_segments = 0;
// 		}
	}
	template<class DataOutput> void dump_save(DataOutput& output) const
	{
		stream_position_t startPos = output.getStream()->tell();
		stream_position_t fileSize = output.getStream()->size();
		assert(0 == startPos);

		dump_header header(*this);
		output << header;
		assert(m_synchronized_segments <= m_fixed.size());

		int firstSegment = 0;
		// 0 != fileSize ��ʾֻ��Ҫд�������ӵĲ���
		if (0 != m_incPos && 0 != fileSize)
		{
		//	output.getStream()->seek(startPos + (m_incPos - m_startPos));
			output.getStream()->seek(m_incPos);
			firstSegment = m_synchronized_segments;
		}
		for (int i = firstSegment; i != m_fixed.size(); ++i)
		{
			m_fixed[i].dump_save(output);
		}
		m_synchronized_segments = m_fixed.size();
		m_incPos = output.getStream()->tell();
		m_inc.dump_save(output);
	}
	//@}
	DATA_IO_REG_DUMP_LOAD_SAVE(MultiWayTable_Impl)

protected:
	//@{
	//! ���л�֧��
	//! looks like as dump_load/dump_save
	//
	template<class DataInput> void load(DataInput& input, unsigned int version)
	{
		m_fixed.clear();

		dump_header header;
		input >> header;
		m_optimizeSegmentCount = header.m_optimizeSegmentCount;

		if (header.fixedIndexCount > m_fixed.capacity())
			m_fixed.reserve(header.fixedIndexCount);

		for (uint32_t i = 0; i < header.fixedIndexCount; ++i)
		{
			m_fixed.push_back(fixed_index(0));
			input >> m_fixed.back();
		}
		input >> m_inc;
	}
	template<class DataOutput> void save(DataOutput& output, unsigned int version) const
	{
		dump_header header(*this);
		output << header;

		typename fixed_index_vec::const_iterator iter;
		for (iter = m_fixed.begin(); iter != m_fixed.end(); ++iter)
		{
			output << *iter;
		}
		output << m_inc;
	}
	DATA_IO_REG_LOAD_SAVE_V(MultiWayTable_Impl, 1)
	//@}

protected:
	mutable uint32_t m_key_count; //!< ���Բ�׼ȷ������׼ȷʱ���Զ����¼���һ��
	mutable uint32_t m_val_count; //!< ���Բ�׼ȷ������׼ȷʱ���Զ����¼���һ��
	uint32_t  m_maxPoolSize;
	compare_t m_comp;
	increment_index	m_inc;
	fixed_index_vec m_fixed;
	uint32_t m_optimizeSegmentCount;

	mutable int m_synchronized_segments; //!< ���ļ�ͬ���Ĺ̶�������Ŀ
//	mutable stream_position_t m_startPos; //!< �ö������ļ��е���ʼƫ��
	mutable stream_position_t m_incPos;   //!< �����������ļ��е�ƫ�ƣ�֮ǰ���� fixed_index
};

//////////////////////////////////////////////////////////////////////////
template<class BaseClassT>
void MultiWayTable_Impl<BaseClassT>::zip_increment()
{
	i_iter_t iter = m_inc.begin();
	while (m_inc.end() != iter)
	{
		m_fixed.push_back(fixed_index(0, m_comp));

		fixed_index& index = m_fixed.back();
		BaseClassT::set_unzip_prefer(index);

	//	index.init(m_maxPoolSize);
		index.init(m_inc.used_pool_size());

		index.reserve(m_inc.size());
		iter = push_back_increment(iter, index, is_unique_key_t());
		index.trim_extra();
	}
	m_inc.clear();
}

template<class BaseClassT>
typename
MultiWayTable_Impl<BaseClassT>::value_type&
MultiWayTable_Impl<BaseClassT>::insert_multi(const key_type& xkey, const value_type& val)
{
	fixed_index::check_key(xkey);

	// do not use equal_range, equal_range is slower because it need find all segment.

	bool keyExisted = m_inc.end() != m_inc.find(xkey);
	if (!m_inc.can_insert(xkey))
	{
		optimize();
	}
	i_iter_t iter = m_inc.insert(xkey, value_type());

	if (!keyExisted)
	{
		for (int i = 0; i != m_fixed.size(); ++i)
		{
			f_iter_t f = m_fixed[i].find(xkey);
			if (m_fixed[i].end() != f)
			{
				keyExisted = true;
				break;
			}
		}
	}
	if (!keyExisted) m_key_count++;

	m_val_count++;
	return *iter;
}

template<class BaseClassT>
typename
MultiWayTable_Impl<BaseClassT>::value_type&
MultiWayTable_Impl<BaseClassT>::insert_unique(const key_type& xkey, bool& successed)
{
	fixed_index::check_key(xkey);

	// do not use equal_range, equal_range is slower because it need find all segment.

	for (int i = 0; i != m_fixed.size(); ++i)
	{
		f_iter_t f = m_fixed[i].find(xkey);
		if (m_fixed[i].end() != f)
		{
			successed = false;
			return *f;
		}
	}
	if (!m_inc.can_insert(xkey))
	{
		// optimize would clear m_inc, so first find xkey in m_inc
		i_iter_t f = m_inc.find(xkey);
		if (m_inc.end() != f) {
			successed = false;
			return *f;
		}
		optimize();
	}
	typename increment_index::iter_bool_t ib = m_inc.insert(xkey, value_type());
	if (ib.second) {
		m_key_count++;
		m_val_count++;
	}
//	assert(size() == m_val_count);
//	assert(size() == m_key_count);

	successed = ib.second;
	return (*ib.first);
}

template<class BaseClassT>
typename
MultiWayTable_Impl<BaseClassT>::iter_bool_t
MultiWayTable_Impl<BaseClassT>::
insert_aux(const key_type& xkey, const value_type& val, boost::mpl::true_ isUniqueKey)
{
	fixed_index::check_key(xkey);

	range_t ii = equal_range(xkey, m_comp);
	if (ii.first != ii.second)
	{
		return iter_bool_t(ii.first, false);
	}
	if (!m_inc.can_insert(xkey))
	{
		optimize();
		ii = equal_range(xkey, m_comp);
	}
	typename increment_index::iter_bool_t ib = m_inc.insert(xkey, val);
	ii.first.m_iinc = ib.first;
	ii.first.m_is_in_fixed = false;

	m_key_count++;
	m_val_count++;

	assert(size() == m_val_count);
	assert(size() == m_key_count);

	return iter_bool_t(ii.first, true);
}

template<class BaseClassT>
typename
MultiWayTable_Impl<BaseClassT>::iterator
MultiWayTable_Impl<BaseClassT>::
insert_aux(const key_type& xkey, const value_type& val, boost::mpl::false_ isUniqueKey)
{
	fixed_index::check_key(xkey);

	range_t ii = equal_range(xkey, m_comp);
	bool keyExisted = ii.first != ii.second;
	if (!m_inc.can_insert(xkey))
	{
		optimize();
		ii = equal_range(xkey, m_comp);
	}
	ii.first.m_iinc = m_inc.insert(xkey, val);

 // Ҫ��֤�ͱ�׼һ�¾�Ҫ�� ii.first ָ���²�������Ԫ��
	ii.first.m_fvec.swap(ii.second.m_fvec); // �� first.m_fvec ���� first.m_iinc ��
	ii.first.m_is_in_fixed = false;

	// for exception safe, only when success, update member
	if (!keyExisted)
		m_key_count++;

	m_val_count++;

	return ii.first;
}

//! return false if has a duplicate xkey
template<class BaseClassT>
bool
MultiWayTable_Impl<BaseClassT>::
fast_insert_aux(const key_type& xkey, const value_type& val, boost::mpl::true_ isUniqueKey)
{
	bool successed;
	value_type& vexisted = insert_unique(xkey, successed);
	if (successed)
		vexisted = val;
	return successed;
}

/**
 @brief �ᵼ�� m_val_count �� m_key_count ����

 Ϊ��Ч�ʣ��ᵼ�� m_val_count �� m_key_count ����
 �Ӷ������ڻ�ȡ size() �� distinct_size ʱ��Ҫ���¼���
 */
template<class BaseClassT>
void
MultiWayTable_Impl<BaseClassT>::
fast_insert_aux(const key_type& xkey, const value_type& val, boost::mpl::false_ isUniqueKey)
{
	fixed_index::check_key(xkey);

	if (!m_inc.can_insert(xkey))
	{
		optimize();
	}
	m_inc.insert(xkey, val);
}

template<class BaseClassT>
template<class IterT>
void
MultiWayTable_Impl<BaseClassT>::insert(const key_type& xkey, const IterT& first, const IterT& last)
{
	BOOST_STATIC_ASSERT(!IsUniqueKey);

	fixed_index::check_key(xkey);

	if (!m_inc.can_insert(xkey))
	{
		optimize();
	}
	range_t ii = equal_range(xkey, m_comp);
	if (ii.first != ii.second)
		m_key_count++;

	size_t old_count = m_inc.size();
	m_inc.insert(xkey, first, last);
	m_val_count += m_inc.size() - old_count;
}

template<class BaseClassT>
template<class IterT>
void
MultiWayTable_Impl<BaseClassT>::fast_insert(const key_type& xkey, const IterT& first, const IterT& last)
{
	BOOST_STATIC_ASSERT(!IsUniqueKey);

	fixed_index::check_key(xkey);

	if (!m_inc.can_insert(xkey))
	{
		optimize();
	}
	m_inc.insert(xkey, first, last);
}


//////////////////////////////////////////////////////////////////////////
/**
 @brief MultiWayTable_Base::fixed_index �� SortedTable
 */
template<class KeyT,
		 class ValueT,
		 class CompareT,
		 class KeyCategory
>
class MultiWayTable_Base
{
public:
	BOOST_STATIC_CONSTANT(bool, IsUniqueKey = (boost::is_same<KeyCategory, UniqueKey>::value));

private:
	typedef SortedTable<KeyT, ValueT, CompareT> base_fixed_t;
	typedef PackedTable<KeyT, ValueT, CompareT, IsUniqueKey> base_increment_t;

	BOOST_STATIC_CONSTANT(bool, IsBiWay = (boost::is_same<BiWayTableKey  , KeyCategory>::value));
	BOOST_STATIC_CONSTANT(bool, IsIndex = (boost::is_same<MultiIndexedKey, KeyCategory>::value));

public:
	typedef CompareT	compare_t;
	typedef KeyT		key_type;
	typedef ValueT		value_type;
	typedef ValueT&		reference;
	typedef KeyCategory key_category;
	typedef MultiWayTable_Impl_dump_header dump_header;

	typedef MultiSortedTable<base_fixed_t>           indexed_fixed_t;
	typedef MultiPackedTable<KeyT, ValueT, CompareT> indexed_increment_t;

	typedef typename boost::mpl::if_c<
				IsBiWay, BiWayTable<indexed_fixed_t, base_fixed_t>,
				typename boost::mpl::if_c<
					IsIndex, indexed_fixed_t, base_fixed_t
				>::type
			>::type fixed_index;
	typedef std::vector<fixed_index> fixed_index_vec;

	typedef typename boost::mpl::if_c<
				IsBiWay, BiWayTable<indexed_increment_t, base_increment_t>,
				typename boost::mpl::if_c<
					IsIndex, indexed_increment_t, base_increment_t
				>::type
			>::type increment_index;

	void set_unzip_prefer(fixed_index_vec& fixed, unzip_prefer uzp) { }
	void set_unzip_prefer(fixed_index& fixed) { }
};

template<class KeyT,
		 class ValueT,
		 class CompareT=std::less<KeyT>,
		 class KeyCategory=UniqueKey
>
class MultiWayTable	:
	public	MultiWayTable_Impl<MultiWayTable_Base<KeyT, ValueT, CompareT, KeyCategory> >
{
	typedef MultiWayTable_Impl<MultiWayTable_Base<KeyT, ValueT, CompareT, KeyCategory> > super;

public:
	typedef typename super::size_type size_type;

	explicit MultiWayTable(uint32_t maxSegmentSize = TERARK_IF_DEBUG(8*1024, 2*1024*1024),
						   CompareT comp = CompareT(),
						   size_type initialFixedIndexCapacity = 512)
		: super(maxSegmentSize, comp, initialFixedIndexCapacity)
	{
	}
	DATA_IO_REG_DUMP_LOAD_SAVE(MultiWayTable)
};

} } // namespace terark::prefix_zip

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma warning(pop)
#endif

#endif // __terark_multi_way_table_h__

