#include "Kdgamesystem.h"
#include "Caracter/Player/player.h"
#include "Caracter/Enemy/enemy.h"

KdgameSystem::KdgameSystem()
{

}

KdgameSystem::~KdgameSystem()
{
	Release();
}

void KdgameSystem::Init()
{
	Shatter = new KdCamera();
	

	Shatter->SetProjectionMatrix(60.0f);

	poly.SetMaterial("Asset/Texture/Enviroment/Tree.png");
	m_pPlayer = new Player();
	m_pEnemy = new Enemy();
	m_pPlayer->Init();
	m_pEnemy->Init();
}

void KdgameSystem::Update()
{
	Math::Matrix ShatterMat = Math::Matrix::CreateTranslation(0.0f, 1.0f, -5.0f);

	Shatter->SetCameraMatrix(ShatterMat);

	Shatter->SetToShader();

	if (m_pPlayer)
	{
		m_pPlayer->Update();
	}
	if (m_pEnemy)
	{
		m_pEnemy->SetTarget(m_pPlayer);
		m_pEnemy->Update();
	}
	
}

void KdgameSystem::Draw()
{

	
	D3D.WorkShaderManager().m_HD2DShader.DrawSquarePolygon(poly);

	if (m_pPlayer)
	{
		m_pPlayer->Draw();
	}
	if (m_pEnemy)
	{
		m_pEnemy->Draw();
	}
}

void KdgameSystem::Release()
{
	KdSafeDelete(Shatter);
	KdSafeDelete(m_pPlayer);
	KdSafeDelete(m_pEnemy);

}
