
#include "stdafx.h"

#include "../io3DEngine/ioINILoader.h"
#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioScroll.h"
#include "../io3DEngine/ioXMLDocument.h"
#include "../io3DEngine/ioXMLElement.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioUIImageSetManager.h"
#include "../io3DEngine/ioSoundManager.h"
#include "../io3DEngine/iostringmanager.h"
#include "../io3DEngine/Safesprintf.h"

#include "../ioMyInfo.h"
#include "../ioApplication.h"
#include "../NetworkWrappingFunc.h"
#include "../ioCreateChar.h"
#include "../TextColorTable.h"
#include "../ioItemMaker.h"
#include "../ioClassPrice.h"
#include "../ioSetItemInfo.h"
#include "../ioSetItemInfoManager.h"
#include "../ioDecorationPrice.h"
#include "../EtcHelpFunc.h"
#include "../ioEtcItem.h"
#include "../ioEtcItemManager.h"
#include "../WndID.h"

#include "ioUI3DRender.h"
#include "ioSP2GUIManager.h"

#include "GlobalWnd.h"
#include "ManualWnd.h"
#include "MyInventoryWnd.h"

#include "PreSetSoldierPackageWnd.h"

#include <strsafe.h>

//////////////////////////////////////////////////////////////////////////
PreSetSoldierPackageWnd::PreSetSoldierPackageWnd()
{
	m_dwEtcItemType = 0;

	m_CharInfo.m_class_type = 1;
	m_CharInfo.m_kindred	= 1;
	m_CharInfo.m_sex		= 1;
	m_CharInfo.m_beard	    = 1;
	m_CharInfo.m_face	    = 1;
	m_CharInfo.m_hair	    = 1;
	m_CharInfo.m_skin_color = 1;
	m_CharInfo.m_hair_color = 1;
	m_CharInfo.m_accessories= 1;
	m_CharInfo.m_underwear  = 1;

	m_pManual = NULL;
}

PreSetSoldierPackageWnd::~PreSetSoldierPackageWnd()
{

}

void PreSetSoldierPackageWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/PackageHelpWnd.xml", this );
}

void PreSetSoldierPackageWnd::iwm_hide()
{
	HideChildWnd( ID_SOLDIER_WND );
}

void PreSetSoldierPackageWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_BUY:
		if( cmd == IOBN_BTNUP )
		{
			// 용병패키지 UI 예외처리
			if( m_CharInfo.m_class_type == 0 )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "선택 가능한 용병이 없습니다." );
				return;
			}

			SP2Packet kPacket( CTPK_ETCITEM_USE );
			kPacket << m_dwEtcItemType;	
			TCPNetwork::SendToServer( kPacket );
			TCPNetwork::MouseBusy( true );
		}
		break;
	}
}

void PreSetSoldierPackageWnd::ShowPackage( DWORD dwType )
{
	m_dwEtcItemType = 0;

	ioEtcItemPreSetPackage *pPreSetPackageItem = dynamic_cast<ioEtcItemPreSetPackage*>(g_EtcItemMgr.FindEtcItem(dwType));
	if( !pPreSetPackageItem )
		return;

	m_dwEtcItemType = dwType;
	m_pManual = ioEtcInvenSubManualMgr::GetSingletonPtr();

	pPreSetPackageItem->SetPreSetPackageInfo( m_CharInfo );

	NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ));
	if( pCharWnd )
	{
		pCharWnd->SetZPosition( 600.0f );
		pCharWnd->SetChar( m_CharInfo );
		pCharWnd->SetEquip( false, false, true );
		pCharWnd->ShowWnd();
	}

	ShowWnd();
}

//void PreSetSoldierPackageWnd::OnProcess( float fTimePerSec )
//{
//	if( !IsShow() ) return;
//}

void PreSetSoldierPackageWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
}

void PreSetSoldierPackageWnd::PrintManual( int a_iXPos, int a_iYPos , float fScale )
{
	if( !m_pManual )
		return;

	ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( m_dwEtcItemType );

	if( !pItem ) return;

	int iSubManual = pItem->GetInventorySubManual() - 1;
	ioComplexStringPrinter kPrinter;
	int iMaxLine = m_pManual->GetMaxLine( iSubManual );
	for(int iLine = 1; iLine < iMaxLine; iLine++)
	{
		m_pManual->SetPrinter( iSubManual, iLine, fScale, kPrinter);
		a_iYPos += m_pManual->GetSpaceLine( iSubManual, iLine);
		kPrinter.PrintFullTextWidthCut( a_iXPos, a_iYPos, TAT_LEFT, DESC_WIDTH );
		kPrinter.ClearList();
	}
}

//////////////////////////////////////////////////////////////////////////
PreSetSoldierPackageResultWnd::PreSetSoldierPackageResultWnd()
{
}

PreSetSoldierPackageResultWnd::~PreSetSoldierPackageResultWnd()
{
}

void PreSetSoldierPackageResultWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/PackageHelpWnd.xml", this );
}

void PreSetSoldierPackageResultWnd::SetResultSoldierPackage( CHARACTER &rkCharInfo, int iKeepBonusPeso, int iLimitTime )
{
	m_CharInfo = rkCharInfo;
	m_iLimitTime = iLimitTime;

	NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ));
	if( pCharWnd )
	{
		pCharWnd->SetZPosition( 600.0f );
		pCharWnd->SetChar( m_CharInfo );
		pCharWnd->SetEquip( false, false, true );
		pCharWnd->ShowWnd();
	}

	ShowWnd();
}

void PreSetSoldierPackageResultWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_GO:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd )
			{
				pInvenWnd->ShowSoldierTabDirect( m_CharInfo.m_class_type );
			}
		}
		break;
	}
}

void PreSetSoldierPackageResultWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		TITLE_X		= 129,
		TITLE_Y		= 400,

		FINAL_DESC_X	= 129,
		FINAL_DESC_Y	= 421,
	};

	// Title
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
	g_FontMgr.PrintText( iXPos+TITLE_X, iYPos+TITLE_Y, FONT_SIZE_17, "%s", g_MyInfo.GetClassName( m_CharInfo.m_class_type, m_CharInfo.m_eCharPowerUpType ) );

	// Last
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );

	if( m_iLimitTime == 0 )
	{
		g_FontMgr.PrintText( iXPos+FINAL_DESC_X, iYPos+FINAL_DESC_Y, FONT_SIZE_17, "영구사용 획득" );
	}
	else
	{
		char szTemp[MAX_PATH] = "";
		int iTotalLimitDate = m_iLimitTime / 60;
		int iHour   = iTotalLimitDate / 60;
		int iMinute = iTotalLimitDate % 60;
		if( iMinute == 0 )
			SafeSprintf( szTemp, sizeof( szTemp ), "%d시간", iHour );
		else
			SafeSprintf( szTemp, sizeof( szTemp ), "%d시간 %d분", iHour, iMinute );

		g_FontMgr.PrintText( iXPos+FINAL_DESC_X, iYPos+FINAL_DESC_Y, FONT_SIZE_17, "%s 획득", szTemp );
	}
}

