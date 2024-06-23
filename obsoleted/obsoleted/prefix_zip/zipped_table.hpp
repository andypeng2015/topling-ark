#ifndef __zip_dict_h__
#define __zip_dict_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include "zstring.hpp"
#include "string_pool.hpp"

#include "../string_compare.hpp"
#include <terark/io/DataIO.hpp>
#include <terark/io/MemStream.hpp>

#include <boost/operators.hpp>
//#include <boost/iterator.hpp>
#include <boost/iterator/reverse_iterator.hpp>
#include <boost/static_assert.hpp>

namespace terark { namespace prefix_zip {

/// ǰ������
template<class ValueT, class CompareT, uint_t FlagLength>
class ZippedTable;

template<uint_t FlagLength>
class ZippedStringPool : public AutoGrownMemIO
{
public:
	typedef ZString<FlagLength> zstr_t;
	BOOST_STATIC_CONSTANT(uint_t, MAX_ZSTRING = zstr_t::MAX_ZSTRING);
	BOOST_STATIC_ASSERT(MAX_ZSTRING < 64 * 1024);

	explicit ZippedStringPool(uint32_t maxSize)	: AutoGrownMemIO(maxSize) { }

	///@note if the pool is to be full, throw OutOfSpaceException
	uint32_t put(const std::string& str, uint_t nPrefix, uint_t diff)
	{
		assert(nPrefix <= str.size());

		uint_t nRequire;
		if (0 == diff)
			nRequire = str.size();
		else
			nRequire = str.size() - nPrefix + FlagLength;

		uint32_t offset = this->tell();
		if (offset + nRequire > this->size())
		{
			char szMsg[4 * 1024];
		#if defined(_WIN32) || defined(_WIN64)
			_snprintf
		#else
			snprintf
		#endif
		    ( szMsg, sizeof(szMsg),
			  "%s: [str='%s', nPrefix=%u, diff=%u]",
			  BOOST_CURRENT_FUNCTION, str.c_str(), nPrefix, diff);
			throw OutOfSpaceException(szMsg);
		}
		if (0 == diff) {
			const byte firstByte = byte(str[0]);
			if (ZString_EscapeByte(firstByte) != -1 || 0xFF == firstByte)
			{
				this->writeByte(0xFF);
			}
			this->write(str.c_str(), str.size());
		} else {
			zstr_t* zipped = zstr(this->tell());
			zipped->setDiff(diff);
			zipped->setPrefixLength(nPrefix);
			memcpy(zipped->suffix(), str.c_str() + nPrefix, str.size() - nPrefix);
			this->seek(offset + nRequire);
		}
		return offset;
	}

	zstr_t* zstr(uint32_t offset) const
	{
		assert(offset < this->size());
		return (zstr_t*)(this->buf() + offset);
	}
};

template<class ValueT, class CompareT, uint_t FlagLength = 3>
class ZippedTable;
template<class ValueT, class CompareT, uint_t FlagLength>
class ZippedTable_ConstIterator;

template<class ValueT, class CompareT, uint_t FlagLength>
class ZippedTable_Iterator :
	public boost::random_access_iterator_helper<
				ZippedTable_Iterator<ValueT, CompareT, FlagLength>,
				ValueT,
				std::ptrdiff_t,
				ValueT*,
				ValueT&>
{
	typedef ZippedTable_Iterator  my_type;
	typedef IndexNode<ValueT>	  node_t;
	typedef std::vector<node_t>   index_t;
	typename index_t::iterator m_iter;
	friend class ZippedTable<ValueT, CompareT, FlagLength>;
	friend class ZippedTable_ConstIterator<ValueT, CompareT, FlagLength>;

	ZippedTable_Iterator(typename index_t::iterator iter)
		: m_iter(iter) {}

public:
	typedef std::ptrdiff_t difference_type;

	ZippedTable_Iterator() {}

	ValueT& operator*() const { return (*m_iter).val; }

	my_type& operator++() { ++m_iter; return *this; }
	my_type& operator--() { --m_iter; return *this; }

	friend bool operator==(const my_type& x, const my_type& y)
	{
		return x.m_iter == y.m_iter;
	}

	my_type& operator+=(difference_type diff)
	{
		m_iter += diff;
		return *this;
	}
	my_type& operator-=(difference_type diff)
	{
		m_iter -= diff;
		return *this;
	}

	friend difference_type operator-(const my_type& x,const my_type& y)
	{
		return x.m_iter - y.m_iter;
	}
	friend bool operator<(const my_type& x,const my_type& y)
	{
		x.m_iter < y.m_iter;
	}
};

template<class ValueT, class CompareT, uint_t FlagLength>
class ZippedTable_ConstIterator :
	public boost::random_access_iterator_helper<
				ZippedTable_ConstIterator<ValueT, CompareT, FlagLength>,
				ValueT,
				std::ptrdiff_t,
				const ValueT*,
				const ValueT&>
{
	typedef ZippedTable_ConstIterator my_type;
	typedef IndexNode<ValueT>	  node_t;
	typedef std::vector<node_t>   index_t;

	typename index_t::const_iterator m_iter;
	friend class ZippedTable<ValueT, CompareT, FlagLength>;

	ZippedTable_ConstIterator(typename index_t::const_iterator iter)
		: m_iter(iter) {}

public:
	typedef std::ptrdiff_t difference_type;

	ZippedTable_ConstIterator() {}
	ZippedTable_ConstIterator(ZippedTable_Iterator<ValueT, CompareT, FlagLength> iter)
		: m_iter(iter.m_iter) { }

	const ValueT& operator*() const { return (*m_iter).val; }

	my_type& operator++() { ++m_iter; return *this; }
	my_type& operator--() { --m_iter; return *this; }

	friend bool operator==(const my_type& x,const my_type& y)
	{
		return x.m_iter == y.m_iter;
	}

	my_type& operator+=(difference_type diff)
	{
		m_iter += diff;
		return *this;
	}
	my_type& operator-=(difference_type diff)
	{
		m_iter -= diff;
		return *this;
	}

	friend difference_type operator-(const my_type& x,const my_type& y)
	{
		return x.m_iter - y.m_iter;
	}
	friend bool operator<(const my_type& x,const my_type& y)
	{
		return x.m_iter < y.m_iter;
	}
};

/**
 @ingroup prefix_zip
 @brief ѹ�����ַ���ӳ���

  ֻ�ܰ� CompareT ��ʾ��˳��Ԫ�ؼ����ӳ���һ�㲻����ʹ��
  ��Ҫ���� StringTable �Ĺ̶�����
 */
template<class ValueT, class CompareT, uint_t FlagLength>
class ZippedTable
{
	typedef IndexNode<ValueT>	node_t;

public:
	typedef std::vector<node_t>			 index_t;
	typedef ZString<FlagLength>			 zstr_t;
	typedef ZippedStringPool<FlagLength> zpool_t;

	typedef ValueT value_type;
	typedef typename index_t::size_type size_type;

	typedef ZippedTable_Iterator<ValueT, CompareT, FlagLength>		iterator;
	typedef ZippedTable_ConstIterator<ValueT, CompareT, FlagLength> const_iterator;

	typedef boost::reverse_iterator<iterator>		reverse_iterator;
	typedef boost::reverse_iterator<const_iterator> const_reverse_iterator;

	typedef typename iterator::difference_type difference_type;

	typedef std::pair<iterator, iterator> range_t;
	typedef std::pair<const_iterator, const_iterator> const_range_t;

	BOOST_STATIC_CONSTANT(uint_t, MAX_ZSTRING = zstr_t::MAX_ZSTRING);

private:
	index_t  m_index;
	zpool_t  m_pool;
	CompareT m_comp;

	void do_unzip(const_iterator iter, std::string& str, uint_t nPrefix) const;
	uint_t recordLength(const_iterator iter) const
	{
		return iter.m_iter[1].offset - (*iter.m_iter).offset;
	}
	zstr_t* zstr(const_iterator iter) const
	{
		return m_pool.zstr((*iter.m_iter).offset);
	}

	void push_back(const std::string& key, const ValueT& val, uint_t nPreifx, uint_t diff);

public:
	explicit ZippedTable(uint32_t maxPoolSize, const CompareT& comp = CompareT())
		: m_pool(maxPoolSize), m_comp(comp)
	{
		// push_back first dummy value..
		m_index.push_back(node_t(0));
	}
	ZippedTable(const ZippedTable& rhs)
		: m_index(rhs.m_index), m_pool(0)
	{
		m_pool.clone(rhs.m_pool);
	}
	ZippedTable& operator=(const ZippedTable& rhs)
	{
		m_index = rhs.m_index;
		m_pool.clone(rhs.m_pool);
		return *this;
	}

//////////////////////////////////////////////////////////////////////////
	void swap(ZippedTable& that)
	{
		m_index.swap(that);
		m_pool.swap(that.m_pool);
	}
	friend void swap(ZippedTable& x, ZippedTable& y) { x.swap(y); }

	void clear()
	{
		m_index.clear();
		m_index.push_back(node_t(0));
		m_pool.seek(0);
	}

	bool empty() const { return size() == 0; }
	size_type size() const	{ return m_index.size() - 1; }

	iterator	   begin()	     { return m_index.begin(); }
	const_iterator begin() const { return m_index.begin(); }
	iterator	   end()	   { return m_index.end() - 1; }
	const_iterator end() const { return m_index.end() - 1; }

	reverse_iterator	   rbegin()		  { return		 reverse_iterator(end()); }
	const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
	reverse_iterator	   rend()		  { return		 reverse_iterator(begin()); }
	const_reverse_iterator rend()   const { return const_reverse_iterator(begin()); }

//////////////////////////////////////////////////////////////////////////
	void push_back(const std::string& key, const ValueT& val);

	void pop_back();
	const ValueT& back() const { return *(this->end() - 1); }
		  ValueT& back()	   { return *(this->end() - 1); }

	uint_t strlen(const_iterator iter) const;

	void unzip(const_iterator iter, std::string& str) const;
	std::string unzip(const_iterator iter) const;
	std::string str(const_iterator iter) const { return unzip(iter); }

	std::string unzip(const_reverse_iterator riter) const
	{
		const_iterator iter = riter.base();
		return unzip(--iter);
	}
	std::string str(const_reverse_iterator riter) const { return unzip(riter); }

	bool is_zipped(const_iterator iter) const
	{
		zstr_t* zipped = zstr(iter);
		return zipped->getDiff() != 0;
	}

	uint_t primary_prefix_diff(const_iterator iter) const;

	size_t unzip_str_size() const;

	size_type used_pool_size()  const { return m_pool.tell(); }
	size_type total_pool_size() const { return m_pool.size(); }
	size_type available_pool_size() const { return m_pool.remain(); }

	void reallocate(uint32_t maxPoolSize)
	{
		clear();
		m_pool.reallocate(maxPoolSize);
	}

	/**
	 @brief �ͷ��˷ѵĿռ�

	  ������õĿռ���� 2\%����������һ���ڴ����ַ����أ�����ԭ�ȵ��ڴ��ͷ�\n
	  һ�����ڶ���һ�� ZippedTable ʱ�����øú���֮���ټ�����Ԫ�غܿ���ʧ��
	 */
	void trim_extra()
	{
		if ((double)m_pool.tell()/m_pool.size() < 0.98)
		{
			zpool_t temp(m_pool.tell());
			temp.write(m_pool.buf(), m_pool.tell());
			m_pool.swap(temp);
		}
	}

	/**
	 @brief ��� str �Ƿ����

	  ���̫�󣬾��׳� std::invalid_argument �쳣
	 */
	static void check_string(const std::string& str)
	{
		if (str.size() > MAX_ZSTRING)
		{
			char szMsg[1024];
			sprintf(szMsg, "string size=%d too large for ZippedTable<FlagLength=%d>::MAX_ZSTRING=%d",
				str.size(), FlagLength, MAX_ZSTRING);
			throw std::invalid_argument(szMsg);
		}
	}

//////////////////////////////////////////////////////////////////////////
private:
	template<class SelfPtrT, class IterT, class CompatibleCompareT>
	static IterT __lower_bound(SelfPtrT self, IterT first, IterT last, const std::string& key, CompatibleCompareT& comp);
	template<class SelfPtrT, class IterT, class CompatibleCompareT>
	static IterT __upper_bound(SelfPtrT self, IterT first, IterT last, const std::string& key, CompatibleCompareT& comp);
	template<class SelfPtrT, class IterT, class CompatibleCompareT>
	static std::pair<IterT, IterT> __equal_range(SelfPtrT self, IterT first, IterT last, const std::string& key, CompatibleCompareT& comp);

public:
	template<class IterT, class CompatibleCompareT>
	IterT lower_bound(IterT first, IterT last, const std::string& key, CompatibleCompareT& comp)
	{
		return __lower_bound(this, first, last, key, comp);
	}
	template<class SelfT, class IterT, class CompatibleCompareT>
	IterT upper_bound(IterT first, IterT last, const std::string& key, CompatibleCompareT& comp)
	{
		return __upper_bound(this, first, last, key, comp);
	}
	template<class IterT, class CompatibleCompareT>
	std::pair<IterT, IterT> equal_range(IterT first, IterT last, const std::string& key, CompatibleCompareT& comp)
	{
		return __equal_range(this, first, last, key, comp);
	}

//////////////////////////////////////////////////////////////////////////
	template<class IterT, class CompatibleCompareT>
	IterT lower_bound(IterT first, IterT last, const std::string& key, CompatibleCompareT& comp) const
	{
		return __lower_bound(this, first, last, key, comp);
	}
	template<class SelfT, class IterT, class CompatibleCompareT>
	IterT upper_bound(IterT first, IterT last, const std::string& key, CompatibleCompareT& comp) const
	{
		return __upper_bound(this, first, last, key, comp);
	}
	template<class IterT, class CompatibleCompareT>
	std::pair<IterT, IterT> equal_range(IterT first, IterT last, const std::string& key, CompatibleCompareT& comp) const
	{
		return __equal_range(this, first, last, key, comp);
	}

//////////////////////////////////////////////////////////////////////////
	template<class CompatibleCompareT>
	iterator lower_bound(const std::string& key, CompatibleCompareT& comp)
	{
		return lower_bound(this->begin(), this->end(), key, comp);
	}
	template<class CompatibleCompareT>
	iterator upper_bound(const std::string& key, CompatibleCompareT& comp)
	{
		return upper_bound(this->begin(), this->end(), key, comp);
	}
	template<class CompatibleCompareT>
	range_t  equal_range(const std::string& key, CompatibleCompareT& comp)
	{
		return equal_range(this->begin(), this->end(), key, comp);
	}

	template<class CompatibleCompareT>
	const_iterator lower_bound(const std::string& key, CompatibleCompareT& comp) const
	{
		return lower_bound(this->begin(), this->end(), key, comp);
	}
	template<class CompatibleCompareT>
	const_iterator upper_bound(const std::string& key, CompatibleCompareT& comp) const
	{
		return upper_bound(this->begin(), this->end(), key, comp);
	}
	template<class CompatibleCompareT>
	const_range_t  equal_range(const std::string& key, CompatibleCompareT& comp) const
	{
		return equal_range(this->begin(), this->end(), key, comp);
	}

	//////////////////////////////////////////////////////////////////////////

	iterator lower_bound(const std::string& key)
	{
		return lower_bound(this->begin(), this->end(), key, m_comp);
	}
	iterator upper_bound(const std::string& key)
	{
		return upper_bound(this->begin(), this->end(), key, m_comp);
	}
	range_t  equal_range(const std::string& key)
	{
		return equal_range(this->begin(), this->end(), key, m_comp);
	}

	const_iterator lower_bound(const std::string& key) const
	{
		return lower_bound(this->begin(), this->end(), key, m_comp);
	}
	const_iterator upper_bound(const std::string& key) const
	{
		return upper_bound(this->begin(), this->end(), key, m_comp);
	}
	const_range_t  equal_range(const std::string& key) const
	{
		return equal_range(this->begin(), this->end(), key, m_comp);
	}

	template<class DataOutput> void save_data(DataOutput& output) const;

private:
	template<class DataInput > void load(DataInput & input);
	template<class DataOutput> void save(DataOutput& output) const
	{
		uint32_t count = this->size();
		uint32_t dataSize = used_pool_size();
		output & count & dataSize;
		save_data(output);
	}
	DATA_IO_REGISTER_LOAD_SAVE(ZippedTable)
};

template<class ValueT, class CompareT, uint_t FlagLength>
template<class DataInput>
void ZippedTable<ValueT, CompareT, FlagLength>::load(DataInput& input)
{
	uint32_t count;
	uint32_t dataSize;
	input >> count >> dataSize;

	m_index.clear();
	m_pool.reallocate(dataSize);

	uint32_t offset = 0;
	while (count--)
	{
		ValueT val;
		input >> val;

		uint16_t nZStrLen;
		input >> nZStrLen;
		if (nZStrLen >= MAX_ZSTRING)
			throw SizeValueTooLargeException(nZStrLen, MAX_ZSTRING, "ZippedTable::load@string");

		if (input.read(m_pool.buf() + offset, nZStrLen) != nZStrLen)
			throw EndOfFileException("ZippedTable::load@ExtData");

		m_index.push_back(node_t(offset, val));
		offset += nZStrLen;
	}
	m_index.push_back(node_t(offset)); // last dummy...

	assert(m_pool.size() == offset);

	m_pool.seek(offset);
}

template<class ValueT, class CompareT, uint_t FlagLength>
template<class DataOutput>
void ZippedTable<ValueT, CompareT, FlagLength>::save_data(DataOutput& output) const
{
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		output & *iter;
		uint16_t nZStrLen = recordLength(iter);
		output.operator<<(nZStrLen);
		if (output.write(zstr(iter), nZStrLen) != nZStrLen)
			throw OutOfSpaceException("ZippedTable::save@string");
	}
}

template<class ValueT, class CompareT, uint_t FlagLength>
template<class SelfPtrT, class IterT, class CompatibleCompareT>
IterT ZippedTable<ValueT, CompareT, FlagLength>::__lower_bound(
	SelfPtrT self, IterT first, IterT last, const std::string& key, CompatibleCompareT& comp)
{
	difference_type len = last - first;
	difference_type half;
	IterT middle;

	while (len > 0)
	{
		half = len >> 1;
		middle = first + half;
		if (comp(self->unzip(middle), key)) {
			first = middle + 1;
			len = len - half - 1;
		} else
			len = half;
	}
	return first;
}

template<class ValueT, class CompareT, uint_t FlagLength>
template<class SelfPtrT, class IterT, class CompatibleCompareT>
IterT ZippedTable<ValueT, CompareT, FlagLength>::__upper_bound(
	SelfPtrT self, IterT first, IterT last, const std::string& key, CompatibleCompareT& comp)
{
	difference_type len = last - first;
	difference_type half;
	IterT middle;

	while (len > 0)
	{
		half = len >> 1;
		middle = first + half;
		if (comp(key, self->unzip(middle)))
			len = half;
		else {
			first = middle + 1;
			len = len - half - 1;
		}
	}
	return first;
}

template<class ValueT, class CompareT, uint_t FlagLength>
template<class SelfPtrT, class IterT, class CompatibleCompareT>
std::pair<IterT, IterT>
ZippedTable<ValueT, CompareT, FlagLength>::__equal_range(
	SelfPtrT self, IterT first, IterT last, const std::string& key, CompatibleCompareT& comp)
{
	difference_type len = last - first;
	difference_type half;
	IterT middle, left, right;

	while (len > 0) {
		half = len >> 1;
		middle = first + half;
		std::string strMiddle = self->unzip(middle);
		if (comp(strMiddle, key)) {
			first = middle + 1;
			len = len - half - 1;
		}
		else if (comp(key, strMiddle))
			len = half;
		else {
			left = __lower_bound<SelfPtrT>(self, first, middle, key, comp);
			right = __upper_bound<SelfPtrT>(self, ++middle, first + len, key, comp);
			return std::pair<IterT, IterT>(left, right);
		}
	}
	return std::pair<IterT, IterT>(first, first);
}

template<class ValueT, class CompareT, uint_t FlagLength>
uint_t ZippedTable<ValueT, CompareT, FlagLength>::primary_prefix_diff(const_iterator iter) const
{
	assert(iter >= begin() && iter < end());

	uint_t diff = 0;
	for (; ; )
	{
		zstr_t* zipped = zstr(iter-difference_type(diff));
		uint_t theDiff = zipped->getDiff();
		if (0 == theDiff)
			break;
		diff += theDiff;
	}
	return diff;
}

template<class ValueT, class CompareT, uint_t FlagLength>
uint_t ZippedTable<ValueT, CompareT, FlagLength>::strlen(const_iterator iter) const
{
	assert(iter >= begin() && iter < end());

	zstr_t* zipped = zstr(iter);
	uint_t diff = zipped->getDiff();
	if (0 == diff) {
		if (zipped->isFirstByteEscape())
			return recordLength(iter) - 1;
		else
			return recordLength(iter);
	} else {
		uint_t nSuffix = recordLength(iter) - FlagLength; // suffix length of zstr_t..
		return zipped->prefixLength() + nSuffix;
	}
}

template<class ValueT, class CompareT, uint_t FlagLength>
size_t ZippedTable<ValueT, CompareT, FlagLength>::unzip_str_size() const
{
	size_t size = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		// ��������ʽ��������ô�Ż�
		// �ܱ��ⲻ��Ҫ������һ���ֽڱ��� string ���Ȼ��߽����� '\0'
		size += strlen(iter) + 1;
	}
	return size;
}

/*
 * [0] �л�             getDiff() = 0, text() = �л�
 * [1] �л���           getDiff() = 1, nPrefix = 4, suffix() = ��
 * [2] �л�����         getDiff() = 1, nPrefix = 6, suffix() = ��
 * [3] �л�����       getDiff() = 1, nPrefix = 8, suffix() = ��
 * [4] �л����񹲺�     getDiff() = 1, nPrefix =10, suffix() = ��
 * [5] �л����񹲺͹�   getDiff() = 1, nPrefix =12, suffix() = ��
 **[6] �л�����ͬ     getDiff() = 3, nPrefix =10, suffix() = ͬ
 * [7] �л�����ͬ�ܶ� getDiff() = 1, nPrefix =12, suffix() = �ܶ�
 */
template<class ValueT, class CompareT, uint_t FlagLength>
void ZippedTable<ValueT, CompareT, FlagLength>::unzip(const_iterator iter, std::string& str) const
{
	str.resize(this->strlen(iter));
	do_unzip(iter, str, str.size());
}

template<class ValueT, class CompareT, uint_t FlagLength>
std::string ZippedTable<ValueT, CompareT, FlagLength>::unzip(const_iterator iter) const
{
	std::string str;
	str.resize(this->strlen(iter));
	do_unzip(iter, str, str.size());
	return str;
}

template<class ValueT, class CompareT, uint_t FlagLength>
void ZippedTable<ValueT, CompareT, FlagLength>::do_unzip(const_iterator iter, std::string& str, uint_t nPrefix) const
{
	assert(iter >= this->begin());
	assert(iter <  this->end());

	zstr_t* zipped = zstr(iter);

	uint_t diff = zipped->getDiff();
	char* pszData = const_cast<char*>(str.data());
	if (0 == diff) {
		memcpy(pszData, zipped->text(), nPrefix);
	} else {
		uint_t myPrefix = zipped->prefixLength();
		if (nPrefix > myPrefix) {
#if defined(_DEBUG) || !defined(NDEBUG)
			uint_t mySuffixLength = recordLength(iter) - FlagLength;
			assert(nPrefix - myPrefix <= mySuffixLength);
#endif
			memcpy(pszData + myPrefix, zipped->suffix(), nPrefix - myPrefix);
			do_unzip(iter - diff, str, myPrefix);
		} else
			do_unzip(iter - diff, str, nPrefix);
	}
}

// push_back Ԫ�ص�˳��������ֵ�˳��
// val ���ܰ��������ַ�(ASCII code range is [1, 32])
template<class ValueT, class CompareT, uint_t FlagLength>
void ZippedTable<ValueT, CompareT, FlagLength>::push_back(const std::string& key, const ValueT& val)
{
	check_string(key);

	if (empty())
	{
		push_back(key, val, 0, 0);
		return;
	}

	iterator iBack = end() - 1;
	std::string strBack = unzip(iBack);

	// ��������һ�������е����ģ�Ԫ�ش�
	assert(m_comp(strBack, key)); // strBack < key

	typename CompareT::prefix_compare comp(CompareT::prefix_compare::common_prefix_length(key, strBack));

	// ǰ׺����Ӧ�� FlagLength һ������
	// ���Ϊ common_prefix_length(key, strBack)
	if (comp.getPrefix() <= FlagLength)
	{
		// ǰ׺�̣ܶ�����Ҫѹ��
		push_back(key, val, 0, 0);
	}
	else
	{
		iterator iPrefix = iBack - primary_prefix_diff(iBack);

		// �� diff ��С�ڵ��� zstr_t::MAX_DIFF
		// ��� end() - iter1 ���� zstr_t::MAX_DIFF����ѹʱ�ͻ��һ�㿪����˳�ν�ѹ�����õ�ǰ׺��
		// �����ͬǰ׺�Ĵʺܶ࣬�����Ĵʣ��� diff ���ܾͻᳬ����Χ���Ӷ������������
		// ǰ׺���ܸ� key ��ͬ����С string���� iterator ������ diff �ı�ﷶΧ�ڣ�
		if (iBack - iPrefix > zstr_t::MAX_DIFF - 1)
			iPrefix = iBack - (zstr_t::MAX_DIFF - 1);

		iterator iLower = lower_bound(iPrefix, end(), key, comp);

		// now [iLower, end) has the same (prefix length = comp.getPrefix())
		uint_t diff = end() - iLower;

		push_back(key, val, comp.getPrefix(), diff);
	}
}

template<class ValueT, class CompareT, uint_t FlagLength>
void ZippedTable<ValueT, CompareT, FlagLength>::push_back(
	const std::string& key, const value_type& val, uint_t nPreifx, uint_t diff)
{
	m_index.back().val = val;

	uint32_t backOffset = m_pool.put(key, nPreifx, diff);

	uint32_t offset = m_pool.tell();
	m_index.push_back(node_t(offset)); // push a dummy

	// ѹ���ĳߴ����� string �ĳߴ�С(��key��һ���ֽ�Ϊ�����ַ�����)
	assert(offset - backOffset <= key.size() + 1);
}

template<class ValueT, class CompareT, uint_t FlagLength>
void ZippedTable<ValueT, CompareT, FlagLength>::pop_back()
{
	m_pool.seek(m_index.end()[-2].offset);
	m_index.pop_back();
}

} } // namespace terark::prefix_zip


#endif // __zip_dict_h__

