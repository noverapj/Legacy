#pragma once


#include "../io3DEngine/ioButton.h"
#include "../CharacterInfo.h"

#include "NewShopBuyWnd.h"


class ioUIRenderImage;
class ManualMgr;

class InvenFourExtraCompoundBuyWnd : public ioWnd
{
public:
	enum
	{
		ID_BUY_BTN           = 1,
		ID_CHAR_SELECT_BTN   = 2,
		ID_WEAPON_SELECT_BTN = 3, // ID_WEAPON_SELECT_BTN ~ID_CLOAK_SELECT_BTN enum값 변경 불가
		ID_ARMOR_SELECT_BTN  = 4,
		ID_HELMET_SELECT_BTN = 5,
		ID_CLOAK_SELECT_BTN  = 6,

		ID_WEAPON_ICON_BTN   = 7,
		ID_ARMOR_ICON_BTN    = 8,
		ID_HELMET_ICON_BTN   = 9,
		ID_CLOAK_ICON_BTN    = 10,

		ID_SOLDIER_WND       = 1000,
		ID_PULLDOWN_WND      = 1001,
	};

	enum
	{
		ADD_SELECT_TO_ICON   = 4,
		MAX_BACK_EFFECT      = 2,
	};

public:
	class SoldierSort : public std::binary_function< const NewShopPullDownItem& , const NewShopPullDownItem&, bool >
	{
	public:
		bool operator() ( const NewShopPullDownItem &lhs, const NewShopPullDownItem &rhs ) const
		{
			if( lhs.m_dwTextColor < rhs.m_dwTextColor )
				return true;
			if( lhs.m_dwTextColor == rhs.m_dwTextColor )
			{
				if( lhs.m_iMagicCode < rhs.m_iMagicCode )
					return true;
			}
			return false;
		}
	};

protected:
	DWORD        m_dwEtcItemType;
	ManualMgr    *m_pManual;
	int          m_iCurBtnUpClassType;
	ioUIRenderImage *m_pBackEffect;

	DWORD m_dwBackAniTime;
	DWORD m_dwBackAniCheckTime[MAX_BACK_EFFECT];
	DWORD m_dwBackAniSizePerGap;
	DWORD m_dwBackAniAlphaGap;

	float m_fBackAniScale[MAX_BACK_EFFECT];
	BYTE  m_byBackAniAlpha[MAX_BACK_EFFECT];

	DWORD m_dwStartAniCheckTime;

	bool  m_bReleaseHelmet;
	bool  m_bReleaseArmor;
	bool  m_bReleaseCloak;
	bool  m_bReleaseWeapon;

protected:
	CHARACTER m_CharInfo;

protected:
	void SetPullDownMenu( ioWnd *pOpenBtn );
	void SetBtnTitle( int iClassType );
	void SetIconBtn( DWORD dwID, int iSlotIdx );
	void GetSoldierList( vNewShopPullDownItem &rkList );
	void GetExtraList( DWORD dwID, int iClassType, vNewShopPullDownItem &rkList );

protected:
	void PullDownBtnUP( ioWnd *pOpenBtn, int iListArray );
	void PrintManual( int a_iXPos, int a_iYPos , float fScale );
	bool IsHaveAllExtra( int iClassType );
	void SendItemCompound();

public:
	void SetEtcItemType( DWORD dwEtcItemType );

public:
	virtual void iwm_show();
	virtual void iwm_hide();

public:
	virtual void iwm_create();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	InvenFourExtraCompoundBuyWnd(void);
	virtual ~InvenFourExtraCompoundBuyWnd(void);
};

//////////////////////////////////////////////////////////////////////////
class InvenFourExtraCompoundBuyResultWnd : public ioWnd
{
public:
	enum
	{
		ID_ROLLING		= 1,
		ID_USE			= 2,
		ID_GO			= 3,
		ID_SOLDIER_WND  = 1000,
	};

	enum
	{
		ROLLING_BTN = 1,
		RESULT_BTN	= 2,
	};

protected:
	ioUIRenderImage *m_pEffect;

	ioHashStringVec m_vSuccessEfffect;
	ioHashStringVec m_vFailEfffect;

	DWORD m_dwRollingTotalTime;
	DWORD m_dwRollingEndTime;

	int m_iCurEffectArray;
	DWORD m_dwRollingEffectTime;
	DWORD m_dwRollingEffectCheckTime;

	DWORD m_dwBackEffectTime;
	DWORD m_dwBackEffectCheckTime;

	ioHashString m_szDestroySound;
	ioHashString m_szRollingSound;
	ioHashString m_szSuccessSound;
	ioHashString m_szFailSound;
	ioHashString m_szStartAni;
	ioHashString m_szSuccessAni;
	ioHashString m_szFailAni;

	bool m_bResult;
	bool m_bSuccess;

	ioHashString m_sTitleName;
	ioHashString m_sClassTime;

	DWORD m_dwEtcItemType;
	CHARACTER m_CharInfo;

public:
	void ShowResult( int iClassType, int iClassTime, DWORD dwEtcItemType, bool bSuccess );

protected:
	void ReBatchButton( int iType );

	void LoadIconList();
	void GetCurIconList();

	void RollingIcon( float fTimePerSec );
	void RollingEffect( float fTimePerSec );
	void UpdateCompoundInfo();

public:
	virtual void iwm_create();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_hide();

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	virtual void OnRenderAfterChild();

public:
	InvenFourExtraCompoundBuyResultWnd();
	virtual ~InvenFourExtraCompoundBuyResultWnd();
};

