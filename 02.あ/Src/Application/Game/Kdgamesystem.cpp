#include "Kdgamesystem.h"

KdgameSystem::KdgameSystem()
{

}

KdgameSystem::~KdgameSystem()
{

}

void KdgameSystem::Init()
{

	poly.SetMaterial("Asset/Texture/Enviroment/Tree.png");
}

void KdgameSystem::Update()
{

}

void KdgameSystem::Draw()
{
	D3D.WorkShaderManager().m_HD2DShader.DrawSquarePolygon(poly);
}

void KdgameSystem::Release()
{

}
