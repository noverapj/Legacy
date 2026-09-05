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
#include "../io3DEngine/ioStringManager.h"

#include "../DirectInput.h"
#include "../TextColorTable.h"
#include "../NetworkWrappingFunc.h"

#include "../WndID.h"
#include "../ioMyInfo.h"

#include "ioSP2GUIManager.h"
#include "GashaponPresentListWnd.h"

#include <strsafe.h>
#include "../ioPresentMgr.h"
#include "../ioDecorationPrice.h"
#include "../ioEtcItemManager.h"
#include "../ioItemMaker.h"
#include "../ioExtraItemInfoManager.h"
#include "../ioMedalItemInfoManager.h"

GashaponPresentListBtn::GashaponPresentListBtn()
{
}

GashaponPresentListBtn::~GashaponPresentListBtn()
{
	m_szNameList.clear();
}
void GashaponPresentListBtn::OnRender()
{
	NewShopIconBtn::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );

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

void GashaponPresentListBtn::AddName( const ioHashString &rName )
{
	m_szNameList.push_back( rName.c_str() );
}

void GashaponPresentListBtn::ClearName()
{
	m_szNameList.clear();
}

//////////////////////////////////////////////////////////////////////////

IntVec GashaponPresentListWnd::m_vPresentTypeList;
IntVec GashaponPresentListWnd::m_vPresentValue1List;
IntVec GashaponPresentListWnd::m_vPresentValue2List;
IntVec GashaponPresentListWnd::m_vPresentValue3List;
IntVec GashaponPresentListWnd::m_vPresentValue4List;

GashaponPresentListWnd::GashaponPresentListWnd()
{
	m_iMaxPage = 0;
	m_iCurPage = 0;

	m_vGashponPresentInfoVec.clear();
}

GashaponPresentListWnd::~GashaponPresentListWnd()
{
	m_vGashponPresentInfoVec.clear();
}

void GashaponPresentListWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_vGashponPresentInfoVec.clear();
	int iMax = xElement.GetIntAttribute_e( "Max_Present" );
	for (int i = 0; i < iMax ; i++)
	{
		GashaponPresentInfo kInfo;
		char szName[MAX_PATH]="";
		StringCbPrintf_e( szName, sizeof( szName ), "Icon_Name_%d", i+1 );
		kInfo.m_szIconName = xElement.GetStringAttribute( szName );
		if( kInfo.m_szIconName == "" )
			continue;

		StringCbPrintf_e( szName, sizeof( szName ), "SubIcon_Name_%d", i+1 );
		kInfo.m_szSubIconName = xElement.GetStringAttribute( szName );

		ZeroMemory( szName, sizeof( szName ) );
		StringCbPrintf_e( szName, sizeof( szName ), "Present_Name1_%d", i+1 );
		kInfo.m_szPresentName1 = xElement.GetStringAttribute( szName );
		if( kInfo.m_szPresentName1 == "" )
			continue;

		ZeroMemory( szName, sizeof( szName ) );
		StringCbPrintf_e( szName, sizeof( szName ), "Present_Name2_%d", i+1 );
		kInfo.m_szPresentName2 = xElement.GetStringAttribute( szName );
		if( kInfo.m_szPresentName2 == "" )
			continue;

		m_vGashponPresentInfoVec.push_back( kInfo );
	}

	SetMaxPage();
}

void GashaponPresentListWnd::UpdatePage( int iPage )
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

void GashaponPresentListWnd::UpdateIconBtn()
{
	int iStartArray = m_iCurPage * ICON_BTN_MAX;

	for( int i=0; i < ICON_BTN_MAX; ++i )
	{
		GashaponPresentListBtn *pBtn = dynamic_cast< GashaponPresentListBtn* >(FindChildWnd(ID_ICON_BTN1+i));
		if( !pBtn ) continue;

		int iCurArray = iStartArray + i;
		if( COMPARE( iCurArray, 0, (int)m_vGashponPresentInfoVec.size() ) )
		{
			pBtn->SetActive();
			pBtn->ClearName();
			pBtn->AddName( m_vGashponPresentInfoVec[iCurArray].m_szPresentName1 );
			pBtn->AddName( m_vGashponPresentInfoVec[iCurArray].m_szPresentName2 );
		
			if( !m_vGashponPresentInfoVec[iCurArray].m_szIconName.IsEmpty() )
			{
				pBtn->SetIcon(  g_UIImageSetMgr.CreateImageByFullName( m_vGashponPresentInfoVec[iCurArray].m_szIconName ) );
				pBtn->SetSubIcon(  g_UIImageSetMgr.CreateImageByFullName( m_vGashponPresentInfoVec[iCurArray].m_szSubIconName ) );
			}
		}
		else
		{
			pBtn->SetInActive();
			pBtn->ClearName();
			ioHashString szEmptyText="-";
			pBtn->AddName( szEmptyText );
		}
	}
}

void GashaponPresentListWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		PAGE_X		= 238,
		PAGE_Y		= 425,

		DESC_X		= 238,
		DESC_Y		= 470,

		LINE_GAP	= 20,

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

void GashaponPresentListWnd::iwm_show()
{
	if( !m_vPresentTypeList.empty() )
		SetGashaponList( m_vPresentTypeList, m_vPresentValue1List, m_vPresentValue2List, m_vPresentValue3List, m_vPresentValue4List );

	UpdatePage( 0 );

	CheckRadioButton( ID_ALL_BTN, ID_ALL_BTN, ID_ALL_BTN );
}

void GashaponPresentListWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	case ID_X:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	}
}

void GashaponPresentListWnd::SetMaxPage()
{
	int iTotalCnt = m_vGashponPresentInfoVec.size();
	m_iMaxPage = iTotalCnt / ICON_BTN_MAX;

	if( iTotalCnt%ICON_BTN_MAX > 0 )
		m_iMaxPage += 1;
}

void GashaponPresentListWnd::SetGashaponList( IntVec &rPresentTypeList, IntVec &rPresentValue1List, IntVec &rPresentValue2List, IntVec &rPresentValue3List, IntVec &rPresentValue4List )
{
	m_vGashponPresentInfoVec.clear();

	int iSize = rPresentTypeList.size();

	if( iSize != rPresentValue1List.size() || 
		iSize != rPresentValue2List.size() ||
		iSize != rPresentValue3List.size() ||
		iSize != rPresentValue4List.size() )
	{
		LOG.PrintTimeAndLog( 0, "%s Size Error [%d/%d/%d]", __FUNCTION__, iSize, rPresentValue1List.size(), rPresentValue2List.size() );
		return;
	}

	for (int i = 0; i < iSize ; i++)
	{
		ioHashString szIconName = g_PresentMgr.GetPresentIconText( rPresentTypeList[i], rPresentValue1List[i], rPresentValue2List[i] );
		if( szIconName.IsEmpty() )
			continue;

		GashaponPresentInfo kInfo;
		kInfo.m_szIconName = szIconName;
		kInfo.m_szSubIconName = g_PresentMgr.GetPresentSubIconText( rPresentTypeList[i], rPresentValue1List[i], rPresentValue2List[i] );
		kInfo.m_szPresentName1 = g_PresentMgr.GetPresentValue1Text( rPresentTypeList[i], rPresentValue1List[i], rPresentValue2List[i] );
		kInfo.m_szPresentName2 = g_PresentMgr.GetPresentValue2Text( rPresentTypeList[i], rPresentValue1List[i], rPresentValue2List[i] );
		m_vGashponPresentInfoVec.push_back( kInfo );
	}

	SetMaxPage();
}

void GashaponPresentListWnd::SetPresentList( IntVec &rPresentTypeList, IntVec &rPresentValue1List, IntVec &rPresentValue2List, IntVec &rPresentValue3List, IntVec &rPresentValue4List )
{
	m_vPresentTypeList   = rPresentTypeList;
	m_vPresentValue1List = rPresentValue1List;
	m_vPresentValue2List = rPresentValue2List;
	m_vPresentValue3List = rPresentValue3List;
	m_vPresentValue4List = rPresentValue4List;
}