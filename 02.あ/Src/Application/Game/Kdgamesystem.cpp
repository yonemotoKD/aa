#include "Kdgamesystem.h"

KdgameSystem::KdgameSystem()
{

}

KdgameSystem::~KdgameSystem()
{

}

void KdgameSystem::Init()
{
	Shatter = new KdCamera();
	Shatter->SetProjectionMatrix(60.0f);

	poly.SetMaterial("Asset/Texture/Enviroment/Tree.png");

	
}

void KdgameSystem::Update()
{
	Math::Matrix ShatterMat = Math::Matrix::CreateTranslation(0.0f, 1.0f, -5.0f);

	Shatter->SetCameraMatrix(ShatterMat);

	Shatter->SetToShader();
}

void KdgameSystem::Draw()
{

	
	D3D.WorkShaderManager().m_HD2DShader.DrawSquarePolygon(poly);
}

void KdgameSystem::Release()
{
	KdSafeDelete(Shatter);

}
