#pragma once
#include <stdlib.h>

class Stream
{
public :
	enum SeekType
	{
		Begin,
		Current,
		End,
	};

	virtual ~Stream(){}
	virtual void Close(){}
	virtual int Read(void* buf, size_t size) { return 0; }
	virtual int Write(const void* buf, size_t size) { return 0; }
	virtual int Seek(int offset, SeekType type){ return 0; }
	virtual int Position() const { return 0; }
};
