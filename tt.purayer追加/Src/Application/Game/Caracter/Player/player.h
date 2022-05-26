#pragma once

class Player
{
public:
	Player();
	~Player();

	void Init();
	void Update();
	void Draw();

	const Math::Vector3 GetPos()const { 
		return m_mWorld.Translation();}

private:
	KdSquarePolygon* m_Play;
	KdTexture* m_pScopeTex;
	float movespeed = 0.0f;
	int animFlame = 0;
	Math::Matrix m_mWorld;
	Math::Vector3 m_moveDir = { 0.0f,0.0f,0.0f };
	void Release();
};

