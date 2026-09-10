
#include "StdAfx.h"

#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioXMLElement.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioINILoader.h"
#include "../io3DEngine/HelpFunc.h"
#include "../io3DEngine/iostringmanager.h"
#include "../io3dengine/safesprintf.h"
#include "../io3DEngine/ioUIImageSetManager.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioUIRenderFrame.h"
#include "../io3DEngine/ioEdit.h"
#include "../io3DEngine/InputBox.h"

#include "../NetworkWrappingFunc.h"
#include "../TextColorTable.h"
#include "../EtcHelpFunc.h"

#include "../ioMyInfo.h"
#include "../ioApplication.h"

#include "../SlangUtil.h"
#include "../ioEtcItem.h"
#include "../ioEtcItemManager.h"
#include "../copyfolder.h"
#include "../ioGuildInfo.h"

#include "../Local/ioLocalManager.h"
#include "../Local/ioLocalParent.h"

#include "../WndID.h"
#include "ioSP2GUIManager.h"

#include "GlobalWnd.h"
#include "ManualWnd.h"
#include "ioMessageBox.h"

#include "MyInventoryInfoWnd.h"

#include ".\RecordInitWnd.h"
#include <strsafe.h>


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
RecordInitWnd::RecordInitWnd(void)
{
	m_pManual   = NULL;
	m_pEditFrm  = NULL;
	m_pPreEdit  = NULL;
	m_pInitEdit = NULL;
}

RecordInitWnd::~RecordInitWnd(void)
{
	SAFEDELETE( m_pEditFrm );
}

void RecordInitWnd::ShowRecordInitWnd( DWORD dwEtcType )
{
	m_dwEtcType = dwEtcType;

	m_pManual = ioEtcInvenSubManualMgr::GetSingletonPtr();

	ShowWnd();
	
	m_szTitle.Clear();
	ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( m_dwEtcType );
	if( pItem )
	{
		InventoryIconBtn *pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_ICON));
		if( pIconBtn )
		{
			ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( pItem->GetIconName() );
			ioUIRenderImage *pSubImg = g_UIImageSetMgr.CreateImageByFullName( pItem->GetSubIconName() );
			pIconBtn->SetIcon( pImg, FLOAT1, FLOAT1 );
			pIconBtn->SetSubIcon( pSubImg );
		}
		m_szTitle = pItem->GetName();
	}

	ioEdit *pEdit = dynamic_cast<ioEdit*>( FindChildWnd( ID_INIT_EDIT ) );
	if( pEdit && pEdit->IsShow() )
	{		
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString( "" );
		pEdit->KillKeyFocus();
		m_pInitEdit = pEdit;		
	}
}

void RecordInitWnd::iwm_hide()
{
	if( m_pPreEdit )	
	{
		ioWnd *pParent = m_pPreEdit->GetParent();
		if( pParent )
		{
			if( pParent->IsShow() )
				((ioEdit*)m_pPreEdit)->SetKeyFocus();	
		}
	}	
	m_pPreEdit = NULL;
}

void RecordInitWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "EditFrm" )
	{
		SAFEDELETE( m_pEditFrm );
		m_pEditFrm = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void RecordInitWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_INIT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			ioEdit *pEdit = dynamic_cast<ioEdit*>(m_pInitEdit);
			if( strcmp( pEdit->GetText(), STR(1) ) != 0 )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );			
				return;
			}

			SendRecordInit();
		}
		break;
	case ID_INIT_EDIT:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 || param == 1 )
			{
				if( ioEdit::m_pKeyFocusEdit != pWnd )
					m_pPreEdit = ioEdit::m_pKeyFocusEdit;

				ioEdit *pEdit = dynamic_cast<ioEdit*>(pWnd);
				pEdit->SetKeyFocus();
			}
		}
		break;
	}
}

void RecordInitWnd::SendRecordInit()
{
	SP2Packet kPacket( CTPK_ETCITEM_USE );
	kPacket << (int)m_dwEtcType;
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );
}

void RecordInitWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum 
	{
		TITLE_X_OFFSET	= 129,
		TITLE_Y_OFFSET	= 152,

		INFO_X			= 26,
		INFO_Y			= 397,
		INFO_GAP		= 24,

		EDIT_TEXT_X		= 62,
		EDIT_TEXT_Y		= 421,
	};


	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos+TITLE_X_OFFSET, iYPos+TITLE_Y_OFFSET, FONT_SIZE_17, STR(1), m_szTitle.c_str() );


	PrintManual( iXPos+27, iYPos+206, FONT_SIZE_12 );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "[" );
	kPrinter.SetTextColor( TCT_DEFAULT_RED );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(2) );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(3) );
	kPrinter.PrintFullText( iXPos + 129, iYPos + 397, TAT_CENTER );
	kPrinter.ClearList();

	if( m_pEditFrm )
	{
		m_pEditFrm->Render( iXPos, iYPos );

		if( m_pInitEdit != ioEdit::m_pKeyFocusEdit )
		{
			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
			g_FontMgr.PrintText( iXPos + 129, iYPos + 420, FONT_SIZE_12, STR(4) );
		}
	}
}

void RecordInitWnd::PrintManual( int a_iXPos, int a_iYPos , float fScale )
{
	if( !m_pManual )
		return;


	ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( m_dwEtcType );
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
RecordInitResultWnd::RecordInitResultWnd(void)
{
	m_pIcon = NULL;
	m_pSubIcon = NULL;
	m_pBackEffect = NULL;
}

RecordInitResultWnd::~RecordInitResultWnd(void)
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );
	SAFEDELETE( m_pBackEffect );
}

void RecordInitResultWnd::iwm_command( ioWnd *pWnd , int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_GO:
		if( cmd == IOBN_BTNUP )
		{
		}
		break;
	}
}

void RecordInitResultWnd::SetInfoAndShow( DWORD dwEtcType )
{
	m_dwEtcType = dwEtcType;

	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pSubIcon);
	ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( m_dwEtcType );
	if( pItem )
	{
		m_pIcon = g_UIImageSetMgr.CreateImageByFullName( pItem->GetIconName() );
		m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( pItem->GetSubIconName() );
	}

	ShowWnd();
	SetChildInActive( ID_GO );
}

void RecordInitResultWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "back_effect" )
	{
		SAFEDELETE( m_pBackEffect );
		m_pBackEffect = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void RecordInitResultWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		ICON_X		= 129,
		ICON_Y		= 105,

		TITLE_X		= 129,
		TITLE_Y		= 152,

		LIST_X		= 27,
		RIGHT_X		= 227,
		LIST_Y		= 206,
		LIST_GAP	= 21,

		FINAL_DESC_X	= 129,
		FINAL_DESC_Y	= 421,
	};

	if( m_pBackEffect )
		m_pBackEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_MULTIPLY );

	if( m_pIcon )
		m_pIcon->Render( iXPos+ICON_X, iYPos+ICON_Y );
	if( m_pSubIcon )
		m_pSubIcon->Render( iXPos+ICON_X, iYPos+ICON_Y );

	// Title
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	if( m_dwEtcType == ioEtcItem::EIT_ETC_BATTLE_RECORD_INIT )
		g_FontMgr.PrintText( iXPos+TITLE_X, iYPos+TITLE_Y, FONT_SIZE_17, STR(1) );
	else if( m_dwEtcType == ioEtcItem::EIT_ETC_LADDER_RECORD_INIT )
		g_FontMgr.PrintText( iXPos+TITLE_X, iYPos+TITLE_Y, FONT_SIZE_17, STR(2) );
	else if( m_dwEtcType == ioEtcItem::EIT_ETC_HERO_RECORD_INIT )
		g_FontMgr.PrintText( iXPos+TITLE_X, iYPos+TITLE_Y, FONT_SIZE_17, STR(3) );

	// Last
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
	g_FontMgr.PrintText( iXPos+FINAL_DESC_X, iYPos+FINAL_DESC_Y, FONT_SIZE_17, STR(4) );
}

