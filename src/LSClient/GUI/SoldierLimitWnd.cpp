#include "StdAfx.h"

#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioUIRenderer.h"
#include "../io3DEngine/ioMath.h"
#include "../io3DEngine/ioUIImageSetManager.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioOrientBox.h"
#include "../io3DEngine/HelpFunc.h"
#include "../io3DEngine/iostringmanager.h"
#include "../io3dengine/safesprintf.h"

#include "../TextColorTable.h"
#include "../ioItemMaker.h"
#include "../WndID.h"
#include "../ioMyInfo.h"
#include "../ioCreateChar.h"
#include "../ioSetItemInfo.h"
#include "../ioSetItemInfoManager.h"
#include "../NetworkWrappingFunc.h"
#include "../ioClassPrice.h"
#include "../ioItemMaker.h"
#include "../ioBattleRoomMgr.h"
#include "../ioPlayStage.h"
#include "../ChatMessageFunc.h"
#include "../Setting.h"
#include "../ioMyLevelMgr.h"
#include "../ioLadderTeamMgr.h"
#include "../EtcHelpFunc.h"
#include "../ioPlayMode.h"
#include "../ioBaseChar.h"

#include "SoldierSelectWnd.h"
#include "MySoldierWnd.h"
#include "ioSP2GUIManager.h"
#include "SoldierLimitWnd.h"
#include <strsafe.h>

SoldierLimitPopupWnd::SoldierLimitPopupWnd() : m_pPlayStage( NULL )
{
	m_pIconBack = NULL;
	m_pIcon		= NULL;
}

SoldierLimitPopupWnd::~SoldierLimitPopupWnd()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pIcon );
}

void SoldierLimitPopupWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "Icon" )
	{
		SAFEDELETE( m_pIcon );
		m_pIcon = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void SoldierLimitPopupWnd::iwm_show()
{
	if( g_GUIMgr.IsShow( BANKRUPTCY_CHAR_WND ) )
		g_GUIMgr.HideWnd( BANKRUPTCY_CHAR_WND );

	g_App.ResetConnectedMoveMode();
}

void SoldierLimitPopupWnd::iwm_hide()
{
	char szTitle[MAX_PATH] = "";
	SafeSprintf( szTitle, sizeof( szTitle ), "사용 가능한 용병 없음" );
	SetTitleText( szTitle );
}

void SoldierLimitPopupWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_SHOP:
		if( cmd == IOBN_BTNUP )
		{	
			HideWnd();
			g_GUIMgr.ShowWnd( NEW_SHOP_WND );
		}
		break;
	}
}

void SoldierLimitPopupWnd::OnProcess( float fTimePerSec )
{
	if( g_App.IsSingleMode() )
		return;

	if( !IsShow() && g_MyInfo.IsCharBankruptcy() )
	{
		if( g_GUIMgr.IsShow( NEW_SHOP_WND ) || g_GUIMgr.IsShow( MY_INVENTORY_WND ) )
			return;
		else
		{
			ShowWnd();
		}
	}
}

void SoldierLimitPopupWnd::OnRender()
{
	ioWnd::OnRender();
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pIconBack )
		m_pIconBack->Render( iXPos + 151, iYPos + 98, UI_RENDER_MULTIPLY, TFO_BILINEAR );
	if( m_pIcon )
		m_pIcon->Render( iXPos + 151, iYPos + 98 );

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 144, FONT_SIZE_13, STR(5) );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );	
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, "사용하고 싶은 용병을" );
	kPrinter.PrintFullText( iXPos + 151, iYPos + 164, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, "상점에서 고용해 주세요." );
	kPrinter.PrintFullText( iXPos + 151, iYPos + 184, TAT_CENTER );
	kPrinter.ClearList();
}
//////////////////////////////////////////////////////////////////////////
RentalSoldierLimitPopupWnd::RentalSoldierLimitPopupWnd() : m_pPlayStage( NULL )
{
	m_pIconBack = NULL;
	m_pCharIcon = NULL;

	m_dwProcessCheckTime = 0;
	m_iCharType = 0;
	m_dwRoomOutTime = 0;
	m_nGradeType = 0;
}

RentalSoldierLimitPopupWnd::~RentalSoldierLimitPopupWnd()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pCharIcon );
}

void RentalSoldierLimitPopupWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void RentalSoldierLimitPopupWnd::iwm_show()
{
	g_App.ResetConnectedMoveMode();
}

void RentalSoldierLimitPopupWnd::iwm_hide()
{

}

void RentalSoldierLimitPopupWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_SOLDIER_SELECT:
		if( cmd == IOBN_BTNUP )
		{	
			if( m_pPlayStage )
			{
				ioBaseChar *pOwnerChar = m_pPlayStage->GetOwnerChar();
				if( pOwnerChar == NULL ) return;
				if( pOwnerChar->IsCanChangeCharState( -1, true ) )
				{
					SoldierSelectWnd *pSoldier = dynamic_cast<SoldierSelectWnd*>(g_GUIMgr.FindWnd( SOLDIER_SELECT_WND ));
					if( pSoldier && !pSoldier->IsShow() )
					{
						int iArray = g_MyInfo.GetCharArrayToSlotIndex( pOwnerChar->GetSelectCharArray() );
						pSoldier->SetSelectWaitShow( iArray, true );
					}
				}
			}
		}
		break;
	}
}

void RentalSoldierLimitPopupWnd::ProcessRentalCheck()
{
	if( m_pPlayStage == NULL ) return;
	if( m_pPlayStage->GetModeType() == MT_MYROOM ) return;

	ioPlayMode *pPlayMode = m_pPlayStage->GetPlayMode();
	if( pPlayMode == NULL ) return;
	if( pPlayMode->GetModeState() != ioPlayMode::MS_PLAY ) return;

	if( FRAMEGETTIME() - m_dwProcessCheckTime < 1000 ) return;

	m_dwProcessCheckTime = FRAMEGETTIME();

	if( !IsShow() )
	{
		ioBaseChar *pOwnerChar = m_pPlayStage->GetOwnerChar();
		if( pOwnerChar == NULL ) return;
		if( pOwnerChar->GetState() == CS_OBSERVER || pOwnerChar->GetState() == CS_VIEW || pOwnerChar->GetState() == CS_LOADING || pOwnerChar->GetState() == CS_READY )
			return;
		if( pOwnerChar->IsHasCrown() )
			return;
		if( pOwnerChar->IsGangsi() )
			return;

		if( g_MyInfo.IsCharExerciseRentalInActive( pOwnerChar->GetSelectCharArray() ) )
		{
			// 대여 시간 만료
			SAFEDELETE( m_pCharIcon );
			m_pCharIcon = g_MyInfo.GetSoldierIcon( pOwnerChar->GetCharacterInfo().m_class_type, pOwnerChar->IsMale() );			

			m_nGradeType = g_UIImageRenderMgr.GetPowerUpGradeType( pOwnerChar->GetCharacterInfo().m_class_type );

			char szTitle[MAX_PATH] = "";
			SafeSprintf( szTitle, sizeof( szTitle ), STR(1), LIMIT_SEC );
			SetTitleText( szTitle );

			m_dwRoomOutTime = FRAMEGETTIME();
			ShowWnd();
		}
	}
	else
	{
		if( g_GUIMgr.IsShow( SOLDIER_LIMIT_POPUP_WND ) )
		{
			HideWnd();
			return;
		}

		ioBaseChar *pOwnerChar = m_pPlayStage->GetOwnerChar();
		if( pOwnerChar == NULL )
		{
			HideWnd();
			return;
		}

		if( pOwnerChar->GetState() == CS_OBSERVER || pOwnerChar->GetState() == CS_VIEW || pOwnerChar->GetState() == CS_LOADING || pOwnerChar->GetState() == CS_READY )
		{
			HideWnd();
			return;
		}

		if( pOwnerChar->IsGangsi() )
		{
			HideWnd();
			return;
		}

		if( g_MyInfo.IsCharExerciseRentalInActive( pOwnerChar->GetSelectCharArray() ) == false )
		{
			HideWnd();
			return;
		}

		int iSec = ( FRAMEGETTIME() - m_dwRoomOutTime ) / 1000;
		char szTitle[MAX_PATH] = "";
		SafeSprintf( szTitle, sizeof( szTitle ), STR(2), max( 0, LIMIT_SEC - iSec ) );
		SetTitleText( szTitle );

		if( FRAMEGETTIME() - m_dwRoomOutTime > LIMIT_SEC * 1000 )
		{
			m_pPlayStage = NULL;

			SP2Packet kPacket( CTPK_EXIT_ROOM );
			kPacket << EXIT_ROOM_CHAR_LIMIT << 0 << true;
			TCPNetwork::SendToServer( kPacket );
			if( ioPlayMode::GetModeType() != MT_TRAINING && ioPlayMode::GetModeType() != MT_HEADQUARTERS && ioPlayMode::GetModeType() != MT_HOUSE )
			{	
				g_BattleRoomMgr.LeaveRoom();
				g_LadderTeamMgr.LeaveTeam();
			}
		}
	}
}

void RentalSoldierLimitPopupWnd::OnProcess( float fTimePerSec )
{
	ProcessRentalCheck();

	if( !IsShow() )
		return;
}

void RentalSoldierLimitPopupWnd::OnRender()
{
	ioWnd::OnRender();
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pIconBack )
		m_pIconBack->Render( iXPos + 151, iYPos + 98, UI_RENDER_MULTIPLY, TFO_BILINEAR );

	g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iXPos + 151, iYPos + 98 );

	if( m_pCharIcon )
		m_pCharIcon->Render( iXPos + 151, iYPos + 98 );

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 144, FONT_SIZE_13, STR(1) );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_RED );	

	int iSec = ( FRAMEGETTIME() - m_dwRoomOutTime ) / 1000;
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(2), max( 0, LIMIT_SEC - iSec ) );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(3) );
	kPrinter.PrintFullText( iXPos + 151, iYPos + 164, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(4) );
	kPrinter.PrintFullText( iXPos + 151, iYPos + 184, TAT_CENTER );
	kPrinter.ClearList();
}
