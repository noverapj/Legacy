#include "StdAfx.h"

#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioEdit.h"
#include "../io3DEngine/InputBox.h"
#include "../io3DEngine/ioUIImageSetManager.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/iostringmanager.h"

#include "../ioMyInfo.h"
#include "../ioApplication.h"
#include "../NetworkWrappingFunc.h"
#include "../ioComplexStringPrinter.h"
#include "../TextColorTable.h"
#include "../ioEtcItemManager.h"
#include "../SlangUtil.h"
#include "../Setting.h"
#include "../copyfolder.h"
#include "../WndID.h"
#include "../ioQuestManager.h"

#include "ioSP2GUIManager.h"
#include "ioMessageBox.h"
#include ".\idchangewnd.h"
#include <strsafe.h>
#include "../Local/ioLocalManager.h"
#include "../Local/ioLocalParent.h"

IDChangeWnd::IDChangeWnd(void)
{
	m_pIcon = NULL;
	m_pSubIcon = NULL;
	m_pBackEffect = NULL;
}

IDChangeWnd::~IDChangeWnd(void)
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );
	SAFEDELETE( m_pBackEffect );
}

void IDChangeWnd::iwm_command( ioWnd *pWnd , int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_SHOTDOWN_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SetExit();
		}
		break;
	}
}

void IDChangeWnd::SetInfoAndShow( const ioHashString &szNewName )
{
	m_iShutDownCnt = 0;
	m_szNewName = szNewName;

	m_iShutDownCnt = 10;
	m_dwCntStartTime = FRAMEGETTIME();

	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pSubIcon);
	ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_CHANGE_ID );
	if( pItem )
	{
		m_pIcon = g_UIImageSetMgr.CreateImageByFullName( pItem->GetIconName() );
		m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( pItem->GetSubIconName() );
	}

	ShowWnd();
}

void IDChangeWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "back_effect" )
	{
		SAFEDELETE( m_pBackEffect );
		m_pBackEffect = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void IDChangeWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	enum { COUNT_WAIT_TIME = 1000, }; 
	if( FRAMEGETTIME() - m_dwCntStartTime < COUNT_WAIT_TIME )
		return;

	m_dwCntStartTime = FRAMEGETTIME();
	m_iShutDownCnt--;
	if( m_iShutDownCnt < 0 )
	{
		m_iShutDownCnt = 0;
		SetExit();
	}
}

void IDChangeWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		ICON_X	= 125,
		ICON_Y	= 98,

		TITLE_X	= 125,
		TITLE_Y	= 146,

		DESC_X	= 125,
		DESC_Y	= 169,

		LEFT_X	= 27,
		LEFT_Y	= 240,

		RIGHT_X	= 227,
		RIGHT_Y	= 240,
	};

	if( m_pBackEffect )
		m_pBackEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_MULTIPLY );

	if( m_pIcon )
		m_pIcon->Render( iXPos+ICON_X, iYPos+ICON_Y );
	if( m_pSubIcon )
		m_pSubIcon->Render( iXPos+ICON_X, iYPos+ICON_Y );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
	g_FontMgr.PrintText( iXPos+TITLE_X, iYPos+TITLE_Y, FONT_SIZE_17, STR(1) );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos+DESC_X, iYPos+DESC_Y, FONT_SIZE_13, STR(2) );
	g_FontMgr.PrintText( iXPos+DESC_X, iYPos+DESC_Y+20, FONT_SIZE_13, STR(3) );

	g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
	g_FontMgr.PrintText( iXPos+DESC_X, iYPos+DESC_Y+40, FONT_SIZE_13, STR(4), m_iShutDownCnt );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos+LEFT_X, iYPos+LEFT_Y, FONT_SIZE_13, STR(5) );

	g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
	g_FontMgr.PrintText( iXPos+LEFT_X, iYPos+LEFT_Y+21, FONT_SIZE_13, STR(6) );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_RIGHT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos+RIGHT_X, iYPos+RIGHT_Y, FONT_SIZE_13, STR(7), m_szNewName.c_str() );

	g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
	g_FontMgr.PrintText( iXPos+RIGHT_X, iYPos+RIGHT_Y+21, FONT_SIZE_13, STR(8), g_MyInfo.GetPublicID().c_str() );
}

void IDChangeWnd::SetExit()
{
	HideWnd();

	g_QuestMgr.ProcessChangeStage();
	SP2Packet kPacket( CTPK_LOGOUT );
	TCPNetwork::SendToServer( kPacket );
	MakeNewSaveFile();

	LOG.PrintTimeAndLog(0, "ID_CHANGE to LOGOUT" );
}

void IDChangeWnd::MakeNewSaveFile()
{
	char szRootDir[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH,szRootDir);

	char szSourcePath[MAX_PATH]="";
	char szDestath[MAX_PATH]="";

	StringCbPrintf( szSourcePath, sizeof( szSourcePath ), "%s\\Save\\%s", szRootDir, g_MyInfo.GetPublicID().c_str() );
	StringCbPrintf( szDestath, sizeof( szDestath ), "%s\\Save\\%s", szRootDir, m_szNewName.c_str() );

	bool bActive = true;
	CCopyFolder cf;	
	cf.CopyFolder( szSourcePath, szDestath, bActive );
}


