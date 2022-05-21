#pragma once
#include <Src/Application/Game/Caracter/Enemy/enemy.h>
class Ballet
{
public:
	Ballet();
	~Ballet();
	void SetPos(const Math::Vector3 pos) { m_mWorld.Translation(pos); };
	void Init();
	void Update();
	void Draw();
	void SetMoveDir(Math::Vector3 dir) { m_moveDir = dir; };
	//void SetTarget(Enemy* Target) { m_target = Target; };
private:
	KdSquarePolygon* m_Ballet;
	//Player* m_Target;

	float movespeed = 0.02f;
	Math::Matrix m_mWorld;
	Math::Vector3 m_moveDir = { 0.0f,0.0f,0.0f };
	Math::Vector3 m_target = { 0.0f,0.0f,0.0f };
	void Release();
};