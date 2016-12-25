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
	 * @brief intŒ^‚Ì“Ç‚İ‚İ
	 *
	 * @return “Ç‚İ‚ñ‚¾intŒ^‚Ì’l
	 */
	int ReadInt()
	{
		if (offset_ + sizeof(int) > buffer_->size()) return 0;
		auto ret = *(int*)&(*buffer_)[offset_];
		offset_ += sizeof(int);
		return ret;
	}

	/*!
	 * @brief intŒ^‚Ì“Ç‚İ‚İ(ƒV[ƒN‚È‚µ)
	 *
	 * @return “Ç‚İ‚ñ‚¾intŒ^‚Ì’l
	 */
	int ReadIntNoSeek()
	{
		auto temp = offset_;
		auto ret = ReadInt();
		offset_ = temp;
		return ret;
	}

	/*!
	 * @brief floatŒ^‚Ì“Ç‚İ‚İ
	 *
	 * @return “Ç‚İ‚ñ‚¾floatŒ^‚Ì’l
	 */
	float ReadFloat()
	{
		if (offset_ + sizeof(float) > buffer_->size()) return 0;
		auto ret = *(float*)&(*buffer_)[offset_];
		offset_ += sizeof(float);
		return ret;
	}

	/*!
	 * @brief floatŒ^‚Ì“Ç‚İ‚İ(ƒV[ƒN‚È‚µ)
	 *
	 * @return “Ç‚İ‚ñ‚¾floatŒ^‚Ì’l
	 */
	float ReadFloatNoSeek()
	{
		auto temp = offset_;
		auto ret = ReadFloat();
		offset_ = temp;
		return ret;
	}

	/*!
	 * @brief •¶š—ñ‚Ì“Ç‚İ‚İ
	 *
	 * @return “Ç‚İ‚ñ‚¾•¶š—ñ
	 */
	std::string ReadString()
	{
		int len = ReadInt();
		std::string ret = std::string(&(*buffer_)[offset_], len);
		offset_ += ret.length();
		return ret;
	}

	/*!
	 * @brief •¶š—ñ‚Ì“Ç‚İ‚İ(ƒV[ƒN‚È‚µ)
	 *
	 * @return “Ç‚İ‚ñ‚¾•¶š—ñ
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
