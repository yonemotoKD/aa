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

struct SphereInfo
{
	SphereInfo() = default;

	SphereInfo(const Math::Vector3& pos, float radius) :
		m_pos(pos),
		m_radius(radius) {}

	Math::Vector3 m_pos;
	float m_radius = 1.0f;
};

struct BumpResult
{
	bool m_IsHit = false;
	Math::Vector3 m_PushVec;
};

//球判定
bool Enemy::CheckCollisionBump(const SphereInfo& info,
	BumpResult& result)
{
	Math::Vector3 selfPos = GetPos();
	Math::Vector3 betweenVec = info.m_pos - selfPos;

	
	float distanceSqr = betweenVec.LengthSquared();

	
	float hitRadius = 0.25f + info.m_radius;

	result.m_IsHit = (distanceSqr <= (hitRadius * hitRadius));

	if (result.m_IsHit)
	{
		
		result.m_PushVec = betweenVec;
		result.m_PushVec.Normalize();

		float distance = std::sqrtf(distanceSqr);
		result.m_PushVec *= hitRadius - distance;
	}
	return result.m_IsHit;
}
void Enemy::Release()
{
	KdSafeDelete(m_Enemy);
}
