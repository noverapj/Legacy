#pragma once

#include "../CharacterInfo.h"
#include "../ioCreateChar.h"

#include "../io3DEngine/ioMovingWnd.h"

class ioUI3DRender;

//////////////////////////////////////////////////////////////////////////
class MSCrossWnd : public ioWnd
{
public:
	enum
	{
		ID_L_ROTATE     = 1,
		ID_R_ROTATE     = 2,
		ID_PLUS         = 3,
		ID_MINUS        = 4,
		ID_CHANGE       = 5,
	};

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_show();
		
public:
	virtual void iwm_lbuttondown( const ioMouse& mouse );
	virtual void iwm_lbuttonup( const ioMouse& mouse );
	virtual void iwm_mouseover( const ioMouse& mouse );
	virtual void iwm_mouseleave( const ioMouse& mouse );

public:
	MSCrossWnd();
	virtual ~MSCrossWnd();
};
//////////////////////////////////////////////////////////////////////////
class CrossWnd : public MSCrossWnd
{
public:
	virtual void iwm_show();

public:
	CrossWnd(){}
	virtual ~CrossWnd(){}
};
//////////////////////////////////////////////////////////////////////////
class CharOverWnd : public ioWnd
{
public:
	enum
	{
		ID_CROSS_WND	= 1,
	};

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:	
	virtual void iwm_mouseover( const ioMouse& mouse );
	virtual void iwm_mouseleave( const ioMouse& mouse );

public:
	CharOverWnd(){}
	virtual ~CharOverWnd(){}
};
//////////////////////////////////////////////////////////////////////////
class MSExpWnd : public ioWnd
{
	ioUIRenderImage *m_pGaugeBack;
	ioUIRenderImage *m_pGaugeBar;
	ioUIRenderImage *m_pGaugeLeft;
	ioUIRenderImage *m_pGaugeCenter;
	ioUIRenderImage *m_pGaugeRight;
	ioUIRenderImage *m_pGaugeExp;
	ioUIRenderImage *m_pGaugePer;
	ioUIRenderImage *m_pGaugeNumber;
	int              m_iCharIndex;

	ioHashString     m_szClassName;
	bool             m_bAllShow;
	void RenderGauge( int xPos, int yPos, int iExpPer );

public:
	void SetMyCharacter( int iIndex ){ m_iCharIndex = iIndex; }
	void ShowExpBar( bool bShow ){ m_bAllShow = bShow; }

public:
	virtual void iwm_show();
	
protected:
	virtual void OnRender();
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

public:
	MSExpWnd();
	virtual ~MSExpWnd();
};

//////////////////////////////////////////////////////////////////////////
class ioSetItemInfo;
class ioItem;

class MSCharacterWnd : public ioWnd
{
protected:
	int           m_iCharIndex;
	float         m_fRotateYaw;

	CHARACTER	  m_CurCharInfo;
	int			  m_ItemCodeArray[ioCreateChar::MAX_CCS];

protected:		// 초기 위치 지정
	float		  m_fCharZPos;
	float         m_fBackRot;

protected:
	void ProcessRotate( float fTimePerSec );
	void RenderChar( int iXPos, int iYPos );

public:
	void LoadMyCharacter( int iIndex );

public:
	void RotateY( float fYaw );
	void SetRotate( float fBackRot );

public:
	void EquipSetItem( const ioSetItemInfo *pSetInfo );
	void EquipItem( const ioItem *pItem );	
	void RestoreCharItems();

public:
	inline int GetCharIndex() const { return m_iCharIndex; }
	inline float GetRotatePos() const { return m_fBackRot; }

public:
	virtual void iwm_create();

protected:
	virtual void OnProcess( float fTimePerSec );

public:
	MSCharacterWnd();
	virtual ~MSCharacterWnd();
};

//////////////////////////////////////////////////////////////////////////
class MySoldierWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_CHARACTER_WND   = 1,
		ID_EXP_WND         = 2,
		ID_CROSS_WND       = 3,
	};

protected:
	void        ShowExpBar( bool bShow );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_show();
	virtual void iwm_hide();

public:	
	virtual void iwm_mouseover( const ioMouse& mouse );
	virtual void iwm_mouseleave( const ioMouse& mouse );

protected:
	virtual void OnRender();

public:
	void SetSoldier( int iIndex );

public:
	MySoldierWnd();
	virtual ~MySoldierWnd();
};
