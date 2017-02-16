#pragma once
#include "Stream.hpp"
#include <vector>

class MemoryStream : public Stream
{
public :
	MemoryStream();
	MemoryStream(int count);
	MemoryStream(const std::vector<char>& buf);
	~MemoryStream();
	void Close();
	int Read(void* buf, size_t size);
	int Write(const void* buf, size_t size);
	int Seek(int offset, SeekType type);
	int Position() const;
	std::vector<char> ToArray();

private :
	std::vector<char> write_buf_;
	const std::vector<char>* read_buf_;
	size_t offset_;
};
