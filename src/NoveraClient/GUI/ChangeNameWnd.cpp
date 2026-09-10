
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

#include ".\ChangeNameWnd.h"
#include <strsafe.h>


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ChangeNameWnd::ChangeNameWnd(void)
{
	m_pManual = NULL;
	m_pEditFrm = NULL;
}

ChangeNameWnd::~ChangeNameWnd(void)
{
	SAFEDELETE( m_pEditFrm );
}

void ChangeNameWnd::ShowChangeIDWnd( DWORD dwEtcType )
{
	m_dwEtcType = dwEtcType;

	m_pManual = ioEtcInvenSubManualMgr::GetSingletonPtr();

	ShowWnd();

	m_bRenderNotice = false;
	m_szCurName.Clear();
	m_szNewName.Clear();
	m_szDefaultNotice.Clear();
	m_szTitle.Clear();
	m_szErrorNotice.Clear();

	m_Info[0].ClearList();
	m_Info[1].ClearList();

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

	if( m_dwEtcType == ioEtcItem::EIT_ETC_CHANGE_ID )
	{
		m_szCurName = g_MyInfo.GetPublicID();
		m_szDefaultNotice = STR(1);

		m_Info[0].SetTextStyle( TS_NORMAL );
		m_Info[0].SetBkColor( 0, 0, 0 );	
		m_Info[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
		m_Info[0].AddTextPiece( FONT_SIZE_12, STR(2), m_szCurName.c_str() );
	}
	else if( m_dwEtcType == ioEtcItem::EIT_ETC_GUILD_NAME_CHANGE )
	{
		ioGuildData *pGuildData = g_GuildInfo.GetGuildData( g_GuildInfo.GetGuildIndex() );
		if( pGuildData )
			m_szCurName = pGuildData->GetGuildName();

		m_szDefaultNotice = STR(3);

		m_Info[0].SetTextStyle( TS_NORMAL );
		m_Info[0].SetBkColor( 0, 0, 0 );	
		m_Info[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
		m_Info[0].AddTextPiece( FONT_SIZE_12, STR(4), m_szCurName.c_str() );
	}

	m_Info[1].SetTextStyle( TS_NORMAL );
	m_Info[1].SetBkColor( 0, 0, 0 );	
	m_Info[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_Info[1].AddTextPiece( FONT_SIZE_12, STR(5) );

	m_pPreEdit = ioEdit::m_pKeyFocusEdit;

	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_CHANGE_EDIT ) );
	if( pEdit )
	{
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString( "" );
		pEdit->KillKeyFocus();
		m_bRenderNotice = true;
	}

	g_App.DisableAltCtrl();
}

void ChangeNameWnd::iwm_hide()
{
	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_CHANGE_EDIT ) );
	if( pEdit )
	{
		pEdit->ClearString();
		pEdit->KillKeyFocus();		
	}

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

	g_App.EnableAltCtrl();
}

void ChangeNameWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
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

void ChangeNameWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_CHANGE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			ChangeName();
		}
		break;
	case ID_CHANGE_EDIT:
		if( cmd == IOED_INPUT )
		{
		}
		else if( cmd == IOBN_BTNDOWN )
		{
			ioEdit *pEdit = dynamic_cast<ioEdit*> ( pWnd );
			if( pEdit )
			{
				if( ioEdit::m_pKeyFocusEdit != pEdit )
					m_pPreEdit = ioEdit::m_pKeyFocusEdit;

				if( strncmp( pEdit->GetText(), m_szDefaultNotice.c_str(),  m_szDefaultNotice.Length() ) == 0 ||
					strncmp( pEdit->GetText(), m_szErrorNotice.c_str(),  m_szErrorNotice.Length() ) == 0 )
				{
					pEdit->ClearString();
					pEdit->SetKeyFocus();
					g_InputBox.SetString( "" );
				}
			}
		}
		break;
	case MESSAGE_BOX3:
		if( cmd == IOBN_BTNUP )
		{
			if( param == IDYES )
			{
				SendNewName();
			}
		}
		break;
	}
}

void ChangeNameWnd::ChangeName()
{
	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	if( !pUserEtcItem )
	{
		ErrorEdit( STR(1) );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	if( !pUserEtcItem->GetEtcItem( m_dwEtcType, kSlot) )
	{
		ErrorEdit( STR(2) );
		return;
	}

	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_CHANGE_EDIT ) );
	if( !pEdit )
	{
		ErrorEdit( STR(3) );
		return;
	}

	m_szNewName = pEdit->GetText();
	if( m_szNewName.IsEmpty() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4), m_szDefaultNotice.c_str() );
		return;
	}

	if( m_szNewName == m_szDefaultNotice )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5), m_szDefaultNotice.c_str() );
		return;
	}

	if( m_dwEtcType == ioEtcItem::EIT_ETC_CHANGE_ID )
	{
		if( !g_App.IsRightID( m_szNewName.c_str() ) )
		{
			ErrorEdit( STR(6) );
			return;
		}

		ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
		if( pLocal )
		{
			if( !pLocal->IsRightNewID( m_szNewName.c_str() ) )
			{
				ErrorEdit( STR(7) );
				return;
			}
		}

		ioHashString szNotMakeString;
		if( g_App.IsNotMakeID( m_szNewName.c_str(), szNotMakeString ) )
		{
			char szError[MAX_PATH]="";
			ErrorEdit( STR(8) );
			return;
		}

		char szSlangName[MAX_PATH]="";
		StringCbPrintf( szSlangName, sizeof( szSlangName ), g_SlangUtil.ConvertString( m_szNewName.c_str() ) );
		if( m_szNewName != szSlangName )
		{
			ErrorEdit( STR(9) );
			return;
		}
	}
	else if( m_dwEtcType == ioEtcItem::EIT_ETC_GUILD_NAME_CHANGE )
	{
		if( !g_GuildInfo.IsGuild() )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(10) );
			HideWnd();
			return;
		}
		else if( !g_GuildInfo.IsGuildMaster() )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(11) );
			HideWnd();
			return;
		}

		if( !g_App.IsRightID( m_szNewName.c_str() ) )
		{
			ErrorEdit( STR(12) );
			return;
		}

		ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
		if( pLocal )
		{
			if( !pLocal->IsRightNewID( m_szNewName.c_str() ) )
			{
				ErrorEdit( STR(13) );
				return;
			}
		}

		ioHashString szNotMakeString;
		if( g_App.IsNotMakeID( m_szNewName.c_str(), szNotMakeString ) )
		{
			char szError[MAX_PATH] = "";
			SafeSprintf( szError, sizeof( szError ), STR(14), szNotMakeString.c_str() );
			ErrorEdit( szError );
			return;
		}

		char szSlangName[MAX_PATH]="";
		StringCbPrintf( szSlangName, sizeof( szSlangName ), g_SlangUtil.ConvertString( m_szNewName.c_str() ) );
		if( m_szNewName != szSlangName )
		{
			ErrorEdit( STR(15) );
			return;
		}

		if( g_SlangUtil.IsLimitedGuildName( m_szNewName.c_str() ) ) 
		{
			ErrorEdit( STR(16) );
			return;
		}

		ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
		if( !pUserEtcItem || !pUserEtcItem->IsCanUse( ioEtcItem::EIT_ETC_GUILD_NAME_CHANGE ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(17) );
			return;
		}
	}

	ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );	
	kPrinter[0].SetTextColor( TCT_DEFAULT_GREEN );
	kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(18), m_szNewName.c_str() );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );	
	kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(19) );

	if( m_dwEtcType == ioEtcItem::EIT_ETC_CHANGE_ID )
		g_GUIMgr.SetPrevMsgListBoxWithTitle( NULL, MB_YESNO, this, kPrinter, STR(20), STR(21), STR(22), STR(23) );
	else
		g_GUIMgr.SetPrevMsgListBoxWithTitle( NULL, MB_YESNO, this, kPrinter, STR(24), STR(25), STR(26), STR(27) );

	if( pEdit )
	{
		pEdit->KillKeyFocus();
	}	
}

void ChangeNameWnd::SendNewName()
{
	if( m_dwEtcType == ioEtcItem::EIT_ETC_CHANGE_ID )
	{
		SP2Packet kPacket( CTPK_ETCITEM_USE );
		kPacket << (int)m_dwEtcType;
		kPacket << m_szNewName;
		TCPNetwork::SendToServer( kPacket );
		TCPNetwork::MouseBusy( true );
	}
	else if( m_dwEtcType == ioEtcItem::EIT_ETC_GUILD_NAME_CHANGE )
	{
		SP2Packet kPacket( CTPK_ETCITEM_USE );
		kPacket << (int)ioEtcItem::EIT_ETC_GUILD_NAME_CHANGE;
		kPacket << g_GuildInfo.GetGuildIndex() << m_szNewName;
		TCPNetwork::SendToServer( kPacket );
		TCPNetwork::MouseBusy( true );
	}
}

void ChangeNameWnd::ErrorEdit( const char *szError )
{
	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_CHANGE_EDIT );
	if( pEdit )
	{
		m_szDefaultNotice = szError;

		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString( "" );
		pEdit->KillKeyFocus();
		m_bRenderNotice = true;
	}	
}

void ChangeNameWnd::OnRender()
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


	m_Info[0].PrintFullText( iXPos+INFO_X, iYPos+INFO_Y, TAT_LEFT );
	m_Info[1].PrintFullText( iXPos+INFO_X, iYPos+INFO_Y+INFO_GAP, TAT_LEFT );


	if( m_pEditFrm )
		m_pEditFrm->Render( iXPos, iYPos );

	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_CHANGE_EDIT );
	if( pEdit )
	{
		if( m_bRenderNotice && pEdit != ioEdit::m_pKeyFocusEdit )
		{
			ioComplexStringPrinter kPrinter;
			kPrinter.SetBkColor( 0, 0, 0 );
			kPrinter.SetTextStyle( TS_NORMAL );
			kPrinter.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
			kPrinter.AddTextPiece( FONT_SIZE_12, STR(2), m_szDefaultNotice.c_str() );
			kPrinter.PrintFullText( iXPos + EDIT_TEXT_X, iYPos + EDIT_TEXT_Y, TAT_LEFT );
			kPrinter.ClearList();
		}
		else
		{
			m_bRenderNotice = false;
		}
	}
}

void ChangeNameWnd::PrintManual( int a_iXPos, int a_iYPos , float fScale )
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
ChangeNameResultWnd::ChangeNameResultWnd(void)
{
	m_pIcon = NULL;
	m_pSubIcon = NULL;
	m_pBackEffect = NULL;
}

ChangeNameResultWnd::~ChangeNameResultWnd(void)
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );
	SAFEDELETE( m_pBackEffect );
}

void ChangeNameResultWnd::iwm_command( ioWnd *pWnd , int cmd, DWORD param )
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

void ChangeNameResultWnd::SetInfoAndShow( const ioHashString &szNewName )
{
	m_dwEtcType = ioEtcItem::EIT_ETC_CHANGE_ID;

	m_szPreName = g_MyInfo.GetPublicID();
	m_szNewName = szNewName;

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

void ChangeNameResultWnd::SetInfoAndShow( const ioHashString &szPreName, const ioHashString &szNewName )
{
	m_dwEtcType = ioEtcItem::EIT_ETC_GUILD_NAME_CHANGE;
	
	m_szPreName = szPreName;
	m_szNewName = szNewName;

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

void ChangeNameResultWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "back_effect" )
	{
		SAFEDELETE( m_pBackEffect );
		m_pBackEffect = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void ChangeNameResultWnd::OnRender()
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
	if( m_dwEtcType == ioEtcItem::EIT_ETC_CHANGE_ID )
		g_FontMgr.PrintText( iXPos+TITLE_X, iYPos+TITLE_Y, FONT_SIZE_17, STR(1) );
	else
		g_FontMgr.PrintText( iXPos+TITLE_X, iYPos+TITLE_Y, FONT_SIZE_17, STR(2) );

	// Last
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
	g_FontMgr.PrintText( iXPos+FINAL_DESC_X, iYPos+FINAL_DESC_Y, FONT_SIZE_17, STR(3) );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos+LIST_X, iYPos+LIST_Y, FONT_SIZE_13, STR(4) );

	g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
	g_FontMgr.PrintText( iXPos+LIST_X, iYPos+LIST_Y+LIST_GAP, FONT_SIZE_13, STR(5) );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_RIGHT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintTextWidthCut( iXPos+RIGHT_X, iYPos+LIST_Y, FONT_SIZE_13, 196.0f, STR(6), m_szNewName.c_str() );

	g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
	g_FontMgr.PrintTextWidthCut( iXPos+RIGHT_X, iYPos+LIST_Y+LIST_GAP, FONT_SIZE_13, 196.0f, STR(7), m_szPreName.c_str() );
}

