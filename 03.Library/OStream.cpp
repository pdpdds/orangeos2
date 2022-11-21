//stdlib.cpp
#include "OStream.h"
#include "console.h"

extern Console OrangeConsole;

namespace std
{
	OStream cout;		//OStream cout

	OStream& OStream::operator<<(char *cp)
	{
		OrangeConsole.Write(cp) ;
		return *this;
	}

	OStream& OStream::operator<<(char c)
	{
		OrangeConsole.WriteChar(c) ;
		return *this;
	}

	OStream& OStream::operator<<(unsigned char *cq)
	{
		OrangeConsole.Write((char*)cq);
		return *this;
	}

	OStream& OStream::operator<<(unsigned char c1)
	{
		OrangeConsole.WriteChar(c1) ;
		return *this;
	}
}

