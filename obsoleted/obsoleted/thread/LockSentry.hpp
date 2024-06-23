/* vim: set tabstop=4 : */
/********************************************************************
	@file LockSentry.hpp
	@brief �ṩ���� LockSentry ��λ����

	@date	2006-9-28 17:26
	@author	Lei Peng
	@{
*********************************************************************/
#ifndef __terark_thread_LockSentry_h__
#define __terark_thread_LockSentry_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <assert.h>
#include <boost/static_assert.hpp>
#include <terark/stdtypes.hpp>

namespace terark { namespace thread {

/**
 @addtogroup thread
 @{
 */

/**
 @defgroup LockSentry LockSentry ��λ����
 @{
  - ���ʣ���λ(Sentry) ��ָ���� C++ �ġ���Դ���뼴��ʼ�������ԣ�������Ҫ���ض�˳����õĺ������С�
    ����ʱ����һ������������ʱ��������һ��������������ǿ׳����ȫ�ĳ���

  - ʹ����λ������Ա������ǽ������쳣ʱ��й©�ȴ������õĳ���Ӧ��ʹ����λ��

  - ��ģ���ṩ�������Զ����������������λ��

  - ��λ�ĵ����÷����£�

  @code
	{// make a scope
		SomeLockSentry sentry(lockableObject);
		// do something...
	}
  @endcode

  - �������Ͻ���������Ҫ�����Ĵ���飬�����Խ�����λ�Ϳ���ʵ�֡�\n
	��ˣ�boost::thread �е�����������ֻ����ʹ�� sentry������ scoped_lock.

  - ��ģ��ʵ�ֵ���λ
	- �����Զ���λ�����ݱ������Ķ������ͣ��Զ��Ƶ����ʵ���λ���Ͳ�ʵ�������ǳ�����ʹ�á�
		@see LockSentry, MutexLockSentry, ReadLockSentry, WriteLockSentry
	- �̶���λ��ֻ������һ�����͵���λ����������õ�ִ�д��롿
		@see MutexLockSentryable, ReadLockSentryable, WriteLockSentryable
 @}
 */
//@}

/**
 @ingroup LockSentry
 @brief �Զ��Ƶ��� sentry �Ķ������Ͳ�������ȷ�� sentry ����

  ʹ�ô���Ĳ������ͣ�ͨ�������Ƶ�������һ��������󣬸ô�������� LockProxyInterface
  �����������ֻռ����ָ��Ŀռ䣬��˰������� m_lockProxyBuf �У��ֹ�ʹ�� placement new
  ������ô�����󣬾Ͳ���ʹ�� new-delete���Ӷ������˿��������еĶ��⿪����ֻ���麯�������ϡ�

 @note �����ֻ��MutexLockSentry/ReadLockSentry/WriteLockSentry �Ĺ������࣬���� terark ���ڲ�ʹ��
 @note �û������ܴ��������Ķ��� can not create a 'LockSentry' by user code!
 */
class LockSentry
{
protected:
	/**
	 @brief ����������ĳ������
	 */
	class LockProxyInterface
	{
	public:
		virtual void lock() = 0;
		virtual void unlock() = 0;
	};

protected:
	char m_lockProxyBuf[2 * sizeof(void*)]; ///< ���� LockProxyInterface ���������Ŀռ�
	bool m_isLocked; ///< �Ƿ����������������������ʱ��ͨ�� LockProxyInterface �ӿڵ��� unlock

protected:
	/**
	 @brief sentry �ĺ���ʵ�ֺ���

	 @param xlock Ҫ���Զ������Ķ���
	 @param tag   δ�ã������������Ƶ�
	 */
	template<class LockableType, class LockProxyType>
	void sentry_imp(LockableType& xlock, LockProxyType* tag)
	{
		// @note if this STATIC_ASSERT failed, make m_lockProxyBuf a little bigger!!
		BOOST_STATIC_ASSERT(sizeof(m_lockProxyBuf) >= sizeof(LockProxyType));
		assert(!m_isLocked);
		new(m_lockProxyBuf)LockProxyType(xlock);
		LockProxyInterface* plock = (LockProxyInterface*)m_lockProxyBuf;
		plock->lock(); // maybe throw exceptions..

		// @note must at the end, for exception safe!!
		m_isLocked = true;
	}

	LockSentry() { m_isLocked = false; } // only can be used by derived class
	~LockSentry()
	{
		if (m_isLocked) {
			// @note ��ʱ����֪�� xlock �ľ������ͣ�ֻ֪������ LockProxyInterface �ӿ�
			LockProxyInterface* xlock = (LockProxyInterface*)m_lockProxyBuf;
			xlock->unlock();
		}
	}

public:
	/**
	 @brief �ֹ��ͷŸö������

	  �������Ͻ����ú������Ǳ���ģ�����ʹ���˸ú����Ĵ��룬
	  ��������дΪ����Ҫ���øú���������һ����ʽ��

     @note ���Ƽ�ʹ�øú���
	 */
	void release()
	{
		assert(m_isLocked);
		LockProxyInterface* xlock = (LockProxyInterface*)m_lockProxyBuf;
		xlock->unlock();
		m_isLocked = false;
	}

	/**
	 @brief �жϸö����Ƿ��Ѽ���

	 @note ������ʹ�øú���
	 */
	bool isLocked() const throw() { return m_isLocked; }
};

/**
 @ingroup LockSentry
 @brief ����������λ����

  ���ڿ��Ա��ӻ������Ķ���
  can be used for 'DummyLockable' also!
 */
class MutexLockSentry : public LockSentry
{
	DECLARE_NONE_COPYABLE_CLASS(MutexLockSentry)

private:
	/**
	 @brief ���廥��������Ĵ���

	 @param MutexLockable �ɱ��������Ķ�������
	  �� MutexLockable::lock/unlock ��������� lock/unlock
	 */
	template<class MutexLockable>
	class LockProxy : public LockSentry::LockProxyInterface
	{
		MutexLockable* m_xlock;
	public:
		LockProxy(MutexLockable& xlock) : m_xlock(&xlock) {}

		void lock()   { m_xlock->lock();   }
		void unlock() { m_xlock->unlock(); }
	};

public:
	MutexLockSentry() { }

	/**
	 @brief ���캯��
	 @param xlock Ҫ�������Ķ���

	 ͨ�����������Զ��Ƶ���ʵ������ģ���� LockProxy<MutexLockable>����ʵ������ôһ������
	 */
	template<class MutexLockable>
	MutexLockSentry(MutexLockable& xlock) { sentry(xlock); }

	/**
	 @brief �ṩͬһ������������Զ��������������Ĺ���

	 @param xlock Ҫ�������Ķ���

	 - ͨ�����������Զ��Ƶ���ʵ������ģ���� LockProxy<MutexLockable>����ʵ������ôһ������

	 @note ������ʹ��
	 */
	template<class MutexLockable>
	void sentry(MutexLockable& xlock)
	{
		sentry_imp(xlock, (LockProxy<MutexLockable>*)(0));
	}
};

/**
 @ingroup LockSentry
 @brief ��������λ����

  ���ڿ��Ա��Ӷ����Ķ���
 */
class ReadLockSentry : public LockSentry
{
	DECLARE_NONE_COPYABLE_CLASS(ReadLockSentry)

private:
	/**
	 @brief ���廥��������Ĵ���

	 @param ReadLockable �ɱ��������Ķ�������
	  �� ReadLockable::readLock/unlock ��������� lock/unlock
	 */
	template<class ReadLockable>
	class LockProxy : public LockSentry::LockProxyInterface
	{
		ReadLockable* m_xlock;
	public:
		LockProxy(ReadLockable& xlock) : m_xlock(&xlock) {}

		void lock()   { m_xlock->readLock(); }
		void unlock() { m_xlock->unlock(); }
	};

public:
	ReadLockSentry() { }

	/**
	 @brief ���캯��
	 @param xlock Ҫ�������Ķ���

	 ͨ�����������Զ��Ƶ���ʵ������ģ���� LockProxy<ReadLockable>����ʵ������ôһ������
	 */
	template<class ReadLockable>
	ReadLockSentry(ReadLockable& xlock) { sentry(xlock); }

	/**
	 @brief �ṩͬһ������������Զ��������������Ĺ���

	 @param xlock Ҫ�������Ķ���

	 - ͨ�����������Զ��Ƶ���ʵ������ģ���� LockProxy<ReadLockable>����ʵ������ôһ������

	 @note ������ʹ��
	 */
	template<class ReadLockable>
	void sentry(ReadLockable& xlock)
	{
		sentry_imp(xlock, (LockProxy<ReadLockable>*)(0));
	}
};

/**
 @ingroup LockSentry
 @brief д������λ����

  ���ڿ��Ա���д���Ķ���
 */
class WriteLockSentry : public LockSentry
{
	DECLARE_NONE_COPYABLE_CLASS(WriteLockSentry)

private:
	/**
	 @brief ���廥��������Ĵ���

	 @param WriteLockable �ɱ��������Ķ�������
	  �� WriteLockable::writeLock/unlock ��������� lock/unlock
	 */
	template<class WriteLockable>
	class LockProxy : public LockSentry::LockProxyInterface
	{
		WriteLockable* m_xlock;
	public:
		LockProxy(WriteLockable& xlock) : m_xlock(&xlock) {}

		void lock()   { m_xlock->writeLock(); }
		void unlock() { m_xlock->unlock(); }
	};

public:
	WriteLockSentry() { }

	/**
	 @brief ���캯��
	 @param xlock Ҫ�������Ķ���

	 ͨ�����������Զ��Ƶ���ʵ������ģ���� LockProxy<WriteLockable>����ʵ������ôһ������
	 */
	template<class WriteLockable>
	WriteLockSentry(WriteLockable& xlock) { sentry(xlock); }

	/**
	 @brief �ṩͬһ������������Զ��������������Ĺ���

	 @param xlock Ҫ�������Ķ���

	 - ͨ�����������Զ��Ƶ���ʵ������ģ���� LockProxy<WriteLockable>����ʵ������ôһ������

	 @note ������ʹ��
	 */
	template<class WriteLockable>
	void sentry(WriteLockable& xlock)
	{
		sentry_imp(xlock, (LockProxy<WriteLockable>*)(0));
	}
};

//////////////////////////////////////////////////////////////////

/**
 @ingroup LockSentry
 @brief �ṩ�������� MutexLockableClass �Ĺ���

  - ���Զ��� MutexLockSentry �ĺô����ڼ������麯�����ã����Ҽ����˴��������
  - ��Ϊ�Զ��� MutexLockSentry ����һ�������ɴ�����Ĵ�����麯�����������ģ��ͨ���������Ż���
    ��ת��Ϊ�� MutexLockableClass ��Ա������ֱ�ӵ����ˡ�
  - ��ˣ�ʹ�������ģ����Զ��� MutexLockSentry ����Ч
  - �� MutexLockableClass �ڲ�ʹ��һ��
  @code typedef MutexLockSentryable<MutexLockableClass> MutexLockSentry; @endcode
    ���Ժܷ����ʹ��
  @code MutexLockableClass::MutexLockSentry sentry(object); @endcode
 */
template<class MutexLockableClass>
class MutexLockSentryable
{
	MutexLockableClass& m_xobj;
public:
	MutexLockSentryable(MutexLockableClass& xobj)
	   	: m_xobj(xobj) { xobj.lock(); }
	~MutexLockSentryable() { m_xobj.unlock(); }
};

/**
 @ingroup LockSentry
 @brief ���� MutexLockSentryable
 @see MutexLockSentryable
 */
template<class ReadLockableClass>
class ReadLockSentryable
{
	ReadLockableClass& m_xobj;
public:
	ReadLockSentryable(ReadLockableClass& xobj)
	   	: m_xobj(xobj) { xobj.readLock(); }
	~ReadLockSentryable() { m_xobj.unlock(); }
};

/**
 @ingroup LockSentry
 @brief ���� MutexLockSentryable
 @see MutexLockSentryable
 */
template<class WriteLockableClass>
class WriteLockSentryable
{
	WriteLockableClass& m_xobj;
public:
	WriteLockSentryable(WriteLockableClass& xobj)
	   	: m_xobj(xobj) { xobj.writeLock(); }
	~WriteLockSentryable() { m_xobj.unlock(); }
};

} } // namespace terark::thread

#endif // __terark_thread_LockSentry_h__

// @} end file LockSentry.hpp

