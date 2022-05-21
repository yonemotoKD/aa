#pragma once
class Player;
class Enemy
{
public:
	Enemy();
	~Enemy();

	void Init();
	void Update();
	void Draw();
	void SetTarget(Player* a_pOwner);
	const Math::Vector3 GetPos()const {
		return m_mWorld.Translation();
	}
	bool Enemy::CheckCollisionBump(const SphereInfo& info,
		BumpResult& result);

private:
	KdSquarePolygon* m_Enemy;
	Player* m_Target;

	float movespeed = 0.02f;
	Math::Matrix m_mWorld;
	Math::Vector3 m_moveDir = { 0.0f,0.0f,0.0f };
	void Release();
};
