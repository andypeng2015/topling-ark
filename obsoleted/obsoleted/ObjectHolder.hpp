/* vim: set tabstop=4 : */
/********************************************************************
	@file ObjectHolder.hpp
	@brief �ṩ���û��ֹ�������󴴽���ȡֵ�����ٵĹ���

	@date	2006-9-28 14:59
	@author	Lei Peng
	@{
*********************************************************************/
#ifndef ObjectHolder_h__
#define ObjectHolder_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <assert.h>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_base_of.hpp>

namespace terark {
/**
 @brief ������һ��� ObjectType �ߴ���ȵ��ڴ��Ϲ��� ObjectType ����

  - ������һ��� ObjectType �ߴ���ȵ��ڴ��Ϲ��� ObjectType ����������ڴ��ϣ�
    ���û��ֹ���������ȡ�����ٱ����ݵĶ���

  - ���������������Ҫ������ǰ��������������㣬
    Debug �����������ʧ�ܣ�Release ��ᵼ��δ������Ϊ

 @see circular_queue HeavyObjectPool MemberFunThread LockableContainer_ElemRef
 */
template<class ObjectType>
class ObjectHolder
{
#if defined(_DEBUG) || defined(DEBUG) || !defined(NDEBUG)
#pragma message("debug version of ObjectHolder...")
	bool m_isValid;
#endif
	char m_data[sizeof(ObjectType)];

public:
	typedef ObjectType value_type;

#if defined(_DEBUG) || defined(DEBUG) || !defined(NDEBUG)
	ObjectHolder() : m_isValid(false) {}
#endif

	template<class DerivedClass>
	ObjectHolder<DerivedClass>& holder_cast()
	{
		BOOST_STATIC_ASSERT((boost::is_base_of<ObjectType, DerivedClass>::value));
		BOOST_STATIC_ASSERT(sizeof(ObjectType) == sizeof(DerivedClass));
		assert(m_isValid);
		return *(ObjectHolder<DerivedClass>*)(m_data);
	}

	/**
	 @brief ���ٱ����ݵĶ���

	  -ǰ�����������ѱ����쵫δ������
	  -��������������ѱ�����
	 */
	void destroy()
	{
		assert(m_isValid);
		ObjectType* p = reinterpret_cast<ObjectType*>(m_data);
		p->~ObjectType();
#if defined(_DEBUG) || defined(DEBUG) || !defined(NDEBUG)
		m_isValid = false;
#endif
	}

	/**
	 @brief ����˵��

  	  -ǰ����������δ������
	  -��������������ѱ�����
	 */
	void construct()
	{
		assert(!m_isValid);
		new (m_data) ObjectType();
#if defined(_DEBUG) || defined(DEBUG) || !defined(NDEBUG)
		m_isValid = true;
#endif
	}
	void construct(const ObjectType& x)
	{
		assert(!m_isValid);
		new (m_data) ObjectType(x);
#if defined(_DEBUG) || defined(DEBUG) || !defined(NDEBUG)
		m_isValid = true;
#endif
	}

	template<class T1>
	void construct(const T1& p1)
	{
		assert(!m_isValid);
		new (m_data) ObjectType(p1);
#if defined(_DEBUG) || defined(DEBUG) || !defined(NDEBUG)
		m_isValid = true;
#endif
	}
	template<class T1, class T2>
	void construct(const T1& p1, const T2& p2)
	{
		assert(!m_isValid);
		new (m_data) ObjectType(p1, p2);
#if defined(_DEBUG) || defined(DEBUG) || !defined(NDEBUG)
		m_isValid = true;
#endif
	}
	template<class T1, class T2, class T3>
	void construct(const T1& p1, const T2& p2, const T3& p3)
	{
		assert(!m_isValid);
		new (m_data) ObjectType(p1, p2, p3);
#if defined(_DEBUG) || defined(DEBUG) || !defined(NDEBUG)
		m_isValid = true;
#endif
	}
	template<class T1, class T2, class T3, class T4>
	void construct(const T1& p1, const T2& p2, const T3& p3, const T4& p4)
	{
		assert(!m_isValid);
		new (m_data) ObjectType(p1, p2, p3, p4);
#if defined(_DEBUG) || defined(DEBUG) || !defined(NDEBUG)
		m_isValid = true;
#endif
	}
	template<class T1, class T2, class T3, class T4, class T5>
	void construct(const T1& p1, const T2& p2, const T3& p3, const T4& p4, const T5& p5)
	{
		assert(!m_isValid);
		new (m_data) ObjectType(p1, p2, p3, p4, p5);
#if defined(_DEBUG) || defined(DEBUG) || !defined(NDEBUG)
		m_isValid = true;
#endif
	}

	/**
	 @brief �������ֵ����������� x

  	  -ǰ�����������ѱ�����
	  -��������������ֵ������������� x����������
	 */
	void assignto(ObjectType& x)
	{
		x = value();
		destroy();
	}

	/**
	 @brief ��ȡ�����ݵĶ���

  	  -ǰ�����������ѱ�����
	  @return �����ݵĶ���
	 */
	ObjectType& value() throw()
	{
		assert(m_isValid);
		return *reinterpret_cast<ObjectType*>(m_data);
	}

	/**
	 @brief ��ȡ�����ݵĶ���

  	  -ǰ�����������ѱ�����
	  @return �����ݵĶ���
	 */
	const ObjectType& value() const throw()
	{
		assert(m_isValid);
		return *reinterpret_cast<const ObjectType*>(m_data);
	}
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

template<class Object>
class PointerHolder
{
	Object* m_p;

public:
	typedef Object object_t;

	PointerHolder(Object* obj) : m_p(obj) {}

	const Object& getObject() const { return *m_p; }
	      Object& getObject()       { return *m_p; }

	void setPointer(Object* p) { m_p = p; }
};

template<class Object>
class NestHolder
{
	Object m_x;

private:
	NestHolder(const Object& x); // disable

public:
	typedef Object object_t;

	NestHolder() {}

	template<class T0>
	explicit NestHolder(const T0& x0)
		: m_x(x0)
	{}
	template<class T0, class T1>
	NestHolder(const T0& x0, const T1& x1)
		: m_x(x0, x1)
	{}
	template<class T0, class T1, class T2>
	NestHolder(const T0& x0, const T1& x1, const T2& x2)
		: m_x(x0, x1, x2)
	{}

	template<class T0, class T1, class T2, class T3, class T4,
			 class T5, class T6, class T7, class T8, class T9>
	NestHolder(const T0& x0, const T1& x1, const T2& x2, const T3& x3, const T4& x4,
			  const T5& x5, const T6& x6, const T7& x7, const T8& x8, const T9& x9)
    : m_x(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9)
	{}

	const Object& getObject() const { return m_x; }
	      Object& getObject()       { return m_x; }
};

} // namespace terark

#endif // ObjectHolder_h__

// @} end file ObjectHolder.hpp

