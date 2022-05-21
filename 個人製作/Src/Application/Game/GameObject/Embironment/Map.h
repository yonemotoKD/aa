#pragma once
#include<fstream>
#include<sstream>
class Player;
class Map
{
public:
	Map() {};
	~Map() { Rerease(); };
	void  LoadMap(char* name);
	void Init();
	void Update();
	void Draw();
	void Map::collision(Math::Vector2* a_playerPos, float a_playerRadius, Math::Vector2* a_pScreen, Math::Vector2* b_playerPos, bool* jumpFlg);
	void Map::Balletcollision(Math::Vector2* a_playerPos, float a_playerRadius, Math::Vector2* b_playerPos, bool* BalletFlg);
	void Map::SetPlay(Player* a_pOwner) {
		if (a_pOwner == nullptr)return;	//nullチェック
		m_play = a_pOwner;
	}
	void Rerease();
private:
	KdTexture m_Map;
	KdTexture m_gimmick;
	static const int MAP_CHIP_H = 29;
	static const int MAP_CHIP_W = 140;
	const float CHIP_RADIUS = 24.0f;//タイルの半径
	float m_screenX;

	int mapChipData[MAP_CHIP_H][MAP_CHIP_W] = { NULL };

	Math::Matrix mapChipMat[MAP_CHIP_H][MAP_CHIP_W];
	Math::Vector2 m_moveScreen;
	float mapChipX[MAP_CHIP_H][MAP_CHIP_W] = { NULL };
	float mapChipY[MAP_CHIP_H][MAP_CHIP_W] = { NULL };

	bool ballet[MAP_CHIP_H][MAP_CHIP_W];
	Math::Rectangle chipRect;

	Player* m_play = nullptr;
	FILE* fp_c;
	//KdTexture* m_pMapTex;	//画像

	//汎用行列
	Math::Matrix matrix;
	Math::Color color = { 1.0f ,1.0f , 1.0f , 1.0f };//(R,G,B,A)...R = Red,G = Green,B = Blue,A = 府透明度
	//DirectX::SpriteBatch* spriteBatch;
	//DirectX::SpriteFont* spriteFont;

	
	
};
