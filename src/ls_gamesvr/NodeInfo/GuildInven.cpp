#include "stdafx.h"
#include "GuildInven.h"

GuildInven::GuildInven()
{
	Init();
}

GuildInven::~GuildInven()
{
	Destroy();
}

void GuildInven::Init()
{
	m_mGuildInvenInfos.clear();
}

void GuildInven::Destroy()
{
}

void GuildInven::DBToData()
{
	//길드 인벤토리 정보. 겟


}

BOOL GuildInven::IsExistItem(const DWORD dwItemIndex)
{
	ITEMINFOS::iterator it = m_mGuildInvenInfos.find(dwItemIndex);

	if( it == m_mGuildInvenInfos.end() )
		return FALSE;

	return TRUE;
}

BOOL GuildInven::AddItem(const DWORD dwItemIndex, const DWORD dwItemCode)
{
	if( IsExistItem(dwItemIndex) )
		return FALSE;

	m_mGuildInvenInfos.insert( std::make_pair(dwItemIndex, dwItemCode) );
	return TRUE;
}

BOOL GuildInven::DeleteItem(const DWORD dwItemIndex)
{
	if( !IsExistItem(dwItemIndex) )
		return FALSE;

	m_mGuildInvenInfos.erase(dwItemIndex);
	return TRUE;
}