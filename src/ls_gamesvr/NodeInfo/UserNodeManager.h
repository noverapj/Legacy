// UserNodeManager.h: interface for the UserNodeManager class.
//
//////////////////////////////////////////////////////////////////////

#pragma once


#include "User.h"
#include "UserCopyNode.h"
class CQueryResultData;
using namespace std;

#define FRIEND_USER_POINT   		   1000
#define WEB_UDP_KEY_VALUE              "ei@TSo^#903Rfkdoep45"

typedef struct tagSortUser
{
	UserParent  *m_pUser;
	int          m_iSortPoint; // 낮을 수록 순위가 높다.
	tagSortUser()
	{
		m_pUser      = NULL;
		m_iSortPoint = FRIEND_USER_POINT;
	}
}SortUser;

enum ACCOUNT_TYPE
{
	IP_DEFAULT = 0,
	IP_ADMIN   = 1,
	IP_CLOUD   = 2
};

typedef vector< SortUser > vSortUser;
typedef vSortUser::iterator vSortUser_iter;

class UserSort
{
public:
	bool operator()( const SortUser &lhs , const SortUser &rhs ) const
	{
		if( lhs.m_iSortPoint < rhs.m_iSortPoint )
		{
			return true;
		}
		//else if( lhs.m_iSortPoint == rhs.m_iSortPoint )
		//{
		//	if(lhs.m_pUser && rhs.m_pUser)
		//	{
		//		int iCmpValue = _stricmp( lhs.m_pUser->GetPublicID().c_str(), rhs.m_pUser->GetPublicID().c_str() );	
		//		if( iCmpValue < 0 )
		//			return true;
		//	}
		//}
		return false;
	}
};

typedef boost::unordered_map<DWORD,User*> uUser;
typedef uUser::iterator uUser_iter;

typedef vector<User*> vUser;
typedef vUser::iterator vUser_iter;

typedef struct tagTempUserData  
{
	DWORD m_dwTableIndex;
	DWORD m_dwUserIndex;
	int   m_iGradeLevel;
	int   m_iLadderPoint;
	ioHashString m_szUserID;
	ioHashString m_szUserPos;
	tagTempUserData()
	{
		m_dwTableIndex = m_dwUserIndex = 0;
		m_iGradeLevel  = m_iLadderPoint = 0;
	}
}TempUserData;
typedef vector<TempUserData> vTempUserData;

class UserNodeManager
{
private:
	static UserNodeManager *sg_Instance;

	// 원본 유저 노드
	vUser	                m_vUserNode;
	uUser					m_uUserNode;
	MemPooler<User>			m_MemNode;

	// 복사 유저 노드
	uUserCopyNode           m_uUserCopyNode;
	uUserCopyNodeTable		m_uUserCopyNodePublicID;
	uUserCopyNodeTable		m_uUserCopyNodePrivateID;

	//
	DWORD                   m_dwNodeGhostCheckTime;
	DWORD                   m_dwNodeSaveCheckTime;
#ifdef XTRAP
	DWORD                   m_dwNodeXtrapCheckTime;
#endif
#ifdef NPROTECT
	DWORD                   m_dwNodeNProtectCheckTime;
#endif 
#ifdef HACKSHIELD
	DWORD                   m_dwNodeHackShieldCheckTime;
#endif 
	DWORD                   m_dwNodeRefillMonsterCoinTime;
	DWORD                   m_dwNodeTimeGashaponCheckTime;
	DWORD                   m_dwNodeSyncTime;
	DWORD                   m_dwNodeDestoryTime;
	DWORD                   m_dwNodeShutDownCheckTime;
	DWORD                   m_dwNodeSelectShutDownCheckTime;
	DWORD                   m_dwNodeEventProcessTime;
	DWORD					m_dwNodeCloverCheckTime;
	DWORD                   m_dwNodeRefillRaidTime;
	DWORD                   m_dwNodeRefillOakBarrelSwordTime;
	DWORD                   m_dwNodeTimeGateCheckTime;
	int                     m_iMaxConnection;
	int						m_iStableConnection;

	ioHashStringVec         m_vLikeDeveloperID;
	ioHashStringVec         m_vDeveloperID;

protected:
	struct CharRentalData
	{
		DWORD m_dwUserIndex;
		CTime m_RentalTime;
		CharRentalData()
		{
			m_dwUserIndex = 0;
		}
	};
	typedef std::vector< CharRentalData > CharRentalList;

protected:
	struct UserSyncData
	{
		DWORD m_dwSyncTime;
		User  *m_pUser;

		UserSyncData()
		{
			m_pUser = NULL;
			m_dwSyncTime = 0;
		}
	};
	typedef std::vector< UserSyncData > UserSyncDataVec;
	UserSyncDataVec m_UserSyncDataVec;

protected:
	// 오크통 일일한도 수량 체크 타임
	CTime m_OakBarrelInitTime;
	
	//HRYOON0825 게임방 입장로그
	int m_iMakePrisonerRoom;		//포탈방 생성
	int m_iQuickStartPrisoner;		//포탈 빠른생성
	int m_iListSelectPrisoner;		//포탈 방리스트에서 선택 

public:
	static UserNodeManager &GetInstance();
	static void ReleaseInstance();

public:
	void InitMemoryPool();
	void ReleaseMemoryPool();

public:
	User *CreateNewNode(SOCKET s);
	void AddUserNode(User *pUserNode);
	void ChangeUserNode(User *pUserNode);
	void RemoveNode(User *pUserNode);
	void MoveUserNode(User *pUserNode);
	void DisconnectNode( ioHashString &szID );
	void ServerMovingPassiveLogOut( SP2Packet &rkPacket );

public:        // 타서버가 원본인 전투/래더/채널 노드가 없어질 때 처리
	void RemoveBattleRoomCopyNode( DWORD dwIndex );
	void RemoveLadderTeamCopyNode( DWORD dwIndex );
	void RemoveShuffleRoomCopyNode( DWORD dwIndex );
	void RemoveChannelCopyNode( DWORD dwIndex );

public:
	void AddCopyUser( UserCopyNode *pUser );
	void RemoveCopyUser( const DWORD dwUserIndex );	
	void RemoveCopyUserTablePrivateID( const ioHashString &szPrivateID );
	void RemoveCopyUserTablePublicID( const ioHashString &szPublicID );
	UserCopyNode* GetUserCopyNode( const DWORD dwUserIndex );
	UserCopyNode* GetUserCopyNodeByPublicID( const ioHashString &szPublicID );
	UserCopyNode* GetUserCopyNodeByPrivateID( const ioHashString &szPrivateID );

public:
	void UpdateUserSync( User *pUser );

public:
	int RemainderNode()			{ return m_MemNode.GetCount(); }
	int GetMaxConnection()		{ return m_iMaxConnection; }
	int GetStableConnection()	{ return m_iStableConnection; }

public:
	void ViewUserNode();
	void LoadDeveloperID();


	bool IsDeveloper( const char *szID );
	bool IsAfford();

public:
	int GetNodeSize()			{ return (m_vUserNode.size() + m_uUserNode.size()); }
	int GetNodeSizeByChannelingType( ChannelingType eChannelingType );
	int GetCopyUserNodeSize(){ return m_uUserCopyNode.size(); }
	
public:
	User* GetUserNode( DWORD dwIndex );								//유저 인덱스
	User* GetUserNodeByPublicID( const ioHashString &szID );		//유저 아이디 
	User* GetUserNodeByPrivateID( const ioHashString &szID );		//유저 로그인 아이디 	
	User* GetUserNodeByGUID( const ioHashString &szUserGUID );
	User* GetMoveUserNode( DWORD dwIndex, int iMovingValue );
	User* GetUserInfo( DWORD index);
	BOOL GetUserInfos(vUser& vUserInfos, int iStart, int iEnd);

	UserParent *GetGlobalUserNode( DWORD dwIndex );
	UserParent *GetGlobalUserNode( const ioHashString &szPublicID );
	UserParent *GetGlobalUserNodeByPrivateID( const ioHashString &szPrivateID );

public:
	bool IsConnectUser( const ioHashString &szPrivateID );	
	void SendPingUser( const ioHashString &szID );

public:
	bool IsPublicIDConnectUser( const DWORD dwUserIndex );
	void SetNodeShutDownCheckTime( DWORD dwNodeShutDownCheckTime ) { m_dwNodeShutDownCheckTime = dwNodeShutDownCheckTime; }

public:
	void InitUserEventItem( const int iType );
	void InitUserLadderPointNRecord();
	void AllUserUpdateLadderPointNExpert();
	void AllCampUserCampDataSync();
	void AllUserTournamentTeamDelete( DWORD dwTourIndex );
	void AllUserTournamentTeamPosSync( DWORD dwTeamIndex, SHORT Position, BYTE TourPos, bool bSync );
	void UserTournamentBattleInvite( DWORD dwBattleIndex, DWORD dwBlueIndex, DWORD dwRedIndex );
	void UserTournamentBattleInvite( DWORD dwBattleIndex, DWORD dwTeamIndex, DWORD dwBlueIndex, DWORD dwRedIndex );

	void AllUserLeaveServer(bool bBlock=true);
	void AllUserDisconnect(const int iDBAgentIndex);

public:  // 초대 리스트
	void SendPlazaInviteList( User *pUser, PlazaType ePlazaType, int iCurPage, int iMaxCount, int iRoomLevel );
	void SendBattleRoomInviteList( User *pUser, int iCurPage, int iMaxCount, int iBattleMatchLevel, int iSelectModeTerm );
	void SendLadderTeamInviteList( User *pUser, int iCurPage, int iMaxCount );
	void SendServerLobbyUserList( User *pUser, int iCurPage, int iMaxCount );
	void SendHeadquartersInviteList( User *pUser, int iCurPage, int iMaxCount, int iRoomLevel );

public:  // 서버 동기화
	void ConnectServerNodeSync( ServerNode *pServerNode );

public:
	void UpdateRelativeGradeAllUser( DWORD dwUniqueCode );

public:       //DB RESULT
	bool GlobalQueryParse(SP2Packet &packet);

	// Recv Query.
	void OnResultPingPong( CQueryResultData *query_data );
	void OnResultUpdateUserCount( CQueryResultData *query_data );
	void OnResultUpdateUserMoveServerID( CQueryResultData *query_data );
	void OnResultUpdateServerOn( CQueryResultData *query_data );
	void OnResultDeleteGameServerInfo(CQueryResultData *query_data);
	void OnResultSelectSecondEncryptKey(CQueryResultData *query_data);

	void OnResultSelectUpdateUserLoginInfo(CQueryResultData *query_data);
	void OnResultSelectUserLoginInfo(CQueryResultData *query_data);

	void OnResultUpdateUserLogout(CQueryResultData *query_data);
	void OnResultUpdateALLUserLogout(CQueryResultData *query_data);

	void OnResultFindUserData(CQueryResultData *query_data);
	void OnResultSelectUserData(CQueryResultData *query_data);
	void OnResultUpdateUserData(CQueryResultData *query_data);
	void OnResultUpdateUserLadderPoint(CQueryResultData *query_data);
	void OnResultUpdateUserHeroExpert(CQueryResultData *query_data);
	void OnResultInitUserHeroExpert(CQueryResultData *query_data);

	void OnResultLoginSelectControlKeys(CQueryResultData *query_data);
	void OnResultLoginSelectAllAwardData(CQueryResultData *query_data);
	void OnResultLoginSelectAwardExpert(CQueryResultData *query_data);
	void OnResultLoginSelectAllClassExpert(CQueryResultData *query_data);
	void OnResultLoginSelectUserRecord(CQueryResultData *query_data);
	void OnResultLoginSelectAllEtcItemData(CQueryResultData *query_data);
	void OnResultLoginSelectAllExtraItemData(CQueryResultData *query_data);
	void OnResultLoginSelectAllQuestCompleteData(CQueryResultData *query_data);
	void OnResultLoginSelectAllQuestData(CQueryResultData *query_data);
	void OnResultLoginSelectAllCharData(CQueryResultData *query_data);
	void OnResultLoginSelectAllInvenData(CQueryResultData *query_data);
	void OnResultLoginSelectAllGrowth(CQueryResultData *query_data);
	void OnResultLoginSelectAllMedalItemData(CQueryResultData *query_data);
	void OnResultLoginSelectAllExMedalSlotData(CQueryResultData *query_data);
	void OnResultLoginSelectAllEventData(CQueryResultData *query_data);
	void OnResultLoginSelectAllAlchemicData(CQueryResultData *query_data);
	void OnResultLoginSelectAllPetItemData( CQueryResultData* query_data );
	void OnResultLoginSelectCostumeData(CQueryResultData* query_data);
	void OnResultLoginSelectAccessoryData(CQueryResultData* query_data);

	void OnResultInsertCharData(CQueryResultData *query_data);
	void OnResultSelectCharIndex(CQueryResultData *query_data);
	void OnResultSelectCharData(CQueryResultData *query_data);	
	void OnResultUpdateCharData(CQueryResultData *query_data);
	void OnResultDeleteCharData(CQueryResultData *query_data);
	void OnResultDeleteCharLimitDate(CQueryResultData *query_data);
	void OnResultUpdateCharRegDate(CQueryResultData *query_data);
	void OnResultSelectCharRentalHistory(CQueryResultData *query_data);

	void OnResultInsertClassExpert(CQueryResultData *query_data);
	void OnResultSelectClassExpertIndex(CQueryResultData *query_data);
	void OnResultUpdateClassExpert(CQueryResultData *query_data);

	void OnResultInsertInvenData(CQueryResultData *query_data);
	void OnResultSelectInvenIndex(CQueryResultData *query_data);
	void OnResultUpdateInvenData(CQueryResultData *query_data);

	void OnResultInsertEtcItemData(CQueryResultData *query_data);
	void OnResultSelectEtcItemIndex(CQueryResultData *query_data);
	void OnResultUpdateEtcItemData(CQueryResultData *query_data);

	void OnResultUpdateAwardExpert(CQueryResultData *query_data);
	void OnResultInsertAwardData(CQueryResultData *query_data);
	void OnResultSelectAwardIndex(CQueryResultData *query_data);
	void OnResultUpdateAwardData(CQueryResultData *query_data);

	void OnResultSelectItemBuyCnt(CQueryResultData *query_data);
	void OnResultUpdateItemBuyCnt(CQueryResultData *query_data);

	void OnResultInsertGrowth(CQueryResultData *query_data);
	void OnResultSelectGrowthIndex(CQueryResultData *query_data);
	void OnResultUpdateGrowth(CQueryResultData *query_data);

	void OnResultInsertFishData(CQueryResultData *query_data);
	void OnResultSelectFishDataIndex(CQueryResultData *query_data);
	void OnResultSelectAllFishData(CQueryResultData *query_data);
	void OnResultUpdateFishData(CQueryResultData *query_data);

	void OnResultInsertExtraItemData(CQueryResultData *query_data);
	void OnResultSelectExtraItemIndex(CQueryResultData *query_data);
	void OnResultUpdateExtraItemData(CQueryResultData *query_data);

	void OnResultSelectFriendList(CQueryResultData *query_data);
	void OnResultSelectFriendRequestList(CQueryResultData *query_data);
	void OnResultSelectFriendApplication(CQueryResultData *query_data);
	void OnResultSelectInsertFriend(CQueryResultData *query_data);
	void OnResultSelectUserIDCheck(CQueryResultData *query_data);
	void OnResultDeleteFriend(CQueryResultData *query_data);
	void OnResultSelectFriendDeveloperInsert(CQueryResultData *query_data);

	void OnResultSelectBestFriendList(CQueryResultData *query_data);
	void OnResultSelectBestFriendAdd(CQueryResultData *query_data);

	void OnResultUpdateEventData(CQueryResultData *query_data);
	void OnResultSelectEventIndex(CQueryResultData *query_data);
	void OnResultSelectUserEntry(CQueryResultData *query_data);

	void OnResultUpdateUserRecord(CQueryResultData *query_data);

    void OnResultInsertQuestData(CQueryResultData *query_data);
	void OnResultSelectQuestIndex(CQueryResultData *query_data);
	void OnResultUpdateQuestData(CQueryResultData *query_data);

	void OnResultInsertQuestCompleteData(CQueryResultData *query_data);
	void OnResultSelectQuestCompleteIndex(CQueryResultData *query_data);
	void OnResultUpdateQuestCompleteData(CQueryResultData *query_data);

	void OnResultInsertAlchemicData(CQueryResultData *query_data);
	void OnResultSelectAlchemicIndex(CQueryResultData *query_data);
	void OnResultUpdateAlchemicData(CQueryResultData *query_data);

	// GUILD
	void OnResultSelectCreateGuild(CQueryResultData *query_data);
	void OnResultSelectCreateGuildReg(CQueryResultData *query_data);
	void OnResultSelectCreateGuildInfo(CQueryResultData *query_data);
	void OnResultSelectUserGuildInfo(CQueryResultData *query_data);
	void OnResultSelectEntryDelayGuildList(CQueryResultData *query_data);
	void OnResultSelectGuildEntryDelayMember(CQueryResultData *query_data);
	void OnResultSelectGuildMemberList(CQueryResultData *query_data);
	void OnResultSelectGuildMemberListEx(CQueryResultData *query_data);
	void OnResultSelectGuildMarkBlockInfo(CQueryResultData *query_data);
	void OnResultSelectGuildNameChange(CQueryResultData *query_data);
	void OnResultSelectGuildEntryApp(CQueryResultData *query_data);
	void OnResultSelectGuildEntryAppMasterGet(CQueryResultData *query_data);
	void OnResultSelectGuildEntryAgree(CQueryResultData *query_data);
	void OnResultSelectGuildEntryAgreeUserGuildInfo(CQueryResultData *query_data);
	void OnResultUpdateGuildMasterChange(CQueryResultData *query_data);
	void OnResultUpdateGuildPositionChange(CQueryResultData *query_data);
	void OnResultSelectGuildSimpleData(CQueryResultData *query_data);
	void OnResultSelectGuildMarkChangeKeyValue(CQueryResultData *query_data);
	void OnResultSelectGuildUserLadderPointADD(CQueryResultData *query_data);
	void OnResultSelectCampSeasonBonus(CQueryResultData *query_data);
	void OnResultSelectUserExist(CQueryResultData *query_data);
	void OnResultSelectPresentData(CQueryResultData *query_data);
	void OnResultSelectUserIndexAndPresentCnt(CQueryResultData *query_data);

	void OnResultSelectSubscriptionData(CQueryResultData *query_data);

	void OnResultSelectPublicIDExist(CQueryResultData *query_data);
	void OnResultSelectChangedPublicID( CQueryResultData *query_data );

	void OnResultSelectBlockType( CQueryResultData *query_data );
	void OnResultInsertTrial(CQueryResultData *query_data);

	void OnResultSelectMemberCount( CQueryResultData *query_data );
	void OnResultInsertMember(CQueryResultData *query_data);

	void OnResultSelectFirstPublicIDExist( CQueryResultData *query_data );
	void OnResultSelectChangedFirstPublicID( CQueryResultData *query_data );

	void OnResultInsertMedalItemData(CQueryResultData *query_data);
	void OnResultSelectMedalItemIndex(CQueryResultData *query_data);
	void OnResultUpdateMedalItemData(CQueryResultData *query_data);

	void OnResultInsertExMedalSlotData(CQueryResultData *query_data);
	void OnResultSelectExMedalSlotIndex(CQueryResultData *query_data);
	void OnResultUpdateExMedalSlotData(CQueryResultData *query_data);

	void OnResultSelectHeroData(CQueryResultData *query_data);
	void OnResultSelectHeroTop100Data(CQueryResultData *query_data);

	// 거래소
	void OnResultSelectCreateTrade(CQueryResultData *query_data);
	void OnResultSelectCreateTradeIndex(CQueryResultData *query_data);
	void OnResultTradeItemComplete(CQueryResultData *query_data);
	void OnResultTradeItemCancel(CQueryResultData *query_data);

	void OnResultDBServerTestLastQuery( CQueryResultData *query_data );

	void OnResultSelectItemCustomUniqueIndex(CQueryResultData *query_data);

	void OnResultSelectHeadquartersDataCount(CQueryResultData *query_data);
	void OnResultSelectHeadquartersData(CQueryResultData *query_data);
	void OnResultSelectUserBirthDate( CQueryResultData* query_data );
	void OnResultSelectFriendRecommendData(CQueryResultData *query_data);

	void OnResultSelectDisconnectCheck(CQueryResultData *query_data);
	void OnResultSelectUserSelectShutDown( CQueryResultData* query_data );

	// 2020-06-05 by bckim, 하드시리얼 유저 제재
	void OnResultSelectDisconnectHddSerialCheck(CQueryResultData *query_data);
	// End. 2020-06-05 by bckim, 하드시리얼 유저 제재

	void OnResultInsertTournamentTeamCreate(CQueryResultData *query_data);
	void OnResultSelectTournamentTeamIndex(CQueryResultData *query_data);
	void OnResultSelectTournamentTeamList(CQueryResultData *query_data);
	void OnResultSelectTournamentCreateTeamData(CQueryResultData *query_data);
	void OnResultSelectTournamentTeamMember(CQueryResultData *query_data);
	void OnResultSelectTournamentTeamAppList(CQueryResultData *query_data);
	void OnResultInsertTournamentTeamAppAdd(CQueryResultData *query_data);
	void OnResultUpdateTournamentTeamAppReg(CQueryResultData *query_data);
	void OnResultSelectTournamentTeamAppAgreeMember(CQueryResultData *query_data);
	void OnResultSelectTournamentAppAgreeTeam(CQueryResultData *query_data);
	void OnResultDeleteTournamentTeamMember(CQueryResultData *query_data);
	void OnResultSelectTournamentHistoryList(CQueryResultData *query_data);
	void OnResultSelectTournamentHistoryUserList(CQueryResultData *query_data);
	void OnResultSelectTournamentReward(CQueryResultData *query_data);
	void OnResultInsertTournamentCustomAdd(CQueryResultData *query_data);
	void OnResultSelectTournamentCustomReward(CQueryResultData *query_data);

	void OnResultSelectCloverInfoRequest( CQueryResultData* query_data );
	void OnResultUpdateFriendReceiveCloverInfo( CQueryResultData* query_data );

	void OnResultSelectBingoNumber( CQueryResultData* query_data );
	void OnResultSelectBingoPresent( CQueryResultData* query_data );

	void OnResultInsertPresent( CQueryResultData* query_data );
	void OnresultInsertPresentByPrivateID( CQueryResultData* query_data );

	void OnResultSelectRelativeGradeInfo( CQueryResultData* query_data );

	void OnResultInsertTournamentCheerDecision( CQueryResultData* query_data );
	void OnResultSelectTournamentCheerList( CQueryResultData* query_data );
	void OnResultSelectTournamentCheerReward( CQueryResultData* query_data );

	void OnResultSelectAttendanceRecord( CQueryResultData* query_data );

	//pet
	void ProcessInsertPetData( User *pUser, CQueryResultData &query_data );
	void SendDiffServerNewPetData( UserParent *pUserParent, DWORD dwUserIndex, CQueryResultData &query_data );
	void OnResultInsertPetData( CQueryResultData &query_data );
	void InsertRespondPacketParsing( DWORD dwPacketID, CQueryResultData *query_data, User *pUser, ioUserPet *pPet );

	//void OnResultSelectPetIndex( CQueryResultData *query_data ); 
	void OnResultUpdatePetData( CQueryResultData *query_data );

	//costume
	void OnResultInsertCostumeData(CQueryResultData *query_data);
	void OnResultUpdateCostumeData(CQueryResultData *query_data);
	void OnResultDeleteCostumeData(CQueryResultData *query_data);

	//Accessory
	void OnResultDeleteAccessoryData(CQueryResultData *query_data);
	void OnResultInsertAccessoryData(CQueryResultData *query_data);

	//mission
	void OnResultGetMissionData(CQueryResultData *query_data);
	void OnResultUpdateMissionData(CQueryResultData *query_data);
	void OnResultInitMissionData(CQueryResultData *query_data);

	//roll book
	void OnResultGetRollBookData(CQueryResultData* query_data);
	void OnResultUpdateRollBookData(CQueryResultData* query_data);

	//for present delete
	void OnResultDeletePresent(CQueryResultData* query_data);

	//길드
	void OnResultGetGuildAttendaceMemberGet(CQueryResultData* query_data);
	void OnResultInsertUserGuildAttendanceInfo(CQueryResultData* query_data);

	void OnResultSelectPirateRouletteNumber(CQueryResultData* query_data);
	void OnResultSelectPirateRoulettePresent(CQueryResultData* query_data);

	//보너스 캐쉬
	void OnResultInsertBonusCash(CQueryResultData* query_data);
	void OnResultUpdateBonusCash(CQueryResultData* query_data);
	void OnResultSelectBonusCash(CQueryResultData* query_data);
	void OnResultSelectExpiredBonusCash(CQueryResultData* query_data);

	// 유저 코인
	void OnResultLoginSelectUserCoin(CQueryResultData* query_data);
	void OnResultInsertUserCoin(CQueryResultData* query_data);
	void OnResultUpdateUserCoin(CQueryResultData* query_data);


public:
	void UserNode_DataSync();
	void UserNode_GhostCheck();
	void UserNode_SaveCheck();
	void ProcessFlush();

	// 2018-06-18 by bckim, 카드 짝 마추기 이벤트 추가
	void UserNode_CardMatchingTickProcess();
	// End. 2018-06-18 by bckim, 카드 짝 마추기 이벤트 추가

	// test
	void SendMessageTest( SP2Packet &rkPacket, User *pOnwer = NULL );

#ifdef XTRAP
	void UserNode_XtrapCheck();
#endif
#ifdef NPROTECT
	void UserNode_NProtectCheck();
#endif 
#ifdef HACKSHIELD
	void UserNode_HackShieldCheck();
#endif 
	void UserNode_RefillMonsterCoin(); 
	void UserNode_RefillClover(); 
	void UserNode_RefillRaidCoin(); 
	void UserNode_TimeGashaponCheck();
	void UserNode_AllSave();
	void UserNode_QuestRemove( DWORD dwMainIndex, DWORD dwSubIndex );
	void UserNode_QuestOneDayRemoveAll();
	void UserNode_AllHeroDataSync();
	void UserNode_ShutDownCheck();
	void UserNode_SelectShutDownCheck();
	void UserNode_EventProcessTime();
	void UserNode_MissionTargetTypeReset(IntVec& vResetTypeList);
	void SendMessageAll( SP2Packet &rkPacket, User *pOwner = NULL );
	void SendMessage( ioHashString &szID, SP2Packet &rkPacket );
	void SendAllServerAlarmMent( SP2Packet &rkPacket );
	void SendUDPMessageAll( SP2Packet &rkPacket );
	void SendUDPMessageCampUserAll( SP2Packet &rkPacket );
	void SendLobbyMessageAll( SP2Packet &rkPacket, User *pOwner = NULL );
	void SendLobbyUDPMessageAll( SP2Packet &rkPacket, User *pOwner = NULL );
	void SendNotUseRelayMessageAll(int relayServerID);
	void SendCompensationToAllUser(const int iType, const int iCode, const int iValue);
	void UserNode_InitOakBarrelLimitSword(); 
	void UserNode_TimeGateCheck();
	void UserNode_InitPractice(); 
public:
	void FindShuffleGlobalQueueUser( User *pUser, int iGlbalSearchingTryCount, DWORDVec& vUserGroup );

protected:
	//개발자 IP
	std::set< ioHashString > m_sDeveloperIPSet;

	//클라우드 IP
	std::set< ioHashString > m_sCloudIPSet;

	//채널링별 현재 유저 수 
	std::map< ChannelingType, int > m_mChannelingCurUserCnt;

	//채널링별 10분간 최대 동접 수
	std::map< ChannelingType, int > m_mChannelingMaxUserCnt;

public:	
	void LoadSpecialIP();

	bool IsCloudIP( const char *szClientIP );
	bool IsAdminIP( const char *szClientIP );

	int GetUserAccountType( const char *szClientIP );

public:
	void InitChannelingUserCntMap();

	void CheckMaxUserCnt( ChannelingType eChannelingType );
	bool IsExistChannelingType( std::map< ChannelingType, int > &mMap, ChannelingType eChannelingType );

	//10분간 최대 동접 인원
	void SetMaxUserCntToCurCnt();
	void SetChannelingMaxUserCnt( ChannelingType eChannelingType, int iUserNum );

	int GetChennelingMaxUserCnt( ChannelingType eChannelingType );

	//현재 동접 수
	void SetChannelingCurUserCnt( ChannelingType eChannelingType, int iUserNum );
	void IncreaseChannelingUserCnt( ChannelingType eChannelingType );
	void DecreaseChannelingUserCnt( ChannelingType eChannelingType );

	//팝업
	void OnResultSelectGetSpentMoney(CQueryResultData *query_data);
	void OnResultSelectGetPopupIndex(CQueryResultData *query_data);

	void OnResultSelectUserChannelingKeyValue(CQueryResultData *query_data);

	//길드 본부
	void OnResultSelectGuildBlockInfos(CQueryResultData *query_data);
	void OnResultGuildBlockRetrieveORDelete(CQueryResultData *query_data);
	void OnResultGuildBlockConstructORMove(CQueryResultData *query_data);
	void OnResultSelectGuildInvenVersion(CQueryResultData *query_data);
	void OnResultSelectGuildInvenInfo(CQueryResultData *query_data);
	void OnResultAddGuildBlockItem(CQueryResultData *query_data);
	void OnResultDefaultConstructGuildBlock(CQueryResultData *query_data);

	void SendGuildInvenInfoWithSQLResult(const DWORD dwUserIndex, const DWORD dwGuildIndex, const int iRequestType);
	void TestEnterGuildRoom(DWORD dwUserIndex, DWORD dwGuildIndex, DWORD dwRoomIndex);	//TEST 유영재

	//개인 본부
	void SendPersonalHQInviteList( User *pUser, int iCurPage, int iMaxCount, int iRoomLevel );
	void OnResultPersonalHQConstruct(CQueryResultData *query_data);
	void OnResultPersonalHQRetrieve(CQueryResultData *query_data);
	void OnResultPersonalHQIsExist(CQueryResultData *query_data);
	void OnResultLoginSelectPersonalHQInvenData(CQueryResultData *query_data);
	void OnResultPersonalHQBlocksInfo(CQueryResultData *query_data);
	void OnResultPersonalHQBlockAdd(CQueryResultData *query_data);

	//기간제 캐쉬상자
	void OnResultSelectTimeCashTable(CQueryResultData *query_data);
	void OnResultInsertTimeCashTable(CQueryResultData *query_data);
	void OnResultUpdateTimeCashTable(CQueryResultData *query_data);

	//칭호
	void OnResultUpdateStatus(CQueryResultData *query_data);
	void OnResultInsertOrUpdate(CQueryResultData *query_data);
	void OnResultSelectTitle(CQueryResultData *query_data);

	// 오크통
	// 로그인 시 받아오는 오크통 데이터
	void OnResultSelectOakBarrel(CQueryResultData *query_data);
	// 오크통 데이터 업데이트
	void OnResultUpdateOakBarrel(CQueryResultData *query_data);
	// 오크통 칼 일일 사용 한도 수량 초기화 시각 업데이트
	void OnResultTimeUpdateOakBarrel(CQueryResultData *query_data);
	// 오크통 칼 일일 사용 한도 수량 초기화 업데이트
	void OnResultLimitSwordUpdateOakBarrel(CQueryResultData *query_data);

	void OnResultSelectCardMatching(CQueryResultData *query_data);	// 2018-06-18 by bckim, 카드 짝 마추기 이벤트 추가		// 카드 매칭 // 로그인 시 받아오는 카드 매칭 데이터		
	void OnResultSelectDiceGame(CQueryResultData *query_data);		// 2018-08-30 by bckim, 주사위 이벤트 추가

	void OnResultSelectUserMatch(CQueryResultData *query_data);
	void OnResultUpdateUserMatch(CQueryResultData *query_data);
	void OnResultSelectUserMatchBattle(CQueryResultData *query_data);

	void OnResultSelectUserSpiritData(CQueryResultData *query_data);
	void OnResultInsertUserSpiritData(CQueryResultData *query_data);
	void OnResultUpdateUserSpiritData(CQueryResultData *query_data);

	// 커스텀 메달
	void OnResultSelectUserCustomMedalData(CQueryResultData *query_data);
	void OnResultInsertUserCustomMedalData(CQueryResultData *query_data);
	void OnResultUpdateUserCustomMedalStatus(CQueryResultData *query_data);

	// 타임게이트
	void OnResultSelectUserTimeGateData(CQueryResultData *query_data);
	void OnResultUpdateUserTimeGateData(CQueryResultData *query_data);

	// 
	void OnResultSelectPracticeList(CQueryResultData *query_data);
	void OnResultUpdatePracticeAdmission(CQueryResultData *query_data);
	void OnResultUpdatePracticeTime(CQueryResultData *query_data);
	void OnResultInitPracticeData(CQueryResultData *query_data);
	void OnResultPracticeRankList(CQueryResultData *query_data);
	void AllUserPracticeInitData();

	void OnResultPCBangPlayTime(CQueryResultData *query_data);

	//HRYOON0825 게임방 입장로그
	//데이터 초기화
	void SetResetPrisonerRoom() { m_iQuickStartPrisoner = 0; m_iMakePrisonerRoom = 0; m_iListSelectPrisoner = 0; };

	//누적- 방 타입별로 누적
	//포탈로 방생성
	void SetMakePrisonerRoomCount() { m_iMakePrisonerRoom++; };	

	//포탈 빠른시작
	void SetQuickStartPrisonerRoomCount()  { m_iQuickStartPrisoner++; };	

	//방리스트 포탈 선택
	void SetChoicePrisonerRoomCount()  { m_iListSelectPrisoner++; };	

	//데이터 가져오기
	//포탈로 방생성
	int GetMakePrisonerRoomCount() { return m_iMakePrisonerRoom; };	

	//포탈 빠른시작
	int GetQuickStartPrisonerRoomCount()  { return m_iQuickStartPrisoner; };	

	//방리스트 포탈 선택
	int GetChoicePrisonerRoomCount()  { return m_iListSelectPrisoner; };	

	//방입장, 자동, 누적 변수 생성

	// 알림
	// 서버사이드 알람 테스트
	void TestAlarmToAllUsers(User * pUser);
	// 장비의 25 레벨 강화 관련 이벤트를 전서버에 알린다.
	void AlarmEventInReinforceOfEtcItemToAllUsers(User * pUser, int iMentType, int iItemCode, int iResultReinforce);
	// 타임 게이트 관련 전서버 알림
	void AlarmEventOfTimeGateToAllUsers(User * pUser, bool bAllAlarm, int iListType, int iPresentType, int iValue1, int iValue2);
	// 선물관련 전서버 알림
	void AlarmEventOfPresentToAllUsers(User * pUser, int iPresentMent, int iPresentState, int iPresentType, int iPresentValue1, DWORD dwLimitDate);
	// 낚시 관련 전서버 알림
	void AlarmEventOfFishingToAllUsers(User * pUser, bool bAllAlarm, int iItemType, 
									bool bEventFishing, int iEventFishingPresentType, int iEventFishingPresentValue1, int iEventFishingPresentValue2,
									bool bSpecial, int iPresentType, int iPresentValue1, int iPresentValue2);
	// extra item 구매 관련 전서버 알림
	void AlarmEventInBuyExtraItemBuyToAllUsers(User * pUser, int iResult, int iTradeType, bool bAlarm, int iItemCode, int iReinforce, int iMachineCode, int iPeriodTime);
	// super gashapon(etc item) 사용 관련 전서버 알림
	void AlarmEventInSuperGashaponPresentToAllUsers(User * pUser, bool bAlarm, int iProductType, DWORD dwEtcItemCode, DWORD dwPresentIndex);
#ifdef OHTG_NEW_RANROM_BOX_20201109
	// new super gashapon(etc item) 사용 관련 전서버 알림
	void AlarmEventInNewSuperGashaponPresentToAllUsers(User * pUser, bool bAlarm, int iProductType, DWORD dwEtcItemCode, DWORD dwCategoryIndex, DWORD dwPackageIndex);
#endif //OHTG_NEW_RANROM_BOX_20201109
	// etc item, accessary recharge process 관련 전서버 알림
	void AlarmEventOfItemRechargeProcessToAllUsers(User * pUser, int iPeriodType, DWORD dwUseEtcItemIndex, int iItemCode);
	// 유저의 특정 레벨업을 전서버 알림
	void AlarmEventInGradeUpToAllUsers(User * pUser);
	// 골드 이벤트 관련 전서버 알림
	void AlarmEventInGoldItemEventToAllUsers(User * pUser, bool bAlarm, int iPresentType, int iPresentValue1, int iPresentValue2);
	// fishinf sell 관련 전서버 알림
	void AlarmEventInFishingSellToAllUsers(User * pUser, bool bAlarm, int iItemType, int iResultPeso);
	// catalyst growth success 관련 전서버 알림
	void AlarmEventInGrowthCatalystSuccessToAllUsers(User * pUser, int iPeriodType, int iItemCode);
	// random deco present에 관한 전서버 알림
	void AlarmEventInRandomDecoPresentToAllUsers(User * pUser, bool bAlarm, DWORD dwItemType, int iPresentType, int iPresentValue1, int iPresentValue2);
	// award lucky present에 관한 전서버 알림
	void AlarmEventInAwardLuckyPresentToAllUsers(User * pUser, bool bAlarm, int iPresentType, int iPresentValue1, int iPresentValue2);
	// 발굴(excavation)에 관한 전서버 알림
	void AlarmEventInExcavationToAllUsers(User * pUser, bool bAlarm, int iArtifactType, int iArtifactValue1, int iArtifactValue2, float fMapRate);
	// 2018-01-15 by bckim, 탐사 확장
	void AlarmEventInExcavation_EX_ToAllUsers(User * pUser, BOOL bRoomAlarm, BOOL bWholeAlarm, int iRewardType, int iIndex, int iType, int iGrade, int iPrice, int iMultiple);

	// gashapon present에 관한 전서버 알림
	void AlarmEventInGashaponPresentToAllUsers(User * pUser, bool bAlarm, DWORD dwEtcItemType, int iPresentType, int iPresentValue1, int iPresentValue2);

private:     	/* Singleton Class */
	UserNodeManager();
	virtual ~UserNodeManager();
};

#define g_UserNodeManager UserNodeManager::GetInstance()
