/* vim: set tabstop=4 : */
#ifndef __terark_thread_lockable_h__
#define __terark_thread_lockable_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <assert.h>
#include <terark/stdtypes.hpp>
#include "../thread/SynchObject.hpp"

namespace terark { namespace thread {

/**
 @addtogroup thread
 @{
 @defgroup lockable Ϊ����������������
 @{

  �����κ�һ���� Class��ʹ�� SomeLockable<Class> �Ϳ���ʹ�������Ա� SomeLock ��
  ��������ʹ�����Ͷ����Ϊһ�壬�û�����Ͳ��õ����ĸ������õ����ĸ���

  @code
	Class1 x, y;
	MutexLock lock1, lock2;
	{ // code1
	   MutexLock::MutexLockSentry sentry(lock1);
	   x.func();
	   // do something...
	}
	//...
	{ // code2
	   MutexLock::MutexLockSentry sentry(lock1); // should be lock2
	   // do something...
	   y.func();
	}
	//...
  @endcode
	ʹ�� MutexLockable....
  @code
	typedef MutexLockable<Class1> MutexClass1;
	MutexClass1 x, y;
	{ // code1
	   MutexClass1::MutexLockSentry sentry(x);
	   x.func();
	   //...
	}
	//....
	{ // code2
	   MutexClass1::MutexLockSentry sentry(y);
	   y.func();
	   //...
	}
  @endcode
 */

/**
 @defgroup LockableTraits ������������

  ͨ�� LockableTraits<Class>::LockableTag ������ Class ��������
  @{
 */

/// ���� tag �Ļ��࣬������������
struct AnyLockableTag {};

/// ��������
struct NoneLockableTag  : public AnyLockableTag {};

/// �ɻ�����
struct MutexLockableTag : public AnyLockableTag {};

/// ���ź�����
struct SemaphoreableTag : public AnyLockableTag {};

/// concurrent queue
struct PutGetLockableTag : public MutexLockableTag {};

/// �ɶ�д��
struct ReadWriteLockableTag : public AnyLockableTag {};

/// ����������Ԫ��
struct ContainerElementLockableTag : public ReadWriteLockableTag {};

/// Ϊ���� Traits �������ǲ�������
template<class Class>
struct LockableTraits
{
	typedef NoneLockableTag LockableTag;
};

//@}

/**
 @brief ����ģ���������
 */
template<class Class>
class DummyLockable : public Class
{
public:
	typedef Class Base;

	void lock()    const throw() { }
	void unlock()  const throw() { }
	void trylock() const throw() { }
};
template<class Class>
struct LockableTraits<DummyLockable<Class> >
{
	typedef NoneLockableTag LockableTag; ///< ������
};
class DummyLock : public DummyLockable<EmptyClass> {};

template<class Class>
class MutexLockable : public Class
{
	MutexLock m_mutex;

public:
	typedef MutexLockSentryable<MutexLockable> MutexLockSentry;

	void lock()    { m_mutex.lock(); }
	void unlock()  { m_mutex.unlock(); }
	bool trylock() { return m_mutex.trylock(); }
};

template<class Class>
struct LockableTraits<MutexLockable<Class> >
{
	typedef MutexLockableTag LockableTag; ///< ������
};

/**
 @brief �ɻ���������Ĵ���
 */
template<class MutexLockableClass>
class MutexLockableProxy
{
	MutexLockableClass* m_pLockableObject;

public:
	typedef MutexLockSentryable<MutexLockableProxy> MutexLockSentry;

	MutexLockableProxy(MutexLockableClass* destObject)
		: m_pLockableObject(destObject) { }

	void lock()    { m_pLockableObject->lock(); }
	void unlock()  { m_pLockableObject->unlock(); }
	bool trylock() { return m_pLockableObject->trylock(); }
};
template<class Class>
struct LockableTraits<MutexLockableProxy<Class> >
{
	typedef MutexLockableTag LockableTag;
};

/**
 @brief �ɻ������������չ����
 */
template<class Class, class MutexLockableClass>
class ExtMutexLockable :
	public Class, public MutexLockableProxy<MutexLockableClass>
{
public:
	ExtMutexLockable(MutexLockableClass* pMutexLockable)
		: MutexLockableProxy<MutexLockableClass>(pMutexLockable)
	{ }
	typedef MutexLockSentryable<ExtMutexLockable> MutexLockSentry;
};

template<class Class, class MutexLockableClass>
struct LockableTraits<ExtMutexLockable<Class, MutexLockableClass> >
{
	typedef MutexLockableTag LockableTag;
};

/**
 @brief �ɶ�д��
 */
template<class Class>
class ReadWriteLockable : public Class
{
	ReadWriteLock m_rwlock;

public:
	typedef  ReadLockSentryable<ReadWriteLockable>  ReadLockSentry;
	typedef WriteLockSentryable<ReadWriteLockable> WriteLockSentry;

	void readLock()  { m_rwlock.readLock();  }
	void writeLock() { m_rwlock.writeLock(); }
	void unlock()	 { m_rwlock.unlock();    }

	///@brief timeout is in milliseconds.
	bool readLock(int timeout)  { return m_rwlock.readLock(timeout);  }
	bool writeLock(int timeout) { return m_rwlock.writeLock(timeout); }
};
template<class Class>
struct LockableTraits<ReadWriteLockable<Class> >
{
	typedef ReadWriteLockableTag LockableTag;
};

/**
 @brief �ɶ�д������Ĵ���
 */
template<class ReadWriteLockableClass>
class ReadWriteLockableProxy
{
	ReadWriteLockableClass* m_pLockableObject;

public:
	typedef  ReadLockSentryable<ReadWriteLockableProxy>  ReadLockSentry;
	typedef WriteLockSentryable<ReadWriteLockableProxy> WriteLockSentry;

	ReadWriteLockableProxy(ReadWriteLockableClass* destObject)
		: m_pLockableObject(destObject) { }

	void readLock()  { return m_pLockableObject->readLock(); }
	void writeLock() { return m_pLockableObject->writeLock(); }

	void unlock()	 { return m_pLockableObject->unlock(); }

	bool readLock(int timeout)  { return m_pLockableObject->readLock(timeout); }
	bool writeLock(int timeout) { return m_pLockableObject->writeLock(timeout); }
};
template<class Class>
struct LockableTraits<ReadWriteLockableProxy<Class> >
{
	typedef ReadWriteLockableTag LockableTag;
};

/**
 @brief �ɶ�д���������չ����
 */
template<class Class, class ReadWriteLockableClass>
class ExtReadWriteLockable :
	public Class, public ReadWriteLockableProxy<ReadWriteLockableClass>
{
public:
	ExtReadWriteLockable(ReadWriteLockableClass* pMutexLockable)
		: ReadWriteLockableProxy<ReadWriteLockableClass>(pMutexLockable)
	{ }
	typedef  ReadLockSentryable<ExtReadWriteLockable>  ReadLockSentry;
	typedef WriteLockSentryable<ExtReadWriteLockable> WriteLockSentry;
};

template<class Class, class ReadWriteLockableClass>
struct LockableTraits<ExtReadWriteLockable<Class, ReadWriteLockableClass> >
{
	typedef ReadWriteLockableTag LockableTag;
};

/**
 @brief ���źŶ���
 */
template<class Class>
class Semaphoreable : public Class
{
protected:
	Semaphore m_sem;
private:
	Semaphoreable(const Semaphoreable&);
	Semaphoreable& operator=(const Semaphoreable&);
public:
	Semaphoreable(int count, int shared = 0)
		: m_sem(count, shared)
   	{ }

	void wait() { m_sem.wait(); }

	bool wait(int timeout) { return m_sem.wait(timeout); }
	void post() { m_sem.post(); }

#if defined(_WIN32) || defined(WIN32)
	void post(int count) { m_sem.post(count); }
#else
	bool wait(const ::timespec& abstimeout) { return m_sem.wait(abstimeout); }
#endif

	int getValue() { return m_sem.getValue();	}
};
template<class Class>
struct LockableTraits<Semaphoreable<Class> >
{
	typedef SemaphoreableTag LockableTag;
};

//@} // end defgroup
//@} // end addtogroup

} } // namespace terark::thread

#endif // __terark_thread_lockable_h__

