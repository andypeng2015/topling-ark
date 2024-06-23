/* vim: set tabstop=4 : */
/********************************************************************
	@file thread.hpp
	@brief �̶߳���

	@date	2006-9-30 14:45
	@author	Lei Peng
	@{
*********************************************************************/
#ifndef __terark_thread_h__
#define __terark_thread_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include "thread_base.hpp"

#if defined(_WIN32) || defined(WIN32)
#	include "win32/thread_impl.hpp"
#else
#	include "posix/thread_impl.hpp"
#endif

#include <boost/function.hpp>

namespace terark { namespace thread {

/**
 @ingroup ThreadObject
 @brief ��һ����ͨ�����Ա����һ���߳���

  ��һ����ͨ�����Ա���������һ���̣߳����һ�����ж����ͬ���̣߳�ʹ�������ģ��ǳ�����
  @code
  class MyClass
  {
    private:
	 void threadProc1()
	 {
	   // do something...
	 }
	 void threadProc2()
	 {
	   // do something...
	 }
	 MemberFunThread<MyClass, &MyClass::threadProc1> thread1;
	 MemberFunThread<MyClass, &MyClass::threadProc2> thread2;
	public:
	 void fun()
	 {
	   thread1.startWith(this);
	   thread2.startWith(this);

	   // do something...

	 }
  };
  @endcode
 */
/**
 @ingroup ThreadObject
 @brief ��ģ�� MemberFunThread �Ĳ����ػ�

 ���Գ�Ա��������ֵΪ void ���ػ������ػ��� getReturnValue ��Ա����
 */
template<class Class, void (Class::*PtrToMemberFunction)()>
class MemberFunThread : public Thread
{
	Class* m_object;
public:
	explicit MemberFunThread(Class* pObject) : m_object(pObject) { }
	Class* getObject() const { return m_object; }
protected:
	void run()
	{
		(m_object->*PtrToMemberFunction)();
	}
};

class BindThread : public Thread
{
	boost::function0<void> m_func;
public:
	explicit BindThread(const boost::function0<void>& func) : m_func(func) {}
protected:
	void run()
	{
		m_func();
	}
};

}

} // namespace terark::thread

#endif // __terark_thread_h__

// @} end file thread.hpp

