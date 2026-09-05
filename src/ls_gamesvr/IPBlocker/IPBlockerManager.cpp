#include "StdAfx.h"
#include "IPBlockerManager.h"

IPBlockerManager::IPBlockerManager()
{
	Init();
}

IPBlockerManager::~IPBlockerManager()
{
}

void IPBlockerManager::Init()
{
	SetWhiteList(FALSE);
}

void IPBlockerManager::Load()
{
	m_vBlackList.Load("config/sp2_blacklist.ini");
	m_vWhiteList.Load("config/sp2_whitelist.ini");
}

void IPBlockerManager::Destroy()
{
}

BOOL IPBlockerManager::CheckBlackList(char *IP)
{
	if( !IP )
		return TRUE;

	if( m_vBlackList.IsActive() )
	{
		return m_vBlackList.Find(IP) ? TRUE : FALSE;
	}

	return FALSE;
}

BOOL IPBlockerManager::CheckWhiteList(char *IP)
{
	if( !IP )
		return FALSE;

	if( m_vWhiteList.IsActive() && IsWhiteListOn() )
	{
		return m_vWhiteList.Find(IP) ? TRUE : FALSE;
	}

	return TRUE;
}