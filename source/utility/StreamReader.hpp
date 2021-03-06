#pragma once

#include <vector>
#include <string>

class StreamReader
{
public:
	StreamReader() : buffer_(nullptr), offset_(0)
	{
	}

	StreamReader(std::vector<char>& buffer) : buffer_(&buffer), offset_(0)
	{
	}

	/*!
	 * @brief bool型の読み込み
	 *
	 * @return 読み込んだbool型の値
	 */
	bool ReadBool()
	{
		if (offset_ + sizeof(bool) > buffer_->size()) return 0;
		auto ret = (*(unsigned char*)&(*buffer_)[offset_]) != 0;
		offset_ += sizeof(bool);
		return ret;
	}

	/*!
	 * @brief bool型の読み込み(シークなし)
	 *
	 * @return 読み込んだbool型の値
	 */
	bool ReadBoolNoSeek()
	{
		auto temp = offset_;
		auto ret = ReadBool();
		offset_ = temp;
		return ret;
	}

	/*!
	 * @brief int型の読み込み
	 *
	 * @return 読み込んだint型の値
	 */
	int ReadInt()
	{
		if (offset_ + sizeof(int) > buffer_->size()) return 0;
		auto ret = *(int*)&(*buffer_)[offset_];
		offset_ += sizeof(int);
		return ret;
	}

	/*!
	 * @brief int型の読み込み(シークなし)
	 *
	 * @return 読み込んだint型の値
	 */
	int ReadIntNoSeek()
	{
		auto temp = offset_;
		auto ret = ReadInt();
		offset_ = temp;
		return ret;
	}

	/*!
	 * @brief float型の読み込み
	 *
	 * @return 読み込んだfloat型の値
	 */
	float ReadFloat()
	{
		if (offset_ + sizeof(float) > buffer_->size()) return 0;
		auto ret = *(float*)&(*buffer_)[offset_];
		offset_ += sizeof(float);
		return ret;
	}

	/*!
	 * @brief float型の読み込み(シークなし)
	 *
	 * @return 読み込んだfloat型の値
	 */
	float ReadFloatNoSeek()
	{
		auto temp = offset_;
		auto ret = ReadFloat();
		offset_ = temp;
		return ret;
	}

	/*!
	 * @brief 文字列の読み込み
	 *
	 * @return 読み込んだ文字列
	 */
	std::string ReadString()
	{
		int len = ReadInt();
		std::string ret = std::string(&(*buffer_)[offset_], len);
		offset_ += ret.length();
		return ret;
	}

	/*!
	 * @brief 文字列の読み込み(シークなし)
	 *
	 * @return 読み込んだ文字列
	 */
	std::string ReadStringNoSeek()
	{
		auto temp = offset_;
		auto ret = ReadString();
		offset_ = temp;
		return ret;
	}

private :
	std::vector<char>* buffer_;
	size_t offset_;
};
