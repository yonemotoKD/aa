#include"GameMaster.h"
#include"GameObject/Character/Player.h"
#include"Application/Game/GameObject/Embironment/Map.h"
#include"Application/Game/GameObject/Character/ballet.h"
GameSystem::GameSystem()
{

}

void GameSystem::Init()
{
	m_pmap = new Map();
    m_pPlay = new Player();
    m_pballet = new Ballet();
	m_pPlay->Init();
	m_pmap->Init();
	m_pballet->Init();
}

void GameSystem::Update()
{
	if (m_pPlay)
	{
		m_pPlay->SetMap(m_pmap);
		m_pPlay->Update();
	}
	
	if (m_pmap)
	{
		m_pmap->SetPlay(m_pPlay);
		m_pmap->Update();
		
	}

	if (m_pballet)
	{
		m_pballet->SetPlay(m_pPlay);
		m_pballet->SetMap(m_pmap);
		m_pballet->Update();

	}
}

void GameSystem::Draw()
{
	if (m_pmap)
	{
		m_pmap->Draw();
	}
	if (m_pballet)
	{
		m_pballet->Draw();
	}
	if (m_pPlay)
	{
		m_pPlay->Draw();
	}
}

void GameSystem::Release()
{
	KdSafeDelete(m_pPlay);
	KdSafeDelete(m_pmap);
	KdSafeDelete(m_pballet);
}
