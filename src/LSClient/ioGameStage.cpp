

#include "stdafx.h"
#include "ioGameStage.h"

ioGameStage::ioGameStage( ioGameSceneManager *pSceneMgr )
{
	m_pSceneMgr = pSceneMgr;
}

ioGameStage::~ioGameStage()
{
}

ioGameStage::GameStage ioGameStage::GetTypeID() const
{
	return GS_NONE;
}

const char* ioGameStage::GetTypeText() const
{
	return "GS_NONE";
}