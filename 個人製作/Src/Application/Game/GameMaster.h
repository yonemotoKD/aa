#pragma once

class Player;
class Map;
class Ballet;
class GameSystem
{
public:
	GameSystem();
	~GameSystem() { Release(); };

	void Init();
	void Update();
	void Draw();

private:
	Player* m_pPlay;
	Map* m_pmap;
	Ballet* m_pballet;
	void Release();
};