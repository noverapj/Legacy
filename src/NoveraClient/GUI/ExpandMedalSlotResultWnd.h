#pragma once

#include "../io3DEngine/ioButton.h"

class ioUIRenderImage;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class MedalSlotResultBtn : public ioButton
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
	MedalSlotResultBtn(void);
	virtual ~MedalSlotResultBtn(void);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MEDAL_ICON_SCALE			0.85f
#define MEDAL_ICON_BACK_SCALE		1.0f
#define MEDAL_ICON_DISABLE_SCALE	0.85f

class ExpandMedalSlotResultWnd : public ioWnd
{
public:
	enum 
	{
		ID_VIEW_SOLDIER			= 1,

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

	ioHashStringVec m_vEndEfffect;

	int m_iCurEffectArray;
	DWORD m_dwRollingEffectTime;
	DWORD m_dwRollingEffectCheckTime;

	ioHashString m_szResultSound;

protected:
	void UpdateData();

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();
	virtual void OnRenderAfterChild();
	virtual void OnProcess( float fTimePerSec );

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	void SetInfo( int iClassType, int iSlotNumber );

protected:
	void RollingEffect( float fTimePerSec );

public:
	ExpandMedalSlotResultWnd(void);
	virtual ~ExpandMedalSlotResultWnd(void);
};

