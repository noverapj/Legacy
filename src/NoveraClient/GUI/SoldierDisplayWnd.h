#pragma once

class SoldierDisplayChangeWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT = 1,
		ID_CHANGE_OK = 2,
		ID_CHANGE_CANCEL = 3,
	};

protected:
	ioPlayStage *m_pPlayStage;
	int m_iClassType;
	
public:
	void SetPlayStage( ioPlayStage *pPlayStage );

public:
	virtual void iwm_hide();
	virtual bool iwm_spacebar();

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void SetClassType( int iClassType ){ m_iClassType = iClassType; }

public:
	SoldierDisplayChangeWnd();
	virtual ~SoldierDisplayChangeWnd();
};
//////////////////////////////////////////////////////////////////////////
class SoldierDisplaySelectBtn : public ioButton
{
protected:
	ioUIRenderFrame *m_pCustomNormal;
	ioUIRenderFrame *m_pCustomOver;

	ioUIRenderImage *m_pNone;
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pSoldierIcon;
	ioUIRenderImage *m_pSoldierSubIcon;
	ioUIRenderImage *m_pDisplayIcon;

protected:
	int  m_iClassType;
	bool m_bHaveClass;
	int m_nGradeType;

public:
	void SetClassType( int iClassType );
	int  GetClassType(){ return m_iClassType; }

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

protected:
	virtual void OnRender();

public:
	SoldierDisplaySelectBtn();
	virtual ~SoldierDisplaySelectBtn();
};
//////////////////////////////////////////////////////////////////////////
class SoldierDisplaySelectWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT = 1,
		ID_LEFT_PAGE = 2,
		ID_RIGHT_PAGE= 3,

		ID_SOLDIER_BTN1 = 100,
		ID_SOLDIER_BTN30= 129,
	};

	enum
	{
		MAX_SOLDIER_BTN = 30,
		MAX_HORZ = 6,
		MAX_VERT = 5,
	};

protected:
	ioPlayStage *m_pPlayStage;
	IntVec m_CharList;
	int m_iCurPage;
	int m_iMaxPage;
	IntVec m_vExtraClassVec;

public:
	void SetPlayStage( ioPlayStage *pPlayStage );

protected:
	void SetBtnPos();
	void UpdateSetPage( int iCurPage );

public:
	virtual void iwm_show();
	virtual bool iwm_esc();

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	bool QuestGuide( int iClassType, float &rfXPos, float &rfYPos );

public:
	SoldierDisplaySelectWnd();
	virtual ~SoldierDisplaySelectWnd();
};
//////////////////////////////////////////////////////////////////////////
class SoldierDisplayAlarmWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT = 1,
		ID_CLOSE= 2,
		ID_SHOP = 3,
	};

protected:
	ioUIRenderImage *m_pSoldierIcon;
	ioUIRenderImage *m_pSoldierSubIcon;

protected:
	int		m_iClassType;
	bool	m_bHaveClass;

public:
	void SetClassType( int iClassType, bool bHaveClass );

public:
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	SoldierDisplayAlarmWnd();
	virtual ~SoldierDisplayAlarmWnd();
};
