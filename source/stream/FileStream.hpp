#pragma once
#include "Stream.hpp"
#include <stdio.h>

class FileStream : public Stream
{
public :
	FileStream(const char* path, bool read);
	~FileStream();
	void Close();
	int Read(void* buf, size_t size);
	int Write(const void* buf, size_t size);
	int Seek(int offset, SeekType type);
	int Position() const;

private :
	FILE* fp_;
};
