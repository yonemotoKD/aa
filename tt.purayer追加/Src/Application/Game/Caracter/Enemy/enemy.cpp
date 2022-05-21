#include "../Player/player.h"
#include "enemy.h"
Enemy::Enemy()
{

}

Enemy::~Enemy()
{

}

void Enemy::Init()
{
	m_Enemy = new KdSquarePolygon();
	m_Enemy->SetMaterial("Asset/Texture/Enviroment/enemy.png");
	m_Enemy->SetSplit(4, 4);
	m_Enemy->SetScale({ 0.5f,0.5f });

	m_mWorld.Translation(Math::Vector3(3.0f, 0.0f, 5.0f));
}

void Enemy::Update()
{
	Math::Vector3 moveDir = { 1.0f,4.0f,0.0f };
	Math::Vector3 targetDir = m_Target->GetPos() - m_mWorld.Translation();
	m_Enemy->SetUVRect(0);
	m_mWorld *=
		Math::Matrix::CreateTranslation(targetDir * movespeed);
}

void Enemy::Draw()
{
	D3D.WorkShaderManager().m_HD2DShader.DrawSquarePolygon(*m_Enemy, m_mWorld);
}

void Enemy::SetTarget(Player* a_pOwner)
{
	if (a_pOwner == nullptr)return;	//nullチェック
	m_Target = a_pOwner;
}

void Enemy::Release()
{
	KdSafeDelete(m_Enemy);
}
