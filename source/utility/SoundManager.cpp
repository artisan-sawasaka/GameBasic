#include "SoundManager.h"
#include <cri_le_atom_asr.h>
#include <cri_le_atom_pc.h>
#include "FileUtility.hpp"

static const int MAX_KEEP_BGM_VALUE		= 10;	// 内部で保持するBGMファイルの最大数
static const int MAX_KEEP_VOICE_VALUE	= 12;	// 内部で保持するVOICEファイルの最大数

// ------------------------------------------------------------------------------------------------------------------------
// 内部関数
// ------------------------------------------------------------------------------------------------------------------------
void *adx2_alloc(void *obj, CriUint32 size)
{
    return malloc(size);
}

void adx2_free(void *obj, void *ptr)
{
    free(ptr);
}

void adx2_error_callback(const CriChar8 *errid, CriUint32 p1, CriUint32 p2, CriUint32 *parray)
{
    const CriChar8 *errmsg;
    errmsg = criErr_ConvertIdToMessage(errid, p1, p2);
}

// ------------------------------------------------------------------------------------------------------------------------
// コンストラクタとデストラクタ
// ------------------------------------------------------------------------------------------------------------------------
SoundManager::SoundManager()
	: initialized_(nullptr)
	, base_bgm_data_(nullptr)
	, base_se_data_(nullptr)
	, last_bgm_name_("")
	, last_bgm_id_(-1)
	, bgm_volume_(0.5f)
	, se_volume_(0.5f)
	, voice_volume_(0.5f)
	, hca_pool_(nullptr)
	, hca_mx_pool_(nullptr)
	, dbas_id_(-1)
{
}

SoundManager::~SoundManager()
{
	Finalize();
}

// ------------------------------------------------------------------------------------------------------------------------
// 初期化と終了化
// ------------------------------------------------------------------------------------------------------------------------
void SoundManager::Initialize(const InitializeParam& param)
{
    Finalize();
    param_ = param;
    int max_sound = param_.max_bgm + param_.max_se + param_.max_voice;

    criErr_SetCallback(&adx2_error_callback);
    criAtomEx_SetUserAllocator(&adx2_alloc, &adx2_free, nullptr);
    
    // ファイル構造体初期化
    CriFsConfig fs_config;
    criFs_SetDefaultConfig(&fs_config);
    fs_config.num_loaders = max_sound * 3;
    fs_config.max_files = fs_config.num_loaders;
    
    // AtomEx初期化用コンフィグ
    CriAtomExConfig atom_ex;
    criAtomEx_SetDefaultConfig(&atom_ex);
    atom_ex.server_frequency = param_.frame_rate;
    atom_ex.max_virtual_voices = max_sound * 2;
    atom_ex.fs_config = &fs_config;

    // ASR初期化
    CriAtomExAsrConfig asr;
    criAtomExAsr_SetDefaultConfig(&asr);
    asr.server_frequency = param_.frame_rate;
    
    // HCA-MX初期化
    CriAtomExHcaMxConfig hca_mx;
    criAtomExHcaMx_SetDefaultConfig(&hca_mx);
    hca_mx.server_frequency = param_.frame_rate;
	hca_mx.max_sampling_rate = param.max_sampling_rate;
	hca_mx.output_sampling_rate = param.max_sampling_rate;

    CriAtomExConfig_PC initialize_conf;
    criAtomEx_SetDefaultConfig_PC(&initialize_conf);
    initialize_conf.atom_ex = atom_ex;
    initialize_conf.asr = asr;
    initialize_conf.hca_mx = hca_mx;
    
    criAtomEx_Initialize_PC(&initialize_conf, nullptr, 0);
    
    dbas_id_ = criAtomDbas_Create(nullptr, nullptr, 0);
    
    // 標準ボイスプールの設定
	CriAtomExStandardVoicePoolConfig hca_config;
    criAtomExVoicePool_SetDefaultConfigForStandardVoicePool(&hca_config);
    hca_config.num_voices = max_sound;
    hca_config.player_config.max_sampling_rate = param_.max_sampling_rate;
	hca_config.player_config.streaming_flag = CRI_TRUE;
	hca_pool_ = criAtomExVoicePool_AllocateStandardVoicePool(&hca_config, nullptr, 0);

    // HCA-MXボイスプールの設定
    CriAtomExHcaMxVoicePoolConfig hca_mx_config;
    criAtomExVoicePool_SetDefaultConfigForHcaMxVoicePool(&hca_mx_config);
    hca_mx_config.num_voices = max_sound;
    hca_mx_config.player_config.streaming_flag = CRI_TRUE;
    hca_mx_config.player_config.max_sampling_rate = param_.max_sampling_rate;
	hca_mx_pool_ = criAtomExVoicePool_AllocateHcaMxVoicePool(&hca_mx_config, nullptr, 0);

    // プレイヤーの生成
    player_.resize(max_sound);
	for (size_t i = 0; i < player_.size(); ++i) {
		player_[i] = criAtomExPlayer_Create(nullptr, nullptr, 0);
        criAtomExPlayer_AttachFader(player_[i], nullptr, nullptr, 0);
    }
    
    // ACFファイルの設定
    if (param_.acf_file != nullptr) {
		if (File::Exist(param_.acf_file)) {
			criAtomEx_RegisterAcfFile(nullptr, param_.acf_file, nullptr, 0);
		}
    }
    
    initialized_ = true;
}

void SoundManager::Finalize()
{
	if (!initialized_) return;

    // 停止命令
    StopAll(0);
    
	// プレイヤーの解放
	for (size_t i = 0; i < player_.size(); ++i) {
		auto& player = player_[i];
		while (true) {
			auto state = criAtomExPlayer_GetStatus(player);
			if (state == CRIATOMEXPLAYER_STATUS_STOP ||
				state == CRIATOMEXPLAYER_STATUS_PLAYEND ||
				state == CRIATOMEXPLAYER_STATUS_ERROR) {
				break;
			}
			Update(0);
			Sleep(10);
		}
		criAtomExPlayer_Destroy(player);
	}
	std::vector<CriAtomExPlayerHn>().swap(player_);

	// ボイスプール開放
    ReleaseVoicePool_(hca_mx_pool_);
    ReleaseVoicePool_(hca_pool_);
    
    // ACB解放
    ReleaseAcb_(base_bgm_data_);
    ReleaseAcb_(base_se_data_);
	ReleaseAcbList_(voice_list_);

    if (dbas_id_ == 0) {
        criAtomDbas_Destroy(dbas_id_);
        dbas_id_ = -1;
    }
    criAtomEx_UnregisterAcf();
    criAtomEx_Finalize_PC();

	initialized_ = false;
}

// ------------------------------------------------------------------------------------------------------------------------
// 読み込み
// ------------------------------------------------------------------------------------------------------------------------
bool SoundManager::LoadBgmFile(const std::string& acb_file, const std::string& awb_file)
{
    if (!initialized_) return false;

	if (!File::Exist(acb_file)) return false;
    if (!File::Exist(awb_file)) return false;

	ReleaseAcb_(base_bgm_data_);
	base_bgm_data_ = criAtomExAcb_LoadAcbFile(nullptr, acb_file.c_str(), nullptr, awb_file.c_str(), nullptr, 0);

	return true;
}

bool SoundManager::LoadSeFile(const std::string& acb_file)
{
    if (!initialized_) return false;
    
	if (!File::Exist(acb_file)) return false;
    
    ReleaseAcb_(base_se_data_);
    base_se_data_ = criAtomExAcb_LoadAcbFile(nullptr, acb_file.c_str(), nullptr, nullptr, nullptr, 0);
    
    return true;
}

// ------------------------------------------------------------------------------------------------------------------------
// BGM
// ------------------------------------------------------------------------------------------------------------------------
int SoundManager::PlayBgm(int id, float delay, float fadein_time, float fadeout_time, int track)
{
    if (!initialized_) return -1;
    if (id == last_bgm_id_) return -1;
	if (track >= param_.max_bgm) return -1;

	auto acb_func = [this, id](CriAtomExPlayerHn player, CriAtomExAcbHn acb){
        criAtomExPlayer_SetCueId(player, acb, id);
        last_bgm_name_ = "";
        last_bgm_id_ = id;
    };
    return PlayBgm_(acb_func, delay, fadein_time, fadeout_time, track);
}

int SoundManager::PlayBgm(const std::string& cue, float delay, float fadein_time, float fadeout_time, int track)
{
    if (!initialized_) return -1;
    if (cue == last_bgm_name_) return -1;
	if (track >= param_.max_bgm) return -1;

    auto acb_func = [this, cue](CriAtomExPlayerHn player, CriAtomExAcbHn acb){
        criAtomExPlayer_SetCueName(player, acb, cue.c_str());
        last_bgm_name_ = cue;
        last_bgm_id_ = -1;
    };
    return PlayBgm_(acb_func, delay, fadein_time, fadeout_time, track);
}

void SoundManager::StopBgm(float time, int track)
{
    if (!initialized_) return ;
	if (track >= param_.max_bgm) return;

	if (track < 0) {
		for (int i = 0; i < param_.max_bgm; ++i) {
			StopBgm(time, i);
		}
	} else {
		auto& player = player_[GetTrackIndex_(SoundType::BGM, track)];
		if (time == 0) {
			criAtomExPlayer_StopWithoutReleaseTime(player);
		} else {
			criAtomExPlayer_SetFadeOutTime(player, static_cast<CriSint32>(time * 1000));
			criAtomExPlayer_Stop(player);
		}
	}
	last_bgm_name_ = "";
	last_bgm_id_ = -1;
}

void SoundManager::SetVolumeBgm(float volume)
{
    if (!initialized_) return ;
    
    bgm_volume_ = volume;
    for (int i = 0; i < param_.max_bgm; ++i) {
        auto& player = player_[GetTrackIndex_(SoundType::BGM, i)];
        criAtomExPlayer_SetVolume(player, volume);
        criAtomExPlayer_UpdateAll(player);
    }
}

bool SoundManager::IsStopBgm(int track) const
{
    if (!initialized_) return true;
	if (0 < track || track >= param_.max_bgm) return true;

	auto& player = player_[GetTrackIndex_(SoundType::BGM, track)];
	auto state = criAtomExPlayer_GetStatus(player);
    return state == CRIATOMEXPLAYER_STATUS_STOP ||
           state == CRIATOMEXPLAYER_STATUS_PLAYEND ||
           state == CRIATOMEXPLAYER_STATUS_ERROR;
}

/**
 * @brief BGMをポーズします。
 */
void SoundManager::PauseBgm(int track)
{
	if (!initialized_) return ;
	if (track >= param_.max_bgm) return;

	if (track < 0) {
		for (int i = 0; i < param_.max_bgm; ++i) {
			auto& player = player_[GetTrackIndex_(SoundType::BGM, i)];
			criAtomExPlayer_Pause(player, CRI_TRUE);
		}
	} else {
		auto& player = player_[GetTrackIndex_(SoundType::BGM, track)];
		criAtomExPlayer_Pause(player, CRI_TRUE);
	}
}

/**
 * @brief BGMを再開します。
 */
void SoundManager::ResumeBgm(int track)
{
	if (!initialized_) return;
	if (track >= param_.max_bgm) return;

	if (track < 0) {
		for (int i = 0; i < param_.max_bgm; ++i) {
			auto& player = player_[GetTrackIndex_(SoundType::BGM, i)];
			criAtomExPlayer_Pause(player, CRI_FALSE);
		}
	} else {
		auto& player = player_[GetTrackIndex_(SoundType::BGM, track)];
		criAtomExPlayer_Pause(player, CRI_FALSE);
	}
}

// ------------------------------------------------------------------------------------------------------------------------
// SE
// ------------------------------------------------------------------------------------------------------------------------
int SoundManager::PlaySe(int id, int track, float delay)
{
    if (!initialized_) return -1;
    if (track >= param_.max_se) return -1;
    return PlaySe_([this, id](CriAtomExPlayerHn player){criAtomExPlayer_SetCueId(player, base_se_data_, id);}, track, delay);
}

int SoundManager::PlaySe(const std::string& cue, int track, float delay)
{
    if (!initialized_) return -1;
    if (track >= param_.max_se) return -1;
    return PlaySe_([this, cue](CriAtomExPlayerHn player){criAtomExPlayer_SetCueName(player, base_se_data_, cue.c_str());}, track, delay);
}

void SoundManager::StopSe(int track, float time)
{
    if (!initialized_) return ;
    if (track >= param_.max_se) return ;
    
    if (track < 0) {
        for (int i = 0; i < param_.max_se; ++i) {
            StopSe(i, time);
        }
	} else {
		auto& player = player_[GetTrackIndex_(SoundType::SE, track)];
		if (time == 0) {
			criAtomExPlayer_StopWithoutReleaseTime(player);
		} else {
			criAtomExPlayer_SetFadeOutTime(player, static_cast<CriSint32>(time * 1000));
			criAtomExPlayer_Stop(player);
		}
	}
}
void SoundManager::SetVolumeSe(float volume)
{
    if (!initialized_) return ;
    
    se_volume_ = volume;
    for (int i = 0; i < param_.max_se; ++i) {
        auto& player = player_[GetTrackIndex_(SoundType::SE, i)];
        criAtomExPlayer_SetVolume(player, volume);
        criAtomExPlayer_UpdateAll(player);
    }
}

bool SoundManager::IsStopSe(int track) const
{
    if (!initialized_) return true;
	if (0 < track || track >= param_.max_se) return true;

	auto& player = player_[GetTrackIndex_(SoundType::SE, track)];
    auto state = criAtomExPlayer_GetStatus(player);
    return state == CRIATOMEXPLAYER_STATUS_STOP ||
           state == CRIATOMEXPLAYER_STATUS_PLAYEND ||
           state == CRIATOMEXPLAYER_STATUS_ERROR;
}

// ------------------------------------------------------------------------------------------------------------------------
// VOICE
// ------------------------------------------------------------------------------------------------------------------------
int SoundManager::PlayVoice(const std::string& file, int id, int track, float delay)
{
    if (!initialized_) return -1;
    if (track >= param_.max_voice) return -1;
    auto acb_func = [id](CriAtomExPlayerHn player, CriAtomExAcbHn acb){
        criAtomExPlayer_SetCueId(player, acb, id);
    };
    return PlayVoice_(acb_func, file, track, delay);
}

int SoundManager::PlayVoice(const std::string& file, const std::string& cue, int track, float delay)
{
    if (!initialized_) return -1;
    if (track >= param_.max_voice) return -1;
    auto acb_func = [cue](CriAtomExPlayerHn player, CriAtomExAcbHn acb){
        criAtomExPlayer_SetCueName(player, acb, cue.c_str());
    };
    return PlayVoice_(acb_func, file, track, delay);
}

void SoundManager::StopVoice(int track, float time)
{
    if (!initialized_) return ;
    if (track >= param_.max_voice) return ;
    
    if (track < 0) {
        for (int i = 0; i < param_.max_voice; ++i) {
            StopVoice(i, time);
        }
	} else {
		auto& player = player_[GetTrackIndex_(SoundType::VOICE, track)];
		if (time == 0) {
			criAtomExPlayer_StopWithoutReleaseTime(player);
		} else {
			criAtomExPlayer_SetFadeOutTime(player, static_cast<CriSint32>(time * 1000));
			criAtomExPlayer_Stop(player);
		}
	}
}

void SoundManager::SetVolumeVoice(float volume)
{
    if (!initialized_) return ;
    
    voice_volume_ = volume;
    for (int i = 0; i < param_.max_voice; ++i) {
        auto& player = player_[GetTrackIndex_(SoundType::VOICE, i)];
        criAtomExPlayer_SetVolume(player, volume);
        criAtomExPlayer_UpdateAll(player);
    }
}

bool SoundManager::IsStopVoice(int track) const
{
    if (!initialized_) return true;
    if (track >= param_.max_voice) return true;
    
	auto& player = player_[GetTrackIndex_(SoundType::VOICE, track)];
	auto state = criAtomExPlayer_GetStatus(player);
    return state == CRIATOMEXPLAYER_STATUS_STOP ||
           state == CRIATOMEXPLAYER_STATUS_PLAYEND ||
           state == CRIATOMEXPLAYER_STATUS_ERROR;
}

// ------------------------------------------------------------------------------------------------------------------------
// その他
// ------------------------------------------------------------------------------------------------------------------------
void SoundManager::Update(float df)
{
	if (!initialized_) return ;

	criAtomEx_ExecuteMain();
}

void SoundManager::StopAll(float time)
{
    if (!initialized_) return ;
    
	for (size_t i = 0; i < player_.size(); ++i) {
		auto& player = player_[i];
        if (time == 0) {
            criAtomExPlayer_StopWithoutReleaseTime(player);
        } else {
            criAtomExPlayer_SetFadeOutTime(player, static_cast<CriSint32>(time * 1000));
            criAtomExPlayer_Stop(player);
        }
    }
	last_bgm_name_ = "";
	last_bgm_id_ = -1;
}

// ------------------------------------------------------------------------------------------------------------------------
// private関数
// ------------------------------------------------------------------------------------------------------------------------
int SoundManager::GetTrackIndex_(SoundType::Type type, int index) const
{
    if (type == SoundType::BGM) {
        return index;
    } else if (type == SoundType::SE) {
        return index + param_.max_bgm;
    }
    return index + param_.max_bgm + param_.max_se;
}

void SoundManager::ReleaseAcb_(CriAtomExAcbHn& handle)
{
    if (handle != nullptr) {
        criAtomExAcb_Release(handle);
        handle = nullptr;
    }
}

void SoundManager::ReleaseAcbList_(std::list<AcbInfo>& handles)
{
	for (auto it = handles.begin(); it != handles.end(); ++it) {
        criAtomExAcb_Release(it->data);
    }
    handles.clear();
}

void SoundManager::ReleaseVoicePool_(CriAtomExVoicePoolHn& handle)
{
    if (handle != nullptr) {
        criAtomExVoicePool_Free(handle);
        handle = nullptr;
    }
}

CriAtomExAcbHn SoundManager::GetAcbData_(std::list<AcbInfo>& acb_infos, const std::string& file, int max_value, bool is_awb)
{
    auto it = std::find_if(acb_infos.begin(), acb_infos.end(),
                           [file](const AcbInfo& info){ return info.file == file; });
    if (it != acb_infos.end()) {
        auto data = *it;
		if (it != acb_infos.begin()) {
			// データを一番新しい位置に配置
			acb_infos.erase(it);
			acb_infos.push_front(data);
		}
        return data.data;
    }
    
    // 拡張子なしのパスを取得
    std::string base = file;
    size_t pos = base.find_last_of('.');
    if (pos != std::string::npos) base = base.substr(0, pos);

    // ファイルのチェック
    auto acb = base + ".acb";
    auto awb = base + ".awb";

	if (!File::Exist(acb)) return nullptr;
	if (is_awb && !File::Exist(awb)) return nullptr;

    // データ生成
    AcbInfo info;
    info.file = file;
	if (is_awb) {
		info.data = criAtomExAcb_LoadAcbFile(nullptr, acb.c_str(), nullptr, awb.c_str(), nullptr, 0);
	} else {
		info.data = criAtomExAcb_LoadAcbFile(nullptr, acb.c_str(), nullptr, nullptr, nullptr, 0);
	}
    if (info.data == nullptr) return nullptr;

    // リストに登録
    if (acb_infos.size() >= static_cast<size_t>(max_value)) {
        // 一番古いものを消す
        ReleaseAcb_(acb_infos.back().data);
        acb_infos.pop_back();
    }
    acb_infos.push_front(info);
    
    return info.data;
}

int SoundManager::PlayBgm_(std::function<void(CriAtomExPlayerHn, CriAtomExAcbHn)> acb_func, float delay, float fadein_time, float fadeout_time, int track)
{
    if (base_bgm_data_ == nullptr) return -1 ;

    // 空いているトラックを確認
    if (track < 0) {
        for (int i = 0; i < param_.max_bgm; ++i) {
            if (IsStopBgm(i)) {
                track = i;
                break;
            }
        }
        if (track < 0) return -1;
    }


	auto& player = player_[GetTrackIndex_(SoundType::BGM, track)];
	acb_func(player, base_bgm_data_);
	criAtomExPlayer_SetFadeInStartOffset(player, static_cast<CriSint32>(delay * 1000));
	criAtomExPlayer_SetFadeInTime(player, static_cast<CriSint32>(fadein_time * 1000));
	criAtomExPlayer_SetFadeOutTime(player, static_cast<CriSint32>(fadeout_time * 1000));
	criAtomExPlayer_UpdateAll(player);
	criAtomExPlayer_Start(player);

	return track;
}

int SoundManager::PlaySe_(std::function<void(CriAtomExPlayerHn)> acb_func, int track, float delay)
{
    if (base_se_data_ == nullptr) return -1;
	
    // 空いているトラックを確認
    if (track < 0) {
        for (int i = 0; i < param_.max_se; ++i) {
            if (IsStopSe(i)) {
                track = i;
                break;
            }
        }
        if (track < 0) return -1;
    }

	auto& player = player_[GetTrackIndex_(SoundType::SE, track)];
	criAtomExPlayer_StopWithoutReleaseTime(player);
	acb_func(player);
	criAtomExPlayer_SetFadeInStartOffset(player, static_cast<CriSint32>(delay * 1000));
	criAtomExPlayer_SetFadeInTime(player, 0);
	criAtomExPlayer_SetFadeOutTime(player, 0);
	criAtomExPlayer_UpdateAll(player);
	criAtomExPlayer_Start(player);

	return track;
}

int SoundManager::PlayVoice_(std::function<void(CriAtomExPlayerHn, CriAtomExAcbHn)> acb_func, const std::string& file, int track, float delay)
{
    // 空いているトラックを確認
    if (track < 0) {
        for (int i = 0; i < param_.max_voice; ++i) {
            if (IsStopVoice(i)) {
                track = i;
                break;
            }
        }
        if (track < 0) return -1;
    }

	// ファイルからacbを取得
	CriAtomExAcbHn acb_handle = GetAcbData_(voice_list_, file, MAX_KEEP_VOICE_VALUE, false);
	if (acb_handle == nullptr) return -1;

	auto& player = player_[GetTrackIndex_(SoundType::VOICE, track)];
	criAtomExPlayer_StopWithoutReleaseTime(player);
	acb_func(player, acb_handle);
	criAtomExPlayer_SetFadeInStartOffset(player, static_cast<CriSint32>(delay * 1000));
	criAtomExPlayer_SetFadeInTime(player, 0);
	criAtomExPlayer_SetFadeOutTime(player, 0);
	criAtomExPlayer_UpdateAll(player);
	criAtomExPlayer_Start(player);

    return track;
}
