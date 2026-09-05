#pragma once

class PCRoomAuthorityWnd : public ioWnd
{
public:
	enum
	{
		ID_SOLDIER_BTN   = 1,
	};
    
protected:
	ioUIRenderImage *m_pBackGround;
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pPCRoomIcon;
	ioUIRenderImage *m_pPCRoomPeso;
	ioUIRenderImage *m_pPCRoomExp;
	ioUIRenderImage *m_pSoldierIcon;
	ioUIRenderImage *m_pSoldierSubIcon;

protected:
	int m_iPesoBonus;
	int m_iExpBonus;

protected:
	static bool sg_bAutoShowPossible;

public:
	virtual void iwm_show();
	virtual bool iwm_spacebar();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	void OnBonusSoldier( int iXPos, int iYPos );
	void OnBonusPeso( int iXPos, int iYPos );
	void OnBonusExp( int iXPos, int iYPos );

protected:
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	static bool IsAutoShowPossible(){ return sg_bAutoShowPossible; }

public:
	PCRoomAuthorityWnd();
	virtual ~PCRoomAuthorityWnd();
};
//////////////////////////////////////////////////////////////////////////
class PCRoomSoldierBonusBtn : public ioButton
{
protected:
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pCharIcon;
	ioUIRenderImage *m_pCharSubIcon;

protected:
	int m_iClassType;

public:
	void SetClassType( int iClassType );
	int  GetClassType(){ return m_iClassType; }

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnRender();

public:
	PCRoomSoldierBonusBtn();
	virtual ~PCRoomSoldierBonusBtn();
};
//////////////////////////////////////////////////////////////////////////
class PCRoomSoldierBonusWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT         = 100,
		ID_LEFT_PAGE	= 1,
		ID_RIGHT_PAGE	= 2,

		ID_BONUS_CHAR_BTN1  = 10,
		ID_BONUS_CHAR_BTN2  = 11,
		ID_BONUS_CHAR_BTN3  = 12,
		ID_BONUS_CHAR_BTN4  = 13,
		ID_BONUS_CHAR_BTN5  = 14,
		ID_BONUS_CHAR_BTN6  = 15,
	};

	enum
	{
		MAX_PAGE_LIST = 6,
	};

protected:
	ioPlayStage *m_pPlayStage;

protected:
	int m_iSelectBtn;

	int m_iCurPage;
	int m_iMaxPage;
	IntVec m_ClassTypeList;

protected:
	void UpdateSetPage( int iCurPage );
	int  FindClassTypeBtn( bool bFirstBtn );
	int  LastClassTypeBtn();
	bool IsCanExerciseChar( bool bSelectWait );

public:
	virtual void iwm_show();
	virtual bool iwm_spacebar();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	inline int GetSelectBtn(){ return m_iSelectBtn; }

public:
	void SetPlayStage( ioPlayStage *pPlayStage );

public:
	PCRoomSoldierBonusWnd();
	virtual ~PCRoomSoldierBonusWnd();
};

