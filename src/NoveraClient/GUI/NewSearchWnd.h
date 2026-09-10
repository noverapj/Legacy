#pragma once
#include "../io3DEngine/ioWnd.h"
#include "../ioShopManager.h"
#include "ioPullDownEventListener.h"
#include "ioPullDownItem.h"

class MyInventoryWnd;
class UISoldierSelectWnd;	// 내 용병 선택 창
class SoldierSelectWnd;		// 용병 교체 창 

// 풀다운 아이템 구분을 위한 상위 클래스 --------------------------------------------------------------------------------------------------------------------------------------------------------------
class SearchPullDown : public SinglePullDownItem
{
public:
	enum NewPullDownType
	{
		NSPDT_NONE,
		NSPDT_CATEGORY,
		NSPDT_FILTER,
	};
	NewPullDownType m_NewPullDownType;

public:
	SearchPullDown()
	{
		m_NewPullDownType = NSPDT_NONE;
	}
	virtual ~SearchPullDown(){}

};

// 카테고리(하위 탭버튼) 풀 다운 아이템 ------------------------------------------------------------------------------------------------------------------------------------------------------------
class CategoryDown : public SearchPullDown
{
public:	
	DWORD m_dwSmallTabID;

	CategoryDown()
	{
		m_NewPullDownType	= NSPDT_CATEGORY;
		m_dwSmallTabID			= 0;
	}
};

// 검색(Enter) 아이템 --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct sSearchInfo
{
public:
	ioHashString	m_sName;
	int				m_nIndex;

	sSearchInfo()
	{
		m_sName = "";
		m_nIndex = 0;
	}
}SearchInfo;
typedef std::vector<SearchInfo> vSearchInfoVec;

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class NewSearchWnd : public ioWnd, public PullDownEventListener
{	
public:
	enum
	{
		ID_CATEGORY_BTN		= 1,
		ID_CATEGORY_LIST	= 2,
		ID_EDITBOX			= 3,
		ID_EDIT_CLEAR		= 4,		
		ID_SEARCH_BTN		= 5,
		ID_SEARCH_RADIO		= 6,

		// 내용병 선택창 및 게임 용병 교체 
		ID_MY_SEARCH_BTN	= 7,
		ID_MY_SEARCH_TAB	= 8,

	};

	enum SearchType
	{
		ST_SEARCH,		
		ST_SEARCH_LIST,
		ST_FAIL,
	};

	enum WndType
	{
		WT_MY_INVENTORY,		// 내아이템
		WT_MY_INVENTORY_INFO,	// 내용병 선택 찾기
		WT_MY_USER_INFO,		// 교체 용병 선택 검색 
		WT_FAIL,
	};

protected:	
	ioUIRenderFrame*	m_pBlueFrm;
	ioUIRenderFrame*	m_pInputFrm;
	ioUIRenderFrame*	m_pSearchFail;
	ioEdit*				m_pPreEdit;

	ioUIRenderFrame*	m_pMyInputFrm;	// 내용병 선택창 	
	ioUIRenderFrame*	m_pMultiplyFrm;	// 게임 용병 교체 
	

protected:
	WndType				m_WndType;
	SearchType			m_SearchType;
	SearchType			m_PrevSearchType;
	ioHashString		m_CurrFindKeyWord;

	vSearchInfoVec		m_vSearchInfoVec;	
	int					m_nSelectEnterIndex;

	MyInventoryWnd*		m_pMyInventoryWnd;		// 내아이템
	UISoldierSelectWnd*	m_pUISoldierSelectWnd;	// 내 용병 선택 창
	SoldierSelectWnd*	m_pSoldierSelectWnd;	// 용병 교체 창

protected:	
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

public:
	virtual void iwm_show();
	virtual void iwm_hide();

	void iwm_MyInventoryMyInfoShow();	// 내용병 선택창
	void iwm_MyInventoryUserInfoShow();	// 게임 용병 교체 
	void iwm_MyInventoryShow();			// 내아이템 

public:
	void CreateCategoryPullDownItem();
	void CreateCategoryPullDownTitle( const ioHashString& szTitle );

public:
	void CreateFilterPullDownItem();

public:
	void ChangeSearchType( SearchType eType );

	void ClearString();
	void EnableEditFocus();
	void DisableEditFocus();
	bool IsKeyFocus();
	void KillKeyFocus();
	void SetKeyFocus();		
	void InputEditEnter();

	void EnableSearch();
	void DisableSearch();

public:
	void EnableFilter();

public:
	void EnableSearchFail();
	void DisbleSearchFail();

public:
	void UpdateSmallTab();

public:
	const char*	GetFindKeyWord()		{	return m_CurrFindKeyWord.c_str();	}
	SearchType	GetSearchType()			{	return m_SearchType;				}
	WndType		GetWndType()			{	return m_WndType;					}
	bool		IsFindKeyWordEmpty()	{	return m_CurrFindKeyWord.IsEmpty(); }
	bool		IsSearchInfoVecEmpty()	{	return m_vSearchInfoVec.empty();	}
	bool		IsSearch( ioHashString strName, ioHashString strFindName );

public:
	void		SetWndType( WndType wndtype )	{	m_WndType = wndtype;		}
	void		SearchInfoVecClear();


public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );	
	virtual void iwm_pulldown_event( const PullDownEvent& Event );

public:
	void OnCategoryPullDownEvent( SearchPullDown* pItem );
	void OnFilterPullDownEvent( SearchPullDown* pItem );	

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	NewSearchWnd();
	virtual ~NewSearchWnd();
};