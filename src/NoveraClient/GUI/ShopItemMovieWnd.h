#pragma once

class ioUIRenderImage;

class ShopItemMovieWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT		= 1,
		
		ID_SUB_DESC1_WND = 11,
		ID_SUB_DESC2_WND = 12,
		ID_SUB_DESC3_WND = 13,
		ID_SUB_DESC4_WND = 14,
		ID_SUB_DESC_MAX = 15,

		ID_FLASH_WND = 101,

		DT_NONE		= 10000,
		DT_FIRST	= 10001,
		DT_ALL		= 10002,
	};

protected:
	ioUIRenderImage *m_pDescAllBg;
	ioUIRenderImage *m_pDescFirstBg;

	int m_nType;
	
public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	virtual void OnRender();

public:
	void SetLoadFlash( const ioHashString& szFileName );
	void SetResetFlash();

public:
	void SetSelectDesc( int nArray, DWORD dwItemCode );
	void SetSelectType( int nType );
	void SetDownFail();

protected:
	void SetDescShow();

protected:
	void RenderDescBG( int nXpos, int nYpos );
	
public:
	ShopItemMovieWnd();
	virtual ~ShopItemMovieWnd();
};

class ShopItemDescWnd : public ioWnd
{
protected:
	enum
	{
		IC_WeaponCode = 10000,
		IC_NoWeaponCode = 100000,
		IC_YES = 1,
		IC_Weapon = 0,
		IC_Armor = 1,
		IC_Head = 2,
		IC_Cloak = 3,
	};

protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pPart;
	ioUIRenderImage *m_pInput;

	ioHashString m_szWeaponPart;
	ioHashString m_szArmorPart;
	ioHashString m_szHeadPart;
	ioHashString m_szCloakPart;

	ioHashString m_szPassiveInput;
	ioHashString m_szWeaponInput;
	ioHashString m_szArmorInput;
	ioHashString m_szHeadInput;
	ioHashString m_szCloakInput;

protected:
	ioHashString m_szSkillName;
	ioHashString m_szSkillDesc;

public:
	virtual void iwm_show();
	virtual void iwm_hide();

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnRender();
	
public:
	void SetDesc( DWORD dwItemCode );

protected:
	void DescRender( int nXpos, int nYpos );

public:
	ShopItemDescWnd();
	virtual ~ShopItemDescWnd();
};