#include "StdAfx.h"
#include "SelectExtraGashaponWnd.h"
#include "ioExtraItemListWnd.h"

#include "../Channeling/ioChannelingNodeParent.h"
#include "../ioVoiceChat/Fourier.h"
#include "../ioExtendSoundManager.h"

SelectExtraIconBtn::SelectExtraIconBtn()
{
	m_pIcon       = NULL;
	m_pIconBack   = NULL;

	m_fScale = FLOAT1;
	m_fBackScale = FLOAT1;
	m_nGradeType = 0;
}

SelectExtraIconBtn::~SelectExtraIconBtn()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pIconBack );
}

void SelectExtraIconBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );	
	}	
}

void SelectExtraIconBtn::OnDrawNormal( int iXPos, int iYPos )
{
	ioButton::OnDrawNormal( iXPos, iYPos );

	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
}

void SelectExtraIconBtn::OnDrawOvered( int iXPos, int iYPos )
{
	ioButton::OnDrawOvered( iXPos, iYPos );

	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
	RenderIcon( iXPos, iYPos , UI_RENDER_SCREEN );
}

void SelectExtraIconBtn::OnDrawPushed( int iXPos, int iYPos )
{
	ioButton::OnDrawPushed( iXPos, iYPos );

	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
	RenderIcon( iXPos, iYPos , UI_RENDER_SCREEN );
}

void SelectExtraIconBtn::OnDrawDisabled( int iXPos, int iYPos )
{
	ioButton::OnDrawDisabled( iXPos, iYPos );

	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
}

void SelectExtraIconBtn::RenderBack( int iXPos, int iYPos )
{
	int iNewXPos = iXPos + GetWidth() / 2;
	int iNewYPos = iYPos + GetHeight() / 2;

	// Icon
	if( m_pIconBack )
	{
		m_pIconBack->SetScale( m_fBackScale );

		if( m_fBackScale != FLOAT1 )
			m_pIconBack->Render( iNewXPos, iNewYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		else
			m_pIconBack->Render( iNewXPos, iNewYPos, UI_RENDER_MULTIPLY );
	}
}

void SelectExtraIconBtn::RenderIcon( int iXPos, int iYPos, UIRenderType eType )
{
	enum 
	{
		X_TEXT_OFFSET   = 20,
		Y_TEXT_OFFSET   = 2,
	};

	int iNewXPos = iXPos + GetWidth() / 2;
	int iNewYPos = iYPos + GetHeight() / 2;

	if( m_pIcon )
	{
		m_pIcon->SetScale( m_fScale );

		UIRenderType eNewType = eType;
		if( HasWndStyle( IWS_INACTIVE ) )
			eNewType = UI_RENDER_GRAY;

		if ( eNewType != UI_RENDER_SCREEN )
			g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iNewXPos, iNewYPos, m_fScale, eNewType );

		if( m_fScale != FLOAT1 )
			m_pIcon->Render( iNewXPos, iNewYPos, eNewType, TFO_BILINEAR );
		else
			m_pIcon->Render( iNewXPos, iNewYPos, eNewType );
	}
}

bool SelectExtraIconBtn::SetIcon( ioUIRenderImage *pIcon, float fScale, float fBackScale )
{
	m_fScale = fScale;
	m_fBackScale = fBackScale;
	m_nGradeType = 0;

	SAFEDELETE( m_pIcon );
	if( !pIcon )
	{
		return false;
	}

	m_pIcon = pIcon;

	return true;
}
//////////////////////////////////////////////////////////////////////////
SelectExtraInfoWnd::SelectExtraInfoWnd()
{
	m_pEmptyImg = NULL;

	m_nGradeType = 0;
}

SelectExtraInfoWnd::~SelectExtraInfoWnd()
{
	ClearDesc();

	SAFEDELETE(m_pEmptyImg);
}

void SelectExtraInfoWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_SELECT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( GetParent() )
				GetParent()->iwm_command( this, cmd, dwID );
		}
		break;
	}
}

void SelectExtraInfoWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "empty_image" )
	{
		SAFEDELETE( m_pEmptyImg );
		m_pEmptyImg = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void SelectExtraInfoWnd::SetBlank()
{
	SetInActive();

	HideChildWnd( ID_ICON_BTN );
	HideChildWnd( ID_SELECT_BTN );

	ClearDesc();

	m_nGradeType = 0;
}

void SelectExtraInfoWnd::ClearDesc()
{
	for (int i = 0; i < MAX_DESC ; i++)
	{
		m_szDescArray[i].ClearList();
	}
}

bool SelectExtraInfoWnd::IsIconOver()
{
	if( g_GUIMgr.GetPreOverWnd() == FindChildWnd( ID_ICON_BTN ) )
		return true;
	return false;
}

void SelectExtraInfoWnd::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( HasWndStyle( IWS_INACTIVE ) )
	{	
		if( m_pDisableFrm )
			m_pDisableFrm->Render( iXPos, iYPos );
		else if( m_pNormalFrm )
			m_pNormalFrm->Render( iXPos, iYPos );

		// 기타 이미지 추가.
		if( m_pEmptyImg )
		{
			m_pEmptyImg->Render( iXPos+52, iYPos+49, UI_RENDER_MULTIPLY );
		}
	}
	else
	{
		ioButton::OnRender();
	}

	RenderDesc( iXPos, iYPos );
}

void SelectExtraInfoWnd::RenderDesc( int iXPos, int iYPos )
{
	enum 
	{
		X_OFFSET           = 52,
		Y_OFFSET           = 105,
		Y_LINE_GAP         = 16,
		Y_OFFSET_SECOND    = -8,
	};

	iXPos += X_OFFSET;
	iYPos += Y_OFFSET;

	if( HasWndStyle( IWS_INACTIVE ) )
	{
		for( int i = 0 ; i < MAX_DESC ; i++ )
		{
			m_szDescArray[i].ClearList();
		}

		m_szDescArray[0].SetTextStyle( TS_NORMAL );
		m_szDescArray[0].SetBkColor( 0, 0, 0 );	
		m_szDescArray[0].SetTextColor( TCT_DEFAULT_GRAY );
		m_szDescArray[0].AddTextPiece( FONT_SIZE_13, "없음" );
		m_szDescArray[0].PrintFullTextWidthCut( iXPos, iYPos, TAT_CENTER, INVENTORY_ITEM_INFO_WIDTHCUT_SIZE );

		m_szDescArray[1].SetTextStyle( TS_NORMAL );
		m_szDescArray[1].SetBkColor( 0, 0, 0 );	
		m_szDescArray[1].SetTextColor( TCT_DEFAULT_GRAY );
		m_szDescArray[1].AddTextPiece( FONT_SIZE_11, "-" );
		m_szDescArray[1].PrintFullTextWidthCut( iXPos, iYPos+Y_LINE_GAP, TAT_CENTER, INVENTORY_ITEM_INFO_WIDTHCUT_SIZE );
	}
	else
	{
		int iDescCnt = 0;
		for( int i = 0 ; i < MAX_DESC ; i++ )
		{
			if( m_szDescArray[i].IsEmpty() )
				continue;

			iDescCnt++;
		}

		if( iDescCnt == 3 )
			iYPos += Y_OFFSET_SECOND;

		int iCnt  = 0;
		for( int i = 0 ; i < MAX_DESC ; i++ )
		{
			if( m_szDescArray[i].IsEmpty() )
				continue;

			m_szDescArray[i].PrintFullTextWidthCut( iXPos, iYPos + ( Y_LINE_GAP * iCnt ), TAT_CENTER, INVENTORY_ITEM_INFO_WIDTHCUT_SIZE );
			iCnt++;
		}
	}
}

void SelectExtraInfoWnd::SetExtraItemCode( int iExtraItemCode )
{
	SetBlank();
	SetActive();

	m_iExtraItemCode = iExtraItemCode;
	if( !SetExtraItemInfo() )
	{
		SetBlank();
		return;
	}
}

bool SelectExtraInfoWnd::SetExtraItemInfo()
{
	if( m_iExtraItemCode <= 0 )
		return false;

	ioHashString szIcon;
	ioHashString szName;

	const ioItem *pItem = g_ItemMaker.GetItemConst( m_iExtraItemCode, __FUNCTION__ );
	if( pItem )
	{
		szIcon = pItem->GetItemLargeIconName();
		szName = pItem->GetName();
		m_nGradeType = pItem->GetGradeType();
	}

	// icon btn
	ShowChildWnd( ID_ICON_BTN );

	SelectExtraIconBtn *pBtn = dynamic_cast< SelectExtraIconBtn* > ( FindChildWnd( ID_ICON_BTN ) );
	if( !pBtn )
	{
		LOG.PrintTimeAndLog( 0, "%s pBtn == NULL.", __FUNCTION__ );
	}
	else
	{
		ioUIRenderImage *pImg = NULL;
		if( !szIcon.IsEmpty() )
			pImg = g_UIImageSetMgr.CreateImageByFullName( szIcon );
		pBtn->SetIcon( pImg, FLOAT1, FLOAT1 );
		pBtn->SetGradeType( m_nGradeType );
	}

	ShowChildWnd( ID_SELECT_BTN );

	// desc
	ClearDesc();
	SetIsWidthCutDesc( 0, true );

	// Name
	m_szDescArray[0].SetTextStyle( TS_NORMAL );
	m_szDescArray[0].SetBkColor( 0, 0, 0 );	
	m_szDescArray[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 진회
	m_szDescArray[0].AddTextPiece( FONT_SIZE_13, "%s", szName.c_str() );

	m_szDescArray[1].ClearList();
	m_szDescArray[1].SetTextStyle( TS_NORMAL );
	m_szDescArray[1].SetBkColor( 0, 0, 0 );	
	m_szDescArray[1].SetTextColor( TCT_DEFAULT_BLUE );
	m_szDescArray[1].AddTextPiece( FONT_SIZE_13, "영구사용" );

	return true;
}

void SelectExtraInfoWnd::SetIsWidthCutDesc( int iArray , bool bWidthCut )
{
	if( COMPARE( iArray, 0, MAX_DESC ) )
		m_bWidthCutDescArray[iArray] = bWidthCut;
}

bool SelectExtraInfoWnd::IsWidthCutDesc( int iArray )
{
	if( COMPARE( iArray, 0, MAX_DESC ) )
		return m_bWidthCutDescArray[iArray];

	return false;
}
//////////////////////////////////////////////////////////////////////////
SelectExtraGashaponSubWnd::SelectExtraGashaponSubWnd()
{
	m_dwCurTabID   = ID_TAB_1_BTN;
	m_pEditFrm     = NULL;
	m_iMachineCode = 0;
	m_iEtcItemType = 0;
}

SelectExtraGashaponSubWnd::~SelectExtraGashaponSubWnd()
{
	m_vCurList.clear();
	SAFEDELETE( m_pEditFrm );
}

void SelectExtraGashaponSubWnd::ShowSubWnd( int iEtcItemType )
{
	ioEtcItemSelectExtraGashapon *pEtcItem = dynamic_cast< ioEtcItemSelectExtraGashapon * > ( g_EtcItemMgr.FindEtcItem( iEtcItemType ) );
	if( !pEtcItem )
		return;

	m_iEtcItemType = iEtcItemType;
	m_iMachineCode = pEtcItem->GetExtraItemMachineCode();

	ChangeTab( ID_TAB_1_BTN, "" );
	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_FIND_EDIT ) );
	if( pEdit )
	{
		pEdit->ClearString();
		pEdit->KillKeyFocus();
	}

	ShowWnd();
}

void SelectExtraGashaponSubWnd::iwm_hide()
{
	for (int i = 0; i < MAX_ITEM_INFO_WND_BTN ; i++)
		HideChildWnd( ID_ITEM_INFO_WND_1_BTN + i );

	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_FIND_EDIT ) );
	if( pEdit )
	{
		pEdit->ClearString();
		pEdit->KillKeyFocus();		
	}
}

void SelectExtraGashaponSubWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	UpdatePage( curPos );
}

void SelectExtraGashaponSubWnd::iwm_wheel( int zDelta )
{
	if( !IsShow() )
	{
		ioWnd::iwm_wheel( zDelta );
	}
	else
	{ 
		// 휠 이벤트 처리를 했다면 부모로 이벤트를 날리지 않는다.
		if( zDelta == WHEEL_DELTA )
		{
			ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
			if( pScroll )
				pScroll->SetScrollPos( m_iCurPage - 1 );

			if( g_GUIMgr.GetPreOverWnd() )
			{
				ioMouse NullMouse;
				g_GUIMgr.GetPreOverWnd()->iwm_mouseover( NullMouse );
			}
		}
		else if( zDelta == -WHEEL_DELTA )
		{
			ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
			if( pScroll )
				pScroll->SetScrollPos( m_iCurPage + 1 );

			if( g_GUIMgr.GetPreOverWnd() )
			{
				ioMouse NullMouse;
				g_GUIMgr.GetPreOverWnd()->iwm_mouseover( NullMouse );
			}
		}
	}
}

void SelectExtraGashaponSubWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_TAB_1_BTN:
	case ID_TAB_2_BTN:
	case ID_TAB_3_BTN:
	case ID_TAB_4_BTN:
	case ID_TAB_5_BTN:
		if( cmd == IOBN_BTNDOWN )
		{
			ChangeTab( dwID, "" );
		}
		break;
	case ID_ITEM_INFO_WND_1_BTN:
	case ID_ITEM_INFO_WND_2_BTN:
	case ID_ITEM_INFO_WND_3_BTN:
	case ID_ITEM_INFO_WND_4_BTN:
	case ID_ITEM_INFO_WND_5_BTN:
	case ID_ITEM_INFO_WND_6_BTN:
	case ID_ITEM_INFO_WND_7_BTN:
	case ID_ITEM_INFO_WND_8_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SelectExtraInfoWnd *pItem = dynamic_cast<SelectExtraInfoWnd*>(FindChildWnd(dwID));
			if( pItem )
			{
				SelectExtraGashaponWnd *pMainWnd = dynamic_cast<SelectExtraGashaponWnd*>(g_GUIMgr.FindWnd( SELECT_EXTRA_GASHAPON_WND ));
				if( pMainWnd )
				{
					if( param == SelectExtraInfoWnd::ID_SELECT_BTN )
					{	
						pMainWnd->SetSelectExtraItem( pItem->GetExtraItemCode() );
						HideWnd();
					}
				}				
			}
		}
		break;
	case ID_FIND_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SetFind();
		}
		break;
	}
}

void SelectExtraGashaponSubWnd::UpdatePage( int iNewPage )
{
	for (int i = 0; i < MAX_ITEM_INFO_WND_BTN ; i++)
		HideChildWnd( ID_ITEM_INFO_WND_1_BTN + i );

	m_iCurPage = iNewPage;

	int iItemSize = m_vCurList.size();
	int iStartArray = m_iCurPage * PAGE_PER_ITEM_NUM;
	for( int i = 0 ; i < MAX_ITEM_INFO_WND_BTN ; i++ )
	{
		SelectExtraInfoWnd *pItem = dynamic_cast<SelectExtraInfoWnd*>(FindChildWnd(ID_ITEM_INFO_WND_1_BTN + i));
		if( !pItem )
			continue;

		pItem->ShowWnd();

		int iCurArray = iStartArray +i;
		if( !COMPARE( iCurArray, 0, iItemSize ) )
		{
			// 초기화
			pItem->SetExtraItemCode( 0 );
			continue;
		}

		pItem->SetExtraItemCode( m_vCurList[iCurArray].m_iItemCode );
	}
}

void SelectExtraGashaponSubWnd::ChangeTab( DWORD dwID, const char *szFind  )
{
	if( strcmp( szFind, "" ) == 0 )
	{
		CheckRadioButton( ID_TAB_1_BTN, ID_TAB_5_BTN, dwID );
		m_dwCurTabID = dwID;
	}
	else
	{
		// 검색이 있으면 모든 탭 선택되지 않은 상태로 변경 
		for (int i = ID_TAB_1_BTN; i < ID_TAB_5_BTN+1 ; i++)
		{
			ioRadioButton *pButton = dynamic_cast<ioRadioButton*> ( FindChildWnd( i ) );
			if( !pButton )
				continue;
			pButton->SetClick( false );
		}
	}

	SetExtraItemInfoVec( m_dwCurTabID, szFind );

	m_iCurPage = 0;

	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll )
	{
		int iItemSize = m_vCurList.size();

		int iMaxPos =  iItemSize / PAGE_PER_ITEM_NUM;
		if( iItemSize % PAGE_PER_ITEM_NUM )
			iMaxPos++;

		pScroll->SetPageRange(2);
		pScroll->SetScrollRange( 0, iMaxPos );
		pScroll->SetScrollPos( m_iCurPage );
	}
}

void SelectExtraGashaponSubWnd::SetExtraItemInfoVec( DWORD dwTabID, const char *szFind )
{
	ItemListInfoSortVector vTotalList;
	g_ExtraItemInfoMgr.GetRandomItemList( m_iMachineCode, vTotalList );

	m_vCurList.clear();
	if( vTotalList.empty() )
		return;

	bool bFind = false;
	if( strcmp( szFind, "" ) != 0 )
		bFind = true;

	int iSize = vTotalList.size();
	for (int i = 0; i < iSize ; i++)
	{
		if( bFind )
		{
			const ioItem *pItem = g_ItemMaker.GetItemConst( vTotalList[i].m_iItemCode, __FUNCTION__ );
			if( pItem )
			{
				if( strstr( pItem->GetName().c_str(), szFind ) == NULL ) 
					continue;
			}	
		}
		else
		{
			int iGroup = vTotalList[i].m_iItemCode/DEFAULT_BASIC_ITEM_CODE + 1;
			if( !IsRightTab( dwTabID, iGroup ) )
				continue;
		}

		m_vCurList.push_back( vTotalList[i] );
	}

	if( m_vCurList.empty() )
		return;

	std::sort( m_vCurList.begin(), m_vCurList.end(), ShopExtraItemSort() );
}

void SelectExtraGashaponSubWnd::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioWnd::OnRender();

	enum
	{
		X_OFFSET = 17,
		Y_OFFSET = 474,
	};

	if( m_pEditFrm )
		m_pEditFrm->Render( iXPos+X_OFFSET, iYPos+Y_OFFSET );
}

int SelectExtraGashaponSubWnd::GetTabType( DWORD dwTabID )
{
	int iArray = ( dwTabID - ID_TAB_1_BTN );
	if( !COMPARE( iArray, 0, MAX_TAB_BTN ) )
		return -1;

	switch( dwTabID )
	{
	case ID_TAB_2_BTN:
		return 1;
	case ID_TAB_3_BTN:
		return 2;
	case ID_TAB_4_BTN:
		return 3;
	case ID_TAB_5_BTN:
		return 4;
	}

	return -1;
}

bool SelectExtraGashaponSubWnd::IsRightTab( DWORD dwTabID, int iValue1 )
{
	if( dwTabID == ID_TAB_1_BTN )
		return true;
	else if( iValue1 == GetTabType(dwTabID) )
		return true;

	return false;
}

void SelectExtraGashaponSubWnd::Update()
{
	UpdatePage( m_iCurPage );
}

void SelectExtraGashaponSubWnd::SetFind()
{
	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_FIND_EDIT ) );
	if( !pEdit )
	{
		LOG.PrintTimeAndLog( 0, "%s pEdit == NULL.", __FUNCTION__ );
		return;
	}

	if( strcmp( pEdit->GetText(), "" ) == 0 )
		return;

	ChangeTab( ID_TAB_1_BTN, pEdit->GetText() );
}

void SelectExtraGashaponSubWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
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

void SelectExtraGashaponSubWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	if( g_InputBox.IsEnter() )
	{
		iwm_command( FindChildWnd( ID_FIND_BTN ), IOBN_BTNUP, 0 );
	}	
}
///////////////////////////////////////////////////////////////////////////
SelectExtraGashaponBtn::SelectExtraGashaponBtn()
{
	m_pCardCover = NULL;
	m_pCoverOverAdd = NULL;
	m_pSelect = NULL;
	m_pSelectEdge = NULL;
	m_pLight  = NULL;

	m_pPresentIcon = NULL;
	m_pSubIcon = NULL;

	m_iPresentType = 0;
	m_iPresentValue1 = m_iPresentValue2 = 0;

	m_dwAniInDelay    = 0;
	m_dwAniOutDelay   = 0;
	m_dwAniWhiteDelay = 0;
	m_dwAniBackWhiteDelay = 0;
	m_dwAniBackWaitDelay  = 0;

	m_bGetCard = false;
	m_bSelect  = false;

	m_pEffect = NULL;
	m_iCurEffectArray     = -1;
	m_dwRollingEffectTime = 0;
	m_dwRollingEffectCheckTime = 0;
	m_vEndEfffect_S.clear();

	m_nGradeType = 0;
}

SelectExtraGashaponBtn::~SelectExtraGashaponBtn()
{
	SAFEDELETE( m_pCardCover );
	SAFEDELETE( m_pCoverOverAdd );
	SAFEDELETE( m_pSelect );
	SAFEDELETE( m_pSelectEdge );
	SAFEDELETE( m_pLight );

	SAFEDELETE( m_pPresentIcon );
	SAFEDELETE( m_pSubIcon );
	SAFEDELETE( m_pEffect );
}

void SelectExtraGashaponBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "CardCover" )
	{
		SAFEDELETE( m_pCardCover );
		m_pCardCover = pImage;
	}
	else if( szType == "CoverOverAdd" )
	{
		SAFEDELETE( m_pCoverOverAdd );
		m_pCoverOverAdd = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void SelectExtraGashaponBtn::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "PresentSelect" )
	{
		SAFEDELETE( m_pSelect );
		m_pSelect = pFrame;
	}
	else if( szType == "PresentLight" )
	{
		SAFEDELETE( m_pLight );
		m_pLight = pFrame;
	}
	else if( szType == "SelectEdge" )
	{
		SAFEDELETE( m_pSelectEdge );
		m_pSelectEdge = pFrame;
	}
	else
	{
		ioButton::AddRenderFrame( szType, pFrame );
	}
}

void SelectExtraGashaponBtn::SetPresent( short iPresentType, int iPresentValue1, int iPresentValue2 )
{
	// info
	m_iPresentType	= iPresentType;
	m_iPresentValue1= iPresentValue1;
	m_iPresentValue2= iPresentValue2;

	// icon
	SAFEDELETE( m_pPresentIcon );
	const ioHashString &rkPresentIcon = g_PresentMgr.GetPresentIconText( m_iPresentType, m_iPresentValue1, m_iPresentValue2 );
	m_pPresentIcon = g_UIImageSetMgr.CreateImageByFullName( rkPresentIcon );

	SAFEDELETE( m_pSubIcon );
	const ioHashString &rkSubIcon = g_PresentMgr.GetPresentSubIconText( m_iPresentType, m_iPresentValue1, m_iPresentValue2 );
	m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( rkSubIcon );

	// text
	char szName[MAX_PATH] = "";
	StringCbPrintf( szName, sizeof( szName ), g_PresentMgr.GetPresentValue1Text( m_iPresentType, m_iPresentValue1, m_iPresentValue2 ).c_str() );
	
	char szSplitText[MAX_SPLIT_LINE][MAX_PATH];
	ZeroMemory( szSplitText, sizeof( szSplitText ) );
	SplitText( szName, 62.0f, szSplitText );

	for (int i = 0; i < MAX_SPLIT_LINE ; i++)
	{
		m_szTitles[i] = szSplitText[i];
	}

	m_nGradeType = g_PresentMgr.GetGradeType( m_iPresentType, m_iPresentValue1 );
}

void SelectExtraGashaponBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	RollingEffect( fTimePerSec );
	m_AniState.UpdateTime();
	ProcessAni();
}

void SelectExtraGashaponBtn::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwAniInDelay    = xElement.GetIntAttribute_e( "AniInDelay" );
	m_dwAniOutDelay   = xElement.GetIntAttribute_e( "AniOutDelay" );
	m_dwAniWhiteDelay = xElement.GetIntAttribute_e( "AniWhiteDelay" );
	m_dwAniEndDelay   = xElement.GetIntAttribute_e( "AniEndDelay" );
	m_dwAniBackWhiteDelay = xElement.GetIntAttribute_e( "AniBackWhiteDelay" );
	m_dwAniBackWaitDelay  = xElement.GetIntAttribute_e( "AniBackWaitDelay" );
	m_dwAniCardMaxPercent = xElement.GetIntAttribute_e( "AniCardMaxPercent" );

	m_dwRollingEffectTime = xElement.GetIntAttribute_e( "Effect_Gap_Time" );

	m_vEndEfffect_S.clear();
	int iMax = xElement.GetIntAttribute_e( "Max_Success_Effect" );
	m_vEndEfffect_S.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf( szName, sizeof( szName ), "Success_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;
		m_vEndEfffect_S.push_back( szEffect );
	}

	char szKeyName[MAX_PATH]="";
	StringCbPrintf( szKeyName, sizeof( szKeyName ), "success_sound" );
	m_szSuccessSound = xElement.GetStringAttribute( szKeyName );
}

void SelectExtraGashaponBtn::iwm_create()
{
	InitAniState();
}

void SelectExtraGashaponBtn::iwm_show()
{
	SetEmptyCard();
}

void SelectExtraGashaponBtn::iwm_hide()
{
	if( !m_szSuccessSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szSuccessSound, 0 );
}

void SelectExtraGashaponBtn::InitAniState()
{
	FSMState *pState = new FSMState(AT_NONE);
	if( pState )
		pState->Add( AT_OUT,  AT_OUT );
	m_AniState.Add( pState );

	pState = new FSMState(AT_OUT);
	if( pState )
		pState->Add( AT_OUT, AT_IN, m_dwAniOutDelay );
	m_AniState.Add( pState );

	pState = new FSMState(AT_IN);
	if( pState )
		pState->Add( AT_IN, AT_WHITE, m_dwAniInDelay );
	m_AniState.Add( pState );	

	pState = new FSMState(AT_WHITE);
	if( pState )
		pState->Add( AT_WHITE, AT_END, m_dwAniWhiteDelay );
	m_AniState.Add( pState );	

	pState = new FSMState(AT_END);
	if( pState )
		pState->Add( AT_END, AT_NONE, m_dwAniEndDelay );
	m_AniState.Add( pState );	

	pState = new FSMState(AT_BACK_WHITE);
	if( pState )
	{
		pState->Add( AT_BACK_WHITE, AT_BACK_WAIT, m_dwAniBackWhiteDelay );
		pState->Add( AT_OUT,  AT_OUT );
	}
	m_AniState.Add( pState );	

	pState = new FSMState(AT_BACK_WAIT);
	if( pState )
	{
		pState->Add( AT_BACK_WAIT, AT_BACK_WHITE, m_dwAniBackWaitDelay );
		pState->Add( AT_OUT,  AT_OUT );
	}
	m_AniState.Add( pState );	

	pState = new FSMState(AT_BACK_OUT);
	if( pState )
		pState->Add( AT_BACK_OUT, AT_BACK_IN, m_dwAniOutDelay );
	m_AniState.Add( pState );

	pState = new FSMState(AT_BACK_IN);
	if( pState )
		pState->Add( AT_BACK_IN, AT_BACK_END, m_dwAniInDelay );
 	m_AniState.Add( pState );	

	pState = new FSMState(AT_BACK_END);
	if( pState )
		pState->Add( AT_NONE, AT_NONE );
	m_AniState.Add( pState );	

	m_AniState.SetCurState( AT_NONE );
}

void SelectExtraGashaponBtn::ProcessAni()
{
	if( m_AniState.GetCurState() == AT_NONE )
		return;

	enum 
	{ 
		MAX_PERCENT = 100,
		MIN_PERCENT = 10,

		X_OFFSET = 35,
		Y_OFFSET = 44,
	};

	if( m_AniState.GetCurState() == AT_BACK_WHITE )
	{
		float fRate = ( (float) m_AniState.GetCurTime() / (float) m_AniState.GetDelay() );
		int iColor = 63.0f * ( FLOAT1 - fRate );
		if( m_pCardCover )
			m_pCardCover->SetColor( iColor, iColor, iColor );
	}
	else if( m_AniState.GetCurState() == AT_BACK_WAIT )
	{
		if( m_pCardCover )
			m_pCardCover->SetColor(  255, 255, 255 );
	}
	else if( m_AniState.GetCurState() == AT_OUT )
	{
		// 감속 0->1->2
		int iPercent = sin( (PI/2 * m_AniState.GetCurTime()  ) / m_AniState.GetDelay() ) * m_dwAniCardMaxPercent;	
		float fYScale = (float) ( MAX_PERCENT - iPercent );
		fYScale *= 0.01;
		if( m_pCardCover )
		{
			m_pCardCover->SetColor(  255, 255, 255 );
			m_pCardCover->SetScale( fYScale, FLOAT1 );
		}
	}
	else if( m_AniState.GetCurState() == AT_IN )
	{
		// 가속 2->1->0
		int iPercent = cos( (PI/2 * m_AniState.GetCurTime()  ) / m_AniState.GetDelay() ) * m_dwAniCardMaxPercent;	
		float fYScale = (float) ( MAX_PERCENT - iPercent );
		fYScale *= 0.01;
		if( m_pNormal )
			m_pNormal->SetScale( fYScale, FLOAT1 );
	}
	else if( m_AniState.GetCurState() == AT_WHITE )
	{
		float fRate = ( (float) m_AniState.GetCurTime() / (float) m_AniState.GetDelay() );
		int iAlpha = 255.0f * ( FLOAT1 - fRate );
		if( m_pLight )
			m_pLight->SetAlpha( iAlpha );
		if( m_pNormal )
			m_pNormal->SetScale( FLOAT1, FLOAT1 );
	}
	else if( m_AniState.GetCurState() == AT_END )
	{
		// 초기화
		if( m_pCardCover )
		{
			m_pCardCover->SetColor(  255, 255, 255 );
			m_pCardCover->SetScale( FLOAT1, FLOAT1 );
		}
		if( m_pNormal )
			m_pNormal->SetScale( FLOAT1, FLOAT1 );
		if( m_pLight )
			m_pLight->SetAlpha( 0 );
	}
	else if( m_AniState.GetCurState() == AT_BACK_OUT )
	{
		// 감속 0->1->2
		int iPercent = sin( (PI/2 * m_AniState.GetCurTime()  ) / m_AniState.GetDelay() ) * m_dwAniCardMaxPercent;	
		float fYScale = (float) ( MAX_PERCENT - iPercent );
		fYScale *= 0.01;
		if( m_pNormal )
		{
			m_pNormal->SetScale( fYScale, FLOAT1 );
		}
	}
	else if( m_AniState.GetCurState() == AT_BACK_IN )
	{
		// 가속 2->1->0
		int iPercent = cos( (PI/2 * m_AniState.GetCurTime()  ) / m_AniState.GetDelay() ) * m_dwAniCardMaxPercent;	
		float fYScale = (float) ( MAX_PERCENT - iPercent );
		fYScale *= 0.01;
		if( m_pCardCover )
		{
			m_pCardCover->SetColor(  255, 255, 255 );
			m_pCardCover->SetScale( fYScale, FLOAT1 );
		}
		SetActive();
	}
	else if( m_AniState.GetCurState() == AT_BACK_END )
	{
		m_AniState.SetCurState( AT_NONE );
		SetEmptyCard();
		if( m_pParent )
		{
			SelectExtraGashaponWnd *pParent = dynamic_cast<SelectExtraGashaponWnd*> ( m_pParent );
			if( pParent )
				pParent->SetEmptyCardAni();
		}
	}
}

void SelectExtraGashaponBtn::SetOutAni()
{
	m_AniState.SetCurState( AT_OUT );
}

void SelectExtraGashaponBtn::SetNoneAni()
{
	m_AniState.SetCurState( AT_NONE );
}

void SelectExtraGashaponBtn::SetBackWhiteAni()
{
	m_AniState.SetCurState( AT_BACK_WHITE );
}

void SelectExtraGashaponBtn::SetBackOutAni()
{
	m_AniState.SetCurState( AT_BACK_OUT );
}
void SelectExtraGashaponBtn::SetEmptyCard()
{
	m_iPresentType = 0;
	m_bGetCard     = false;
	m_bSelect      = false;
	m_iCurEffectArray          = -1;
	m_dwRollingEffectCheckTime = 0;
	m_AniState.SetCurState( AT_BACK_WHITE );

	if( m_pCardCover )
	{
		m_pCardCover->SetColor( 255, 255, 255 );
		m_pCardCover->SetScale( FLOAT1, FLOAT1 );
	}
	if( m_pNormal )
		m_pNormal->SetScale( FLOAT1, FLOAT1 );
	if( m_pLight )
		m_pLight->SetAlpha( 0 );

	SetActive();

	for (int i = 0; i < MAX_SPLIT_LINE ; i++)
	{
		m_szTitles[i].Clear();
	}
}

void SelectExtraGashaponBtn::SetGetCard( bool bGetCard )
{
	m_bGetCard = bGetCard;
}

void SelectExtraGashaponBtn::SetSelect( bool bSelect )
{
	m_bSelect = bSelect;
}

void SelectExtraGashaponBtn::RollingEffect( float fTimePerSec )
{
	if(  m_dwRollingEffectCheckTime == 0 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( dwCurTime - m_dwRollingEffectCheckTime > m_dwRollingEffectTime )		// 교체
	{
		SAFEDELETE( m_pEffect );

		m_iCurEffectArray++;

		if( !COMPARE( m_iCurEffectArray, 0, (int)m_vEndEfffect_S.size() ) )
			m_iCurEffectArray = 0;

		ioHashString szEffect;
		szEffect = m_vEndEfffect_S[m_iCurEffectArray];

		if( !szEffect.IsEmpty() )
		{
			m_pEffect = g_UIImageSetMgr.CreateImageByFullName( szEffect );
		}

		m_dwRollingEffectCheckTime = dwCurTime;

		if( m_iCurEffectArray >= (int)m_vEndEfffect_S.size()-1 ) // end
		{
			SAFEDELETE( m_pEffect );
			m_dwRollingEffectCheckTime = 0;
			if( !m_szSuccessSound.IsEmpty() )
				g_SoundMgr.PlaySound( m_szSuccessSound, DSBVOLUME_MAX, PSM_THREAD );
		}
	}
}

void SelectExtraGashaponBtn::SetGetEffect()
{
	m_dwRollingEffectCheckTime = FRAMEGETTIME();
	m_iCurEffectArray = -1;
	SAFEDELETE( m_pEffect );
}

void SelectExtraGashaponBtn::OnDrawNormal( int iXPos, int iYPos )
{
	if( IsEmptyCard() ||  m_AniState.GetCurState() == AT_OUT || m_AniState.GetCurState() == AT_BACK_IN )
	{
		RenderCover(iXPos, iYPos, UI_RENDER_NORMAL );
		RenderCoverWhite(iXPos, iYPos, UI_RENDER_NORMAL_ADD_COLOR );
	}
	else
	{
		RenderInfo( iXPos, iYPos, UI_RENDER_NORMAL );
		RenderEffect( iXPos, iYPos, UI_RENDER_ADD );
		RenderSelect( iXPos, iYPos, UI_RENDER_ADD );
		RenderLight( iXPos, iYPos, UI_RENDER_ADD );
	}
}

void SelectExtraGashaponBtn::OnDrawOvered( int iXPos, int iYPos )
{
	OnDrawPushed( iXPos, iYPos );
}

void SelectExtraGashaponBtn::OnDrawPushed( int iXPos, int iYPos )
{
	if( IsEmptyCard() || m_AniState.GetCurState() == AT_OUT || m_AniState.GetCurState() == AT_BACK_IN )
	{
		RenderCover(iXPos, iYPos, UI_RENDER_NORMAL );
		RenderCoverOverAdd(iXPos, iYPos, UI_RENDER_ADD );
		RenderCoverWhite(iXPos, iYPos, UI_RENDER_NORMAL_ADD_COLOR );
	}
	else
	{
		RenderInfo( iXPos, iYPos, UI_RENDER_NORMAL );
		RenderEffect( iXPos, iYPos, UI_RENDER_ADD );
		RenderSelect( iXPos, iYPos, UI_RENDER_ADD );
		RenderLight( iXPos, iYPos, UI_RENDER_ADD );
	}
}

void SelectExtraGashaponBtn::OnDrawDisabled( int iXPos, int iYPos )
{
	if( IsEmptyCard() || m_AniState.GetCurState() == AT_OUT || m_AniState.GetCurState() == AT_BACK_IN )
	{
		RenderCover(iXPos, iYPos, UI_RENDER_GRAY );
	}
	else
	{
		RenderInfo( iXPos, iYPos, UI_RENDER_GRAY );
	}
}

void SelectExtraGashaponBtn::RenderCover( int iXPos, int iYPos, UIRenderType eType )
{
	enum 
	{
		X_OFFSET = 35,
		Y_OFFSET = 44,
	};

	if( m_pCardCover )
		m_pCardCover->Render( iXPos + X_OFFSET, iYPos + Y_OFFSET, eType, TFO_BILINEAR );
}

void SelectExtraGashaponBtn::RenderCoverWhite( int iXPos, int iYPos, UIRenderType eType )
{
	if( m_AniState.GetCurState() != AT_BACK_WHITE )
		return;

	enum 
	{
		X_OFFSET = 35,
		Y_OFFSET = 44,
	};

	if( m_pCardCover )
		m_pCardCover->Render( iXPos + X_OFFSET, iYPos + Y_OFFSET, eType, TFO_BILINEAR );
}

void SelectExtraGashaponBtn::RenderCoverOverAdd( int iXPos, int iYPos, UIRenderType eType )
{
	enum 
	{
		X_OFFSET = 35,
		Y_OFFSET = 44,
	};

	if( m_pCoverOverAdd )
		m_pCoverOverAdd->Render( iXPos + X_OFFSET, iYPos + Y_OFFSET, eType, TFO_BILINEAR );
}

void SelectExtraGashaponBtn::RenderEffect( int iXPos, int iYPos, UIRenderType eType )
{
	enum 
	{
		X_OFFSET = 35,
		Y_OFFSET = 29,
	};

	if( !m_pEffect )
		return;

	m_pEffect->SetScale( 1.36f );
	m_pEffect->Render( iXPos + X_OFFSET, iYPos + Y_OFFSET, eType, TFO_BILINEAR );
}

void SelectExtraGashaponBtn::RenderSelect( int iXPos, int iYPos, UIRenderType eType )
{
	if( !m_bSelect )
		return;

	if( m_pSelect )
		m_pSelect->Render( iXPos, iYPos, eType );
	if( m_pSelectEdge )
		m_pSelectEdge->Render( iXPos, iYPos );
}

void SelectExtraGashaponBtn::RenderLight( int iXPos, int iYPos, UIRenderType eType )
{
	if( m_AniState.GetCurState() != AT_WHITE )
		return;

	if( !m_pLight )
		return;

	m_pLight->Render( iXPos, iYPos, eType );
}

void SelectExtraGashaponBtn::RenderInfo( int iXPos, int iYPos, UIRenderType eType )
{
	enum 
	{
		X_OFFSET_BACK = 35,
		Y_OFFSET_BACK = 44, 

		X_OFFSET_ICON = 35,
		Y_OFFSET_ICON = 29,

		X_OFFSET_ONE_TEXT = 35,
		Y_OFFSET_ONE_TEXT = 66,

		X_OFFSET_TWO_TEXT = 35,
		Y_OFFSET_TWO_TEXT = 60,
		TWO_TEXT_LINE_GAP = 12,
	};
	// back
	if( m_pNormal )
	{
		m_pNormal->Render( iXPos + X_OFFSET_BACK, iYPos + Y_OFFSET_BACK, eType, TFO_BILINEAR );
	}

	if( m_AniState.GetCurState() == AT_OUT      || 
		m_AniState.GetCurState() == AT_IN       ||
		m_AniState.GetCurState() == AT_BACK_OUT ||
		m_AniState.GetCurState() == AT_BACK_IN )
		return;

	g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iXPos + X_OFFSET_ICON, iYPos + Y_OFFSET_ICON, 0.68f, eType );

	// icon
	if( m_pPresentIcon )
	{
		m_pPresentIcon->SetScale( 0.68f );
		m_pPresentIcon->Render( iXPos + X_OFFSET_ICON, iYPos + Y_OFFSET_ICON, eType, TFO_BILINEAR );
	}
	if( m_pSubIcon )
	{
		m_pSubIcon->SetScale( 0.68f );
		m_pSubIcon->Render( iXPos + X_OFFSET_ICON, iYPos + Y_OFFSET_ICON, eType, TFO_BILINEAR );
	}
	// text
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );

	if( eType == UI_RENDER_NORMAL )
	{
		if( ( (m_iPresentValue1/1000)%10 ) == 1 )
			g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
		else
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	}
	else if( eType == UI_RENDER_GRAY )
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
	}

	bool bTwoLines = true;
	for (int i = 0; i < MAX_SPLIT_LINE ; i++)
	{
		if( m_szTitles[i].IsEmpty() )
		{
			bTwoLines = false;
			break;
		}
	}

	if( bTwoLines )
	{
		for (int i = 0; i < MAX_SPLIT_LINE ; i++)
		{
			g_FontMgr.PrintText( iXPos + X_OFFSET_TWO_TEXT, iYPos + Y_OFFSET_TWO_TEXT + ( TWO_TEXT_LINE_GAP*i), FONT_SIZE_11, m_szTitles[i].c_str() );
		}
	}
	else
	{
		for (int i = 0; i < MAX_SPLIT_LINE-1 ; i++)
		{
			g_FontMgr.PrintText( iXPos + X_OFFSET_ONE_TEXT, iYPos + Y_OFFSET_ONE_TEXT, FONT_SIZE_11, m_szTitles[i].c_str() );
		}
	}
}

bool SelectExtraGashaponBtn::CheckHanByte( char *szText )
{
	int len = strlen(szText);
	int hbyte_count = 0;
	for(int i = 0; i < len; i++ )
	{

#if defined( SRC_OVERSEAS )

#if defined( MULTI_BYTE_CHECK )
		if( IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (BYTE)szText[i] ) )
#else
		if( false && IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (BYTE)szText[i] ) )
#endif

#else
		if( IsDBCSLeadByte( (BYTE)szText[i] ) )
#endif

		{
			i++;		
			if(len <= i) // 마지막 한글이 깨진 글자다.
				return true;
		}	
	}

	return false;
}

void SelectExtraGashaponBtn::SplitText( const char *szText, float fSplitWidth  , OUT char szSplitText[MAX_SPLIT_LINE][MAX_PATH])
{
	int  TextLen = strlen(szText);
	const char *pSrc = &szText[0];
	int  iCurLen = 0;
	for(int i = 0;i < MAX_SPLIT_LINE;i++)
	{
		if(iCurLen >= TextLen)
			break;    

		int iLineLen = 0;
		while(true)
		{
			if( iLineLen >= MAX_PATH )
				break;
			szSplitText[i][iLineLen] = *pSrc;

			float fTextW = g_FontMgr.GetTextWidth( szSplitText[i], TS_NORMAL, FONT_SIZE_11 );
			if(fTextW < fSplitWidth )
			{
				pSrc++;
				iLineLen++;
				iCurLen++;
			}
			else
			{
				szSplitText[i][iLineLen] = NULL;
				if(CheckHanByte(szSplitText[i]))          //한글을 자르면 글자가 깨지므로...
				{
					szSplitText[i][iLineLen - 1] = NULL;
					pSrc--;
					iCurLen--;
				}
				break;    //다음 라인으로 넘김.
			}
			if(iCurLen >= TextLen)
				break;                     //입력된 텍스트를 모두 검사했다.
		}
	}
}
////////////////////////////////////////////////////////////////////////////////
SelectExtraGashaponWnd::SelectExtraGashaponWnd(void)
{
	m_iEtcItemArray    = -1;
	m_iSelectItemBtnID = 0;

	for (int i = 0; i < MAX_EXTRA_ITEM_CODE; i++)
	{
		m_iExtraItemCodes[i] = 0;
	}

	m_sDescCard = "아래 10개 중 1개를 랜덤으로 획득할 수 있습니다";

	m_pLineLeft   = NULL;
	m_pLineCenter = NULL;
	m_pLineRight  = NULL;

	m_dwAniWaitDelay = 0;
	m_dwAniRollDelay = 0;
	m_dwAniRollCnt   = 0;
	m_dwCurAniRollCnt= 0;
	m_iMaxSelectItem = 0;
	m_iMachineCode   = 0;
	m_iEtcItemType   = 0;

	m_UseType = UT_NONE;
}

SelectExtraGashaponWnd::~SelectExtraGashaponWnd(void)
{
	SAFEDELETE( m_pLineLeft );
	SAFEDELETE( m_pLineCenter );
	SAFEDELETE( m_pLineRight );
}

void SelectExtraGashaponWnd::SendBuy()
{
	if( g_App.IsMouseBusy() )
		return;

	g_App.SetMouseBusy( true );

	SP2Packet kPacket( CTPK_BUY_SELECT_EXTRA_GASHAPON );
	kPacket << m_iEtcItemType;
	kPacket << m_iEtcItemArray;
	for (int i = 0; i < MAX_EXTRA_ITEM_CODE; i++)
	{
		kPacket << m_iExtraItemCodes[i];
	}
	TCPNetwork::SendToServer( kPacket );
}

void SelectExtraGashaponWnd::SendUse()
{
	if( g_App.IsMouseBusy() )
		return;

	g_App.SetMouseBusy( true );

	SP2Packet kPacket( CTPK_ETCITEM_USE );
	kPacket << m_iEtcItemType;
	kPacket << m_iEtcItemArray + 1;
	for (int i = 0; i < MAX_EXTRA_ITEM_CODE; i++)
	{
		if ( m_iExtraItemCodes[i] != 0 )
			kPacket << m_iExtraItemCodes[i];
	}
	TCPNetwork::SendToServer( kPacket );
}

void SelectExtraGashaponWnd::CheckBuy()
{
	// 장비 제한 체크
	if( g_MyInfo.IsExtraItemFull( true ) )
		return;

	// cash
	if( g_MyInfo.GetEntryType() == ET_TERMINATION || g_MyInfo.GetEntryType() == ET_TEMPORARY ||
		g_MyInfo.IsExpertEntryTemporary() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "회원확인후 구매가 가능합니다." );
		return;
	}

	// select 체크
	bool bError = true;
	for (int i = 0; i < m_iMaxSelectItem; i++)
	{
		if( m_iExtraItemCodes[i] > 0 )
		{
			bError = false;
			break;
		}
	}
	if( bError )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "아이템을 선택해 주세요." );
		return;
	}


	int iCash = 0;
	char szName[MAX_PATH]="";
	ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( m_iEtcItemType );
	if( pEtcItem )
	{
		iCash = pEtcItem->GetCash( m_iEtcItemArray );
		StringCbCopy( szName, sizeof( szName ), pEtcItem->GetShopName( m_iEtcItemArray ).c_str() );
	}

	if( iCash <= 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "캐쉬 가격이 잘못되었습니다." );
		return;
	}

	if( g_MyInfo.GetTotalCash() < iCash )
	{
		if( ioChannelingNodeParent::IsBillingTestID( g_MyInfo.GetPublicID() ) )
		{
			LOG.PrintTimeAndLog( 0, "IsBillingTestID is true : %s", g_MyInfo.GetPublicID().c_str() );
		}
		else
		{
			g_GUIMgr.ShowWnd( BILLING_WANT_WND );
			return;
		}
	}

	// show confirm 
	char szPrice[MAX_PATH]="";
	Help::ConvertNumToStrComma( iCash, szPrice, sizeof( szPrice ) );
	StringCbCat( szPrice, sizeof( szPrice ), " 골드");

	ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );	
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter[0].AddTextPiece( FONT_SIZE_13, "%s", szName );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );	
	kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter[1].AddTextPiece( FONT_SIZE_13, "구매금액 : %s", szPrice );

	kPrinter[2].SetTextStyle( TS_NORMAL );
	kPrinter[2].SetBkColor( 0, 0, 0 );	
	kPrinter[2].SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter[2].AddTextPiece( FONT_SIZE_13, "구매하시겠습니까?" );

	kPrinter[3].SetTextStyle( TS_NORMAL );
	kPrinter[3].SetBkColor( 0, 0, 0 );	
	kPrinter[3].SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter[3].AddTextPiece( FONT_SIZE_13, "                 " );

	kPrinter[4].SetTextStyle( TS_NORMAL );
	kPrinter[4].SetBkColor( 0, 0, 0 );	
	kPrinter[4].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[4].AddTextPiece( FONT_SIZE_13, "본 상품은 청약철회 제외 상품입니다" );

	kPrinter[5].SetTextStyle( TS_NORMAL );
	kPrinter[5].SetBkColor( 0, 0, 0 );	
	kPrinter[5].SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter[5].AddTextPiece( FONT_SIZE_13, "                 " );

	NewShopBuyConfirmWnd *pBuyMsgWnd = dynamic_cast< NewShopBuyConfirmWnd* >( g_GUIMgr.FindWnd( NEWSHOP_BUY_CONFIRM_WND ) ); 
	if( pBuyMsgWnd )
		pBuyMsgWnd->SetPrintMessage( this, kPrinter, SUBSCRIPTION_NONE );
}

void SelectExtraGashaponWnd::CheckUse()
{
	// 장비 제한 체크
	if( g_MyInfo.IsExtraItemFull( true ) )
		return;

	// cash
	if( g_MyInfo.GetEntryType() == ET_TERMINATION || g_MyInfo.GetEntryType() == ET_TEMPORARY ||
		g_MyInfo.IsExpertEntryTemporary() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "회원확인후 구매가 가능합니다." );
		return;
	}

	// select 체크
	bool bError = true;
	for (int i = 0; i < m_iMaxSelectItem; i++)
	{
		if( m_iExtraItemCodes[i] > 0 )
		{
			bError = false;
			break;
		}
	}
	if( bError )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "아이템을 선택해 주세요." );
		return;
	}

	g_GUIMgr.SetMsgBox( MB_YESNO, this, "사용하시겠습니까?" );
}

void SelectExtraGashaponWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwAniWaitDelay = xElement.GetIntAttribute_e( "WaitDelay" );
	m_dwAniRollDelay = xElement.GetIntAttribute_e( "RollDelay" );
	m_dwAniRollCnt   = xElement.GetIntAttribute_e( "RollCnt" );


	char szKeyName[MAX_PATH]="";
	StringCbPrintf( szKeyName, sizeof( szKeyName ), "turn_sound" );
	m_szTurnSound = xElement.GetStringAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf( szKeyName, sizeof( szKeyName ), "rolling_sound" );
	m_szRollingSound = xElement.GetStringAttribute( szKeyName );
}

void SelectExtraGashaponWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "LineLeft" )
	{
		SAFEDELETE( m_pLineLeft );
		m_pLineLeft = pImage;
	}
	else if( szType == "LineCenter" )
	{
		SAFEDELETE( m_pLineCenter );
		m_pLineCenter = pImage;
	}
	else if( szType == "LineRight" )
	{
		SAFEDELETE( m_pLineRight );
		m_pLineRight = pImage;
		if( m_pLineRight )
			m_pLineRight->SetReverseFormatting( ioUIRenderImage::RF_HORZ_REVERSE );
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );	
	}
}

void SelectExtraGashaponWnd::ShowSelectExtraGashaponWnd( int iEtcItemType, UseType usetype )
{
	InitAniState();

	ioEtcItemSelectExtraGashapon *pEtcItem = dynamic_cast< ioEtcItemSelectExtraGashapon * > ( g_EtcItemMgr.FindEtcItem( iEtcItemType ) );
	if( !pEtcItem )	
		return;	
	if ( COMPARE( usetype, UT_BUY_BTN, UT_USE_BTN + 1 ) )
		m_UseType = usetype;
	m_iEtcItemType   = iEtcItemType;
	m_iMaxSelectItem = pEtcItem->GetValueSize();
	m_iMachineCode   = pEtcItem->GetExtraItemMachineCode();

	if( !pEtcItem->GetTtitle().IsEmpty() )
		SetTitleText( pEtcItem->GetTtitle().c_str() );
	else
		SetTitleText( "" );


	if( !pEtcItem->GetMent1().IsEmpty() )
		m_szMent1 = pEtcItem->GetMent1();


	if( !pEtcItem->GetMent2().IsEmpty() )
		m_szMent2 = pEtcItem->GetMent2();

	Init( false );
	ShowWnd();
}

void SelectExtraGashaponWnd::iwm_create()
{
	for(int i = 0;i < MAX_ITEM_BTN; i++)
		g_GUIMgr.AddWndByTemplate( ioHashString("SelectExtraGashaponBtn"), this, ID_ITEM_1 + i );
}

void SelectExtraGashaponWnd::iwm_show()
{
	int XIndex = 0;
	int YIndex = 0;
	for(int i = 0;i < MAX_ITEM_BTN;i++)
	{
		ioWnd *pWnd = FindChildWnd( ID_ITEM_1 + i );
		if( pWnd )
		{
			pWnd->SetWndPos( 20 + 72 * XIndex, 146 + 91 * YIndex );
			XIndex++;
			if( XIndex == MAX_ITEM_BTN / 2 )
			{
				XIndex = 0;
				YIndex++;
			}
		}
	}
}

void SelectExtraGashaponWnd::iwm_hide()
{
	if( !m_szRollingSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szRollingSound, 0 );

	m_szMent1.Clear();
	m_szMent2.Clear();
}

void SelectExtraGashaponWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_BUY:
		if( cmd == IOBN_BTNUP )
		{
			if ( m_UseType == UT_BUY_BTN )
				CheckBuy();
			else if ( m_UseType == UT_USE_BTN )
				CheckUse();
		}
		break;
	case ID_AGAIN:
		if( cmd == IOBN_BTNUP )
		{
			Init( true );
		}
		break;
	case ID_GO:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd )
				pInvenWnd->ShowExtraItemTabDirect( g_MyInfo.GetClassType() );
		}
		break;
	case ID_OK:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_ITEM_1:
	case ID_ITEM_2:
	case ID_ITEM_3:
	case ID_ITEM_4:
	case ID_ITEM_5:
	case ID_ITEM_6:
	case ID_ITEM_7:
	case ID_ITEM_8:
	case ID_ITEM_9:
	case ID_ITEM_10:
		if( cmd == IOBN_BTNUP )
		{
			// 결과 화면에서는 클릭 불가
			ioWnd *pAgain = FindChildWnd( ID_AGAIN );
			if( pAgain )
			{
				if( pAgain->IsShow() )
					return;
			}

			// 결과 화면에서는 클릭 불가
			ioWnd *pOk = FindChildWnd( ID_OK );
			if( pOk )
			{
				if( pOk->IsShow() )
					return;
			}

			// 획득중 화면에서 클릭 불가
			ioWnd *pWait = FindChildWnd( ID_WAITING );
			if( pWait )
			{
				if( pWait->IsShow() )
					return;
			}

			SelectExtraGashaponBtn *pCardBtn = dynamic_cast< SelectExtraGashaponBtn * >( FindChildWnd( dwID ) );
			if( !pCardBtn )
				return;

			if( pCardBtn->IsEmptyCard() )
			{
				bool bCan = false;
				for (int i = 0; i < m_iMaxSelectItem; i++)
				{
					if( m_iExtraItemCodes[i] == 0 )
					{
						bCan = true;
						break;
					}
				}

				if( !bCan )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "카드 선택은 %d개까지 가능합니다.", m_iMaxSelectItem );
					return;
				}

				m_iSelectItemBtnID = dwID;
				SelectExtraGashaponSubWnd *pWnd = dynamic_cast<SelectExtraGashaponSubWnd*>(g_GUIMgr.FindWnd( SELECT_EXTRA_GASHAPON_SUB_WND ));
				if( pWnd )
				{
					if( pWnd->IsShow() )
					{
						pWnd->GoToTop();
					}
					else
					{
						pWnd->ShowSubWnd( m_iEtcItemType );
					}
				}
			}
			else
			{
				for (int i = 0; i < m_iMaxSelectItem; i++)
				{
					if( m_iExtraItemCodes[i] == pCardBtn->GetPresentValue1() )
					{
						m_iEtcItemArray--;
						if( m_iEtcItemArray >= 0 )
							SetTitleBuyBtn( m_iEtcItemArray );
						m_iExtraItemCodes[i] = 0;
						pCardBtn->SetBackOutAni();
						break;
					}
				}
			}
		}
		break;
	case MESSAGE_BOX:
		{
			if( cmd == IOBN_BTNUP )
			{
				if( param == IDYES )
				{
					if ( m_UseType == UT_USE_BTN )
						SendUse();
				}
			}
		}
		break;
	}
}

void SelectExtraGashaponWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ProcessAni();
}

void SelectExtraGashaponWnd::InitAniState()
{
	FSMState *pState = new FSMState(AT_NONE);
	if( pState )
		pState->Add( AT_WAIT,  AT_WAIT );
	m_AniState.Add( pState );

	pState = new FSMState(AT_WAIT);
	if( pState )
		pState->Add( AT_WAIT, AT_SOUND, m_dwAniWaitDelay );
	m_AniState.Add( pState );

	pState = new FSMState(AT_SOUND);
	if( pState )
		pState->Add( AT_ROLL, AT_ROLL );
	m_AniState.Add( pState );

	pState = new FSMState(AT_ROLL);
	if( pState )
		pState->Add( AT_ROLL, AT_CHANGE, m_dwAniRollDelay );
	m_AniState.Add( pState );	

	pState = new FSMState(AT_CHANGE);
	if( pState )
	{
		pState->Add( AT_ROLL, AT_ROLL );
		pState->Add( AT_END, AT_END );
	}
	m_AniState.Add( pState );	

	pState = new FSMState(AT_END);
	if( pState )
		pState->Add( AT_NONE, AT_NONE );
	m_AniState.Add( pState );	

	m_AniState.SetCurState( AT_NONE );
}

void SelectExtraGashaponWnd::ProcessAni()
{
	m_AniState.UpdateTime();

	if( m_AniState.GetCurState() == AT_SOUND )
	{
		if( !m_szRollingSound.IsEmpty() )
			g_SoundMgr.PlaySound( m_szRollingSound, DSBVOLUME_MAX, PSM_LOOP | PSM_THREAD );

		m_AniState.ChangeState( AT_ROLL );
	}
	else if( m_AniState.GetCurState() == AT_CHANGE )
	{
		int iPreSelectID = 0 ;
		for (int i = ID_ITEM_1; i < ID_ITEM_10+1 ; i++)
		{
			SelectExtraGashaponBtn *pCardBtn = dynamic_cast< SelectExtraGashaponBtn * >( FindChildWnd( i ) );
			if( !pCardBtn )
				continue;
			if( pCardBtn->IsEmptyCard() )
				continue;
			if( !pCardBtn->IsSelect() )
				continue;
			iPreSelectID = i;
			break;
		}

		enum { MAX_LOOP = 20, };
		for (int j = 0; j < MAX_LOOP ; j++)
		{
			int iSelectArray = rand()%MAX_ITEM_BTN;
			if( iPreSelectID == ( iSelectArray+ID_ITEM_1 ) )
				continue;

			for (int i = ID_ITEM_1; i < ID_ITEM_10+1 ; i++)
			{
				SelectExtraGashaponBtn *pCardBtn = dynamic_cast< SelectExtraGashaponBtn * >( FindChildWnd( i ) );
				if( !pCardBtn )
					continue;
				if( pCardBtn->IsEmptyCard() )
					continue;
				if( i == ( iSelectArray+ID_ITEM_1 ) )
					pCardBtn->SetSelect( true );
				else
					pCardBtn->SetSelect( false );
			}	
		}

		m_dwCurAniRollCnt++;
		if( m_dwCurAniRollCnt >= m_dwAniRollCnt )
			m_AniState.ChangeState( AT_END );
		else
			m_AniState.ChangeState( AT_ROLL );
	}
	else if( m_AniState.GetCurState() == AT_END )
	{
		if( !m_szRollingSound.IsEmpty() )
			g_SoundMgr.StopSound( m_szRollingSound, 0 );

		if( g_ExSoundMgr.IsSound( ExSound::EST_GASHAPON_GET ) )
		{
			g_ExSoundMgr.PlaySound( ExSound::EST_GASHAPON_GET );
		}

		for (int i = ID_ITEM_1; i < ID_ITEM_10+1 ; i++)
		{
			SelectExtraGashaponBtn *pCardBtn = dynamic_cast< SelectExtraGashaponBtn * >( FindChildWnd( i ) );
			if( !pCardBtn )
				continue;
			if( pCardBtn->IsEmptyCard() )
				continue;
			pCardBtn->SetSelect( false );
			if( pCardBtn->IsGetCard() )
			{
				pCardBtn->SetGetEffect();
			}
			else
			{
				pCardBtn->SetInActive();
			}
		}

		HideChildWnd( ID_BUY );
		HideChildWnd( ID_CANCEL );
		HideChildWnd( ID_WAITING );
		if ( m_UseType == UT_BUY_BTN )
		{
			ShowChildWnd( ID_AGAIN );
			HideChildWnd( ID_OK );
		}
		else if ( m_UseType == UT_USE_BTN )
		{
			ShowChildWnd( ID_OK );
			HideChildWnd( ID_AGAIN );
		}
		ShowChildWnd( ID_GO );

		m_AniState.ChangeState( AT_NONE );
	}
}

void SelectExtraGashaponWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum 
	{
		X_OFFSET_DESC = 197,
		Y_OFFSET_DESC = 53,
		LINE_GAP      = 18,

		X_OFFSET_SELECT = 197,
		Y_OFFSET_SELECT = 97,

		X_OFFSET_DESC_CARD = 198,
		Y_OFFSET_DESC_CARD = 128,

		X_OFFSET_LEFT = 20,
		Y_OFFSET_LEFT = 134,

		X_OFFSET_RIGHT = 371,
		Y_OFFSET_RIGHT = 134,

		ALL_CARD_WIDTH = 344,

		X_OFFSET_CENTER_LEFT = 27,
		Y_OFFSET_CENTER_LEFT = 134,

		X_OFFSET_CENTER_RIGHT = 371,
		Y_OFFSET_CENTER_RIGHT = 134,
	};

	// text
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );	
	kPrinter.SetBkColor( 0, 0, 0 );
	if( m_szMent1.IsEmpty() )
	{
		kPrinter.AddTextPiece( FONT_SIZE_13, "카드를 클릭하여 원하는 장비로 변경하세요" );
	}
	else
	{
		kPrinter.AddTextPiece( FONT_SIZE_13, m_szMent1.c_str() );
	}

	kPrinter.PrintFullText( iXPos + X_OFFSET_DESC, iYPos + Y_OFFSET_DESC, TAT_CENTER );
	kPrinter.ClearList();
		
	if( m_szMent2.IsEmpty() )
	{
		kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter.AddTextPiece( FONT_SIZE_13, "장비 변경시 2개째 부터 ");
		kPrinter.SetTextColor( TCT_DEFAULT_GOLD );
		kPrinter.AddTextPiece( FONT_SIZE_13, "1,000 골드씩 ");
		kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter.AddTextPiece( FONT_SIZE_13, "비용이 증가합니다");
	}
	else
	{
		kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter.AddTextPiece( FONT_SIZE_13, m_szMent2.c_str() );
	}

	kPrinter.PrintFullText( iXPos + X_OFFSET_DESC, iYPos + Y_OFFSET_DESC + LINE_GAP, TAT_CENTER );
	kPrinter.ClearList();
	
	char szSelect[MAX_PATH]="";
	StringCbPrintf( szSelect, sizeof( szSelect ), "변경 가능 : %d개", m_iMaxSelectItem - (m_iEtcItemArray+1) );

	kPrinter.SetTextColor( TCT_DEFAULT_RED );	
	kPrinter.AddTextPiece( FONT_SIZE_15, szSelect );
	kPrinter.PrintFullText( iXPos + X_OFFSET_SELECT, iYPos + Y_OFFSET_SELECT, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextColor( TCT_DEFAULT_LIGHTGRAY );	
	kPrinter.AddTextPiece( FONT_SIZE_12, m_sDescCard.c_str() );
	kPrinter.PrintFullText( iXPos + X_OFFSET_DESC_CARD, iYPos + Y_OFFSET_DESC_CARD, TAT_CENTER );
	kPrinter.ClearList();

	// 테두리
	if( m_pLineLeft )
		m_pLineLeft->Render( iXPos + X_OFFSET_LEFT, iYPos + Y_OFFSET_LEFT );

	if( m_pLineCenter )
	{
		// 자동 계산
		
		// left center
		int iWidth = ( ALL_CARD_WIDTH - ( g_FontMgr.GetTextWidth( m_sDescCard.c_str(), TS_NORMAL, FONT_SIZE_12 ) + 10 ) ) / 2;
		m_pLineCenter->SetSize( iWidth, m_pLineCenter->GetHeight() );
		m_pLineCenter->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );

		m_pLineCenter->Render( iXPos + X_OFFSET_CENTER_LEFT, iYPos + Y_OFFSET_CENTER_LEFT );

		// right center
		m_pLineCenter->Render( iXPos + X_OFFSET_CENTER_RIGHT - iWidth, iYPos + Y_OFFSET_CENTER_RIGHT );
	}

	if( m_pLineRight )
		m_pLineRight->Render( iXPos + X_OFFSET_RIGHT, iYPos + Y_OFFSET_RIGHT );
}

void SelectExtraGashaponWnd::SetSelectExtraItem( int iItemCode )
{
	// 중복 체크
	IntVec vCheck;
	for (int i = 0; i < m_iMaxSelectItem; i++)
	{
		if( m_iExtraItemCodes[i] == 0 )
			continue;
		vCheck.push_back( m_iExtraItemCodes[i] );
	}
	vCheck.push_back( iItemCode );

	if( !vCheck.empty() )
		std::sort( vCheck.begin(), vCheck.end() );

	int iSize = vCheck.size();
	for (int i = 0; i < iSize-1 ; i++)
	{
		if( vCheck[i] == vCheck[i+1] )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "같은 아이템은 선택할 수 없습니다." );
			return;
		}
	}

	for (int i = 0; i < m_iMaxSelectItem; i++)
	{
		if( m_iExtraItemCodes[i] == 0 )
		{
			m_iExtraItemCodes[i] = iItemCode;
			m_iEtcItemArray++;
			break;
		}
	}

	if( m_iEtcItemArray+1 >= m_iMaxSelectItem )
	{
		for (int i = ID_ITEM_1; i < ID_ITEM_10+1 ; i++)
		{
			SelectExtraGashaponBtn *pCardBtn = dynamic_cast< SelectExtraGashaponBtn * >( FindChildWnd( i ) );
			if( !pCardBtn )
				continue;
			if( !pCardBtn->IsEmptyCard() )
				continue;
			pCardBtn->SetEmptyCard();
			pCardBtn->SetNoneAni();
		}
	}

	SetTitleBuyBtn( m_iEtcItemArray );

	SelectExtraGashaponBtn *pCardBtn = dynamic_cast< SelectExtraGashaponBtn * >( FindChildWnd( m_iSelectItemBtnID ) );
	if( pCardBtn )
	{
		pCardBtn->SetOutAni();
		pCardBtn->SetPresent( PRESENT_EXTRAITEM, iItemCode, 0 );
	}

	m_iSelectItemBtnID = 0;
}

void SelectExtraGashaponWnd::SetTitleBuyBtn( int iCashArray )
{
	int iCash = 0;
	ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( m_iEtcItemType );
	if( pEtcItem )
	{
		iCash = pEtcItem->GetCash( iCashArray );
	}

	char szPrice[MAX_PATH]="";
	Help::ConvertNumToStrComma( iCash, szPrice, sizeof( szPrice ) );

	char szTitle[MAX_PATH]="";
	if ( m_UseType == UT_BUY_BTN )
		StringCbPrintf( szTitle, sizeof( szTitle ), "구입 결정 (%s 골드)", szPrice );
	else if ( m_UseType == UT_USE_BTN )
		StringCbPrintf( szTitle, sizeof( szTitle ), "사용하기!" );

	ioWnd *pWnd = FindChildWnd( ID_BUY );
	if( pWnd )
	{
		pWnd->SetTitleText( szTitle );
	}
}

void SelectExtraGashaponWnd::SetResult( int iGetItemCode )
{
	// init
	for (int i = ID_ITEM_1; i < ID_ITEM_10+1 ; i++)
	{
		SelectExtraGashaponBtn *pCardBtn = dynamic_cast< SelectExtraGashaponBtn * >( FindChildWnd( i ) );
		if( !pCardBtn )
			continue;
		pCardBtn->SetGetCard( false );
	}

	// set get
	bool bGetCard = false;
	IntVec vEmptyID;
	for (int i = ID_ITEM_1; i < ID_ITEM_10+1 ; i++)
	{
		SelectExtraGashaponBtn *pCardBtn = dynamic_cast< SelectExtraGashaponBtn * >( FindChildWnd( i ) );
		if( !pCardBtn )
			continue;
		
		if( pCardBtn->IsEmptyCard() )
		{
			vEmptyID.push_back( i );
			continue;
		}

		if( pCardBtn->GetPresentValue1() != iGetItemCode )
			continue;
		if( pCardBtn->IsGetCard() )
			continue;
		pCardBtn->SetGetCard( true );
		bGetCard = true;
	}

	// random
	ItemListInfoSortVector vTotalList;
	g_ExtraItemInfoMgr.GetRandomItemList( m_iMachineCode, vTotalList );
	int   iExtraListSize = vTotalList.size();

	int iSize             = vEmptyID.size();
	int iRandGetCardArray = -1;

	if( !bGetCard )
		iRandGetCardArray = rand()%iSize;
	
	enum { MAX_DUPLICATE_LOOP = 20, };

	for (int i = 0; i < iSize ; i++)
	{
		SelectExtraGashaponBtn *pCardBtn = dynamic_cast< SelectExtraGashaponBtn * >( FindChildWnd( vEmptyID[i] ) );
		if( !pCardBtn )
			continue;
		if( !pCardBtn->IsEmptyCard() )
			continue;
		if( iRandGetCardArray == i )
		{
			// 이미 받은 카드 셋팅
			pCardBtn->SetPresent( PRESENT_EXTRAITEM, iGetItemCode, 0 );
			pCardBtn->SetGetCard( true );
		}
		else
		{
			// 못 받을 랜덤 카드 셋팅( 중복 없이 )
			int iRandExtraItemCode = 0;
			for (int z = 0; z < MAX_DUPLICATE_LOOP ; z++)
			{
				bool bDuplicate = false;
				int iArray = rand()%iExtraListSize;
				for (int j = ID_ITEM_1; j <  ID_ITEM_10+1; j++)
				{
					SelectExtraGashaponBtn *pCardBtn = dynamic_cast< SelectExtraGashaponBtn * >( FindChildWnd( j ) );
					if( !pCardBtn )
						continue;
					if( pCardBtn->IsEmptyCard() )
						continue;
					if( pCardBtn->GetPresentValue1() == vTotalList[iArray].m_iItemCode )
					{
						bDuplicate = true;
						break;
					}
					iRandExtraItemCode = vTotalList[iArray].m_iItemCode;
				}

				if( !bDuplicate )
					break;
			}
			
			pCardBtn->SetPresent( PRESENT_EXTRAITEM, iRandExtraItemCode, 0 );
		}
	}

	HideChildWnd( ID_BUY );
	HideChildWnd( ID_CANCEL );
	ShowChildWnd( ID_WAITING );
	HideChildWnd( ID_AGAIN );
	HideChildWnd( ID_OK );
	HideChildWnd( ID_GO );

	// ani
	for (int i = 0; i < iSize ; i++)
	{
		SelectExtraGashaponBtn *pCardBtn = dynamic_cast< SelectExtraGashaponBtn * >( FindChildWnd( vEmptyID[i] ) );
		if( !pCardBtn )
			continue;
		pCardBtn->SetOutAni();
	}

	if( !m_szTurnSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_szTurnSound, DSBVOLUME_MAX, PSM_THREAD );

	m_AniState.ChangeState( AT_WAIT );
}

void SelectExtraGashaponWnd::SetEmptyCardAni()
{
	if( m_iEtcItemArray+1 < m_iMaxSelectItem )
	{
		for (int i = ID_ITEM_1; i < ID_ITEM_10+1 ; i++)
		{
			SelectExtraGashaponBtn *pCardBtn = dynamic_cast< SelectExtraGashaponBtn * >( FindChildWnd( i ) );
			if( !pCardBtn )
				continue;
			if( !pCardBtn->IsEmptyCard() )
				continue;
			pCardBtn->SetBackWhiteAni();
		}
	}
}

void SelectExtraGashaponWnd::Init( bool bAni )
{
	m_iEtcItemArray    = -1;
	m_iSelectItemBtnID = 0;
	m_dwCurAniRollCnt  = 0;

	for (int i = 0; i < MAX_EXTRA_ITEM_CODE; i++)
	{
		m_iExtraItemCodes[i] = 0;
	}

	ShowChildWnd( ID_BUY );
	ShowChildWnd( ID_CANCEL );
	HideChildWnd( ID_WAITING );
	HideChildWnd( ID_AGAIN );
	HideChildWnd( ID_OK );
	HideChildWnd( ID_GO );

	for (int i = ID_ITEM_1; i < ID_ITEM_10+1 ; i++)
	{
		SelectExtraGashaponBtn *pCardBtn = dynamic_cast< SelectExtraGashaponBtn * >( FindChildWnd( i ) );
		if( pCardBtn )
		{
			if( bAni )
			{
				pCardBtn->SetBackOutAni();
			}
			else
			{
				pCardBtn->SetEmptyCard();
			}
		}
	}
	
	SetTitleBuyBtn( 0 );

	m_AniState.SetCurState( AT_NONE );
}