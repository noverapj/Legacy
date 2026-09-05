#pragma once
#include "ioWnd.h"
#include "ioWndEX.h"
#include "ioButton.h"

class __EX WndMove
{
public:
	enum
	{
		WMH_NONE       = 0,
		WMH_CHECK      = 1,
		WMH_MOVE       = 2,
	};

protected:
	float m_fSpeed;
	float m_fWeight;
	float m_fAngle;
	DWORD m_dwState;
	DWORD m_dwCurTime;
	DWORD m_dwMouseUpTime;
	float m_fXPos;
	float m_fYPos;
	ioWnd *m_pHelpWnd;

	POINT m_OldMouse;

protected:
	int ProcessCollision( int iCurrScreenWidth, int iCurrScreenHeight );
	void ProcessMoveCollision( int iCurrScreenWidth, int iCurrScreenHeight );

public:
	void SetWnd( ioWnd *pWnd );
	void SetState( DWORD dwState, CONST POINT& MousePos );

	void SetSpeed( float fSpeed )  { m_fSpeed = fSpeed; }
	void SetWeight( float fWeight ){ m_fWeight = fWeight; }
	void SetAngle( float fAngle )  { m_fAngle = fAngle; }

	DWORD GetState(){ return m_dwState; }

public:
	void MouseLDown( CONST POINT& MousePos );
	bool MouseLUp( CONST POINT& MousePos );

public:
	void ProcessMove( const ioMouse& rkMouse );

public:
	WndMove();
	virtual ~WndMove();
};

//---------------------------------------------------------------------------------------

class __EX ioMovingWnd : public ioWnd
{
protected:
	WndMove m_MoveWnd;

public:
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

public:
	virtual void iwm_lbuttondown( const ioMouse& mouse );
	virtual void iwm_lbuttonup( const ioMouse& mouse );
	bool IsMoving();

protected:
	virtual void OnMouseProcess( const ioMouse& mouse );

public:
	ioMovingWnd();
	virtual ~ioMovingWnd();
};

//---------------------------------------------------------------------------------------

class __EX ioMovingWndEX : public ioWndEX
{
protected:
	WndMove m_MoveWnd;

public:
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

public:
	virtual void iwm_lbuttondown( const ioMouse& mouse );
	virtual void iwm_lbuttonup( const ioMouse& mouse );
	bool IsMoving();

protected:
	virtual void OnMouseProcess( const ioMouse& mouse );

public:
	ioMovingWndEX();
	virtual ~ioMovingWndEX();
};

//---------------------------------------------------------------------------------------

class __EX ioMovingBtn : public ioScaleButton
{
protected:
	WndMove m_MoveWnd;

public:
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

public:
	virtual void iwm_lbuttondown( const ioMouse& mouse );
	virtual void iwm_lbuttonup( const ioMouse& mouse );
	virtual void iwm_mouseover( const ioMouse& mouse );

public:
	virtual bool IsCanMouseReaction() const;

protected:
	virtual void OnMouseProcess( const ioMouse& mouse );

public:
	ioMovingBtn();
	virtual ~ioMovingBtn();
};

//---------------------------------------------------------------------------------------

class __EX ioMovingNormalBtn : public ioButton
{
protected:
	WndMove m_MoveWnd;

public:
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

public:
	virtual void iwm_lbuttondown( const ioMouse& mouse );
	virtual void iwm_lbuttonup( const ioMouse& mouse );
	virtual void iwm_mouseover( const ioMouse& mouse );

public:
	virtual bool IsCanMouseReaction() const;

protected:
	virtual void OnMouseProcess( const ioMouse& mouse );

public:
	ioMovingNormalBtn();
	virtual ~ioMovingNormalBtn();
};