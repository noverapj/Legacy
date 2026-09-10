#include "StdAfx.h"

#include "../ioMyInfo.h"
#include "../ioApplication.h"

#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioINILoader.h"

#include "UserListWnd.h"
#include ".\ioblacklistmanager.h"

#include <strsafe.h>

template<> ioBlackListManager* Singleton< ioBlackListManager >::ms_Singleton = 0;

ioBlackListManager::ioBlackListManager(void)
{
	m_vBlackList.clear();
	m_bSave = false;
	m_vBlackList.reserve(MAX_BLACK_LIST);
}

ioBlackListManager::~ioBlackListManager(void)
{
	m_vBlackList.clear();
}

void ioBlackListManager::LoadBlackList()
{
	if(g_MyInfo.GetPublicID().IsEmpty()) return;

	char fName[MAX_PATH]="";
	StringCbPrintf( fName, sizeof(fName), "Save\\%s\\BlackList.ini",g_MyInfo.GetPublicID().c_str() );

	ioINILoader kLoader( fName, false );
	kLoader.SetTitle_e( "BlackList" );
	const int iMax = kLoader.LoadInt("Max", 0);

	for (int i = 0; i < iMax ; i++)
	{
		if(i >= MAX_BLACK_LIST) break;

		char szKeyName[MAX_PATH]="";
		StringCbPrintf_e(szKeyName, sizeof(szKeyName), "%d", i+1);
		char szBuf[MAX_PATH]="";
		kLoader.LoadString(szKeyName, "", szBuf, sizeof(szBuf));
		if(strcmp(szBuf, "") == 0) continue;

		m_vBlackList.push_back(szBuf);
	}
	UserListWnd::sg_bUserListUpdate =  true;
}

void ioBlackListManager::SaveBlackList()
{
	if(!m_bSave) return;
	if(g_MyInfo.GetPublicID().IsEmpty()) return;

	const int iMax = m_vBlackList.size();

	char fName[MAX_PATH]="";
	StringCbPrintf( fName, sizeof(fName), "Save\\%s\\BlackList.ini",g_MyInfo.GetPublicID().c_str() );

	ioINILoader kLoader( fName, false );
	kLoader.SetTitle_e( "BlackList" );
	kLoader.SaveInt("Max", iMax);

	for (int i = 0; i < iMax ; i++)
	{
		char szKeyName[MAX_PATH]="";
		StringCbPrintf(szKeyName, sizeof(szKeyName), "%d", i+1);
		kLoader.SaveString(szKeyName, m_vBlackList[i].c_str());
	}
}

bool ioBlackListManager::InsertBlackList( const ioHashString &szID )
{
	if( !g_App.IsRightID(szID.c_str()) ) return false;
	if( IsBlackList( szID ) ) return false ;
	if( m_vBlackList.size() >= MAX_BLACK_LIST ) 
		return false;

	m_vBlackList.push_back(szID);
	m_bSave = true;
	UserListWnd::sg_bUserListUpdate =  true;

	return true;
}

void ioBlackListManager::RemoveBlackList( const ioHashString &szID )
{
	if( !g_App.IsRightID(szID.c_str()) ) return;

	ioHashStringVec::iterator iter = std::find( m_vBlackList.begin(), m_vBlackList.end(), szID );
	if( iter != m_vBlackList.end() )
	{
		m_vBlackList.erase( iter );
		m_bSave = true;
	}

	UserListWnd::sg_bUserListUpdate =  true;
}

bool ioBlackListManager::IsBlackList( const ioHashString &szID )
{
	ioHashStringVec::iterator iter = std::find( m_vBlackList.begin(), m_vBlackList.end(), szID );
	if( iter != m_vBlackList.end() )
		return true;
	else 
		return false;
}

ioHashString *ioBlackListManager::GetNode( int iArray )
{
	int iSize = m_vBlackList.size();
	if( !COMPARE( iArray, 0, iSize ) )
		return NULL;

	return &m_vBlackList[iArray];
}

ioBlackListManager& ioBlackListManager::GetSingleton()
{
	return Singleton< ioBlackListManager >::GetSingleton();
}