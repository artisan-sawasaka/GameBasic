#include "WinMain.h"
#include <stdarg.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Gdiplus.lib")

AppBase::AppBase(TCHAR* wnClassName, TCHAR* title)
	: wnd_x_(32)
	, wnd_y_(32)
	, wnd_w_(800)
	, wnd_h_(450)
	, average_fps_(0)
	, fps_(60)
	, fps_count_(0)
	, old_time_(0)
	, hwnd_(NULL)
	, style_(WS_OVERLAPPED |  WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX)
	, bitmap_(NULL)
	, hdc_mem_(NULL)
{
	// タイマー精度の初期化
	timeGetDevCaps(&timer_caps_, sizeof(TIMECAPS));
	timeBeginPeriod(timer_caps_.wPeriodMin);

	lstrcpy(wndClassName_, wnClassName);
	lstrcpy(title_, title);

	// ウインドウクラスの初期化
	wcex_.cbSize = sizeof(WNDCLASSEX);
	wcex_.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex_.lpfnWndProc = (WNDPROC)WndProc_;
	wcex_.cbClsExtra = 0;
	wcex_.cbWndExtra = 0;
	wcex_.hInstance = GetModuleHandle(NULL);
	wcex_.hIcon = LoadIcon(NULL , IDI_APPLICATION);
	wcex_.hCursor = LoadCursor(NULL , IDC_ARROW) ;
	wcex_.hIconSm = LoadIcon(NULL , IDI_APPLICATION);
	wcex_.lpszMenuName = NULL;
	wcex_.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex_.lpszClassName = wndClassName_;
}

AppBase::~AppBase()
{
	if (bitmap_ != NULL) {
		DeleteObject(bitmap_);
	}
	if (hdc_mem_ != NULL) {
		DeleteDC(hdc_mem_);
	}
	timeEndPeriod(timer_caps_.wPeriodMin);
}

/*!
 * @brief ウインドウの座標を変更
 */
void AppBase::SetWindowPosition(int x, int y)
{
	wnd_x_ = x;
	wnd_y_ = y;
	if (hwnd_ != NULL) {
		MoveWindow(hwnd_, wnd_x_, wnd_y_, wnd_w_, wnd_h_, TRUE);
	}
}

/*!
 * @brief ウインドウのサイズを変更
 */
void AppBase::SetWindowSize(int w, int h)
{
	wnd_w_ = w;
	wnd_h_ = h;
	if (hwnd_ != NULL) {
		SIZE size = GetClientSize_(wnd_w_, wnd_h_);
		MoveWindow(hwnd_, wnd_x_, wnd_y_, size.cx, size.cy, TRUE);
	}
}

/*!
 * @brief 開始
 */
bool AppBase::Start()
{
	// ウインドウ登録
	if (!RegisterClassEx(&wcex_)) {
		return false;
	}

	// ウインドウ生成
	SIZE size = GetClientSize_(wnd_w_, wnd_h_);
	hwnd_ = ::CreateWindowEx(WS_EX_CLIENTEDGE,
		wndClassName_,
		title_,
		style_,
		wnd_x_,
		wnd_y_,
		size.cx,
		size.cy,
		NULL,
		NULL,
		GetModuleHandle(NULL),
		this);
	if (hwnd_ == NULL) {
		return false;
	}
	ShowWindow(hwnd_, SW_SHOWNORMAL);
	UpdateWindow(hwnd_);

	// 時間の初期化
	old_time_ = timeGetTime();
	start_time_ = old_time_;

	// ダブルバッファ作成
	HDC hdc = GetDC(hwnd_);
	hdc_mem_ = CreateCompatibleDC(hdc);
	bitmap_ = CreateCompatibleBitmap(hdc, wnd_w_, wnd_h_);
	SelectObject(hdc_mem_, bitmap_);
	ReleaseDC(hwnd_, hdc);

	// 継承先の初期化呼び出し
	Initialize();

	// メッセージループ
	while (true) {
		MSG msg;
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			// 垂直動機待ち
			float df = VSync_();

			// 継承先のメインループの呼び出し
			Update(df);

			// 再描画呼び出し
			InvalidateRect(hwnd_, NULL, FALSE);
		}
	}
	
	// 継承先の終了化呼び出し
	Finalize();

	return true;
}

/*!
 * @brief メインウインドウメッセージ
 */
LRESULT CALLBACK AppBase::WndProc_(HWND hWnd, UINT msg, UINT wParam, LONG lParam)
{
	AppBase* app = reinterpret_cast<AppBase*>(GetWindowLong(hWnd, GWL_USERDATA));
	switch (msg) {
	case WM_CREATE : {
			LPCREATESTRUCT data = reinterpret_cast< LPCREATESTRUCT >(lParam);
			app = reinterpret_cast<AppBase*>(data->lpCreateParams);
			SetWindowLong(hWnd, GWL_USERDATA, reinterpret_cast< long >(app));
			break;
		}
	case WM_DESTROY :
		::PostQuitMessage(0);
		return 0;
	case WM_SYSKEYDOWN :
		if (wParam == VK_F4) {
			PostMessage(hWnd, WM_CLOSE, 0, 0);
		}
		break;
	case WM_PAINT : {
			if (app != nullptr) {
				app->Render_();
			}
			break;
		}
	}

	if (app != nullptr) {
		return app->WndProc(hWnd, msg, wParam, lParam);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

/*!
 * @brief クライアント領域のサイズを取得
 */
SIZE AppBase::GetClientSize_(int w, int h)
{
	RECT wndRect;
	wndRect.top = 0;
	wndRect.left = 0;
	wndRect.right = w;
	wndRect.bottom = h;
	AdjustWindowRectEx(&wndRect, style_, FALSE, WS_EX_CLIENTEDGE);

	// Window7だとクライアント領域の扱いがおかしいので調整する
	SIZE size = {
		wndRect.right - wndRect.left - 2,
		wndRect.bottom - wndRect.top - 2
	};
	return size;
}

/*!
 * @brief 垂直動機待ち
 */
float AppBase::VSync_()
{
	// 待ち時間判定
	DWORD time = timeGetTime();
	int wait = ++fps_count_ * 1000 / fps_ - (time - start_time_);
	if (wait > 0) {
		Sleep(wait);
	} else {
		Sleep(0);
	}

	// 前のフレームとの時間差を取得
	time = timeGetTime();
	float ret = (time - old_time_) / 1000.0f;
	old_time_ = time;

	// 平均FPS計測
	if (fps_count_ % fps_ == 0) {
		average_fps_ = 1000.f / ((time - start_time_) / static_cast<float>(fps_));
		fps_count_ = 0;
		start_time_ = time;
	}

	return ret;
}

/*!
 * @brief 描画処理
 */
void AppBase::Render_()
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd_, &ps);
	BitBlt(hdc, -1, -1, wnd_w_, wnd_h_, hdc_mem_, 0, 0, SRCCOPY);
	EndPaint(hwnd_, &ps);
}
