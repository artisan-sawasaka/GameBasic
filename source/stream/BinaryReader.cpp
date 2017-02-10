#pragma once
#include "BinaryReader.hpp"

BinaryReader::BinaryReader(Stream& stream)
	: stream_(stream)
{
}

BinaryReader::~BinaryReader()
{
}

void BinaryReader::Close()
{
	stream_.Close();
}

std::vector<char> BinaryReader::ReadBytes(int count)
{
	std::vector<char> buf(count);
	stream_.Read(&buf[0], count);
	return buf;
}

int8_t BinaryReader::ReadInt8()
{
	int8_t ret;
	stream_.Read(&ret, sizeof(int8_t));
	return ret;
}

uint8_t BinaryReader::ReadUInt8()
{
	uint8_t ret;
	stream_.Read(&ret, sizeof(uint8_t));
	return ret;
}

int16_t BinaryReader::ReadInt16()
{
	int16_t ret;
	stream_.Read(&ret, sizeof(int16_t));
	return ret;
}

uint16_t BinaryReader::ReadUInt16()
{
	uint16_t ret;
	stream_.Read(&ret, sizeof(uint16_t));
	return ret;
}

int32_t BinaryReader::ReadInt32()
{
	int32_t ret;
	stream_.Read(&ret, sizeof(int32_t));
	return ret;
}

uint32_t BinaryReader::ReadUInt32()
{
	uint32_t ret;
	stream_.Read(&ret, sizeof(uint32_t));
	return ret;
}

int64_t BinaryReader::ReadInt64()
{
	int64_t ret;
	stream_.Read(&ret, sizeof(int64_t));
	return ret;
}

uint64_t BinaryReader::ReadUInt64()
{
	uint64_t ret;
	stream_.Read(&ret, sizeof(uint64_t));
	return ret;
}
