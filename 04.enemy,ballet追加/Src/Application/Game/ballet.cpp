#include "ballet.h"

Ballet::Ballet()
{

}

Ballet::~Ballet()
{

}

void Ballet::Init()
{
	m_Ballet = new KdSquarePolygon();
	m_Ballet->SetMaterial("Asset/Texture/ballet/ballet.png");
	m_Ballet->SetSplit(4, 4);
	m_Ballet->SetScale({ 0.5f,0.5f });

	m_moveDir = Math::Vector3::Zero;
	m_mWorld.Translation(Math::Vector3(3.0f, 0.0f, 5.0f));

}

void Ballet::Update()
{
	m_mWorld *=
		Math::Matrix::CreateTranslation(m_moveDir * movespeed);

}

void Ballet::Draw()
{
	D3D.WorkShaderManager().m_HD2DShader.DrawSquarePolygon(*m_Ballet, m_mWorld);
}

void Ballet::Release()
{
	KdSafeDelete(m_Ballet);
}
