#pragma once

#include "main/WinMain.h"
#include "SceneBase.h"
#include "utility/counter.h"
#include "utility/bezier.h"
#include "utility/StateManager.h"
#include "master/MasterData.hpp"
#include <memory>
#include <map>

/*!
 * @brief タイトルシーン
 */
class SceneTitle : public SceneBase
{
public :
	~SceneTitle(){}

	/*!
	 * @brief 初期化
	 */
	void Initialize(const SceneBaseParam* param);

	/*!
	 * @brief 更新
	 *
	 * @param df 前のフレームとの差分時間
	 */
	void Update(float df);

	/*!
	 * @brief 描画
	 */
	void Render();

private :
	enum State {
		ST_INIT,
		ST_FADE_IN_INIT,
		ST_FADE_IN,
		ST_UPDATE,
	};
	
	void Reload_();
	void SetBezier_(const Bezier::ControlPoint& cp);

	std::map<std::string, std::shared_ptr<Gdiplus::Bitmap>> bitmaps_;
	std::map<std::string, MasterData::TitleUIData*> objects_;
	StateManager<State> state_;

	SaturationCounter stop_counter_;
	RootCounter root_counter_;
	InteriorCounter fade_counter_;
	LoopCounter cursor_counter_;
	Bezier::Counter<int> bezier_counter_;
	Bezier::Timer<int> bezier_timer_;

	int cursour_y_;
};
