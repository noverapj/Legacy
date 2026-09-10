#pragma once

class InvenSoldierSelectBuyWnd : public ioWnd
{
public:
	enum
	{
		ID_BUY_BTN          = 1,
		ID_CHAR_SELECT_BTN	= 2,

		ID_SOLDIER_WND      = 1000,
		ID_PULLDOWN_WND     = 1001,
	};

protected:
	DWORD        m_dwEtcItemType;
	ManualMgr    *m_pManual;

	bool m_bSetShowPullDownWnd;

protected:
	CHARACTER m_CharInfo;
	CHARACTER m_OverCharInfo;
	int m_iCurBtnUpClassType;
	int m_iCurBtnOverClassType;

protected:
	void SetPullDownMenu( ioWnd *pOpenBtn );
	void SetBtnTitle( int iClassType );
	void GetSoldierList( vNewShopPullDownItem &rkList );

protected:
	void PullDownBtnUP( ioWnd *pOpenBtn, int iListArray );
	void PullDownBtnOver( ioWnd *pOpenBtn, int iListArray );
	void PullDownBtnLeave();

	void PrintManual( int a_iXPos, int a_iYPos , float fScale );

public:
	void SetEtcItemType( DWORD dwEtcItemType );

public:
	virtual void iwm_show();
	virtual void iwm_hide();

public:
	virtual void iwm_create();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	InvenSoldierSelectBuyWnd();
	virtual ~InvenSoldierSelectBuyWnd();
};
//////////////////////////////////////////////////////////////////////////
class InvenSoldierSelectBuyAgreeWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT  = 1,
		ID_AGREE = 2,
		ID_CANCEL= 3,
	};

protected:
	ioUIRenderImage *m_pCharIcon;
	ioUIRenderImage *m_pCharSubIcon;
	ioUIRenderImage *m_pBackEffect;

protected:
	int   m_iClassType;
	DWORD m_dwEtcItemType;
	int m_nGradeType;

	ioHashString m_sSelectorTitle;

public:
	void SetAgree( int iClassType, DWORD dwEtcItemType );

protected:
	void SendUse();

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	virtual void OnRender();

public:
	InvenSoldierSelectBuyAgreeWnd();
	virtual ~InvenSoldierSelectBuyAgreeWnd();
};
//////////////////////////////////////////////////////////////////////////
class InvenSoldierSelectBuyResultWnd : public ioWnd
{
public:
	enum
	{
		ID_GO	= 1,
	};

protected:
	ioUIRenderImage *m_pCharIcon;
	ioUIRenderImage *m_pCharSubIcon;
	ioUIRenderImage *m_pBackEffect;

	ioHashString m_TitleName;
	ioHashString m_sClassTime;

	ManualMgr* m_pManual;
	DWORD m_dwEtcItemType;
	int m_nGradeType;

public:
	void ShowResult( int iClassType, int iClassTime, DWORD dwEtcItemType );

protected:
	void PrintManual( int a_iXPos, int a_iYPos , float fScale );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnRender();

public:
	InvenSoldierSelectBuyResultWnd();
	virtual ~InvenSoldierSelectBuyResultWnd();
};
//////////////////////////////////////////////////////////////////////////
class InvenSoldierExpBonusWnd : public ioWnd
{
public:
	enum
	{
		ID_SELECT_BTN       = 1,
		ID_CHAR_SELECT_BTN	= 2,

		ID_SOLDIER_WND      = 1000,
		ID_PULLDOWN_WND     = 1001,
	};

protected:
	DWORD        m_dwEtcItemType;
	ManualMgr    *m_pManual;

protected:
	CHARACTER m_CharInfo;
	CHARACTER m_OverCharInfo;
	int m_iCurBtnUpClassType;
	int m_iCurBtnOverClassType;
	int m_iClassLimitLevel;

protected:
	void SetPullDownMenu( ioWnd *pOpenBtn );
	void SetBtnTitle( int iClassType );
	void GetSoldierList( vNewShopPullDownItem &rkList );

protected:
	void PullDownBtnUP( ioWnd *pOpenBtn, int iListArray );
	void PullDownBtnOver( ioWnd *pOpenBtn, int iListArray );
	void PullDownBtnLeave();
	void PrintManual( int a_iXPos, int a_iYPos , float fScale );

public:
	void SetEtcItemType( DWORD dwEtcItemType );

public:
	virtual void iwm_show();
	virtual void iwm_hide();

public:
	virtual void iwm_create();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	InvenSoldierExpBonusWnd();
	virtual ~InvenSoldierExpBonusWnd();
};
//////////////////////////////////////////////////////////////////////////
class InvenSoldierExpBonusResultWnd : public ioWnd
{
public:
	enum
	{
		ID_GO = 1,
	};

protected:
	ioUIRenderImage *m_pCharIcon;
	ioUIRenderImage *m_pCharSubIcon;
	ioUIRenderImage *m_pBackEffect;

	ioHashString m_TitleName;
	ioHashString m_EtcItemName;

	DWORD m_dwEtcItemType;
	int   m_iClassType;
	int m_nGradeType;

public:
	void ShowResult( int iClassType, DWORD dwEtcItemType );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnRender();

public:
	InvenSoldierExpBonusResultWnd();
	virtual ~InvenSoldierExpBonusResultWnd();
};
