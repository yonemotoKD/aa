#pragma once

//====================================================
//
// システム系のヘッダーファイル
//
//====================================================

// 便利機能
#include "Utility/KdUtility.h"
#include "Utility/KdCSVData.h"

// 音関連
#include "Audio/KdAudio.h"

// バッファ
#include "Direct3D/KdBuffer.h"
// Direct3D
#include "Direct3D/KdDirect3D.h"
// テクスチャ
#include "Direct3D/KdTexture.h"
// メッシュ
#include "Direct3D/KdMesh.h"
// モデル
#include "Direct3D/KdModel.h"
// カメラ
#include "Direct3D/KdCamera.h"
// 板ポリゴン
#include "Direct3D/KdSquarePolygon.h"
// 軌跡ポリゴン
#include "Direct3D/KdTrailPolygon.h"

// アニメーション
#include "Math/KdAnimation.h"
// コマ送りアニメーション
#include "Math/KdFrameAnimation.h"
// 当たり判定
#include "Math/KdCollision.h"
// 形状毎の当たり判定
#include "Math/KdCollider.h"
// 数値に緩急を付ける機能
#include "Math/KdEasing.h"

// 入力関連
#include "Input/KdInput.h"

// シェーダ
#include "Shader/KdShaderManager.h"
#include "Shader/KdLightManager.h"

// ウィンドウ
#include "Window/KdWindow.h"

// ゲームオブジェクト関連
#include "GameObject/KdGameObject.h"
#include "GameObject/KdGameObjectFactory.h"