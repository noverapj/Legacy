#pragma once
#include "MyInventoryWnd.h"
class PracticeBtn : public ioButton
{
public:
	enum 
	{
		ID_ICON_BTN			= 1,
		ID_CUSTOM_BTN		= 2,
	};

	enum
	{
		MAX_DESC			= 3,
	};

protected:
	ioComplexStringPrinter m_szDescArray[3];
	ioUIRenderImage*	m_pIconBack;
	ioUIRenderImage*	m_pStar_1;
	ioUIRenderImage*	m_pStar_2;
	ioUIRenderImage*	m_pStar_3;
	ioUIRenderImage*	m_pStar_Gray1;
	ioUIRenderImage*	m_pStar_Gray2;
	ioUIRenderImage*	m_pStar_Gray3;
	ioUIRenderFrame*	m_pSelectFrm;
	ioUIRenderFrame*	m_pOverFrm;
	vInventoryItemInfoList m_vInventoryItemInfoList;
	ioHashString m_szCountText;
	int m_iModeIndex;
	int m_iResult;
	int m_iMaxCount;
	bool m_bSelcet;

public:
	void InitBG( int iBGindex );
	void SetModeIndex( int iIndex ){m_iModeIndex = iIndex;}
	void SetGrade( int iResult ){m_iResult = iResult;}
	void SetSelect( bool bSelect = false ){ m_bSelcet = bSelect; }
	vInventoryItemInfoList GetItemInfo(){return m_vInventoryItemInfoList;}

protected:
	void ClearDesc();
	void RenderDesc( int iXPos, int iYPos );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

public:
	PracticeBtn();
	virtual ~PracticeBtn();
};

class PracticeLobbyInfoWnd : public ioWnd
{
	enum
	{
		ID_REWARD_HELP = 1,
		ID_RANK_HELP = 2,
		ID_START_BTN = 3,
		ID_REWARD_BTN1 = 4,
		ID_REWARD_BTN2 = 5,
		ID_REWARD_BTN3 = 6,
		ID_RANK_BTN = 7,

		ID_REWARD_HELP_TOOLTIP = 101,
		ID_RANKING_HELP_TOOLTIP = 102,
		ID_REWARD_TOOLTIP = 103,

		ID_SCROLL = 1000,
	};
	// 로비의 수련장 탭에서 표시할 정보 UI
protected:
	ioPlayStage *m_pPlayStage;
	vInventoryItemInfoList m_vInventoryItemInfoList;
	//수련장 ui
	typedef std::vector< IMsgItem* > ChatMsgList;
	ChatMsgList m_ChatList;
	
	ioUIRenderImage *m_pImageBack;
	ioUIRenderImage *m_pRewardHelp;
	ioUIRenderImage *m_pRankHelp;
	ioUIRenderImage *m_pRankBg;
	ioUIRenderImage *m_pRewardBg;
	ioUIRenderImage *m_pRewardIconBg;
	ioUIRenderImage	*m_pRewardIcon;
	ioUIRenderImage	*m_pStar;
	ioUIRenderImage	*m_pGrayStar;

	typedef std::map< ioHashString, ioUIRenderImage * > PracticeIconMap; 
	PracticeIconMap			m_RewardIconMap;

	DWORD m_dwOveredIcon;
	DWORD m_dwBusyTime;
	int m_iCurPos;
protected:
	ioHashString	m_szIconName[3];
	IoString m_szGuide;

	ioHashString	m_szText[8];
	ioHashString	m_szRank;

	bool  m_bScreen;
	bool  m_bBusy;
	DWORD m_dwScreenTime;
	DWORD m_dwIndex;


public:
	ioUIRenderImage* GetImage( const ioHashString &szIconName );
	void SetINIText(ioINILoader kLoader, DWORD dwIndex);
	void SetPlayStage( ioPlayStage *pPlayStage );
	void SetModeIndex(DWORD dwIndex);
	void SetRewardList(vInventoryItemInfoList&	vItemInfoList);	
	void ClearIconMap();
	bool GetCurItemInfo( OUT InventoryItemInfo &rkInfo, IN DWORD dwItemInfoID );
	DWORD GetOveredIcon(){return m_dwOveredIcon;}
//??? ui
protected:
	void SetHelpText();

public:
	virtual void iwm_create();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_wheel( int zDelta );
	virtual void iwm_vscroll( DWORD dwID, int curPos );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

public:
	PracticeLobbyInfoWnd();
	virtual ~PracticeLobbyInfoWnd();
};

class PracticeMainWnd :	public ioWnd
{
public:
	enum
	{
		ID_PRE_PAGE_BTN		= 201,
		ID_NEXT_PAGE_BTN	= 202,

		ID_PRACTICE_1		= 101,
		ID_PRACTICE_2		= 102,
		ID_PRACTICE_3		= 103,
		ID_PRACTICE_4		= 104,
		ID_PRACTICE_5		= 105,
		ID_PRACTICE_6		= 106,
		ID_PRACTICE_7		= 107,
		ID_PRACTICE_8		= 108,
		//현재 빈칸용
		ID_PRACTICE_9		= 109,
		ID_PRACTICE_10		= 110,
		ID_PRACTICE_END			,
		ID_EMPTY_BTN		=999,
		MAX_DESC		= 2,
	};
private:
	PracticeLobbyInfoWnd* m_pLobby;


protected:
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pIcon;
	ioHashString m_szDescArray[MAX_DESC];
	ioComplexStringPrinter	m_PagePrinter;
	bool			m_bHaveEmpty;
	int				m_iCurPage;
	int				m_iMaxPage;
	int				m_iPosX;
	int				m_iPosY;
	int				m_iPageYPos;
	int				m_iSelectBtn;
	DWORD m_dwTabID;

protected:
	void InitPracticeBtn(DWORD dwIndex);
	void ClearDesc();
	void PrevPage();
	void NextPage();
	void CheckPracticeBtn( DWORD dwID );

public:
	void PracticeUiMove( int nAddYPos = 0 );
	void SetLobbyInfo(PracticeLobbyInfoWnd* pLobby);
	

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );	// 2019-07-09
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();



public:
	PracticeMainWnd(void);
	virtual ~PracticeMainWnd(void);
};

class PracticeRankingWnd :	public ioWnd
{
public:
	enum{	
			ID_EXIT = 3,
			ID_PAGE_AMOUNT =5,
			ID_PREV_BTN = 201,
			ID_NEXT_BTN = 202,		
		};
protected:
	typedef std::map< ioHashString, ioUIRenderImage * >PracticeIconMap;
	PracticeIconMap			m_RewardIconMap;
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pLine;
	ioUIRenderImage *m_pRankTextBg1;
	ioUIRenderImage *m_pRankTextBg2;
	ioUIRenderImage *m_pRankTextBg3;
	ioUIRenderImage *m_pRewardTextBg1;
	ioUIRenderImage *m_pRewardTextBg2;
	ioUIRenderImage *m_pRewardTextBg3;
	ioUIRenderImage *m_pRewardIconBg;

protected:
	ioHashString	m_szIconName[4];
	ioHashString	m_szModeName[8];

	ioHashString	m_szRank;
	ioHashString	m_szTextInfo[3];
	ioHashString	m_szRewardInfo[4];
	ioHashString	m_szRewardInfo2[4];
	ioHashString	m_szRewardNameInfo[4];
	ioHashString	m_szRewardDetail[4];
	ioHashString	m_szRewardTitle[4];
	ioHashString	m_szHelp;

	std::map<int, int> m_mapReward;

	int             m_iItemCode[4];
	int             m_iRewardCount[4];
	int				m_iCurPage;
	int				m_iMaxPage;
	int				m_iPosX;
	int				m_iPosY;
	int				m_iPageYPos;
	int				m_iModeMax;
	DWORD m_dwTabID;

protected:
	ioUIRenderImage* GetImage( const ioHashString &szIconName );
	void SetPage(bool bNext);
	void RenderRankBoard(int iX, int iY);
	void RenderRewardIconBoard(int iX, int iY);
	void RenderRewardBoard(int iX, int iY);
	void ClearIconMap();	
	
public:
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void ParseExtraInfo( ioXMLElement &xElement );
protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	PracticeRankingWnd(void);
	virtual ~PracticeRankingWnd(void);
};