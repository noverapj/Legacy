#pragma once

#include "../io3DEngine/ioButton.h"

class ioUIRenderImage;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SelectSoldierListBtn : public ioButton
{
protected:
	int m_iCurMagicCode;

	ioHashString m_szDefaultText;
	ioHashString m_szSelectText;

public:
	void InitData();
	void SetListBtnInfo( ioHashString szTitle, int iMagicCode );

	inline int GetListMagicCode() const { return m_iCurMagicCode; }

protected:
	virtual void OnRender();

public:
	virtual void iwm_show();
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	SelectSoldierListBtn();
	virtual ~SelectSoldierListBtn();
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class MedalSlotBtn : public ioButton
{
protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pSubIcon;
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pIconDisable;

	float m_fIconScale;
	float m_fIconBackScale;
	float m_fIconDisableScale;

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

protected:
	virtual void OnDrawNormal( int iXPos, int iYPos );
	virtual void OnDrawOvered( int iXPos, int iYPos );
	virtual void OnDrawPushed( int iXPos, int iYPos );
	virtual void OnDrawDisabled( int iXPos, int iYPos );

protected:
	void RenderIcon(int iXPos, int iYPos, UIRenderType eType);
	void RenderBack(int iXPos, int iYPos );
	void RenderIconDisable( int iXPos, int iYPos );

public:
	void SetIcon( ioUIRenderImage* pIcon, float fIconScale, float fIconBackScale, float fIconDisableScale );
	void SetSubIcon( ioUIRenderImage *pIcon );

public:
	MedalSlotBtn(void);
	virtual ~MedalSlotBtn(void);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MEDAL_ICON_SCALE			0.85f
#define MEDAL_ICON_BACK_SCALE		1.0f
#define MEDAL_ICON_DISABLE_SCALE	0.85f

class ExpandMedalSlotWnd : public ioWnd
{
public:
	typedef struct tagClassTypeInfo
	{
		int m_iClassType;
		int m_iCurLevel;

		tagClassTypeInfo()
		{
			m_iClassType = 0;
			m_iCurLevel = 0;
		};
	}ClassTypeInfo;
	typedef std::vector< ClassTypeInfo > ClassTypeInfoList;

	class ClassTypeInfoSort : public std::binary_function< const ClassTypeInfo&, const ClassTypeInfo&, bool >
	{
	public:
		bool operator()( const ClassTypeInfo &lhs , const ClassTypeInfo &rhs ) const
		{
			if( lhs.m_iCurLevel < rhs.m_iCurLevel )
			{
				return true;
			}				
			else if( lhs.m_iCurLevel == rhs.m_iCurLevel )
			{
				if( lhs.m_iClassType < rhs.m_iClassType )
					return true;
			}
			return false;
		}
	};

public:
	enum 
	{
		ID_CHANGE_CLASS_BTN		= 1,
		ID_SELECT_BTN			= 2,

		ID_MEDAL_ITEM_1_BTN     = 11,
		ID_MEDAL_ITEM_2_BTN     = 12,
		ID_MEDAL_ITEM_3_BTN     = 13,
		ID_MEDAL_ITEM_4_BTN     = 14,
		ID_MEDAL_ITEM_5_BTN     = 15,
		ID_MEDAL_ITEM_6_BTN     = 16,

		ID_CLASS_LIST_WND		= 101,
		ID_SOLDIER_WND			= 102,
	};

protected:
	ioUIRenderImage *m_pEffect;

	int m_iClassType;

	CHARACTER m_CharInfo;
	DWORD	m_dwItemIndex;
	int		m_iUseLevel;
	BYTE	m_iSlotNumber;

	// RollingEffect
	struct EffectPos
	{
		int x, y;
		EffectPos()
		{
			Init();
		}
		void Init()
		{
			x = y = 0;
		}
		void SetPosition( int iNewX, int iNewY )
		{
			x = iNewX;
			y = iNewY;
		}
	};
	EffectPos m_EffectPos;

	ioHashStringVec m_vRollingEffect;

	int m_iCurEffectArray;
	DWORD m_dwRollingEffectTime;
	DWORD m_dwRollingEffectCheckTime;

	ioHashString m_szRollingSound;

protected:
	void UpdateData();
	bool CheckEnableSlot();
	void CheckSelectBtn();

protected:
	void GetClassList( vNewShopPullDownItem &rkList );
	void SetClassSelect( ioWnd *pOpenBtn, int iListArray );

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	void SetItemInfo( DWORD dwItemIndex );
	void SetClassType( int iClassType ) { m_iClassType = iClassType; }

protected:
	void SendExpandMedalSlotOpen();
	void RollingEffect( float fTimePerSec );

public:
	ExpandMedalSlotWnd(void);
	virtual ~ExpandMedalSlotWnd(void);
};

