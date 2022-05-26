#include "player.h"

Player::Player()
{
}

Player::~Player()
{
}

void Player::Init()
{
	m_Play = new KdSquarePolygon();
	m_Play->SetMaterial("Asset/Texture/Enviroment/player.png");
	m_pScopeTex = new KdTexture();
	m_pScopeTex->Load();
	m_Play->SetSplit(9, 4);
	m_Play->SetScale({ 0.5f,0.5f });

	m_mWorld.Translation(Math::Vector3(0.0f, 0.0f, 0.0f));
}

void Player::Update()
{

	m_Play->SetUVRect(0);
	//m_Play->SetUVRect(++animFlame % 60);
	Math::Vector3 moveDir = { 0.0f,0.0f,0.0f };
	
	if (GetAsyncKeyState('D')) { moveDir.x = 1.0f; }
	if(GetAsyncKeyState('A')){ moveDir.x = -1.0f; }
	if(GetAsyncKeyState('W')){ moveDir.y = 1.0f; }
	if(GetAsyncKeyState('S')){ moveDir.y = -1.0f; }
	
	moveDir.Normalize();

	if (moveDir.LengthSquared() > 0) 
	{ 
		m_moveDir = moveDir;
		movespeed = 0.03f;
	}
	else
	{
		movespeed *= 0.95f;
	}
	

	POINT nowPos;
	GetCursorPos(&nowPos);
	KdCamera* camera = Application::Instance().GetCamera();

	Math::Vector2 curPos =
		Math::Vector2(nowPos.x - (1280 / 2), -(nowPos.y - (720 / 2)));
	Math::Vector3 resultPos;
	camera->ConvertScreenToWorldDetail(curPos, resultPos);

	Math::Vector3 resultRayDir = resultPos -
		camera->GetCameraMatrix().Translation();
	resultRayDir.Normalize();

	Math::Vector3 charPos = camera->GetCameraMatrix().Translation() +
		(resultRayDir * 5.0f);
	m_mWorld = Math::Matrix::CreateTranslation(charPos);
}

void Player::Draw()
{
	D3D.WorkShaderManager().m_HD2DShader.DrawSquarePolygon(*m_Play,m_mWorld);
	D3D.WorkShaderManager().m_spriteShader.DrawTex(m_pScopeTex,0,0,);
	//D3D.WorkShaderManager().m_spriteShader.DrawCircle(0,0,10,&kBlueColor);
}

void Player::Release()
{
	KdSafeDelete(m_Play);
	KdSafeDelete(m_pScopeTex);
}
