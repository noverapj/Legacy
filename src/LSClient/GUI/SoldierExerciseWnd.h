#pragma once


#include "FSMStateMgr.h"
#include "../ioSetItemInfo.h"

#define ARROW_SCALE 0.67f
#define NEW_SCALE 0.75f

class SoldierExerciseBtn : public ioButton
{
public:
	enum SoldierBoughtType
	{
		SBT_NONE             = 0,
		SBT_BOUGHT           = 1,
		SBT_EXERCISE_GENERAL = 2,
		SBT_EXERCISE_PCROOM  = 3,
		SBT_EXERCISE_EVENT   = 4,
	};
protected:
	enum AniType
	{
		AT_NONE            = 0,
		AT_START_DOWN      = 1,
		AT_START_UP        = 2,
		AT_START_STOP      = 3,
		AT_WHITE_OUT       = 4,
		AT_DOWN            = 5,
		AT_UP              = 6, 
		AT_MOUSE_OVER      = 7,
		AT_MOUSE_LEAVE     = 8,
	};
protected:
	bool              m_bForceOver;
	SoldierBoughtType m_eBoughtType;
	int               m_iClassType;              
	int               m_iNeedLevel;
	ioUIRenderImage  *m_pLock;

	bool             m_bArrow;
	int              m_iDownHeight;
	ioUIRenderImage *m_pArrow;
	FSMStateMgr      m_AniState;

	bool  m_bScreenAction;
	bool  m_bScreenSoldier;
	DWORD m_dwScreenTime;
	DWORD m_dwScreenStartTime;

	ioSetItemInfo::NeedLevelType m_eNeedLevelType;
	ioUIRenderImage *m_pSoldierIcon;
	ioUIRenderImage *m_pSoldierSubIcon;
	ioUIRenderImage *m_pLockNum;
	ioUIRenderImage *m_pLockBattleMark;
	ioUIRenderImage *m_pLockMedalMark;
	ioUIRenderImage *m_pLockAwardMark;
	ioUIRenderImage *m_pBoughtType;
	ioUIRenderImage *m_pExerciseGeneral;
	ioUIRenderImage *m_pExercisePcroom;
	ioUIRenderImage *m_pExerciseEvent;
	ioUIRenderImage *m_pNewMark;
	ioUIRenderImage *m_pNewEffect;
	ioUIRenderImage *m_pHitMark;
	ioUIRenderImage *m_pHitEffect;
	ioUIRenderImage *m_pSaleMark;
	ioUIRenderImage *m_pSaleEffect;
	ioUIRenderImage *m_pEventMark;
	ioUIRenderImage *m_pEventEffect;
	ioUIRenderImage *m_pIconBack;

	DWORD m_dwNewEffectWaitTime;
	DWORD m_dwNewEffectCurTime;
	int   m_iMarkType; // SHOP_MARK_TYPE_NONE = 0, SHOP_MARK_TYPE_SALE = 1, SHOP_MARK_TYPE_NEW = 2, SHOP_MARK_TYPE_HIT = 3, SHOP_MARK_TYPE_EVENT = 4,
	bool  m_bMarkEffect;
	float m_fIconScale;

public:
	virtual void iwm_create();
	virtual void iwm_mouseleave( const ioMouse& mouse );
	virtual void iwm_mouseover( const ioMouse& mouse );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

	void SetSoldierIcon( ioUIRenderImage *pSoldierIcon, float fIconScale, bool bScreenAction );
	void SetSoldierSubIcon( ioUIRenderImage *pSubIcon );
	void SetForceOver( bool bOver ) { m_bForceOver = bOver; }
	void SetBoughtType( SoldierBoughtType eBoughtType );
	int  GetClassType() const { return m_iClassType; }
	void SetClassType(int iClassType) { m_iClassType = iClassType; }
	void SetNeedTypeLv( ioSetItemInfo::NeedLevelType eType, int iNeedLv );
	void SetMarkType( int iMarkType ) { m_iMarkType = iMarkType; }

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnDrawNormal( int iXPos, int iYPos );
	virtual void RenderImageList( int iXPos, int iYPos, UIRenderType eRenderType );
	virtual void OnRender();	

	void RenderSoldierIcon( int iXPos, int iYPos );
	void RenderType( int iXPos, int iYPos );
	void RenderNewMark( int iXPos, int iYPos );
	void RenderNeedLv( int iXPos, int iYPos );

	void InitAniState();
	void UpdateAniState( );
	void RenderArrow( int iXPos, int iYPos);	

	void ProcessNewEffect();

public:
	SoldierExerciseBtn();
	virtual ~SoldierExerciseBtn();
};