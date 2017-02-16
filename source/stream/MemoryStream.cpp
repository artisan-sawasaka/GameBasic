#include "MemoryStream.hpp"

MemoryStream::MemoryStream()
	: write_buf_()
	, read_buf_(nullptr)
	, offset_(0)
{
}

MemoryStream::MemoryStream(int count)
	: write_buf_(count)
	, read_buf_(nullptr)
	, offset_(0)
{
}

MemoryStream::MemoryStream(const std::vector<char>& buf)
	: write_buf_()
	, read_buf_(&buf)
	, offset_(0)
{
}

MemoryStream::~MemoryStream()
{
	Close();
}

void MemoryStream::Close()
{
	std::vector<char>().swap(write_buf_);
	offset_ = 0;
}

int MemoryStream::Read(void* buf, size_t size)
{
	if (read_buf_ == nullptr) return -1;
	if (offset_ >= read_buf_->size()) return -1;

	if (offset_ + size >= read_buf_->size()) {
		size = read_buf_->size() - offset_;
	}
	memcpy(buf, &(*read_buf_)[offset_], size);
	offset_ += size;

	return size;
}

int MemoryStream::Write(const void* buf, size_t size)
{
	if (read_buf_ != nullptr) return -1;

	if (offset_ + size >= write_buf_.size()) {
		write_buf_.resize(write_buf_.size() * 2);
	}
	memcpy(&write_buf_[offset_], buf, size);
	offset_ += size;

	return size;
}

int MemoryStream::Seek(int offset, SeekType type)
{
	if (type == Stream::Begin) {
		offset_ = offset;
	} else if (type == Stream::Current) {
		offset_ += offset;
	} else if (type == Stream::End) {
		if (read_buf_ != nullptr) {
			offset_ = read_buf_->size() + offset;
		} else {
			offset_ = write_buf_.size() + offset;
		}
	}

	return offset_;
}

int MemoryStream::Position() const
{
	return offset_;
}

std::vector<char> MemoryStream::ToArray()
{
	if (read_buf_ != nullptr) {
		return *read_buf_;
	} else {
		std::vector<char> ret(offset_);
		memcpy(&ret[0], &write_buf_[0], ret.size());
		return ret;
	}
}
