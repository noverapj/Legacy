#pragma once

class ioNamedTitleInfoManager : public Singleton< ioNamedTitleInfoManager >
{
protected:
	NamedTitleInfoList m_vInfoList;

	DWORD m_dwRedBaseEquipColor;
	DWORD m_dwBlueBaseEquipColor;

	DWORD m_dwTextBaseColor;
	DWORD m_dwTextPremiumColor;
	DWORD m_dwTextEquipColor;

	ioHashString m_szNamedTitleIcon;	
	ioHashString m_szNamedTitlePremiumIcon;
	ioHashString m_szNamedTitleUIMark;
	ioUIRenderImage* m_pPremiumBaseMark;

	DWORD m_dwEquipRefreshTime;

protected:
	DWORD m_dwCurEquipCode;
	DWORD m_dwEquipTime;
		
public:
	static ioNamedTitleInfoManager& GetSingleton();

public:
	void ClearData();
	void LoadProperty();

protected:
	void LoadBase( ioINILoader &rkLoader );
	
public:
	bool GetNamedTitleInfo( DWORD dwCode, NamedTitleInfo &sNamedTitleInfo );
	bool GetNamedTitleGrade( DWORD dwCode, int nLevel, NamedTitleGrade &sNamedTitleGrade );
	bool IsEnableUpgrade( DWORD dwCode, int nLevel );

public:
	ioHashString GetNamedTitleName( DWORD dwCode );
	ioHashString GetNamedTitleIcon();
	ioHashString GetNamedTitlePremiumIcon();
	ioHashString GetNamedTitleUiMark();
	DWORD GetNamedTitleTextColor( DWORD dwCode, int nLevel, BYTE byPremium, BYTE byEquip );

public:
	void RenderNamedTitle( int nXPos, int nYPos, float fFontSize, DWORD dwCode, int nLevel, BYTE byPremium, TeamType eTeam, ioUIRenderImage* pCurMark, UIRenderType eRenderType = UI_RENDER_NORMAL );
	
public:
	void OnNamedTitleUpdate( SP2Packet &rkPacket, ioPlayStage* pPlayStage );
	void OnNamedTitleChange( SP2Packet &rkPacket, ioPlayStage* pPlayStage );
	void OnNamedTitlePremium( SP2Packet &rkPacket );
	void OnNamedTitleEtcUse( SP2Packet &rkPacket );
	void OnNamedTitleEquip( SP2Packet &rkPacket, ioPlayStage* pPlayStage );

protected:
	void OnNamedTitleInsertSuccess( SP2Packet &rkPacket );
	void OnNamedTitleUpdateSuccess( SP2Packet &rkPacket );
	void OnNamedTitleLevelUpSuccess( SP2Packet &rkPacket, ioPlayStage* pPlayStage );
	void OnNamedTitlePremiumSuccess( SP2Packet &rkPacket, ioPlayStage* pPlayStage );

public:
	void Process();

public:
	ioNamedTitleInfoManager();
	virtual ~ioNamedTitleInfoManager();
};

#define g_NamedTitleInfoMgr ioNamedTitleInfoManager::GetSingleton()