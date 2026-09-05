#pragma once


#include "../Setting.h"

class ioUIRenderImage;
class ioUIRenderFrame;
class GameOptionBasicWnd : public ioWnd
{
protected:
	Setting::Spec m_DefaultSpec;
	SettingOption m_CurrentOption;

protected:
	void SetButtonCheck( DWORD dwID, bool bCheck );

	bool ApplyChangedOption();
	void SaveCurrentSetting( bool bExceptResolution );

protected:
	virtual void UpdateByCurrentOption()=0;
	virtual bool CheckResolutionChange( const SettingOption &rkPreOption );

public:
	virtual void iwm_show();

public:
	GameOptionBasicWnd();
	virtual ~GameOptionBasicWnd();
};
// ------------------------------------------------------------------------------------------------------------------
class GameOptionVolumeTab : public GameOptionBasicWnd
{
public:
	enum
	{
		ID_HORZ_SCROLL_BGM			= 1,
		ID_HORZ_SCROLL_EFFECT		= 2,
		ID_HORZ_SCROLL_VOICE		= 3,
		ID_HORZ_SCROLL_MIC			= 4,

		ID_BGM_BUTTON				= 5,
		ID_EFFECT_BUTTON			= 6,
		ID_OTHER_VOICE_BUTTON		= 7, // disable, enable on UI. ( 처음 한번 및 새롭게 생성되는 UI에 대해서만)
		ID_MIC_BUTTON				= 8, // disable, enable on UI.

		ID_COMMIT					= 9,
		ID_CANCEL					= 10,
		ID_APPLY					= 11, 

		ID_VOICE_CHAT				= 12,   // voice chat on, off

		ID_HORZ_SCROLL_CUSTOM		= 13,
		ID_CUSTOM_BUTTON			= 14,

		ID_PLAY_KILL_SOUND_BUTTON	= 15, //Play or None Play Kill Sound
		ID_PLAY_COMBO_SOUND_BUTTON	= 16, //Play or None Play Combo Sound
		ID_PLAY_EXTEND_SOUND_BUTTON	= 17, //Play or None Play Extend Sound
	};

protected:
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pIconVolume;
	ioUIRenderImage *m_pIconMic;

protected:
	int m_iMicVolumePerMax;

protected:
	virtual void UpdateByCurrentOption();

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_vscroll(DWORD dwID, int curPos );

	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	void ChangeMic();
	void ChangeOtherVoice();

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnRender();

public:
	GameOptionVolumeTab();
	virtual ~GameOptionVolumeTab();
};

//-----------------------------------------------------------------------------------------------------------------------
class GameSpecRadioBtn : public ioButton    // 다른 구조를 가지는 라디오버튼
{
protected:
	ioUIRenderImage *m_pChecked;

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnRender();

public:
	GameSpecRadioBtn();
	virtual ~GameSpecRadioBtn();
};

class GameOptionSpecTab : public GameOptionBasicWnd
{
public:
	enum
	{
		ID_VERY_HIGH_SPEC	= 1,
		ID_HIGH_SPEC		= 2,
		ID_MIDDLE_SPEC		= 3,
		ID_LOW_SPEC			= 4,
		ID_MINIMUM_SPEC		= 5,
		ID_CUSTOM_SPEC		= 6,

		ID_RESOLUTION_TEXT  = 8,
		ID_RESOLUTION_BOX   = 9,

		ID_TRAIL_GLOW       = 10,

		ID_ANTI_ALIASING_TEXT = 7,
		ID_ANTI_ALIASING_QUALITY = 11,
		ID_ANTI_ALIASING_HIGH = 12,
		ID_ANTI_ALIASING_MIDDLE = 13,
		ID_ANTI_ALIASING_LOW = 14,

		ID_MAP_OBJECT_QUALITY = 15,
		ID_MAP_OBJECT_HIGH = 16,
		ID_MAP_OBJECT_MIDDLE = 17,
		ID_MAP_OBJECT_LOW = 18,

		ID_SCREEN_GLOW = 19,
		ID_MAP_EFFECT = 20,
		ID_SHADOW = 21,

		ID_SCREEN_SET_TEXT = 22,
		ID_SCREEN_BOX      = 23,

		ID_LIGHT_QUALITY = 25,
		ID_LIGHT_HIGH = 26,
		ID_LIGHT_MIDDLE = 27,
		ID_LIGHT_LOW = 28,

		ID_GRAPHIC_OPTION_TEXT = 29,

		ID_OBJECT_EDGE = 30,
		ID_MAP_EDGE = 31,

		ID_TEXTURE_QUALITY = 35,
		ID_TEXTURE_HIGH = 36,
		ID_TEXTURE_MIDDLE = 37,
		ID_TEXTURE_LOW = 38,

		ID_EFFECT_QUALITY = 40,
		ID_EFFECT_HIGH = 41,
		ID_EFFECT_MIDDLE = 42,
		ID_EFFECT_LOW = 43,

		ID_16BIT_FRAMEBUFFER = 45,
		ID_SOFT_SKINNING = 46,

		ID_COMMIT	= 50,
		ID_CANCEL	= 51,
		ID_APPLY	= 52,
		ID_BACKGROUND_CHANGE	= 53,

		ID_SPEC_AUTO_CHECK = 56,
		ID_MOVIE_MODE = 57,

		ID_FILTER_TEXT = 60,
		ID_FILTER_BOX  = 61,

		ID_OVERAY_TEXT = 65,
		ID_OVERAY_BOX  = 66,

		ID_RESOLUTION_LIST = 3700,
		ID_FILTER_LIST = 3710,
		ID_OVERAY_LIST = 3720,
		ID_SCREEN_LIST = 3730,
	};

protected:
	ioUIRenderImage *m_pDot;

protected:
	int  m_iRestoreWidth;
	int  m_iRestoreHeight;
	bool m_bRestoreFullScreen;

protected:
	ioPlayStage *m_pPlayStage;

protected:
	virtual void UpdateByCurrentOption();
	virtual bool CheckResolutionChange( const SettingOption &rkPreOption );

	void SetCustomSpec();
	void UnCheckAutoSpec();
	void RestoreResolution();

protected:
	void SetSpecOption( DWORD dwID );
	void SetOnOffOption( bool &bOption );
	void SetTextureOption( DWORD dwID );
	void SetAntiAliasingOption( DWORD dwID );
	void SetLightOption( DWORD dwID );
	void SetEffectOption( DWORD dwID );
	void SetMapObjectOption( DWORD dwID );
	
	void ToggleMovieMode();

	void SetAntiOptionEnable( bool bEnable );

	void OpenScreenList( ioWnd *pOpenBtn );
	void SetChangeScreen( int iIndex );
	void UpdateScreenBoxText();

	void OpenResolutionList( ioWnd *pOpenBtn );
	void SetChangeResolution( int iIndex );
	void ChangeResolutionText( int iWidth, int iHeight );

	void OpenFilterList( ioWnd *pOpenBtn );
	void SetChangeFilter( int iIndex );
	void UpdateFilterBoxText();

	void OpenOverayList( ioWnd *pOpenBtn );
	void SetChangeOveray( int iIndex );
	void UpdateOverayBoxText();
	
public:
	DWORD GetDefaultSpecButtonID();

public:
	void SetPlayStage( ioPlayStage *pPlayStage ) { m_pPlayStage = pPlayStage; }

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnRender();

public:
	GameOptionSpecTab();
	virtual ~GameOptionSpecTab();
};
//--------------------------------------------------------------------------------------------------------------------

class GameOptionWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT             = 1,
		ID_UI_INIT          = 2,

		ID_SPEC_TAB_BTN     = 3,
		ID_VOLUME_TAB_BTN   = 4,
		ID_GAME_TAB_BTN     = 5,
		ID_MOVIE_SCREENSHOT_TAB_BTN   = 6,
		ID_KEYBOARD_TAB_BTN = 7,
		ID_JOYPAD_TAB_BTN   = 8,

		ID_SPEC_TAB_WND     = 101,
		ID_VOLUME_TAB_WND   = 102,
		ID_GAME_TAB_WND     = 103,
		ID_MOVIE_SCREENSHOT_TAB_WND = 104,
		ID_KEYBOARD_TAB_WND = 105,
		ID_JOYPAD_TAB_WND   = 106,
	};

protected:
	ioPlayStage *m_pPlayStage;

protected:
	DWORD  m_dwCheckBtn;

protected:
	void ChangeWindow( DWORD dwBtn );

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

	void ShowSelectTab( DWORD dwTabBtnID, DWORD dwChildTabBtnID );

public:
	void SetPlayStage( ioPlayStage *pPlayStage );

public:
	GameOptionWnd();
	virtual ~GameOptionWnd();
};

//--------------------------------------------------------------------------------------------------------------------

class BackgroundChangeBtn : public ioButton
{
protected:
	ioUIRenderImage *m_pDefaultBack;
	ioUIRenderImage *m_pStripBlack;
	ioUIRenderFrame *m_pLineFrame;

protected:
	ioUIRenderImage *m_pCustomBack;

public:
	virtual void iwm_show();

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void OnRender();

public:
	BackgroundChangeBtn();
	virtual ~BackgroundChangeBtn();
};

class BackgroundChangeWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT             = 1,
		ID_LOBBY_CHANGE     = 2,
		ID_LOADING_CHANGE   = 3,
		ID_BATTLE_CHANGE    = 4,
		ID_LADDER_CHANGE    = 5,
		ID_MODE_FIND_CHANGE	= 6,
	};

protected:
	void OpenFileListWnd( ioWnd *pReturnBtn, int iRwtType );

public:
	virtual void iwm_show();
	virtual void iwm_hide();

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	BackgroundChangeWnd();
	virtual ~BackgroundChangeWnd();
};
