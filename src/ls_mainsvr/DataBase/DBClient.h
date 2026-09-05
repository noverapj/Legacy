// DBClient.h: interface for the DBClient class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "../QueryData/QueryData.h"
#include "../Util/cSerialize.h"
#include "../NodeInfo/PracticeNodeManager.h"


//DB AGENT MSG TYPE
// GET : Select , SET : Insert , DEL : Delete , UPD : Update
#define DBAGENT_ITEM_BUYCNT_SET						0x0001
#define DBAGENT_TOTAL_REG_USER_SET					0x0002

#define DBAGENT_GUILD_INFO_GET						0x0003
#define DBAGENT_GUILD_ENTRY_DELAY_MEMBER_DEL		0x0004
#define DBAGENT_GUILD_DELETE_DEL					0x0005
#define DBAGENT_GUILD_RECORD_UPD					0x0006
#define DBAGENT_GUILD_RANKING_UPD					0x0007

#define DBAGENT_CAMP_DATA_GET						0x0008
#define DBAGENT_CAMP_SEASON_UPD                     0x0009
#define DBAGENT_CAMP_SEASON_END_PORCESS_UPD         0x0010
#define DBAGENT_CAMP_SEASON_END_SERVER_CLOSE_UPD    0x0011
#define DBAGENT_CAMP_SPECIAL_USER_COUNT_GET         0x0012 
#define DBAGENT_CAMP_SEASON_END_FIRST_BACKUP_SET    0x0013
#define DBAGENT_CAMP_SEASON_END_LAST_BACKUP_SET     0x0014
#define DBAGENT_CAMP_DATA_UPD                       0x0015
#define DBAGENT_HERO_EXPERT_MINUS_TO_ZERO_UPD       0x0016

#define DBAGENT_TRADE_INFO_GET						0x0017
#define DBAGENT_TRADE_DELETE						0x0018
#define DBAGENT_SERVER_INFO_SET					    0x0019

#define DBAGENT_TOURNAMENT_DATA_GET                 0x0020
#define DBAGENT_TOURNAMENT_DATA_UPD                 0x0021
#define DBAGENT_TOURNAMENT_TEAM_DEL                 0x0022
#define DBAGENT_TOURNAMENT_TEAM_LIST_GET            0x0023
#define DBAGNET_TOURNAMENT_ALL_DEL                  0x0024
#define DBAGNET_TOURNAMENT_BACKUP                   0x0025
#define DBAGENT_TOURNAMENT_TEAM_POINT_SAVE_UPD      0x0026
#define DBAGENT_TOURNAMENT_WINNER_HISTORY_SET       0x0027
#define DBAGENT_TOURNAMENT_REWARD_ADD               0x0028
#define DBAGENT_TOURNAMENT_CUSTOM_INFO_GET          0x0029
#define DBAGENT_TOURNAMENT_CUSTOM_ROUND_GET         0x0030
#define DBAGENT_TOURNAMENT_CUSTOM_DATA_GET          0x0031
#define DBAGENT_TOURNAMENT_CONFIRM_USER_SET         0x0032
#define DBAGENT_TOURNAMENT_CONFIRM_USER_LIST        0x0033
#define DBAGENT_TOURNAMENT_INFO_DATA_SAVE_UPD       0x0034
#define DBAGENT_TOURNAMENT_ROUND_DATA_SAVE_UPD      0x0035
#define DBAGENT_TOURNAMENT_CUSTOM_DATA_DELETE       0x0036
#define DBAGENT_TOURNAMENT_CUSTOM_BACKUP            0x0037
#define DBAGENT_TOURNAMENT_CUSTOM_REWARD            0x0038
#define DBAGENT_RELATIVE_GRADE_UPDATE               0x0039
#define DBAGENT_TOURNAMENT_PREV_CAMP_INFO_GET		0x0040
#define DBAGENT_USERBLOCK_SET						0x0041
#define DBAGENT_EVENTSHOP_BUYCOUNT_GET				0x0042
#define DBAGENT_EVENTSHOP_BUYCOUNT_SET				0x0043
#define DBAGENT_EVENTSHOP_BUYCOUNT_DEL				0x0044
#define DBAGENT_RESET_BINGONUMBER					0x0045
#define DBAGENT_RESET_MISSION_DATA					0x0046
#define DBAGENT_DELETE_OLD_MISSION_DATA				0x0047

#define DBAGENT_PRACTICE_TOTAL_INFO_GET				0x0050
#define DBAGENT_PRACTICE_INDEX_RANK_PRESENT			0x0051
#define DBAGENT_PRACTICE_BLOCKUSER_LIST				0x0052

#define DBAGENT_CHANGE_USERBLOCK_SET				0x0060
#define DBAGENT_GAME_PINGPONG						0x0999




//작업 방식
#define _INSERTDB       0
#define _DELETEDB       1
#define _SELECTDB       2
#define _UPDATEDB       3   
#define _SELECTEX1DB    4 

//결과 행동
#define _RESULT_CHECK   0
#define _RESULT_NAUGHT  1
#define _RESULT_DESTROY 2

class CConnectNode;
class DBClient : public CConnectNode 
{
private:
	static DBClient *sg_Instance;
	DWORD	m_dwCurrentTime;
	int		m_iClassPriceTime;
	int		m_iDBAgentThreadID;
	bool	m_bOnceRun;
protected:
	ioHashString	m_DBAgentIP;
	int				m_iDBAgentPort;

	cSerialize		m_FT;
	vVALUETYPE		m_VT;
	CQueryData		m_Query;

public:
	static DBClient &GetInstance();
	static void ReleaseInstance();

private:
	ValueType GetValueType(VariableType nType,int len);

public:
	inline ioHashString &GetDBAgentIP(){ return m_DBAgentIP; }
	inline int GetDBAgentPort(){ return m_iDBAgentPort; }

public:
	virtual void SessionClose( BOOL safely=TRUE );
	virtual bool SendMessage( CPacket &rkPacket );
	virtual void ReceivePacket( CPacket &packet );
	virtual void PacketParsing( CPacket &packet );

public:
	virtual void OnCreate();       //초기화
	virtual void OnDestroy();
	virtual bool CheckNS( CPacket &rkPacket );	
	virtual int  GetConnectType();

public:
	bool ConnectTo();

private:
	void OnPing();
	void OnClassPriceInfo();
	void OnTotalRegUserInfo();

public:
	void ProcessTime();

public:
	void Reset();

	void OnClose( SP2Packet &packet );
	void OnInsertGameServerInfo( const int64 serverId, const ioHashString &szIP, const int iSSPort, const ioHashString &szName, const int iCSPort );
	void OnSelectItemBuyCnt();
	void OnSelectTotalRegUser( bool bServerDown = false );

	void OnSelectGuildInfoList( int iGuildIDX, int iSelectCount );
	void OnDeleteGuildEntryDelayMember( DWORD dwGuildIndex );
	void OnDeleteGuild( DWORD dwGuildIndex );
	void OnUpdateGuildRecord( DWORD dwGuildIndex, int iWinCount, int iLoseCount, int iKillCount, int iDeathCount );
	void OnUpdateGuildRanking( DWORD dwGuildIndex, DWORD dwRank, DWORD dwGuildPoint, DWORD dwCurGuildPoint, DWORD dwGuildLevel, DWORD dwMaxEntry );

	void OnSelectCampData();
	void OnUpdateCampData( int iBlueCampPoint, int iBlueCampTodayPoint, int iBlueCampBluePoint, int iRedCampPoint, int iRedCampTodayPoint, int iRedCampBluePoint );
	void OnSelectCampSpecialUserCount( int iCampType );

	//
	void OnInitCampSeason();
	void OnEndCampSeasonProcess();
	void OnEndCampSeasonServerClose();
	void OnEndCampSeasonFirstBackup();
	void OnEndCampSeasonLastBackup();
	void OnHeroExpertMinusToZero();

	//
	void OnSelectTradeItemInfo( int iIndex, int iSelectCount );
	void OnTradeItemDelete( DWORD dwTradeIndex, DWORD dwServerIndex );

	//
	void OnSelectTournamentData( DWORD dwIndex, int iSelectCount );
	void OnSelectTournamentCustomData( DWORD dwIndex, DWORD dwUserIndex, DWORD dwServerIndex );
	void OnUpdateTournamentData( DWORD dwIndex, DWORD dwStartDate, DWORD dwEndDate, BYTE Type, BYTE State );
	void OnDeleteTournamentTeam( DWORD dwTeamIndex );
	void OnDeleteTournamentAllTeam( DWORD dwTourIndex );
	void OnUpdateTournamentBackUP( DWORD dwTourIndex );
	void OnSelectTournamentTeamList( DWORD dwTeamdIndex, int iSelectCount );
	void OnUpdateTournamentTeamPointSave( DWORD dwTeamIndex, SHORT Position, SHORT StartPosition, BYTE TourPos, int iLadderPoint );
	void OnInsertTournamentWinnerHistory( const ioHashString &rkTitle, DWORD dwStartDate, DWORD dwEndDate, DWORD dwTeamIndex, const ioHashString &rkTeamName, const ioHashString &rkCampName, int iCampPos ); 
	void OnInsertTournamentRewardAdd(DWORD dwStartDate, DWORD dwTourIndex, short StartTeamCount, short sCheerTeamAdjust, short sCheerUserAdjust );
	void OnSelectTournamentCustomInfo( DWORD dwTourIndex );
	void OnSelectTournamentCustomRound( DWORD dwTourIndex, DWORD dwTourInfoIndex );
	void OnInsertTournamentConfirmUser( DWORD dwTourIndex, DWORD dwUserIndex );
	void OnSelectTournamentConfirmUserList( DWORD dwTourIndex, DWORD dwLastUserIndex, int iSelectCount );
	void OnUpdateTournamentInfoDataSave( DWORD dwTourInfoIndex, ioHashString &rkAnnounce, DWORD dwAppDate, DWORD dwDelayDate );
	void OnUpdateTournamentRoundDataSave( cSerialize &v_FT );
	void OnDeleteTournamentCustomData( DWORD dwTourIndex );
	void OnInsertTournamentCustomBackup( DWORD dwTourIndex );
	void OnInsertTournamentCustomReward( DWORD dwTourIndex );
	void OnSelectPrevTournamentChampInfo( DWORD dwTourIndex );
	
	// 
	void OnUserBlock(const ioHashString& szPublicID, BYTE byLimitType, CTime limitTime, const ioHashString& szReportID, const ioHashString& szReportIP, const ioHashString& szReason, const ioHashString& szNote, const DWORD dwMgrToolIndex );
	void OnChangeUserBlock(const int ClientNumber, const ioHashString& szPublicID, BYTE byLimitType, CTime limitTime, const ioHashString& szReportID, const ioHashString& szReportIP, const ioHashString& szReason, const ioHashString& szNote, const DWORD dwMgrToolIndex );
	void OnUpdateRelativeGrade( DWORD dwUniqueCode, int iReduceRate );
	
	void OnDeleteGoodsBuyCount(const BYTE byType);
	void OnSelectGoodsBuyCount(const int iCount, const int iPage);
	void OnUpdateGoodsBuyCount(const DWORD dwUserIndex, const BYTE byType, const DWORD dwGoodsIndex, const BYTE byCount);
	void OnResetBingoNumber();
	void OnResetOldMissionData( IN int iYear, IN int iMonth, IN int iDay );

	// 수련장
	void OnSelectPracticeInfoList( DWORD dwUserIndex, int iPracticeIndex, int iSelectCount );
	void OnPractice_Index_RankPresent(ioHashString szSenderId, std::vector< SPracticeReward > vSPracticeReward);
	void OnPractice_BlockUserList(ioHashString szKickId);

private:			/* Singleton Class */
	DBClient( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize );
	virtual ~DBClient();

	void OnDeleteOldMissionData();
};

#define g_DBClient DBClient::GetInstance()
