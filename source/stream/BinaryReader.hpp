#pragma once
#include "Stream.hpp"
#include <cstdint>
#include <vector>

class BinaryReader
{
public :
	BinaryReader(Stream& stream);
	~BinaryReader();

	void Close();
	std::vector<char> ReadBytes(int count);
	int8_t ReadInt8();
	uint8_t ReadUInt8();
	int16_t ReadInt16();
	uint16_t ReadUInt16();
	int32_t ReadInt32();
	uint32_t ReadUInt32();
	int64_t ReadInt64();
	uint64_t ReadUInt64();
	Stream& GetStream() { return stream_; }

private :
	Stream& stream_;
};
