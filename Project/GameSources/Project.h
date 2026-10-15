/*!
@file Project.h
@brief コンテンツ用のヘッダをまとめる
*/

#pragma once

// 特別上に無いといけないもの
#include "GameObjectForEdit.h"

// インプット関係
#include "ProjectBehavior.h"

// シーン関係
#include "Scene.h"

// オブジェクト関係
#include "Character.h"
#include "Player.h"
#include "MainCamera.h"
#include "Ground.h"
#include "Wall.h"
#include "Koban.h"

// ステージ関係
#include "GameStage.h"
#include "SelectStage.h"

// エディター関係
#include "Editor.h"
#include "Arrow.h"
#include "EditorCamera.h"
#include "RayHitCheck.h"
#include "StageLoader.h"
#include "EditObjectRegistry.h"

// ステートマシン関係
#include "CameraState.h"
