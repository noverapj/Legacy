#pragma once

#include "../ioComplexStringPrinter.h"

class ioEdit;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SettingJoyBtn : public ioButton // great grand child
{
protected:
	CJoyKeyInfo::KeyTypeArray m_eKeyType;
	WORD m_wKey;

public:
	void SetKeyType(CJoyKeyInfo::KeyTypeArray eKeyType);
	CJoyKeyInfo::KeyTypeArray GetKeyType();
	void SetKeyValue( WORD wKey );

	void SetSelect();
	void UnSelect(float fWidthCutSize);
public:
	SettingJoyBtn();
	virtual ~SettingJoyBtn();
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SettingJoyKeyItemWnd : public ioWnd // grandchild
{
public:
	enum ItemType // 배열 Array로 이용되므로 0부터 순차적으로 증가할것
	{
		IT_NONE             = -1,

		IT_TITLE_BASIC		= 0,
		IT_UP,
		IT_DOWN,
		IT_LEFT,
		IT_RIGHT,
		IT_ATTACK,
		IT_DEFENSE,
		IT_JUMP,

		IT_TITLE_SKILL,
		IT_CLOAK_SKILL,
		IT_HELM_SKILL,
		IT_ARMOR_SKILL,
		IT_WEAPON_SKILL,

		IT_TITLE_SOLDIER,
		IT_SOLDIER1,
		IT_SOLDIER2,
		IT_SOLDIER3,
		IT_SOLDIER4,
		IT_SOLDIER5,
		IT_SOLDIER6,
		IT_SOLDIER7,
		IT_SOLDIER8,
		IT_SOLDIER9,
		IT_SOLDIER10,
		IT_SOLDIER11,
		IT_SOLDIER12,
		IT_SOLDIER13,
		IT_SOLDIER14,
		IT_SOLDIER15,
		IT_SOLDIER16,
		IT_SOLDIER17,
		IT_SOLDIER18,
		IT_SOLDIER19,
		IT_SOLDIER20,
		IT_SOLDIER21,
		IT_SOLDIER22,
		IT_SOLDIER23,
		IT_SOLDIER24,
		IT_SOLDIER25,
		IT_SOLDIER26,
		IT_SOLDIER27,
		IT_SOLDIER28,
		IT_SOLDIER29,
		IT_SOLDIER30,
		IT_SOLDIER31,
		IT_SOLDIER32,
		IT_SOLDIER33,
		IT_SOLDIER34,
		IT_SOLDIER35,
		IT_SOLDIER36,

		IT_MAX,
	};
public:
	enum
	{
		ID_SMALL_FIRST_BTN  = 1,
		ID_SMALL_SECOND_BTN = 2,
		ID_BIG_BTN          = 3,
	};

protected:
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pIconJoypad;

	ItemType m_eType;

protected:
	void SetHelpText();

public:
	void SetItem(ItemType eItemType,
		         CJoyKeyInfo::KeyTypeArray eFirstKeyType,
				 WORD wFirstKey );
	
	void UnSelect();
	void SetSelect(CJoyKeyInfo::KeyTypeArray eKeyType);

public:
	virtual void SetActive();
	virtual void SetInActive();

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnRender();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	SettingJoyKeyItemWnd();
	virtual ~SettingJoyKeyItemWnd();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioPlayStage;
class GameOptionSettingJoyKeyTab : public GameOptionBasicWnd // child
{
public:
	enum
	{
		ID_SAVE   = 1,
		ID_CANCEL = 2,
		ID_APPLY  = 3,
		ID_DEFAULT= 4,

		ID_USE_CHECK	= 11,
		ID_UNUSED_CHECK  = 12,

		ID_SETTING_KEY_ITEM_WND1  = 101,
		ID_SETTING_KEY_ITEM_WND2  = 102,
		ID_SETTING_KEY_ITEM_WND3  = 103,
		ID_SETTING_KEY_ITEM_WND4  = 104,
		ID_SETTING_KEY_ITEM_WND5  = 105,
		ID_SETTING_KEY_ITEM_WND6  = 106,
		ID_SETTING_KEY_ITEM_WND7  = 107,
		ID_SETTING_KEY_ITEM_WND8  = 108,
		ID_SETTING_KEY_ITEM_WND9  = 109,
		ID_SETTING_KEY_ITEM_WND10 = 110,
		ID_SETTING_KEY_ITEM_WND11 = 111,

		ID_VERT_SCROLL            = 1001,
	};

protected:
	enum
	{
		MAX_SHOW_ITEM_WND = 11,
	};

protected:
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pIconJoypad;

protected:
	int      m_iCurPage;
	CJoyKeyInfo m_KeyInfo;
	CJoyKeyInfo::KeyTypeArray m_eSelectKeyType;
	typedef std::vector<SettingJoyKeyItemWnd::ItemType> vSettingJoyKeyItemType;
	vSettingJoyKeyItemType m_vWholeItemType;

	static ioPlayStage *m_pPlayStage;

	ioComplexStringPrinter m_Warning[3];

protected:
	void UpdateSetPage( int iNewPage );
	void ChangeKeyValue( WORD wKey );
	void SetSelect(CJoyKeyInfo::KeyTypeArray eKeyType);

	SettingJoyKeyItemWnd::ItemType GetItemType( int iPage );
	void GameOptionSettingJoyKeyTab::GetKeyType( IN SettingJoyKeyItemWnd::ItemType eItemType, 
												 OUT CJoyKeyInfo::KeyTypeArray &reFirstKeyType );

	void CheckUseJoyPad();
	void SetUseJoyPad( DWORD dwID );

public:
	void Init();
	void Save();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void UpdateByCurrentOption();

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	virtual void iwm_vscroll(DWORD dwID, int curPos );
	virtual void iwm_wheel( int zDelta );

public:
	static void SetPlayStage( ioPlayStage *pPlayStage ) { m_pPlayStage = pPlayStage; }

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnRender();

public:
	GameOptionSettingJoyKeyTab();
	virtual ~GameOptionSettingJoyKeyTab();
};

