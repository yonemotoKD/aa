﻿#pragma once

//=========================================
// カメラクラス
//=========================================
class KdCamera
{
public:
	// コンストラクター
	KdCamera() {}

	~KdCamera() {}

	void SetProjectionMatrix(float fov, float maxRange = 2000, float minRange = 0.01f, float aspectRatio = D3D.GetBackBuffer()->GetAspectRatio());
	void SetProjectionMatrix(const DirectX::SimpleMath::Matrix& rProj);

	void SetFocus(float focusDist, float focusForeRange, float focusBackRange);

	// カメラ行列取得
	inline const DirectX::SimpleMath::Matrix& GetCameraMatrix() const { return m_mCam; }

	// ビュー行列取得
	inline const DirectX::SimpleMath::Matrix& GetViewMatrix() const { return m_mView; }
	
	// 射影行列取得
	inline const DirectX::SimpleMath::Matrix& GetProjMatrix() const { return m_mProj; }

	// カメラ行列のセット・カメラ行列からビュー行列を生成
	virtual void SetCameraMatrix(const DirectX::SimpleMath::Matrix& mCam);

	// ビュー行列のセット・ビュー行列からカメラ行列を生成
	virtual void SetViewMatrix(const DirectX::SimpleMath::Matrix& mView);

	// カメラ情報(ビュー・射影行列など)をシェーダへセット
	void SetToShader() const;

	const DirectX::SimpleMath::Matrix& GetCamera() { return m_mCam; }
	DirectX::SimpleMath::Matrix& WorkCamera() { return m_mCam; }

	// カメラからのレイ情報を生成
	static void GenerateCameraRayInfoFromClientPos(const POINT& clientPos, Math::Vector3& rayPos, Math::Vector3& rayDir, float& rayRange);

	//スクリーン座標を（2D座標を）ワールド座標に（3D座標に）置換する。
	void ConvertScreenToWorldDetail(const Math::Vector2& pos,Math::Vector3&resulut);
	
	//スクリーン座標を（2D座標を）ワールド座標に（3D座標に）置換する。
	void ConvertWorldDetailToScreen(const Math::Vector3& pos,Math::Vector2&resulut);


protected:

	// カメラ行列
	DirectX::SimpleMath::Matrix	m_mCam;
	// ビュー行列
	DirectX::SimpleMath::Matrix	m_mView;
	// 射影行列
	DirectX::SimpleMath::Matrix	m_mProj;

	float m_nearClippingDistance = 0.0f;
	float m_farClippingDistance = 2000.0f;

	float m_focusDistance = 0.0f;
	float m_focusForeRange = 0.0f;
	float m_focusBackRange = 2000.0f;
};
