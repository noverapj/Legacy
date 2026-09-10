#pragma once

class SearchNamedTitleInfo
{
public:
	DWORD m_dwCode;
	BYTE m_byPremium;
	ioHashString m_szName;

public:
	void Clear()
	{
		m_dwCode = 0;
		m_byPremium = 0;
		m_szName.Clear();
	}

public:
	SearchNamedTitleInfo()
	{
		Clear();
	}
	~SearchNamedTitleInfo()	{ }
};

class SearchNamedTitleSortFunc : public std::binary_function< const SearchNamedTitleInfo&, const SearchNamedTitleInfo&, bool >
{
public:
	bool operator()( const SearchNamedTitleInfo &lhs, const SearchNamedTitleInfo &rhs ) const
	{
		if( strcmp( lhs.m_szName.c_str(), rhs.m_szName.c_str() ) < 0 )
			return true;

		return false;	
	}
};

typedef std::vector< SearchNamedTitleInfo > vSearchNamedTitleInfo;

class NamedTitleSelectWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT_BTN				= 1,
		ID_FIND_EDIT			= 2,
		ID_FIND_BTN				= 3,
		ID_RELEASE_BTN			= 4,
		ID_FIND_RESET_BTN		= 5,
		
		ID_VERT_SCROLL			= 1001,
	};

	enum
	{
		MAX_TAB_BTN		= 3,

		ID_TAB_1_BTN	= 101,
		ID_TAB_2_BTN	= 102,
		ID_TAB_3_BTN	= 103,
	};

	enum
	{
		MAX_ITEM_INFO_WND_BTN	= 15,

		ID_ITEM_INFO_WND_1_BTN	= 201,
		ID_ITEM_INFO_WND_2_BTN	= 202,
		ID_ITEM_INFO_WND_3_BTN	= 203,
		ID_ITEM_INFO_WND_4_BTN	= 204,
		ID_ITEM_INFO_WND_5_BTN	= 205,
		ID_ITEM_INFO_WND_6_BTN	= 206,
		ID_ITEM_INFO_WND_7_BTN	= 207,
		ID_ITEM_INFO_WND_8_BTN	= 208,
		ID_ITEM_INFO_WND_9_BTN	= 209,
		ID_ITEM_INFO_WND_10_BTN	= 210,
		ID_ITEM_INFO_WND_11_BTN	= 211,
		ID_ITEM_INFO_WND_12_BTN	= 212,
		ID_ITEM_INFO_WND_13_BTN	= 213,
		ID_ITEM_INFO_WND_14_BTN	= 214,
		ID_ITEM_INFO_WND_15_BTN	= 215,
	};

protected:
	int m_nCurPage;
	
	vSearchNamedTitleInfo m_NamedTitleList;
	vSearchNamedTitleInfo m_CurFindNamedTitleList;
	ioHashString m_szCurFindKeyword;

	DWORD m_dwNamedTitleCode;

	DWORD m_dwCurTabID;
	bool m_bNoSearch;
	bool m_bEmpty;
	ioComplexStringPrinter m_szEmpty;
	ioComplexStringPrinter m_szNoSearch;

	ioHashString m_szNamedTitle;
	BYTE m_byNamedTitlePremium;
	ioUIRenderImage* m_pNamedTitleUiMark;

public:
	virtual void iwm_vscroll(DWORD dwID, int curPos );
	virtual void iwm_wheel( int zDelta );
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void RenderNamedTitle( int nXPos, int nYPos );

public:
	void Update();

protected:
	void ChangeTab( DWORD dwID, const char *szFind );
	void UpdatePage( int iNewPage );

protected:
	void SetSearchNamedTitleVec();
	void SetFilterNamedTitleVec( DWORD dwTabID, const char *szFind );
	void CompareFilterNamedTitle( const char *szFind, bool bAll, bool bPremium );
	void UpdateEquipNamedTitle();

protected:
	void SetFind();
	void SetFindReset();

public:
	void ShowSelectWnd();
	DWORD GetNamedTitleCode();

public:
	NamedTitleSelectWnd();
	virtual ~NamedTitleSelectWnd();
};

////////////////////////////////////////////////////////////
class FilterNamedTitleInfoWnd : public ioButton
{
public:
	enum 
	{
		ID_SELECT_BTN = 1,
	};
	
protected:
	ioComplexStringPrinter m_szNamedTitle;
	DWORD m_dwCode;
	NamedTitleSlot m_NamedTitleSlot;
	ioUIRenderImage* m_pNewMark;

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnRender();
	
protected:
	void RenderInfo( int nXPos, int nYPos );

protected:	
	bool SetInfo();

public:
	void SetBlank();
	void SetNamedTitleInfo( DWORD dwCode );
	DWORD GetCode();

public:
	FilterNamedTitleInfoWnd();
	virtual ~FilterNamedTitleInfoWnd();
};