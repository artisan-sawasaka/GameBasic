#pragma once

#include <vector>
#include <string>
#include <algorithm>

class File
{
public:
	/*!
	 * @brief バイナリファイルを読み込む
	 *
	 * @param path ファイルのパス
	 *
	 * @return 読み込んだバイナリ
	 */
	static std::vector<char> ReadAllBytes(const std::string& path)
	{
		FILE* fp = std::fopen(path.c_str(), "rb");
		if (fp == nullptr) return std::vector<char>();
		std::fseek(fp, 0, SEEK_END);
		std::vector<char> buffer(ftell(fp));
		std::fseek(fp, 0, SEEK_SET);
		std::fread(&buffer[0], sizeof(char), buffer.size(), fp);
		std::fclose(fp);

		return buffer;
	}

	/*!
	 * @brief テキストファイルを読み込む
	 *
	 * @param path ファイルのパス
	 *
	 * @return 読み込んだテキスト
	 */
	static std::string ReadAllText(const std::string& path)
	{
		FILE* fp = std::fopen(path.c_str(), "rb");
		if (fp == nullptr) return std::string();
		std::fseek(fp, 0, SEEK_END);
		std::string ret;
		size_t length = ftell(fp);
		ret.resize(ftell(fp));
		std::fseek(fp, 0, SEEK_SET);
		std::fread(&ret[0], sizeof(char), ret.length(), fp);
		std::fclose(fp);

		return ret;
	}

	/*!
	 * @brief テキストファイルを行で分解して読み込む
	 *
	 * @param path ファイルのパス
	 *
	 * @return 読み込んだテキスト
	 */
	static std::vector<std::string> ReadAllLines(const std::string& path)
	{
		// ファイル読み込み
		auto str = File::ReadAllText(path);
		if (str.empty()) return std::vector<std::string>();

		// 改行数を取得
		size_t count = std::count(std::begin(str), std::end(str), '\n');
		if (str[str.length() - 1] != '\n') ++count;

		// 戻す変数を設定
		std::vector<std::string> ret;
		ret.reserve(count);

		// 環境によって改行コード変更
		static const std::string token = "\r\n";

		// 分解処理
		const char* src = str.c_str();
		const char* end = str.c_str() + str.length();
		while (src < end) {
			const char* pp = std::strstr(src, token.c_str());
			if (pp == nullptr) {
				ret.push_back(std::string(src, end));
				break;
			}
			ret.push_back(std::string(src, pp - src));
			src = pp + token.length();
		}

		return ret;
	}
};
