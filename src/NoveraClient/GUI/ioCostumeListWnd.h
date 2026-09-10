#pragma once

///////////////////////////////////////////////////////////////////
class ioCostumeListWnd : public ioWnd
{
public:
	enum
	{
		ID_PRE_BTN	= 1,
		ID_NEXT_BTN	= 2,
		ID_X		= 3,

		ID_ALL_BTN		= 10,
		ID_WEAPON_BTN	= 11,
		ID_ARMOR_BTN	= 12,
		ID_HELMET_BTN	= 13,
		ID_CLOAK_BTN	= 14,

		ID_ICON_BTN1	= 100,
		ID_ICON_BTN2	= 101,
		ID_ICON_BTN3	= 102,
		ID_ICON_BTN4	= 103,
		ID_ICON_BTN5	= 104,
		ID_ICON_BTN6	= 105,
		ID_ICON_BTN7	= 106,
		ID_ICON_BTN8	= 107,
		ID_ICON_BTN9	= 108,
		ID_ICON_BTN10	= 109,
		ID_ICON_BTN11	= 110,
		ID_ICON_BTN12	= 111,
		
		ICON_BTN_MAX	= 12,
	};

protected:
	int m_nMaxPage;
	int m_nCurPage;
	int m_nMachineCode;
	DWORD m_dwCurSubBtn;

	ioHashString m_szPageText;

	CostumeShopSubInfoList m_vCurList;

public:
	void ShowItemList( int nMachineCode );

protected:
	void UpdatePage( int nPage );
	void ChangeSubBtn( DWORD dwSubBtn );

	void UpdateIconBtn();

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	ioCostumeListWnd();
	virtual ~ioCostumeListWnd();
};
////////////////////////////////////////////////////////////////////////////////////////////////
