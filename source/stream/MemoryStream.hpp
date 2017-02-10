#pragma once
#include "Stream.hpp"
#include <vector>

class MemoryStream : public Stream
{
public :
	MemoryStream();
	MemoryStream(std::vector<char>& buf);
	~MemoryStream();
	void Close();
	int Read(void* buf, size_t size);
	int Write(const void* buf, size_t size);
	int Seek(int offset, SeekType type);
	int Position() const;

private :
	std::vector<char> buf_;
	size_t offset_;
};
