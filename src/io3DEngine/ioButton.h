

#ifndef _ioButton_h_
#define _ioButton_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

class ioUIImage;
class ioUIRenderFrame;
class ioUIRenderImage;

class __EX ioButton : public ioWnd
{
protected:
	ioUIRenderFrame *m_pNormalFrm;
	ioUIRenderFrame *m_pOverFrm;
	ioUIRenderFrame *m_pOverAddFrm;
	ioUIRenderFrame *m_pPushFrm;
	ioUIRenderFrame *m_pBackFrm;
	ioUIRenderFrame *m_pDisableFrm;
	ioUIRenderFrame *m_pExDisableNorFrm;
	ioUIRenderFrame *m_pExDisablePushFrm;

	ioUIRenderImage *m_pNormal;
	ioUIRenderImage *m_pOver;
	ioUIRenderImage *m_pOverAdd;
	ioUIRenderImage *m_pPush;
	ioUIRenderImage *m_pBack;
	ioUIRenderImage *m_pDisable;
	ioUIRenderImage *m_pExDisableNor;
	ioUIRenderImage *m_pExDisablePush;

	ioHashString m_szHelp;
	ioHashString m_szOverSound;
	ioHashString m_szClickSound;

	// 최상단에 찍힌다.
	typedef std::vector< ioUIRenderImage* > UIImageList;
	UIImageList m_NormalList;         // 일반 상태의 이미지
	UIImageList m_DisableList;       // 비활성 + 반비활성 상태의 이미지

	//Disable UI Title Offset
	bool	m_bTitleOffsetUse;
	int		m_nDisableTextStyleOffsetX;
	int		m_nDisableTextStyleOffsetY;

public:
	void SetHelp( const char *szHelp );
	const char* GetHelp();

	enum eRenerImg
	{
		eRI_NORMAL,
		eRI_NORMALADD,
		eRI_OVER,
		eRI_OVERADD,
		eRI_PUSH,
		eRI_BACK,
		eRI_DISABLE,
		eRI_EXDIASSABLENOR,
		eRI_EXDIASSABLEOVER,
		eRI_EXDISABLEPUSH,
	};

	ioUIRenderImage*	GetRenderImg( eRenerImg eType );
	void ChangeRenderImage( ioUIImage* pIcon );

public:
	inline bool IsOvered() const
	{
		if( m_bOver && !m_bClicked )
			return true;

		return false;
	}
	
	inline bool IsPushed() const
	{
		return m_bClicked;
	}

	inline bool IsDisabled() const
	{
		if( m_dwStyle & IWS_INACTIVE )
			return true;

		return false;
	}

	inline bool IsExDisabled() const
	{
		if( m_dwStyle & IWS_EXACTIVE )
			return true;

		return false;
	}

protected:
	void PrintNormalTitle( int iXPos, int iYPos );
	void PrintOverTitle( int iXPos, int iYPos );
	void PrintPushTitle( int iXPos, int iYPos );
	void PrintDisableTitle( int iXPos, int iYPos );

public:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void SetWindowAlpha( int iAlpha );

protected:
	virtual void OnRender();
	virtual void CheckFrameReSize( int iWidth, int iHeight, int iPrevWidth, int iPrevHeight );

	virtual void OnDrawNormal( int iXPos, int iYPos );
	virtual void OnDrawOvered( int iXPos, int iYPos );
	virtual void OnDrawOveredAdd( int iXPos, int iYPos );
	virtual void OnDrawPushed( int iXPos, int iYPos );
	virtual void OnDrawDisabled( int iXPos, int iYPos );
	virtual void OnDrawExDisabledNormal( int iXPos, int iYPos );
	virtual void OnDrawExDisabledOvered( int iXPos, int iYPos );
	virtual void OnDrawExDisabledPushed( int iXPos, int iYPos );
	virtual void OnDrawBack( int iXPos, int iYPos );

protected:
	virtual void OnDrawNormalList( int iXPos, int iYPos );
	virtual void OnDrawDisableList( int iXPos, int iYPos );

public:
	virtual void iwm_lbuttondown( const ioMouse& mouse );
	virtual void iwm_lbuttonup( const ioMouse& mouse );
	virtual void iwm_lbuttonDBLCLK( const ioMouse& mouse );
	virtual void iwm_rbuttondown( const ioMouse& mouse );
	virtual void iwm_rbuttonup( const ioMouse& mouse );
	virtual void iwm_mouseover( const ioMouse& mouse );

public:
	virtual DWORD GetWndType() const;

	//************************************************************************************************************************
	// Tool을 위한 인터페이스들
	virtual void SetSaveData( ioXMLElement& xml );
	DECL_NAME_INTERFACE_FOR_TOOL( Button, ioButton )
	virtual ioWnd* _Clone() { return new ioButton(); }
	virtual void Copy( ioWnd* wnd );
	// Renderframe ( Get )	
	virtual int GetNumFixedRenderFrame() const;
	virtual const char* GetFixedRenderFrameType( int index );
	virtual ioUIRenderElement** GetRenderFrame( const char* type ); 
	// FixedRenderimage ( Get )
	virtual int GetNumFixedRenderImage() const;
	virtual const char* GetFixedRenderImageType( int index );
	virtual ioUIRenderElement** GetRenderImage( const char* type );
	// etc
	const char* GetOverSound() const { return m_szOverSound.c_str(); }
	const char* GetClickSound() const { return m_szClickSound.c_str(); }
	void SetOverSound( const char* sound ) { m_szOverSound = sound; }
	void SetClickSound( const char* sound ) { m_szClickSound = sound; }
	//************************************************************************************************************************

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	ioButton();
	virtual ~ioButton();
};

class __EX ioScreenButton : public ioButton
{
protected:
	virtual void OnDrawOvered( int iXPos, int iYPos );
	virtual void OnDrawPushed( int iXPos, int iYPos );

public:
	ioScreenButton();
	virtual ~ioScreenButton();
};

class __EX ioConvexButton : public ioButton
{
protected:
	virtual void OnDrawOvered( int iXPos, int iYPos );
	virtual void OnDrawPushed( int iXPos, int iYPos );

public:
	ioConvexButton();
	virtual ~ioConvexButton();
};

class __EX ioScaleButton : public ioButton
{
protected:
	float m_fScaleRate;
	DWORD m_dwScaleTime;

	float m_fCurScaleRate;
	DWORD m_dwScaleStartTime;

public:
	void SetScaleRate( float fRate );
	void SetScaleDuration( DWORD dwDuration );

protected:
	void ImageScale( float fRate );

protected:
	virtual void OnProcess( float fTimePerSec );
	
protected:
	virtual void OnDrawOvered( int iXPos, int iYPos );
	virtual void OnDrawPushed( int iXPos, int iYPos );

public:
	virtual bool IsNeedProcess() const;
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	ioScaleButton();
	virtual ~ioScaleButton();
};

//HARDCODE: 라디오 버튼의 경우 다른 라디오버튼의 상태를 변경시키는 것은 부모윈도우에서 담당해주어야 한다.
//			ioWnd::CheckRadioButton 사용 ( API와 같다..결국은 M$도 같은 생각인듯..)
class __EX ioRadioButton : public ioButton
{
protected:
	int m_iPosX;
	int m_iPosY;

public:
	void SetClick( bool bClick ) { m_bClicked = bClick; }
	virtual void ReSetPosition(DWORD dwID){}
public:
	virtual void iwm_lbuttonup( const ioMouse& mouse );
	virtual void iwm_mouseleave( const ioMouse& mouse );

protected:
	virtual void OnDrawPushed( int iXPos, int iYPos );

public:
	virtual DWORD GetWndType() const;

	//************************************************************************************************************************
	// Tool을 위한 인터페이스들
	virtual ioWnd* _Clone() { return new ioRadioButton(); }
	DECL_NAME_INTERFACE_FOR_TOOL( RadioButton, ioRadioButton )
	//************************************************************************************************************************

public:
	ioRadioButton();
	virtual ~ioRadioButton();
};

//HARDCODE: 체크 버튼의 체크 이미지는 노말상태의 이미지로 세팅해야한다.
class __EX ioCheckButton : public ioButton
{
protected:
	ioUIRenderImage *m_pCheckImg;
	bool m_bChecked;

protected:
	DWORD m_dwUnCheckTextColor;
	DWORD m_dwUnCheckBkColor;
	bool  m_bUnCheckColorSetted;

public:
	void SetCheck( bool bCheck ) { m_bChecked = bCheck; }
	bool IsChecked() const { return m_bChecked; }

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void SetWindowAlpha( int iAlpha );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnDrawBack( int iXPos, int iYPos );

	virtual void OnRender();
	
public:
	virtual void iwm_lbuttonup( const ioMouse& mouse );

public:
	virtual DWORD GetWndType() const;

	//************************************************************************************************************************
	// Tool을 위한 인터페이스들
	virtual void SetSaveData( ioXMLElement& xml );
	DECL_NAME_INTERFACE_FOR_TOOL( CheckBox, ioCheckButton )
	virtual ioWnd* _Clone() { return new ioCheckButton(); }
	virtual void Copy( ioWnd* wnd );
	void SetUnCheckTextColor( DWORD color );
	void SetUnCheckBackColor( DWORD color );
	DWORD GetUnCheckTextColor() const { return m_dwUnCheckTextColor; }
	DWORD GetUnCheckBackColor() const { return m_dwUnCheckBkColor; }
	virtual int GetNumFixedRenderImage() const;
	virtual const char* GetFixedRenderImageType( int index );	
	virtual ioUIRenderElement** GetRenderImage( const char* type );
	//************************************************************************************************************************

public:	
	ioCheckButton();
	virtual ~ioCheckButton();
};

// 깜박이는 버튼

class __EX ioActiveScreenBtn : public ioButton
{
protected:
	bool m_bScreen;
	DWORD m_dwScreenTimer;

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnDrawNormal( int iXPos, int iYPos );

public:
	//************************************************************************************************************************
	// Tool을 위한 인터페이스들
	DECL_NAME_INTERFACE_FOR_TOOL( ActiveScreenBtn, ioActiveScreenBtn )
	virtual ioWnd* _Clone() { return new ioActiveScreenBtn(); }
	virtual void Copy( ioWnd* wnd );
	//************************************************************************************************************************

public:
	ioActiveScreenBtn();
	virtual ~ioActiveScreenBtn();
};

// 토글 버튼

class __EX ioToggleBtn : public ioButton
{
protected:
	bool m_bToggle;

protected:
	virtual void OnRender();

public:
	ioToggleBtn();
	virtual ~ioToggleBtn();

	virtual void iwm_lbuttondown( const ioMouse& mouse );

	bool isToggled() const { return m_bToggle; }
};

#endif