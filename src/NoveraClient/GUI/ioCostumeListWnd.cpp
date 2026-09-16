
#include "StdAfx.h"
#include "../ioDateHelp.h"

#include "ioCostumeListWnd.h"
#include "ioExtraItemListWnd.h"

//////////////////////////////////////////////////////////////////////////
ioCostumeListWnd::ioCostumeListWnd()
{
	m_nMaxPage = 0;
	m_nCurPage = 0;
	m_nMachineCode = 0;

	m_dwCurSubBtn = ID_ALL_BTN;

	m_vCurList.clear();
}

ioCostumeListWnd::~ioCostumeListWnd()
{
}

void ioCostumeListWnd::ShowItemList( int nMachineCode )
{
	ShowWnd();

	m_nMachineCode = nMachineCode;

	ChangeSubBtn( ID_ALL_BTN );

	HideChildWnd( ID_WEAPON_BTN );
}

void ioCostumeListWnd::ChangeSubBtn( DWORD dwSubBtn )
{
	m_dwCurSubBtn = dwSubBtn;
	CheckRadioButton( ID_ALL_BTN, ID_CLOAK_BTN, m_dwCurSubBtn );

	CostumeShopSubInfoList vTotalList;
	g_CostumeShopInfoMgr.GetCostumeItemList( m_nMachineCode, vTotalList );

	m_vCurList.clear();
	if( vTotalList.empty() )
		return;

	int nSlot = m_dwCurSubBtn - ID_ALL_BTN - 1;

	CostumeShopSubInfoList::iterator iter = vTotalList.begin();
	while( iter != vTotalList.end() )
	{
		CostumeShopSubInfo &rkInfo = (*iter);

		int nCostumeSlot = rkInfo.m_nCostumeCode/DEFAULT_BASIC_COSTUME_CODE;

		if( nSlot == -1 || nSlot == nCostumeSlot )
			m_vCurList.push_back( rkInfo );

		++iter;
	}

	if( m_vCurList.empty() )
		return;

	std::sort( m_vCurList.begin(), m_vCurList.end(), CostumeShopSubInfoSort() );

	int nTotalCnt = m_vCurList.size();
	m_nMaxPage = nTotalCnt / ICON_BTN_MAX;

	if( nTotalCnt%ICON_BTN_MAX > 0 )
		m_nMaxPage += 1;

	UpdatePage( 0 );
}

void ioCostumeListWnd::UpdatePage( int nPage )
{
	m_nCurPage = nPage;

	char szBuf[MAX_PATH] = "";
	wsprintf_e( szBuf, "%d/%d", m_nCurPage+1, m_nMaxPage );
	m_szPageText = szBuf;

	SetChildActive( ID_PRE_BTN );
	SetChildActive( ID_NEXT_BTN );

	if( m_nCurPage == 0 )
		SetChildInActive( ID_PRE_BTN );
	
	if( m_nCurPage == m_nMaxPage-1 )
		SetChildInActive( ID_NEXT_BTN );

	UpdateIconBtn();
}

void ioCostumeListWnd::UpdateIconBtn()
{
	int nStartArray = m_nCurPage * ICON_BTN_MAX;

	for( int i=0; i<ICON_BTN_MAX; ++i )
	{
		ItemListIconBtn *pBtn = dynamic_cast< ItemListIconBtn* >(FindChildWnd(ID_ICON_BTN1+i));
		if( !pBtn ) 
			continue;

		CostumeShopSubInfo kInfo;

		int nCurArray = nStartArray + i;
		if( COMPARE( nCurArray, 0, (int)m_vCurList.size() ) )
		{
			kInfo = m_vCurList[nCurArray];
			pBtn->SetActive();
		}
		else
			pBtn->SetInActive();

		CostumeInfo sCostumeInfo;		
		if( g_CostumeInfoMgr.GetCostumeInfo( kInfo.m_nCostumeCode, sCostumeInfo ) )
		{
			pBtn->SetName( sCostumeInfo.m_szName );
			pBtn->SetItemCode( kInfo.m_nCostumeCode );
			pBtn->SetMarkType( kInfo.m_nShopMarkType );

			if( 0 < kInfo.m_nLimitDate )
			{
				CTime kLimitTime;
				CTime kCurrTime = DateHelp::GetCurrTime();
				DateHelp::ConvertCTime( kInfo.m_nLimitDate, kLimitTime );
				CTimeSpan TimeGap = kLimitTime - kCurrTime;
				pBtn->SetLimitDay( TimeGap.GetDays() );
			}

			if( !sCostumeInfo.m_szIconName.IsEmpty() )
				pBtn->SetIcon(  g_UIImageSetMgr.CreateImageByFullName( sCostumeInfo.m_szIconName ) );
			pBtn->SetTopSubIconType( NewShopIconBtn::TSIT_COSTUME );
		}
		else
		{
			ioHashString sName = "-";
			pBtn->SetName( sName );
		}
	}
}

void ioCostumeListWnd::OnRender()
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
	kPrinter[1].AddTextPiece( FONT_SIZE_13, " " );

	kPrinter[1].SetTextStyle( TS_NORMAL);
	kPrinter[1].SetBkColor( 0, 0, 0 );	
	kPrinter[1].SetTextColor( TCT_DEFAULT_RED ); 
	kPrinter[1].AddTextPiece( FONT_SIZE_13, " " );

	kPrinter[1].SetTextStyle( TS_NORMAL);
	kPrinter[1].SetBkColor( 0, 0, 0 );	
	kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY ); 
	kPrinter[1].AddTextPiece( FONT_SIZE_13, " " );
	
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

void ioCostumeListWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_PRE_BTN:
		if( cmd == IOBN_BTNUP && !pWnd->HasWndStyle( IWS_INACTIVE ) )
		{
			int nNewPage = m_nCurPage - 1;
			UpdatePage( nNewPage );
		}
		break;
	case ID_NEXT_BTN:
		if( cmd == IOBN_BTNUP && !pWnd->HasWndStyle( IWS_INACTIVE ) )
		{
			int nNewPage = m_nCurPage + 1;
			UpdatePage( nNewPage );
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