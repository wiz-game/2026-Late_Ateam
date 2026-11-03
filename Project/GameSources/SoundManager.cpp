/*!
@file SoundManager.cpp
@brief BGMとSEを管理するクラス
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	//--------------------------------------------------------------------------------------
	//	class SoundManager : singleton
	//--------------------------------------------------------------------------------------
	SoundManager& SoundManager::Instance() {
		static SoundManager instance;
		return instance;
	}

	SoundManager::SoundManager() {
		m_xAudioMana = App::GetApp()->GetXAudio2Manager();
		m_bgm.reset();

		m_bgmVolume = 1.0f;
		m_seVolume = 1.0f;
		m_bgmMasterVolume = 1.0f;
		m_seMasterVolume = 1.0f;
	}

	void SoundManager::PlayBGM(wstring BGMKey) {
		if (m_bgm.expired()) {
			auto sPtrXAudioMana = m_xAudioMana.lock();
			if (sPtrXAudioMana) {
				m_bgm = sPtrXAudioMana->Start(BGMKey, XAUDIO2_LOOP_INFINITE, m_bgmMasterVolume * m_bgmVolume);
			}
		}
	}

	void SoundManager::StopBGM() {
		if (!m_bgm.expired()) {
			auto sPtrXAudioMana = m_xAudioMana.lock();
			if (sPtrXAudioMana) {
				auto sPtrBGM = m_bgm.lock();
				if (sPtrBGM) {
					sPtrXAudioMana->Stop(sPtrBGM);
					m_bgm.reset();
				}
			}
		}
	}

	weak_ptr<SoundItem> SoundManager::PlaySE(wstring SEKey, bool IsLoop) {
		weak_ptr<SoundItem> seItem;
		size_t loop;
		if (IsLoop) {
			loop = XAUDIO2_LOOP_INFINITE;
		}
		else {
			loop = false;
		}
		auto sPtrXAudioMana = m_xAudioMana.lock();
		if (sPtrXAudioMana) {
			seItem = sPtrXAudioMana->Start(SEKey, loop, m_seMasterVolume * m_seVolume);
			m_seList.push_back(seItem);
		}
		return seItem;
	}

	void SoundManager::StopSE(weak_ptr<SoundItem> WPtrSEItem)
	{
		auto sPtrSEItem = WPtrSEItem.lock();
		if (sPtrSEItem)
		{
			for (int i = 0; i < m_seList.size(); i++)
			{
				auto sPtrSE = m_seList[i].lock();
				if (sPtrSE)
				{
					// 指定と一致したSEを削除
					if (sPtrSE == sPtrSEItem) {
						auto sPtrXAudioMana = m_xAudioMana.lock();
						sPtrXAudioMana->Stop(sPtrSE);
						m_seList.erase(m_seList.begin() + i);
						break;
					}
				}
			}
		}
	}

	void SoundManager::AutoEraseSE() {
		for (int i = 0; i < m_seList.size(); i++)
		{
			auto sPtrSE = m_seList[i].lock();
			if (!sPtrSE)
			{
				auto sPtrXAudioMana = m_xAudioMana.lock();
				sPtrXAudioMana->Stop(sPtrSE);
				m_seList.erase(m_seList.begin() + i);
				continue;
			}
			auto sPtrAudioResource = sPtrSE->m_AudioResource.lock();
			if (!sPtrAudioResource)
			{
				auto sPtrXAudioMana = m_xAudioMana.lock();
				sPtrXAudioMana->Stop(sPtrSE);
				m_seList.erase(m_seList.begin() + i);
				continue;
			}
		}
	}

	void SoundManager::AllStopSE()
	{
		auto sPtrXAudioMana = m_xAudioMana.lock();
		if (sPtrXAudioMana)
		{
			for (auto& se : m_seList)
			{
				auto sPtrSE = se.lock();
				if (sPtrSE)
				{
					sPtrXAudioMana->Stop(sPtrSE);
				}
			}

			m_seList.clear();
		}
	}

	void SoundManager::SetBGMVolume(float SetVolume)
	{
		m_bgmVolume = SetVolume;
		auto sPtrBGM = m_bgm.lock();
		if (sPtrBGM)
		{
			sPtrBGM->m_SourceVoice->SetVolume(m_bgmMasterVolume * m_bgmVolume);
		}
	}

	float SoundManager::GetBGMVolume()
	{
		return m_bgmVolume;
	}

	void SoundManager::SetSEVolume(float SetVolume)
	{
		m_seVolume = SetVolume;
		for (auto se : m_seList)
		{
			auto sPtrSE = se.lock();
			if (sPtrSE)
			{
				auto sPtrAudioResource = sPtrSE->m_AudioResource.lock();
				if (sPtrAudioResource)
				{
					sPtrSE->m_SourceVoice->SetVolume(m_seMasterVolume * m_seVolume);
				}
			}
		}
	}

	float SoundManager::GetSEVolume()
	{
		return m_seVolume;
	}
}