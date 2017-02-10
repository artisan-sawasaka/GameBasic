#include "MemoryStream.hpp"

MemoryStream::MemoryStream()
	: buf_()
	, offset_(0)
{
}

MemoryStream::MemoryStream(std::vector<char>& buf)
	: buf_(buf)
	, offset_(0)
{
}

MemoryStream::~MemoryStream()
{
	Close();
}

void MemoryStream::Close()
{
	std::vector<char>().swap(buf_);
	offset_ = 0;
}

int MemoryStream::Read(void* buf, size_t size)
{
	if (offset_ >= buf_.size()) return -1;

	if (offset_ + size >= buf_.size()) {
		size = buf_.size() - offset_;
	}
	memcpy(buf, &buf_[offset_], size);
	offset_ += size;

	return size;
}

int MemoryStream::Write(const void* buf, size_t size)
{
	if (offset_ + size >= buf_.size()) {
		buf_.resize(offset_ + size);
	}
	memcpy(&buf_[offset_], buf, size);
	offset_ += size;

	return size;
}

int MemoryStream::Seek(int offset, SeekType type)
{
	if (offset_ >= buf_.size()) return -1;
	
	if (type == Stream::Begin) {
		offset_ = offset;
	} else if (type == Stream::Current) {
		offset_ += offset;
	} else if (type == Stream::End) {
		offset_ = buf_.size() + offset;
	}
	return offset_;
}

int MemoryStream::Position() const
{
	if (offset_ >= buf_.size()) return -1;

	return offset_;
}
