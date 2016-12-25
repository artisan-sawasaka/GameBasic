#pragma once

#include <Windows.h>

class ConsoleManager
{
public :
	/*!
	 * @brief コンソールウインドウを開く
	 */
	void Open();

	/*!
	 * @brief コンソールウインドウを閉じる
	 */
	void Close();

	/*!
	 * @brief コンソールウインドウに文字を出力
	 */
	void Print(const char* s, ...);

	static ConsoleManager* GetInstance() {
		static ConsoleManager v;
		return &v;
	}

private :
	ConsoleManager() : handle_(0){}
	HANDLE handle_;
};
