#pragma once

class ioEdit;

class ioUIRenderImage;
class SettingCustomSoundItemWnd : public ioWnd // grand child
{
public:
	enum
	{
		ID_KEYWORD_BTN  = 1,
		ID_KEYWORD_EDIT = 2,
		ID_FILENAME_BTN = 3,
		ID_PLAY_BTN     = 4,

		ID_PARAM_KEYWORD_BTNUP  = 10000,
		ID_PARAM_FILENAME_BTNUP = 10001,
		ID_PARAM_PLAY_BTNUP     = 10002,
	};
public:
	void SetInfo( const ioHashString &rszKeyword, const char *szFileName );
	void SetKeywordBtnToEdit();
	void SetKeywordEditToBtn();
	void SetFileNameInfo( const char *szFileName );
	void SetKeywordEdit( const char *szKeyword );
	
	void GetInfo( OUT ioHashString &rszKeyword, OUT ioHashString &rszFileName );
	const char *GetKeywordByEdit();
	void GetFileName( OUT ioHashString &rszFileName );

	void ShowKeywordBtn();
	void ClearInfo();
protected:
	virtual void iwm_create();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_show();
	virtual void iwm_hide();

public:
	SettingCustomSoundItemWnd();
	virtual ~SettingCustomSoundItemWnd();
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class GameOptionSettingCustomSoundTab : public ioWnd // child
{
public:
	enum
	{
		ID_SAVE    = 1,
		ID_CANCEL  = 2,
		ID_SHOP    = 3,

		ID_SETTING_CUSTOMSOUND_ITEM_WND1  = 101,
		ID_SETTING_CUSTOMSOUND_ITEM_WND2  = 102,
		ID_SETTING_CUSTOMSOUND_ITEM_WND3  = 103,
		ID_SETTING_CUSTOMSOUND_ITEM_WND4  = 104,
		ID_SETTING_CUSTOMSOUND_ITEM_WND5  = 105,
		ID_SETTING_CUSTOMSOUND_ITEM_WND6  = 106,
		ID_SETTING_CUSTOMSOUND_ITEM_WND7  = 107,
		ID_SETTING_CUSTOMSOUND_ITEM_WND8  = 108,
		ID_SETTING_CUSTOMSOUND_ITEM_WND9  = 109,
		ID_SETTING_CUSTOMSOUND_ITEM_WND10 = 110,
	};
protected:
	enum
	{
		MAX_SHOW_ITEM_WND = 10,
		ID_NONE_SELECT_ITEM_WND  = 1000000,
	};

protected:
	ioEdit *m_pPreEdit;
	DWORD   m_dwSelectEditItemID;
	ioUIRenderImage *m_pDescIcon;

	void SetKeywordEditToBtnByID();
	void SaveKeywordAndFileName();
	bool IsExistKeyword( const char *szKeyword , DWORD dwExceptID );

protected:
	virtual void OnRender();
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	void Init();
	void Save();

	bool CheckWaveFile( const char *szFileName );
	void PlayWaveFile( const char *szFileName );
public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	GameOptionSettingCustomSoundTab();
	virtual ~GameOptionSettingCustomSoundTab();
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SettingBtn : public ioButton // great grand child
{
protected:
	CKeyInfo::KeyTypeArray m_eKeyType;
	WORD m_wKey;

public:
	void SetKeyType(CKeyInfo::KeyTypeArray eKeyType);
	CKeyInfo::KeyTypeArray GetKeyType();
	void SetKeyValue( WORD wKey );
	void SetTitle( float fWidthCutSize );

public:
	SettingBtn();
	virtual ~SettingBtn();
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SettingKeyItemWnd : public ioWnd // grandchild
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

		IT_TITLE_ADDTION,
		IT_LEFT_UP,
		IT_RIGHT_UP,
		IT_LEFT_DOWN,
		IT_RIGHT_DOWN,

		IT_TITLE_SKILL,
		IT_CLOAK_SKILL,
		IT_HELM_SKILL,
		IT_ARMOR_SKILL,
		IT_WEAPON_SKILL,

		IT_TITLE_FUNCTION,
		IT_CONSUMPTION1,
		IT_CONSUMPTION2,
		IT_CONSUMPTION3,
		IT_CONSUMPTION4,
		IT_HELP,
		IT_SHOP,
		IT_GROWTH,
		IT_PRESENT,
		IT_QUEST,		
		IT_MYHOMEPAGE,
		IT_PET,
		IT_COOLTIME_RESET,
		IT_OPTION,
		IT_MESSENGER,
		IT_VOICE,
		IT_SCREENSHOT,
		IT_MOVIECAPTURE,
		IT_SPECIAL_MOTION,
		IT_ZOOMIN,
		IT_ZOOMOUT,

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
		IT_SOLDIER37,
		IT_SOLDIER38,
		IT_SOLDIER39,
		IT_SOLDIER40,
		IT_SOLDIER41,
		IT_SOLDIER42,
		IT_SOLDIER43,
		IT_SOLDIER44,
		IT_SOLDIER45,
		IT_SOLDIER46,
		IT_SOLDIER47,
		IT_SOLDIER48,
		IT_SOLDIER49,
		IT_SOLDIER50,

		IT_TITLE_BATTLE_MODE,
		IT_ENTRY,
		IT_SOLDIER_SELECT,
		IT_TAG_SWITCH,

		IT_TITLE_MACRO,
		IT_MACRO1,
		IT_MACRO2,
		IT_MACRO3,
		IT_MACRO4,
		IT_MACRO5,
		IT_MACRO6,
		IT_MACRO7,
		IT_MACRO8,
		IT_MACRO9,
		IT_MACRO10,
		IT_MACRO11,
		IT_MACRO12,

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
	ioUIRenderImage *m_pIconKey;

	ItemType m_eType;

protected:
	void SetTitle( const ioHashString &rsText );

public:
	void SetItem(ItemType eItemType,
		         CKeyInfo::KeyTypeArray eFirstKeyType,
				 CKeyInfo::KeyTypeArray eSecondKeyType,
				 WORD wFirstKey, 
				 WORD wSecondKey );

	void SetHelpText( IN ItemType eItemType, OUT ioHashString &rsText );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnRender();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	SettingKeyItemWnd();
	virtual ~SettingKeyItemWnd();
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioPlayStage;
class GameOptionSettingKeyTab : public ioWnd // child
{
public:
	enum
	{
		ID_SAVE   = 1,
		ID_CANCEL = 2,
		ID_APPLY  = 3,
		ID_DEFAULT= 4,

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
		ID_SETTING_KEY_ITEM_WND12 = 112,
		ID_SETTING_KEY_ITEM_WND13 = 113,
		ID_SETTING_KEY_ITEM_WND14 = 114,
		
		ID_KEY_INPUT_CONTROL_WND  = 501,
		ID_VERT_SCROLL            = 1001,
	};

protected:
	enum
	{
		MAX_SHOW_ITEM_WND = 14,
	};

protected:
	int      m_iCurPage;
	CKeyInfo m_KeyInfo;
	CKeyInfo::KeyTypeArray m_eSelectKeyType;
	typedef std::vector<SettingKeyItemWnd::ItemType> vSettingKeyItemType;
	vSettingKeyItemType m_vWholeItemType;

	static ioPlayStage *m_pPlayStage;

protected:
	void UpdateSetPage( int iNewPage );
	void ChangeKeyValue( WORD wKey );

	SettingKeyItemWnd::ItemType GetItemType( int iPage );
	SettingKeyItemWnd::ItemType GetItemTypeByKeyType( CKeyInfo::KeyTypeArray eKeyType );
	void GameOptionSettingKeyTab::GetKeyType( IN SettingKeyItemWnd::ItemType eItemType, 
									          OUT CKeyInfo::KeyTypeArray &reFirstKeyType, 
		                                      OUT CKeyInfo::KeyTypeArray &reSecondKeyType);

	void ShowKeyInputControlWnd( ioWnd * pWnd );
	bool ShowExistKeyControlWnd( WORD wKey );

public:
	void Init();
	bool Save();

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	virtual void iwm_vscroll(DWORD dwID, int curPos );
	virtual void iwm_wheel( int zDelta );


public:
	static void SetPlayStage( ioPlayStage *pPlayStage ) { m_pPlayStage = pPlayStage; }

public:
	GameOptionSettingKeyTab();
	virtual ~GameOptionSettingKeyTab();
};
//////////////////////////////////////////////////////////////////////////
class MacroEdit : public ioEdit // great grand child
{
protected:
	virtual void OnRender();
public:
	MacroEdit();
	virtual ~MacroEdit();
};
//////////////////////////////////////////////////////////////////////////
class SettingMacroItemBtn : public ioButton
{
protected:
	ioHashString m_szKeyText;

protected:
	virtual void OnRender();

public:
	void SetKeyText( const char *szKeyText ){ m_szKeyText = szKeyText; }

public:
	SettingMacroItemBtn();
	virtual ~SettingMacroItemBtn();
};

//////////////////////////////////////////////////////////////////////////
class SettingMacroItemWnd : public ioWnd // grand child
{
public:
	enum
	{
		ID_MACRO_TYPE_BTN  = 1,
		ID_MACRO_EDIT      = 2,

		ID_PARAM_MACRO_BTNUP     = 10000,
		ID_PARAM_MACRO_TYP_BTNUP = 10001,
	};

	ioHashString m_szDefaultTypeAll;
	ioHashString m_szDefaultTypeTeam;

protected:
	void ChangeTypeText( ioWnd *pWnd );

public:
	void SetItem( WORD wKey , const CKeyInfo::MecroTextInfo &rMacroTextInfo );
	void GetMecroTextInfo( OUT CKeyInfo::MecroTextInfo &rkInfo );
	void SetMacroEdit();

protected:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_show();
	virtual void iwm_hide();

public:
	SettingMacroItemWnd();
	virtual ~SettingMacroItemWnd();
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class GameOptionSettingGameTab : public GameOptionBasicWnd // child
{
public:
	enum
	{
		ID_SAVE   = 1,
		ID_CANCEL = 2,
		ID_APPLY  = 3,
		ID_DEFAULT= 4,

		ID_GENERAL_TITLE_WND     = 101,
		ID_INIT_UI_POS           = 102,
		ID_AUTO_REJECT_PARTY_BTN = 103,
		ID_BLOCK_USERS_CHAT_BTN  = 104,
		ID_AUTO_REJECT_CAMP_BTN  = 105,

		ID_CHAT_BUBBLE_ALARM     = 113,
		ID_GUILD_CHAT_ALARM      = 114,
		ID_SHOW_SKILL_BUFF_EMOTICON     = 115,
		ID_HIDE_MINI_SOLDIER_SELECT_SLOT= 116,
		ID_HIDE_CHAR_CHANGE_GAUGE		= 117,
		ID_BLOCK_USERS_CUSTOM_SOUND_BTN = 118,

		// 필터 방법
		ID_SLANG_FILTER_FUNC	= 201,
		ID_SLANG_FILTER_CHANGE	= 202,
		ID_SLANG_FILTER_STAR	= 203,

		// 필터 대상
		ID_SLANG_FILTER_TARGET	= 204,
		ID_SLANG_FILTER_OTHER	= 205,
		ID_SLANG_FILTER_ALL		= 206,

		// 코스튬 보이기 방법
		ID_COSTUME_SHOW_FUNC	= 221,

		// 매크로
		ID_SETTING_MACRO_ITEM_WND1  = 301,
		ID_SETTING_MACRO_ITEM_WND2  = 302,
		ID_SETTING_MACRO_ITEM_WND3  = 303,
		ID_SETTING_MACRO_ITEM_WND4  = 304,
		ID_SETTING_MACRO_ITEM_WND5  = 305,
		ID_SETTING_MACRO_ITEM_WND6  = 306,
		ID_SETTING_MACRO_ITEM_WND7  = 307,
		ID_SETTING_MACRO_ITEM_WND8  = 308,
		ID_SETTING_MACRO_ITEM_WND9  = 309,
		ID_SETTING_MACRO_ITEM_WND10 = 310,
		ID_SETTING_MACRO_ITEM_WND11 = 311,
		ID_SETTING_MACRO_ITEM_WND12 = 312,
	};

protected:
	enum 
	{
		MAX_SHOW_ITEM_WND = 12,
		ID_NONE_SELECT_ITEM_WND  = 1000000,
	};

protected:
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pIconGame;
	ioUIRenderImage *m_pIconMacro;

protected:
	CKeyInfo m_KeyInfo;
	ioEdit *m_pPreEdit;
	DWORD m_dwSelectItemIDForMacroEdit;

public:
	void Init();
	void Save();

protected:
	void SetSlangFilterFunc( DWORD dwID );
	void SetSlangFilterTarget( DWORD dwID );
	
protected:
	virtual void UpdateByCurrentOption();

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_show();
	virtual void iwm_hide();

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void UpdateSetPage();
	void SetMacroTextInfo( DWORD dwID );
	void OnProcessText();

public:
	GameOptionSettingGameTab();
	virtual ~GameOptionSettingGameTab();
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class GameOptionMovieScreenShotTab : public GameOptionBasicWnd // child
{
public:
	enum
	{
		ID_SAVE   = 1,
		ID_CANCEL = 2,
		ID_APPLY  = 3,
		ID_DEFAULT= 4,

		// 동영상 크기
		ID_MOVIE_SIZE_TITLE      = 102,
		ID_MOVIE_SIZE_BTN        = 103,
		// 동영상 프레임
		ID_MOVIE_FRAME_TITLE     = 104,
		ID_MOVIE_FRAME_BTN       = 105,
		// 동영상 화질
		ID_MOVIE_QUALITY_TITLE   = 106,
		ID_MOVIE_QUALITY_BTN     = 107,
		// 동영상 음질
		ID_MOVIE_SOUND_TITLE     = 108,
		ID_MOVIE_SOUND_BTN       = 109,
		// 동영상 코덱
		ID_MOVIE_CODEC_TITLE     = 110,
		ID_MOVIE_CODEC_BTN       = 111,
		// 스샷 파일종류
		ID_SCREENSHOT_FORMAT     = 114,
		ID_SCREENSHOT_BTN		 = 115,

		// 저장 위치
		ID_SCREENSHOT_PATH       = 119,
		ID_SCREENSHOT_PATH_BTN   = 120,
		ID_SCREENSHOT_PATH_EDIT  = 121,

		// 모드 전환
		ID_SCREENSHOT_MODE       = 122,
		ID_SCREENSHOT_MODE_BTN   = 123,
		ID_MOVIE_MODE_BTN		 = 124,

		// 스샷 모드 옵션
		ID_SCREENSHOT_MODE_OPTION = 125, 

		ID_NAME_SHOW_BTN          = 126, 
		ID_HP_SHOW_BTN            = 127,
		ID_TEAMCIRCLE_SHOW_BTN    = 128,
		ID_CHAT_BUBBLE_BTN        = 129,
		ID_MOVIE_MODE_UI_SHOW_BTN = 130,

		// 동영상 옵션 리스트 윈도우
		ID_MOVIE_RESOLUTION_LIST = 3700,
		ID_SCREEN_SHOT_RESOLUTION_LIST = 3710,
	};

protected:
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pIconMovie;
	ioUIRenderImage *m_pIconScreenShot;

protected:
	ioEdit *m_pPreEdit;
	static ioPlayStage *m_pPlayStage;

public:
	void Init();
	void Save();

protected:
	virtual void UpdateByCurrentOption();

public:
	virtual void iwm_create();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	bool CreateScreenShotFolder( const char *szPath);
	void GetDefaultScreenShotFolderName( OUT ioHashString &rFolderName );

public:
	static void SetPlayStage( ioPlayStage *pPlayStage ) { m_pPlayStage = pPlayStage; }

protected:
	void UpdateEditBtn( bool bShowBtn );
	void OpenResolutionList( ioWnd *pOpenWnd, int iOptionType, int iValue );
	void OpenScreenShotList( ioWnd *pOpenWnd );
	void SetTextEditBtn( const char *szSavePath );
	void GetTextEditBtn( OUT ioHashString &rszText );
	void SetOptionSavePath(); 

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnRender();

public:
	GameOptionMovieScreenShotTab();
	virtual ~GameOptionMovieScreenShotTab();
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class GameOptionSettingTab : public ioWnd // parent
{
public:
	enum
	{
		// btn
		ID_GAME_SETTING_TAB_BTN			= 1,
		ID_KEY_SETTING_TAB_BTN			= 2,
		ID_MACRO_SETTING_TAB_BTN		= 3,
		ID_CUSTOMSOUND_SETTING_TAB_BTN	= 4,
		ID_MOVIE_AND_SCREENSHOT_TAB_BTN	= 5,
		ID_JOY_KEY_SETTING_TAB_BTN		= 6,

		// wnd
		ID_GAME_SETTING_TAB_WND			= 101,
		ID_KEY_SETTING_TAB_WND			= 102,
		ID_MACRO_SETTING_TAB_WND		= 103,
		ID_CUSTOMSOUND_SETTING_TAB_WND	= 104,
		ID_MOVIE_AND_SCREENSHOT_TAB_WND	= 105,
		ID_JOY_KEY_SETTING_TAB_WND		= 106,

		// child 
		ID_PARAM_CHILD_SAVE   = 1001,
		ID_PARAM_CHILD_CANCEL = 1002,
		ID_PARAM_MOVIE_AND_SCREEN_SAVE = 1003,
		ID_PARAM_JOY_KEY_SAVE = 1004,
	};

protected:
	DWORD m_dwCheckBtn;

protected:
	void ChangeWindow( DWORD dwBtn );

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	GameOptionSettingTab(void);
	virtual ~GameOptionSettingTab(void);
};
