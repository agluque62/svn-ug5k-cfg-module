#include "../../include/base/exception.h"

/** */
Exception::Exception(const char *s)
#ifdef _WIN32
	:runtime_error(""), m_error(GetLastError())
#else
	:runtime_error(""), m_error(errno)		// Obtener ultimo error en linux/mac
#endif
{
	m_msg = string(s);
}

/** */
Exception::Exception(string s)
#ifdef _WIN32
	:runtime_error(""), m_error(GetLastError())
#else
	:runtime_error(""), m_error(errno)		// Obtener ultimo error en linux/mac
#endif
{
	m_msg = s;
}

/** */
Exception::Exception(string s, int error)
	: runtime_error(""), m_error(error)
{
	m_msg = s;
}

/** */
unsigned long Exception::Code(void)
{
	return m_error;
}

/** */
#if defined _WIN32
const string Exception::what()
#elif defined __APPLE__
const string Exception::what() const _NOEXCEPT
#elif defined _PPC82xx_
const string Exception::what()
#else
const string Exception::what()
#endif
{
	try
	{
		std::ostringstream os;

		os << Prefix() << "(ERROR: " << m_error << "). ";
		if (m_msg.empty()==false)
			os << m_msg;
#ifndef _WIN32
		os << " " << strerror(m_error);
#endif
		return os.str();

//		if (m_msg.empty())
//		{
//#ifdef _WIN32
//			std::ostringstream os;
//			os << Prefix() << "(ERROR: " << m_error << "). " << runtime_error::what();
//			//char buf[256];
//			//if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, m_error, 0, (LPSTR)buf, sizeof(buf), 0))
//			//{
//			//	os << ": " << buf;
//			//}
//			m_msg = os.str();
//#else
//			// 
//			std::ostringstream os;
//			os << Prefix() << "(ERROR: " << m_error << "): " << runtime_error::what();
//			// os << ": " << strerror(m_error);
//			m_msg = os.str();
//#endif
//		}
//
//		return m_msg.c_str();
	}
	catch (...)
	{
		return "error on error";
	}
}

