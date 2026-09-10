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

#include "../ioItemMaker.h"
#include "../ioCreateChar.h"
#include "../NetworkWrappingFunc.h"
#include "../ioMyInfo.h"
#include "../ioApplication.h"
#include "../ioSetItemInfo.h"
#include "../ioSetItemInfoManager.h"
#include "../WndID.h"
#include "../Setting.h"
#include "../EtcHelpFunc.h"
#include "../TextColorTable.h"

#include "ioUI3DRender.h"
#include "ioSP2GUIManager.h"
#include "MySoldierWnd.h"
#include <strsafe.h>
#include "../TextColorTable.h"

MSCrossWnd::MSCrossWnd()
{
}

MSCrossWnd::~MSCrossWnd()
{
}

void MSCrossWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_L_ROTATE:
	case ID_R_ROTATE:
	case ID_PLUS:
	case ID_MINUS:
	case ID_CHANGE:
		if( cmd == IOBN_BTNUP || cmd == IOBN_BTNDOWN || cmd == IOWN_OVERED )
		{
			if( ioWnd::m_pParent )
				ioWnd::m_pParent->iwm_command( this, cmd, dwID );
		}
		break;
	}
}

void MSCrossWnd::iwm_show()
{
	SetChildActive( ID_L_ROTATE );
	SetChildActive( ID_R_ROTATE );
	SetChildInActive( ID_PLUS );
	SetChildInActive( ID_MINUS );
	SetChildActive( ID_CHANGE );

	if( ioWnd::m_pParent )
	{
		int iXPos = ( ioWnd::m_pParent->GetWidth() / 2 ) - ( GetWidth() / 2 );
		int iYPos = ioWnd::m_pParent->GetHeight() - ( GetHeight() + 40 );
		SetWndPos( iXPos, iYPos );
	}
}

void MSCrossWnd::iwm_lbuttondown( const ioMouse& mouse )
{
	if( ioWnd::m_pParent )
		ioWnd::m_pParent->iwm_lbuttondown( mouse );

	ioWnd::iwm_lbuttondown( mouse );
}

void MSCrossWnd::iwm_lbuttonup( const ioMouse& mouse )
{
	if( ioWnd::m_pParent )
		ioWnd::m_pParent->iwm_lbuttonup( mouse );

	ioWnd::iwm_lbuttonup( mouse );
}

void MSCrossWnd::iwm_mouseover( const ioMouse& mouse )
{
	if( ioWnd::m_pParent )
	{
		if( ioWnd::m_pParent )
			ioWnd::m_pParent->iwm_command( this, IOBN_BTNUP, ID_L_ROTATE );
	}

	ioWnd::iwm_mouseover( mouse );
}

void MSCrossWnd::iwm_mouseleave( const ioMouse& mouse )
{
	ioWnd::iwm_mouseleave( mouse );
}
//////////////////////////////////////////////////////////////////////////
void CrossWnd::iwm_show()
{
	SetChildActive( ID_L_ROTATE );
	SetChildActive( ID_R_ROTATE );
	SetChildInActive( ID_PLUS );
	SetChildInActive( ID_MINUS );
	SetChildInActive( ID_CHANGE );

	if( ioWnd::m_pParent )
	{
		int iXPos = ( ioWnd::m_pParent->GetWidth() / 2 ) - ( GetWidth() / 2 );
		int iYPos = ioWnd::m_pParent->GetHeight() - ( GetHeight() + 10 );
		SetWndPos( iXPos, iYPos );
	}
}
//////////////////////////////////////////////////////////////////////////
void CharOverWnd::iwm_mouseover( const ioMouse& mouse )
{
	ShowChildWnd( ID_CROSS_WND );
	ioWnd::iwm_mouseover( mouse );
}

void CharOverWnd::iwm_mouseleave( const ioMouse& mouse )
{
	HideChildWnd( ID_CROSS_WND );
	ioWnd::iwm_mouseleave( mouse );
}

void CharOverWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_CROSS_WND:
		if( cmd == IOWN_OVERED )
		{
			ShowChildWnd( ID_CROSS_WND );
		}
		else if( cmd == IOBN_BTNDOWN )
		{
			if( ioWnd::m_pParent )
				ioWnd::m_pParent->iwm_command( this, IOBN_BTNDOWN, param );
		}
		else if( cmd == IOBN_BTNUP )
		{
			if( ioWnd::m_pParent )
				ioWnd::m_pParent->iwm_command( this, IOBN_BTNUP, param );
		}
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
MSExpWnd::MSExpWnd()
{
	m_pGaugeBack	= NULL;
	m_pGaugeBar		= NULL;
	m_pGaugeLeft	= NULL;
	m_pGaugeCenter	= NULL;
	m_pGaugeRight	= NULL;
	m_pGaugeExp		= NULL;
	m_pGaugePer		= NULL;
	m_pGaugeNumber	= NULL;
	m_iCharIndex    = -1;
	m_bAllShow		= false;
}

MSExpWnd::~MSExpWnd()
{
	SAFEDELETE( m_pGaugeBack );
	SAFEDELETE( m_pGaugeBar );
	SAFEDELETE( m_pGaugeLeft );
	SAFEDELETE( m_pGaugeCenter );
	SAFEDELETE( m_pGaugeRight );
	SAFEDELETE( m_pGaugeExp );
	SAFEDELETE( m_pGaugePer );
	SAFEDELETE( m_pGaugeNumber );
}

void MSExpWnd::iwm_show()
{
	if( ioWnd::m_pParent )
	{
		int iXPos = ioWnd::m_pParent->GetWidth() / 2;
		int iYPos = ioWnd::m_pParent->GetHeight() - 14;
		SetWndPos( iXPos, iYPos );
	}	
}

void MSExpWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "GaugeBack" )
	{
		SAFEDELETE( m_pGaugeBack );
		m_pGaugeBack = pImage;
	}
	else if( szType == "GaugeBar" )
	{		
		SAFEDELETE( m_pGaugeBar );	
		m_pGaugeBar = pImage;
	}
	else if( szType == "GaugeLeft" )
	{
		SAFEDELETE( m_pGaugeLeft );
		m_pGaugeLeft = pImage;
	}
	else if( szType == "GaugeCenter" )
	{
		SAFEDELETE( m_pGaugeCenter );
		m_pGaugeCenter = pImage;
	}
	else if( szType == "GaugeRight" )
	{
		SAFEDELETE( m_pGaugeRight );
		m_pGaugeRight = pImage;
	}
	else if( strcmp(szType.c_str(), "Exp") == 0 )
	{
		SAFEDELETE( m_pGaugeExp );
		m_pGaugeExp = pImage;
	}
	else if( strcmp(szType.c_str(), "Per") == 0 )
	{
		SAFEDELETE( m_pGaugePer );
		m_pGaugePer = pImage;
	}
	else if( szType == "Number" )
	{
		SAFEDELETE( m_pGaugeNumber );
		m_pGaugeNumber = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void MSExpWnd::RenderGauge( int xPos, int yPos, int iExpPer )
{
	if( !m_pGaugeBack || !m_pGaugeBar || !m_pGaugeLeft || !m_pGaugeCenter ||
		!m_pGaugeRight|| !m_pGaugeExp || !m_pGaugePer  || !m_pGaugeNumber ) return;

	int iPer   = iExpPer;
	int iWidth = m_pGaugeBar->GetWidth();
	int iEndWidth = (float)iWidth * ( (float)iPer / 100 );
	
	m_pGaugeBack->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
	m_pGaugeBack->Render( xPos, yPos );

	if( iPer != 0 )
	{
		m_pGaugeBar->SetHorzFormatting( ioUIRenderImage::HF_LEFT_ALIGN );
		m_pGaugeBar->RenderWidthCut( xPos - 58, yPos + 2, 0, iEndWidth );
		
		//
		iWidth = m_pGaugeLeft->GetWidth();
		m_pGaugeLeft->SetHorzFormatting( ioUIRenderImage::HF_LEFT_ALIGN );
		if( iEndWidth <= 10 )
		{
			if( iEndWidth % 10 == 0 )
				iWidth -= 5 - (iEndWidth / 2);
			else 
				iWidth -= 5 - ( iEndWidth + 1 ) / 2;
			m_pGaugeLeft->RenderWidthCut( xPos - 63, yPos - 7, 0, iWidth, UI_RENDER_SCREEN );
		}
		else
			m_pGaugeLeft->Render( xPos - 63, yPos - 7, UI_RENDER_SCREEN );

		if( iEndWidth > 10 )
		{
			int iRealWidth = iEndWidth - 10;
			m_pGaugeCenter->SetSize( iRealWidth, m_pGaugeCenter->GetHeight() );
			m_pGaugeCenter->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pGaugeCenter->Render( xPos - 53, yPos - 7, UI_RENDER_SCREEN );
		}
		
		iWidth = m_pGaugeRight->GetWidth();
		m_pGaugeRight->SetHorzFormatting( ioUIRenderImage::HF_LEFT_ALIGN );
		if( iEndWidth <= 10 )
		{
			int leftWidth = 10;
			if( iEndWidth % 10 == 0 )
				leftWidth -= 5 - (iEndWidth / 2);
			else 
				leftWidth -= 5 - ( iEndWidth + 1 ) / 2;
			int iRealWidth = 5 - (iEndWidth / 2);
			
			m_pGaugeRight->RenderWidthCut( ( xPos - 63 ) + leftWidth - iRealWidth, yPos - 7, iRealWidth, iWidth, UI_RENDER_SCREEN );
		}
		else
			m_pGaugeRight->Render( ( xPos - 63 ) + iEndWidth , yPos - 7, UI_RENDER_SCREEN );
	}
	//
	m_pGaugeExp->SetHorzFormatting( ioUIRenderImage::HF_RIGHT_ALIGN );
	m_pGaugeNumber->SetHorzFormatting( ioUIRenderImage::HF_LEFT_ALIGN );
	m_pGaugePer->SetHorzFormatting( ioUIRenderImage::HF_LEFT_ALIGN );

	if( iPer < 10 )
	{
		m_pGaugeExp->Render( xPos + 2, yPos + 2 );
		m_pGaugeNumber->RenderNum( xPos + 5, yPos + 2, iPer );
		m_pGaugePer->Render( xPos + 12, yPos + 2 );
	}
	else
	{
		m_pGaugeExp->Render( xPos - 1, yPos + 2 );
		m_pGaugeNumber->RenderNum( xPos + 2, yPos + 2, iPer, -1 );
		m_pGaugePer->Render( xPos + 16, yPos + 2 );
	}
}

void MSExpWnd::OnRender()
{
	ioWnd::OnRender();

	if( m_iCharIndex == -1 ) return;

	float fXPos, fYPos;
	fXPos = GetDerivedPosX();
	fYPos = GetDerivedPosY();

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );

	const ioSetItemInfo *pInfo = g_SetItemInfoMgr.GetSetInfoByIdx( g_MyInfo.GetClassType( m_iCharIndex ) - 1 );
	if( pInfo )
		m_szClassName = pInfo->GetName();

	char szLimitDate[MAX_PATH] = "";
	if( g_MyInfo.IsCharMortmain( g_MyInfo.GetClassType( m_iCharIndex ) ) )
		SafeSprintf( szLimitDate, sizeof( szLimitDate ), STR(1) );
	else
		SafeSprintf( szLimitDate, sizeof( szLimitDate ), STR(2), g_MyInfo.GetCharTotalSecond( m_iCharIndex ) / 60 );

	if( m_bAllShow )
	{
		g_FontMgr.PrintText( fXPos, fYPos - 20.0f, FONT_SIZE_13, STR(3), g_MyInfo.GetClassLevel( g_MyInfo.GetClassType( m_iCharIndex), true ), m_szClassName.c_str(), szLimitDate );
		RenderGauge( fXPos, fYPos + 2, g_MyInfo.GetExpRate( g_MyInfo.GetClassType( m_iCharIndex ) ) );
	}
	else
	{
		g_FontMgr.PrintText( fXPos, fYPos - FLOAT10, FONT_SIZE_13, STR(4), g_MyInfo.GetClassLevel( g_MyInfo.GetClassType( m_iCharIndex), true ), m_szClassName.c_str(), szLimitDate );
	}
}
//////////////////////////////////////////////////////////////////////////
MSCharacterWnd::MSCharacterWnd()
{
	m_iCharIndex  = -1;
	m_fRotateYaw  = 0.0f;
	
	m_fCharZPos   = FLOAT500;
	m_fBackRot    = 0.0f;
}

MSCharacterWnd::~MSCharacterWnd()
{
}

void MSCharacterWnd::iwm_create()
{
	m_CurCharInfo.Init();
	memset( m_ItemCodeArray, 0xff, sizeof(m_ItemCodeArray) );
}

void MSCharacterWnd::LoadMyCharacter( int iIndex )
{
	DWORD dwCharIndex = g_MyInfo.GetCharIndex( iIndex );
	if( dwCharIndex == 0 )
		return;
	
	m_iCharIndex = iIndex;

	UICharLoadingBuffer *pUIChar = g_MyInfo.GetSoldierSelectUIChar( dwCharIndex );
	if( !pUIChar )
	{
		g_MyInfo.SetBeforeCharLoading( true );
		pUIChar = g_MyInfo.GetSoldierSelectUIChar( dwCharIndex );
	}
	if( !pUIChar ) return;
	if( !pUIChar->m_aMyChar ) return;
	if( !pUIChar->m_pUI3DRender ) return;

	CHARACTER kNewInfo = g_MyInfo.GetCharacter( m_iCharIndex );
//	if( kNewInfo != m_CurCharInfo )
	{
		pUIChar->m_aMyChar->SetPosition( 0.0f, -100.0f, 1000 );
		pUIChar->m_aMyChar->SetYaw( FLOAT1 );
		pUIChar->m_aMyChar->RotateY( m_fBackRot );
		pUIChar->m_pUI3DRender->SetLookAt( 0.0f, 0.0f, 900.0f );
		pUIChar->m_pUI3DRender->SetProjection( FLOAT1, 600.0f, 45.0f, 86.0f/180.0f );

		m_CurCharInfo = kNewInfo;
		memset( m_ItemCodeArray, 0xff, sizeof(m_ItemCodeArray) );	// 아이템 초기화
	}
	pUIChar->m_aMyChar->SetThreadLoading( true );

/*
	// Equip Item..
	for( int i=0 ; i< ioCreateChar::MAX_CCS ; i++ )
	{
		const ITEM_DATA &rkItem = g_MyInfo.GetCharItem( m_iCharIndex, i );

		if( m_ItemCodeArray[i] != rkItem.m_item_code )
		{
			if( rkItem.m_item_code > 0 )
				pUIChar->m_aMyChar->EquipItem( rkItem.m_item_code );
			else
				pUIChar->m_aMyChar->ReleaseItem( i );

			m_ItemCodeArray[i] = rkItem.m_item_code;
		}
	}

	pUIChar->m_aMyChar->ClearAllAnimations();

	if( !pUIChar->m_aMyChar->SetWeaponShopMotion() )
	{
		ioHashString szUIAnimation = pUIChar->m_pUI3DRender->GetModelAnimation( 0, pUIChar->m_aMyChar->IsMale() );
		if( !szUIAnimation.IsEmpty() )
		{
			pUIChar->m_aMyChar->SetLoopAni( szUIAnimation );
		}
	}
*/
}

void MSCharacterWnd::RotateY( float fYaw )
{
	m_fRotateYaw = fYaw;
}

void MSCharacterWnd::SetRotate( float fBackRot )
{
	m_fBackRot = fBackRot;

	UICharLoadingBuffer *pUIChar = g_MyInfo.GetSoldierSelectUIChar( g_MyInfo.GetCharIndex( m_iCharIndex ) );
	if( !pUIChar ) return;
	if( !pUIChar->m_aMyChar ) return;

	pUIChar->m_aMyChar->RotateY( m_fBackRot );
}

void MSCharacterWnd::ProcessRotate( float fTimePerSec ) 
{
	UICharLoadingBuffer *pUIChar = g_MyInfo.GetSoldierSelectUIChar( g_MyInfo.GetCharIndex( m_iCharIndex ) );
	if( !pUIChar ) return;
	if( !pUIChar->m_aMyChar ) return;

	pUIChar->m_aMyChar->RotateY( m_fRotateYaw );
	m_fBackRot += m_fRotateYaw;
}

void MSCharacterWnd::RestoreCharItems()
{
	UICharLoadingBuffer *pUIChar = g_MyInfo.GetSoldierSelectUIChar( g_MyInfo.GetCharIndex( m_iCharIndex ) );
	if( !pUIChar ) return;
	if( !pUIChar->m_aMyChar ) return;

	pUIChar->m_aMyChar->ReleaseAllEquipItems();

	for( int i=0; i<ioCreateChar::MAX_CCS; i++ )
	{
		const ITEM_DATA &rkItem = g_MyInfo.GetCharItem( m_iCharIndex, i );
		if( rkItem.m_item_code > 0 )
		{
			pUIChar->m_aMyChar->EquipItem( rkItem.m_item_code, rkItem.m_item_male_custom, rkItem.m_item_female_custom );
		}
	}
}

void MSCharacterWnd::EquipSetItem( const ioSetItemInfo *pSetInfo )
{
	if( !pSetInfo ) return;

	UICharLoadingBuffer *pUIChar = g_MyInfo.GetSoldierSelectUIChar( g_MyInfo.GetCharIndex( m_iCharIndex ) );
	if( !pUIChar ) return;
	if( !pUIChar->m_aMyChar ) return;

	pUIChar->m_aMyChar->ReleaseAllEquipItems();

	int iItemCodeCnt = pSetInfo->GetSetItemCnt();
	for( int i=0 ; i<iItemCodeCnt ; i++ )
	{
		pUIChar->m_aMyChar->EquipItem( pSetInfo->GetSetItemCode( i ), 0, 0 );
	}
}

void MSCharacterWnd::EquipItem( const ioItem *pItem )
{
	if( !pItem ) return;
	UICharLoadingBuffer *pUIChar = g_MyInfo.GetSoldierSelectUIChar( g_MyInfo.GetCharIndex( m_iCharIndex ) );
	if( !pUIChar ) return;
	if( !pUIChar->m_aMyChar ) return;

	pUIChar->m_aMyChar->EquipItem( pItem->GetItemCode(), pItem->GetItemMaleCustom(), pItem->GetItemFemaleCustom() );
}

void MSCharacterWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return ;

	ProcessRotate( fTimePerSec );

	UICharLoadingBuffer *pUIChar = g_MyInfo.GetSoldierSelectUIChar( g_MyInfo.GetCharIndex( m_iCharIndex ) );
	if( !pUIChar ) return;
	if( !pUIChar->m_aMyChar ) return;

	pUIChar->m_aMyChar->Update( fTimePerSec );

	for( int i = 0;i < MAX_INVENTORY;i++)
	{
		const ITEM_DATA &rkItem = g_MyInfo.GetCharItem( m_iCharIndex, i );
		if( pUIChar->m_aMyChar->GetEquipedItemCode( i ) != (DWORD)rkItem.m_item_code )
		{
			LoadMyCharacter( m_iCharIndex );
			break;
		}
	}

	if( g_MyInfo.GetCharIndex( m_iCharIndex ) == 0 && GetParent() )
	{
		GetParent()->HideWnd();
	}
}

void MSCharacterWnd::RenderChar( int iXPos, int iYPos )
{
	UICharLoadingBuffer *pUIChar = g_MyInfo.GetSoldierSelectUIChar( g_MyInfo.GetCharIndex( m_iCharIndex ) );
	if( !pUIChar ) return;
	if( !pUIChar->m_pUI3DRender ) return;

	D3DRECT rcD3D;
	rcD3D.x1 = GetDerivedPosX() + 2;
	rcD3D.y1 = GetDerivedPosY() + 2;
	rcD3D.x2 = rcD3D.x1 + 86;
	rcD3D.y2 = rcD3D.y1 + 180;
	pUIChar->m_pUI3DRender->RenderViewPort( &rcD3D, true );
}

//////////////////////////////////////////////////////////////////////////
MySoldierWnd::MySoldierWnd()
{
}

MySoldierWnd::~MySoldierWnd()
{
}

void MySoldierWnd::iwm_mouseover( const ioMouse& mouse )
{
	ShowChildWnd( ID_CROSS_WND );
	ShowExpBar( true );
	ioWnd::iwm_mouseover( mouse );
}

void MySoldierWnd::iwm_mouseleave( const ioMouse& mouse )
{
	HideChildWnd( ID_CROSS_WND );
	ShowExpBar( false );
	ioWnd::iwm_mouseleave( mouse );
}

void MySoldierWnd::SetSoldier( int iIndex )
{
	MSCharacterWnd *pChar = dynamic_cast<MSCharacterWnd*>( FindChildWnd( ID_CHARACTER_WND ) );
	if( pChar )
		pChar->LoadMyCharacter( iIndex );

	MSExpWnd *pExp = dynamic_cast<MSExpWnd*>( FindChildWnd( ID_EXP_WND ) );
	if( pExp )
		pExp->SetMyCharacter( iIndex );
}

void MySoldierWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_CHARACTER_WND:
		break;
	case ID_EXP_WND:
		if( cmd == IOWN_OVERED )
		{
			ShowExpBar( true );
		}
		break;
	case ID_CROSS_WND:
		if( cmd == IOWN_OVERED )
		{
			ShowExpBar( true );
			ShowChildWnd( ID_CROSS_WND );
		}
		else if( cmd == IOBN_BTNDOWN )
		{
			switch( param )
			{
			case MSCrossWnd::ID_L_ROTATE:
				{
					MSCharacterWnd *pChar = dynamic_cast<MSCharacterWnd*>( FindChildWnd( ID_CHARACTER_WND ) );
					if( pChar )
						pChar->RotateY( FLOAT1 );
				}
				break;
			case MSCrossWnd::ID_R_ROTATE:
				{
					MSCharacterWnd *pChar = dynamic_cast<MSCharacterWnd*>( FindChildWnd( ID_CHARACTER_WND ) );
					if( pChar )
						pChar->RotateY( -FLOAT1 );
				}
				break;
			case MSCrossWnd::ID_PLUS:
				break;
			case MSCrossWnd::ID_MINUS:
				break;
			case MSCrossWnd::ID_CHANGE:
				if( cmd == IOBN_BTNDOWN )
				{
				}
				break;
			}
		}
		else if( cmd == IOBN_BTNUP )
		{
			switch( param )
			{
			case MSCrossWnd::ID_L_ROTATE:
			case MSCrossWnd::ID_R_ROTATE:
				{
					MSCharacterWnd *pChar = dynamic_cast<MSCharacterWnd*>( FindChildWnd( ID_CHARACTER_WND ) );
					if( pChar )
						pChar->RotateY( 0.0f );
				}
				break;
			case MSCrossWnd::ID_PLUS:
				break;
			case MSCrossWnd::ID_MINUS:
				break;
			case MSCrossWnd::ID_CHANGE:
				break;
			}
		}
		break;
	}
}

void MySoldierWnd::iwm_show()
{
	ShowChildWnd( ID_EXP_WND );
	ShowChildWnd( ID_CHARACTER_WND );

	char szTitle[MAX_PATH] = "";
	sprintf_e( szTitle, "CharWnd%d", ioWnd::GetID() );
	int iXPos = g_GUIMgr.GUIPosLoadInt( szTitle, "XPos", ioWnd::GetXPos() );
	int iYPos = g_GUIMgr.GUIPosLoadInt( szTitle, "YPos", ioWnd::GetYPos() );
	ioWnd::SetWndPos( iXPos, iYPos );

	MSCharacterWnd *pChar = dynamic_cast<MSCharacterWnd*>( FindChildWnd( ID_CHARACTER_WND ) );
	if( pChar )
		pChar->SetRotate( g_GUIMgr.GUIPosLoadInt( szTitle, "YRot", 0 ) );
}

void MySoldierWnd::iwm_hide()
{
	HideChildWnd( ID_EXP_WND );
	HideChildWnd( ID_CHARACTER_WND );

	char szTitle[MAX_PATH] = "";
	sprintf_e( szTitle, "CharWnd%d", ioWnd::GetID() );
	g_GUIMgr.GUIPosSaveInt( szTitle, "XPos", ioWnd::GetXPos() );
	g_GUIMgr.GUIPosSaveInt( szTitle, "YPos", ioWnd::GetYPos() );
	MSCharacterWnd *pChar = dynamic_cast<MSCharacterWnd*>( FindChildWnd( ID_CHARACTER_WND ) );
	if( pChar )
	{
		g_GUIMgr.GUIPosSaveInt( szTitle, "YRot", (int)pChar->GetRotatePos() );
	}
}

void MySoldierWnd::ShowExpBar( bool bShow )
{
	MSExpWnd *pExp = dynamic_cast<MSExpWnd*>( FindChildWnd( ID_EXP_WND ) );
	if( pExp )
		pExp->ShowExpBar( bShow );
}

void MySoldierWnd::OnRender()
{
	ioWnd::OnRender();
}