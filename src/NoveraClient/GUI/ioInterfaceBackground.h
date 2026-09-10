#pragma once

enum BackGroundType
{
	BGT_NONE   = 0,
	BGT_LOBBY  = 1,
	BGT_BATTLE = 2,
	BGT_LADDER = 3,
	BGT_RESULT = 4,
	BGT_MODE   = 5,
	BGT_RABKBATTLE = 6,
};

class ioInterfaceBackground : public Singleton< ioInterfaceBackground >
{	
protected:
	typedef std::map< BackGroundType, ioUIRenderImage * > BackGroundMap;
	BackGroundMap m_BackGroundMap;

protected:
	enum
	{
		CUSTOM_RENDER = 0,
		CUSTOM_FADEIN,
		MAX_CUSTOM,
	};

	struct CustomBack
	{
		bool m_bOnRender;
		ioHashString m_SetName;
		ioUIRenderImage *m_pCustomRes;		
		CustomBack()
		{
			m_bOnRender  = false;
			m_pCustomRes = NULL;
		}
	};
	CustomBack m_CustomLobbyBack[MAX_CUSTOM];
	CustomBack m_CustomLoadingBack;
	CustomBack m_CustomBattleBack;
	CustomBack m_CustomLadderBack;
	CustomBack m_CustomFindModeBack[MAX_CUSTOM];

protected:
	BackGroundType m_eBackType;
	BackGroundType m_ePrevBackType;

protected:
	ioHashString    m_szLoadingRes;
	ioUIRenderImage *m_pLoadingRes;

protected:
	bool  m_bFadeInStart;
	DWORD m_dwCurrentTime;
	DWORD m_dwFadeInTime;
	int   m_iAlphaRate;
	bool  m_bCustomFadeIn;

protected:
	bool  m_bFirstLoading;
	int   m_iModeType;
	int   m_iMyRoomType;

protected:
	void ClearBackResource();
	bool Render( ioUIRenderImage *pRes, int iAlphaRate );

public:
	void LoadBackgroundINI();

public:
	ioUIRenderImage *GetBackgroundRes( BackGroundType eBackType );

public:  // 로비 커스텀
	void LoadCustomLobbyBack( bool bFadeIn = false );
	void ClearLobbyBackground();
	bool CopyCustomLobbyBack( char *szFullName );
	bool IsCustomLobbyBack();
	void CustomLobbyRenderFadeIn( int iAlphaRate );

public:  // 로딩 커스텀
	void LoadCustomLoadingBack();
	void ClearLoadingBackground();
	bool CopyCustomLoadingBack( char *szFullName );
	bool IsCustomLoadingBack();

public:  // 전투 대기방 커스텀
	void LoadCustomBattleBack();
	void ClearBattleBackground();
	bool CopyCustomBattleBack( char *szFullName );
	bool IsCustomBattleBack();

public:  // 래더 대기방 커스텀
	void LoadCustomLadderBack();
	void ClearLadderBackground();
	bool CopyCustomLadderBack( char *szFullName );
	bool IsCustomLadderBack();

public:  // 모드 검색 커스텀(훈련소/광장 등)
	void LoadCustomFindModeBack( bool bFadeIn = false );
	void ClearFindModeBackground();
	bool CopyCustomFindModeBack( char *szFullName );
	bool IsCustomFindModeBack();
	void CustomFindModehRenderFadeIn( int iAlphaRate );

public:
	void SetBackground( BackGroundType eBackType );
	void SetLoading( const ioHashString &rkLoading );

public:
	ioUIRenderImage *GetLoading( bool bFirstLoading, int iModeType, int iMyRoomType );

public:
	void ProcessBackground();
	void RenderBackground();

public:
	static ioInterfaceBackground &GetSingleton();

public:
	ioInterfaceBackground();
	virtual ~ioInterfaceBackground();
};

#define g_BackgroundMgr ioInterfaceBackground::GetSingleton()

