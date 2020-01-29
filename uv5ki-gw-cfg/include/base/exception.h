#ifndef _DFN_EXCEPTION_
#define _DFN_EXCEPTION_

#ifdef _WIN32
#	include <windows.h>
#else
#	include <errno.h>
#endif

#include <stdexcept> 
#include <string> 
#include <cstring>
#include <iostream>
#include <sstream>

using namespace std;

class Exception : public runtime_error
{
public:
	Exception(const char *s="");
	Exception(string s);
	Exception(string s, int error);
	~Exception(void) throw(){}
protected:

#if defined _WIN32
    virtual const char *Prefix()
    {
        return "WIN32";
	}
#elif defined __APPLE__
    virtual const char *Prefix() const
    {
        return "APPLE";
	}
#elif defined _PPC82xx_
    virtual const char *Prefix()
    {
        return "PPC";
	}
#else
    virtual const char *Prefix()
    {
        return "LINUX";
	}
#endif

private:
    unsigned long m_error;
    mutable string m_msg;
public:
	unsigned long Code(void);
#if defined _WIN32
	virtual const string what();
#elif defined __APPLE__
	virtual const string what() const _NOEXCEPT;
#elif defined _PPC82xx_
	virtual const string what();
#else
	virtual const string what();
#endif
	string Message() {
		return m_msg;
	}
};

#endif
