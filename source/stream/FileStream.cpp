#include "FileStream.hpp"

FileStream::FileStream(const char* path, Type type)
	: fp_(nullptr)
{
	if (type == TypeRead) {
		fp_ = fopen(path, "rb");
	} else if (type == TypeWrite) {
		fp_ = fopen(path, "wb");
	} else if (type == TypeAdd) {
		fp_ = fopen(path, "ab");
	}
}

FileStream::~FileStream()
{
	Close();
}

void FileStream::Close()
{
	if (fp_ != nullptr) {
		fclose(fp_);
		fp_ = nullptr;
	}
}

int FileStream::Read(void* buf, size_t size)
{
	if (fp_ == nullptr) return -1;

	return fread(buf, 1, size, fp_);
}

int FileStream::Write(const void* buf, size_t size)
{
	if (fp_ == nullptr) return -1;

	return fwrite(buf, 1, size, fp_);
}

int FileStream::Seek(int offset, SeekType type)
{
	if (fp_ == nullptr) return -1;
	
	int seek_type[] = {
		SEEK_SET,
		SEEK_CUR,
		SEEK_END,
	};
	return fseek(fp_, offset, seek_type[type]);
}

int FileStream::Position() const
{
	if (fp_ == nullptr) return -1;

	return ftell(fp_);
}
