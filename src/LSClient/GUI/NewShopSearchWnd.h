#pragma once
#include "../io3DEngine/ioWnd.h"
#include "../ioShopManager.h"
#include "ioPullDownEventListener.h"
#include "ioPullDownItem.h"

// 풀다운 아이템 구분을 위한 상위 클래스 --------------------------------------------------------------------------------------------------------------------------------------------------------------

class NewShopSearchPullDown : public SinglePullDownItem
{
public:
	enum NewShopPullDownType
	{
		NSPDT_NONE,
		NSPDT_CATEGORY,
		NSPDT_FILTER,
	};
	NewShopPullDownType m_NewShopPullDownType;

public:
	NewShopSearchPullDown()
	{
		m_NewShopPullDownType = NSPDT_NONE;
	}
	virtual ~NewShopSearchPullDown(){}

};

// 카테고리(상점 하위 탭버튼) 풀 다운 아이템 ------------------------------------------------------------------------------------------------------------------------------------------------------------

class CategoryDownItem : public NewShopSearchPullDown
{
public:	
	DWORD m_dwSmallTabID;

	CategoryDownItem()
	{
		m_NewShopPullDownType	= NSPDT_CATEGORY;
		m_dwSmallTabID			= 0;
	}
};

// 검색 풀 다운 아이템 --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class SearchFilterDownItem : public NewShopSearchPullDown
{
public:
	NewShopFilter m_eFilter;

	SearchFilterDownItem()
	{
		m_NewShopPullDownType	= NSPDT_FILTER;
		m_eFilter				= NSF_NONE;
	}
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class NewShopSearchWnd : public ioWnd, public PullDownEventListener
{

public:
	enum
	{
		ID_CATEGORY_BTN		= 1,
		ID_CATEGORY_LIST	= 2,
		ID_SEARCH_RADIO		= 3,
		ID_SORT_RADIO		= 4,
		ID_EDITBOX			= 5,
		ID_EDIT_OK			= 6,
		ID_EDIT_CLEAR		= 7,
		ID_FILTER_BTN		= 8,
		ID_FITER_LIST		= 9,
		ID_INIT				= 10,
	};

	enum SearchType
	{
		ST_WORD,
		ST_FILTER,
		ST_FAIL,
	};

protected:	
	ioUIRenderFrame*	m_pBlueFrm;
	ioUIRenderFrame*	m_pInputFrm;
	ioUIRenderFrame*	m_pSearchFail;

	ioEdit*				m_pPreEdit;

protected:
	SearchType			m_SearchType;
	SearchType			m_PrevSearchType;
	NewShopFilter		m_eCurrNewShopFilter;
	ioHashString		m_CurrFindKeyWord;

protected:	
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

public:
	virtual void iwm_show();
	virtual void iwm_hide();

public:
	void CreateCategoryPullDownItem();
	void CreateCategoryPullDownTitle( const ioHashString& szTitle );

public:
	void CreateFilterPullDownItem();
	void CreateFilterPullDownTitle( const ioHashString& szTitle );

public:
	void ChangeSearchType( SearchType eType );

	void ClearString();
	void EnableEditFocus();
	void DisableEditFocus();
	bool IsKeyFocus();

	void EnableSearch();
	void DisableSearch();

public:
	void EnableFilter();
	void DisableFilter();

public:
	void EnableSearchFail();
	void DisbleSearchFail();

public:
	void UpdateSmallTab();

public:
	NewShopFilter GetCurrNewShopFilter(){ return m_eCurrNewShopFilter; }
	const char* GetFindKeyWord(){ return m_CurrFindKeyWord.c_str(); }

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );	
	virtual void iwm_pulldown_event( const PullDownEvent& Event );

public:
	void OnCategoryPullDownEvent( NewShopSearchPullDown* pItem );
	void OnFilterPullDownEvent( NewShopSearchPullDown* pItem );	

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	NewShopSearchWnd();
	virtual ~NewShopSearchWnd();
};