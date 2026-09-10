#pragma once
#include "../ioComplexStringPrinter.h"
class ioUIRenderImage;
class ioXMLElement;

enum ReinforceSubType
{
	HERO_REINFORCE = 1,
	WEAPON_REINFORCE = 2,
};

class ReinforceSelectWnd :	public ioWnd
{
public:
	enum
	{
		HERO_REINFORCE_WND = 1,
		WEAPON_REINFORCE_WND = 2,
		ID_EXIT_BTN          = 11,
	};
protected:
	int	m_iSoulStoneCount;
public:
	void SetInfo();
public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual bool iwm_esc();
	virtual void iwm_show();
	virtual void iwm_hide();

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
public:
	virtual void OnRender();
protected:
	bool	CheckEnableHeroPowerup( ioHashString& szError, int &iClassType, bool& bIschar, int& iUseMtrlCount, int& iCurMtrlCount );
	bool	CheckEnableWeaponPowerup( ioHashString& szError, int &iIndex, bool& bIsWeapon, int& iUseMtrlCount, int& iCurMtrlCount );
public:
	ReinforceSelectWnd(void);
	virtual ~ReinforceSelectWnd(void);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////


class ReinforceSelectSubWnd : public ioButton
{
public:
	//button
	enum
	{
		ID_SELECT	= 1,
	};

	//subWnd의 설명
	enum
	{
		SUB_DESC_LINE = 6,
	};
	//text.text의 str
	enum 
	{
		HERO_DESC_STR = 1,
		WEAPON_DESC_STR = 7,
	};

protected:
	ReinforceSubType m_SubType;
	ioComplexStringPrinter m_ReinforceDesc[SUB_DESC_LINE];
protected:
	void ClearList();
	void SubDescRender();
public:
	void SetInfo( ReinforceSubType eType );
	
public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	virtual void OnRender();
	virtual void OnRenderAfterChild();

public:
	ReinforceSelectSubWnd();
	virtual ~ReinforceSelectSubWnd();
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SoulStoneBuyWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT  = 1,
		ID_SHOP  = 2,
		ID_CANCEL= 3,
	};

	enum
	{
		SOULSTONE_DESC_LINE = 4,
	};
protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pSubIcon;

protected:
	ioComplexStringPrinter m_SoulstoneDesc[SOULSTONE_DESC_LINE];

public:
	void ShowBuyWnd( int iUseMtrlCount, int iCurMtrlCount );
public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	SoulStoneBuyWnd();
	virtual ~SoulStoneBuyWnd();
};