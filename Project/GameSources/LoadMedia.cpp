/*!
@file LoadMedia.h
@brief mediaファイルの読み込み実体
*/

#include "stdafx.h"
#include "Project.h"
#include "DXModelResource.h"

namespace basecross
{
	LoadMedia::LoadMedia()
	{
		// 直下でスキップするフォルダ名
		m_skipFolderNames = { L"Shaders", L"StageDatas" };

		// 親フォルダと子フォルダの組み合わせでスキップするフォルダ名
		m_parentSkipFolderNames =
		{
			{ L"Effects", L"Texture" },
			{ L"Effects", L"Model" }
		};
	}

	LoadMedia::~LoadMedia()
	{}

	void LoadMedia::RegisterMediaFiles(const wstring& Directory)
	{
		// ファイルパスを作成
		auto filePath = Directory + L"\\*";
		WIN32_FIND_DATA findFileData;
		HANDLE hFind = FindFirstFile(filePath.c_str(), &findFileData);

		// ハンドルのオープンチェック
		if (hFind == INVALID_HANDLE_VALUE)
		{
			throw BaseException
			(
				L"ディレクトリを開けませんでした",
				L"if (hFind == INVALID_HANDLE_VALUE)",
				L"Scene::RegisterMediaFiles"
			);
			return;
		}

		do
		{
			// 特殊なエントリはスキップ
			if (wcscmp(findFileData.cFileName, L".") == 0 || wcscmp(findFileData.cFileName, L"..") == 0) continue;

			// 現在のエントリのフルパスを作成
			wstring fullPath = Directory + L"\\" + findFileData.cFileName;

			// エントリがディレクトリの場合
			if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				// スキップするフォルダかチェック
				if (ShouldSkipFolder(Directory, findFileData.cFileName))
				{
					continue;
				}

				// サブフォルダなので再帰
				RegisterMediaFiles(fullPath);
			}
			// エントリがファイルの場合
			else
			{
				// ファイル名を取得
				wstring fileNamePath = findFileData.cFileName;
				// pos(.)を取得
				size_t pos = fileNamePath.rfind(L'.');
				// 取得できなかった場合チェックしない
				if (pos != wstring::npos)
				{
					// pos(.)含めて拡張子を取得し一致するregister関数を呼ぶ
					wstring ext = fileNamePath.substr(pos);
					// pos(.)より前を登録名として取得
					wstring registerName = fileNamePath.substr(0, pos);
					if (ext == L".png")
					{
						App::GetApp()->RegisterTexture(registerName, fullPath);
					}
					else if (ext == L".bmf")
					{
						RegisterSingleModel(registerName, Directory + L"\\", findFileData.cFileName);
					}
					else if (ext == L".dxm")
					{
						RegisterDXModel(registerName, fullPath);
					}
					else if (ext == L".wav")
					{
						App::GetApp()->RegisterWav(registerName, fullPath);
					}
					else if (ext == L".bmp")
					{
						for (const auto& keyName : SkyBox::pairs)
						{
							App::GetApp()->RegisterTexture(registerName, fullPath);
						}
					}
				}
			}

		} while (FindNextFile(hFind, &findFileData) != 0);

		FindClose(hFind);
	}

	bool LoadMedia::ShouldSkipFolder(const wstring& ParentDir, const wstring& FolderName)
	{
		// 直下でスキップするフォルダ名
		for (const auto& skipFolder : m_skipFolderNames)
		{
			if (FolderName == skipFolder) return true;
		}

		// 親フォルダを取得
		size_t pos = ParentDir.rfind(L'\\');
		wstring parentFolder = (pos != wstring::npos) ? ParentDir.substr(pos + 1) : ParentDir;

		// Effects内のTextureフォルダはスキップする
		for (const auto& skipFolder : m_parentSkipFolderNames)
		{
			if (parentFolder == skipFolder.first && FolderName == skipFolder.second)
			{
				return true;
			}
		}

		return false;
	}

	void LoadMedia::RegisterSingleModel(const wstring& Key, const wstring& ModelDir, const wstring& ModelFile, bool AccessWrite)
	{
		try
		{
			wstring modelData = ModelDir + ModelFile;
			BinaryReader reader(modelData);
			BinaryReader modelTypeCheckReader(modelData);
			// ヘッダーの読込
			auto pHeader = reader.ReadArray<char>(16);
			pHeader = modelTypeCheckReader.ReadArray<char>(16);
			// マルチかどうかの分岐
			string str = pHeader;
			if (str != "BDV1.0")
			{
				throw BaseException
				(
					L"データ形式がシングルモデルではないです\n",
					modelData,
					L"Scene::RegisterSingleModel()"
				);
			}

			// 頂点を読み込んでタイプによってモデル形式を変える
			BlockHeader blockHeader = modelTypeCheckReader.Read<BlockHeader>();
			// 頂点がVertexかVertexWithTangentならStaticモデルとして読み込む
			if (blockHeader.m_Type == BlockType::Vertex)
			{
				auto sPtrStaticModel = MeshResource::CreateStaticModelMeshBase(reader, ModelDir, ModelFile, AccessWrite);
				App::GetApp()->RegisterResource(Key, sPtrStaticModel);
			}
			// SkinedならBone付きモデルとして読み込む
			else if (blockHeader.m_Type == BlockType::SkinedVertex)
			{
				auto sPtrBoneModel = MeshResource::CreateBoneModelMeshBase(reader, ModelDir, ModelFile, AccessWrite);
				App::GetApp()->RegisterResource(Key, sPtrBoneModel);
			}
		}
		catch (...)
		{
			throw;
		}

	}

	void LoadMedia::RegisterDXModel(const wstring& Key, const wstring& ModelPath)
	{
		auto dxmodel = dxm::DXModel::CreateFromFile(ModelPath);
		DXModelResource::RegisterDXModel(Key, dxmodel);
	}
}