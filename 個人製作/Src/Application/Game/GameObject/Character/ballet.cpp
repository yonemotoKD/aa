#include "ballet.h"
#include "Application/Game/GameObject/Character/Player.h"
#include"Application/Game/GameObject/Embironment/Map.h"
void Ballet::Init()
{
	m_Ballet.Load("Asset/Texture/Character/Ballet/asid-ballet.png");
	
	ballet_type = 0;

	for (int i = 0;i < BulletNum;i++)
	{
		m_pos[i] = Math::Vector2(0, 0);
		m_bpos[i] = Math::Vector2(0, 0);
		m_move[i] = Math::Vector2(0, 0);
		balletAlive[i] = false;
	}
	
	
	m_screenX = 0;
}

void Ballet::Update()
{
	for (int i = 0; i < BulletNum; i++)
	{
		if (!balletAlive[i])
		{
			m_pos[i] = m_bpos[i];
			m_move[i].x = 0.0f;

			if (GetAsyncKeyState('Q'))
			{
				m_size[i] = m_play->GetSize();
				m_pos[i] = m_play->GetPos();
				balletAlive[i] = true;

			}

		}
		if (balletAlive[i])
		{
				m_move[i].x = 7.0f * m_size[i];
			m_map->Balletcollision(&m_pos[i], m_radius, &m_move[i], &balletAlive[i]);
		}
		m_moveScreen = m_play->GetScreenX();
		
		m_pos[i].x += m_move[i].x;
		TransMat[i] = DirectX::XMMatrixTranslation(m_pos[i].x - m_moveScreen.x, m_pos[i].y - m_moveScreen.y, 0);
		ScaleMat[i] = DirectX::XMMatrixTranslation(m_size[i], 0, 0);
		m_mWorld[i] = ScaleMat[i] * TransMat[i];
	}
}

void Ballet::Draw()
{
	
	for (int i = 0; i < BulletNum; i++)
	{
		D3D.WorkShaderManager().m_spriteShader.SetMatrix(m_mWorld[i]);

		Math::Rectangle srcRect = { 0 , 0 , 64 , 64 };

		Math::Color color = { 1.0f ,1.0f , 1.0f , 1.0f };
		if (balletAlive[i])
		{
			D3D.WorkShaderManager().m_spriteShader.DrawTex(&m_Ballet, 0, 0, 48, 48, &srcRect, &color, Math::Vector2(0.5f, 0.5f));
		}
	}
}

void Ballet::SetPos(Math::Vector2 a_pos)
{
	for (int i = 0; i < BulletNum; i++)
	{
		if (balletAlive[i])
		{
			m_bpos[i] = a_pos;
		}
	}
}
void Ballet::SetPlay(Player* a_pOwner) {
	if (a_pOwner == nullptr)return;	//nullチェック
	m_play = a_pOwner;
}
void Ballet::SetMap(Map* a_pOwner)
{
	if (a_pOwner == nullptr)return;	//nullチェック
	m_map = a_pOwner;
}
void Ballet::Release()
{
	KdSafeDelete(m_play);
	KdSafeDelete(m_map);
	
}
