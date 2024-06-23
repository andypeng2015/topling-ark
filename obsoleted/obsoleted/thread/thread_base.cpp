/* vim: set tabstop=4 : */
#include "../thread/Exception.hpp"
#include <terark/stdtypes.hpp>
#include <terark/stdtypes.hpp>
#include <typeinfo>
#include <errno.h>

#include "thread.hpp"

namespace terark { namespace thread {

ThreadBase::ThreadBase()
{
	m_exitCode = 0;
	m_exception = 0;
	m_state = state_Initial;
}

ThreadBase::~ThreadBase()
{
	delete m_exception;
}

void ThreadBase::onComplete()
{
	// do nothing...
}

void ThreadBase::threadMain()
{
	m_state = state_Running;
	try {
		run(); // run...........
		m_exitCode = 0;
	}
	catch (const ThreadException& exp) {
		m_exitCode = errno;
		m_exception = new ThreadException(exp);
		DEBUG_printf("%s::run() throw terark::thread::ThreadException, %s\n",
				typeid(*this).name(), exp.what());
	}
	catch (const std::exception& exp) {
		m_exitCode = errno;
		m_exception = new std::exception(exp);
		DEBUG_printf("%s::run() throw std::exception, %s\n",
				typeid(*this).name(), exp.what());
	}
	catch (...) {
		m_state = state_Completed;
		m_exitCode = errno;
		DEBUG_printf("%s::run() throw an unknown exception, errno=%d(0x%04d)\n",
				typeid(*this).name(), m_exitCode, m_exitCode);
		DEBUG_fflush(stdout);
		throw;
	}
	DEBUG_only(if (m_exception) perror("perror: "));

	m_state = state_Completed;

	onComplete();
}

void ThreadBase::threadProcImpl(void* param)
{
	ThreadBase* self = (ThreadBase*)(param);
	self->threadMain();
}

int ThreadBase::exitCode()
{
	switch (m_state)
	{
	default: {
		char szBuf[128];
		sprintf(szBuf, "terark::thread::ThreadBase::exitCode(), invalid m_state=%d", m_state);
		throw ThreadException(szBuf); }
	case state_Initial:
	case state_Ready:
		throw ThreadException("terark::thread::ThreadBase::exitCode(), thread not started!");
	case state_Running:
		throw ThreadException("terark::thread::ThreadBase::exitCode(), thread is still running!");
	case state_Completed:
		return m_exitCode;
	}
}

void ThreadBase::start(int stackSize)
{
	switch (m_state)
	{
	default: {
		char szBuf[128];
		sprintf(szBuf, "terark::thread::ThreadBase::start(), invalid m_state=%d", m_state);
		throw ThreadException(szBuf); }
	case state_Initial:
		// û����������ȵ��� do_start �������� m_state = state_Ready
		// ���п����� do_start �� m_state = state_Ready ֮�䣬���߳��Ѿ����У�
		// m_state �ѱ�����Ϊ state_Running...
		// ���Ա��������� m_state����ʧ��ʱ�ٻָ�
		m_state = state_Ready;
		try {
			do_start(stackSize);
		}
		catch (const ThreadException&)
		{
			// �̴߳���ʧ�ܣ��ָ�״̬�����׳��쳣
			m_state = state_Initial;
			throw;
		}
		break;
	case state_Ready:
	case state_Running:
		throw ThreadException("terark::thread::ThreadBase::start(), thread has been started!");
	case state_Completed:
		throw ThreadException(
			 "terark::thread::ThreadBase::start(), thread has been completed, can not start multi times!");
	}
}

void ThreadBase::join()
{
	switch (this->state())
	{
	default: {
		 char szBuf[128];
		 sprintf(szBuf, "terark::thread::ThreadBase::join(), invalid m_state=%d", m_state);
		 throw ThreadException(szBuf); }
	case state_Initial:
		 throw ThreadException("terark::thread::ThreadBase::join(), thread not started!");
	case state_Ready:
	case state_Running:
	case state_Completed:
		 do_join();
		 break;
	}
}

} } // namespace terark::thread
