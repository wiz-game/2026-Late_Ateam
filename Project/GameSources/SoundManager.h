/*!
@file SoundManager.h
@brief BGMとSEを管理するクラス
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	class SoundManager {
	private:
		weak_ptr<SoundItem> m_bgm;
		vector<weak_ptr<SoundItem>> m_seList;
		weak_ptr<XAudio2Manager> m_xAudioMana;

		float m_bgmVolume;
		float m_seVolume;
		float m_bgmMasterVolume;
		float m_seMasterVolume;

		SoundManager();
		virtual ~SoundManager() {}

	public:
		static SoundManager& Instance();

		// BGMの再生/停止
		void PlayBGM(wstring BGMKey);
		void StopBGM();

		// SEの再生/停止
		weak_ptr<SoundItem> PlaySE(wstring SEKey, bool IsLoop = false);
		void StopSE(weak_ptr<SoundItem> WPtrSEItem);

		// SE再生中に出たいらないメモリの解放
		void AutoEraseSE();

		// 全てのSEの再生を停止
		void AllStopSE();

		// BGMボリュームのセッター/ゲッター
		void SetBGMVolume(float SetVolume);
		float GetBGMVolume();

		// SEボリュームのセッター/ゲッター
		void SetSEVolume(float SetVolume);
		float GetSEVolume();
	};
}