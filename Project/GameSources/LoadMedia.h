/*!
@file LoadMedia.h
@brief mediaファイルの読み込み
*/

#pragma once
#include "stdafx.h"

namespace basecross
{
	class LoadMedia
	{
	private:
		vector<wstring> m_skipFolderNames;
		map<wstring, wstring> m_parentSkipFolderNames;
	public:
		LoadMedia();
		~LoadMedia();

		void RegisterMediaFiles(const wstring& Directory);
		bool ShouldSkipFolder(const wstring& ParentDir, const wstring& FolderName);
		void RegisterSingleModel(const wstring& Key, const wstring& ModelDir, const wstring& ModelFile, bool AccessWrite = false);
		void RegisterDXModel(const wstring& Key, const wstring& ModelPath);
	};
}