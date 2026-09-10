

#include "StdAfx.h"

#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioStringConverter.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioUIRenderFrame.h"
#include "../io3DEngine/ioUIImageSetManager.h"
#include "../io3DEngine/ioUITitle.h"
#include "../io3DEngine/ioSoundManager.h"

#include "../io3DEngine/ioXMLDocument.h"
#include "../io3DEngine/ioXMLElement.h"

#include "../io3DEngine/ioINILoader.h"
#include "../io3DEngine/HelpFunc.h"

#include "../DirectInput.h"
#include "../TextColorTable.h"
#include "../NetworkWrappingFunc.h"

#include "../WndID.h"
#include "../ioMyInfo.h"

#include "../ioDecorationPrice.h"

#include "ioSP2GUIManager.h"
#include "NewShopWnd.h"
#include "ioDecoItemListWnd.h"

#include <strsafe.h>
#include "ioExtraItemListWnd.h"
#include "../io3DEngine/ioStringManager.h"

ioDecoItemListWnd::ioDecoItemListWnd()
{
	m_iMaxPage = 0;
	m_iCurPage = 0;
	
	m_bFemale = false;

	m_dwCurSubBtn = ID_ALL_BTN;

	m_vCurList.clear();
}

ioDecoItemListWnd::~ioDecoItemListWnd()
{
}

void ioDecoItemListWnd::iwm_create()
{
	m_vMaleDecoInfoList.clear();
	m_vFemaleDecoInfoList.clear();

	ioINILoader_e kItemLoader( "config/sp2_decoitem_list.ini" );

	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";

	// Male
	kItemLoader.SetTitle_e( "DecoMan" );

	int iCnt = kItemLoader.LoadInt_e( "item_cnt", 0 );
	for( int i=0; i < iCnt; ++i )
	{
		DecoItemListInfo kInfo;

		wsprintf_e( szKey, "item%d_code", i+1 );
		kInfo.m_iTypeCode = kItemLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "item%d_name", i+1 );
		kItemLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_Name = szBuf;

		wsprintf_e( szKey, "item%d_icon", i+1 );
		kItemLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_Icon = szBuf;

		wsprintf_e( szKey, "item%d_shop_order", i+1 );
		kInfo.m_iShopOrder = kItemLoader.LoadInt( szKey, 999999999 ); // ORDER_DEFAULT 

		wsprintf_e( szKey, "item%d_shop_mark_type", i+1 );
		kInfo.m_iShopMarkType = kItemLoader.LoadInt( szKey, 0 );         // SHOP_MARK_TYPE_NONE

		wsprintf_e( szKey, "item%d_active", i+1 );
		if( !kItemLoader.LoadBool( szKey, true ) )
			continue;

		m_vMaleDecoInfoList.push_back( kInfo );
	}

	// Female
	kItemLoader.SetTitle_e( "DecoWoman" );

	iCnt = kItemLoader.LoadInt_e( "item_cnt", 0 );
	for( int i=0; i < iCnt; ++i )
	{
		DecoItemListInfo kInfo;

		wsprintf_e( szKey, "item%d_code", i+1 );
		kInfo.m_iTypeCode = kItemLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "item%d_name", i+1 );
		kItemLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_Name = szBuf;

		wsprintf_e( szKey, "item%d_icon", i+1 );
		kItemLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_Icon = szBuf;

		wsprintf_e( szKey, "item%d_shop_order", i+1 );
		kInfo.m_iShopOrder = kItemLoader.LoadInt( szKey, 999999999 ); // ORDER_DEFAULT 

		wsprintf_e( szKey, "item%d_shop_mark_type", i+1 );
		kInfo.m_iShopMarkType = kItemLoader.LoadInt( szKey, 0 );         // SHOP_MARK_TYPE_NONE

		wsprintf_e( szKey, "item%d_active", i+1 );
		if( !kItemLoader.LoadBool( szKey, true ) )
			continue;

		m_vFemaleDecoInfoList.push_back( kInfo );
	}
}

void ioDecoItemListWnd::ShowItemList( bool bFemale )
{
	ShowWnd();

	m_bFemale = bFemale;

	ChangeSubBtn( ID_ALL_BTN );
}

void ioDecoItemListWnd::ChangeSubBtn( DWORD dwSubBtn )
{
	m_dwCurSubBtn = dwSubBtn;
	CheckRadioButton( ID_ALL_BTN, ID_UNDERWEAR_BTN, m_dwCurSubBtn );

	m_vCurList.clear();

	if( m_bFemale && m_vFemaleDecoInfoList.empty() )
		return;

	if( !m_bFemale && m_vMaleDecoInfoList.empty() )
		return;


	DecoItemListInfoVector::iterator iter = m_vMaleDecoInfoList.begin();
	DecoItemListInfoVector::iterator iter_e = m_vMaleDecoInfoList.end();

	if( m_bFemale )
	{
		iter = m_vFemaleDecoInfoList.begin();
		iter_e = m_vFemaleDecoInfoList.end();
	}

	while( iter != iter_e )
	{
		DecoItemListInfo kInfo = (*iter);

		bool bAdd = false;
		int iDecoType = kInfo.m_iTypeCode;

		switch( m_dwCurSubBtn )
		{
		case ID_ALL_BTN:
			bAdd = true;
			break;
		case ID_KINDRED_BTN:
			if( iDecoType == UID_KINDRED )
				bAdd = true;
			break;
		case ID_HAIR_BTN:
			if( iDecoType == UID_HAIR )
				bAdd = true;
			break;
		case ID_HAIR_COLOR_BTN:
			if( iDecoType == UID_HAIR_COLOR )
				bAdd = true;
			break;
		case ID_FACE_BTN:
			if( iDecoType == UID_FACE )
				bAdd = true;
			break;
		case ID_SKIN_COLOR_BTN:
			if( iDecoType == UID_SKIN_COLOR )
				bAdd = true;
			break;
		case ID_UNDERWEAR_BTN:
			if( iDecoType == UID_UNDERWEAR )
				bAdd = true;
			break;
		}

		if( bAdd )
		{
			m_vCurList.push_back( kInfo );
		}

		++iter;
	}

	std::sort( m_vCurList.begin(), m_vCurList.end(), DecoItemListSort() );

	int iTotalCnt = m_vCurList.size();
	m_iMaxPage = iTotalCnt / ICON_BTN_MAX;

	if( iTotalCnt%ICON_BTN_MAX > 0 )
		m_iMaxPage += 1;

	UpdatePage( 0 );
}

void ioDecoItemListWnd::UpdatePage( int iPage )
{
	m_iCurPage = iPage;

	char szBuf[MAX_PATH] = "";
	wsprintf_e( szBuf, "%d/%d", m_iCurPage+1, m_iMaxPage );
	m_szPageText = szBuf;

	SetChildActive( ID_PRE_BTN );
	SetChildActive( ID_NEXT_BTN );

	if( m_iCurPage == 0 )
		SetChildInActive( ID_PRE_BTN );
	
	if( m_iCurPage == m_iMaxPage-1 )
		SetChildInActive( ID_NEXT_BTN );

	UpdateIconBtn();
}

void ioDecoItemListWnd::UpdateIconBtn()
{
	int iStartArray = m_iCurPage * ICON_BTN_MAX;

	for( int i=0; i < ICON_BTN_MAX; ++i )
	{
		ItemListIconBtn *pBtn = dynamic_cast< ItemListIconBtn* >(FindChildWnd(ID_ICON_BTN1+i));
		if( !pBtn ) continue;

		DecoItemListInfo kInfo;
		kInfo.m_Icon = "";
		kInfo.m_Name = "-";

		int iCurArray = iStartArray + i;
		if( COMPARE( iCurArray, 0, (int)m_vCurList.size() ) )
		{
			kInfo = m_vCurList[iCurArray];
			pBtn->SetActive();
		}
		else
		{
			pBtn->SetInActive();
		}

		pBtn->SetName( kInfo.m_Name );
		pBtn->SetItemCode( 0 );
		pBtn->SetMarkType( kInfo.m_iShopMarkType );
		if( !kInfo.m_Icon.IsEmpty() )
			pBtn->SetIcon(  g_UIImageSetMgr.CreateImageByFullName( kInfo.m_Icon ) );
	}
}

void ioDecoItemListWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		DESC_X		= 238,
		DESC_Y		= 470,

		LINE_GAP	= 20,

		PAGE_X		= 238,
		PAGE_Y		= 425,

		MAX_TEXT    = 2,
	};

	ioComplexStringPrinter kPrinter[MAX_TEXT];
	kPrinter[0].SetTextStyle( TS_NORMAL);
	kPrinter[0].SetBkColor( 0, 0, 0 );	
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); 
	kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(1) );

	kPrinter[0].SetTextStyle( TS_NORMAL);
	kPrinter[0].SetBkColor( 0, 0, 0 );	
	kPrinter[0].SetTextColor( TCT_DEFAULT_RED ); 
	kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(2) );

	kPrinter[0].SetTextStyle( TS_NORMAL);
	kPrinter[0].SetBkColor( 0, 0, 0 );	
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); 
	kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(3) );

	kPrinter[1].SetTextStyle( TS_NORMAL);
	kPrinter[1].SetBkColor( 0, 0, 0 );	
	kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY ); 
	kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(4) );

	kPrinter[1].SetTextStyle( TS_NORMAL);
	kPrinter[1].SetBkColor( 0, 0, 0 );	
	kPrinter[1].SetTextColor( TCT_DEFAULT_RED ); 
	kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(5) );

	kPrinter[1].SetTextStyle( TS_NORMAL);
	kPrinter[1].SetBkColor( 0, 0, 0 );	
	kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY ); 
	kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(6) );


	for (int i = 0; i < MAX_TEXT ; i++)
	{	
		kPrinter[i].PrintFullText( iXPos + DESC_X, iYPos + DESC_Y + ( i*LINE_GAP), TAT_CENTER );
	}
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos+PAGE_X, iYPos+PAGE_Y, FONT_SIZE_13, m_szPageText.c_str() );
}

void ioDecoItemListWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_PRE_BTN:
		if( cmd == IOBN_BTNUP && !pWnd->HasWndStyle( IWS_INACTIVE ) )
		{
			int iNewPage = m_iCurPage - 1;
			UpdatePage( iNewPage );
		}
		break;
	case ID_NEXT_BTN:
		if( cmd == IOBN_BTNUP && !pWnd->HasWndStyle( IWS_INACTIVE ) )
		{
			int iNewPage = m_iCurPage + 1;
			UpdatePage( iNewPage );
		}
		break;
	case ID_ALL_BTN:
	case ID_KINDRED_BTN:
	case ID_HAIR_BTN:
	case ID_HAIR_COLOR_BTN:
	case ID_FACE_BTN:
	case ID_SKIN_COLOR_BTN:
	case ID_UNDERWEAR_BTN:
		if( cmd == IOBN_BTNDOWN )
		{
			ChangeSubBtn( dwID );
		}
		break;
	case ID_X:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////

