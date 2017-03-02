#include "Device.h"

#define SAFE_RELEASE(a) if (a != nullptr) { a->Release(); a = nullptr; }

Device::Device()
	: d3d_(nullptr)
	, device_(nullptr)
	, hwnd_(nullptr)
	, width_(0)
	, height_(0)
	, device_lost_(false)
	, screen_window_(true)
{
	memset(&d3dpp_, 0, sizeof(d3dpp_));
	memset(&d3dpp_window_, 0, sizeof(d3dpp_window_));
	memset(&d3dpp_full_, 0, sizeof(d3dpp_full_));
	memset(&window_rect_, 0, sizeof(window_rect_));
}

Device::~Device()
{
	Finalize();
}

/*!
 * @brief 初期化
 */
bool Device::Initialize(HWND hwnd, uint32_t width, uint32_t height)
{
	hwnd_ = hwnd;
	width_ = width;
	height_ = height;

	// Direct3Dオブジェクトの作成
	d3d_ = ::Direct3DCreate9(D3D_SDK_VERSION);
	if (d3d_ == nullptr) {
		return false;
	}

	// ウインドウの位置の取得
	GetWindowRect(hwnd_, &window_rect_);

	// D3DDeviceオブジェクトの設定(ウインドウ・モード用)
	::ZeroMemory(&d3dpp_window_, sizeof(d3dpp_window_));
	d3dpp_window_.BackBufferWidth = width;
	d3dpp_window_.BackBufferHeight = height;
	d3dpp_window_.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp_window_.BackBufferCount = 1;
	d3dpp_window_.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp_window_.MultiSampleQuality = 0;
	d3dpp_window_.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp_window_.hDeviceWindow = hwnd_;
	d3dpp_window_.Windowed = TRUE;
	d3dpp_window_.EnableAutoDepthStencil = TRUE;
	d3dpp_window_.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp_window_.Flags = 0;
	d3dpp_window_.FullScreen_RefreshRateInHz = 0;
	//d3dpp_window_.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	d3dpp_window_.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	// D3DDeviceオブジェクトの設定(フルスクリーン・モード)
	::ZeroMemory(&d3dpp_full_, sizeof(d3dpp_full_));
	d3dpp_full_.BackBufferWidth = width;
	d3dpp_full_.BackBufferHeight = height;
	d3dpp_full_.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp_full_.BackBufferCount = 1;
	d3dpp_full_.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp_full_.MultiSampleQuality = 0;
	d3dpp_full_.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp_full_.hDeviceWindow = hwnd_;
	d3dpp_full_.Windowed = FALSE;
	d3dpp_full_.EnableAutoDepthStencil = TRUE;
	d3dpp_full_.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp_full_.Flags = 0;
	d3dpp_full_.FullScreen_RefreshRateInHz = 60;
	//d3dpp_full_.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	d3dpp_full_.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	// D3DDeviceオブジェクトの作成
	d3dpp_ = d3dpp_window_;

	// 表示用のデバイスを作成
	HRESULT hr;

	// デバイス作成
	bool ret = false;
	hr = d3d_->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd_, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp_, &device_);
	if (!SUCCEEDED(hr)) {
		hr = d3d_->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd_, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp_, &device_);
		if (!SUCCEEDED(hr)) {
			hr = d3d_->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hwnd_, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp_, &device_);
			if (!SUCCEEDED(hr)) {
				return false;
			}
		}
	}

	// ステートの初期化
	InitRenderState_();

	return true;
}

void Device::Finalize()
{
	SAFE_RELEASE(device_);
	SAFE_RELEASE(d3d_);
}

bool Device::CheckIdle()
{
	if (d3d_ == nullptr || device_ == nullptr) {
		PostMessage(hwnd_, WM_DESTROY, 0, 0);
		return false;
	}

	if (device_lost_) {
		// デバイス状態のチェック
		HRESULT hr  = device_->TestCooperativeLevel();
		if (FAILED(hr)) {
			if (hr == D3DERR_DEVICELOST) {
				return false;  // デバイスはまだ失われている
			}
			if (hr != D3DERR_DEVICENOTRESET) {
				//GpLog::SetString("%s(%d)：AppIdleがD3DERR_DEVICENOTRESETになりました。\n", std::strrchr(__FILE__, '\\') + 1, __LINE__);
				PostMessage(hwnd_, WM_DESTROY, 0, 0);
				return false;	// 予期せぬエラー
			}

			OnLostDevice_();
			hr = device_->Reset(&d3dpp_); // 復元を試みる
			if (FAILED(hr)) {
				if (hr == D3DERR_DEVICELOST) {
					return false; // デバイスはまだ失われている
				}
				//GpLog::SetString("%s(%d)：AppIdleのリセットに失敗しました。\n", std::strrchr(__FILE__, '\\') + 1, __LINE__);
				PostMessage(hwnd_, WM_DESTROY, 0, 0);
				return false;
			}
			// 解放したDirect3Dで管理していないリソースを再取得
			InitRenderState_();
			if (FAILED(hr)) {
				return false;
			}
		}
		// デバイスが復元した
		device_lost_ = false;
	}

	return true;
}

void Device::CheckDevice()
{
	if (device_ == nullptr)  return ;

	HRESULT hr  = device_->TestCooperativeLevel();
	if (hr == D3DERR_DEVICELOST) {
		device_lost_ = true;
	}
}

/*!
 * @brief スクリーンの切り替え
 */
void Device::ChangeScreen(bool window)
{
	if (device_ == nullptr)  return ;

	static bool last_window;

	screen_window_ = window;
	if (screen_window_) {
		d3dpp_ = d3dpp_window_;
	} else {
		d3dpp_ = d3dpp_full_;
		GetWindowRect(hwnd_, &window_rect_);
	}
	OnLostDevice_();

	HRESULT hr = device_->Reset(&d3dpp_);
	if (FAILED(hr)) {
		if (hr == D3DERR_DEVICELOST) {
			device_lost_ = true;
			return ;
		} else {
			//GpLog::SetString("%s(%d)：ChangeScreenのリセットに失敗しました。\n", std::strrchr(__FILE__, '\\') + 1, __LINE__);
			//PostMessage(hwnd_, WM_DESTROY, 0, 0);
			return ;
		}
	}
	
	InitRenderState_();

	if (screen_window_) {
		SetWindowLong(hwnd_, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		ShowWindow(hwnd_, SW_SHOWNORMAL);
		UpdateWindow(hwnd_);
		if (!last_window) {
			SetWindowPos(hwnd_, HWND_NOTOPMOST,
						 window_rect_.left, window_rect_.top,
						 window_rect_.right - window_rect_.left,
						 window_rect_.bottom - window_rect_.top,
						 SWP_SHOWWINDOW);
		}
	} else {
		SetWindowLong(hwnd_, GWL_STYLE, WS_POPUP | WS_VISIBLE);
		ShowWindow(hwnd_, SW_SHOWNORMAL);
		UpdateWindow(hwnd_);
	}
	Sleep(100);

	if (screen_window_) {
		last_window = true;
	} else {
		last_window = false;
	}
}

void Device::SetBackBufferSize(uint32_t width, uint32_t height)
{
	if (device_ == nullptr)  return ;

	d3dpp_window_.BackBufferWidth = width;
	d3dpp_window_.BackBufferHeight = height;
	d3dpp_ = d3dpp_window_;

	OnLostDevice_();
	HRESULT hr = device_->Reset(&d3dpp_);
	if (FAILED(hr)) {
		if (hr == D3DERR_DEVICELOST) {
			return ;
		} else {
			return ;
		}
	}
	InitRenderState_();
}

void Device::AddDeviceLostListener(DeviceLostListener* listener)
{
	device_lost_listeners_.insert(listener);
}

void Device::RemoveDeviceLostListener(DeviceLostListener* listener)
{
	device_lost_listeners_.erase(listener);
}

/*!
 * @brief オブジェクトの初期処理
 */
void Device::InitRenderState_()
{
	if (device_ == nullptr)  return ;

	float	fFogStart = 0.0f, fFogEnd = 1.0f,  fFogDensity = 1.0f,  PointSize = 1.0f,
			PointSizeMin = 1.0f, PointScaleA = 1.0f, PointScaleB = 0.0f, PointScaleC = 0.0f,
			PointSizeMax = 64.0f, TweenFactor = 0.0f;

	// 3Dデバイス処理
	device_->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);											// 面を塗りつぶし
	device_->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);										// グローシューディングモード
	device_->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);												// 深度バッファへの書き込みを有効
	device_->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);											// ピクセルごとのアルファ テストを有効
	device_->SetRenderState(D3DRS_LASTPIXEL, TRUE);													// 線で最後のピクセルの描画を有効
	device_->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);											// カメラからの距離を基準とするピクセルの受け取りや受け取り拒否を許可 
	device_->SetRenderState(D3DRS_ALPHAREF, 0x00000000);											// ピクセルをテストするための基準アルファ値を指定
	device_->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);										// アプリケーションにアルファ値を基準とするピクセルの受け取りや受け取り拒否を許可
	device_->SetRenderState(D3DRS_DITHERENABLE, FALSE);												// リソースの種類を定義する。
	device_->SetRenderState(D3DRS_FOGENABLE, FALSE);												// フォグ ブレンディングの有無
	device_->SetRenderState(D3DRS_SPECULARENABLE, FALSE);											// スペキュラ ハイライトの有無
	device_->SetRenderState(D3DRS_FOGCOLOR, 0);														// MicrosoftR Direct3DR の基本的なカラー タイプを定義する。
	device_->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);										// ピクセル フォグに使われるフォグ式
	device_->SetRenderState(D3DRS_FOGSTART, *(reinterpret_cast<DWORD*>(&fFogStart)));				// ピクセルまたは頂点フォグ エフェクトが開始する深度
	device_->SetRenderState(D3DRS_FOGEND, *(reinterpret_cast<DWORD*>(&fFogEnd)));					// ピクセルまたは頂点フォグ エフェクトが終了する深度
	device_->SetRenderState(D3DRS_FOGDENSITY, *(reinterpret_cast<DWORD*>(&fFogDensity)));			// ピクセルまたは頂点フォグのフォグ密度
	device_->SetRenderState(D3DRS_RANGEFOGENABLE, FALSE);											// 範囲ベースの頂点フォグの有無
	device_->SetRenderState(D3DRS_STENCILENABLE, FALSE);											// ステンシル処理の有無
	device_->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);									// ステンシル テストに失敗したときに実行するステンシル処理
	device_->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);									// 深度テスト (Z テスト) に失敗した場合に実行するステンシル処理
	device_->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);									// ステンシル テストおよび深度 (Z) テストの両方にパスした場合に実行するステンシル処理
	device_->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);										// ステンシル テストのための比較関数
	device_->SetRenderState(D3DRS_STENCILREF, 0);													// ステンシル テストのための int 基準値
	device_->SetRenderState(D3DRS_STENCILMASK, 0xFFFFFFFF);											// ステンシル テストのための有効ビットを決定するマスク
	device_->SetRenderState(D3DRS_STENCILWRITEMASK, 0xFFFFFFFF);									// ステンシル バッファに書き込む値に適用する書き込みマスク
	device_->SetRenderState(D3DRS_TEXTUREFACTOR, 0xFFFFFFFF);										// ブレンディング処理によるマルチテクスチャ ブレンディングで使われる色
	device_->SetRenderState(D3DRS_WRAP0, 0);														// 複数のテクスチャ座標セットに対するテクスチャラッピング動作
	device_->SetRenderState(D3DRS_WRAP1, 0);														// 複数のテクスチャ座標セットに対するテクスチャラッピング動作
	device_->SetRenderState(D3DRS_WRAP2, 0);														// 複数のテクスチャ座標セットに対するテクスチャラッピング動作
	device_->SetRenderState(D3DRS_WRAP3, 0);														// 複数のテクスチャ座標セットに対するテクスチャラッピング動作
	device_->SetRenderState(D3DRS_WRAP4, 0);														// 複数のテクスチャ座標セットに対するテクスチャラッピング動作
	device_->SetRenderState(D3DRS_WRAP5, 0);														// 複数のテクスチャ座標セットに対するテクスチャラッピング動作
	device_->SetRenderState(D3DRS_WRAP6, 0);														// 複数のテクスチャ座標セットに対するテクスチャラッピング動作
	device_->SetRenderState(D3DRS_WRAP7, 0);														// 複数のテクスチャ座標セットに対するテクスチャラッピング動作
	device_->SetRenderState(D3DRS_WRAP8, 0);														// 複数のテクスチャ座標セットに対するテクスチャラッピング動作
	device_->SetRenderState(D3DRS_WRAP9, 0);														// 複数のテクスチャ座標セットに対するテクスチャラッピング動作
	device_->SetRenderState(D3DRS_WRAP10, 0);														// 複数のテクスチャ座標セットに対するテクスチャラッピング動作
	device_->SetRenderState(D3DRS_WRAP11, 0);														// 複数のテクスチャ座標セットに対するテクスチャラッピング動作
	device_->SetRenderState(D3DRS_WRAP12, 0);														// 複数のテクスチャ座標セットに対するテクスチャラッピング動作
	device_->SetRenderState(D3DRS_WRAP13, 0);														// 複数のテクスチャ座標セットに対するテクスチャラッピング動作
	device_->SetRenderState(D3DRS_WRAP14, 0);														// 複数のテクスチャ座標セットに対するテクスチャラッピング動作
	device_->SetRenderState(D3DRS_WRAP15, 0);														// 複数のテクスチャ座標セットに対するテクスチャラッピング動作
	device_->SetRenderState(D3DRS_CLIPPING, TRUE);													// MicrosoftR Direct3DR によるプリミティブのクリッピングの有無
	device_->SetRenderState(D3DRS_AMBIENT, 0);														// アンビエント ライトの色
	device_->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_NONE);										// 頂点フォグで使われるフォグ式 
	device_->SetRenderState(D3DRS_COLORVERTEX, TRUE);												// 頂点単位の色の有無
	device_->SetRenderState(D3DRS_LOCALVIEWER, TRUE);												// カメラとの相対角度に依存したスペキュラ ハイライトの有無
	device_->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);											// 頂点法線の自動正規化の有無
	device_->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);							// ライティング計算に使われるディフューズ色のソース
	device_->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR2);							// ライティング計算に使われるスペキュラ色のソース
	device_->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);							// ライティング計算に使われるアンビエント色のソース
	device_->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);							// ライティング計算に使われるエミッション色のソース
	device_->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_DISABLE);										// ジオメトリがある場合、ジオメトリ ブレンディングを実行するために使う行列の個数
	device_->SetRenderState(D3DRS_CLIPPLANEENABLE, 0);												// ユーザー定義のクリップ面を有効または無効にする
	device_->SetRenderState(D3DRS_POINTSIZE,	*(reinterpret_cast<uint32_t*>(&PointSize)));		// 各頂点に対してポイント サイズが指定されていない場合に、ポイント サイズ計算で使うサイズを指定する float値
	device_->SetRenderState(D3DRS_POINTSIZE_MIN, *(reinterpret_cast<uint32_t*>(&PointSizeMin)));	// ポイント プリミティブの最小サイズを指定する float 値
	device_->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);										// BOOL 値
	device_->SetRenderState(D3DRS_POINTSCALEENABLE, FALSE);											// ポイント プリミティブに対するサイズの計算を制御する BOOL 値
	device_->SetRenderState(D3DRS_POINTSCALE_A, *(reinterpret_cast<uint32_t*>(&PointScaleA)));		// ポイント プリミティブに対する距離ベースのサイズの減衰を制御する float 値
	device_->SetRenderState(D3DRS_POINTSCALE_B, *(reinterpret_cast<uint32_t*>(&PointScaleB)));		// ポイント プリミティブに対する距離ベースのサイズの減衰を制御する float 値
	device_->SetRenderState(D3DRS_POINTSCALE_C, *(reinterpret_cast<uint32_t*>(&PointScaleC)));		// ポイント プリミティブに対する距離ベースのサイズの減衰を制御する float 値
	device_->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);										// マルチサンプル レンダリングのターゲット バッファを使うときの、個々のサンプルの計算方法を決定する 
	device_->SetRenderState(D3DRS_MULTISAMPLEMASK, 0xFFFFFFFF);										// マルチサンプル バッファを蓄積バッファとして使い、各パスがサンプルのサブセットを更新するようなジオメトリのマルチパス レンダリングを行うことができる。
	device_->SetRenderState(D3DRS_PATCHEDGESTYLE, D3DPATCHEDGE_DISCRETE);							// パッチ エッジが浮動小数点数スタイルのテセレーションを使うかどうかを設定する
	device_->SetRenderState(D3DRS_DEBUGMONITORTOKEN, D3DDMT_ENABLE);								// モニタをデバッグする場合にのみ設定する
	device_->SetRenderState(D3DRS_POINTSIZE_MAX, *(reinterpret_cast<uint32_t*>(&PointSizeMax)));	// ポイント スプライトが制限される最大サイズを指定する float 値
	device_->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);									// インデックス付きの頂点ブレンディングを有効または無効にする BOOL 値
	device_->SetRenderState(D3DRS_COLORWRITEENABLE, 0x0000000F);									// レンダリング ターゲットのカラー バッファに対するチャンネルごとの書き込みを有効にする UINT 値
	device_->SetRenderState(D3DRS_TWEENFACTOR, *(reinterpret_cast<uint32_t*>(&TweenFactor)));		// トゥイーン係数を制御する float 値
	device_->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);											// アルファ ブレンディング レンダリング ステートがTRUEに設定されている場合に適用する算術演算の選択に使われる値
	device_->SetRenderState(D3DRS_POSITIONDEGREE, D3DDEGREE_CUBIC);									// N パッチ位置補間次数
	device_->SetRenderState(D3DRS_NORMALDEGREE,  D3DDEGREE_LINEAR);									// N パッチ法線補間次数
	device_->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);										// シザーテストの有無
	device_->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0);											// z ファイティングを減らすために、同一平面上のプリミティブにどれだけバイアスを適用するかを決定するのに使う
	device_->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);									// 線のアンチエイリアシングの有無
	device_->SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, FALSE);										// 2 面のステンシルの有無
	device_->SetRenderState(D3DRS_CCW_STENCILFAIL, 0x00000001);										// 反時計回りのステンシル テストに失敗したときに実行するステンシル処理
	device_->SetRenderState(D3DRS_CCW_STENCILZFAIL, 0x00000001);									// 反時計回りのステンシル テストにパスし、Z テストに失敗した場合に実行するステンシル処理
	device_->SetRenderState(D3DRS_CCW_STENCILPASS, 0x00000001);										// 反時計回りのステンシル テストおよび Z テストの両方にパスした場合に実行するステンシル処理
	device_->SetRenderState(D3DRS_CCW_STENCILFUNC, 0x00000008);										// 比較関数。反時計回りのステンシル テストにパスするのは((ref & mask)ステンシル関数(stencil & mask))がTRUEの場合
	device_->SetRenderState(D3DRS_COLORWRITEENABLE1, 0x0000000f);									// デバイスに対する追加の ColorWriteEnable の値
	device_->SetRenderState(D3DRS_COLORWRITEENABLE2, 0x0000000f);									// デバイスに対する追加の ColorWriteEnable の値
	device_->SetRenderState(D3DRS_COLORWRITEENABLE3, 0x0000000f);									// デバイスに対する追加の ColorWriteEnable の値
	device_->SetRenderState(D3DRS_BLENDFACTOR, 0xffffffff);											// アルファ ブレンディング処理の間に定数ブレンディング係数に使う D3DCOLOR
	device_->SetRenderState(D3DRS_SRGBWRITEENABLE, 0);												// レンダリング ターゲットへの書き込みで、sRGB へのガンマ補正を有効にする
	device_->SetRenderState(D3DRS_DEPTHBIAS, 0);													// 深度値の比較に使う浮動小数点値
	device_->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, FALSE);									// アルファ チャンネルに対する個別のブレンディング モードの有無
	device_->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);										// D3DBLEND 列挙型のメンバのいずれかである
	device_->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ZERO);									// D3DBLEND 列挙型のメンバのいずれかである
	device_->SetRenderState(D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD);									// レンダリング ステートの D3DRS_ALPHABLENDENABLE が TRUE に設定されている場合に、個別のアルファ ブレンディングに適用する算術演算の選択に使う値。 
	device_->SetRenderState(D3DRS_ZENABLE, D3DZB_USEW);												// Zバッファを有効
	device_->SetRenderState(D3DRS_AMBIENT, 0xffffffff);												// アンビエントライト
	device_->SetRenderState(D3DRS_LIGHTING, TRUE);													// ライトを有効
	device_->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);											// カリングの設定
	device_->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);											// アルファブレンドを有効

	// フィルタ設定
	device_->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);			// ミップマップ
	device_->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);			// 拡大
	device_->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);			// 縮小
	device_->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);		// テクスチャの色を使用
	device_->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);		// 頂点の色を使用
	device_->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);		// レンダリング時の色の計算方法の設定
	device_->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);		// テクスチャの色を使用
	device_->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);		// 頂点の色を使用

	// ビューポート
	D3DVIEWPORT9 view = { 0, 0, width_, height_, 0.0f, 1.0f };
	device_->SetViewport(&view);

	// ライト
	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(D3DLIGHT9));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Diffuse.r = 1.0f;
	light.Diffuse.g = 1.0f;
	light.Diffuse.b = 1.0f;
	light.Direction = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	device_->SetLight(0, &light);
	device_->LightEnable(0, TRUE);

	// デバイスリセット
	OnResetDevice_();
}

void Device::OnLostDevice_()
{
	for (auto it = device_lost_listeners_.begin(); it != device_lost_listeners_.end(); ++it) {
		(*it)->OnLostDevice();
	}
}

void Device::OnResetDevice_()
{
	for (auto it = device_lost_listeners_.begin(); it != device_lost_listeners_.end(); ++it) {
		(*it)->OnResetDevice();
	}
}
