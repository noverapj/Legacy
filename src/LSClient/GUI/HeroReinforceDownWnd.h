#pragma once

class ioUIRenderImage;
class ioComplexStringPrinter;

class ioPetView;

class HeroReinforceDownWnd : public ioWnd
{
public: 
	enum 
	{
		ID_ENABLE_REINFORECE_SOLDIER_SELECT_BTN  = 100,
		ID_REINFORCE_BTN						 = 101,
		ID_SOLDIER_WND							 = 102,
	};

protected:
	CHARACTER m_CharInfo;
	ioUIRenderImage *m_pDarkTextBack;
	ioUIRenderImage *m_pLightTextBack;
	ioUIRenderImage *m_pSoldierInfoBG;

	IoString m_SoldierReinforceInfo;
	IoString m_SoulStoneCountText;

	ioComplexStringPrinter m_SoldierSelectPrinter;
	ioHashString m_szSoulStoneText;
	ioHashString m_SoldierInfo;

public:
	void SetSoldierInfo( int iClassType );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRenderAfterChild();
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	void ClearList();

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

public:
	HeroReinforceDownWnd(void);
	virtual ~HeroReinforceDownWnd(void);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class HeroReinforceDoneWnd : public ioWnd
{
public: 
	enum 
	{
		ID_REINFORECE_AGAIN_BTN					 = 100,
		ID_SEE_HERO_BTN							 = 101,
		ID_SOLDIER_WND							 = 102,
	};

protected:
	ioUIRenderImage *m_pDarkTextBack;
	ioUIRenderImage *m_pLightTextBack;
	ioUIRenderImage *m_pSoldierInfoBG;

	CHARACTER m_CharInfo;
	
	IoString m_ReinforcedSoldierInfo;
	IoString m_SoulStoneCountText;

	ioComplexStringPrinter m_ReinforcedPrinter;
	ioHashString m_szEffectName;
	ioHashString m_szSuccessSound;
	ioHashString m_szSoulStoneText;
	ioHashString m_SoldierInfo;

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnRenderAfterChild();

protected:
	void ClearList();

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
public:
	void SetSoldierInfo( int iClassType );

public:
	HeroReinforceDoneWnd(void);
	virtual ~HeroReinforceDoneWnd(void);
};
