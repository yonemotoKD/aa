#include "KdLightManager.h"

void KdLightManager::Init()
{
}

void KdLightManager::PreUpdate()
{
	KdShaderManager::cbLight& cbLight = D3D.WorkShaderManager().m_cb8_Light.Work();

	// ポイントライトの使用数の初期化
	cbLight.PointLight_Num = 0;
}

void KdLightManager::PreDraw()
{
	KdShaderManager::cbLight& cbLight = D3D.WorkShaderManager().m_cb8_Light.Work();

	// 情報をセットする
	cbLight.DirLight_Dir = m_dirLightDir;
	cbLight.DirLight_Color = m_dirLightColor;
	cbLight.AmbientLight = m_ambientLightColor;

	// DepthMapFromLight作成・Shadow描画用行列の作成
	Math::Vector3 lightDir = cbLight.DirLight_Dir;
	Math::Vector3 lightPos = D3D.WorkShaderManager().m_cb7_Camera.Get().CamPos;

	// ビュー行列
	cbLight.DirLight_mVP = DirectX::XMMatrixLookAtLH(lightPos - lightDir * 40, lightPos, Math::Vector3::Up);
	// 平行光の正射影行列を合成
	cbLight.DirLight_mVP *= DirectX::XMMatrixOrthographicLH(50, 50, 0, 100);

	// 光の情報をシェーダーに送る
	D3D.WorkShaderManager().m_cb8_Light.Write();
}

void KdLightManager::AddPointLight(const Math::Vector3& Color, float	Radius, const Math::Vector3& Pos, bool IsBright)
{
	KdShaderManager::cbLight& cbLight = D3D.WorkShaderManager().m_cb8_Light.Work();

	if (cbLight.PointLight_Num >= KdShaderManager::cbLight::MaxPointLightNum) { return; }

	cbLight.PointLights[cbLight.PointLight_Num].Color = Color;
	cbLight.PointLights[cbLight.PointLight_Num].Radius = Radius;
	cbLight.PointLights[cbLight.PointLight_Num].Pos = Pos;
	cbLight.PointLights[cbLight.PointLight_Num].IsBright = IsBright;

	++cbLight.PointLight_Num;
}

void KdLightManager::AddPointLight(const KdShaderManager::cbLight::PointLight& pointLight)
{
	KdShaderManager::cbLight& cbLight = D3D.WorkShaderManager().m_cb8_Light.Work();

	if (cbLight.PointLight_Num >= KdShaderManager::cbLight::MaxPointLightNum) { return; }

	cbLight.PointLights[cbLight.PointLight_Num] = pointLight;

	++cbLight.PointLight_Num;
}