#include "Player.h"
#include"Application/Game/GameSystem/collision.h"
#include"Application/Game/GameObject/Embironment/Map.h"
void Player::Init()
{
	type = SlimeType::Normal;

	m_Play.Load("Asset/Texture/Character/Player/Normal-Slime.png");
	m_AsidPlay.Load("Asset/Texture/Character/Player/Asid-Slime.png");
	

	m_pos = Math::Vector2(0, 0);
	m_move = Math::Vector2(0, 0);
	m_Screen = Math::Vector2(0, 0);
	m_Screenspead= Math::Vector2(0, 0);
	time = 0;
	jumpFlg = false;
	asid = false;
	TypeSwap = false;
}

void Player::Update()
{
	m_move.x = 0;
	m_Screenspead.x = 0;
	m_move.y -= gravity;
	//m_Screenspead.x = 3;if (m_pos.y >= 0)

	
		m_Screenspead.y -= gravity;
	
	if (type == SlimeType::Normal)
	{
		
		
	}
	if (type == SlimeType::Asid)
	{
		
		
	}

	
	//m_move.y = 0;
	if (GetAsyncKeyState('D')) 
	{ m_move.x = 5.0f; 
	m_size = 1;
	if (m_pos.x >= 0)
	{
		m_Screenspead.x = 5.0f;
		
	}
	}
	if (GetAsyncKeyState('A')) 
	{ m_move.x = -5.0f; 
	m_size = -1;
	if (m_Screen.x >= 0)
	{
		m_Screenspead.x = -5.0f;
		
	}
	
	}
	if (!jumpFlg)
	{
	
		if (GetAsyncKeyState('W'))
		{
			m_move.y = m_jump;
			
				m_Screenspead.y = m_jump;
			
			jumpFlg = true;
		}
	}
	if (GetAsyncKeyState('S'))
	{
		if (!TypeSwap)
		{
			if (type == SlimeType::Normal )//&& asid)
			{
				type = SlimeType::Asid;
				TypeSwap = true;
				time = 200;
			}
			else if (type == SlimeType::Asid)
			{
				type = SlimeType::Normal;
				TypeSwap = true;
				time = 200;
			}
		}
		
	}

	if (time <= 0)
	{
		TypeSwap = false;
		time = 0;
	}
	
	m_pmap->collision(&m_pos, m_radius, &m_Screenspead, &m_move, &jumpFlg);
	m_pos += m_move;
	m_Screen += m_Screenspead;
	Math::Matrix ScaleMat, TransMat, RotateMat;
	//m_mWorld = DirectX::XMMatrixTranslation(m_pos.x , m_pos.y , 0);
	TransMat = DirectX::XMMatrixTranslation(m_pos.x - m_Screen.x, m_pos.y - m_Screen.y, 0);
	ScaleMat = DirectX::XMMatrixScaling(m_size, 1, 1);
	m_mWorld = ScaleMat * TransMat;
	
	time--;
}

void Player::Draw()
{

	
	D3D.WorkShaderManager().m_spriteShader.SetMatrix(m_mWorld);

	Math::Rectangle srcRect = { 0 , 0 , 64 , 64 };

	Math::Color color = { 1.0f ,1.0f , 1.0f , 1.0f };

	if (type == SlimeType::Normal)
	{
		D3D.WorkShaderManager().m_spriteShader.DrawTex(&m_Play, 0, 0, 48, 48, &srcRect, &color, Math::Vector2(0.5f, 0.5f));
	}
	else if (type == SlimeType::Asid)
	{
		D3D.WorkShaderManager().m_spriteShader.DrawTex(&m_AsidPlay, 0, 0, 48, 48, &srcRect, &color, Math::Vector2(0.5f, 0.5f));
	}
}

void Player::SetPos(Math::Vector2 a_pos)
{
	m_pos = a_pos;
}

void Player::SetMap(Map* a_pOwner)
{
	if (a_pOwner == nullptr)return;	//nullチェック
	m_pmap = a_pOwner;
}

const Math::Vector2 Player::GetScreenX()
{
	return m_Screen;
}

void Player::Release()
{
	KdSafeDelete(m_pmap);
	m_Play.Release();
	m_AsidPlay.Release();
	m_FirePlay.Release();
}


