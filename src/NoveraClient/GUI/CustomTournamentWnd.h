#pragma once
#include "NewShopToolTip.h"

//////////////////////////////////////////////////////////////////////////
class CustomTournamentNoticeWnd : public ioWnd 
{
protected:
	enum
	{
		ID_EXIT		= 1,
		ID_CLOSE	= 2,
		ID_CONFIRM	= 3,

		ID_PREV		= 10,
		ID_NEXT		= 11,

		ID_EDIT  	= 100,
	};

	enum
	{
		MAX_BTYE = 500,
	};

protected:
	DWORD m_dwTournamentIndex;
	DWORD m_dwOwnerIndex;

	int m_iCurPage;
	int m_iMaxPage;

	int m_iCurTextByte;

protected:
	ioEdit* m_pPreEdit;

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	virtual void iwm_show();
	virtual void iwm_hide();

public:
	void SetEditKeyFocus();
	void KillEditKeyFocus();

public:
	void SendNoticeData( const StringVector& szSrcVec );
	void SetNoticeData( DWORD dwTourIndex, StringVector& szSrcAnnounce, DWORD dwOwnerIndex );

public:
	static void ConvertStringVector( IN const ioHashString& szSrcAnnounce, OUT StringVector& szDest );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	CustomTournamentNoticeWnd();
	virtual ~CustomTournamentNoticeWnd();
};

//////////////////////////////////////////////////////////////////////////
class CustomTournamentAllocateWnd : public ioWnd
{
public:
	enum
	{
		ID_CLOSE	    = 1,
		ID_RANDOM_CHECK = 2,
		ID_CONFIRM	    = 3,
		ID_EXIT			= 4,

		UNALLOCATE_WND  = 10,
		ALLOCATED_WND   = 11,
	};

protected:
	DWORD m_dwTournamentIndex;
	DWORD m_dwTournamentStartTeamCount;

	byte m_ProgressTournament;

public:
	virtual void iwm_show();
	virtual void iwm_hide();

	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual IWDropEffect iwm_dragover( ioDragItem *pItem, const ioMouse& mouse );
	virtual bool iwm_dropitem( ioDragItem *pItem, const ioMouse& mouse, ioWnd *pStartWnd );	

public:
	void SetCustomTournamentData( DWORD dwTourIndex );
	void SetAllocateMessage();

	void SendCustomTournamentAllocatedData();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	CustomTournamentAllocateWnd();
	virtual ~CustomTournamentAllocateWnd();
};

//////////////////////////////////////////////////////////////////////////
class ioUnAllocateButtonDragItem;
class UnAllocateButton : public ioButton
{
protected:
	TournamentNode::AllocateTeamData* m_pCustomTeamData;
	ioUnAllocateButtonDragItem *m_pDragItem;

protected:
	virtual void iwm_hide();

	virtual void iwm_mouseleave( const ioMouse& mouse );
	virtual void iwm_mouseover( const ioMouse& mouse );
	virtual IWDropEffect iwm_dragover( ioDragItem *pItem, const ioMouse& mouse );
	virtual bool iwm_dropitem( ioDragItem *pItem, const ioMouse& mouse, ioWnd *pStartWnd );
	virtual void iwm_dropdone( ioDragItem *pItem, ioWnd *pDroppedWnd, bool bDropped );

public:
	void SetCustomTeamData( TournamentNode::AllocateTeamData* TeamData );
	TournamentNode::AllocateTeamData* GetCustomTeamData(){ return m_pCustomTeamData; }

public:
	UnAllocateButton();
	~UnAllocateButton();
};
//////////////////////////////////////////////////////////////////////////
class UnAllocateWnd : public ioWnd
{
public:
	enum
	{
		ID_TEAM_BTN01 = 100,
		ID_TEAM_BTN02 = 101,
		ID_TEAM_BTN03 = 102,
		ID_TEAM_BTN04 = 103,
		ID_TEAM_BTN05 = 104,
		ID_TEAM_BTN06 = 105,
		ID_TEAM_BTN07 = 106,
		ID_TEAM_BTN08 = 107,
		ID_TEAM_BTN09 = 108,
		ID_TEAM_BTN10 = 109,
		ID_TEAM_BTN11 = 110,
		ID_TEAM_BTN12 = 111,
		ID_TEAM_BTN13 = 112,
		ID_TEAM_BTN14 = 113,
		ID_TEAM_BTN15 = 114,
		ID_TEAM_BTN16 = 115,

		ID_VERT_SCROLL = 1000,
	};

	enum
	{
		TOURNAMENT_REQUEST_TIME	= 20000,		
	};


protected:
	DWORD m_dwTournamentIndex;
	DWORD m_dwUnAllcateTeamCount;

	int m_iCurPage;
	int m_iMaxPage;

	// 동기화 체크 타이머
	DWORD m_dwCustomTeamDataSyncTimer;	
	TournamentNode::AllocateTeamDataMap m_AllocateTeamDataMap;

public:
	void SetCustomTournamentData( DWORD dwTourIndex );

public:
	virtual void iwm_hide();
	virtual void iwm_show();

	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_wheel( int zDelta );
	virtual void iwm_vscroll(DWORD dwID, int curPos );

	virtual bool iwm_dropitem( ioDragItem *pItem, const ioMouse& mouse, ioWnd *pStartWnd );
	virtual IWDropEffect iwm_dragover( ioDragItem *pItem, const ioMouse& mouse );

public:
	virtual void OnProcess( float fTimePerSec );

public:
	inline DWORD GetUnAllocateTeamCount(){ return m_dwUnAllcateTeamCount; }

public:
	void NextPage();
	void PrevPage();
	void SetMaxPage();

	void SetCurPageParsingData();

	bool IsEmptyCustomData( short Position );
	void SetCustomData( DWORD dwTourIdx, ioHashString& szTeamName, short Position );

public:
	void SetDataSync();
	void OnTeamDataSyncProcess();

public:
	UnAllocateWnd();
	virtual ~UnAllocateWnd();
};
//////////////////////////////////////////////////////////////////////////
class ioAllocatedButtonDragItem;
class AllocatedButton : public ioButton
{
protected:
	TournamentNode::AllocateTeamData* m_pCustomTeamData;
	ioAllocatedButtonDragItem *m_pDragItem;

	short m_CurTourPosition;

protected:
	virtual void iwm_hide();

	virtual void iwm_mouseleave( const ioMouse& mouse );
	virtual void iwm_mouseover( const ioMouse& mouse );
	virtual IWDropEffect iwm_dragover( ioDragItem *pItem, const ioMouse& mouse );
	virtual bool iwm_dropitem( ioDragItem *pItem, const ioMouse& mouse, ioWnd *pStartWnd );	

public:
	void SetDragItem();
	inline ioAllocatedButtonDragItem* GetDragItem(){ return m_pDragItem; }

	void SetCustomTeamData( TournamentNode::AllocateTeamData* TeamData );
	TournamentNode::AllocateTeamData* GetCustomTeamData(){ return m_pCustomTeamData; }

	DWORD GetCurTourPosition(){ return m_CurTourPosition; }
	void SetCurTourPosition( short CurTourPosition ){ m_CurTourPosition = CurTourPosition; }

	void SetClickRemove();

public:
	AllocatedButton();
	~AllocatedButton();
};
//////////////////////////////////////////////////////////////////////////
class AllocatedWnd : public ioWnd
{
public:
	enum
	{
		ID_TEAM_BTN01 = 100,
		ID_TEAM_BTN02 = 101,
		ID_TEAM_BTN03 = 102,
		ID_TEAM_BTN04 = 103,
		ID_TEAM_BTN05 = 104,
		ID_TEAM_BTN06 = 105,
		ID_TEAM_BTN07 = 106,
		ID_TEAM_BTN08 = 107,
		ID_TEAM_BTN09 = 108,
		ID_TEAM_BTN10 = 109,
		ID_TEAM_BTN11 = 110,
		ID_TEAM_BTN12 = 111,
		ID_TEAM_BTN13 = 112,
		ID_TEAM_BTN14 = 113,
		ID_TEAM_BTN15 = 114,
		ID_TEAM_BTN16 = 115,

		ID_VERT_SCROLL = 1000,
	};

protected:
	TournamentNode::AllocateTeamDataPtrMap m_CustomTeamDataPtrMap;

	int m_iCurPage;
	int m_iMaxPage;

	DWORD m_dwStartTeamCount;

	ioUIRenderImage* m_pTourLine;

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

	virtual void iwm_show();
	virtual void iwm_hide();

	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_wheel( int zDelta );
	virtual void iwm_vscroll(DWORD dwID, int curPos );

	virtual IWDropEffect iwm_dragover( ioDragItem *pItem, const ioMouse& mouse );
	virtual bool iwm_dropitem( ioDragItem *pItem, const ioMouse& mouse, ioWnd *pStartWnd );
	virtual void iwm_dropdone( ioDragItem *pItem, ioWnd *pDroppedWnd, bool bDropped );

public:
	//virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void NextPage();
	void PrevPage();

public:
	void SetCustomTournamentData( DWORD dwTourIndex );
	DWORD GetAllocateCount();

	inline const TournamentNode::AllocateTeamDataPtrMap& GetCustomTeamDataPtrMap() const { return m_CustomTeamDataPtrMap; }

protected:
	void RemoveEmptyData();
	void SetCurPageRefreshData();
	void InsertCustomTeamData( short Pos, TournamentNode::AllocateTeamData* pData );

protected:
	void SetAllocated( ioUnAllocateButtonDragItem *pUnAllocateItem, short CurPos );
	void SwapAllocated( ioAllocatedButtonDragItem *pAllocatedItem, short CurPos );

public:
	AllocatedWnd();
	virtual ~AllocatedWnd();
};

//////////////////////////////////////////////////////////////////////////
class CustomTournamentAllocateResultWnd : public ioWnd 
{
public:
	enum
	{
		ID_EXIT  = 1,
		ID_CLOSE = 2,
	};

protected:
	ioUIRenderImage *m_pMarkBack;
	ioUIRenderImage *m_pMark;

	DWORD m_dwTournamentIndex;	

public:	
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnRender();

public:
	void SetCustomTournamentData( DWORD dwTournamentIndex );

public:
	CustomTournamentAllocateResultWnd();
	virtual ~CustomTournamentAllocateResultWnd();
};

//////////////////////////////////////////////////////////////////////////

class CustomTournamentRewardButton : public ioButton
{
	enum
	{
		TEXT_LINE_MAX = 2,

		TWINKLE_TIME = 250,
	};

protected:
	DWORD m_dwEtcItemType;

	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pSubIcon;
	ioUIRenderImage *m_pNoneIcon;
	ioUIRenderImage *m_pTournamentCoinIcon;
	ioUIRenderImage *m_pSelectCheck;
	ioUIRenderFrame *m_pExDisable;
	bool m_bChecked;

	StringVector m_szTitleNameVec;
	ioHashString m_szRewardItemPirce;

	DWORD m_dwTournamentCoinSize;
	DWORD m_dwRewardItemPirce;

protected:
	static DWORD m_dwTwinkeTime;
	static bool  m_bTwinke;

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

public:
	virtual void iwm_mouseover( const ioMouse& mouse );
	virtual void iwm_mouseleave( const ioMouse& mouse );

public:
	inline void SetCheck( bool bCheck ){ m_bChecked = bCheck; }
	inline bool isChecked(){ return m_bChecked; }

public:
	void SetRewardItem( DWORD dwEtcItemType );

public:
	inline DWORD GeEtcItemType(){ return m_dwEtcItemType; }

protected:
	virtual void OnProcess( float fTimePerSec );

	virtual void OnRender();
	virtual void OnDrawDisabled( int iXPos, int iYPos );

protected:
	void OnIconRender( int iXPos, int iYPos );
	void OnTextRender( int iXPos, int iYPos );
	void OnCheckRender( int iXPos, int iYPos );

public:
	CustomTournamentRewardButton();
	virtual ~CustomTournamentRewardButton();
};

//////////////////////////////////////////////////////////////////////////

class CustomTournamentRewardLabel : public ioButton
{
protected:
	ioHashString m_szLabel;
	bool m_bPlus;
	int  m_iInfoArrayPos;

public:
	void SetLabel( const ioHashString& szLabel, int iXPos, int iYPos, int iWidth, int iInfoArrayPos, bool bPlus = false );	
	inline DWORD GetInfoArrayPos(){ return m_iInfoArrayPos; }

public:
	virtual void iwm_mouseover( const ioMouse& mouse );
	virtual void iwm_mouseleave( const ioMouse& mouse );

protected:
	virtual void OnRender();

public:
	CustomTournamentRewardLabel();
	~CustomTournamentRewardLabel();
};

//////////////////////////////////////////////////////////////////////////

class CustomTournamentRewardToolTip : public NewShopToolTip
{
protected:
	DWORD m_dwCurWndID;

protected:
	virtual void iwm_show();
	virtual void OnProcess( float fTimePerSec );

public:
	CustomTournamentRewardToolTip();
	~CustomTournamentRewardToolTip();
};

//////////////////////////////////////////////////////////////////////////

struct TournamentRewardItem;
class CustomTournamentRewardSelectWnd : public ioWnd
{
protected:
	enum
	{
		ID_EXIT			 = 1,
		ID_HEART_CHARGE  = 2,
		ID_LEFT			 = 3,
		ID_RIGHT		 = 4,
		ID_BUY			 = 5,
		ID_CANCEL		 = 6,

		ID_REWARD01_CHECK = 10,
		ID_REWARD02_CHECK = 11,
		ID_REWARD03_CHECK = 12,
		ID_REWARD04_CHECK = 13,
		ID_REWARD05_CHECK = 14,
		ID_REWARD06_CHECK = 15,
		ID_REWARD07_CHECK = 16,
		ID_REWARD08_CHECK = 17,
		ID_REWARD09_CHECK = 18,
		ID_REWARD10_CHECK = 19,
		ID_REWARD11_CHECK = 20,
		ID_REWARD12_CHECK = 21,
		ID_REWARD13_CHECK = 22,
		ID_REWARD14_CHECK = 23,
		ID_REWARD15_CHECK = 24,
		ID_REWARD16_CHECK = 25,

		ID_LABEL1		  = 100,
		ID_LABEL2		  = 101,
		ID_LABEL3		  = 102,
		ID_LABEL4		  = 103,
	};

	enum
	{
		LIS_ID_SUB  = 10,
		LIST_MAX    = 16,

		LABEL_ID_SUB    = 100,
		LABEL_MAX       = 4,
		LABEL_GAP	    = 15,
		LABEL_WIDTH_CUT = 84,

		TOURCOIN_ICON_GAP = 2,
	};

	enum
	{
		NONE_ETC_ITEM = 0,
	};

	typedef std::map<int,DWORD> SelectRewardPosMap;
	typedef std::vector<NewShopItemInfo> vNewShopItemInfoVec;

protected:
	ioUIRenderImage *m_pLine;
	ioUIRenderImage *m_pTournamentCoinIcon;

protected:
	SelectRewardPosMap m_SelectRewardPosMap;
	vNewShopItemInfoVec m_vNewShopItemInfo;

	DWORD m_dwTournamentRound;
	DWORD m_dwStartTeamCnt;
	DWORD m_dwTournaemntIndex;	
	DWORD m_dwMaxPlayer;
	DWORD m_dwBuyMax;
	DWORD m_dwSelectRewardPrice;
	DWORD m_dwTotalRewardPrice;

	BYTE m_TournamentPos;
	int m_iMaxPage;
	int m_iCurPage;

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void iwm_show();

public:
	virtual void iwm_create();
	virtual void iwm_wheel( int zDelta );
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();
	void OnPirceRender( int iXPos, int iYPos );

public:
	void SetCustomTournamentData( DWORD dwTournamentIdx, DWORD dwTournamentPos, DWORD dwBuyMax );
	void UpdatePage();
	void UpdateLabel();

	void UpdateLabelNormal( DWORD WndID, int iPos, ioHashString szItemName, int& iPosX, int& iPosY, DWORD dwEtcItemType );
	void UpdateLabelWidthCut( DWORD WndID, int iPos, ioHashString szItemName, int& iPosX, int& iPosY, DWORD dwEtcItemType );
	void UpdateLabelNone( DWORD WndID );

	void UpdateTournamentRewardPrice();
	void UpdateEtcItemInfoVec();

	DWORD FindSelectReward( int iPos );

	void InsertSelectReward( int iPos, DWORD dwEtcType );
	void RemoveSelectReward( int iPos );
	bool CheckSelectReward( DWORD dwID );

	void SendBuyRewardItem();
	bool GetCurItemInfo( OUT NewShopItemInfo &rkInfo, IN DWORD dwWndID );

public:
	void NextPage();
	void PrevPage();	

public:
	CustomTournamentRewardSelectWnd();
	virtual ~CustomTournamentRewardSelectWnd();
};

//////////////////////////////////////////////////////////////////////////
class TournamentCoinItemNeedPopup : public ioWnd
{
public:
	enum
	{
		ID_EXIT			   = 1,
		ID_CREATE_ITEM_BUY = 2,
		ID_CANCEL		   = 3,
	};

protected:
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pIcon;

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnRender();

public:
	TournamentCoinItemNeedPopup();
	virtual ~TournamentCoinItemNeedPopup();
};

//////////////////////////////////////////////////////////////////////////
class TournamentAlarmCustomRewardWnd : public ioWnd
{
public:
	enum
	{
		ID_CLOSE	 = 1,
		ID_CONFIRM	 = 2,
	};

private:
	ioUIRenderImage *m_pDevideDot;
	ioUIRenderImage *m_pPresentBack;
	ioUIRenderImage *m_pPresentNone;
	ioUIRenderImage *m_pBackCircle;
	ioUIRenderImage *m_pBackHub;
	ioUIRenderImage *m_pCustomReward;
	ioUIRenderImage *m_pConfettiBig;
	ioUIRenderImage *m_pConfettiSmall;

	float m_fDevideOffsetX;
	float m_fDevideOffsetY;

	DWORD m_dwDevideVertCnt;
	DWORD m_dwDevideHorzCnt;

	float m_fPresentOffsetX;
	float m_fPresentScaleRate;
	DWORD m_dwPresentBackCnt;

protected:
	struct CustomRewardData
	{
		int   m_TournamentPos;
		DWORD m_TournamentRound;
		ioHashString szTtitle;

		DWORDVec m_dwCustomRewardVec;

		typedef std::vector< ioUIRenderImage*> ioUIRenderImageVec;
		ioUIRenderImageVec m_vUIRenderImageVec;
		ioUIRenderImageVec m_vUIRenderSubImageVec;

		CustomRewardData()
		{
			m_TournamentPos = 0;
			m_TournamentRound = 0;
		}
	};
	typedef std::vector<CustomRewardData> CustomRewardDataVec;

protected:
	CustomRewardDataVec m_CustomRewardDataVec;

protected:
	void DestroyCsutomIconList();
	void DestroyCsutomIcon( CustomRewardData& rkData );

public:
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	void AddCustomTournamentRewardData( DWORDVec& vEtcItemCodeVec, ioHashString szTourTitle, short MaxRound, int iTourPos );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void OnDevideDotRender( int iXPos, int iYPos );
	void OnPresentRender( int iXPos, int iYPos, bool bReward = true );
	void OnBackRender( int iXPos, int iYPos,  bool bReward = true );
	void OnCustomIconRender( int iXPos, int iYPos );

protected:
	void OnCustomRewardNoneRender( int iXPos, int iYPos );
	void OnCustomRewardResultRender( int iXPos, int iYPos );
	void OnCustomNoneRender( int iXPos, int iYPos );

public:
	TournamentAlarmCustomRewardWnd();
	virtual ~TournamentAlarmCustomRewardWnd();
};
//////////////////////////////////////////////////////////////////////////
class TournamentDetailInnerCustomWnd : public ioWnd
{
private:
	ioUIRenderImage *m_pDevideDot;
	ioUIRenderImage *m_pPresentBack;
	ioUIRenderImage *m_pPresentNone;
	ioUIRenderImage *m_pBackCircle;
	ioUIRenderImage *m_pBackHub;
	ioUIRenderImage *m_pCustomReward;
	ioUIRenderImage *m_pConfettiBig;
	ioUIRenderImage *m_pConfettiSmall;

	float m_fDevideOffsetX;
	float m_fDevideOffsetY;

	DWORD m_dwDevideVertCnt;
	DWORD m_dwDevideHorzCnt;

	float m_fPresentOffsetX;
	float m_fPresentScaleRate;
	DWORD m_dwPresentBackCnt;

protected:
	DWORD m_dwTournamentIndex;
	DWORD m_dwTournamentMaxRound;
	DWORD m_TournamentPos;
	DWORD m_TournamentRound;
	byte m_TournamentState;

protected:
	DWORDVec m_dwCustomRewardVec;
	typedef std::vector< ioUIRenderImage*> ioUIRenderImageVec;
	ioUIRenderImageVec m_vUIRenderImageVec;
	ioUIRenderImageVec m_vUIRenderSubImageVec;

protected:
	void DestroyCsutomIconList();

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	void SetTournamentData( DWORD dwTourIndex );

	void ProcessUpdateTournament();
	void ProcessSyncCheckRewardData();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void OnDevideDotRender( int iXPos, int iYPos );
	void OnPresentRender( int iXPos, int iYPos, bool bReward = true );
	void OnBackRender( int iXPos, int iYPos,  bool bReward = true );
	void OnCustomIconRender( int iXPos, int iYPos );

protected:
	void OnCustomRewardNoneRender( int iXPos, int iYPos );
	void OnCustomRewardResultRender( int iXPos, int iYPos );
	void OnCustomNoneRender( int iXPos, int iYPos );

public:
	TournamentDetailInnerCustomWnd();
	virtual ~TournamentDetailInnerCustomWnd();
};

//////////////////////////////////////////////////////////////////////////

class TournamentDetailInnerAllocateWnd : public ioWnd
{
private:
	ioUIRenderImage *m_pBackCircle;
	ioUIRenderImage *m_pBackHub;
	ioUIRenderImage *m_pIcon;

	float m_fDevideOffsetX;
	float m_fDevideOffsetY;

	DWORD m_dwDevideVertCnt;
	DWORD m_dwDevideHorzCnt;

	float m_fPresentOffsetX;
	float m_fPresentScaleRate;
	DWORD m_dwPresentBackCnt;

protected:
	DWORD m_dwTournamentIndex;
	DWORD m_dwTournamentMaxRound;
	DWORD m_TournamentPos;
	DWORD m_TournamentRound;
	DWORD m_dwRoundTeamCount;

	byte m_TournamentState;

	bool m_bAllocate;

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

public:
	void SetTournamentData( DWORD dwTourIndex );
	void RoundCheckProcess();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void OnBackRender( int iXPos, int iYPos,  bool bReward = true );
	void OnCustomIconRender( int iXPos, int iYPos );

	void OnAllocateNoneRender( int iXPos, int iYPos );
	void OAllocateRender( int iXPos, int iYPos );
	void OAllocateDone( int iXPos, int iYPos );

public:
	TournamentDetailInnerAllocateWnd();
	virtual ~TournamentDetailInnerAllocateWnd();
};