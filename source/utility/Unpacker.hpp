#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <map>

class Unpacker
{
private :
	struct Info
	{
		int64_t offset;
		int64_t size;
		std::vector<char> buf;
	};
	std::vector<Info> infos_;
	std::vector<char> empty_;
	std::map<uint32_t, Info*> maps_;

public :
	Unpacker(){}
	~Unpacker(){}

	void Set(const char* path);
	const std::vector<char>& GetData(int index);
	const std::vector<char>& GetData(const std::string& name);
};
