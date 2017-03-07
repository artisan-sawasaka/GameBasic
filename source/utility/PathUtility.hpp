#pragma once

#include <string.h>
#include <string>

class PathUtility
{
public:
	/*!
	 * @brief フルパスからファイル名なしのパスを取得します
	 */
	static std::string GetDirectoryName(const char* str)
	{
		const char* p = strrchr(str, '\\');
		if (p == nullptr) {
			p = strrchr(str, '/');
			if (p == nullptr) {
				return std::string(str);
			}
		}
		return std::string(str, p - str);
	}

	/*!
	 * @brief 指定したパス文字列のファイル名と拡張子を返します。
	 */
	static std::string GetFileName(const char* str)
	{
		const char* p = strrchr(str, '\\');
		if (p == nullptr) {
			p = strrchr(str, '/');
			if (p == nullptr) {
				return std::string(str);
			}
		}
		return std::string(p + 1);
	}

	/*!
	 * @brief 指定したパス文字列のファイル名を拡張子を付けずに返します。
	 */
	static std::string GetFileNameWithoutExtension(const char* str)
	{
		const char* p = strrchr(str, '\\');
		if (p == nullptr) {
			p = strrchr(str, '/');
		}
		if (p == nullptr){
			p = str;
		} else {
			p++;
		}
		const char* pp = strrchr(p, '.');
		if (pp == nullptr) {
			return std::string(p);
		}
		return std::string(p, pp - p);
	}

	/*!
	 * @brief 指定したパス文字列の拡張子を返します。
	 */
	static std::string GetExtension(const char* str)
	{
		const char* pp = strrchr(str, '.');
		if (pp == nullptr) {
			return std::string(str);
		}
		return std::string(pp + 1);
	}

	/*!
	* @brief 指定したパスを結合します。
	*/
	static std::string Combine(const std::string& path1, const std::string& path2)
	{
		std::string ret = path1;
		if (path1.length() > 0 && path1[path1.length() - 1] != '/' && path1[path1.length() - 1] != '\\') {
			ret += "/";
		}
		if (path2.length() > 0 && (path2[0] == '/' || path2[0] == '\\')) {
			return ret + path2.substr(1);
		}
		return ret + path2;
	}
	
};
