/*!
 * @file
 *
 * @brief サウンド管理クラスを提供します。
 *
 * @copyright
 */
#pragma once

#include <cri_le_atom_ex.h>
#include <string>
#include <list>
#include <vector>
#include <functional>

/**
 *@brief サウンド管理クラス
 */
class  SoundManager
{
public :
    /**
     * @brief 初期化パラメータ
     */
    struct InitializeParam {
        int max_bgm;
        int max_se;
        int max_voice;
        int max_sampling_rate;
        float frame_rate;
        const char* acf_file;
    };
    
public:
    /**
     * @brief 初期化を行います。
     *
     * @param param 初期化パラメータを指定します。
     */
    void Initialize(const InitializeParam& param);
    
    /**
     * @brief 終了化を行います。
     */
    void Finalize();
    
    /**
     * @brief BGMファイルを読み込みます。
     *
     * @param acb_file ACBファイルのパスを指定します。
     * @param awb_file AWBファイルのパスを指定します。
	 *
	 * @return 正常な場合は true を返します。
     */
    bool LoadBgmFile(const std::string& acb_file, const std::string& awb_file);

    /**
     * @brief SEファイルを読み込みます。
     *
     * @param acb_file ACBファイルのパスを指定します。
	 *
	 * @return 正常な場合は true を返します。
	 */
    bool LoadSeFile(const std::string& acb_file);
    
    /**
     * @brief BGMを再生します。
     *
	 * @param id idを指定します。
     * @param delay 再生開始までの待ち時間を秒で指定します。
     * @param fadein_time フェードインの時間を秒で指定します。
     * @param fadeout_time フェードアウトの時間を秒で指定します。
	 * @param track 再生トラックを指定します。
     *
     * @return 再生したトラックを返します。再生できなかった場合は-1を返します。
	 */
	int PlayBgm(int id, float delay = 0.0f, float fadein_time = 0.5f, float fadeout_time = 0.5f, int track = 0);

    /**
     * @brief BGMを再生します。
     *
     * @param cue cue名を指定します。
	 * @param delay 再生開始までの待ち時間を秒で指定します。
     * @param fadein_time フェードインの時間を秒で指定します。
     * @param fadeout_time フェードアウトの時間を秒で指定します。
	 * @param track 再生トラックを指定します。
     *
     * @return 再生したトラックを返します。再生できなかった場合は-1を返します。
	 */
    int PlayBgm(const std::string& cue, float delay = 0.0f, float fadein_time = 0.5f, float fadeout_time = 0.5f, int track = 0);

    /**
     * @brief BGMを停止します。
     *
	 * @param time フェードアウトの時間を秒で指定します。
	 * @param track 再生トラックを指定します。
	 */
    void StopBgm(float time = 0.5f, int track = 0);

    /**
     * @brief BGMのボリュームを設定します。
     *
     * @param volume 0.0～1.0で指定します。
     */
	void SetVolumeBgm(float volume);

    /**
     * @brief BGMのボリュームを取得します。
     *
     * @return ボリュームを返します。
     */
    float GetVolumeBgm() const { return bgm_volume_; }

    /**
     * @brief BGMの停止の有無を取得します。
     *
	 * @param track 再生トラックを指定します。
	 *
	 * @return 停止中の場合は true を返します。
     */
    bool IsStopBgm(int track = 0) const;
    
	/**
	 * @brief BGMを一時停止します。
	 */
	void PauseBgm(int track = 0);

	/**
	* @brief BGMを再開します。
	*/
	void ResumeBgm(int track = 0);

	/**
     * @brief SEを再生します。
     *
     * @param id idを指定します。
     * @param track 再生トラックを指定します。-1が指定された場合、空いているトラックを使って再生を行います。
     * @param delay 再生開始までの待ち時間を秒で指定します。
     *
     * @return 再生したトラックを返します。再生できなかった場合は-1を返します。
     */
    int PlaySe(int id, int track = -1, float delay = 0.0f);
    
    /**
     * @brief SEを再生します。
     *
     * @param cue cue名を指定します。
     * @param track 再生トラックを指定します。-1が指定された場合、空いているトラックを使って再生を行います。
     * @param delay 再生開始までの待ち時間を秒で指定します。
     *
     * @return 再生したトラックを返します。再生できなかった場合は-1を返します。
     */
    int PlaySe(const std::string& cue, int track = -1, float delay = 0.0f);
    
    /**
     * @brief SEを停止します。
     *
	 * @param track 停止するトラック番号を指定します。-1の場合は全てのSEを停止します。
	 * @param time フェードアウトの時間を秒で指定します。
     */
    void StopSe(int track = -1, float time = 0.5f);
    
    /**
     * @brief SEのボリュームを設定します。
     *
     * @param volume 0.0～1.0で指定します。
     */
    void SetVolumeSe(float volume);
    
    /**
     * @brief SEのボリュームを取得します。
     *
     * @return ボリュームを返します。
     */
    float GetVolumeSe() const { return se_volume_; }
    
    /**
     * @brief SEの停止の有無を取得します。
     *
     * @return 停止中の場合は true を返します。
     */
    bool IsStopSe(int track) const;

    /**
     * @brief ボイスを再生します。
     *
     * @param file ファイル名を指定します。
     * @param id idを指定します。
     * @param track 再生トラックを指定します。-1が指定された場合、空いているトラックを使って再生を行います。
     * @param delay 再生開始までの待ち時間を秒で指定します。
     *
     * @return 再生したトラックを返します。再生できなかった場合は-1を返します。
     */
    int PlayVoice(const std::string& file, int id, int track = -1, float delay = 0.0f);
    
    /**
     * @brief ボイスを再生します。
     *
     * @param file ファイル名を指定します。
     * @param cue cue名を指定します。
     * @param track 再生トラックを指定します。-1が指定された場合、空いているトラックを使って再生を行います。
     * @param delay 再生開始までの待ち時間を秒で指定します。
     *
     * @return 再生したトラックを返します。再生できなかった場合は-1を返します。
     */
    int PlayVoice(const std::string& file, const std::string& cue, int track = -1, float delay = 0.0f);

    /**
     * @brief ボイスを停止します。
     *
     * @param time フェードアウトの時間を秒で指定します。
     */
    void StopVoice(int track = -1, float time = 0.5f);

    /**
     * @brief ボイスのボリュームを設定します。
     *
     * @param volume 0.0～1.0で指定します。
     */
    void SetVolumeVoice(float volume);

    /**
     * @brief ボイスのボリュームを取得します。
     *
     * @return ボリュームを返します。
     */
    float GetVolumeVoice() const { return voice_volume_; }

    /**
     * @brief ボイスの停止の有無を取得します。
     *
     * @return 停止中の場合は true を返します。
     */
    bool IsStopVoice(int track) const;
    
    /**
     * @brief サウンドの更新処理を行います。
     *        このメソッドは毎フレーム呼び出しを行ってください。
     */
    void Update(float df);

    /**
     * @brief 全サウンドの停止を行います。
     *
     * @param time フェードアウトの時間を秒で指定します。
     */
    void StopAll(float time = 0.0f);
    
    static SoundManager* GetInstance() {
        static SoundManager v;
        return &v;
    }
private :
	class SoundType {
	public :
		enum Type {
			BGM,
			SE,
			VOICE
		};
	};
    struct AcbInfo {
        std::string file;
        CriAtomExAcbHn data;
    };

    SoundManager();
	~SoundManager();
    
    int GetTrackIndex_(SoundType::Type type, int index) const;
    void ReleaseAcb_(CriAtomExAcbHn& handle);
    void ReleaseAcbList_(std::list<AcbInfo>& handles);
    void ReleaseVoicePool_(CriAtomExVoicePoolHn& handle);
	CriAtomExAcbHn GetAcbData_(std::list<AcbInfo>& acb_infos, const std::string& file, int max_value, bool is_awb);
    
    int PlayBgm_(float delay, float fadein_time, float fadeout_time, int track, std::function<void(CriAtomExPlayerHn, CriAtomExAcbHn)> acb_func);
    int PlaySe_(int track, float delay, std::function<void(CriAtomExPlayerHn)> acb_func);
    int PlayVoice_(const std::string& file, int track, float delay, std::function<void(CriAtomExPlayerHn, CriAtomExAcbHn)> acb_func);

    InitializeParam param_;
    bool initialized_;
    
    CriAtomExAcbHn base_bgm_data_;
    CriAtomExAcbHn base_se_data_;
    std::list<AcbInfo> voice_list_;
    std::vector<CriAtomExPlayerHn> player_;
    std::string last_bgm_name_;
    int last_bgm_id_;
    
    float bgm_volume_;
    float se_volume_;
    float voice_volume_;
    
    CriAtomExVoicePoolHn hca_pool_;
    CriAtomExVoicePoolHn hca_mx_pool_;
    CriAtomDbasId dbas_id_;
};
