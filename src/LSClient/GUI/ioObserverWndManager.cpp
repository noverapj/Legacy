
#include "StdAfx.h"

#include "../WndID.h"
#include "../ioMyInfo.h"
#include "../ioPlayStage.h"
#include "../ioBattleRoomMgr.h"

#include "ioSP2GUIManager.h"
#include "ioObserverWndManager.h"
#include "ioObserverWnd.h"

#include <strsafe.h>

template<> ioObserverWndManager* Singleton< ioObserverWndManager >::ms_Singleton = 0;

ioObserverWndManager::ioObserverWndManager( ioPlayStage *pStage )
{
	m_pPlayStage = pStage;

	m_RoundInfoList.clear();
}

ioObserverWndManager::~ioObserverWndManager()
{
	m_RoundInfoList.clear();
}

ioObserverWndManager& ioObserverWndManager::GetSingleton()
{
	return Singleton< ioObserverWndManager >::GetSingleton();
}

void ioObserverWndManager::ClearInfo()
{
	m_RoundInfoList.erase( m_RoundInfoList.begin(), m_RoundInfoList.end() );

	ioObserverBlueWnd *pBlueWnd = dynamic_cast<ioObserverBlueWnd *>( g_GUIMgr.FindWnd( OBSERVER_BLUE_INFO_WND ) );
	if( pBlueWnd )
		pBlueWnd->ClearInfo();

	ioObserverRedWnd *pRedWnd = dynamic_cast<ioObserverRedWnd *>( g_GUIMgr.FindWnd( OBSERVER_RED_INFO_WND ) );
	if( pRedWnd )
		pRedWnd->ClearInfo();
}

void ioObserverWndManager::SetRoundInfo( RoundInfo &rkInfo )
{
	if( !m_pPlayStage )
		return;

	if( rkInfo.m_szName.IsEmpty() ) 
		return;

	if( !g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) && !m_pPlayStage->IsStealthMode() )
		return;

	m_RoundInfoList.push_back( rkInfo );
}

void ioObserverWndManager::UpdateInfo( ModeType eModeType )
{
	if( m_RoundInfoList.empty() )
		return;

	ioObserverBlueWnd *pBlueWnd = dynamic_cast<ioObserverBlueWnd *>( g_GUIMgr.FindWnd( OBSERVER_BLUE_INFO_WND ) );
	ioObserverRedWnd *pRedWnd = dynamic_cast<ioObserverRedWnd *>( g_GUIMgr.FindWnd( OBSERVER_RED_INFO_WND ) );

	if( !pBlueWnd || !pRedWnd )
		return;

	int i = 0;
	int iSize = m_RoundInfoList.size();

	if( eModeType == MT_BOSS )
	{
		// 보스 표시
		for(i = 0;i < iSize;++i)
		{
			RoundInfo &rkInfo = m_RoundInfoList[i];

			if( rkInfo.m_Team == TEAM_BLUE )
			{
				pBlueWnd->SetInfo( rkInfo );
				break;
			}
		}

		// 일반 표시
		int iRedCount = 1;
		for(i = 0;i < iSize;++i)
		{
			RoundInfo &rkInfo = m_RoundInfoList[i];

			if( rkInfo.m_Team == TEAM_RED )
			{
				if( iRedCount < ioObserverBlueWnd::MAX_TEAM_PLAYER )
					pBlueWnd->SetInfo( rkInfo );
				else
					pRedWnd->SetInfo( rkInfo );
				iRedCount++;
			}
		}
	}
	else if( eModeType == MT_MONSTER_SURVIVAL || eModeType == MT_DUNGEON_A || eModeType == MT_RAID || Help::IsMonsterDungeonMode(eModeType))
	{
		for(i = 0;i < iSize;++i)
		{
			RoundInfo &rkInfo = m_RoundInfoList[i];
			if( i < ioObserverBlueWnd::MAX_TEAM_PLAYER )
				pBlueWnd->SetInfo( rkInfo );
			else
				pRedWnd->SetInfo( rkInfo );
		}
	}
	else if( eModeType == MT_GANGSI )
	{
		for(i = 0;i < iSize;++i)
		{
			RoundInfo &rkInfo = m_RoundInfoList[i];
			if( i < ioObserverBlueWnd::MAX_TEAM_PLAYER )
				pBlueWnd->SetInfo( rkInfo );
			else
				pRedWnd->SetInfo( rkInfo );
		}
	}
	else // 기타 모드
	{
		for(int i = 0;i < iSize;++i)
		{
			RoundInfo &rkInfo = m_RoundInfoList[i];

			if( rkInfo.m_Team == TEAM_BLUE )
				pBlueWnd->SetInfo( rkInfo );
			else if( rkInfo.m_Team == TEAM_RED )
				pRedWnd->SetInfo( rkInfo );
			else if( i < ioObserverBlueWnd::MAX_TEAM_PLAYER )
				pBlueWnd->SetInfo( rkInfo );
			else
				pRedWnd->SetInfo( rkInfo );

		}
	}
}

void ioObserverWndManager::ObserverWndShow()
{
	if( !m_pPlayStage ) return;
	if( !g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) && !m_pPlayStage->IsStealthMode() )
		return;

	g_GUIMgr.ShowWnd( OBSERVER_BLUE_INFO_WND );
	g_GUIMgr.ShowWnd( OBSERVER_RED_INFO_WND );
	g_GUIMgr.ShowWnd( OBSERVER_INFO_WND );
}

void ioObserverWndManager::ObserverWndHide()
{
	g_GUIMgr.HideWnd( OBSERVER_BLUE_INFO_WND );
	g_GUIMgr.HideWnd( OBSERVER_RED_INFO_WND );
	g_GUIMgr.HideWnd( OBSERVER_INFO_WND );
}

void ioObserverWndManager::ObserverInfoShow( bool bSpecial )
{
	if( !m_pPlayStage ) return;
	if( !g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) && !m_pPlayStage->IsStealthMode() )
		return;

	ioObserverInfoWnd *pWnd = static_cast<ioObserverInfoWnd*>( g_GUIMgr.FindWnd( OBSERVER_INFO_WND ) );
	if( pWnd )
		pWnd->SetShow( bSpecial );
}