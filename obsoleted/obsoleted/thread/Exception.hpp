/* vim: set tabstop=4 : */
#ifndef daqi_thread_exception_h__
#define daqi_thread_exception_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <exception>
#include <string>
#include <stdio.h>
#include <terark/config.hpp>

namespace terark { namespace thread {

/**
 @defgroup thread ���̱߳��
 @{
  - Ŀǰthread��֧��linux pthread��WinThread��\n
	boost��Ҳ��thread�⣬��ʹ���ио�����������\n
	����ȱ��ĳЩ����������Semaphore��boost�Ľ�����semaphore���׳�����

  - ��ģ�黹�ṩ��һЩ���߳�ʵ����

  - �󲿷��඼��ƽ̨�޹صģ�ֻ���⼸������ƽ̨��صģ� Thread, MutexLock, ReadWriteLock, Semaphore
 */

/**
 @defgroup ThreadObject �̶߳���
 @{
 @par ����Ҫ�� Thread �������࣬���磺

 @code
 #include <terark/thread/thread.hpp>

 class MyThread : public Thread
 {
 protected:
 	void run()
	{
		// do something...
	}
 };
 int main()
 {
 	MyThread thread1;
	thread1.start();

	// do something....

	return 0;
 }
 @endcode
 @see Thread, MemberFunThread
 */

//@}
//@}

/**
 @ingroup thread
 @name �߳��쳣��
 @{
 */

class TERARK_DLL_EXPORT ThreadException : public std::exception
{
protected:
	std::string m_message;
public:
	ThreadException(const char* szMsg = "terark::thread::ThreadException")
		: m_message(szMsg)
	{ }
	virtual ~ThreadException() throw() {}

	const char* what() const throw() { return m_message.c_str(); }
};

class TERARK_DLL_EXPORT LockException : public ThreadException
{
public:
	LockException(const char* szMsg = "terark::thread::LockException")
		: ThreadException(szMsg)
	{ }
};

class TERARK_DLL_EXPORT TimeoutException : public ThreadException
{
	int m_timeout;
public:
	TimeoutException(int timeout=-1, const char* szMsg = "terark::thread::TimeoutException")
		: ThreadException(szMsg), m_timeout(timeout)
	{
		if (-1 != timeout) {
			char szBuf[64];
			sprintf(szBuf, ": wait before [%d] milliseconds.", timeout);
			m_message.append(szBuf);
		}
   	}
};

class TERARK_DLL_EXPORT DeadLockException : public ThreadException
{
public:
	DeadLockException(const char* szMsg = "terark::thread::DeadLockException")
		: ThreadException(szMsg)
	{ }
};

class TERARK_DLL_EXPORT InterruptedException : public ThreadException
{
public:
	InterruptedException(const char* szMsg = "terark::thread::InterruptedException")
		: ThreadException(szMsg)
	{ }
};

//@}

} } // namespace terark::thread

#endif // daqi_thread_exception_h__


