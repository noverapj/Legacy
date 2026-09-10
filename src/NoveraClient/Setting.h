#pragma once

#include "ioClientMaterialScriptModifier.h"

class ioRenderSystem;

struct SettingOption
{
	// 일반 유저 옵션
	bool m_bFullScreen;
	bool m_bShadow;
	bool m_bGlow;
	bool m_bShowMapEffect;
	bool m_bTrailGlow;
	bool m_bSoftSkinning;
	bool m_bUse16bitFrameBuffer;
	bool m_bUseMapEdge;
	bool m_bUseObjectEdge;
	bool m_bSpecAutoCheck;
	bool m_bSwapCopy;

	AntiAliasingQuality m_AntiAliasingQuality;
	TextureQuality m_TextureQuality;
	LightQuality   m_LightQuality;
	EffectQuality  m_EffectQuality;
	MapObjectQuality m_MapObjectQuality;

	FilterType m_FilterType;
	OverayType m_OverayType;

	int  m_iWidth;
	int  m_iHeight;

	int  m_iBGMVolume;
	int  m_iEffectSoundVolume;
	int  m_iVoiceVolume;
	int  m_iCustomSoundVolume;

	bool m_bUseBGM;
	bool m_bUseEffectSound;
	bool m_bUseOtherVoiceChat;
	bool m_bUseMIC;
	bool m_bUseVoiceChat;
	bool m_bUseCustomSound;

	bool m_bAutoRejctParty;
	bool m_bAutoRejctCamp;
	bool m_bChatBubbleAlarm;
	bool m_bGuildChatAlarm;
	bool m_bWholeChatOff;

	ioHashString m_szScreenShotSavePath;
	bool m_bScreenShotFormatJPG;
	bool m_bBlockUsersChat;
	bool m_bBlockUsersCustomSound;

	bool m_bSlangFilterChange;
	bool m_bSlangFilterOther;
	bool m_bHideSkillBuffEmoticon;
	bool m_bHideMiniSoldierSelectSlot;
	bool m_bHideCharChangeGauge;
	bool m_bUseJoyPad;
	bool m_bCostumeShowChange;
	bool m_bPlayKillSound;
	bool m_bPlayComboSound;
	bool m_bPlayExtendSound;
	
	// 스샷 모드 옵션
	bool m_bNoUIModeTeamCircleShow;
	bool m_bNoUIModeIDShow;
	bool m_bNoUIModeHPShow;
	bool m_bNoUIModeChatBubbleShow;
	bool m_bMovieModeUIHide;

	// 관리자 모드 옵션 ( 웹페이지에서 직접 값을 가져오므로 ini 저장하지 않음 )
	bool m_bBoundBox;
	bool m_bShowSphere;
	bool m_bExtraInfo;
	bool m_bShowMemoryUsage;	
	bool m_bShowWndBoundary;
	int  m_iModeSub;
	int  m_iModeMap;

	// 동영상 촬영
	int  m_iMovieCodec;
	int  m_iMovieSizeType;
	int  m_iMovieFrameType;
	int  m_iMovieQualityType;
	int  m_iMovieSoundType;
	
	// 길드 폴더 삭제
	bool m_bGuildFolderDel;
	
	bool m_bChatInfoOff;

	// Chat Option
	int m_iChatBackType;
	bool m_bNormalChat;
	bool m_bWholeChat;
	bool m_bSystemChat;

	// 슬롯 아이템 위치
	DWORD m_dwSlot;

	DWORD m_dwLSCCheckTime;

	bool IsOptionChanged( const SettingOption &rkSetting );
};

class Setting
{
public:
	enum Spec
	{
		HIGH_SPEC,
		MIDDLE_SPEC,
		LOW_SPEC,
		MINIMUM_SPEC,
		CUSTOM_SPEC,
		VERY_HIGH_SPEC,	// 기존에 저장된 숫자를 유지하기 위해 뒤에다 붙임
	};

protected:
	static char m_szCPUName[MAX_PATH];
	static int  m_iCPUClockSpeed;
	static int  m_iCPUCount;
	static int  m_iSystemMemory;

	static char m_szGraphicName[512];
	static int  m_iVertexShader;
	static int  m_iPixelShader;

	static ioHashString m_szHDDSerialNumber;

	typedef std::vector< D3DMULTISAMPLE_TYPE > EnableSampleList;
	static EnableSampleList m_EnableSampleList;

	typedef std::vector< SIZE > SizeList;
	static SizeList m_vResolutionList;

	static char  m_szDirectXVersion[MAX_PATH];
	static DWORD m_dwDirectXVersion;

	static char m_szOSVersion[MAX_PATH];
	static bool m_bWin98Series;

	static char  m_szFullSystemInfo[1024];

	static SettingOption m_Option;
	static Spec m_DefaultSpec;

	static char m_szSettingString[512];

	//	관리자 옵션
	static int  m_iReservedMode;
	
protected:
	static D3DVIEWPORT9 m_ViewPort;
	static D3DXMATRIX	m_matView;
	static D3DXMATRIX   m_matProj;
	static D3DXMATRIX   m_matIdentity;

public:
	static void Initialize( const ioHashString &rkPublicID );
	static void SaveSetting( const ioHashString &rkPublicID, bool bExceptResolution = false );

	static char* GetSettingString();

public:
	static void CheckOSVersion();
	static void SetDirectXVersion( const char *szVersion, DWORD dwVersion );
	static void SetMultiSampleList( const ioRenderSystem *pSystem );

	static char* GetFullSystemInfo() { return m_szFullSystemInfo; }
	static bool IsWin98Series() { return m_bWin98Series; }

public:
	static void SetResolutionList( const ioRenderSystem *pSystem );
	static int  GetResolutionCount();
	static bool GetResolution( int iIndex, int &iWidth, int &iHeight );

protected:
	static void CheckSystemInfo();
	static void LoadCustomSetting( ioINILoader &rkLoader );
	static void LoadSettingAlways( ioINILoader &rkLoader );

	static AntiAliasingQuality ModifyAvailableQuality( AntiAliasingQuality eQuality );

public:
	static void SetOptionBySpec( SettingOption &rkOption, Spec eSpec );
	static void SetSpec( Spec eSpec ) { m_DefaultSpec = eSpec; }
	static Spec GetSpec() { return m_DefaultSpec; }
	static Spec CheckHardwareSpec();

public:
	static void SetSettingOption( const SettingOption &rkSetting ) { m_Option = rkSetting; }
	static const SettingOption& GetSettingOption() { return m_Option; }

	static char* GetCPUName() { return m_szCPUName; }
	static int GetCPUClockSpeed() { return m_iCPUClockSpeed; }	// MHz 단위
	static int GetCPUCount() { return m_iCPUCount; }

	static int GetSystemMemory() { return m_iSystemMemory; }	// MBytes 단위

	static char* GetGraphicCardName() { return m_szGraphicName; }

	static ioHashString GetHDDSerialNumber() { return m_szHDDSerialNumber; }

	static int GetEnableMultiSampleCount() { return m_EnableSampleList.size(); }
	static int GetVertexShaderVersion() { return m_iVertexShader; }
	static int GetPixelShaderVersion() { return m_iPixelShader; }

	static char* GetOSVersion() { return m_szOSVersion; }
	static char* GetDXVersion() { return m_szDirectXVersion; }


public:
	static void SetDimensionConvertInfo( const D3DVIEWPORT9 &rkViewPort, 
										 const D3DXMATRIX &rkView,
										 const D3DXMATRIX &rkProj );

	static bool Check2DInScreen( const D3DXVECTOR3 &v3DPos, int &iXPos, int &iYPos );
	static D3DXVECTOR3 Convert3DTo2D( const D3DXVECTOR3 &v3DPos );

	static int Width();
	static int Height();

	static int HalfWidth() { return Width() / 2; }	
	static int HalfHeight() { return Height() / 2; }

public:	// 일반 유저 옵션
	static void SetCurResolution( int iWidth, int iHeight )
	{
		m_Option.m_iWidth  = iWidth;
		m_Option.m_iHeight = iHeight;
	}

	static AntiAliasingQuality GetAntiAliasingQuality() { return m_Option.m_AntiAliasingQuality; }
	static void SetAntiAliasingQuality( AntiAliasingQuality eQuality ) { m_Option.m_AntiAliasingQuality = eQuality; }
	static D3DMULTISAMPLE_TYPE GetMultiSampleType( AntiAliasingQuality eQuality );

	static LightQuality GetLightQuality() { return m_Option.m_LightQuality; }
	static void SetLightQuality( LightQuality eQuality ) { m_Option.m_LightQuality = eQuality; }

	static EffectQuality GetEffectQuality() { return m_Option.m_EffectQuality; }
	static void SetEffectQuality( EffectQuality eQuality ) { m_Option.m_EffectQuality = eQuality; }

	static MapObjectQuality GetMapObjectQuality() { return m_Option.m_MapObjectQuality; }
	static void SetMapObjectQuality( MapObjectQuality eQuality ) { m_Option.m_MapObjectQuality = eQuality; }

	static FilterType GetFilterType() { return m_Option.m_FilterType; }
	static void SetFilterType( FilterType eType ) { m_Option.m_FilterType = eType; }

	static OverayType GetOverayType() { return m_Option.m_OverayType; }
	static void SetOverayType( OverayType eType ) { m_Option.m_OverayType = eType; }

	static bool UseShadow() { return m_Option.m_bShadow; }
	static void SetUseShadow( bool bUse ) { m_Option.m_bShadow = bUse; }

	static bool UseGlow() { return m_Option.m_bGlow; }
	static void SetUseGlow( bool bUse ) { m_Option.m_bGlow = bUse; }

	static bool TrailGlow() { return m_Option.m_bTrailGlow; }
	static void SetTrailGlow( bool bGlow ) { m_Option.m_bTrailGlow = bGlow; }

	static bool ShowMapEffect() { return m_Option.m_bShowMapEffect; }
	static void SetShowMapEffect( bool bShow ) { m_Option.m_bShowMapEffect = bShow; }

	static TextureQuality GetTextureQuality() { return m_Option.m_TextureQuality; }
	static void SetTextureQuality( TextureQuality eQuality ) { m_Option.m_TextureQuality = eQuality; }

	static bool SoftSkinning() { return m_Option.m_bSoftSkinning; }
	static void SetSoftSknning( bool bSoft ) { m_Option.m_bSoftSkinning = bSoft; }

	static bool Use16BitFrameBuffer() { return m_Option.m_bUse16bitFrameBuffer; }
	static void SetUse16BitFrameBuffer( bool bUse ) { m_Option.m_bUse16bitFrameBuffer = bUse; }

	static bool UseMapEdge() { return m_Option.m_bUseMapEdge; }
	static void SetUseMapEdge( bool bUse ) { m_Option.m_bUseMapEdge = bUse; }

	static bool UseObjectEdge() { return m_Option.m_bUseObjectEdge; }
	static void SetUseObjectEdge( bool bUse ) { m_Option.m_bUseObjectEdge = bUse; }

	static bool SpecAutoCheck() { return m_Option.m_bSpecAutoCheck; }
	static void SetSpecAutoCheck( bool bAuto ) { m_Option.m_bSpecAutoCheck = bAuto; }

	static bool SwapCopy() { return m_Option.m_bSwapCopy; }
	static void SetSwapCopy( bool bCopy ) { m_Option.m_bSwapCopy = bCopy; }

	static int  GetBGMVolume() { return m_Option.m_iBGMVolume; }
	static void SetBGMVolume( int iBGMVolume ) { m_Option.m_iBGMVolume = iBGMVolume ;}

	static int  GetEffectSoundVolume() { return m_Option.m_iEffectSoundVolume; }
	static void SetEffectSoundVolume( int iEffectSoundVolume ) { m_Option.m_iEffectSoundVolume = iEffectSoundVolume ;}

	static int  GetVoiceVolume() { return m_Option.m_iVoiceVolume; }
	static void SetVoiceVolume( int iVoiceVolume ) { m_Option.m_iVoiceVolume = iVoiceVolume ;}

	static int  GetCustomSoundVolume() { return m_Option.m_iCustomSoundVolume; }
	static void SetCustomSoundVolume( int iCustomSoundVolume ) { m_Option.m_iCustomSoundVolume = iCustomSoundVolume; }

	static bool UseBGM() { return m_Option.m_bUseBGM; }
	static void SetUseBGM( bool bUse ) { m_Option.m_bUseBGM = bUse; }
	
	static bool UseEffectSound() { return m_Option.m_bUseEffectSound; }
	static void SetUseEffectSound( bool bUse ) { m_Option.m_bUseEffectSound = bUse; }
	
	static bool UseOtherVoiceChat() { return m_Option.m_bUseOtherVoiceChat; }
	static void SetUseOtherVoiceChat( bool bUse ) { m_Option.m_bUseOtherVoiceChat = bUse; }

	static bool UseMIC() { return m_Option.m_bUseMIC; }
	static void SetUseMIC( bool bUse ) { m_Option.m_bUseMIC = bUse; }

	static bool UseVoiceChat() { return m_Option.m_bUseVoiceChat; }
	static void SetUseVoiceChat( bool bUse ) { m_Option.m_bUseVoiceChat = bUse; }

	static bool UseCustomSound() { return m_Option.m_bUseCustomSound; }
	static void SetUseCustomSound( bool bUse ) { m_Option.m_bUseCustomSound = bUse; }

	static bool AutoRejectParty() { return m_Option.m_bAutoRejctParty; }
	static void setAutoRejectParty( bool bReject ) { m_Option.m_bAutoRejctParty = bReject; }

	static bool AutoRejectCamp() { return m_Option.m_bAutoRejctCamp; }
	static void setAutoRejectCamp( bool bReject ) { m_Option.m_bAutoRejctCamp = bReject; }

	static bool ChatBubbleAlarm(){ return m_Option.m_bChatBubbleAlarm; }
	static void SetChatBubbleAlarm( bool bAlarm ) { m_Option.m_bChatBubbleAlarm = bAlarm; }

	static bool GuildChatAlarm(){ return m_Option.m_bGuildChatAlarm; }
	static void SetGuildChatAlarm( bool bAlarm ) { m_Option.m_bGuildChatAlarm = bAlarm; }

	static bool WholeChatOff() { return m_Option.m_bWholeChatOff; }
	static void SetWholeChatOff( bool bOff ) { m_Option.m_bWholeChatOff = bOff; }

	static ioHashString & GetScreenShotSavePath() { return m_Option.m_szScreenShotSavePath; }
	static void SetScreenShotSavePath( const char* szPath ) { m_Option.m_szScreenShotSavePath = szPath; }

	static bool ScreenShotFormatJPG() { return m_Option.m_bScreenShotFormatJPG; }
	static void SetScreenShotFormatJPG( bool bJpg ) { m_Option.m_bScreenShotFormatJPG = bJpg; }

	static bool UseBlockUsersChat() { return m_Option.m_bBlockUsersChat; }
	static void SetBlockUsersChat( bool bUse ) { m_Option.m_bBlockUsersChat = bUse; }

	static bool UseBlockUsersCustomSound() { return m_Option.m_bBlockUsersCustomSound; }
	static void SetBlockUsersCustomSound( bool bUse ) { m_Option.m_bBlockUsersCustomSound = bUse; }

	static bool IsSlangFilterChange() { return m_Option.m_bSlangFilterChange; }
	static void SetSlangFilterChange( bool bChange ) { m_Option.m_bSlangFilterChange = bChange; }

	static bool IsSlangFilterOther() { return m_Option.m_bSlangFilterOther; }
	static void SetSlangFilterOther( bool bOther ) { m_Option.m_bSlangFilterOther = bOther; }

	static bool IsHideSkillBuffEmoticon() { return m_Option.m_bHideSkillBuffEmoticon; }
	static void SetHideSkillBuffEmoticon( bool bShow ) { m_Option.m_bHideSkillBuffEmoticon = bShow; }

	static bool IsHideMiniSoldierSelectSlot() { return m_Option.m_bHideMiniSoldierSelectSlot; }
	static void SetHideMiniSoldierSelectSlot( bool bShow ) { m_Option.m_bHideMiniSoldierSelectSlot = bShow; }

	static bool IsHideCharChangeGauge() { return m_Option.m_bHideCharChangeGauge; }
	static void SetHideCharChangeGauge( bool bShow ) { m_Option.m_bHideCharChangeGauge = bShow; }

	static bool UseJoyPad() { return m_Option.m_bUseJoyPad; }
	static void SetUseJoyPad( bool bUse ) { m_Option.m_bUseJoyPad = bUse; }

	static bool NoUIModeTeamCircleShow(){ return m_Option.m_bNoUIModeTeamCircleShow; }
	static void SetNoUIModeTeamCircleShow( bool bShow ){ m_Option.m_bNoUIModeTeamCircleShow = bShow; }

	static bool NoUIModeIDShow(){ return m_Option.m_bNoUIModeIDShow; }
	static void SetNoUIModeIDShow( bool bShow ){ m_Option.m_bNoUIModeIDShow = bShow; }

	static bool NoUIModeHPShow(){ return m_Option.m_bNoUIModeHPShow; }
	static void SetNoUIModeHPShow( bool bShow ){ m_Option.m_bNoUIModeHPShow = bShow; }

	static bool NoUIModeChatBubbleShow(){ return m_Option.m_bNoUIModeChatBubbleShow; }
	static void SetNoUIModeChatBubbleShow( bool bShow ){ m_Option.m_bNoUIModeChatBubbleShow = bShow; }

	static bool MovieModeUIHide(){ return m_Option.m_bMovieModeUIHide; }
	static void SetMovieModeUIHide( bool bHide ){ m_Option.m_bMovieModeUIHide = bHide; }

	static bool ChatInfoOff() { return m_Option.m_bChatInfoOff; }
	static void SetChatInfoOff( bool bOff ) { m_Option.m_bChatInfoOff = bOff; }

	//
	static int ChatBackType() { return m_Option.m_iChatBackType; }
	static void SetChatBackType( int iType ) { m_Option.m_iChatBackType = iType; }

	static bool ChatNormalChat() { return m_Option.m_bNormalChat; }
	static void SetChatNormalChat( bool bUse ) { m_Option.m_bNormalChat = bUse; }

	static bool ChatWholeChat() { return m_Option.m_bWholeChat; }
	static void SetChatWholeChat( bool bUse ) { m_Option.m_bWholeChat = bUse; }

	static bool ChatSystemChat() { return m_Option.m_bSystemChat; }
	static void SetChatSystemChat( bool bUse ) { m_Option.m_bSystemChat = bUse; }

	static DWORD GetSlotItem() { return m_Option.m_dwSlot; }
	static void SetSlotItem(DWORD dwSlot) { m_Option.m_dwSlot = dwSlot;}

	static bool IsCostumeShowChange() { return m_Option.m_bCostumeShowChange; }
	static void SetCostumeShowChange( bool bChange ) { m_Option.m_bCostumeShowChange = bChange; }

	static bool IsPlayKillSound() { return m_Option.m_bPlayKillSound; }
	static void SetPlayKillSound( bool bChange ) { m_Option.m_bPlayKillSound = bChange; }

	static bool IsPlayComboSound() { return m_Option.m_bPlayComboSound; }
	static void SetPlayComboSound( bool bChange ) { m_Option.m_bPlayComboSound = bChange; }

	static bool IsPlayExtendSound() { return m_Option.m_bPlayExtendSound; }
	static void SetPlayExtendSound( bool bChange ) { m_Option.m_bPlayExtendSound = bChange; }

public: // 관리자 모드 옵션
	static int  GetReservedMode();
	static void SetReservedMode( int iMode ) { m_iReservedMode = iMode; }

	static bool FullScreen() { return m_Option.m_bFullScreen; }
	static void SetFullScreen( bool bFullScreen ) { m_Option.m_bFullScreen = bFullScreen; }

	static bool ShowBoundBox() { return m_Option.m_bBoundBox; }
	static void SetShowBoundBox( bool bShow ) { m_Option.m_bBoundBox = bShow; }

	static bool ShowBoundSphere() { return m_Option.m_bShowSphere; }
	static void SetShowBoundSphere( bool bShow ) { m_Option.m_bShowSphere = bShow; }

	static bool ShowExtraInfo() { return m_Option.m_bExtraInfo; }
	static void SetShowExtraInfo( bool bShow ) { m_Option.m_bExtraInfo = bShow; }

	static bool ShowMemoryUsage() { return m_Option.m_bShowMemoryUsage; }
	static void SetShowMemoryUsage( bool bShow ) { m_Option.m_bShowMemoryUsage = bShow; }

	static bool ShowWndBoundary() { return m_Option.m_bShowWndBoundary; }
	static void SetSWndBoundary( bool bShow ) { m_Option.m_bShowWndBoundary = bShow; }

	static int  GetModeSub() { return m_Option.m_iModeSub; }
	static void SetModeSub( int iModeSub ) { m_Option.m_iModeSub = iModeSub; }

	static int  GetModeMap() { return m_Option.m_iModeMap; }
	static void SetModeMap( int iModeMap ) { m_Option.m_iModeMap = iModeMap; }

public: // 동영상 촬영 옵션
	static int MovieCodec(){ return m_Option.m_iMovieCodec; }
	static void SetMovieCodec( int iCodec ){ m_Option.m_iMovieCodec = iCodec; }

	static int  MovieSizeType() { return m_Option.m_iMovieSizeType; }
	static void SetMovieSizeType( int iSizeType ){ m_Option.m_iMovieSizeType = iSizeType; }

	static int  MovieFrameType() { return m_Option.m_iMovieFrameType; }
	static void SetMovieFrameType( int iFrameType ){ m_Option.m_iMovieFrameType = iFrameType; }

	static int  MovieQualityType() { return m_Option.m_iMovieQualityType; }
	static void SetMovieQualityType( int iQualityType ){ m_Option.m_iMovieQualityType = iQualityType; }

	static int  MovieSoundType() { return m_Option.m_iMovieSoundType; }
	static void SetMovieSoundType( int iSoundType ){ m_Option.m_iMovieSoundType = iSoundType; }

public:
	static bool GuildFolderDel() { return m_Option.m_bGuildFolderDel; }
	static void SetGuildFolderDel( bool bGuildFolderDel ){ m_Option.m_bGuildFolderDel = bGuildFolderDel; }

	static DWORD GetLSCCheckTime() { return m_Option.m_dwLSCCheckTime; }
	static void  SetLSCCheckTime( DWORD dwTime ) { m_Option.m_dwLSCCheckTime = dwTime; }

public:
	Setting();
	~Setting();
};

