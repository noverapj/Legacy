#pragma once

#include "../io3DEngine/ioStringConverter.h"
#include "../io3DEngine/ioButton.h"

class ioPlayStage;
class ioUIRenderImage;
class ioUIRenderFrame;

class TutorialBtn : public ioButton
{
protected:
	DWORD m_dwScreenTimer;
	DWORD m_dwCurTimer;
	bool  m_bScreen;

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	virtual void OnDrawNormal( int iXPos, int iYPos );

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	TutorialBtn();
	virtual ~TutorialBtn();
};

class TutorialWnd : public ioWnd
{
	public:
	enum
	{
		ID_EXIT  = 1,
		ID_START = 2,
		ID_NEXT  = 3,
		ID_CLEAR = 4,
	};
protected:
	ioUIRenderFrame *m_pHelpBackFrm;
	ioUIRenderImage *m_pDevK;
	ioUIRenderImage *m_pDevKBack;

protected:    //텍스트 순차적 표시
	DWORD     m_dwTextTickTime;
	DWORD     m_dwCurTextTickTime;
	int       m_iCurTextTickCount;
	int       m_iCurTextLineCount;
	int       m_iShowBtnID;
	ioHashString m_szTextTickSnd;
	DWORD     m_dwSkipBtnShowTime;
	DWORD     m_dwCurSkipBtnShowTime;
	DWORD     m_dwAutoNextTime;
	DWORD     m_dwCurAutoNextTime;
	int       m_iCurUseSkipBtnID;
	
	struct StringTick
	{
		char m_szCopyDesc[MAX_PATH];
		StringTick()
		{
			memset( m_szCopyDesc, 0, sizeof( m_szCopyDesc ) );
		}
	};
	typedef std::vector< StringTick > vStringTick;
	vStringTick m_vStringTickList;
	
	void SetTextTick();
	void ProcessTextTick();

protected:
	int       m_iCurStep;
	int       m_iMaxStep;
	int       m_iCurValue;
	int       m_iMaxValue;
	StringVector m_DescList;
	ioHashString m_szHelp;

	float m_fScale;
	float m_fFontYGap;
	DWORD m_dwCurTimer;

	void SetCountUp();
	void SplitDescription( const ioHashString &szDesc );
	void RenderTutorialInfo();
	void ShowButtonID( int iBtnID );
	void SetSkipBtn( int iCurStep, int iMaxStep );
	void SetBattleProcess();
public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual bool iwm_spacebar();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void SetTutorialStep( int iCurStep, int iCurValue, int iMaxStep, int iMaxValue,
						  ioHashString &szTitle, ioHashString &szHelp1, ioHashString &szHelp2, ioHashString &szComplete );
	bool IsTutorialEsc();
	bool IsTextTick();
public:
	TutorialWnd();
	virtual ~TutorialWnd();
};

//////////////////////////////////////////////////////////////////////////
class TutorialKeyboardWnd : public ioWnd
{
protected:
	ioUIRenderImage *m_pButtonNormal;
	ioUIRenderImage *m_pButtonOver;
	ioUIRenderImage *m_pArrowLeft;
	ioUIRenderImage *m_pArrowUp;

protected:
	bool  m_bShowProcess;
	float m_fProcessRate;
	DWORD m_dwProcessTimer;

protected:
	bool  m_bKeyA;
	bool  m_bKeyS;
	bool  m_bKeyD;
	bool  m_bKeyLeft;
	bool  m_bKeyRight;
	bool  m_bKeyUp;
	bool  m_bKeyDown;

public:
	virtual void iwm_show();
	virtual void iwm_hide();

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

protected:
	virtual void OnProcess( float fTimePerSec );
	void OnProcessShow();
	void OnProcessKeyCheck();

protected:
	virtual void OnRender();
	void OnRenderASD( int iXPos, int iYPos );
	void OnRenderArrow( int iXPos, int iYPos );

public:
	void SetHideProcess();

public:
	TutorialKeyboardWnd();
	virtual ~TutorialKeyboardWnd();
};