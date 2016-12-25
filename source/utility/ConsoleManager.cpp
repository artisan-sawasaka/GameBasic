#include "ConsoleManager.h"
#include <io.h>
#include <Fcntl.h>
#include <stdio.h>

/*!
 * @brief コンソールウインドウを開く
 */
void ConsoleManager::Open()
{
	if (handle_ != 0) return ;

    AllocConsole();
    handle_ = GetStdHandle(STD_OUTPUT_HANDLE);
}

/*!
 * @brief コンソールウインドウを閉じる
 */
void ConsoleManager::Close()
{
	if (handle_ != 0) {
		FreeConsole();
	}
	handle_ = 0;
}

/*!
 * @brief コンソールウインドウに文字を出力
 */
void ConsoleManager::Print(const char* s, ...)
{
	if (handle_ == 0) return;

	// テキストフォーマットを変換
	char text[1024];
	va_list arg;
	va_start(arg, s);
	vsprintf(text, s, arg);
	va_end(arg);

	DWORD dwWriteByte;
	WriteConsoleA(handle_, text, strlen(text), &dwWriteByte, NULL);
}
