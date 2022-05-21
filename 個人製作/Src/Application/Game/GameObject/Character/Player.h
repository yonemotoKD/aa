#pragma once
enum SlimeType
{
	Normal,
	Asid,
	Fire
};
class Map;
class Player
{
public:
	Player() {};
	~Player() { Release(); };

	void Init();
	void Update();
	void Draw();
	void SetPos(Math::Vector2 a_pos);
	const Math::Vector2 GetPos()const
	{
		return m_pos;
	};
	const float GetSize()const
	{
		return m_size;
	};
	void SetMap(Map* a_pOwner);
	const Math::Vector2 GetScreenX();//m_screenX‚ğ•Ô‚·
private:
	SlimeType type;
	KdTexture m_Play;
	KdTexture m_AsidPlay;
	KdTexture m_FirePlay;

	Map* m_pmap = nullptr;
	Math::Matrix m_mWorld;

	const float gravity = 1.0f;
	const float m_radius = 20.0f;
	float m_jump = 17.0f;
	float m_jumpScreen = 17.0f;
	float m_size = 1.0f;
	Math::Vector2 m_pos;//À•W
	Math::Vector2 m_move;//ˆÚ“®—Ê
	Math::Vector2 m_Screen;//ˆÚ“®—Ê
	Math::Vector2 m_Screenspead;//ˆÚ“®—Ê

	bool jumpFlg;
	bool TypeSwap;
	bool asid;
	int time = 0;
	void Release();
};