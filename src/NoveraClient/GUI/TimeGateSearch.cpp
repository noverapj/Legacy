

#include "StdAfx.h"

#include "../io3DEngine/ioWnd.h"

#include "../io3DEngine/ioXMLDocument.h"
#include "../io3DEngine/ioXMLElement.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/HelpFunc.h"
#include "../io3DEngine/iostringmanager.h"
#include "../io3dengine/safesprintf.h"

#include "../NetworkWrappingFunc.h"

#include "../ioApplication.h"
#include "../ioMyInfo.h"
#include "../ioBattleRoomMgr.h"
#include "../WndID.h"
#include "../Setting.h"
#include "../NetworkWrappingFunc.h"
#include "../TextColorTable.h"
#include "../PlayEntityHelpStruct.h"
#include "../ioTrainingMode.h"
#include "../ioLadderTeamMgr.h"
#include "../EtcHelpFunc.h"

#include "ioSP2GUIManager.h"
#include "GameChatInput.h"
#include "ExtendedButton.h"
#include "TimeGateSearch.h"
#include "PartyMenuWnd.h"
#include <strsafe.h>

int TimeGateSearch::m_iReserveType = 0;
bool TimeGateSearch::m_bSafetyRoom = false;
DWORD TimeGateSearch::m_dwTrainingStartTime = 0;
DWORD TimeGateSearch::m_dwPartyJoinStartTime = 0;
DWORD TimeGateSearch::m_dwPlazaStartTime = 0;
DWORD TimeGateSearch::m_dwPartyJoinTryCount = 0;
TimeGateSearch::TimeGateSearch()
{
	m_pScaleImageBack = NULL;

	m_dwChangeDuration = 200;
	m_fBigScale = FLOAT1;
	m_fSmallScale = 0.7f;

	m_dwChangeTime	= 0;
	m_iCurBigIdx	= 0;
	m_bReverse	= false;
	m_dwAutoSearchTime = 0;
	m_iSearchType = 0;
	m_dwBattleJoinTry = 0;
	m_dwBattleJoinTrySec = 0;

	m_bPenaltyPeso = false;
}

TimeGateSearch::~TimeGateSearch()
{
	ScaleImageList::iterator iter;
	for( iter=m_ScaleImageList.begin() ; iter!=m_ScaleImageList.end() ; ++iter )
	{
		ScaleImage *pImage = (*iter);
		if( !pImage )
			continue;
		SAFEDELETE( pImage->m_pImage );
		SAFEDELETE( pImage );
	}
	m_ScaleImageList.clear();

	SAFEDELETE( m_pScaleImageBack );
}

void TimeGateSearch::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "ScaleImage" )
	{
		if( !pImage )
			return;
		ScaleImage *pScaleImage = new ScaleImage;
		if( !pScaleImage)
			return;
		pScaleImage->m_pImage = pImage;
		pScaleImage->m_dwScaleTime = 0;
		pScaleImage->m_fCurScale   = m_fSmallScale;
		pImage->SetScale( pScaleImage->m_fCurScale );

		m_ScaleImageList.push_back( pScaleImage );
	}
	else if( szType == "ScaleImageBack" )
	{
		SAFEDELETE( m_pScaleImageBack );
		m_pScaleImageBack = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void TimeGateSearch::SendSearch()
{
	if( g_App.IsMouseBusy() ) return;  

	switch( m_iSearchType )
	{
	case ST_PARTY_JOIN:
		{
			if( !g_LadderTeamMgr.IsLadderTeam() )
			{
				SP2Packet kPacket( CTPK_USER_BATTLEROOM_JOIN );
				// Observer << Invite << PenaltyPeso << iPenaltyPeso << .... 순서로
				kPacket << false << false << m_bPenaltyPeso << g_MyInfo.GetCurPenaltyPeso() << -1 << g_BattleRoomMgr.GetQuickStartModeType() 
					<< g_BattleRoomMgr.GetQuickStartMinPlayer( g_BattleRoomMgr.GetQuickStartModeType() ) 
					<< g_BattleRoomMgr.GetQuickStartMaxPlayer( g_BattleRoomMgr.GetQuickStartModeType() )  
					<< g_BattleRoomMgr.IsQuickStartSameUser( g_BattleRoomMgr.GetQuickStartModeType() );
				TCPNetwork::SendToServer( kPacket );

				m_dwPartyJoinTryCount++;
			}
		}
		break;
	}	
}

void TimeGateSearch::SendCancel()
{
	switch( m_iSearchType )
	{
	case ST_PARTY_JOIN:
		{
			if( !g_LadderTeamMgr.IsLadderTeam() )
			{
				SP2Packet kPacket( CTPK_RESERVE_BATTLEROOM_DELETE );
				TCPNetwork::SendToServer( kPacket );
			}
		}
		break;
	}	
}

void TimeGateSearch::DoReserve()
{
	switch( m_iReserveType )
	{
	case ST_PARTY_JOIN:
		SetSearching( STR(1), m_iReserveType, true );
		break;
	}
	m_iReserveType = 0;
}

void TimeGateSearch::iwm_create()
{
	DoReserve();
}

void TimeGateSearch::iwm_show()
{
	m_dwChangeTime = FRAMEGETTIME() + m_dwChangeDuration;
	m_iCurBigIdx   = 0;

	if( !m_ScaleImageList.empty() )
	{
		ScaleImageList::iterator iter;
		for( iter=m_ScaleImageList.begin() ; iter!=m_ScaleImageList.end() ; ++iter )
		{
			ScaleImage *pImage = (*iter);
			if( !pImage )
				continue;
			pImage->m_fCurScale = m_fSmallScale;
			pImage->m_pImage->SetScale( pImage->m_fCurScale );
			pImage->m_dwScaleTime = 0;
		}

		if( COMPARE( m_iCurBigIdx, 0, (int) m_ScaleImageList.size() ) )
			m_ScaleImageList[m_iCurBigIdx]->m_dwScaleTime = FRAMEGETTIME();
	}

	m_bReverse			= false;
	m_dwAutoSearchTime	= FRAMEGETTIME();	

	// 전투방 페널티 받는지 체크
	m_bPenaltyPeso      = g_BattleRoomMgr.IsBattleRoomMovePenaltyPeso();
}

void TimeGateSearch::iwm_hide()
{
	SendCancel();
	m_iSearchType = 0;
}

void TimeGateSearch::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioMovingWnd::OnProcess( fTimePerSec );

	if( m_dwChangeTime < dwCurTime )
	{
		if( !m_bReverse )
		{
			m_iCurBigIdx++;

			if( m_iCurBigIdx >= (int)m_ScaleImageList.size() )
			{
				m_iCurBigIdx = m_ScaleImageList.size() - 2;
				m_bReverse = true;
			}
		}
		else
		{
			m_iCurBigIdx--;
			if( m_iCurBigIdx < 0 )
			{
				m_iCurBigIdx = 1;
				m_bReverse = false;
			}			
		}			

		m_dwChangeTime = dwCurTime + m_dwChangeDuration;
		if( COMPARE( m_iCurBigIdx, 0, (int)m_ScaleImageList.size() ) )
		{
			m_ScaleImageList[m_iCurBigIdx]->m_fCurScale = m_fSmallScale;
			m_ScaleImageList[m_iCurBigIdx]->m_pImage->SetScale( m_ScaleImageList[m_iCurBigIdx]->m_fCurScale );
			m_ScaleImageList[m_iCurBigIdx]->m_dwScaleTime = FRAMEGETTIME();
		}
	}
	ScaleIconImage();
	AutoSearchProcess();
}

void TimeGateSearch::ScaleIconImage()
{
	int iScaleImageCount = m_ScaleImageList.size();
	for( int i=0 ; i<iScaleImageCount ; i++ )
	{
		ScaleImage *pImage = m_ScaleImageList[i];
		if( !pImage )
			continue;

		int iGapTime  = FRAMEGETTIME() - pImage->m_dwScaleTime;
		int iAniTime  = m_dwChangeDuration;
		float fCurScale = m_fSmallScale;

		if( pImage->m_dwScaleTime != 0 )
		{
			if( iGapTime < iAniTime )
			{
				fCurScale += ( (m_fBigScale - m_fSmallScale) * sinf( ( D3DX_PI / 2 ) * ( (float)iGapTime / (float)iAniTime ) ) );				
			}
			else if( iGapTime < iAniTime * 2 )
			{
				fCurScale -= ( (m_fBigScale - m_fSmallScale) * cosf( ( D3DX_PI / 2 ) * ( (float)( iGapTime + iAniTime ) / (float)iAniTime ) ) );
			}
			else 
			{
				pImage->m_dwScaleTime = 0;
			}
		}

		fCurScale = max( m_fSmallScale, min( fCurScale, m_fBigScale ) );
		pImage->m_fCurScale = fCurScale;
		pImage->m_pImage->SetScale( pImage->m_fCurScale );
	}
}

void TimeGateSearch::AutoSearchProcess()
{
	if( m_dwAutoSearchTime == 0 ) return;
	
	if( m_iSearchType == ST_PARTY_JOIN )
	{
		DWORD dwCurSecGap = ( FRAMEGETTIME() - m_dwPartyJoinStartTime ) / 1000;
		if( dwCurSecGap >= m_dwPartyJoinTryCount * m_dwBattleJoinTrySec )
		{
			//
			SendSearch();
		}
		else
		{
			if( dwCurSecGap >= ( ( m_dwBattleJoinTry - 1 ) * m_dwBattleJoinTrySec ) + 2 )      // 2초정도 텀을 준다.
			{
				HideWnd();
				g_GUIMgr.ShowWnd( TIMEGATE_SEARCH_RESULT );
			}
		}		
	}
	else
	{
		DWORD dwGap = FRAMEGETTIME() - m_dwAutoSearchTime;
		if( dwGap > 10000 )
		{
			//
			SendSearch();
			m_dwAutoSearchTime = FRAMEGETTIME();
		}
	}
}

void TimeGateSearch::SetSearching( const ioHashString &szTitle, int eSearchType, bool bReserve, bool bSafetyRoom )
{
	HideWnd();

	DWORD dwCurTime = FRAMEGETTIME();

	m_szCurTitle = szTitle;
	m_iSearchType = eSearchType;

	switch(m_iSearchType)
	{
	case ST_PARTY_JOIN:
		{
			if( !bReserve )
			{
				m_dwPartyJoinStartTime = dwCurTime;
				m_dwPartyJoinTryCount  = 0;
			}
		}
		break;
	case ST_PLAZA:
		{
			if( !bReserve )
				m_dwPlazaStartTime = dwCurTime;
		}
		break;
	}
	
	ShowWnd();
	SendSearch();
}

void TimeGateSearch::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwChangeDuration = xElement.GetIntAttribute_e( "ChangeDuration" );
	m_fBigScale   = xElement.GetFloatAttribute_e( "BigScale" );
	m_fSmallScale = xElement.GetFloatAttribute_e( "SmallScale" );
	m_dwBattleJoinTry = xElement.GetIntAttribute_e( "BattleJoinTry" );
	m_dwBattleJoinTrySec = xElement.GetIntAttribute_e( "BattleJoinTrySec" );
}

bool TimeGateSearch::iwm_esc()
{
	if( HasWndStyle( IWS_EXIT_ESC ) )
	{
		iwm_command( FindChildWnd( ID_EXIT ), IOBN_BTNUP, 0 );
		return true;
	}
	return false;
}

void TimeGateSearch::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	
	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	}
}

void TimeGateSearch::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );	
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );

	DWORD dwCurGap = 0;
	char szBuf[MAX_PATH] = "";
	DWORD dwCurTime = FRAMEGETTIME();
	switch(m_iSearchType)
	{
	case ST_PARTY_JOIN:
		{
			dwCurGap = dwCurTime - m_dwPartyJoinStartTime;
			dwCurGap /= 1000;
			DWORD dwMax = ( m_dwBattleJoinTry - 1 ) * m_dwBattleJoinTrySec;
			StringCbPrintf( szBuf, sizeof(szBuf), "%s %d / %d", m_szCurTitle.c_str(), min( dwCurGap, dwMax ), dwMax );
		}		
		break;
	case ST_PLAZA:
		{
			dwCurGap = dwCurTime - m_dwPlazaStartTime;
			dwCurGap /= 1000;
			SafeSprintf( szBuf, sizeof(szBuf), STR(1), m_szCurTitle.c_str(), dwCurGap );
		}		
		break;
	}
	g_FontMgr.PrintText( iXPos + 151, iYPos + 53, FONT_SIZE_13, szBuf );

	int iRealXPos = 45;
	ScaleImageList::iterator iter;
	for( iter=m_ScaleImageList.begin() ; iter!=m_ScaleImageList.end() ; ++iter )
	{
		ScaleImage *pImage = (*iter);
		if( !pImage )
			continue;
		if( m_pScaleImageBack )
		{
			m_pScaleImageBack->SetScale( pImage->m_fCurScale );
			m_pScaleImageBack->Render( iXPos + iRealXPos, iYPos + 120, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		}
		if( pImage->m_pImage )
		{
			pImage->m_pImage->Render( iXPos + iRealXPos, iYPos + 120, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
		iRealXPos += 71;
	}
}
//////////////////////////////////////////////////////////////////////////
TimeGateSearchResult::TimeGateSearchResult()
{

}

TimeGateSearchResult::~TimeGateSearchResult()
{

}

bool TimeGateSearchResult::iwm_spacebar()
{
	ioWnd *pBtn = FindChildWnd( ID_SEARCH_CORRECTION );
	if( !pBtn ) return false;
	if( !pBtn->IsShow() ) return false;

	iwm_command( pBtn, IOBN_BTNUP, 0 );
	return true;
}

void TimeGateSearchResult::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CLOSE:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;
	case ID_RESEARCH:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();

			TimeGateSearch *pSearchWnd = dynamic_cast<TimeGateSearch*>(g_GUIMgr.FindWnd( TIMEGATE_SEARCH ));
			if( pSearchWnd )
			{
				pSearchWnd->SetSearching( STR(1), TimeGateSearch::ST_PARTY_JOIN );
			}
		}
		break;
	case ID_SEARCH_CORRECTION:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
			BattleQuickStartOption *pSelectWnd = dynamic_cast<BattleQuickStartOption*>(g_GUIMgr.FindWnd( BATTLE_QUICKSTART_OPTION ));
			if( pSelectWnd )
			{
				pSelectWnd->SetQuickStart( this );
				pSelectWnd->ShowWnd();
			}
		}
		break;
	}
}

void TimeGateSearchResult::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 215, iYPos + 144, FONT_SIZE_13, STR(1) );

	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 215, iYPos + 164, FONT_SIZE_13, STR(2) );
	g_FontMgr.PrintText( iXPos + 215, iYPos + 184, FONT_SIZE_13, STR(3) );
}