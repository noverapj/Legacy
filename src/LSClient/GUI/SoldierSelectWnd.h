#pragma once

#include "../io3DEngine/ioButton.h"
#include "../CharacterInfo.h"
#include "MySoldierWnd.h"
#include "SoldierExerciseWnd.h"
#include "NewSearchWnd.h" // 2019-06-26

class ioUIRenderImage;
class ioUIRenderFrame;
class ioDragItem;
class ioSoldierDragItem;
class ioPlayStage;

class SoldierChar : public MSCharacterWnd
{
protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	SoldierChar();
	virtual ~SoldierChar();
};
//////////////////////////////////////////////////////////////////////////
class SoldierChargeBtn : public ioButton
{
public:
	enum
	{
		LIMIT_MINUTE = 120,      //2시간
		LIMIT_HOUR   = 6000,     //100시간
	};

protected:
	ioUIRenderFrame *m_pBlueNorFrm;
	ioUIRenderFrame *m_pBlueDownFrm;
	ioUIRenderFrame *m_pPinkNorFrm;
	ioUIRenderFrame *m_pPinkDownFrm;
	ioUIRenderFrame *m_pCurrentNorFrm;
	ioUIRenderFrame *m_pCurrentDownFrm;
    
protected:
	int  m_iSoldierNumber;
	bool m_bPCRoomOpen;

public:
	void SetCharacterCount( int iNumber, bool bPCRoomOpen );

protected:
	virtual void OnDrawNormal( int iXPos, int iYPos );
	virtual void OnDrawOvered( int iXPos, int iYPos );
	virtual void OnDrawPushed( int iXPos, int iYPos );

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	SoldierChargeBtn();
	virtual ~SoldierChargeBtn();
};
//////////////////////////////////////////////////////////////////////////
class SoldierRentalBtn : public ioButton
{
protected:
	int  m_iSoldierNumber;

public:
	void SetCharacterCount( int iNumber );

protected:
	virtual void OnProcess( float fTimePerSec );

public:
	SoldierRentalBtn();
	virtual ~SoldierRentalBtn();
};
//////////////////////////////////////////////////////////////////////////
class SoldierInfo : public ioWnd
{
public:
	enum
	{
		MAX_SLOT_KEY = 50,
	};

protected:
	ioUIRenderImage *m_pBlueKeyBack;
	ioUIRenderImage *m_pPinkKeyBack;
	ioUIRenderImage *m_pGrayKeyBack;
	ioUIRenderImage *m_pSoldierShadow;

	int m_nGradeType;

protected:
	int  m_iSoldierNumber;
	int  m_iSoldierSlot;
	bool m_bPCRoomOpen;

protected:
	void RenderKeyState( int iXPos, int iYPos );
	virtual void OnRender();
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

public:
	void SetCharacterCount( int iNumber, int iSlot, bool bPCRoomOpen );
	int  GetCharacterCount();

public:
	SoldierInfo();
	virtual ~SoldierInfo();
};
///////////////////////////////////////////////////////////////////////////
class SoldierBtn : public ioButton
{	
public:
	enum
	{
		ID_CHARACTER		= 1,
		ID_INFO				= 2,
		ID_EXTEND			= 3,
		ID_SHOP				= 4,
		ID_CHARGE			= 5,
		ID_RENTAL_STAND_BY	= 6,
		ID_RENTAL           = 7,
		ID_RENTAL_SOLDIER   = 8,
	};
	
protected:
	ioSoldierDragItem *m_pDragItem;
	ioPlayStage *m_pPlayStage;

	// 2019-06-26
	ioUIRenderFrame*	m_pSelectPosFrm;
	bool				m_bSelectPos;

	int	m_iSoldierNumber;

	bool m_bBtnOver;
	bool m_bBtnDown;
	bool m_bPCRoomOpen;

public:
	static DWORD    sg_dwDragFocusID;
	static DWORD    sg_dwFocusID;

protected:
	void ShowInventorySoldierTab();

	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );	// 2019-06-26
	virtual void OnProcess( float fTimePerSec );

public:
	void SetCharacterCount( ioPlayStage *pPlayStage, int iNumber, int iSlot, bool bForceUpdateChar, bool bPCRoomOpen );
	int  GetCharacterCount();
	void SetSelectPos( bool bSelectPos ) { m_bSelectPos = bSelectPos; }	// 2019-06-26

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_mouseover( const ioMouse& mouse );
	virtual void iwm_mouseleave( const ioMouse& mouse );
	virtual void iwm_lbuttondown( const ioMouse& mouse );
	virtual void iwm_lbuttonup( const ioMouse& mouse );
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnDrawOvered( int iXPos, int iYPos );
	virtual void OnDrawPushed( int iXPos, int iYPos );
	virtual void OnDrawNormal( int iXPos, int iYPos );

protected:
	virtual void OnRender();	// 2019-06-26
	virtual void OnRenderAfterChild();

public:
	SoldierBtn();
	virtual ~SoldierBtn();
};
//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/* 용병 애드온없어졌음. 2010.04.20										*/
/************************************************************************/
class SoldierSelectWnd : public ioWnd
{
public:
	friend class NewSearchWnd;	// 2019-06-26

public:
	enum
	{
		ID_EXIT        = 1,
		ID_VERT_SCROLL = 2,
		
		// first line
		ID_SOLDIER_BTN1= 3,
		ID_SOLDIER_BTN2= 4,
		ID_SOLDIER_BTN3= 5,
		ID_SOLDIER_BTN4= 6,
		ID_SOLDIER_BTN5= 7,
		ID_SOLDIER_BTN6= 8,

		// second line
		ID_SOLDIER_BTN7 = 9,
		ID_SOLDIER_BTN8 = 10,
		ID_SOLDIER_BTN9 = 11,
		ID_SOLDIER_BTN10= 12,
		ID_SOLDIER_BTN11= 13,
		ID_SOLDIER_BTN12= 14,

		ID_MAXIMUM_BTN  = 15,
		ID_MINIMUM_BTN  = 16,

		ID_VERT_SCROLL_TALL     = 19,

		ID_ONE_SOLDIER_BTN      = 21,
		ID_ALL_SOLDIER_BTN      = 22,
		ID_DISPLAY_SOLDIER_BTN  = 23,

		ID_NEW_SEARCH_WND = 5004, // 2019-06-26
	};

	enum WndShapeType
	{
		WST_NORMAL_MIN = 1,
		WST_NORMAL_MAX = 2,
	};

	enum 
	{
		NORMAL_MIN_WIDTH  = 599,
		NORMAL_MIN_HEIGHT = 238,
		NORMAL_MAX_WIDTH  = 599,
		NORMAL_MAX_HEIGHT = 422,
		MYROOM_ADD_HEIGHT = 31,
		BOTTOM_SOLDIER_BTN_GAP = 34,
        
		MAX_MINIMUM_SOLDIER_BTN  = 6,
		MAX_MAXIMUM_SOLDIER_BTN  = 12,

		PAGE_PER_BTN_NUM         = 6,
	};

protected:
	int   m_iCurPage;
	DWORD m_dwRecoverStartTime;
	DWORD m_dwRemainSecond;

	bool  m_bSelectOk;
	bool  m_bNetworkMode;

	ioPlayStage *m_pPlayStage;

protected:
    int       m_iSelectCharArray;
	int       m_iAutoSelectCharArray;
	IntVec    m_vCharSlot;

	int       m_iMaxSodierBtn;
	DWORD     m_dwScrollWndID;
	WndShapeType m_eWndShapeType;

	bool      m_bChangeResolution;

	// 2019-06-26
	NewSearchWnd*				m_pNewSearchWnd;
	bool						m_bSearchInputEnter;

protected:
	ioHashString m_szFightClubShowSnd;
	ioHashString m_szFightClubTickSnd;

protected:
	enum SoldierWndStyle
	{
		FORCED_STYLE  = 0,
		SELECT_STYLE  = 1,
		SETTING_STYLE = 2,
	};
	SoldierWndStyle m_eSoldierWndStyle;

protected:
	void ShowCharExtend();
	void SetCharSlotVec();
	int  GetCharSlotIndexToVecArray( int iSelIdx );
	int  GetCharVecArrayToSlotIndex( int iSelArray, bool bPlus );

	bool IsWndSizeEx();
	bool IsCharStateInActive();
	void ChangeWnd( WndShapeType eWndType );

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_vscroll(DWORD dwID, int curPos );
	virtual void iwm_wheel( int zDelta );
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual bool iwm_spacebar();
	virtual IWDropEffect iwm_dragenter( ioDragItem *pItem, const ioMouse& mouse );
	virtual IWDropEffect iwm_dragover( ioDragItem *pItem, const ioMouse& mouse );
	virtual bool iwm_dropitem( ioDragItem *pItem, const ioMouse& mouse, ioWnd *pStartWnd );
	virtual void iwm_dropdone( ioDragItem *pItem, ioWnd *pDroppedWnd, bool bDropped );
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	void  OnKeyProcess();
	DWORD CheckDropEnable( const ioMouse& mouse, ioDragItem *pItem );
	void  SendCharChange(int iArray );
	void  SetKeySelect();

	bool CheckCurPage( int iIndex, int iCurPage );
	void SettingClassModeProcess( int iClassType );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void SetRemainSecond( DWORD dwSecond );
	void SetFunctionKeySelect( int iArray, bool bNetMode );
	void SetSelectWaitShow( int iArray, bool bNetMode );
	void SetSettingSoldierShow( int iArray, bool bNetMode );
	void ChangeSoldierSlot( int iLeftArray, int iRightArray );
	void AddMySoldier( DWORD dwIndex );
	void SettingSoldierBtn();
	void ReSetWindow();
	void ReSetScroll();

public:
	void SetPlayStage(ioPlayStage * pStage);
	void UpdateRecoverRevivalPenalty();

	// 2019-06-26
	void OnSelectEnter( ioHashString sName, DWORD dwID );
	void UpdateSearch( ioHashString strName, OUT vSearchInfoVec& rkVec );
	void SetSearchInputEnter( bool bSearchInputEnter )	{	m_bSearchInputEnter = bSearchInputEnter;	}
	void SetSearchKeyFocus()							{	m_pNewSearchWnd->SetKeyFocus();				}
	void SetSearchKillKeyFocus()						{	m_pNewSearchWnd->KillKeyFocus();			}
	void SearchInfoVecClear()							{	m_pNewSearchWnd->SearchInfoVecClear();		}
	void ClearSearchHide();	// 2019-07-01

public:
	inline void SelectOk( bool bOk ){ m_bSelectOk = bOk; }
	inline bool IsSelectOk() const { return m_bSelectOk; }
	inline bool IsForcedWait() const { return ( m_eSoldierWndStyle == FORCED_STYLE ); }
	inline bool IsSelectWait() const { return ( m_eSoldierWndStyle == SELECT_STYLE ); }
	inline bool IsSettingWait() const { return ( m_eSoldierWndStyle == SETTING_STYLE ); }
	inline void SetSelectCharArray( int iSelectCharArray ){ m_iSelectCharArray = iSelectCharArray; }
	inline int  GetSelectCharArray() const { return m_iSelectCharArray; }

	// 2019-06-26
	inline bool IsFindKeyWordEmpty()	const {	return m_pNewSearchWnd->IsFindKeyWordEmpty();	}
	inline bool IsSearchKeyFocus()		const {	return m_pNewSearchWnd->IsKeyFocus();			}
	inline bool IsSearchInputEnter()	const {	return m_bSearchInputEnter;						}
	inline bool IsSearchInfoVecEmpty()	const {	return m_pNewSearchWnd->IsSearchInfoVecEmpty(); }

public:
	SoldierSelectWnd();
	virtual ~SoldierSelectWnd();
};
