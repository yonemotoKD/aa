#include "Map.h"
#include"Application/Game/GameSystem/collision.h"
#include "Application/Game/GameObject/Character/Player.h"
void Map::LoadMap(char* name)
{
	FILE* fp;//fileポインタを作成

	if ((fp = fopen(name, "r")) != nullptr)
	{
		std::cout << "file読み取り成功" << std::endl;

		char ch;//格納する文字

		int h = 0;//縦カウント
		int w = 0;//横カウント

		//格納した文字がEOFじゃないとき処理を繰り返す
		//もしその文字が改行だったら縦のカウントを動かして横のカウントを戻す
		//MapChipDataの配列に文字を数字に直したものを格納し横のカウントを動かす
		while ((ch = fgetc(fp)) != EOF)
		{
			if (ch == '\n')
			{
				h++;
				w = 0;
			}
			else
			{
				mapChipData[h][w] = ch - 0x30;
				w++;
			}
		}
		fclose(fp);
	}
	else
	{
		std::cout << ("file読み取り失敗") << std::endl;
	}
}
void Map::Init()
{
	m_Map.Load("Asset/Texture/Map/grand.png");
	m_gimmick.Load("Asset/Texture/Map/gimmick.png");
	//ステージのロード
	LoadMap("Asset/Texture/Map/Stage.txt");

	for (int h = 0; h < MAP_CHIP_H; h++)
	{
		for (int w = 0; w < MAP_CHIP_W; w++)
		{
			std::cout << mapChipData[h][w];
		}
		std::cout << std::endl;
	}

	//マップの初期化
	for (int h = 0; h < MAP_CHIP_H; h++)
	{
		for (int w = 0; w < MAP_CHIP_W; w++)
		{
			mapChipX[h][w] = -640 + (48.0f * w);//画面の左上から
			mapChipY[h][w] = 360 - (48.0f * h);
			ballet[h][w] = true;
		}
	}
	m_screenX = 0;
}

void Map::Update()
{
	/*if (GetAsyncKeyState('D'))
	{
		m_screenX += 3;
	}
	if (GetAsyncKeyState('A'))
	{
		if (m_screenX >= 0)
		{
			m_screenX -= 3;
		}

	}*/

	
	for (int h = 0; h < MAP_CHIP_H; h++)
	{
		for (int w = 0; w < MAP_CHIP_W; w++)
		{
			m_moveScreen = m_play->GetScreenX();
			//mapChipMat[h][w] = DirectX::XMMatrixTranslation(mapChipX[h][w] , mapChipY[h][w], 0);
			mapChipMat[h][w] = DirectX::XMMatrixTranslation(mapChipX[h][w] - m_moveScreen.x, mapChipY[h][w] - m_moveScreen.y, 0);
		}
	}
	
}

void Map::Draw()
{
	//mapの表示
	
	for (int h = 0; h < MAP_CHIP_H; h++)
	{
		for (int w = 0; w < MAP_CHIP_W; w++)
		{
			D3D.WorkShaderManager().m_spriteShader.SetMatrix(mapChipMat[h][w]);
			switch (mapChipData[h][w])
			{
			case 0:
				continue;
				break;
			case 1:
				chipRect = { 64,0,64,64 };
				D3D.WorkShaderManager().m_spriteShader.DrawTex(&m_Map, 0, 0, 48, 48, &chipRect, &color, Math::Vector2(0.5f, 0.5f));
				break;
			case 2:
				chipRect = { 0,0,64,64 };
				D3D.WorkShaderManager().m_spriteShader.DrawTex(&m_Map, 0, 0, 48, 48, &chipRect, &color, Math::Vector2(0.5f, 0.5f));
				break;
			case 3:
				if (ballet[h][w])
				{
					chipRect = { 0,0,64,64 };
					D3D.WorkShaderManager().m_spriteShader.DrawTex(&m_gimmick, 0, 0, 48, 48, &chipRect, &color, Math::Vector2(0.5f, 0.5f));
				}
				break;
			case 4:
				chipRect = { 64,0,64,64 };
				D3D.WorkShaderManager().m_spriteShader.DrawTex(&m_gimmick, 0, 0, 48, 48, &chipRect, &color, Math::Vector2(0.5f, 0.5f));
				break;
			case 5:
				chipRect = { 128,0,64,64 };
				D3D.WorkShaderManager().m_spriteShader.DrawTex(&m_Map, 0, 0, 48, 48, &chipRect, &color, Math::Vector2(0.5f, 0.5f));
				break;
			default:
				break;
			}
			
			
			
			
			
		}
	}
}
void Map::collision(Math::Vector2* a_playerPos, float a_playerRadius, Math::Vector2* a_pScreen, Math::Vector2* b_playerPos, bool* jumpFlg)
{
	//矩形のセット
	Rect object = Collision::SetRect(a_playerPos, a_playerRadius);

	//未来座標のセット
	Rect next = Collision::SetFutureRect(object, b_playerPos);

	for (int h = 0; h < MAP_CHIP_H; h++)
	{
		for (int w = 0; w < MAP_CHIP_W; w++)
		{
			//データなし、中間ポイント、ゴールポイントか判定(0, 1, 2)
			if ((mapChipData[h][w] == 0)||
				(mapChipData[h][w] == 4)||
				(mapChipData[h][w] == 5)
				)
			{
			continue;
			}
			//マップチップの矩形セット
		        //const Rect CHIP_RECT = collision::SetRect(Math::Vector2(mapChipX[h][w], mapChipY[h][w]), CHIP_RADIUS);
			
				Rect CHIP_RECT;
				CHIP_RECT.Up = mapChipY[h][w] + CHIP_RADIUS;
				CHIP_RECT.Down = mapChipY[h][w] - CHIP_RADIUS;
				CHIP_RECT.Right = mapChipX[h][w] + CHIP_RADIUS;
				CHIP_RECT.Left = mapChipX[h][w] - CHIP_RADIUS;
				if ((mapChipData[h][w] == 1) ||
					(mapChipData[h][w] == 2))
				{

					if ((object.Right > CHIP_RECT.Left) && (object.Left < CHIP_RECT.Right))
					{
						if ((next.Up > CHIP_RECT.Up) && (next.Down < CHIP_RECT.Up))				//チップの上判定
						{
							b_playerPos->y = 0;							//移動量のセット
							a_pScreen->y = 0;							//移動量のセット
							a_playerPos->y = CHIP_RECT.Up + a_playerRadius;		//押し戻し処理

							*jumpFlg = false;
						}
						else if ((next.Up > CHIP_RECT.Down) && (next.Down < CHIP_RECT.Down))	//チップ下判定
						{
							b_playerPos->y = 0;	
							a_pScreen->y = 0;
							a_playerPos->y = CHIP_RECT.Down - a_playerRadius;	//押し戻し処理
						}
					}

					if ((object.Up > CHIP_RECT.Down) && (object.Down < CHIP_RECT.Up))
					{
						if ((next.Right > CHIP_RECT.Right) && (next.Left < CHIP_RECT.Right))			//チップ右判定
						{
							b_playerPos->x = 0;							//移動量のセット
							a_pScreen->x = 0;
							a_playerPos->x = CHIP_RECT.Right + a_playerRadius;		//押し戻し処理
						}
						else if ((next.Right > CHIP_RECT.Left) && (next.Left < CHIP_RECT.Left))		//チップ左判定
						{
							b_playerPos->x = 0;							//移動量のセット
							a_pScreen->x = 0;
							a_playerPos->x = CHIP_RECT.Left - a_playerRadius;		//押し戻し処理
						}
					}
                }
				
					if ((mapChipData[h][w] == 3))
					{
						if (ballet[h][w])
						{
						if ((object.Right > CHIP_RECT.Left) && (object.Left < CHIP_RECT.Right))
						{
							if ((next.Up > CHIP_RECT.Up) && (next.Down < CHIP_RECT.Up))				//チップの上判定
							{
								b_playerPos->y = 0;							//移動量のセット
								a_playerPos->y = CHIP_RECT.Up + a_playerRadius;		//押し戻し処理
								a_pScreen->y = 0;
								*jumpFlg = false;
							}
							else if ((next.Up > CHIP_RECT.Down) && (next.Down < CHIP_RECT.Down))	//チップ下判定
							{
								b_playerPos->y = 0;	
								a_pScreen->y = 0;//移動量のセット
								a_playerPos->y = CHIP_RECT.Down - a_playerRadius;	//押し戻し処理
							}
						}

						if ((object.Up > CHIP_RECT.Down) && (object.Down < CHIP_RECT.Up))
						{
							if ((next.Right > CHIP_RECT.Right) && (next.Left < CHIP_RECT.Right))			//チップ右判定
							{
								b_playerPos->x = 0;	
								a_pScreen->x = 0;//移動量のセット
								a_playerPos->x = CHIP_RECT.Right + a_playerRadius;		//押し戻し処理
							}
							else if ((next.Right > CHIP_RECT.Left) && (next.Left < CHIP_RECT.Left))		//チップ左判定
							{
								b_playerPos->x = 0;	
								a_pScreen->x = 0;//移動量のセット
								a_playerPos->x = CHIP_RECT.Left - a_playerRadius;		//押し戻し処理
							}
						}
					}

				}
			
		}
	}



}

void Map::Balletcollision(Math::Vector2* a_playerPos, float a_playerRadius, Math::Vector2* b_playerPos, bool* BalletFlg)
{
	for (int h = 0; h < MAP_CHIP_H; h++)
	{
		for (int w = 0; w < MAP_CHIP_W; w++)
		{
			//矩形のセット
			Rect object = Collision::SetRect(a_playerPos, a_playerRadius);

			//未来座標のセット
			Rect next = Collision::SetFutureRect(object, b_playerPos);
			Rect CHIP_RECT;
			CHIP_RECT.Up = mapChipY[h][w] + CHIP_RADIUS;
			CHIP_RECT.Down = mapChipY[h][w] - CHIP_RADIUS;
			CHIP_RECT.Right = mapChipX[h][w] + CHIP_RADIUS;
			CHIP_RECT.Left = mapChipX[h][w] - CHIP_RADIUS;

			if ((mapChipData[h][w] == 1) ||
				(mapChipData[h][w] == 2))
			{

				if ((object.Right > CHIP_RECT.Left) && (object.Left < CHIP_RECT.Right))
				{
					if ((next.Up > CHIP_RECT.Up) && (next.Down < CHIP_RECT.Up))				//チップの上判定
					{

						*BalletFlg = false;
					}
					else if ((next.Up > CHIP_RECT.Down) && (next.Down < CHIP_RECT.Down))	//チップ下判定
					{
						*BalletFlg = false;
					}
				}

				if ((object.Up > CHIP_RECT.Down) && (object.Down < CHIP_RECT.Up))
				{
					if ((next.Right > CHIP_RECT.Right) && (next.Left < CHIP_RECT.Right))			//チップ右判定
					{
						*BalletFlg = false;
					}
					else if ((next.Right > CHIP_RECT.Left) && (next.Left < CHIP_RECT.Left))		//チップ左判定
					{
						*BalletFlg = false;
					}
				}
			}

			if ((mapChipData[h][w] == 3))
			{
				if (ballet[h][w])
				{
					if ((object.Right > CHIP_RECT.Left) && (object.Left < CHIP_RECT.Right))
					{
						if ((next.Up > CHIP_RECT.Up) && (next.Down < CHIP_RECT.Up))				//チップの上判定
						{

							ballet[h][w] = false;
							*BalletFlg = false;
						}
						else if ((next.Up > CHIP_RECT.Down) && (next.Down < CHIP_RECT.Down))	//チップ下判定
						{
							ballet[h][w] = false;
							*BalletFlg = false;
						}
					}

					if ((object.Up > CHIP_RECT.Down) && (object.Down < CHIP_RECT.Up))
					{
						if ((next.Right > CHIP_RECT.Right) && (next.Left < CHIP_RECT.Right))			//チップ右判定
						{
							ballet[h][w] = false;
							*BalletFlg = false;
						}
						else if ((next.Right > CHIP_RECT.Left) && (next.Left < CHIP_RECT.Left))		//チップ左判定
						{
							ballet[h][w] = false;
							*BalletFlg = false;
						}
					}
				}
			}
		}

      }
	}

void Map::Rerease()
{

	KdSafeDelete(fp_c);
	

}
