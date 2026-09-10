#pragma once

class ioUIRenderImage;
class ioXMLElement;
class ioPlayStage;
class ExPCRoomAuthorityWnd : public ioWnd
{
public:
	enum
	{
		ID_SOLDIER_BTN   = 1,
	};

public:
	virtual bool iwm_spacebar();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	ExPCRoomAuthorityWnd();
	virtual ~ExPCRoomAuthorityWnd();
};
//////////////////////////////////////////////////////////////////////////
class ExPCRoomSoldierBonusBtn : public ioButton
{
public:
	enum
	{
		SELECT_ONE = 0,
		SELECT_TWO = 1,
	};

protected:
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pCharIcon;
	ioUIRenderImage *m_pCharSubIcon;
	ioUIRenderImage *m_pNumBack;

	ioUIRenderFrame *m_pCheckedFrm;	

	int m_nGradeType;

protected:
	int m_iClassType;
	int m_iSelectNum;

	bool m_bChecked;
	
public:
	void SetClassType( int iClassType, bool bCheck, int iSelectNum );

	int GetClassType(){ return m_iClassType; }
	bool IsChecked(){ return m_bChecked; }

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void OnRender();

public:
	ExPCRoomSoldierBonusBtn();
	virtual ~ExPCRoomSoldierBonusBtn();
};
//////////////////////////////////////////////////////////////////////////
class ExPCRoomSoldierBonusWnd : public ioWnd
{
public:
	enum
	{		
		ID_LEFT_PAGE	= 1,
		ID_RIGHT_PAGE	= 2,
		ID_OK			= 3,
		ID_EXIT         = 4,
		
		ID_BONUS_CHAR_BTN01  = 10,
		ID_BONUS_CHAR_BTN02  = 11,
		ID_BONUS_CHAR_BTN03  = 12,
		ID_BONUS_CHAR_BTN04  = 13,
		ID_BONUS_CHAR_BTN05  = 14,
		ID_BONUS_CHAR_BTN06  = 15,
		ID_BONUS_CHAR_BTN07  = 16,
		ID_BONUS_CHAR_BTN08  = 17,
		ID_BONUS_CHAR_BTN09  = 18,
		ID_BONUS_CHAR_BTN10  = 19,
		ID_BONUS_CHAR_BTN11  = 20,
		ID_BONUS_CHAR_BTN12  = 21,
		ID_BONUS_CHAR_BTN13  = 22,
		ID_BONUS_CHAR_BTN14  = 23,
		ID_BONUS_CHAR_BTN15  = 24,

		ID_SELECT_CHAR_VIEW1  = 100,
		ID_SELECT_CHAR_VIEW2  = 101,
	};

	enum
	{
		MAX_PAGE_LIST = 15,		
	};

protected:
	ioUIRenderFrame *m_pCharFrameOne;
	ioUIRenderFrame *m_pCharFrameTwo;
	ioUIRenderFrame *m_pCharFrameNum;

	ioUIRenderImage *m_pCharBottomFrame;	

	ioHashString m_szOneCharName;
	int m_iOneCharType;

	ioHashString m_szTwoCharName;
	int m_iTwoCharType;

	bool m_bFirstAutoSelect;
	bool m_bEraseBegin;

public:
	struct PCRoomSoldierSelect
	{
		int m_iClassType;
		int m_bCharDelete;

		PCRoomSoldierSelect()
		{
			m_iClassType = 0;
			m_bCharDelete = false;
		}
	};
	typedef std::vector<PCRoomSoldierSelect> PCRoomSoldierSelectVec;

protected:
	ioPlayStage *m_pPlayStage;

protected:
	int m_iCurPage;
	int m_iMaxPage;
	IntVec m_ClassTypeList;

protected:
	PCRoomSoldierSelectVec m_PCRoomSoldierSelectVec;

protected:
	void AddPCRoomSoldierList( ioSetItemInfo::PackageType eType );
	bool IsTimeExpireChar( int iClassType );

	void InsertSelectInfo( int iClassType, bool bCharDelete );
	void RemoveSelectInfo( int iClassType );

	void GetSelectInfo( int iClassType, OUT bool& bHas, OUT int& iSelectNum );
	void SelectNewChar( const CHARACTER& CharInfo, int iWND );
	void RefreshCharWnd();
	
protected:
	void UpdateSetPage( int iCurPage );

	int  FindClassTypeBtn( bool bFirstBtn );
	int  LastClassTypeBtn();

	bool IsCanExerciseChar( bool bSelectWait );

public:
	void SetFirstAutoSelect();

public:
	virtual void iwm_show();
	virtual bool iwm_spacebar();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

protected:
	virtual void OnRenderAfterChild();

	void On3DCharInfoRender( int iXPos, int iYPos );
	void OnMentRender( int iXPos, int iYPos ); 

protected:
	ioHashString GetPackageType( int iType );
	ioHashString GetGradeType( int iType );

public:
	void SetPlayStage( ioPlayStage *pPlayStage );

public:
	ExPCRoomSoldierBonusWnd();
	virtual ~ExPCRoomSoldierBonusWnd();
};
//////////////////////////////////////////////////////////////////////////
class SelectCharViewWnd : public ioWnd
{
public:
	enum
	{
		VIEW_WIDTH = 143,
		VIEW_HEIGHT= 267,
	};

	enum 
	{
		ID_CHAR_CONTROL_WND     = 1,
	};

protected:
	ioUI3DRender *m_pUI3DRender;
	ioCreateChar *m_aMyChar;

	CHARACTER m_CharInfo;	

	ioUIRenderImage *m_p3DBack;

	int m_iClassType;
	
	float m_fRotateYaw;
	float m_fZPosition;
	float m_fXMLRotateYaw;

	bool m_bReleaseHelm;
	bool m_bReleaseAllItem;
	bool m_bShopMotion;
	bool m_bUserInfoToolTip;

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

public:
	virtual void iwm_create();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	void CreateCharacter( const CHARACTER &rkInfo );

	void SetCharAnimation( bool bShopMotion );
	void SetCharAnimation( DWORD dwEtcItem, bool bMale );
	void SetCharLoopAnimation( const ioHashString &szAniName, float fFadeIn = 0.0f );
	void SetCharActionAnimation( const ioHashString &szAniName, float fFadeIn = FLOAT100, float fFadeOut = FLOAT100 );
	void SetCharMotionItemAnimation( const ioHashString &szAniName );
	
protected:
	void EquipItem( DWORD dwItemCode, DWORD dwItemMaleCustom, DWORD dwItemFemaleCustom );
	void ReleaseItem( int iSlot );	
	void RotateY( float fYaw );
	void InitRotate();

public:
	void SetChangeSkeleton( DWORD dwEtcItem, bool bMale );
	void SetChar( const CHARACTER &rkInfo );
	void SetEquip( bool bReleaseHelm, bool bReleaseAll, bool bShopMotion );

	void SetCharScale( float fScale );	
	void SetZPosition( float fZPos );

public:
	inline bool IsReleaseHelm(){ return m_bReleaseHelm; }
	inline bool IsReleaseAllItem(){ return m_bReleaseAllItem; }
	inline bool IsShopMotion(){ return m_bShopMotion; }

public:
	virtual void OnProcess( float fTimePerSec );	

protected:	
	virtual void OnRender();

public:
	SelectCharViewWnd();
	virtual ~SelectCharViewWnd();
};
//////////////////////////////////////////////////////////////////////////
class SelectCharControlBtn : public ioButton
{
protected:
	ioUIRenderImage *m_pBtn;

protected:
	void RenderInfo(int iXPos, int iYPos, UIRenderType eType);

public:
	void SetBtnAlphaRate( int iAlphaRate );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnDrawNormal( int iXPos, int iYPos );
	virtual void OnDrawOvered( int iXPos, int iYPos );
	virtual void OnDrawPushed( int iXPos, int iYPos );
	virtual void OnDrawDisabled( int iXPos, int iYPos );

public:
	SelectCharControlBtn();
	virtual ~SelectCharControlBtn();
};
//////////////////////////////////////////////////////////////////////////
class SelectCharControlWnd : public ioWnd
{
public:
	enum
	{
		ID_LEFT_ROTATE_BTN  = 1,
		ID_RIGHT_ROTATE_BTN = 2,
		ID_DEFAULT_POS_BTN  = 3,
		MAX_BTN = 3,
	};

	enum AniType
	{
		AT_NONE      = 0,
		AT_ALPHA_IN  = 1,
		AT_NORMAL    = 2,
		AT_ALPHA_OUT = 3,
		AT_HIDE      = 4,
	};

protected:
	FSMStateMgr m_AniState;

protected:
	void InitAniState();
	void SetChildAlpha( int iAlphaRate );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	void SetDefaultPosTitle( const char* szTitle ); 
	void HideWndAnimation();

protected:
	virtual void OnProcess( float fTimePerSec );

public:
	SelectCharControlWnd();
	virtual ~SelectCharControlWnd();
};