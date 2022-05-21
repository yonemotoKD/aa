#pragma once
class Player;
class Enemy;
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

	KdCamera* GetCamera() {
		return Shatter;
	};
	void Release();
};