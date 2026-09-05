#include "StdAfx.h"

#include "ioShopDataCache.h"

#include "NewShopSearchWnd.h"
#include "NewShopSortDefine.h"
#include "ioPullDownList.h"

NewShopSearchWnd::NewShopSearchWnd()
{
	m_SearchType			= ST_FILTER;
	m_PrevSearchType		= ST_FILTER;

	m_pBlueFrm				= NULL;
	m_pInputFrm				= NULL;	
	m_pSearchFail			= NULL;

	m_pPreEdit				= NULL;

	m_eCurrNewShopFilter	= NSF_NONE;
}

NewShopSearchWnd::~NewShopSearchWnd()
{
	SAFEDELETE( m_pBlueFrm );
	SAFEDELETE( m_pInputFrm );
	SAFEDELETE( m_pSearchFail );
}

void NewShopSearchWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "BlueFrm" )
	{
		SAFEDELETE( m_pBlueFrm );
		m_pBlueFrm = pFrame;
	}
	else if( szType == "InputFrm" )
	{
		SAFEDELETE( m_pInputFrm );
		m_pInputFrm = pFrame;
	}
	else if( szType == "SerachFail" )
	{
		SAFEDELETE( m_pSearchFail );
		m_pSearchFail = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void NewShopSearchWnd::iwm_show()
{
	// 2019-07-18
	ChangeSearchType( ST_WORD );	
}

void NewShopSearchWnd::iwm_hide()
{
	DisableSearch();
	DisableFilter();
	DisableEditFocus();
	m_eCurrNewShopFilter = NSF_NONE;	
}

// 카테고리 풀다운 관련 ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void NewShopSearchWnd::CreateCategoryPullDownItem()
{
	ioPullDownList *pList = dynamic_cast<ioPullDownList*>( FindChildWnd( ID_CATEGORY_LIST ) );
	if( !pList )
		return;

	NewShopWnd* pShop = dynamic_cast<NewShopWnd*>( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
	if( pShop )
	{
		ioHashStringVec NameList;
		pShop->GetSmallTabNameList( NameList );

		DWORD dwSamllTabID = NewShopWnd::ID_SMALL_TAB_1_BTN;
		for( ioHashStringVec::iterator iter = NameList.begin(); iter != NameList.end(); ++iter, ++dwSamllTabID )
		{
			ioHashString szName = *iter;
			if( szName.IsEmpty() )
				continue;

			CategoryDownItem* pPullDown = new CategoryDownItem;
			if( pPullDown )
			{
				pPullDown->m_szTitle		= szName.c_str();
				pPullDown->m_iHeight		= 21;
				pPullDown->m_dwSmallTabID	= dwSamllTabID;
				pPullDown->SetEnableImageRender( false );
				pList->AddList( pPullDown );
			}
		}
	}
}

void NewShopSearchWnd::CreateCategoryPullDownTitle( const ioHashString& szTitle )
{
	ioButton* pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_CATEGORY_BTN ) );
	if( pButton )
	{
		ioUITitle* pTitle = pButton->GetTitle();
		if( pTitle )
		{
			pTitle->SetOffset( pButton->GetWidth()/2 - 6, pTitle->GetTextYOffSet() );
			pTitle->SetText( szTitle.c_str(), "" );
		}
	}
}

// 카테고리 풀다운 관련 ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void NewShopSearchWnd::CreateFilterPullDownTitle( const ioHashString& szTitle )
{
	ioButton* pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_FILTER_BTN ) );
	if( pButton )
	{
		ioUITitle* pTitle = pButton->GetTitle();
		if( pTitle )
		{
			pTitle->SetOffset( pButton->GetWidth()/2, pTitle->GetTextYOffSet() );
			pTitle->SetText( szTitle.c_str(), "" );
		}
	}
}

void NewShopSearchWnd::CreateFilterPullDownItem()
{
	ioPullDownList *pList = dynamic_cast<ioPullDownList*>( FindChildWnd( ID_FITER_LIST ) );
	if( !pList )
		return;

	NewShopWnd* pShop = dynamic_cast<NewShopWnd*>( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
	if( pShop )
	{
		for( int i = 0; i < g_ShopManager.GetFilterCount(); ++i )
		{
			SearchFilterDownItem* pPullDown = new SearchFilterDownItem;
			if( pPullDown )
			{
				NewShopFilterInfo* pInfo = g_ShopManager.GetFilter( i );
				if( pInfo && pInfo->IsUseTab( pShop->GetCurrTab() ) )
				{
					pPullDown->m_szTitle = pInfo->m_FilterName.c_str();
					pPullDown->m_iHeight = 21;
					pPullDown->m_eFilter = pInfo->m_eFiter;
					pPullDown->SetEnableImageRender( false );
					pList->AddList( pPullDown );
				}
			}
		}
	}
}

// 검색/필터 선택 ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void NewShopSearchWnd::ChangeSearchType( SearchType eType )
{
	m_PrevSearchType	= m_SearchType;
	m_SearchType		= eType;

	switch( m_SearchType )
	{
	case ST_WORD:
		{
			ShowChildWnd( ID_CATEGORY_BTN );
			ShowChildWnd( ID_SEARCH_RADIO );
			ShowChildWnd( ID_SORT_RADIO );

			DisbleSearchFail();
			DisableFilter();
			EnableSearch();
		}
		break;
	case ST_FILTER:
		{
			ShowChildWnd( ID_CATEGORY_BTN );
			ShowChildWnd( ID_SEARCH_RADIO );
			ShowChildWnd( ID_SORT_RADIO );

			DisbleSearchFail();
			DisableSearch();
			EnableFilter();
		}
		break;
	case ST_FAIL:
		{
			HideChildWnd( ID_SEARCH_RADIO );
			HideChildWnd( ID_SORT_RADIO );
			HideChildWnd( ID_CATEGORY_BTN );

			DisableFilter();
			DisableSearch();

			EnableSearchFail();
		}
		break;
	}
}

// 검색 On/Off 관련 -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void NewShopSearchWnd::EnableEditFocus()
{
	ioEdit *pEdit = dynamic_cast<ioEdit*>( FindChildWnd( ID_EDITBOX ) );
	if( pEdit && pEdit->IsShow() )
	{
		if( m_pPreEdit != pEdit )
			m_pPreEdit = ioEdit::m_pKeyFocusEdit;

		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString( "" );
		m_CurrFindKeyWord.Clear();
	}
}

void NewShopSearchWnd::DisableEditFocus()
{
	ioEdit* pEdit = dynamic_cast<ioEdit*>( m_pPreEdit );
	if( pEdit )	
	{
		ioWnd *pParent = pEdit->GetParent();
		if( pParent )
		{
			if( pParent->IsShow() )
			{
				pEdit->SetKeyFocus();
			}
		}
	}
	m_pPreEdit = NULL;
	m_CurrFindKeyWord.Clear();
}

bool NewShopSearchWnd::IsKeyFocus()
{
	ioEdit *pEdit = dynamic_cast<ioEdit*>( FindChildWnd( ID_EDITBOX ) );
	if( pEdit && pEdit->IsShow() && pEdit->IsKeyFocus() )
	{
		return true;
	}

	return false;
}

void NewShopSearchWnd::EnableSearch()
{
	ShowChildWnd( ID_EDITBOX );
	ShowChildWnd( ID_EDIT_OK );

	CheckRadioButton( ID_SEARCH_RADIO, ID_SORT_RADIO, ID_SEARCH_RADIO );
	EnableEditFocus();
}

void NewShopSearchWnd::DisableSearch()
{
	HideChildWnd( ID_EDITBOX );
	HideChildWnd( ID_EDIT_OK );
	HideChildWnd( ID_EDIT_CLEAR );

	m_CurrFindKeyWord.Clear();
}

// 필터On/Off 관련 -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void NewShopSearchWnd::EnableFilter()
{
	ShowChildWnd( ID_FILTER_BTN );

	CheckRadioButton( ID_SEARCH_RADIO, ID_SORT_RADIO, ID_SORT_RADIO );
	DisableEditFocus();
	CreateFilterPullDownTitle( "" );	

	NewShopWnd* pShop = dynamic_cast<NewShopWnd*>( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
	if( pShop )
	{
		//저장되있는 캐쉬에서 필터 가져오기
		NewShopFilter eCurrFiter = NSF_NONE;
		NewShopCache* pCache = g_ShopData.GetCash( pShop->GetCurrTab(), pShop->GetCurrSmallTab(), pShop->GetDecoSexType() );
		if( pCache )
			eCurrFiter = static_cast<NewShopFilter>( pCache->eNewShopFilter );
				
		for( int i = 0; i < g_ShopManager.GetFilterCount(); ++i )
		{
			SearchFilterDownItem* pPullDown = new SearchFilterDownItem;
			if( pPullDown )
			{
				NewShopFilterInfo* pInfo = g_ShopManager.GetFilter( i );
				if( pInfo && pInfo->IsUseTab( pShop->GetCurrTab() ) && pInfo->m_eFiter == eCurrFiter )
				{
					CreateFilterPullDownTitle( pInfo->m_FilterName );
					m_eCurrNewShopFilter = pInfo->m_eFiter;

					pShop->UpdateSmallTab( pShop->GetCurrSmallTab(), true );
					break;
				}
			}
		}
	}
}

void NewShopSearchWnd::DisableFilter()
{
	HideChildWnd( ID_FILTER_BTN );
	HideChildWnd( ID_FITER_LIST );

	m_eCurrNewShopFilter = NSF_NONE;
}

void NewShopSearchWnd::EnableSearchFail()
{
	ShowChildWnd( ID_INIT );
}

void NewShopSearchWnd::DisbleSearchFail()
{
	HideChildWnd( ID_INIT );
}

// 상점 Refresh ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void NewShopSearchWnd::UpdateSmallTab()
{
	NewShopWnd* pShop = dynamic_cast<NewShopWnd*>( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
	if( pShop )
		pShop->UpdateSmallTab( pShop->GetCurrSmallTab(), true );
}

// 마우스 이벤트 ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void NewShopSearchWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_CATEGORY_BTN:
		{
			if( cmd == IOBN_BTNUP )
			{
				ioPullDownList *pList = dynamic_cast<ioPullDownList*>( FindChildWnd( ID_CATEGORY_LIST ) );
				if( pList )
				{
					pList->OpenList( pWnd );
					CreateCategoryPullDownItem();
				}
			}
		}
		break;
	case ID_SEARCH_RADIO:
		{
			if( cmd == IOBN_BTNUP )
			{
				m_eCurrNewShopFilter = NSF_NONE;
				ChangeSearchType( ST_WORD );
				UpdateSmallTab();
			}
		}
		break;
	case ID_SORT_RADIO:
		{
			if( cmd == IOBN_BTNUP )
			{
				m_eCurrNewShopFilter = NSF_NONE;
				ChangeSearchType( ST_FILTER );
				UpdateSmallTab();
			}
		}
		break;
	case ID_EDIT_OK:
		{
			if( cmd == IOBN_BTNUP )
			{
				m_CurrFindKeyWord = g_InputBox.GetString();
				UpdateSmallTab();
			}
		}
		break;
	case ID_EDIT_CLEAR:
		{
			if( cmd == IOBN_BTNUP )
			{
				EnableEditFocus();
				UpdateSmallTab();
			}
		}
		break;
	case ID_FILTER_BTN:
		{
			if( cmd == IOBN_BTNUP )
			{
				ioPullDownList *pList = dynamic_cast<ioPullDownList*>( FindChildWnd( ID_FITER_LIST ) );
				if( pList )
				{
					pList->OpenList( pWnd );
					CreateFilterPullDownItem();
				}
			}
		}
		break;
	case ID_INIT:
		{
			if( cmd == IOBN_BTNUP )
			{
				ChangeSearchType( m_PrevSearchType );
				UpdateSmallTab();
			}
		}
		break;
	}
}

void NewShopSearchWnd::iwm_pulldown_event( const PullDownEvent& Event )
{
	const ioPullDownList* pCaller = Event.GetEventCaller();	
	if( !pCaller )
		return;

	//클릭 이벤트
	if( CLASSTYPEQUALS( ClickPullDownEvent ) == CLASSTYPEQUALS( Event ) )
	{
		//다운 클릭
		const ClickPullDownEvent& ClickEvent = dynamic_cast<const ClickPullDownEvent&>( Event );
		if( ClickEvent.IsPulldownClick() )
		{
			//상점 풀다운 아이템
			NewShopSearchPullDown* pItem = dynamic_cast<NewShopSearchPullDown*>( pCaller->GetPullDown( ClickEvent.iClickIdx ) );
			if( pItem )
			{
				switch(  pItem->m_NewShopPullDownType )
				{
				case NewShopSearchPullDown::NSPDT_CATEGORY:
					{
						OnCategoryPullDownEvent( pItem );
					}
					break;
				case NewShopSearchPullDown::NSPDT_FILTER:
					{
						OnFilterPullDownEvent( pItem );
					}
					break;
				}
			}
		}
	}
}

void NewShopSearchWnd::OnCategoryPullDownEvent( NewShopSearchPullDown* pItem )
{
	CategoryDownItem* pCategoryItem = dynamic_cast<CategoryDownItem*>( pItem );
	if( pCategoryItem )
	{
		CreateCategoryPullDownTitle( pCategoryItem->m_szTitle.c_str() );
		NewShopWnd* pShop = dynamic_cast<NewShopWnd*>( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
		if( pShop )
		{
			//저장되있는 캐쉬에서 필터 가져오기
			NewShopFilter eCurrFiter = NSF_NONE;
			const NewShopCache* pCache = g_ShopData.GetCash( pShop->GetCurrTab(), pCategoryItem->m_dwSmallTabID, pShop->GetDecoSexType() );
			if( pCache )
				m_eCurrNewShopFilter = static_cast<NewShopFilter>( pCache->eNewShopFilter );

			pShop->UpdateSmallTab( pCategoryItem->m_dwSmallTabID, true );
		}
	}
}

void NewShopSearchWnd::OnFilterPullDownEvent( NewShopSearchPullDown* pItem )
{
	SearchFilterDownItem* pFilterItem = dynamic_cast<SearchFilterDownItem*>( pItem );
	if( pFilterItem )
	{
		CreateFilterPullDownTitle( pFilterItem->m_szTitle.c_str() );
		m_eCurrNewShopFilter = pFilterItem->m_eFilter;

		NewShopWnd* pShop = dynamic_cast<NewShopWnd*>( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
		if( pShop )
		{
			NewShopCache* pCache = g_ShopData.GetCash( pShop->GetCurrTab(), pShop->GetCurrSmallTab(), pShop->GetDecoSexType() );
			if( pCache )
				pCache->eNewShopFilter = pFilterItem->m_eFilter;

			pShop->UpdateSmallTab( pShop->GetCurrSmallTab(), true );
		}
	}
}

// OnProcess/OnRender ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void NewShopSearchWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioWnd* pWnd = FindChildWnd( ID_EDIT_CLEAR );
	if( pWnd )
	{
		if( pWnd->IsShow() )
		{
			if( !IsKeyFocus() )
			{
				HideChildWnd( ID_EDIT_CLEAR );
			}
		}
		else
		{
			if( IsKeyFocus() )
			{
				ShowChildWnd( ID_EDIT_CLEAR );
			}
		}
	}
	
	ioEdit* pEdit = dynamic_cast<ioEdit*>( FindChildWnd( ID_EDITBOX ) );
	if( pEdit && pEdit->IsKeyFocus() )
	{
		if( g_InputBox.IsEnter() )
		{
			m_CurrFindKeyWord = g_InputBox.GetString();
			iwm_command( FindChildWnd( ID_EDIT_OK ), IOBN_BTNUP, 0 );
		}
	}
	
}

void NewShopSearchWnd::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	switch( m_SearchType )
	{
	case ST_WORD:
	case ST_FILTER:
		{
			ioWnd::OnRender();

			if( m_pBlueFrm )
				m_pBlueFrm->Render( iXPos, iYPos );

			ioWnd* pWnd = FindChildWnd( ID_EDITBOX );
			if( m_pInputFrm && pWnd && pWnd->IsShow() )
				m_pInputFrm->Render( iXPos, iYPos );
		}
		break;
	case ST_FAIL:
		{
			if( m_pSearchFail )
				m_pSearchFail->Render( iXPos, iYPos );

			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
			g_FontMgr.PrintText( iXPos + 211, iYPos + 7, FONT_SIZE_12, "검색결과를 찾을 수 없습니다." );
		}
		break;
	}
}