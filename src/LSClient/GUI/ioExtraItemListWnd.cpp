

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

#include "../DirectInput.h"
#include "../TextColorTable.h"
#include "../NetworkWrappingFunc.h"

#include "../WndID.h"
#include "../ioMyInfo.h"
#include "../ioDateHelp.h"

#include "ioSP2GUIManager.h"
#include "ioExtraItemListWnd.h"

#include <strsafe.h>
#include "../io3DEngine/ioStringManager.h"

//////////////////////////////////////////////////////////////////////////

ItemListIconBtn::ItemListIconBtn()
{
	m_iItemCode = 0;
}

ItemListIconBtn::~ItemListIconBtn()
{
	m_szNameList.clear();
}

void ItemListIconBtn::OnRender()
{
	NewShopIconBtn::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	/*
	int iCheckNum = m_iItemCode % 10000;
	if( iCheckNum > 1000 )
		g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
	*/

	enum { X_TEXT_OFFSET = 32, Y_TEXT_OFFSET = 79, Y_OFFSET_LING_GAP = 18, Y_TEXT_OFFSET_ADD = -8, Y_OFFSET_LING_GAP_ADD = -4,  };

	int iYAdd    = 0;
	int iYGapAdd = 0;
	int iSize = m_szNameList.size();
	if( iSize >= 2 )
	{
		iYAdd    = Y_TEXT_OFFSET_ADD;
		iYGapAdd = Y_OFFSET_LING_GAP_ADD; 
	}

	for(int i = 0;i < iSize;i++)
	{
		g_FontMgr.PrintTextWidthCut( iXPos+X_TEXT_OFFSET, iYPos+Y_TEXT_OFFSET+iYAdd+( i*(Y_OFFSET_LING_GAP+iYGapAdd) ), FONT_SIZE_13, 104.0f, m_szNameList[i].c_str() );
	}
}

void ItemListIconBtn::SetName( const ioHashString &rName )
{
	SplitDescription( rName );
}

void ItemListIconBtn::SplitDescription( const ioHashString &szName )
{
	m_szNameList.clear();
	m_szNameList = ioStringConverter::Split( szName.c_str(), "#" );
}
//////////////////////////////////////////////////////////////////////////
ioExtraItemListWnd::ioExtraItemListWnd()
{
	m_iMaxPage = 0;
	m_iCurPage = 0;
	m_iMachineCode = 0;

	m_dwCurSubBtn = ID_ALL_BTN;

	m_vCurList.clear();
}

ioExtraItemListWnd::~ioExtraItemListWnd()
{
}

void ioExtraItemListWnd::ShowItemList( int iMachineCode )
{
	ShowWnd();

	m_iMachineCode = iMachineCode;

	ChangeSubBtn( ID_ALL_BTN );
}

void ioExtraItemListWnd::ChangeSubBtn( DWORD dwSubBtn )
{
	m_dwCurSubBtn = dwSubBtn;
	CheckRadioButton( ID_ALL_BTN, ID_CLOAK_BTN, m_dwCurSubBtn );

	ItemListInfoSortVector vTotalList;
	g_ExtraItemInfoMgr.GetRandomItemList( m_iMachineCode, vTotalList );

	m_vCurList.clear();
	if( vTotalList.empty() )
		return;

	int iSlot = m_dwCurSubBtn - ID_ALL_BTN - 1;

	ItemListInfoSortVector::iterator iter = vTotalList.begin();
	while( iter != vTotalList.end() )
	{
		ItemListInfoSort kInfo;

		kInfo = (*iter);

		int iItemSlot = kInfo.m_iItemCode / DEFAULT_BASIC_ITEM_CODE;

		if( iSlot == -1 || iSlot == iItemSlot )
			m_vCurList.push_back( kInfo );

		++iter;
	}

	if( m_vCurList.empty() )
		return;

	std::sort( m_vCurList.begin(), m_vCurList.end(), ShopExtraItemSort() );

	int iTotalCnt = m_vCurList.size();
	m_iMaxPage = iTotalCnt / ICON_BTN_MAX;

	if( iTotalCnt%ICON_BTN_MAX > 0 )
		m_iMaxPage += 1;

	UpdatePage( 0 );
}

void ioExtraItemListWnd::UpdatePage( int iPage )
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

void ioExtraItemListWnd::UpdateIconBtn()
{
	int iStartArray = m_iCurPage * ICON_BTN_MAX;

	for( int i=0; i < ICON_BTN_MAX; ++i )
	{
		ItemListIconBtn *pBtn = dynamic_cast< ItemListIconBtn* >(FindChildWnd(ID_ICON_BTN1+i));
		if( !pBtn ) continue;

		ItemListInfoSort kInfo;

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

		const ioItem *pItem = g_ItemMaker.GetItemConst( kInfo.m_iItemCode, __FUNCTION__ );
		if( pItem )
		{
			pBtn->SetName( pItem->GetName() );
			pBtn->SetItemCode( kInfo.m_iItemCode );
			pBtn->SetMarkType( kInfo.m_iShopMarkType );
			if( !pItem->GetItemLargeIconName().IsEmpty() )
				pBtn->SetIcon(  g_UIImageSetMgr.CreateImageByFullName( pItem->GetItemLargeIconName(), 0 ) );
			pBtn->SetGradeType( pItem->GetGradeType() );
		}
		else
		{
			ioHashString sName = "-";
			pBtn->SetName( sName );
		}
	}
}

void ioExtraItemListWnd::OnRender()
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

void ioExtraItemListWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	case ID_WEAPON_BTN:
	case ID_ARMOR_BTN:
	case ID_HELMET_BTN:
	case ID_CLOAK_BTN:
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

