#pragma once
class Player;
class Map;
class Ballet
{
public:
	Ballet() {};
	~Ballet() { Release(); };


	void Init();
	void Update();
	void Draw();
	void SetPos(Math::Vector2 a_pos);
	void SetPlay(Player* a_pOwner);
	void SetMap(Map* a_pOwner);
private:
	Player* m_play = nullptr;
	Map* m_map = nullptr;
	KdTexture m_Ballet;
	KdTexture m_AsidBallet;
	KdTexture m_FireBallet;

	static const int BulletNum = 2;
	Math::Matrix m_mWorld[BulletNum];
	Math::Vector2 m_pos[BulletNum];//���W
	Math::Vector2 m_bpos[BulletNum];//���W
	Math::Vector2 m_move[BulletNum];//�ړ���
	Math::Vector2 m_moveScreen;//�ړ���
	Math::Matrix ScaleMat[BulletNum], TransMat[BulletNum], RotateMat[BulletNum];
	int ballet_type;
	const float m_radius = 20.0f;
	float movespeed = 0.0f;
	float m_screenX;
	float m_size[BulletNum];
	
	
	
	bool balletAlive[BulletNum];
	void Release();
};