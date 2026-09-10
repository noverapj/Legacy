#pragma once


class ioPlayStage;
class ChatMacroMgr : public Singleton< ChatMacroMgr >
{
protected:
	ioPlayStage  *m_pPlayStage;

	DWORD m_dwMacroUserInfoTime;
	DWORD m_dwMacroGuildInfoTime;
	DWORD m_dwMacroChannelMoveTime;

public:
	void SetPlayStage( ioPlayStage *pPlayStage );

protected:                      // 채팅 매크로
	void OnMacroHelp();
	void OnMacroContribute();
	void OnMacroLadderTeamLeave();
	void OnMacroScreenShotMode();
	void OnMacroHeadQuaterPage( char *szMacro );
	void OnMacroUIInitialize();
	void OnMacroAwardList();
	void OnMacroDeveloperInfo();
	void OnMacroDeveloperRoomEnter( char *szMacro );
	void OnMacroDeveloperUDP( char *szMacro );
	void OnMacroUserInfo( char *szMacro );
	void OnMacroGuildInfo( char *szMacro );
	void OnMacroCurrentTime();
	void OnMacroShowAnnouncementWnd();
	void OnMacroBlockGuildMark( char *szMacro );
	void OnMacroMovieMode();
	void OnMacroUserRecord();
	void OnMacroTeamInfo();
	void OnMacroChannelMove( char *szMacro );
	void OnMacroChannelInfo( char *szMacro );
	void OnMacroCampInfo();
	void OnMacroCampUser( char *szMacro );
	void OnMacroGuildNameChange( char *szMacro );
	void OnMacroEventTest();
	void OnMacroWholeChatOnOff( bool bOff );
	void OnMacroMapCameraEvent( char *szMacro );
	void OnMacroFontTexture( char *szMacro );
	void OnMacroQuestGuide( char *szMacro );
	void OnMacroQuestOccur( char *szMacro );
	void OnMacroQuestAttain( char *szMacro );
	void OnMacroQuestDelete();
	void OnMacroPresentDelete();
	void OnMacroChatMode();
	void OnMacroEnterTraining( ioPlayStage *pPlayStage, bool bSafetyRoom );
	void OnMacroEnterFighting( ioPlayStage *pPlayStage );
	void OnMacroJoinPlaza( ioPlayStage *pPlayStage, char *szMacro, bool bInvite );
	void OnMacroChatInfoOnOff( bool bOff );
	void OnMacroControlSyncExOnOff( int iMacro );
	void OnMacroEventShopGoodsList( char *szMacro );
	void OnMacroEventShopGoodsBuy( char *szMacro );
	void OnMacroEventShopState( char *szMacro );
	void OnMacroEventShopBuyUserClear( char *szMacro );
	void OnMacroCustomDDSFromJPG( char *szMacro );
	void OnMacroCustomJPGMixDDSFromDDS( char *szMacro );
	void OnMacroCustomDDSFromLSC( char *szMacro );
	void OnMacroCustomLSCFromDDS( char *szMacro );
	void OnMacroCustomEntityTexture( char *szMacro );
	void OnMacroShowWnd( char *szMacro );
	void OnMacroPresent( char *szMacro );
	void OnMacroBestFriendApplication( char *szMacro );
	void OnMacroBestFriendAgree( char *szMacro );
	void OnMacroBestFriendRefuse( char *szMacro );
	void OnMacroBestFriendClear( char *szMacro );
	void OnMacroMultiMapChange( char *szMacro );
	void OnMacroMacroNotify( char *szMacro );
	void OnMacroExtraItemGrowthCatalyst( char *szMacro );
	void OnMacroGrowthLevelup( char *szMacro );
	void OnMacroMonsterClose( char *szMacro );
	void OnMacroBingoPresent( char *szMacro );
	void OnMacroMonsterSpawn( char *szMacro );

	void OnMacroSuperGashaponAllPackage( char *szMacro );
	void OnMacroSuperGashaponPackage( char *szMacro );
	void OnMacroSuperGashaponInfo( char *szMacro );
	void OnMacroSuperGashaponInfoReset( char *szMacro );

	void OnMacroSendReciveCheckOn( char *szMacro );
	void OnMacroSendReciveCheckOff( char *szMacro );

	void OnMacroShowAttendanceWnd( char *szMacro );
	void OnMacroAttendanceAddDay( char *szMacro );
	void OnMacroAttendancePrevMonth( char *szMacro );
	void OnMacroAttendanceReset( char *szMacro );
	void OnMacroAttendanceDateModify( char *szMacro );

	void OnMacroAwake( char *szMacro );
	void OnMacroCharPowerUpAdd( char *szMacro );
	void OnMacroItemSlotIndex( char *szMacro );
	void OnMacroItemPowerUpAdd( char *szMacro );
	void OnMacroCannotOpenDDSFile( char *szMacro );
	void OnMacroCharInfoWndShow( char *szMacro );
	void OnMacroSetDateMission( char *szMacro );
	void OnMacroSetNextDateMission( char *szMacro );
	void OnMacroSetMissionValue( char *szMacro );
	void OnMacroRollBookWndShow( char *szMacro );
	void OnMacroRollBookProgress( char *szMacro );
	void OnMacroFPS( char *szMacro );
	void OnMacroTimerChange( char *szMacro );
	void OnMacroFpsLagCheck( char *szMacro );
	void OnMacroHousingTest( char *szMacro );
	void OnMacroHousingCommand( char *szMacro );
	void OnMacroHousingBlockSave( char *szMacro );
	void OnMacroComfortableLife( char *szMacro );
	void OnMacroTournamentNextState( char *szMacro );
	void OnMacroTournamentResult( char *szMacro );
	void OnMacroOpenPopup( char* szMacro);
	void OnMacroPracticeCompleted( char* szMacro);
#if defined( SRC_OVERSEAS )
	void OnMacroGashaponAllPackage( char *szMacro );
#endif

	int GetKeyword( IN char *szMacro , OUT char *szKeyword,  int iKeywordSize );
	int ReturnEtcItemMacro( int iMacroType, char *szMacro );

	bool SetCreateStructMacro( int iMacroType, char *szMacro );

public:
	void GetEtcItemMacro( char *szMacro, IntVec& MacroItemList );
	int MacroProcess( const char *szChat , bool bSysMsg = false );

	bool CheckCreateStructMacro( char *szMacro );

//임시
protected:
	void GetFilesFromDirectory( const char *Path, ioHashStringVec &szFileList );
	void LSCAllDecypt( char *szMacro );
	bool ExceptMacro( int iMacroType );

public:
	static ChatMacroMgr& GetSingleton();
		
public:
	ChatMacroMgr();
	virtual ~ChatMacroMgr();
	
};

#define g_ChatMacroMgr ChatMacroMgr::GetSingleton()
