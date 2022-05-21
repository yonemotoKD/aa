#include "Map.h"
#include"Application/Game/GameSystem/collision.h"
#include "Application/Game/GameObject/Character/Player.h"
void Map::LoadMap(char* name)
{
	FILE* fp;//file�|�C���^���쐬

	if ((fp = fopen(name, "r")) != nullptr)
	{
		std::cout << "file�ǂݎ�萬��" << std::endl;

		char ch;//�i�[���镶��

		int h = 0;//�c�J�E���g
		int w = 0;//���J�E���g

		//�i�[����������EOF����Ȃ��Ƃ��������J��Ԃ�
		//�������̕��������s��������c�̃J�E���g�𓮂����ĉ��̃J�E���g��߂�
		//MapChipData�̔z��ɕ����𐔎��ɒ��������̂��i�[�����̃J�E���g�𓮂���
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
		std::cout << ("file�ǂݎ�莸�s") << std::endl;
	}
}
void Map::Init()
{
	m_Map.Load("Asset/Texture/Map/grand.png");
	m_gimmick.Load("Asset/Texture/Map/gimmick.png");
	//�X�e�[�W�̃��[�h
	LoadMap("Asset/Texture/Map/Stage.txt");

	for (int h = 0; h < MAP_CHIP_H; h++)
	{
		for (int w = 0; w < MAP_CHIP_W; w++)
		{
			std::cout << mapChipData[h][w];
		}
		std::cout << std::endl;
	}

	//�}�b�v�̏�����
	for (int h = 0; h < MAP_CHIP_H; h++)
	{
		for (int w = 0; w < MAP_CHIP_W; w++)
		{
			mapChipX[h][w] = -640 + (48.0f * w);//��ʂ̍��ォ��
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
	//map�̕\��
	
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
	//��`�̃Z�b�g
	Rect object = Collision::SetRect(a_playerPos, a_playerRadius);

	//�������W�̃Z�b�g
	Rect next = Collision::SetFutureRect(object, b_playerPos);

	for (int h = 0; h < MAP_CHIP_H; h++)
	{
		for (int w = 0; w < MAP_CHIP_W; w++)
		{
			//�f�[�^�Ȃ��A���ԃ|�C���g�A�S�[���|�C���g������(0, 1, 2)
			if ((mapChipData[h][w] == 0)||
				(mapChipData[h][w] == 4)||
				(mapChipData[h][w] == 5)
				)
			{
			continue;
			}
			//�}�b�v�`�b�v�̋�`�Z�b�g
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
						if ((next.Up > CHIP_RECT.Up) && (next.Down < CHIP_RECT.Up))				//�`�b�v�̏㔻��
						{
							b_playerPos->y = 0;							//�ړ��ʂ̃Z�b�g
							a_pScreen->y = 0;							//�ړ��ʂ̃Z�b�g
							a_playerPos->y = CHIP_RECT.Up + a_playerRadius;		//�����߂�����

							*jumpFlg = false;
						}
						else if ((next.Up > CHIP_RECT.Down) && (next.Down < CHIP_RECT.Down))	//�`�b�v������
						{
							b_playerPos->y = 0;	
							a_pScreen->y = 0;
							a_playerPos->y = CHIP_RECT.Down - a_playerRadius;	//�����߂�����
						}
					}

					if ((object.Up > CHIP_RECT.Down) && (object.Down < CHIP_RECT.Up))
					{
						if ((next.Right > CHIP_RECT.Right) && (next.Left < CHIP_RECT.Right))			//�`�b�v�E����
						{
							b_playerPos->x = 0;							//�ړ��ʂ̃Z�b�g
							a_pScreen->x = 0;
							a_playerPos->x = CHIP_RECT.Right + a_playerRadius;		//�����߂�����
						}
						else if ((next.Right > CHIP_RECT.Left) && (next.Left < CHIP_RECT.Left))		//�`�b�v������
						{
							b_playerPos->x = 0;							//�ړ��ʂ̃Z�b�g
							a_pScreen->x = 0;
							a_playerPos->x = CHIP_RECT.Left - a_playerRadius;		//�����߂�����
						}
					}
                }
				
					if ((mapChipData[h][w] == 3))
					{
						if (ballet[h][w])
						{
						if ((object.Right > CHIP_RECT.Left) && (object.Left < CHIP_RECT.Right))
						{
							if ((next.Up > CHIP_RECT.Up) && (next.Down < CHIP_RECT.Up))				//�`�b�v�̏㔻��
							{
								b_playerPos->y = 0;							//�ړ��ʂ̃Z�b�g
								a_playerPos->y = CHIP_RECT.Up + a_playerRadius;		//�����߂�����
								a_pScreen->y = 0;
								*jumpFlg = false;
							}
							else if ((next.Up > CHIP_RECT.Down) && (next.Down < CHIP_RECT.Down))	//�`�b�v������
							{
								b_playerPos->y = 0;	
								a_pScreen->y = 0;//�ړ��ʂ̃Z�b�g
								a_playerPos->y = CHIP_RECT.Down - a_playerRadius;	//�����߂�����
							}
						}

						if ((object.Up > CHIP_RECT.Down) && (object.Down < CHIP_RECT.Up))
						{
							if ((next.Right > CHIP_RECT.Right) && (next.Left < CHIP_RECT.Right))			//�`�b�v�E����
							{
								b_playerPos->x = 0;	
								a_pScreen->x = 0;//�ړ��ʂ̃Z�b�g
								a_playerPos->x = CHIP_RECT.Right + a_playerRadius;		//�����߂�����
							}
							else if ((next.Right > CHIP_RECT.Left) && (next.Left < CHIP_RECT.Left))		//�`�b�v������
							{
								b_playerPos->x = 0;	
								a_pScreen->x = 0;//�ړ��ʂ̃Z�b�g
								a_playerPos->x = CHIP_RECT.Left - a_playerRadius;		//�����߂�����
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
			//��`�̃Z�b�g
			Rect object = Collision::SetRect(a_playerPos, a_playerRadius);

			//�������W�̃Z�b�g
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
					if ((next.Up > CHIP_RECT.Up) && (next.Down < CHIP_RECT.Up))				//�`�b�v�̏㔻��
					{

						*BalletFlg = false;
					}
					else if ((next.Up > CHIP_RECT.Down) && (next.Down < CHIP_RECT.Down))	//�`�b�v������
					{
						*BalletFlg = false;
					}
				}

				if ((object.Up > CHIP_RECT.Down) && (object.Down < CHIP_RECT.Up))
				{
					if ((next.Right > CHIP_RECT.Right) && (next.Left < CHIP_RECT.Right))			//�`�b�v�E����
					{
						*BalletFlg = false;
					}
					else if ((next.Right > CHIP_RECT.Left) && (next.Left < CHIP_RECT.Left))		//�`�b�v������
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
						if ((next.Up > CHIP_RECT.Up) && (next.Down < CHIP_RECT.Up))				//�`�b�v�̏㔻��
						{

							ballet[h][w] = false;
							*BalletFlg = false;
						}
						else if ((next.Up > CHIP_RECT.Down) && (next.Down < CHIP_RECT.Down))	//�`�b�v������
						{
							ballet[h][w] = false;
							*BalletFlg = false;
						}
					}

					if ((object.Up > CHIP_RECT.Down) && (object.Down < CHIP_RECT.Up))
					{
						if ((next.Right > CHIP_RECT.Right) && (next.Left < CHIP_RECT.Right))			//�`�b�v�E����
						{
							ballet[h][w] = false;
							*BalletFlg = false;
						}
						else if ((next.Right > CHIP_RECT.Left) && (next.Left < CHIP_RECT.Left))		//�`�b�v������
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
