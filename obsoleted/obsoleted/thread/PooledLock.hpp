/* vim: set tabstop=4 : */
#ifndef PooledLock_h__
#define PooledLock_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include "../HeavyObjectPool.hpp"

namespace terark { namespace thread {

//@{
/**
 @ingroup thread
 @brief �������

 ��һ��Ӧ����Ҫ�ǳ�Ƶ���ش���/��������ʱ�򣬴������������Ŀ���ռ��ϵͳ�ܿ������ؾͺܴ���\n
 �ṩ������Ŀ�ľ��Ǽ��ٴ���/�������Ŀ���
 */
class PooledMutex : public PooledObject<MutexLock>
{
	DECLARE_NONE_COPYABLE_CLASS(PooledMutex)
public:
	PooledMutex() {}
	void lock()	   { m_proxy->lock();    }
	void unlock()  { m_proxy->unlock();  }
	bool trylock() { return m_proxy->trylock(); }
};

class PooledReadWriteLock : public PooledObject<ReadWriteLock>
{
	DECLARE_NONE_COPYABLE_CLASS(PooledReadWriteLock)
public:
	PooledReadWriteLock() {}

	void readLock()  { m_proxy->readLock();  }
	void writeLock() { m_proxy->writeLock(); }
	void unlock()	 { m_proxy->unlock();    }

	bool readLock(int timeout)  { return m_proxy->readLock(timeout); }
	bool writeLock(int timeout) { return m_proxy->writeLock(timeout); }
};
//@}

} } // namespace terark::thread

#endif // PooledLock_h__
