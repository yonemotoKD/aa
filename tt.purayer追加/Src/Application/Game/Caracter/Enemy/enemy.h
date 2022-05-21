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
private:
	KdSquarePolygon* m_Enemy;
	Player* m_Target;

	float movespeed = 0.02f;
	Math::Matrix m_mWorld;
	Math::Vector3 m_moveDir = { 0.0f,0.0f,0.0f };
	void Release();
};
