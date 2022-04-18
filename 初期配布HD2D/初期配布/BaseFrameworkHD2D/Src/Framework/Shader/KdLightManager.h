#pragma once

class KdLightManager
{
public:
	KdLightManager() { m_dirLightDir.Normalize(); }
	~KdLightManager() {}

	void Init();

	void PreUpdate();

	void PreDraw();

	void AddPointLight(const Math::Vector3& Color, float Radius, const Math::Vector3& Pos, bool IsBright = false);
	void AddPointLight(const KdShaderManager::cbLight::PointLight& pointLight);

	void SetDirLightDirection(const Math::Vector3& dir) { m_dirLightDir = dir; m_dirLightDir.Normalize(); }
	void SetDirLightColor(const Math::Vector3& col) { m_dirLightColor = col; }

	// 環境光
	Math::Vector4		m_ambientLightColor = { 0.3f, 0.3f, 0.3f, 1.0f };// 光の色

	// 平行光
	Math::Vector3		m_dirLightDir = { -1.0f, -1.0f, 1.0f };		// 光の方向
	Math::Vector3		m_dirLightColor = { 2.0f , 2.0f , 2.0f };	// 光の色
};