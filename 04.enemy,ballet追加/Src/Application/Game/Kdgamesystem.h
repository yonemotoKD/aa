#pragma once
class Player;
class Enemy;
class Ballet;
class KdgameSystem
{
public:
	KdgameSystem();
	~KdgameSystem();

	void Init();
	void Update();
	void Draw();

private:
	KdSquarePolygon poly;
	KdCamera* Shatter;
	Player* m_pPlayer;
	Enemy* m_pEnemy;
	Ballet* m_pBallet;
	void Release();
};