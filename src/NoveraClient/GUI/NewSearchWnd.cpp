#include "StdAfx.h"

#include "ioShopDataCache.h"

#include "NewSearchWnd.h"
#include "NewShopSortDefine.h"
#include "ioPullDownList.h"

#include "MyInventoryWnd.h"
#include "UISoldierSelectWnd.h"	// 내 용병 선택 창
#include "SoldierSelectWnd.h"	// 용병 교체 창

NewSearchWnd::NewSearchWnd()
{
	m_SearchType			= ST_SEARCH_LIST;
	m_PrevSearchType		= ST_SEARCH_LIST;

	m_pBlueFrm				= NULL;
	m_pInputFrm				= NULL;	
	m_pSearchFail			= NULL;

	m_pPreEdit				= NULL;


	m_vSearchInfoVec.clear();
	m_nSelectEnterIndex = 0;

	m_pMyInventoryWnd		= NULL;	
	m_WndType				= WT_FAIL;

	// 내용병 선택창 
	m_pMyInputFrm			= NULL;
	m_pUISoldierSelectWnd	= NULL;

	// 게임 용병 교체
	m_pMultiplyFrm			= NULL;
	m_pSoldierSelectWnd		= NULL;
}

NewSearchWnd::~NewSearchWnd()
{
	m_vSearchInfoVec.clear();
	m_nSelectEnterIndex		= 0;
	m_pMyInventoryWnd		= NULL;
	m_WndType				= WT_FAIL;

	SAFEDELETE( m_pBlueFrm );
	SAFEDELETE( m_pInputFrm );
	SAFEDELETE( m_pSearchFail );	
	SAFEDELETE( m_pMyInputFrm );	// 내용병 선택창	
	SAFEDELETE( m_pMultiplyFrm );	// 게임 용병 교체
}

void NewSearchWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
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
	else if( szType == "MyInputFrm" )
	{
		SAFEDELETE( m_pMyInputFrm );
		m_pMyInputFrm = pFrame;
	}
	else if( szType == "MultiplyFrm" )
	{
		SAFEDELETE( m_pMultiplyFrm );
		m_pMultiplyFrm = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

// 내용병 선택창
void NewSearchWnd::iwm_MyInventoryMyInfoShow()
{
	ioWnd *pWnd = FindChildWnd(ID_CATEGORY_BTN);
	if( NULL != pWnd )
	{
		pWnd->HideWnd();
	}

	pWnd = FindChildWnd(ID_CATEGORY_LIST);
	if( NULL != pWnd )
	{
		pWnd->HideWnd();
	}
	
	pWnd = FindChildWnd(ID_SEARCH_RADIO);
	if( NULL != pWnd )
	{
		pWnd->HideWnd();
	}

	pWnd = FindChildWnd(ID_SEARCH_BTN);
	if( NULL != pWnd )
	{
		pWnd->HideWnd();
	}

	if( NULL != m_pMultiplyFrm )
	{
		m_pMultiplyFrm->SetShow(false);
	}

	pWnd = FindChildWnd(ID_MY_SEARCH_TAB);
	if( NULL != pWnd )
	{
		pWnd->SetInActive();
		pWnd->ShowWnd();
	}

	pWnd = FindChildWnd(ID_MY_SEARCH_BTN);
	if( NULL != pWnd )
	{
		pWnd->ShowWnd();
	}

	if( NULL != m_pBlueFrm )
	{
		m_pBlueFrm->SetShow(false);
	}

	if( NULL != m_pInputFrm )
	{
		m_pInputFrm->SetShow(false);
	}

	if( NULL != m_pMyInputFrm )
	{
		m_pMyInputFrm->SetShow(true);
	}

	// 위치 
	pWnd = FindChildWnd(ID_EDIT_CLEAR);
	if( NULL != pWnd )
	{
		pWnd->SetPosX(215);	
	}

	pWnd = FindChildWnd(ID_EDITBOX);
	if( NULL != pWnd )
	{
		pWnd->SetPosX(17);
		pWnd->SetPosY(8);
		pWnd->SetWidth(197);
		pWnd->SetHeight(19);
	}
}

// 게임 용병 교체 창
void NewSearchWnd::iwm_MyInventoryUserInfoShow()
{
	ioWnd *pWnd = FindChildWnd(ID_CATEGORY_BTN);
	if( NULL != pWnd )	{	pWnd->HideWnd();	}

	pWnd = FindChildWnd(ID_CATEGORY_LIST);
	if( NULL != pWnd )	{	pWnd->HideWnd();	}
	
	pWnd = FindChildWnd(ID_SEARCH_RADIO);
	if( NULL != pWnd )	{	pWnd->HideWnd();	}

	pWnd = FindChildWnd(ID_SEARCH_BTN);
	if( NULL != pWnd )	{	pWnd->HideWnd();	}

	pWnd = FindChildWnd(ID_MY_SEARCH_TAB);
	if( NULL != pWnd )
	{
		pWnd->SetInActive();
		pWnd->SetPosX(467);
		pWnd->SetPosY(10);
		pWnd->SetWidth(120);
		pWnd->GetTitle()->SetOffsetX(60);
		pWnd->ShowWnd();
	}

	pWnd = FindChildWnd(ID_MY_SEARCH_BTN);
	if( NULL != pWnd )
	{
		pWnd->SetPosX(345);
		pWnd->SetPosY(10);
		pWnd->SetWidth(120);
		pWnd->GetTitle()->SetOffsetX(60);
		pWnd->ShowWnd();
	}

	if( NULL != m_pBlueFrm )
	{
		m_pBlueFrm->SetShow(false);
	}

	if( NULL != m_pInputFrm )
	{
		m_pInputFrm->SetShow(false);
	}

	if( NULL != m_pMyInputFrm )
	{
		m_pMyInputFrm->SetSize(331,22);	
		m_pMyInputFrm->SetPosition(12,10);
		m_pMyInputFrm->SetShow(true);		
	}

	if( NULL != m_pMultiplyFrm )
	{
		m_pMultiplyFrm->SetShow(true);
	}

	// 위치 
	pWnd = FindChildWnd(ID_EDIT_CLEAR);
	if( NULL != pWnd )
	{
		pWnd->SetPosX(330);
		pWnd->SetPosY(17);
	}

	pWnd = FindChildWnd(ID_EDITBOX);
	if( NULL != pWnd )
	{
		pWnd->SetPosX(16);
		pWnd->SetPosY(13);
		pWnd->SetWidth(311);
		pWnd->SetHeight(19);
	}
}

// 내아이템
void NewSearchWnd::iwm_MyInventoryShow()
{
	if( NULL != m_pMyInputFrm )
	{
		m_pMyInputFrm->SetShow(false);
	}

	if( NULL != m_pMultiplyFrm )
	{
		m_pMultiplyFrm->SetShow(false);
	}

	ioWnd *pWnd = FindChildWnd(ID_MY_SEARCH_TAB);
	if( NULL != pWnd )
	{
		pWnd->HideWnd();
	}

	pWnd = FindChildWnd(ID_MY_SEARCH_BTN);
	if( NULL != pWnd )
	{
		pWnd->HideWnd();
	}
}

void NewSearchWnd::iwm_show()
{	
	// 2019-07-01
	if( NULL == m_pUISoldierSelectWnd )
	{
		m_pUISoldierSelectWnd = dynamic_cast<UISoldierSelectWnd*>( g_GUIMgr.FindWnd( UI_SOLDIERSELECT_WND ) );
	}

	if( NULL == m_pSoldierSelectWnd )
	{
		m_pSoldierSelectWnd = dynamic_cast<SoldierSelectWnd*>( g_GUIMgr.FindWnd( SOLDIER_SELECT_WND ) );
	}

	if( NULL == m_pMyInventoryWnd )
	{
		m_pMyInventoryWnd = dynamic_cast<MyInventoryWnd*>( g_GUIMgr.FindWnd( MY_INVENTORY_WND ) );
	}

	if( WT_MY_INVENTORY_INFO == m_WndType )
	{
		 // 내 용병 선택
		iwm_MyInventoryMyInfoShow();
		m_SearchType = ST_SEARCH;

		// 2019-07-01
		if( NULL != m_pMyInventoryWnd )
		{
			m_pMyInventoryWnd->ClearSearchHide();
		}

		// 2019-07-02
		if( (NULL != m_pSoldierSelectWnd) && (true == m_pSoldierSelectWnd->IsShow()) )
		{
			m_pSoldierSelectWnd->HideWnd();
		}
	}
	else if( WT_MY_USER_INFO == m_WndType )
	{
		// 교체 용병 선택 
		iwm_MyInventoryUserInfoShow();
		m_SearchType = ST_SEARCH;

		// 2019-07-02
		if( (NULL != m_pMyInventoryWnd) && (true == m_pMyInventoryWnd->IsShow()) )
		{
			m_pMyInventoryWnd->HideWnd();
		}
	}
	else
	{
		// 내아이템
		iwm_MyInventoryShow();
		m_SearchType = ST_SEARCH_LIST;

		// 2019-07-02
		if( (NULL != m_pSoldierSelectWnd) && (true == m_pSoldierSelectWnd->IsShow()) )
		{
			m_pSoldierSelectWnd->HideWnd();
		}

		if( (NULL != m_pUISoldierSelectWnd) && (true == m_pUISoldierSelectWnd->IsShow()) )
		{
			m_pUISoldierSelectWnd->ClearSearchHide();
			m_pUISoldierSelectWnd->HideWnd();
		}
	}

	ChangeSearchType( m_SearchType );

	if( WT_MY_USER_INFO == m_WndType )
	{
		// 2019-07-01
		if( NULL != m_pSoldierSelectWnd )
		{
			KillKeyFocus();
			m_pSoldierSelectWnd->SetSearchInputEnter(true);
		}
	}
}

void NewSearchWnd::iwm_hide()
{
	DisableSearch();
	DisableEditFocus();

	m_SearchType = ST_SEARCH_LIST;
	m_PrevSearchType = ST_SEARCH_LIST;

	if( NULL != m_pMyInventoryWnd )
	{
		m_pMyInventoryWnd->SetSelectCharIndex(0);
		m_pMyInventoryWnd->SetSearchInputEnter(false);
	}
}

// 카테고리 풀다운 관련 ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void NewSearchWnd::CreateCategoryPullDownItem()
{
	ioPullDownList *pList = dynamic_cast<ioPullDownList*>( FindChildWnd( ID_CATEGORY_LIST ) );
	if( !pList )
		return;
	
	if( NULL != m_pMyInventoryWnd )
	{
		ioHashStringVec NameList;
		m_pMyInventoryWnd->GetSmallTabNameList( NameList );

		DWORD dwSamllTabID = MyInventoryWnd::ID_SMALL_TAB_1_BTN;
		for( ioHashStringVec::iterator iter = NameList.begin(); iter != NameList.end(); ++iter, ++dwSamllTabID )
		{
			ioHashString szName = *iter;
			if( true == szName.IsEmpty() )
			{
				continue;
			}

			CategoryDown* pPullDown = new CategoryDown();
			if( NULL != pPullDown )
			{
				pPullDown->m_szTitle = szName.c_str();
				pPullDown->m_iHeight = 21;
				pPullDown->m_dwSmallTabID = dwSamllTabID;
				pPullDown->SetEnableImageRender( false );
				pList->AddList( pPullDown );
			}
		}
	}
}

void NewSearchWnd::CreateCategoryPullDownTitle( const ioHashString& szTitle )
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

// 검색/필터 선택 ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void NewSearchWnd::ChangeSearchType( SearchType eType )
{
	m_PrevSearchType	= m_SearchType;
	m_SearchType		= eType;

	switch( m_SearchType )
	{
		case ST_SEARCH_LIST:
		case ST_SEARCH:
		{
			ShowChildWnd( ID_CATEGORY_BTN );
			if( (WT_MY_INVENTORY_INFO == m_WndType) || (WT_MY_USER_INFO == m_WndType) ) // 내 용병 선택 / 교체 용병 선택 
			{
				HideChildWnd(ID_CATEGORY_BTN);
			}
			
			EnableSearch();
		}
		break;
	case ST_FAIL:
		{
			HideChildWnd( ID_CATEGORY_BTN );
			DisableSearch();
		}
		break;
	}
}

// 검색 On/Off 관련 -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void NewSearchWnd::EnableEditFocus()
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

void NewSearchWnd::DisableEditFocus()
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

bool NewSearchWnd::IsKeyFocus()
{
	ioEdit *pEdit = dynamic_cast<ioEdit*>( FindChildWnd( ID_EDITBOX ) );
	if( pEdit && pEdit->IsShow() && pEdit->IsKeyFocus() )
	{
		return true;
	}

	return false;
}

void NewSearchWnd::KillKeyFocus()
{
	ioEdit *pEdit = dynamic_cast<ioEdit*>( FindChildWnd( ID_EDITBOX ) );
	if( pEdit && pEdit->IsShow() )
	{
		pEdit->KillKeyFocus();
	}
}

void NewSearchWnd::SetKeyFocus()
{
	ioEdit *pEdit = dynamic_cast<ioEdit*>( FindChildWnd( ID_EDITBOX ) );
	if( pEdit && pEdit->IsShow() )
	{
		pEdit->SetKeyFocus();
	}
}

void NewSearchWnd::EnableSearch()
{
	ShowChildWnd( ID_EDITBOX );
	CheckRadioButton( ID_SEARCH_RADIO, ID_SEARCH_RADIO, ID_SEARCH_RADIO );
	EnableEditFocus();
}

void NewSearchWnd::DisableSearch()
{
	HideChildWnd( ID_EDITBOX );
	HideChildWnd( ID_EDIT_CLEAR );

	m_CurrFindKeyWord.Clear();
}

void NewSearchWnd::InputEditEnter()
{
	if( ((NULL != m_pMyInventoryWnd) && (false == m_pMyInventoryWnd->IsSearchInputEnter()))		||
		(NULL != m_pSoldierSelectWnd) && (false == m_pSoldierSelectWnd->IsSearchInputEnter())	||
		(NULL != m_pUISoldierSelectWnd) && (false == m_pUISoldierSelectWnd->IsSearchInputEnter()) )
	{
		KillKeyFocus();		
	}
	else if( ((NULL != m_pMyInventoryWnd) && (true == m_pMyInventoryWnd->IsSearchInputEnter()))			||
			 ((NULL != m_pSoldierSelectWnd) && (true == m_pSoldierSelectWnd->IsSearchInputEnter()))		||
			 ((NULL != m_pUISoldierSelectWnd) && (true == m_pUISoldierSelectWnd->IsSearchInputEnter()))  ) 
	{
		SetKeyFocus();
	}
}

void NewSearchWnd::SearchInfoVecClear()
{
	ioWnd* pWnd = FindChildWnd(ID_MY_SEARCH_TAB);
	if( NULL != pWnd )
	{
		pWnd->SetInActive();
	}
	m_vSearchInfoVec.clear();
	m_CurrFindKeyWord.Clear();	// 2019-07-01
}

// Refresh ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void NewSearchWnd::UpdateSmallTab()
{
	if( NULL != m_pMyInventoryWnd )
	{
		m_pMyInventoryWnd->SetSearchInputEnter(true);
		m_pMyInventoryWnd->UpdateSmallTabSearchList();
	}
}

// 마우스 이벤트 ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void NewSearchWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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

		case ID_SEARCH_BTN:
		{
			if( cmd == IOBN_BTNUP )
			{
				// 내아이템
				m_CurrFindKeyWord = g_InputBox.GetString();
				if( (NULL != m_pMyInventoryWnd) && (true == m_pMyInventoryWnd->IsSearchInputEnter()) )
				{
					SetKeyFocus();
					m_pMyInventoryWnd->SetSearchInputEnter(false);
				}
				else if( (NULL != m_pMyInventoryWnd) && (false == m_pMyInventoryWnd->IsSearchInputEnter()) )
				{
					if( true != IsFindKeyWordEmpty() )
					{
						KillKeyFocus();
						m_pMyInventoryWnd->SetSearchInputEnter(true);
						m_pMyInventoryWnd->UpdateSmallTabSearchList();
					}
				}
			}
		}
		break;

		case ID_EDITBOX:
		case ID_EDIT_CLEAR:
		{
			if( (cmd == IOBN_BTNUP) && (WT_MY_INVENTORY == m_WndType) )
			{
				// 내아이템
				EnableEditFocus();
				if( NULL != m_pMyInventoryWnd )
				{
					m_pMyInventoryWnd->SetSearchInputEnter(false);
					m_pMyInventoryWnd->UpdateSmallTabSearchList();
				}				
			}
			else if( (cmd == IOBN_BTNUP) && (WT_MY_INVENTORY_INFO == m_WndType) )
			{
				// 내용병 선택창
				EnableEditFocus();
				if( NULL != m_pUISoldierSelectWnd )
				{
					m_pUISoldierSelectWnd->SetSearchInputEnter(false);
				}
			}
			else if( (cmd == IOBN_BTNUP) && (WT_MY_USER_INFO == m_WndType) )
			{
				// 게임 용병 교체 
				EnableEditFocus();
				if( NULL != m_pSoldierSelectWnd )
				{
					m_pSoldierSelectWnd->SetSearchInputEnter(false);
				}
			}
			else if(cmd == IOWN_HIDE)	// 2019-07-03
			{
				// 포커스가 사라질때 
			}
			else if(cmd == IOBN_BTNDOWN)// 2019-07-03
			{
				// 인게임 용병창
				if( (NULL != m_pSoldierSelectWnd) && (true == m_pSoldierSelectWnd->IsShow()) )
				{
					if( true == m_pSoldierSelectWnd->IsSearchInputEnter() )
					{
						SetKeyFocus();
						m_pSoldierSelectWnd->SetSearchInputEnter(false);
						SearchInfoVecClear();
					}
					else if( false == m_pSoldierSelectWnd->IsSearchInputEnter() )
					{
						KillKeyFocus();
						m_pSoldierSelectWnd->SetSearchInputEnter(true);
					}
				}

				// 내 용병 선택창
				if( (NULL != m_pUISoldierSelectWnd) && (true == m_pUISoldierSelectWnd->IsShow()) )
				{

					if( true == m_pUISoldierSelectWnd->IsSearchInputEnter() )
					{
						SetKeyFocus();
						m_pUISoldierSelectWnd->SetSearchInputEnter(false);
						SearchInfoVecClear();
					}
					else if( false == m_pUISoldierSelectWnd->IsSearchInputEnter() )
					{
						KillKeyFocus();
						m_pUISoldierSelectWnd->SetSearchInputEnter(true);
					}	
				}

				if( (NULL != m_pMyInventoryWnd) && (true == m_pMyInventoryWnd->IsSearchInputEnter()) )
				{
					SetKeyFocus();	// 2019-07-04
					m_pMyInventoryWnd->SetSearchInputEnter(false);
				}
			}
		}
		break;
	
		case ID_MY_SEARCH_TAB:
		{
			if( (cmd == IOBN_BTNUP) && (false == IsKeyFocus()) )	// 2019-07-01
			{
				if( true == m_vSearchInfoVec.empty() ) break;

				if (m_nSelectEnterIndex >= (int)m_vSearchInfoVec.size())
				{
					m_nSelectEnterIndex = 0;
				}

				// 선택 엔터 인덱스 
				SearchInfo* pInfo = &m_vSearchInfoVec[m_nSelectEnterIndex];

				if( WT_MY_INVENTORY_INFO == m_WndType )
				{
					// 내용병 선택 창 
					if( NULL != m_pUISoldierSelectWnd )
					{
						m_pUISoldierSelectWnd->OnSelectEnter( pInfo->m_sName, pInfo->m_nIndex );
					}
				}
				else if( WT_MY_USER_INFO == m_WndType )
				{
					// 게임 용병 교체 
					if( NULL != m_pSoldierSelectWnd )
					{
						m_pSoldierSelectWnd->OnSelectEnter( pInfo->m_sName, pInfo->m_nIndex );
					}
				}

				++m_nSelectEnterIndex;
			}
		}
		break;

		case ID_MY_SEARCH_BTN:
		{
			if( cmd == IOBN_BTNUP )
			{
				// 2019-07-01
				ioHashString szFindKeyWord = g_InputBox.GetString();
				if( false == szFindKeyWord.IsEmpty() )
				{
					m_CurrFindKeyWord = szFindKeyWord;
				}

				m_nSelectEnterIndex = 0;
				m_vSearchInfoVec.clear();
				ioWnd* pWnd = FindChildWnd(ID_MY_SEARCH_TAB);
				if( NULL != pWnd )
				{
					pWnd->SetInActive();
				}
				
				if( WT_MY_INVENTORY_INFO == m_WndType )
				{
					// 내용병 선택 창 
					if( (NULL != m_pUISoldierSelectWnd) && (true == m_pUISoldierSelectWnd->IsShow()) )
					{
						// 내 용병 선택창
						if( true == m_pUISoldierSelectWnd->IsSearchInputEnter() )
						{
							SetKeyFocus();
							m_pUISoldierSelectWnd->SetSearchInputEnter(false);
						}
						else if( false == m_pUISoldierSelectWnd->IsSearchInputEnter() )
						{
							KillKeyFocus();
							m_pUISoldierSelectWnd->SetSearchInputEnter(true);
						}	

						m_pUISoldierSelectWnd->UpdateSearch( m_CurrFindKeyWord, m_vSearchInfoVec );

						// 첫번째 지정
						if( true == m_vSearchInfoVec.empty() ) 
						{
							if( NULL != pWnd )
							{
								pWnd->SetInActive();
							}
							return;
						}

						if( NULL != pWnd )
						{
							pWnd->SetActive();
						}
						
						SearchInfo* pInfo = &m_vSearchInfoVec[m_nSelectEnterIndex];
						m_pUISoldierSelectWnd->OnSelectEnter( pInfo->m_sName, pInfo->m_nIndex );
						++m_nSelectEnterIndex;
					}
				}
				else if( WT_MY_USER_INFO == m_WndType )
				{
					// 게임 용병 교체 
					if( (NULL != m_pSoldierSelectWnd) && (true == m_pSoldierSelectWnd->IsShow()) )
					{
						if( true == m_pSoldierSelectWnd->IsSearchInputEnter() )
						{
							SetKeyFocus();
							m_pSoldierSelectWnd->SetSearchInputEnter(false);
						}
						else if( false == m_pSoldierSelectWnd->IsSearchInputEnter() )
						{
							KillKeyFocus();
							m_pSoldierSelectWnd->SetSearchInputEnter(true);		

						}

						m_pSoldierSelectWnd->UpdateSearch( m_CurrFindKeyWord, m_vSearchInfoVec );

						// 첫번째 지정
						if( true == m_vSearchInfoVec.empty() ) 
						{
							if( NULL != pWnd )
							{
								pWnd->SetInActive();
							}
							return;
						}
						//else
						//{
						//	InputEditEnter();
						//	m_pSoldierSelectWnd->SetSearchInputEnter(!m_pSoldierSelectWnd->IsSearchInputEnter());
						//}

						if( NULL != pWnd )
						{
							pWnd->SetActive();
						}

						SearchInfo* pInfo = &m_vSearchInfoVec[m_nSelectEnterIndex];
						m_pSoldierSelectWnd->OnSelectEnter( pInfo->m_sName, pInfo->m_nIndex );
						++m_nSelectEnterIndex;
					}				
				}
			}
		}
		break;
	}
}

void NewSearchWnd::iwm_pulldown_event( const PullDownEvent& Event )
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
			SearchPullDown* pItem = dynamic_cast<SearchPullDown*>( pCaller->GetPullDown( ClickEvent.iClickIdx ) );
			if( pItem )
			{
				switch(  pItem->m_NewPullDownType )
				{
				case SearchPullDown::NSPDT_CATEGORY:
					{
						OnCategoryPullDownEvent( pItem );
					}
					break;	
				}
			}
		}
	}
}

void NewSearchWnd::OnCategoryPullDownEvent( SearchPullDown* pItem )
{
	CategoryDown* pCategoryItem = dynamic_cast<CategoryDown*>( pItem );
	if( pCategoryItem )
	{
		CreateCategoryPullDownTitle( pCategoryItem->m_szTitle.c_str() );
		if( NULL != m_pMyInventoryWnd )
		{
			m_pMyInventoryWnd->ChangeSmallTab( pCategoryItem->m_dwSmallTabID );
		}
	}
}

// 비교 
bool NewSearchWnd::IsSearch( ioHashString strName, ioHashString strFindName )
{
	std::string szFind = strFindName.c_str();
	if( !szFind.empty() )
		std::transform( szFind.begin(), szFind.end(), szFind.begin(), tolower );

	//소문자 변환
	std::string szName = strName.c_str();
	std::transform( szName.begin(), szName.end(), szName.begin(), tolower );

	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && pLocal->GetType() == ioLocalManager::LCT_KOREA )
	{
		//조합형 글자 비교(초성 비교 초함)
		if( !Help::Strstr( szName.c_str(), szFind.c_str() ) )
		{
			return false;
		}
	}
	else
	{
		if( strstr( szName.c_str(), szFind.c_str() ) == NULL )
		{
			return false;
		}
	}

	return true;
}


// OnProcess/OnRender ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void NewSearchWnd::OnProcess( float fTimePerSec )
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

	// 2019-06-28
	if( (true == g_Input.IsKeyUp(KEY_TAB)) && (true == m_vSearchInfoVec.empty()) )
	{		
		// 2019-07-02
		if( (NULL != m_pSoldierSelectWnd) && (true == m_pSoldierSelectWnd->IsShow()) )
		{
			if( (false == m_pSoldierSelectWnd->IsSearchKeyFocus()) && (true == m_pSoldierSelectWnd->IsSearchInputEnter()) )
			{
		//		KillKeyFocus();
		//		m_pSoldierSelectWnd->SetSearchInputEnter(true);
				return;
			}
		}

		// 내아이템 
		SetKeyFocus();

		if( (WT_MY_INVENTORY == m_WndType) && (NULL != m_pMyInventoryWnd) && (true != IsFindKeyWordEmpty()) && (true == m_pMyInventoryWnd->IsSearchInputEnter()) )
		{
			KillKeyFocus();
		}
		//else
		//{
		//	SetKeyFocus();
		//}
		return;
	}


	if( (ST_SEARCH == m_SearchType) && (true == g_Input.IsKeyUp(KEY_ENTER)) || (true == g_Input.IsKeyUp(KEY_NUMENTER)) )	// 2019-07-01
	{
		// 인게임 용병창 2019-07-03
		if( (NULL != m_pSoldierSelectWnd) && (true == m_pSoldierSelectWnd->IsShow()) )
		{
			if( true == m_pSoldierSelectWnd->IsSearchInputEnter() )
			{
				SetKeyFocus();
				m_pSoldierSelectWnd->SetSearchInputEnter(false);
				SearchInfoVecClear();
			}
			else if( false == m_pSoldierSelectWnd->IsSearchInputEnter() )
			{
				KillKeyFocus();
				m_pSoldierSelectWnd->SetSearchInputEnter(true);
			}
		}

		// 내 용병 선택창
		if( (NULL != m_pUISoldierSelectWnd) && (true == m_pUISoldierSelectWnd->IsShow()) )
		{

			if( true == m_pUISoldierSelectWnd->IsSearchInputEnter() )
			{
				SetKeyFocus();
				m_pUISoldierSelectWnd->SetSearchInputEnter(false);
				SearchInfoVecClear();
			}
			else if( false == m_pUISoldierSelectWnd->IsSearchInputEnter() )
			{
				KillKeyFocus();
				m_pUISoldierSelectWnd->SetSearchInputEnter(true);
			}	
		}
	}


	ioEdit* pEdit = dynamic_cast<ioEdit*>( FindChildWnd( ID_EDITBOX ) );
	if( pEdit && pEdit->IsKeyFocus() )
	{
		ioHashString szFindKeyWord = g_InputBox.GetString();
		if( true == szFindKeyWord.IsEmpty() )
		{
			m_CurrFindKeyWord = szFindKeyWord;
			return;
		}
		
		if( (ST_SEARCH == m_SearchType) && (true == g_InputBox.IsEnter()) )
		{
			m_CurrFindKeyWord = g_InputBox.GetString();	
			m_nSelectEnterIndex = 0;
			m_vSearchInfoVec.clear();
			ioWnd* pWnd = FindChildWnd(ID_MY_SEARCH_TAB);
			if( NULL != pWnd )
			{
				pWnd->SetInActive();
			}

			if( WT_MY_INVENTORY_INFO == m_WndType )
			{
				// 내용병 선택 창 
				if( (NULL != m_pUISoldierSelectWnd) && (true == m_pUISoldierSelectWnd->IsShow()) )
				{
					m_pUISoldierSelectWnd->UpdateSearch( m_CurrFindKeyWord, m_vSearchInfoVec );

					// 첫번째 지정
					if( true == m_vSearchInfoVec.empty() )
					{
						if( NULL != pWnd )
						{
							pWnd->SetInActive();
						}
						return;
					}

					if( NULL != pWnd )
					{
						pWnd->SetActive();
					}

					SearchInfo* pInfo = &m_vSearchInfoVec[m_nSelectEnterIndex];
					m_pUISoldierSelectWnd->OnSelectEnter( pInfo->m_sName, pInfo->m_nIndex );
					++m_nSelectEnterIndex;
				}
			}
			else if( WT_MY_USER_INFO == m_WndType )
			{
				// 게임 용병 교체 
				if( (NULL != m_pSoldierSelectWnd) && (true == m_pSoldierSelectWnd->IsShow()) )
				{
					m_pSoldierSelectWnd->UpdateSearch( m_CurrFindKeyWord, m_vSearchInfoVec );

					// 첫번째 지정
					if( true == m_vSearchInfoVec.empty() ) 
					{
						if( NULL != pWnd )
						{
							pWnd->SetInActive();
						}
						return;
					}

					if( NULL != pWnd )
					{
						pWnd->SetActive();
					}

					SearchInfo* pInfo = &m_vSearchInfoVec[m_nSelectEnterIndex];
					m_pSoldierSelectWnd->OnSelectEnter( pInfo->m_sName, pInfo->m_nIndex );
					++m_nSelectEnterIndex;
				}				
			}
			else if( WT_MY_INVENTORY == m_WndType )
			{
				// 내아이템 갱신
				m_pMyInventoryWnd->UpdateSmallTabSearch( m_CurrFindKeyWord, m_vSearchInfoVec );
			}
		}
		else if( (ST_SEARCH_LIST == m_SearchType) && (true == g_InputBox.IsEnter()) )
		{
			m_CurrFindKeyWord = g_InputBox.GetString();
			if( true != m_CurrFindKeyWord.IsEmpty() )
			{
				InputEditEnter();
			}
		}
	}

	if( (ST_SEARCH == m_SearchType) && (true == g_Input.IsKeyUp(KEY_TAB)) && (true != m_vSearchInfoVec.empty()) && (false == IsKeyFocus()) ) // 2019-07-01
	{			
		if (m_nSelectEnterIndex >= (int)m_vSearchInfoVec.size())
		{
			m_nSelectEnterIndex = 0;
		}

		// 2019-07-01
		if( WT_MY_INVENTORY_INFO == m_WndType )
		{
			if( (NULL != m_pUISoldierSelectWnd) && (true == m_pUISoldierSelectWnd->IsShow()) )
			{
				if( false == m_pUISoldierSelectWnd->IsSearchInputEnter() )
				{
					SetKeyFocus();
					return;
				}
			}
		}
		else if( WT_MY_USER_INFO == m_WndType )
		{
			if( (NULL != m_pSoldierSelectWnd) && (true == m_pSoldierSelectWnd->IsShow()) )
			{
				if( false == m_pSoldierSelectWnd->IsSearchInputEnter() )
				{
					SetKeyFocus();
					return;
				}
			}
		}
		else if( WT_MY_INVENTORY == m_WndType )
		{
			if( (NULL != m_pMyInventoryWnd) && (true == m_pMyInventoryWnd->IsShow()) )
			{
				if( false == m_pMyInventoryWnd->IsSearchInputEnter() )
				{
					SetKeyFocus();
					return;
				}
			}
		}

		// 선택 엔터 인덱스 
		SearchInfo* pInfo = &m_vSearchInfoVec[m_nSelectEnterIndex];

		if( WT_MY_INVENTORY_INFO == m_WndType )
		{
			// 내용병 선택 창 
			if( (NULL != m_pUISoldierSelectWnd) && (true == m_pUISoldierSelectWnd->IsShow()) )
			{
				m_pUISoldierSelectWnd->OnSelectEnter( pInfo->m_sName, pInfo->m_nIndex );
			}
		}
		else if( WT_MY_USER_INFO == m_WndType )
		{
			// 게임 용병 교체 
			if( (NULL != m_pSoldierSelectWnd) && (true == m_pSoldierSelectWnd->IsShow()) )
			{
				m_pSoldierSelectWnd->OnSelectEnter( pInfo->m_sName, pInfo->m_nIndex );
			}				
		}
		else if( WT_MY_INVENTORY == m_WndType )
		{
			// 내아이템 갱신
			if( (NULL != m_pMyInventoryWnd) && (true == m_pMyInventoryWnd->IsShow()) )
			{
				m_pMyInventoryWnd->OnMyInfoSelectEnter(pInfo->m_nIndex);
			}
		}

		++m_nSelectEnterIndex;
	}
}

void NewSearchWnd::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	switch( m_SearchType )
	{
		case ST_SEARCH_LIST:
		case ST_SEARCH:
		{
			ioWnd::OnRender();

			if( NULL != m_pMultiplyFrm )
			{
				m_pMultiplyFrm->Render( iXPos, iYPos, UI_RENDER_MULTIPLY );
			}

			if( NULL != m_pMyInputFrm )
			{
				m_pMyInputFrm->Render( iXPos, iYPos );
			}

			if( m_pBlueFrm )
				m_pBlueFrm->Render( iXPos, iYPos );

			ioWnd* pWnd = FindChildWnd( ID_EDITBOX );
			if( m_pInputFrm && pWnd && pWnd->IsShow() )
				m_pInputFrm->Render( iXPos, iYPos );
		}
		break;
	}
}