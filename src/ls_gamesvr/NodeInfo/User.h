// User.h: interface for the User class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USER_H__03E2797C_74A5_433B_B34D_2E60C0784CEF__INCLUDED_)
#define AFX_USER_H__03E2797C_74A5_433B_B34D_2E60C0784CEF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UserParent.h"
#include "RoomParent.h"
#include "Item.h"
#include "ioEquipSlot.h"
#include "NodeHelpStructDefine.h"

#include "ioCharacter.h"
#include "ioInventory.h"
#include "ioAward.h"
#include "ioClassExpert.h"
#include "ioFriend.h"
#include "ioUserGuild.h"
#include "ioUserRecord.h"
#include "ioUserGrowthLevel.h"
#include "ChannelParent.h"
#include "../HackCheck.h"
#include "ioEventUserManager.h"
#include "ioUserEtcItem.h"
#include "ioUserPresent.h"
#include "ioUserSubscription.h"
#include "ioUserFishingItem.h"
#include "ioQuest.h"
#include "ioUserExtraItem.h"
#include "ioUserMedalItem.h"
#include "ioUserExpandMedalSlot.h"
#include "ioCharRentalData.h"
#include "ioUserSelectShutDown.h"
#include "ioAlchemicInventory.h"
#include "ioUserTournament.h"
#include "ioClover.h"
#include "ioBingo.h"
#include "ioExerciseCharIndexManager.h"
#include "ioUserPcRoom.h"
#include "ioUserAttendance.h"
#include "ioEtcItemHelp.h"
#include "ioUserPet.h"
#include "ioTimeStamp.h"
#include "ioUserCostume.h"
#include "ioMission.h"
#include "ioUserRollBook.h"
#include "PersonalHQInven.h"
#include "UserTimeCashTable.h"
#include "UserTitleInven.h"
#include "ioPirateRoulette.h"
#include "ioUserAccessory.h"
#include "UserBonusCash.h"
#include "ioUserCoin.h"
#include "ioOakBarrel.h"
#include "ioUserMatch.h"
#include "ioUserSpirit.h"
#include "ioCustomMedal.h"
#include "CustomMedal.h"
#include "ioUserPractice.h"
#include "ioUserExcavation.h"					// 2018-01-15 by bckim, 탐사 확장			
#include "ioBlockPropertyManager.h"				// 2018-01-15 by bckim, 탐사 확장			 // 순서 상 먼저 #pragma once
#include "FlagMode.h"			// 2018-03-15 by bckim, 깃발모드 추가
#include "CardMatching.h"		// 2018-06-18 by bckim, 카드 짝 마추기 이벤트 추가
#include "ArenaModeManager.h"	// 2018-07-25 by bckim, 아레나 모드 추가
#include "DiceGame.h"			// 2018-08-30 by bckim, 주사위 이벤트 추가  


#ifdef XTRAP
#include "../Xtrap/ioXtrap.h"
#endif

#ifdef NPROTECT
#ifdef NPROTECT_CSAUTH3
	#include "../nProtect/ggsrv30.h"
	#ifdef _DEBUG
		#pragma comment( lib, "ggsrv30lib_x86_MD.lib" )
	#else
		#pragma comment( lib, "ggsrv30lib_x86_MT.lib" )
	#endif
#endif
#endif // NPROTECT

#ifdef HACKSHIELD
#include "../HackShield/ioHackShield.h"
#endif

class Room;
class BattleRoomParent;
class LadderTeamParent;
class ShuffleRoomParent;
class CConnectNode;
class ServerNode;
class User;
class ioEtcItem;
class ioSetItemInfo;

struct UserSyncTable
{
	int  m_iGradeLevel;
	int  m_iUserPos;
	int  m_iKillDeathLevel;
	int  m_iLadderPoint;
	bool m_bSafetyLevel;

	UserSyncTable()
	{
		Initialize();
	}

	void Initialize()
	{
		m_iGradeLevel		= -1;
		m_iUserPos			= -1;
		m_iKillDeathLevel   = -1;
		m_iLadderPoint      = 0;
		m_bSafetyLevel      = false;
	}
};

// 룸에서 누굴 만나는냐에 따라서 실시간으로 변경되며 룸 입장할 때 초기화한다.
struct UserBonusTable
{
	bool m_bPCRoomBonus;
	bool m_bGuildBonus;
	bool m_bFriendBonus;
	UserBonusTable()
	{
		Init();
	}

	void Init()
	{
		m_bPCRoomBonus = m_bGuildBonus = m_bFriendBonus = false;
	}
};

// 친구 추천 이벤트용 데이터
struct FriendRecommendEvent
{
	DWORD m_dwTableIndex;
	DWORD m_dwRecommendIndex;

	FriendRecommendEvent()
	{
		Init();
	}

	void Init()
	{
		m_dwTableIndex = 0;
		m_dwRecommendIndex = 0;
	}
};

///////////////////////////////////////////////////////////////////////////////

class User : public CConnectNode,
			 public UserParent
{
	friend class UserNodeManager;

public:
	enum ChatType
	{
		CT_NONE = -1,
		CT_TEAM,
		CT_ALL,
		CT_PRIVATE,
		CT_ANNOUNCE,
		CT_PARTY,
		CT_LADDER,
		CT_SERVER_LOBBY,
		CT_WHOLE_SERVER,
	};

	// 유저노드를 여러개 두지 않고 아래의 스테이트로 유저노드의 삭제를 처리한다. 즉 크리티컬 세션이 필요없어진다.
	enum SessionState
	{
		SS_DISCONNECT	= 0,
		SS_CONNECT		= 1,
		SS_SERVERMOVING = 2,
	};

private:
	int		m_entity;
	SessionState m_eSessionState;
	bool m_bSessionDestroySave;

private:
	USERDATA m_user_data;                   //유저의 모든 정보.
	UserRelativeGradeData m_user_relative_grade_data;
	DWORD    m_dwDBAgentID;                 //사용할 DBAgent ID
	DWORD    m_save_time;                   //저장 시간.
	DWORD    m_dwSaveCheckTime;             //저장 시간 랜덤 + 0 ~ 1800000

private:
	DWORD	 m_sync_time;                   //동기화 시간
	DWORD    m_dwSyncCheckTime;             //유령소켓 해제 시간 120000 ~ 300000.
	DWORD    m_dwConnectTime;               //접속 시간.

	int		 m_ping_total_send_index;		//지금까지 보내온 핑의 인덱스
	int      m_ping_less_error_count;		//동기화 시간보다 빠른 에러
	int		 m_ping_over_error_count;		//동기화 시간보다 늦은 에러
	int		 m_total_ping_error_count;		//총 동기화 에러
	DWORD	 m_prev_over_ping_time;			//이전메세지가 늦게 온경우의 시간차
	bool	 m_first_heart_beat;			//첫번째 Ping 동기화
	DWORD    m_dwPingStep;                  //Ping 단계. 0(좋음) ~~~ N(나쁨)
	bool     m_ladder_point_change;         //래더포인트가 변경되었다.
	bool     m_hero_expert_change;          //영웅전 경험치가 변경되었다.
	bool     m_first_login_user;            //첫 접속 유저

	int		 m_iUserIPType;					//유저 IP타입 ( 일반, 관리자, 클라우드 )

private:
	HackCheck::CheckProblem m_SpeedHackQuiz;
	DWORD	m_dwSpeedHackQuizLimitTime;
	int     m_iCurSpeedHackAnswerChance;

private:
	HackCheck::CheckProblem m_AbuseQuiz;
	DWORD	m_dwAbuseQuizLimitTime;
	int     m_iCurAbuseAnswerChance;

private:
	HackCheck::CheckProblem m_MacroQuiz;
	DWORD	m_dwMacroQuizLimitTime;
	int     m_iCurMacroAnswerChance;

protected://relay 관련 
	int     m_iRelayServerID;
	int     m_iDropKing;
	int     m_iDropFlag;		// 2018-03-15 by bckim, 깃발모드 추가

public://get/set//relay
	int DropKing() const { return m_iDropKing; }
	void DropKing(int val) { m_iDropKing = val; }

	int DropFlag() const { return m_iDropFlag; }		// 2018-03-15 by bckim, 깃발모드 추가
	void DropFlag(int val) { m_iDropFlag = val; }		// 2018-03-15 by bckim, 깃발모드 추가

	int RelayServerID() const { return m_iRelayServerID; }
	void SetRelayServerID(int val) { m_iRelayServerID = val; }
	bool IsRelayUse() { if(m_iRelayServerID != 0)
							return true ;
						else
							return false;
					  }
	void LeaveProcess();
private:
	bool    m_bDeveloper;

private:
	// 룸
	Room	*m_pMyRoom;                 
	int      m_iLeaveRoomIndex;                 // 이탈한 룸 인덱스
	DWORD    m_dwLeaveRoomTime;                 // 룸 이탈 시간
	DWORD    m_dwMemoryLeaveRoomTime;           // 이탈 룸 인덱스 기억 시간.
	bool     m_bJoinServerLobby;                // 서버 로비에 있는 유저.
	DWORD    m_dwMyHeadquartersIndex;           // 내 본부 룸 인덱스

	UserBonusTable m_BonusTable;                // 룸에서 만나는 유저와 보너스 관계 발생
	UserBonusTable m_BonusTableBackup;

	// 전투 룸
	DWORD    m_dwMyBattleRoom;
	int      m_iSearchBatleRoomIndex;           // -1이면 자동
	int      m_iLeaveBattleRoomIndex;           // 이탈한 전투룸 인덱스
	DWORD    m_dwLeaveBattleRoomTime;           // 전투룸 이탈 시간
	DWORD    m_dwMemoryLeaveBattleRoomTime;     // 이탈 전투룸 인덱스 기억 시간.
	DWORD	m_dwLadderMatchTime;				// 래더전 매칭 시간


	// 셔플룸
	DWORD    m_dwMyShuffleRoom;

	// 길드팀
	DWORD    m_dwMyLadderTeam;
	int m_iMyVictories;

	// 연속 모드
	int      m_iModeConsecutivelyCnt;
	
	static int m_iLimiteMaxCharSlot;
	static int m_iDefaultMaxCharSlot;
	static int m_RefCount;
	int m_iCurMaxCharSlot;

	int	 m_select_char;						//현재 선택된 캐릭터.
	typedef std::vector< ioCharacter * > vCharList;
	vCharList m_CharList;	

	// 2018-07-25 by bckim, 아레나 모드 추가
	//int	 m_select_ArenaModeChar;						//현재 선택된 캐릭터.
	vCharList m_ArenaModeCharList;	
	// End. 2018-07-25 by bckim, 아레나 모드 추가


	ioInventory				m_Inventory;
	ioUserEtcItem			m_UserEtcItem;
	ioClassExpert			m_ClassExpert;
	ioAward					m_Award;
	ioFriend				m_Friend;
	ioQuest					m_Quest;
	ioUserRecord			m_UserRecord;
	ioUserGuild				m_UserGuild;
	ioUserPresent			m_UserPresent;
	ioUserSubscription		m_UserSubscription;
	ioUserGrowthLevel		m_UserGrowthLevel;
	ioUserFishingItem		m_UserFishingItem;
	ioUserExtraItem			m_UserExtraItem;
	ioUserMedalItem			m_UserMedalItem;
	ioUserExpandMedalSlot	m_UserExpandMedalSlot;
	UserHeroData			m_UserHeroData;
	ioCharRentalData		m_CharRentalData;
	UserHeadquartersOption	m_UserHeadquartersData;
	ioUserSelectShutDown	m_UserSelectShutDown;
	ioAlchemicInventory		m_AlchemicInventory;
	ioUserTournament		m_UserTournament;
	ioClover				m_Clover;
	ioBingo					m_Bingo;
	ioUserPcRoom			m_UserPcRoom;
	ioUserAttendance		m_UserAttendance;
	ioUserPet				m_UserPetItem;	
	ioTimeStamp				m_Timestamp;
	ioUserCostume			m_UserCostume;
	ioMission				m_UserMission;
	ioUserRollBook			m_UserRollBook;
	PersonalHQInven			m_PersonalHQInven;
	UserTimeCashTable		m_UserTimeCashTabel;
	UserTitleInven			m_TitleInven;
	ioPirateRoulette		m_PirateRoulette;
	ioUserCoin				m_UserCoin;
	ioUserAccessory			m_UserAccessory;
	ioOakBarrel				m_OakBarrel;	// 오크통 개선
	ioUserMatch				m_UserMatch;
	ioUserSpirit			m_UserSpirit;
	ioCustomMedal			m_UserCustomMedal;
	ioUserPractice			m_UserPractice;

	ioUserExcavation		m_UserExcavation;		// 2018-01-15 by bckim, 탐사 확장		
	CardMatching			m_CardMatching;			// 2018-06-18 by bckim, 카드 짝 마추기 이벤트 추가
	DiceGame				m_DiceGame;				// 2018-08-30 by bckim, 주사위 이벤트 추가
public:
	ioUserExcavation*		GetUserExcavation()		{ return &m_UserExcavation; }	// 2018-01-15 by bckim, 탐사 확장
	CardMatching*			GetUserCardMatching()	{ return &m_CardMatching; }		// 2018-06-18 by bckim, 카드 짝 마추기 이벤트 추가
	DiceGame*				GetUserDiceGame()		{ return &m_DiceGame; }			// 2018-08-30 by bckim, 주사위 이벤트 추가
	
private:
	DWORDVec        m_vChannelNode;	
	UserSyncTable   m_SyncTable;

	DWORD           m_dwMyIP;                        //4바이트로 변경된 자기 아이피
 
	int m_PreRoomNum;
	DWORD    m_dwStartTimeLog;

	//
	DWORD m_dwFishingStartTime;
	DWORD m_dwFishingLoopTime;

	int m_iFishingRodType;
	int m_iFishingBaitType;
	DWORD m_dwFisheryCode;
	//

private:
	TeamType m_Team;                        //전투, 훈련의 팀 타입
	TeamType m_ShamBattleTeam;              //파티 연습하기의 팀 타입.

private:                                    //나가기 처리
	bool	 m_bExitRoomReserved;
	DWORD    m_dwExitRoomTime;
	DWORD    m_dwCurExitRoomTime;	
	bool     m_bExitLobby;                  //로비로

private:
	bool     m_bMovieCapturing;
	DWORD    m_dwPCRoomNumber;

protected:
	bool m_bStealth;
	
	bool m_bNeedSendPushStruct;				// 새로들어와서 구조물 정보 보내줘야 하는지 여부
	int m_iNeedSendPushStructMaxIndex;		// 보내줘야하는 구조물들 마지막 index
	int m_iSendPushStructIndex;				// 마지막으로 보낸 구조물 Index
	DWORD m_dwSendPushStructCheckTime;		// 마지막으로 보낸 시간.

protected:									// 네트워크 상태따라 왕관드롭 체크 관련
	int m_iCurCheckKingIndex;
	int m_iCurRecvKingPingCnt;

// 2018-03-15 by bckim, 깃발모드 추가	
protected:									
	int m_iCurCheckFlagIndex;
	int m_iCurRecvFlagPingCnt;
// End. 2018-03-15 by bckim, 깃발모드 추가

	// 2019-02-14 by bckim, 배틀 모드 추가
public:
	int	m_iBattleMode_Order;		//m_eSelect_Order = BATTLE_ORDER_RANDOM;	
	// End. 2019-02-14 by bckim, 배틀 모드 추가

public:
	UserBonusCash		   m_UserBonusCash;

private:
	ioHashString *m_pEncLoginKey;

public:
	static bool m_bUseSecurity;
	static int  m_iSecurityOneSecRecv;

protected:      //서버 이동 정보
	int  m_iServerMovingValue;
	bool m_bReserveServerMoving;            //서버 이동 예약
	bool m_bReserveBattleRoom;              //전투방 예약 입장
	bool m_bReserveLadderTeam;              //길드팀 예약 입장

	bool m_bShuffleGlobalSearch;
	bool m_bReserveShuffleRoom;

protected:      //길드 마크 변경 웹페이지 암호
	DWORD m_dwGuildMarkChangeKeyValue;

#ifdef XTRAP
	BYTE  m_XtrapSessionBuf[ioXtrap::MAX_SESSION_BUF];
	DWORD m_dwXtrapCheckTime;
#endif

#ifdef NPROTECT
#ifdef NPROTECT_CSAUTH3
	CCSAuth3 m_NProtectAuth;
#else
	CCSAuth2 *m_pNProtectAuth;
#endif
	DWORD     m_dwNProtectCheckTime;
	int       m_iSentNProtectCheckCnt;
#endif // NPROTECT

#ifdef HACKSHIELD
	AHNHS_CLIENT_HANDLE m_hHackShield;
	DWORD               m_dwHackShieldCheckTime;
#endif
	bool  m_bFirstChangeID;

	DWORD m_dwExcavatingTime;
	DWORD m_dwTryExcavatedTime;

	bool  m_bSendCheckPCRoom;
	DWORD m_dwOldServerIndex;
	bool m_bSendLogOutUserState;

	float m_fExpBonusEventValue;

public://get set 
	DWORD GetOldServerIndex() const { return m_dwOldServerIndex; } // 
	bool GetSendLogOutUserState() const { return m_bSendLogOutUserState; }
	void SetSendLogOutUserState(bool val) { m_bSendLogOutUserState = val; }

	void SetExpBonusEventValue(float fExpBonusValue) { m_fExpBonusEventValue = fExpBonusValue; }
	float GetExpBonusEventValue() { return m_fExpBonusEventValue; }
	float GetExpBonusEvent();

protected:
	bool  m_bShutDownUser;

protected:  // 친구 추천 이벤트용
	FriendRecommendEvent m_FriendRecommendData;
	
	CTime m_cLoginTime;

protected:
	//enum { MAX_SELECT_EXTRA_ITEM_CODE = 10, };
	int  m_iSelectExtraItemCodes[MAX_SELECT_EXTRA_ITEM_CODE];
	void ClearSelectExtraItemCodes();
public://릴레이 관련 
	bool TimeOutClose(int checkTime);
	void SendUserLogOut();
public:
	void GetSelectExtraItemCodes( IntVec &rvInt );

public:
	int	 GetEntity()	{ return m_entity; }

	void InitServerMovingValue();
	int  GetServerMovingValue();
	int  SetServerMoving();
	bool IsServerMoving();	

	void ReserveServerMoving();
	bool IsReserveServerMoving();

	void SetLadderMatchTime( DWORD dwMatchTime ) { m_dwLadderMatchTime = dwMatchTime; }
	DWORD GetLadderMatchTime() { return m_dwLadderMatchTime; }

	// 2019-02-14 by bckim, 배틀 모드 추가
	void SetBattleModeOrder( int iBattleModeOrder ) { m_iBattleMode_Order = iBattleModeOrder; }
	int GetBattleModeOrder( ) { return m_iBattleMode_Order; } 
	// 2019-02-14 by bckim, 배틀 모드 추가

	inline void ReserveBattleRoom( bool bReserve ) { m_bReserveBattleRoom = bReserve; }
	inline bool IsReserveBattleRoom() { return m_bReserveBattleRoom; }

	inline void ReserveLadderTeam( bool bReserve ) { m_bReserveLadderTeam = bReserve; }
	inline bool IsReserveLadderTeam() { return m_bReserveLadderTeam; }
	
	inline void SetShuffleGlboalSearch( bool bReserve ) { m_bShuffleGlobalSearch = bReserve; }
	inline bool IsShuffleGlboalSearch() { return m_bShuffleGlobalSearch; }

private:
	int  m_iCreateCharCount;                  //용병 DB Insert 카운터

private:
	struct LevelUPData
	{
		int m_iType;
		int m_iLevel;
		LevelUPData()
		{
			m_iType = m_iLevel = 0;
		}
	};
	typedef std::vector< LevelUPData > vLevelUPData;
	vLevelUPData m_vLevelUpClass; 
	DWORDVec m_vLevelUpAndPresentCreateClass; // 레벨업이나 선물로 받은 용병이 DB에 Insert되기 까지 기록
	ioHashString m_szGUID;			          // 접속시 마다 새롭게 부여되는 유일한 id, DB와 통신할때 user을 식별한다. 

	ioHashString m_szPacketGUID;			  // 패킷시 암호화필요할때 사용되는 GUID

	EventUserManager m_EventUserMgr;
	ioHashString m_szBillingGUID; // 구매,잔액조회건마다 새롭게 갱신되는 ID

	ioHashString m_szNewPublicID;
	ioHashString m_szPublicIPForNewPublicID;

	ioHashString m_szBillingUserKey;

	ControlKeys  m_kControlKeys;

protected:	
	SelectGashaponValueVec  m_SelectGashaponItem;

public:
	void ConnectProcessSelectChar();
	void ConnectProcessComplete();
	bool IsConnectProcessComplete(){ return m_user_data.m_bSavePossible; }
	
public:
	int  GetCreateCharCount(){ return m_iCreateCharCount; }
	void SetCreateCharCount( int iCreateCharCount ){ m_iCreateCharCount = iCreateCharCount; }
	bool IsCharCreating(){ return ( GetCreateCharCount() > 0 ); }

	bool IsCharIndex( DWORD dwCharIndex );

public:
	static void LoadHackCheckValue();
	static void LoadCharSlotValue();
	static void	LoadNagleReferenceValue( int refCount = 0 );

public:
	ioHashString * GetEncLoginKey() const;
	void ClearEncLoginKey();

protected:
	void InitData();

public:
	void InitCharList();

	void SaveData();
	void SaveUserData();            //유저 정보 저장
	void SaveUserLadderData();      //유저 래더 포인트
	void SaveUserHeroExpert();      //유저 영웅전 경험치
	void SaveClassExpert();         //클래스 숙련 정보 저장
	void SaveCharacter();           //캐릭터 저장
	void SaveInventory();           //인벤토리 저장
	void SaveEtcItem();             //특별아이템 저장
	void SaveAward();               //시상내역 저장
	void SaveQuest();               //퀘스트 저장
	void SaveRecord();              //레코드 저장
	void SaveGrowth();				//성장 저장
	void SaveFishItem();			//낚시아이템 저장
	void SaveExtraItem();			//추가장비 저장
	void SaveControlKeys();			//조작키 저장
	void SaveMedalItem();			//메달 아이템 저장
	void SaveExpandMedalSlot();		//메달확장슬롯 저장
	void SaveBestFriend();          //절친 저장
	void SaveUserHeadquarters();    //본부 저장
	void SaveAlchemicInventory();	//조합 저장
	void SaveClover();				// 클로버 저장.
	void SaveFriendClover();		// 친구목록 클로버 저장.
	void SaveBingo();				// 빙고 저장.
	void SavePet();					//펫 저장
	void SaveTimeMission();			// 시간 미션 저장.
	void SavePirateRoulette();		// 해적룰렛 저장.
	void SaveOakBarrel();			// 오크통 저장
	void SaveUserMatch();			// 랭킹전 저장
	void SaveUserSpirit() { m_UserSpirit.SaveData(); }	// 정기 저장

public:
	virtual void SessionClose( BOOL safely=TRUE );
	virtual bool SendMessage( CPacket &rkPacket );
	virtual void ReceivePacket( CPacket &packet );
	virtual void PacketParsing(CPacket &packet);
	virtual void SetClientAddr(sockaddr_in client_addr);
	void SetClientAddr( const char* back_iip, const int back_port );
	void SetClientAddressForRelay(char* szIpaddr,int iPort);


	bool RoomBroadCast( SP2Packet &rkPacket );

public:
	virtual void OnCreate();       //초기화
	virtual void OnDestroy();
	void OnSessionDestroy();       //정보 저장 & 초기화
	void OnNameChageSync();        //아이디 변경 후 처리
	virtual bool CheckNS( CPacket &rkPacket );
	virtual int  GetConnectType();
	
public:
	bool IsSessionDestroySave(){ return m_bSessionDestroySave; }
	void SetSessionState( User::SessionState eState ){ m_eSessionState = eState; }
	bool IsDisconnectState(){ return ( m_eSessionState == SS_DISCONNECT ); }
	bool IsConnectState(){ return ( m_eSessionState == SS_CONNECT ); }

public:
	const int GetUserEventType() const { return m_user_data.m_user_event_type; }

public:

#ifdef ANTIHACK
	//rudp
	DWORD m_dwUserSeed;
	DWORD m_dwNPCSeed;
	void	UpdateSeed( DWORD dwRand );
	DWORD	GetUserSeed(){ return m_dwUserSeed; }
	DWORD	GetNPCSeed(){ return m_dwNPCSeed; }

	void UpdateSpPotion();
	void SetDieState();
#endif
	void EnterRoom(Room *pRoom);

	
	void LeaveRoom( BYTE eType = (BYTE)RoomParent::RLT_NORMAL );
	void NetworkCloseLeaveRooom();
	void NetworkCloseLogout();
	void SetLeaveRoomTime( int iIndex );
	void EnterRoomSelectChar();
	void CheckLeaveRoomIndex();

	void InitBonusTable();
	void SetBonusTable( UserBonusTable &kBonusTable );
	bool ChangeBonusTable();
	void BackupBonusTable();
	void CheckRoomBonusTable();
	inline const UserBonusTable &GetBonusTable(){ return m_BonusTable; }

	void ExitRoomToTraining( int iResult, bool bPenalty );
	void PrivatelyLeaveRoomToTraining();
	bool IsPartyPossible();	
			
public:
	void LeaveServer( const int iServerIndex );

	void EnterBattleRoom( BattleRoomParent *pBattleRoom, bool bObserver );
	bool EnterBattleRoom( DWORD dwBattleRoom, ServerNode *pBattleRoomServer );
	void LeaveBattleRoom( bool bJoinFailed = false );
	void LeaveBattleRoomException( DWORD dwNodeIndex );
	bool IsBattleRoom();
	void ExitRoomToBattleRoomJoin( BattleRoomParent *pBattleRoom, bool bObserver, bool bMovePesoPenalty, int iPenaltyPeso );
	void SetLeaveBattleRoomTime( int iIndex );
	void CheckLeaveBattleRoomIndex();

public:
	void EnterShuffleRoom( ShuffleRoomParent *pShuffleRoomNode, bool bReJoinCheck = true );
	void LeaveShuffleRoom( bool bJoinFailed = false );
	void LeaveShuffleRoomException( DWORD dwNodeIndex );
	bool IsShuffleRoom();

public:
	void EnterLadderTeam( LadderTeamParent *pLadderTeam );
	bool EnterLadderTeam( DWORD dwLadderTeam, ServerNode *pLadderTeamServer );
	void LeaveLadderTeam();
	void LeaveLadderTeamException( DWORD dwNodeIndex );
	bool IsLadderTeam();
	void ExitRoomToLadderTeamJoin( LadderTeamParent *pLadderTeam );
	void LeaveGuildToLeaveGuildPlaza(DWORD dwGuildIndex);
	void LeaveGuildToQuestClear();

public:
	void EnterChannel( ChannelParent *pNode );
	void LeaveChannel( ChannelParent *pNode = NULL );
	void LeaveChannelException( DWORD dwNodeIndex );
	ChannelParent *FindChannel( DWORD dwIndex );

public:
	void EquipDBItemToChar( int iCharArray );
	void EquipDBItemToAllChar();
	void EquipGangsiItemToAllChar();
	void EquipDBItemToLiveChar();
	void AllCharReEquipDBItem( int iSlotIndex );
	void ReleaseEquipAllChar();
	void ReleaseAllItemSelectChar();
	void ClearRealEquipItemOwner( const ioHashString &rkOutUser );
	ioItem* ReleaseItem( int iSlot );
	bool IsSlotEquiped( int iSlot ) const;
	bool IsEquipedItem();
	const ioItem* GetItem( int iSlot ) const;
	const int GetEquipItemCount();

	ioCharacter* GetCharacter( int iArray );
	ioCharacter* GetLeaderCharacter();
	void CreateSelectCharData( ioCharacter *pChar );
	void StartCharLimitDate( DWORD dwCheckSecond, const ioHashString &szCallFunName, int iLine );
	void StartCharLimitDate( ioCharacter *pChar );
	bool UpdateCharLimitDate();

	// Ex Info
	ioInventory*		GetInventory();
	ioUserEtcItem*		GetUserEtcItem();
	ioClassExpert*		GetClassExpert();
	ioAward*			GetAward(); 
	ioQuest*			GetQuest();
	ioUserRecord*		GetUserRecord();
	ioUserGuild*		GetUserGuild();
	ioUserPresent*		GetUserPresent();
	ioFriend*			GetFriend();
	ioClover*			GetClover();
	ioBingo*			GetBingo();
	ioPirateRoulette*	GetPirateRoulette();
	ioOakBarrel*		GetOakBarrel();		// 오크통 개선
	ioUserMatch*		GetUserMatch()	{ return &m_UserMatch; }
	ioUserSpirit*		GetUserSpirit()	{ return &m_UserSpirit; }
	ioUserPractice*		GetUserPractice()		{ return &m_UserPractice;	}

	ioUserSubscription*    GetUserSubscription();	
	ioUserGrowthLevel*     GetUserGrowthLevel();
	ioUserFishingItem*     GetUserFishingItem();
	ioUserExtraItem*       GetUserExtraItem();
	ioUserMedalItem*       GetUserMedalItem();
	ioUserExpandMedalSlot* GetUserExpandMedalSlot();
	ioCharRentalData*      GetCharRentalData();
	ioAlchemicInventory*   GetAlchemicInventory();
	ioUserTournament*      GetUserTournament();
	ioUserAttendance*      GetUserAttendance();
	ioUserPet*			   GetUserPetItem();
	ioUserCostume*		   GetUserCostume() { return &m_UserCostume; }
	ioUserAccessory*	   GetUserAccessory() { return &m_UserAccessory; }
	ioMission*			   GetUserMission()	{ return &m_UserMission; }
	ioUserRollBook*		   GetUserRollBook() { return &m_UserRollBook; }
	PersonalHQInven*	   GetUserPersonalHQInven() { return &m_PersonalHQInven; }
	UserTimeCashTable*	   GetUserTimeCashTable() { return &m_UserTimeCashTabel; }
	UserTitleInven*		   GetUserTitleInfo() { return &m_TitleInven; }
	UserBonusCash*		   GetUserBonusCash() { return &m_UserBonusCash; }		//?
	ioCustomMedal*		   GetUserCustomMedal() { return &m_UserCustomMedal; }
	//
	void GetEquipMedalClassTypeArray( int iClassType, IntVec &kMedal );
	void GetEquipCustomMedalClassTypeArray( int iClassType, IntOfStatVec &kCustomMedal );
	void GetEquipCustomMedalClassStat( int iItemIndex, int iItemCode, IntOfStatVec &kStat );

	bool  GetDisplayCharacter( DWORD dwCharIndex, float &rkXPos, float &rkZPos );
	DWORD GetDisplayCharMotion( DWORD dwCharIndex );
	bool IsHeadquartersLock();

public: // 친구

	void User::InsertFriend( int iIndex, DWORD dwUserIndex, const ioHashString &szName, int iGradeLevel, int iCampPos, DWORD dwRegTime
						, int iSendCount, DWORD dwSendDate, int iReceiveCount, DWORD dwReceiveDate, int iBeforeReceiveCount, bool bSave );

	bool DeleteFriend( const ioHashString &szName );
	bool IsFriend( const ioHashString &szName );
	bool IsFriendRegHourCheck( const ioHashString &szName, DWORD dw24HourBefore );
	int  GetFriendOnlineUserCount( DWORD dw24HourBefore );
	int  GetFriendLastIndex();
	int  GetFriendSize(){ return m_Friend.GetFriendSize(); }

	void InsertBestFriend( DWORD dwTableIndex, DWORD dwFriendUserIndex, DWORD dwState, DWORD dwMagicDate );
	void UpdateBestFriend( DWORD dwUserIndex, DWORD dwState, DWORD dwMagicDate );

	void SendFriendAndGuildUserLogOut();
	void SendFriendPacket( int iLastIndex, bool bSendLogin );
	void SendBestFriendPacket( int iLastIndex );
	bool SendFriendServerMove( int iLastIndex, int iSendCount, SP2Packet &rkPacket );

#ifdef XTRAP
	bool SendXtrapStep1();
	bool SendXtrapStep1forHackDetect();
#endif
#ifdef NPROTECT
	bool SendNProtectCheck();
#endif 
#ifdef HACKSHIELD
	bool SendHackShieldCheck();
#endif 

	// Ex Info Fun
	void  AddAward( int iCategory, int iPoint );
	bool  IsPCRoomAuthority();
	DWORD GetPCRoomNumber() const;
	void  SetPCRoomNumber( DWORD dwPCRoomNumber ) { m_dwPCRoomNumber = dwPCRoomNumber; }
	void  ChildrenDayEventEndProcess();
	bool  IsRoomLoadingOrServerMoving();
	
public:
	virtual DWORD GetUserIndex() const { return m_user_data.m_user_idx; }
	virtual DWORD GetUserDBAgentID() const;
	virtual const DWORD GetAgentThreadID() const { return m_user_data.m_private_id.GetHashCode(); }
	virtual const ioHashString& GetPrivateID() const { return m_user_data.m_private_id; }
	virtual const ioHashString& GetPublicID() const { return m_user_data.m_public_id; }
	virtual int GetUserCampPos() const { return m_user_data.m_camp_position; }
	virtual int GetGradeLevel();
	virtual int GetUserPos();
	virtual int GetKillDeathLevel();
	virtual int GetLadderPoint() { return m_user_data.m_ladder_point; }
	virtual bool IsSafetyLevel();
	virtual const bool IsUserOriginal(){ return true; }
	virtual bool RelayPacket( SP2Packet &rkPacket );
	virtual ModeType GetModeType();
	virtual const bool IsDeveloper(){ return m_bDeveloper; }	
	virtual int GetUserRanking(){ return m_user_data.m_user_rank; }
	virtual DWORD GetPingStep(){ return m_dwPingStep; }
	virtual bool  IsGuild();
	virtual DWORD GetGuildIndex();
	virtual DWORD GetGuildMark();
	virtual bool IsBestFriend( DWORD dwUserIndex );
	virtual void GetBestFriend( DWORDVec &rkUserIndexList );
	bool IsGeneralGrade();
	
	bool IsObserver();
	bool IsStealth(){ return m_bStealth; }
	BlockType GetBlockType(){ return m_user_data.m_eBlockType; }

	// 타 서버에 자신의 정보 동기화
public:
	void SyncUserLogin();
	void FillUserLogin( SP2Packet &rkPacket );
	void SyncUserLogout();
	void SyncUserUpdate();	
	void SyncUserPos();
	void SyncUserGuild();
	void SyncUserCamp();
	void SyncUserPublicID();
	void SyncUserBestFriend();
	void SyncUserShuffle();
	void FillUserBestFriend( SP2Packet &rkPacket );

public:
	int GetGradeExpert();
	int GetGradeExpRate();
	int GetBackupGradeExp();
	int GetClassLevel( int iSelectChar, bool bOriginal );
	int GetClassLevelByType( int iClassType, bool bOriginal );
	bool IsLeaveSafeRoom();

public:
	void SetMoney( __int64 i64Money );
	void AddMoney( __int64 i64Money );
	void RemoveMoney( __int64 i64Money );
	void SetCash( int iCash );
	void AddCash( int iCash );
	__int64 DecreasePenaltyPeso( __int64 i64Money );
	void SetChannelingCash( int iCash );
	void AddChannelingCash( int iCash );
	void SetPurchasedCash( int iPurchasedCash );
	void AddPurchasedCash( int iPurchasedCash );

	// 타임게이트
	__int64 GetUserTimeGate();
	void SetUserTimeGate( __int64 iTimeGateOpen );

	__int64 GetMoney() const { return m_user_data.m_money; }
	int GetCash() const { return m_user_data.m_cash; }
	int GetPurchasedCash() const { return m_user_data.m_purchased_cash; }
	int GetUserState() const { return m_user_data.m_user_state; }
	int GetJoinChannelSize(){ return m_vChannelNode.size(); }

	ChannelingType      GetChannelingType() const;
	const ioHashString &GetChannelingUserID() const;
	const ioHashString &GetChannelingUserNo() const;
	int       GetChannelingCash() const;
	BlockType GetBlockType() const;
	void      SetBlockType( BlockType eBlockType );
	float     GetBlockPointPer(); 
	CTime     GetBlockTime() const;
	void      SetBlockTime( CTime &rkTime );

	bool  IsFirstJoinUser() const { return m_first_login_user; }
	CTime GetLastLogOutTime() const { return m_user_data.m_connect_time; }
	void SetLastLogOutTime(CTime connectTime) { m_user_data.m_connect_time = connectTime; }

	//빌링에 요청하는 패킷
	void SetOutPutCashPacket( SP2Packet &rkPacket, const int& iBuyCash, const int& iMachineCode, const int& iType );
	
public:
	bool AddGradeExp( int iExp );
	void AddClassExp( int iClassType, int iExp );
	int  GradeNClassUPBonus();
	
	void AddKillCount( RoomStyle eRoomStyle, ModeType eModeType, int iCount );
	void AddDeathCount( RoomStyle eRoomStyle, ModeType eModeType, int iCount );
	void AddWinCount( RoomStyle eRoomStyle, ModeType eModeType, int iCount );
	void AddLoseCount( RoomStyle eRoomStyle, ModeType eModeType, int iCount );

public:
	DWORD GetEtcMotionAni( DWORD dwMotionType );

	bool InitEtcItemUseBattleRecord();
	bool InitEtcItemUseLadderRecord();
	bool InitEtcItemUseHeroRecord();
	
	bool InitHeroSeasonRecord();
	bool HeroSeasonEndDecreaseRate();
	void AddHeroExpert( int iHeroExpert );
	void AddLadderPoint( int iLadderPoint );
	bool SetLadderPoint( int iLadderPoint );
	void AddCampSeasonBonus( DWORD dwBonusIndex, int iBlueCampPoint, int iBlueCampBonusPoint, int iBlueEntry, 
							 int iRedCampPoint, int iRedCampBonusPoint, int iRedEntry, int iMyCampType, int iMyCampPoint, int iMyCampRank );

	void InitUserLadderPointNRecord();
public:
	inline const int GetConnectCnt() const { return m_user_data.m_connect_count; }
	inline DWORD GetDwordMyIP() const { return m_dwMyIP; }

	DWORD GetCharIndex(int array) const;
	int GetCharClassType(int array) const; 
	int GetTopLevelClassType();
	int GetCharArray( DWORD dwIndex ) const;
	bool IsCharMortmain( int iArray ) const;
	bool IsCharClassType( int iClassType ) const;
	bool IsActiveRentalClassType( int iClassType );
	
	int GetCharCount() const { return m_CharList.size(); }         // 구매 용병 + 체험 용병
	int GetActiveCharCount();
	int GetBuyCharCount();                                         // 구매 용병
	int GetActiveBuyCharCount();                                   // 구매 용병 + 고용중인 용병
	int GetExerciseCharCount();                                    // 체험 용병
	int GetExerciseCharCount( int chExercise );
	int GetActiveRentalCount();

	inline DWORD GetSaveTime() const { return m_save_time; }
	inline DWORD GetSaveCheckTime() const { return m_dwSaveCheckTime; }
	inline DWORD GetSyncTime() const { return m_sync_time; }
	inline DWORD GetSyncCheckTime() const { return m_dwSyncCheckTime; }
#ifdef XTRAP
	inline DWORD GetXtrapCheckTime() const { return m_dwXtrapCheckTime; }
#endif
#ifdef NPROTECT
	inline DWORD GetNProtectCheckTime() const { return m_dwNProtectCheckTime; }
	inline int   GetSentNProtectCheckCnt() const { return m_iSentNProtectCheckCnt; }
#endif 
#ifdef HACKSHIELD
	inline DWORD GetHackShieldCheckTime() const { return m_dwHackShieldCheckTime; }
#endif
	inline TeamType GetTeam() const { return m_Team; }
	inline TeamType GetShamBattleTeam() const { return m_ShamBattleTeam; }
	inline int GetSelectChar() const { return m_select_char; }
	inline void SetSelectCharArray( int iSelectArray ){ m_select_char = iSelectArray; }
	int GetSelectClassType();

	__int64 GetPartyExp();
	int GetPartyLevel();
	__int64 GetLadderExp();
	int GetLadderLevel();
	int GetHeroExp();
	int GetHeroMatchPoint();
	int GetHeroTitle();
	ModeType GetPlayingMode();
	
public:
	LadderTeamParent  *GetMyLadderTeam();
	BattleRoomParent *GetMyBattleRoom();
	ShuffleRoomParent *GetMyShuffleRoom();
	DWORD GetMyBattleRoomIndex(){ return m_dwMyBattleRoom; }
	DWORD GetMyShuffleRoomIndex(){ return m_dwMyShuffleRoom; }
	Room *GetMyRoom() { return m_pMyRoom; }
	bool IsServerLobby(){ return m_bJoinServerLobby; }
	
	int GetMyVictories() { return m_iMyVictories; }
	void IncreaseMyVictories( bool bIncrease );

public:
	float GetModeConsecutivelyBonus();
	void  SetModeConsecutively( ModeType eModeType );
	inline int GetModeConsecutivelyCnt(){ return m_iModeConsecutivelyCnt; }

public:
	void SendAllCharInfo();
	void SendFreeDayEvent(DWORD dwPCRoom);

public:
	void ClearCharJoinedInfo();
	void SetCharJoined( int iArray, bool bJoined );
	bool IsCharJoined( int iArray );
	bool IsClassType( int iClassType );
	bool IsClassTypeExceptExercise( int iClassType );
	bool IsClassTypeExerciseStyle( int iClassType, int chExercise );
	bool IsBuyActiveChar( int iClassType );        //구매한 용병중 Active
	bool IsActiveChar( int iClassType );           //PC방 체험 용병은 Active
	int  GetCharArrayByClass( int iClassType );
	int  GetExerciseCharArrayByClass( int iClassType );	
	void CheckCharSlot( ioCharacter *pChar );
	void IntegrityCharSlot();
	int  GetCharSlotIndexToArray( int iSlotIndex );
	bool IsCharPeriodTime( int iClassType );
#ifdef __OHTG_SOLDIER_TAKE_CHANGE__
	bool IsSoldierMortMain( int iClassType );
	bool SendSoldierTake( int iClassType, int iPeriodTime);
#endif //__OHTG_SOLDIER_TAKE_CHANGE__

public:
	// Monster Coin
	int GetModeStartMonsterCoin();
	bool UseModeStartMonsterCoin( int iUseCnt, bool bUseGoldCoin = true);
	bool UseGoldMonsterCoin();
	bool UseRouletteCoin( const int iUseCount, const BOOL bOnlyCheck );
	int  GetEtcMonsterCoin();
	int  GetEtcGoldMonsterCoin();
	int  AddEtcMonsterCoin( int iAddCoin );
	int  AddRefillSeconds( DWORD dwAddSeconds );
	int  GetRefillSeconds();
	void CheckRefillMonsterCoin( DWORD dwProcessSec, bool bFirstSync = false );

public:
	float GetSoldierExpBonus( int iClassType );	

public:
	void SetUserData( const USERDATA &user_data, bool first_login_user );
	void SetUserHeroData( const UserHeroData &rkData );
	void SetUserRelativeGradeData( int iInitTime, bool bReward, int iBackupExp );
	void SetUserHeadquartersData( const UserHeadquartersOption &rkData );
	void SetFriendRecommendData( DWORD dwTableIndex, DWORD dwRecommendIndex );

	ioCharacter* AddCharDataToPointer();
	void SetPrivateID( const ioHashString &szID ) { m_user_data.m_private_id = szID; }
	void SetPublicID( const ioHashString &rszPublicID ) { m_user_data.m_public_id = rszPublicID; }
	
	//[HRYOON구매] 아이템 구매 실패 관련 처리
#ifdef HRYOON_TEST
	bool m_UserBonusCashStatus;
	void SetBonusCashStatus(bool userStatus ) { m_UserBonusCashStatus = userStatus; }
	bool GetBonusCashStatus() { return m_UserBonusCashStatus; }
#endif
//hr US
	virtual void SetUSMemberIndex( DWORD eMemberIndex ) { m_user_data.m_dwUSMemberIndex = eMemberIndex; } 
	DWORD GetUSMemberIndex() const { return m_user_data.m_dwUSMemberIndex; }

	void SetUSMemberID( const ioHashString &szUSID ) { m_user_data.m_USMemberID = szUSID; }			//스팀계정 저장용
	const ioHashString& GetUSMemberID() const { return m_user_data.m_USMemberID; }
	
	virtual void SetUSMemberType( DWORD eMemberType ) { m_user_data.m_USMemberType = eMemberType; } 
	DWORD GetUSMemberType() const { return m_user_data.m_USMemberType; }

public:
		void SetMatchTier(int iStartTier, int iEndTier) { m_UserMatch.SetTier( iStartTier, iEndTier ); }
		int  GetStartTier() { return m_UserMatch.GetStartTier(); }
		int  GetEndTier() { return m_UserMatch.GetEndTier(); }

public:
	void SetTeam( TeamType eTeam );
	void SetShamBattleTeam( TeamType eTeam );

	void InitCharDie();
	void SetCharDie( bool bCharDie );

	bool ToggleExitRoomReserve();
	bool IsExitRoomDelay();
	void SetExitRoomDelay( DWORD dwExitTime );
	bool ExitRoomTimeOver();
	void SetExitPosition( bool bHeadquarters );
	void SetExperienceChar( SP2Packet &rkPacket );

	void ClearExitRoomReserve();
	bool IsExitRoomReserved() const { return m_bExitRoomReserved; }

	bool CheckPreRoomNum( ModeType eModeType );
	void ClearPreRoomNum();
	int GetPreRoomNum() { return m_PreRoomNum; }

    void BattleRoomKickOut( BYTE eType = (BYTE)RoomParent::RLT_NORMAL );
	void BattleRoomMyInfo();
	
	void LadderTeamKickOut();
	void LadderTeamMyInfo();

	void ShuffleRoomKickOut( BYTE eType = (BYTE)RoomParent::RLT_NORMAL );
	void ShuffleRoomMyInfo();

	bool IsUserShuffleRoomJoin();

	void UserVoteRoomKickOut( const ioHashString &rkReason );           //투표로 강퇴됨

public:
	void SetGuildMarkChangeKeyValue( DWORD dwKeyValue ){ m_dwGuildMarkChangeKeyValue = dwKeyValue; }
	bool IsGuildUser( const ioHashString &rkName );

protected:
	bool AddNewItemToDBSlot( int iCharArray, DWORD dwItemCode, int iItemIndex  );
	
public:
	void FillConnectUserData( SP2Packet &rkPacket );
	void FillEquipItemData( SP2Packet &rkPacket );
	void FillJoinUserData( SP2Packet &rkPacket, bool bExperienceChar );

	void SetTransferAddress( ServerNode* &node, ioHashString &ipAddr, int &port );

	void FillChangeCharData( SP2Packet &rkPacket );
	void FillExperienceChar( SP2Packet &rkPacket );
	void FillFinalRoundResult( RoomStyle eRoomStyle, ModeType eModeType, SP2Packet &kPacket );
	void FillItemSlotData( SP2Packet &rkPacket );
	void FillClassData( int iClassType, bool bCurrentEquip, SP2Packet &rkPacket );
	void FillRoomAndUserInfo( SP2Packet &rkPacket );
	int  FillGradeNClassLevelUPBonus( SP2Packet &rkPacket, int iUpClassType, int iUpLevel );
	void FillToolTipInfo( SP2Packet &rkPacket );
	void FillSimpleToolTipInfo( SP2Packet &rkPacket );
	void FillTotalGrowthLevelDataByClassType( IN int iSelectClassType, OUT SP2Packet &rkPacket );
	void FillEquipMedalItem( SP2Packet &rkPacket );
	void FillEquipCustomMedalItem( SP2Packet &rkPacket );
	void FillEquipMedalItemByClassType( IN int iClassType, OUT SP2Packet &rkPacket );
	void FillEquipCustomMedalItemByClassType( IN int iClassType, OUT SP2Packet &rkPacket );
	void FillExMedalSlotByClassType( IN int iClassType, OUT SP2Packet &rkPacket );
	void FillGrowthLevelData( SP2Packet &rkPacket );
	void FillGrowthLevelDataByClassType( int iClassType, SP2Packet &rkPacket );
	void FillHeroMatchInfo( SP2Packet &rkPacket );
	void FillSuccessionMatchInfo( SP2Packet &rkPacket );
	void FillUserCharListInfo( SP2Packet &rkPacket, int iStartArray, int iSyncCount );
	void FillUserCharSubInfo( SP2Packet &rkPacket, int iClassType );

public:
	void SetSelectCharDB( int iClassType );
	int SetPresentChar( int iClassType, int iLimitTime );
	int SetPresentCharDisassemble( int iClassType );
	int SetPresentEtcItem( int iEtcItemType, int iServerValue );
	int SetPresentExtraItem( int iExtraItemCode, int iExtraItemReinforce, int iExtraItemLimitDate, int iExtraItemPeriodType,
							 DWORD dwItemMaleCustom, DWORD dwItemFemaleCustom, int iMentType );
	int SetPresentExtraItemBox( int iExtraItemMachine, int iPesoArray, bool bCash );
	int SetPresentMedalItem( int iMedalItemType, int iMedalItemLimitTime );

	int SetPresentAlchemicItem( int iCode, int iCount );

	int SetPresentPetItem( int iPetCode, int iPetLevel, int iPetRank, DWORD dwIndex, DWORD dwSlotIndex );

	int SetPresentCostume( int iCode, int iLimitDate, DWORD dwPresentIndex, DWORD dwPresentSlotIndex );
	int SetPresentBonusCash( int iAmount, int iLimitDate, DWORD dwPresentIndex, DWORD dwPresentSlotIndex );
	int SetPresentAccessory( int iCode, int iLimitDate, DWORD dwPresentIndex, DWORD dwPresentSlotIndex );

	int SetPresentSpirit( int iCode, int iQuantity );


	void SetPresentCustomMedal( int iIndex, int iSlotIndex, int iCode, int iValue, SP2Packet &rkPacket );
	void SetPresentCustomMedalDB( int iIndex, int iSlotIndex, int iCode, int iValue, SP2Packet &rkPacket );
	bool IsCustomMedalType( int iItemType );
	CustomMedal* GetCustomMedalData( int iIndex, int iSlotIndex );
	
	// 서버 이동
public:
	void FillMoveData( SP2Packet &rkPacket );
	void FillInventoryMoveData( SP2Packet &rkPacket );
	void FillInventoryMoveData( SP2Packet &rkPacket, int iStartIndex );

	void FillExtraItemMoveData( SP2Packet &rkPacket );
	void FillExtraItemMoveData( SP2Packet &rkPacket, int iStartRow );

	void FillQuestMoveData( SP2Packet &rkPacket );
	void FillAlchemicInvenMoveData( SP2Packet &rkPacket );
	void FillPetMoveData( SP2Packet &rkPacket ); 
	void FillAwakeMoveData( SP2Packet &rkPacket ); 
	void FillSoldierMoveData( SP2Packet &rkPacket ); 
	void FillEtcItemMoveData( SP2Packet &rkPacket );
	void FillMedalItemMoveData( SP2Packet &rkPacket );
	void FillCostumeMoveData( SP2Packet &rkPacket );
	void FillMissionMoveData( SP2Packet &rkPacket );
	void FillRollBookMoveData( SP2Packet &rkPacket );
	void FillPersonalHQData( SP2Packet &rkPacket );
	void FillTimeCashDate( SP2Packet &rkPacket );
	void FillTitleMoveData( SP2Packet &rkPacket );
	void FillBonusCashData( SP2Packet &rkPacket );
	void FillAccessoryMoveData( SP2Packet &rkPacket );

	void ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode = false );
	void ApplyInventoryMoveData( SP2Packet &rkPacket );
	void ApplyExtraItemMoveData( SP2Packet &rkPacket );
	void ApplyQuestMoveData( SP2Packet &rkPacket );
	void ApplyAlchemicInvenData( SP2Packet &rkPacket );
	void ApplyPetData( SP2Packet &rkPacket );
	void ApplyAwakeMoveData( SP2Packet &rkPacket );
	void ApplySoldierMoveData( SP2Packet &rkPacket );
	void ApplyEtcItemMoveData( SP2Packet &rkPacket );
	void ApplyMedalItemMoveData( SP2Packet &rkPacket );
	void ApplyCostumeMoveData( SP2Packet &rkPacket );
	void ApplyMissionMoveData( SP2Packet &rkPacket );
	void ApplyRollBookMoveData( SP2Packet &rkPacket );
	void ApplyPersonalHQMoveData(SP2Packet &rkPacket);
	void ApplyTimeCashDate( SP2Packet &rkPacket );
	void ApplyTitleMoveData(SP2Packet &rkPacket);
	void ApplyBonusCashData( SP2Packet &rkPacket );
	void ApplyAccessoryMoveData( SP2Packet &rkPacket );

	void FillExerciseIndex( SP2Packet &rkPacket );

public:  // 룸 유저들과 정보 동기화
	void SendGradeSync();
	void SyncModeEtcItem( IN IntVec &rvModeEtcItemList, OUT IntVec &rvReturnEtcItem );

	//TCP
public:
	bool OnRoomProcessPacket( SP2Packet &rkPacket );
	bool OnBattleRoomProcessPacket( SP2Packet &rkPacket );
	bool OnLadderTeamProcessPacket( SP2Packet &rkPacket );

	void OnClose( SP2Packet &packet );
	void OnConnect( SP2Packet &packet );
	void OnMovingServer( SP2Packet &rkPacket );
	void OnJoinServerLobbyInfo( SP2Packet &rkPacket );

	void OnFriendList( SP2Packet &rkPacket );
	void OnFriendRequestList( SP2Packet &rkPacket );
	void OnFriendApplication( SP2Packet &rkPacket );
	void OnFriendCommand( SP2Packet &rkPacket );
	void OnFriendDelete( SP2Packet &rkPacket );

	void OnBestFriendInsert( SP2Packet &rkPacket );
	void OnBestFriendInsertFailed( SP2Packet &rkPacket );
	void _OnBestFriendDismiss( const ioHashString &rkFriendName, bool bResult );
	void OnBestFriendDismiss( SP2Packet &rkPacket );
	void OnBestFriendExceptionList( SP2Packet &rkPacket );

	void OnUserPosRefresh( SP2Packet &rkPacket );
	void OnUserLogin( SP2Packet &rkPacket );
	void OnRegisteredUser( SP2Packet &rkPacket );
	void OnDeleteFriendByWeb( SP2Packet &rkPacket );

	void _OnLeaderCreate( int iClassType );
	void OnCharCreate( SP2Packet &packet );
	void OnServiceChar( SP2Packet &rkPacket );
	void _OnCharDelete( int iCharArray );
	void OnCharDelete( SP2Packet &packet );
	void OnChangeLeaderChar( SP2Packet &rkPacket );
	int _OnSetMyRentalCharException( DWORD dwCharIndex );
	void OnSetMyRentalChar( SP2Packet &rkPacket );	
	void OnCharDisassemble( SP2Packet &packet );

	void OnJoinRoom( SP2Packet &packet );
	void OnDropItem( SP2Packet &rkPacket );
	void OnItemMoveDrop( SP2Packet &rkPacket );
	void OnPickItem( SP2Packet &rkPacket );
	void OnDeleteFieldItem( SP2Packet &rkPacket );
	void OnSearchPlazaRoom( SP2Packet &rkPacket );
	void OnPlazaRoomList( SP2Packet &rkPacket );
	void OnCreatePlaza( SP2Packet &rkPacket );
	
	void AutoCreatePlaza( SP2Packet &rkPacket );
	bool CreatePlazaInThisSvr( ioHashString& szPlazaName, ioHashString& szPlazaPW, int iMaxPlayer, int iSubIndex, int iPlazaType, bool bAuto );
	int CheckPlazaCreateDefaultErr();

	void SetPlazaInfoWithPacket( SP2Packet &rkPacket, ioHashString& szPlazaName, ioHashString& szPlazaPW, int& iMaxPlayer, int& iSubIndex, int& iPlazaType );
	void SetPlazaInfoWithDefault( int& iMaxPlayer, int& iSubIndex, int& iPlazaType );

	void OnPlazaCommand( SP2Packet &rkPacket );
	void OnPlazaInviteList( SP2Packet &rkPacket );
	void OnPlazaInvite( SP2Packet &rkPacket );

	void OnHeadquartersInviteList( SP2Packet &rkPacket );
	void _OnJoinHeadquarters( UserParent *pRequestUser, int iMapIndex, bool bInvited );
	void OnJoinHeadquarters( SP2Packet &rkPacket );
	void _OnHeadquartersInfo( UserParent *pRequestUser );
	void OnHeadquartersInfo( SP2Packet &rkPacket );
	void _OnHeadquartersJoinAgree( DWORD dwHeadquartersModeIndex );
	void OnHeadquartersJoinAgree( SP2Packet &rkPacket );
	
	void OnChangeSingleChar( SP2Packet &rkPacket );
	void OnChangeChar( SP2Packet &rkPacket );
	bool SetChangeChar( int iCharArray, bool bWait, int iSelectCharArray, DWORD dwCharChangeIndex );	

	void OnUseItem( SP2Packet &rkPacket );
	void OnBuyItem( SP2Packet &rkPacket );
	void OnEquipItem( SP2Packet &rkPacket );
	void OnIncreaseStat( SP2Packet &rkPacket );
	void OnInitStat( SP2Packet &rkPacket );

	void OnCharSlotChange( SP2Packet &rkPacket );

	void OnLogOut( SP2Packet &rkPacket );
	
	void OnBattleRoomReserveDelete( SP2Packet &rkPacket );
	void OnJoinBattleRoomList( SP2Packet &rkPacket );
	void OnCreateBattleRoom( SP2Packet &rkPacket );
	void _OnBattleRoomJoinResult( int iResultType, bool bMovePenalty, int iPenaltyPeso, bool bServerMove = false );
	void OnUserBattleRoomJoin( SP2Packet &rkPacket );
	void OnUserBattleRoomLeave( SP2Packet &rkPacket );
	void OnBattleRoomInviteList( SP2Packet &rkPacket ); 

	void OnUserShuffleRoomJoin( SP2Packet &rkPacket );
	void OnUserShuffleRoomJoinCancel( SP2Packet &rkPacket );
	void OnUserShuffleRoomLeave( SP2Packet &rkPacket );
	void _OnShuffleRoomJoinResult( int iResultType );
	void OnLevelUpItem( SP2Packet &rkPacket );
	
	void OnTutorialStep( SP2Packet &rkPacket );

	void OnCharLimitCheck( SP2Packet &rkPacket );
	void OnCharDecorationBuy( SP2Packet &rkPacket );
	int  _OnEtcItemMaxCheck( ioEtcItem* pEtcItem, ioUserEtcItem::ETCITEMSLOT &kEtcItemSlot, int iServerValue, bool bMortmain );
	DWORD _OnEtcItemSetting( ioEtcItem* pEtcItem, ioUserEtcItem::ETCITEMSLOT &kEtcItemSlot, int iServerValue, bool bMortmain );
	void OnEtcItemBuy( SP2Packet &rkPacket );
	void OnEtcItemUse( SP2Packet &rkPacket );
	void OnEtcItemMotionOption( SP2Packet &rkPacket );
	void OnEtcItemSell( SP2Packet &rkPacket );
	int _OnEtcItemPackageDecoInsert( SP2Packet &rkPacket, DWORD dwEtcItemType, bool bSoldierPackage = false );
	void OnEtcItemSoldierPackage( SP2Packet &rkPacket, DWORD dwEtcItemType, int iClassType, int eActiveFilter );
	void OnEtcItemDecorationPackage( SP2Packet &rkPacket, DWORD dwEtcItemType );
	void OnCharExtend( SP2Packet &rkPacket );
	void OnCharCharge( SP2Packet &rkPacket );
	void OnCharChangePeriod( SP2Packet &rkPacket );

	int _OnEtcItemPreSetDecoInsert( CHARACTER kCharInfo, ITEMSLOTVec vItemSlot, DWORD dwEtcItemType );
	bool OnEtcItemPreSetPackage( int iClassType, int iLimitTime, ITEMSLOTVec vItemSlot, DWORD dwEtcItemType, bool bPresent );

	void OnPartyUserInviteAnswer( SP2Packet &rkPacket );
	void OnRelayChat( SP2Packet &rkPacket );
	void OnBattleRoomInfo( SP2Packet &rkPacket );
	void OnServerLobbyChat( SP2Packet &rkPacket );
	void OnMyRoomServerChange( SP2Packet &rkPacket );
	void OnPlazaRoomInfo( SP2Packet &rkPacket );
	void OnTrial( SP2Packet &rkPacket );
	void OnHackQuiz( SP2Packet &rkPacket );
	void OnMacroQuiz( SP2Packet &rkPacket );
	void OnAbuseQuizStart( SP2Packet &rkPacket );
	void OnAbuseQuiz( SP2Packet &rkPacket );
	bool _OnFollowUser( UserParent *pRequestUser, int iUserPos, int iRoomIndex, int iLeaveRoomIndex, int iLeaveBattleRoomIndex, bool bDeveloper, int iAbilityLevel );
	void _OnBattleRoomFollow( BattleRoomParent *pBattleRoom, int iNextPos );
	void _OnLadderTeamFollow( LadderTeamParent *pLadderTeam, int iNextPos );
	void OnFollowUser( SP2Packet &rkPacket );
	void _OnUserPosIndex( UserParent *pRequestUser, bool bSafetyLevel, int iUserPos, int iPrevBattleIndex );
	void OnUserPosIndex( SP2Packet &rkPacket );
	void OnDeveloperMacro( SP2Packet &rkPacket );
	void OnExerciseCharCreate( SP2Packet &packet );
	void OnPresentTestSend( SP2Packet &rkPacket );

	void OnChannelCreate( SP2Packet &rkPacket );
	void OnChannelInvite( SP2Packet &rkPacket );
	int  _OnChannelInviteException( int iIndex );
	void OnChannelLeave( SP2Packet &rkPacket );
	void OnChannelChat( SP2Packet &rkPacket );
	void OnUserInfoRefresh( SP2Packet &rkPacket );
	void OnSimpleUserInfoRefresh( SP2Packet &rkPacket );
	void OnUserCharInfoRefresh( SP2Packet &rkPacket );
	void OnUserCharSubInfoRefresh( SP2Packet &rkPacket );
	void OnUserCharRentalRequest( SP2Packet &rkPacket );
	void _OnUserCharRentalAgree( UserParent *pOwnerParent, const ioHashString &rkOwnerID, DWORD dwOwnerDBAgentID, const CHARACTER &rkCharInfo, RentalData &rkRentalData );
	void OnUserCharRentalAgree( SP2Packet &rkPacket );
	void OnUserCharRentalTimeEnd( SP2Packet &rkPacket );
	void OnUserInfoExist( SP2Packet &rkPacket );
	void OnMemoMsg( SP2Packet &rkPacket );
	void OnOfflineMemoList( SP2Packet &rkPacket );
	void OnBankruptcyPeso( SP2Packet &rkPacket );
	void OnUserEntryRefresh( SP2Packet &rkPacket );
	void OnFirstChangeID( SP2Packet &rkPacket );
	void OnAbstract( SP2Packet &rkPacket );

	void OnGrowthLevelUp( SP2Packet &rkPacket );
	void OnGrowthLevelInit( SP2Packet &rkPacket );
	void OnLevelGrowth( SP2Packet &rkPacket );
	void OnFishingState( SP2Packet &rkPacket );
	void OnEtcItemAction( SP2Packet &rkPacket );
	void OnEtcItemSwitch( SP2Packet &rkPacket );
	void OnRouletteState( SP2Packet &rkPacket );

	void OnGuildRankList( SP2Packet &rkPacket );
	void OnGuildInfo( SP2Packet &rkPacket );
	void OnGuildUserList( SP2Packet &rkPacket );
	void OnGuildJoinerChange( SP2Packet &rkPacket );
	void OnGuildEntryApp( SP2Packet &rkPacket );
	void OnGuildEntryCancel( SP2Packet &rkPacket );
	void OnGuildEntryDelayMember( SP2Packet &rkPacket );
	void OnGuildEntryAgree( SP2Packet &rkPacket );
	void OnGuildEntryRefuse( SP2Packet &rkPacket );
	void _OnGuildInvitation( UserParent *pSendUser, DWORD dwGuildIndex, int iGuildMark, const ioHashString szGuildName );
	void OnGuildInvitation( SP2Packet &rkPacket );
	void OnGuildLeave( SP2Packet &rkPacket );
	void OnGuildTitleChange( SP2Packet &rkPacket );
	void OnGuildMasterChange( SP2Packet &rkPacket );
	void OnGuildPositionChange( SP2Packet &rkPacket );
	void OnGuildKickOut( SP2Packet &rkPacket );
	void _OnGuildMarkChange( DWORD dwGuildIndex, DWORD dwGuildMark, bool bBlock = false );
	void OnGuildChat( SP2Packet &rkPacket );
	void OnGuildMarkChangeKeyValue( SP2Packet &rkPacket );
	void OnGuildMarkChangeKeyValueDelete( SP2Packet &rkPacket );
	void OnGuildTitleSync( SP2Packet &rkPacket );
	void OnGuildExist( SP2Packet &rkPacket );
	void OnLadderTeamList( SP2Packet &rkPacket );
	void OnCreateLadderTeam( SP2Packet &rkPacket );
	void OnJoinLadderTeam( SP2Packet &rkPacket );
	void OnLadderTeamJoinInfo( SP2Packet &rkPacket );
	void OnLadderTeamLeave( SP2Packet &rkPacket );
	void OnLadderTeamInviteList( SP2Packet &rkPacket );
	void OnLadderOtherTeamNameChange( SP2Packet &rkPacket );
	void OnLadderBattleRequestAgree( SP2Packet &rkPacket );
	void OnLadderUserHQMove( SP2Packet &rkPacket );
	void OnLadderTeamRanking( SP2Packet &rkPacket );
	void OnLadderTeamRankList( SP2Packet &rkPacket );

	void OnMovieControl( SP2Packet &rkPacket );
	void _OnSelectPresent( DWORD dwSelectCount );
	void OnPresentRequest( SP2Packet &rkPacket );
	void OnPresentRecv( SP2Packet &rkPacket );
	void OnPresentDisassemble( SP2Packet &rkPacket );
	void OnPresentSell( SP2Packet &rkPacket );
	void OnPresentBuy( SP2Packet &rkPacket );
	void _OnDBPresentBuy( DWORD dwRecvUserIndex, int iRecvPresentCnt, const char *szRecvPrivateID, const char *szRecvPublicID, short iPresentType, int iBuyValue1, int iBuyValue2 );

	void OnVoiceInfo( SP2Packet &rkPacket );
	void OnProtectCheck( SP2Packet &rkPacket );

	// 청약
	void OnSubscriptionRequest( SP2Packet &rkPacket );

	void OnSubscriptionBuy( SP2Packet &rkPacket );
	void OnSubscriptionRecv( SP2Packet &rkPacket );
	void OnSubscriptionDisassemble( SP2Packet &rkPacket );
	void OnSubscriptionRetr( SP2Packet &rkPacket );
	void OnSubscriptionRetrCheck( SP2Packet &rkPacket );

	bool CheckSubscriptionRetr( SP2Packet &rkPacket, DWORD dwIndex, const ioHashString& szSubscriptionID );
	bool SetSubscriptionRetr( DWORD dwIndex, const ioHashString& szSubscriptionID );

	void _OnSelectSubscription( DWORD dwSelectCount );
	void _OnDBSubscriptionBuy( DWORD dwRecvUserIndex, const char *szRecvPrivateID, short iPresentType, int iBuyValue1, int iBuyValue2 );

	//
	void OnGetCash( SP2Packet &rkPacket );
	void OnHoleSendComplete( SP2Packet &rkPacket );
	void OnUDPRecvTimeOut( SP2Packet &rkPacket );
	void OnEventDataUpdate( SP2Packet &rkPacket );
	void OnServerLobbyInfo( SP2Packet &rkPacket );
	void OnCampDataSync( SP2Packet &rkPacket );
	void OnCampChangePos( SP2Packet &rkPacket );
	void OnCampBattleEndLeaveTeam( SP2Packet &rkPacket );
	void OnAllItemDrop( SP2Packet &rkPacket );
	void OnChangeGangsi( SP2Packet &rkPacket );
	void OnGashaponList( SP2Packet &rkPacket );
	void OnExcavationCommand( SP2Packet &rkPacket );
	
	void OnServerAlarmMsg( SP2Packet &rkPacket );
	void OnControlKeys( SP2Packet &rkPacket );

	// ExtraItem
	BOOL IsWearingExtraItem( int iIndex, int iTargetClass );
	void PutonExtraItem( const int iCharArray, int iSlot, int iNewIndex );
	void TakeoffExtraItem( const int iCharArray, int iSlot );
	void OnExtraItemBuy( SP2Packet &rkPacket );
	void OnExtraItemChange( SP2Packet &rkPacket );
	void OnExtraItemSell( SP2Packet &rkPacket );
	void OnExtraItemDisassemble( SP2Packet &rkPacket );
	int	 GetEquipedClassWithExtraItem(int iItemIndex);

	// MedalItem
	void OnMedalItemSell( SP2Packet &rkPacket );

	// CheckBaseValue
	void OnCheckBaseValue( SP2Packet &rkPacket );

	void OnShutdownDate( SP2Packet &rkPacket );

	// Alchemic
	void OnAlchemicFunc( SP2Packet &rkPacket );

	// Pet
	bool FillEquipPetData( SP2Packet &rkPacket );
	bool OnPetEggUse( SP2Packet &rkPacket, bool &bCash, int &iType );
	void OnPetChange( SP2Packet &rkPacket );
	void OnPetSell( SP2Packet &rkPacket ); 
	void OnPetNurture( SP2Packet &rkPacket ); //펫 육성
	void OnPetComPound( SP2Packet &rkPacket );
	void OnPetEquipInfo( SP2Packet &rkPacket );


	// Quest 
	void OnQuestOccur( SP2Packet &rkPacket );
	void OnQuestAttain( SP2Packet &rkPacket );
	void OnQuestAlarm( SP2Packet &rkPacket );
	void OnQuestReward( SP2Packet &rkPacket );
	bool _OnQuestDirectReward( int iPresentType, int iPresentValue1, int iPresentValue2, int iPresentValue3, int iPresentValue4, int iMentType );
	bool _OnQuestRewardPresent( const ioHashString &rkSendID, int iPresentType, int iPresentValue1, int iPresentValue2, int iPresentValue3, int iPresentValue4, 
								int iPresentState, int iPresentMent, int iPresentPeriod, bool bDirectPresent );
	void OnQuestAllDelete( SP2Packet &rkPacket );
	
	void OnPresentAllDelete( SP2Packet &rkPacket );

	void OnHeroTop100Data( SP2Packet &rkPacket );
	void OnHeroMatchOtherInfo( SP2Packet &rkPacket );

	void OnTradeCreate( SP2Packet &rkPacket );
	void OnTradeList( SP2Packet &rkPacket );
	void OnTradeItem( SP2Packet &rkPacket );
	void OnTradeCancel( SP2Packet &rkPacket );

	void OnEventShopGoodsList( SP2Packet &rkPacket );
	void _OnEventShopGoodsBuy( int iBuyState, SP2Packet &rkPacket );   // 메인 서버에서 받은 정보
	void OnEventShopGoodsBuy( SP2Packet &rkPacket );    // 클라이언트에서 받은 정보
	void OnEventShopState( SP2Packet &rkPacket );
	void OnEventShopBuyUserClear( SP2Packet &rkPacket );
	void OnEventNpcClose( SP2Packet &rkPacket );

	// billing msg
	void OnBillingGetCash( SP2Packet &rkPacket );
	void OnBillingOutputCash( SP2Packet &rkPacket );
	void OnBillingLogin( SP2Packet &rkPacket );
	void OnBillingRefundCash( SP2Packet &rkPacket );
	void OnBillingUserInfo( SP2Packet &rkPacket );
	void OnBillingAutoUpgradeLogin( SP2Packet &rkPacket );
	void OnBillingPCRoom( SP2Packet &rkPacket );
	void OnBillingAutoUpgradeOTP( SP2Packet &rkPacket );
	void OnBillingGetMileage( SP2Packet &rkPacket );
	void OnBillingAddMileage( SP2Packet &rkPacket );
	void OnBillingIPBonus( SP2Packet &rkPacket );
	void OnBillingAddCash( SP2Packet &rkPacket );
	void OnBillingFillCashUrl( SP2Packet &rkPacket );
	void OnBillingTimeoutBillingGUID( SP2Packet &rkPacket );

	// 청약철회
	void OnBillingSubscriptionRetractCheck(SP2Packet& rkPacket );
	void OnBillingSubscriptionRetract(SP2Packet& rkPacket );

	//세션서버 컨트롤 
	void OnSessionControl( SP2Packet& rkPacket );

	void OnNexonTerminateMessage( SP2Packet& rkPacket, int controlType );

	// autoupgrade msg
	void OnAutoUpgradeLogin( SP2Packet &rkPacket );
	void OnAutoUpgradeOTP( SP2Packet &rkPacket );

	void OnMedalItemChange(SP2Packet &rkPacket);

	void OnNProtectCheck( SP2Packet &rkPacket );

	void OnRoomStealthEnter( SP2Packet &rkPacket );
	void OnCampSeasonBonus( SP2Packet &rkPacket );
	void OnCustomItemSkinUniqueIndex( SP2Packet &rkPacket );
	void OnCustomItemSkinDelete( SP2Packet &rkPacket );
	void OnGetMileage( SP2Packet &rkPacket );

	void OnHeadquartersOptionCmd( SP2Packet &rkPacket );
	void OnHeadquartersCommand( SP2Packet &rkPacket );

	void OnDisconnectAlreadyID( SP2Packet &rkPacket );
	void OnPCInfo( SP2Packet &rkPacket );

	void OnBuySelectExtraGashapon( SP2Packet &rkPacket );

	//
	void OnTournamentRegularRequest( SP2Packet &rkPacket );
	void OnTournamentMainInfo( SP2Packet &rkPacket );
	void OnTournamentListRequest( SP2Packet &rkPacket );
	void OnTournamentTeamCreate( SP2Packet &rkPacket );
	void OnTournamentTeamInfo( SP2Packet &rkPacket );
	void OnTournamentTeamUserList( SP2Packet &rkPacket );
	void OnTournamentTeamInvitation( SP2Packet &rkPacket );
	void OnTournamentTeamEntryDelayMember( SP2Packet &rkPacket );
	void OnTournamentTeamEntryApp( SP2Packet &rkPacket );
	void OnTournamentTeamEntryRefuse( SP2Packet &rkPacket );
	void OnTournamentTeamEntryAgree( SP2Packet &rkPacket );
	void OnTournamentTeamLeave( SP2Packet &rkPacket );
	void OnTournamentScheduleInfo( SP2Packet &rkPacket );
	void OnTournamentRoundTeamData( SP2Packet &rkPacket );
	void OnTournamentRoomList( SP2Packet &rkPacket );
	void OnTournamentTeamAllocateList( SP2Packet &rkPacket );
	void OnTournamentTeamAllocateData( SP2Packet &rkPacket );
	void OnTournamentJoinConfirmCheck( SP2Packet &rkPacket );
	void OnTournamentJoinConfirmRequest( SP2Packet &rkPacket );
	void OnTournamentJoinConfirmCommand( SP2Packet &rkPacket );
	void OnTournamentAnnounceChange( SP2Packet &rkPacket );
	void OnTournamentTotalTeamList( SP2Packet &rkPacket );
	void OnTournamentCustomStateStart( SP2Packet &rkPacket );
	void OnTournamentCustomRewardList( SP2Packet &rkPacket );
	void OnTournamentCustomRewardBuy( SP2Packet &rkPacket );
	void OnTournamentCheerDecision( SP2Packet &rkPacket );	
	void OnTournamentCheerDecisionOK( SP2Packet &rkPacket );

	// s -> c
	void CloverRefill( CTime& CurrentTime );	// 리필
	void GiftCloverInfo( const int iType );
	void CloverFriendInfo( const DWORD dwFriendIndex, const int iType );

	// c -> s
	void OnCloverChargeReq( SP2Packet& kPacket );		//! 충전
	void OnCloverSendReq( SP2Packet& kPacket );			//! 보내기
	void OnCloverReceiveReq( SP2Packet& kPacket );		//! 받기
	void OnCloverReceiveDelete( SP2Packet& kPacket );	//! 기간만료 클로버 삭제.

	void OnBingoStart( SP2Packet& kPacket );
	void OnBingoNumberInitialize( SP2Packet& kPacket );
	void OnBingoALLInitialize( SP2Packet& kPacket );	

	void OnSendSuperGashponPackage( DWORD dwEtcItemType, DWORD dwPackageIndex, int iUseType );
	void OnSendSuperGashponSubPackage( DWORD dwEtcItemType, int iUseType );
	void OnSendSuperGashponLimitInfo(  DWORD dwEtcItemType, DWORD dwLimit );

	void OnFillCashUrl( SP2Packet& kPacket );

	void OnPirateRouletteInfoRequest( SP2Packet& kPacket );
	void OnPirateRouletteUseSwordRequest( SP2Packet& kPacket );
	void OnPirateRouletteResetRequest( SP2Packet& kPacket );
	void ExitRoomToLobby();// 수련장때매 임시로 추가

	//수련장
	void OnPracticeIndexRank( SP2Packet &rkPacket );
	void OnPracticeList( SP2Packet &rkPacket );
	void OnPracticeRankList( SP2Packet &rkPacket );
	void OnPracticeEnter( SP2Packet &rkPacket );
	void SendPracticeIndexRank(int dwID, int dwGrade, int dwCount, int dwTime, int dwRank);
	void SendPracticeInfoList();
	void SendPracticeRankList(bool bLogin);
	void CheckInitPractice();

protected:
	// billing msg
	void _OnBillingOutputCashSoldier( SP2Packet &rkPacket, int iReturnItemPrice,int iPayAmt, int iTransactionID, int& iBuyType, int& iValue1, int& iValue2 );
	void _OnBillingOutputCashDeco( SP2Packet &rkPacket, int iReturnItemPrice, int iPayAmt, int iTransactionID, int& iBuyType, int& iValue1, int& iValue2 );
	void _OnBillingOutputCashSoldierExtend( SP2Packet &rkPacket, int iReturnItemPrice, int iPayAmt, int iTransactionID );
	void _OnBillingOutputCashEtc( SP2Packet &rkPacket, int iReturnItemPrice,int iPayAmt, int iTransactionID, int& iBuyType, int& iValue1, int& iValue2 );
	void _OnBillingOutputCashSoldierChangePeriod( SP2Packet &rkPacket, int iReturnItemPrice, int iPayAmt, int iTransactionID );
	void _OnBillingOutputCashExtra( SP2Packet &rkPacket, int iReturnItemPrice,int iPayAmt, int iTransactionID, int& iBuyType, int& iValue1, int& iValue2 );
	void _OnBillingOutputCashPresent( SP2Packet &rkPacket, int iReturnItemPrice,int iPayAmt, int iTransactionID );
	void _OnBillingOutputCashSubscription( SP2Packet &rkPacket, int iReturnItemPrice,int iPayAmt, int iTransactionID, ioHashString szSubscriptionID = "" );
	void _OnBillingOutputCashCostume( SP2Packet &rkPacket, int iReturnItemPrice,int iPayAmt, int iTransactionID, int& iBuyType, int& iValue1, int& iValue2 );
	void _OnBillingOutputCashPopup( SP2Packet &rkPacket, int iReturnItemPrice, int iPayAmt, int iTransactionID );

protected:
	bool CheckHackCount( DWORD dwCurGap );
	void PrintHackLog( DWORD dwCurGap );

	//UDP
public:
	void OnUDPConnect( SP2Packet &rkPacket );
	void _OnCheckPingStep( DWORD dwClientTime );
	void OnSyncTime( SP2Packet &rkPacket );
	void OnReserveRoomJoin( SP2Packet &rkPacket );
	void OnWebEvent( SP2Packet &rkPacket );
	void OnWebRefreshBlock( SP2Packet &rkPacket );
	void OnWebGetCash( SP2Packet &rkPacket );
	void OnWebRefreshUserEntry( SP2Packet &rkPacket );
	
	void OnCheckKingUserPing( SP2Packet &rkPacket );
	void OnCheckFlagUserPing( SP2Packet &rkPacket ); // 2018-03-15 by bckim, 깃발모드 추가

public:
	void  SetStartTimeLog(DWORD dwStartTimeLog);
	DWORD GetStartTimeLog();
	inline const ioHashString& GetGUID() const { return m_szGUID; }

	ioHashString& GetPacketGUID() { return m_szPacketGUID; }

	EventUserManager& GetEventUserMgr();

	EntryType GetEntryType() const;
	bool IsEntryFormality() const;
	void SetEntryType( EntryType eEntryType );

	void ImmediatelyEquipItem( ioItem *pItem, const ioHashString& szItemName, int eObjectCreateType, SP2Packet &rkPacket );
	void SetDefaultDecoItem( const CHARACTER &rkCharInfo );

private:
	void DeleteCharData( int iCharArray );
	void FixSelectChar();
	bool IsDeleteExerciseChar( Room *pRoom, BYTE chExerciseType );
	bool IsBuyExerciseChar( int iExerciseStyle );

	bool IsCanBuyItem( const ioSetItemInfo *pSetItemInfo );
	bool IsCanBuyItemBySameGradeLevel( const ioSetItemInfo *pSetItemInfo );

	void DeleteGuildMarkChangeKeyValue();

	ITEMSLOT GetItemSlot( int iDecoType, const CHARACTER &rkCharInfo );
	RaceDetailType GetRaceDetailType( const CHARACTER &rkInfo );

	int  GetFriendSlotSize();

public:
	bool DeleteExerciseChar(BYTE chExerciseStyle );
	bool DeleteExercisePCRoomChar( DWORD dwCharIdx );

	void DeleteExerciseRentalChar( int iCharArray, bool bReturnPacket );
	void DeleteExerciseRentalCharAll();
	void CheckExerciseRentalCharDeleteTime();
	void BuyRentalCharacterProcess( ioCharacter *pCharacter );

public:
	inline bool IsBillingWait() { return !m_szBillingGUID.IsEmpty(); }
	inline const ioHashString& GetBillingGUID() const { return m_szBillingGUID; }
	void SetBillingGUID( const char* szGUID );
	void ClearBillingGUID();

	void StartEtcItemTime( const char *szFunction, int iType = 0/*ioEtcItem::EIT_NONE*/ ); // ioEtcItem.h 추가하지 않기 위해서
	void UpdateEtcItemTime( const char *szFunction, int iType = 0/*ioEtcItem::EIT_NONE*/ );
	void LeaveRoomEtcItemTime(); 
	void DeleteEtcItemPassedDate();

	bool IsNewPublicID() const { return !m_szNewPublicID.IsEmpty(); }
	void SetNewPublicID( const char *szNewPublicID ) { m_szNewPublicID = szNewPublicID; }
	void SetPublicIPForNewPublicID( const char *szPublicIPForNewPublicID) { m_szPublicIPForNewPublicID = szPublicIPForNewPublicID; }

	inline const ioHashString& GetBillingUserKey() const { return m_szBillingUserKey; }
	inline void SetBillingUserKey( const ioHashString &rszBillingUserKey ) { m_szBillingUserKey = rszBillingUserKey; }

	void SendEventData();

	// Growth
	void OnCheckTimeGrowth( SP2Packet &rkPacket );
	void OnAddTimeGrowth( SP2Packet &rkPacket );
	void OnRemoveTimeGrowth( SP2Packet &rkPacket );

	void CheckTimeGrowth();

	void ClearKingPingCnt();
	inline int GetCurKingPingCnt() { return m_iCurRecvKingPingCnt; }

	void ClearFlagPingCnt();								// 2018-03-15 by bckim, 깃발모드 추가
	inline int GetCurFlagPingCnt() { return m_iCurRecvFlagPingCnt; }	// 2018-03-15 by bckim, 깃발모드 추가	

	// Extend CharSlot
	inline int GetCurMaxCharSlot() { return m_iCurMaxCharSlot; }
	void CheckCurMaxCharSlot();

	// ExtraItem
	int FindExtraItemEquipChar( int iSlotIndex );

	void DeleteExtraItemPassedDate( bool bImmediately );

	// Item Compound
	bool OnItemCompound( SP2Packet &rkPacket, DWORD dwType );
	bool OnItemMaterialCompound( SP2Packet &rkPacket, DWORD dwType );
	bool OnMultipleItemCompound( SP2Packet &rkPacket, DWORD dwType );

	// Item GrowthCatalyst
	bool OnItemGrowthCatalyst( SP2Packet &rkPacket, DWORD dwType );

	bool OnItemCompoundEx( SP2Packet &rkPacket, DWORD dwType, int iRandValue );

	void SetFirstChangeID(bool bFirstChangeID) { m_bFirstChangeID = bFirstChangeID; }

	// Expand Medal
	bool OnExpandMedalSlotOpen( SP2Packet &rkPacket, DWORD dwType );
	void DeleteExMedalSlotPassedDate();

	//Plaza
	void OnSpawnNpcInPlaza(SP2Packet &rkPacket);

	//SuperGashapon
	void OnSuperGashaponGetAll(SP2Packet &rkPacket);
	void OnSuperGashaponGet(SP2Packet &rkPacket);
	void OnSuperGashaponInfoGet(SP2Packet &rkPacket);
	void OnSuperGashaponInfoReset(SP2Packet &rkPacket);

	bool FindSuperGashaponPackageRandomPresent( DWORD dwEtcItemType, DWORD& dwPackageIndex);		// 2019-03-11 by bckim, 티메가테 

	//
	void OnMacroAttendanceAddDay(SP2Packet &rkPacket);
	void OnMacroAttendancePrevMonth(SP2Packet &rkPacket);
	void OnMacroAttendanceDateModify(SP2Packet &rkPacket);

public:
	bool IsFishingState();
	bool CheckFishingInfo();

	int GetFishingLevel();
	int GetFishingExpert();
	bool AddFishingExp( int iExp );
	
	int GetFishingSlotExtendItem();
	int GetFishingExtraType( int iEtcType );

	int GetFishingRodType();
	int GetFishingBaitType();

	bool HasEtcItem( int iEtcType );

	int ChanceMortmainCharEventLotto( int &iMortmainCnt );

public:
	void  SetEquipExcavating( bool bEquip, int iEtcItemType );

	bool  IsRealExcavating();
	DWORD GetExcavatingTime() const { return m_dwExcavatingTime; }
	void  SetExcavatingTime( DWORD dwExcavatingTime ) { m_dwExcavatingTime = dwExcavatingTime; }

	DWORD GetTryExcavatedTime() const { return m_dwTryExcavatedTime; }
	void  SetTryExcavatedTime(DWORD dwTryExcavatedTime) { m_dwTryExcavatedTime = dwTryExcavatedTime; }

	int  GetExcavationLevel();
	int  GetExcavationExp();
	bool AddExcavationExp( int iExp );

	void SetAllCharAllDecoration();

	void SetDBAgentID( DWORD dwDBAgentID ) { m_dwDBAgentID = dwDBAgentID; }


	// 2018-01-15 by bckim, 탐사 확장
public: //탐사
	void InitExcavtionInfo();
	void SendExcavationErrorPacket(int iType, int iErrorType, BOOL bBroadCast = FALSE, int value1 = 0, int value2 = 0);
	void EquipExcavationKit(SP2Packet& rkPacket);
	void ReleaseExcavationKit(SP2Packet& rkPacket);
	void StartExcavation(SP2Packet& rkPacket);
	void ExcuteExcavation();
	void ReappraiseExcavationReward(SP2Packet& rkPacket);
	void EndExcavation();
	void RechargeExcavationKit(SP2Packet& rkPacket);
	void UseRechargeKit();
	void TimeRechargeExcavationKit(SP2Packet& rkPacket);
	void ExcavationReaminingOperation(int value1);
	void SendExcavationSuccessInfo(BOOL bSuccess);
	void GetResultAddExcavationExp(int& iAfterLevel, int& iAfterExp, int iAddExp);
	void SetExcavationInfo(int iLevel, int iExp);
	void ExtendFishInventory(int iSlot);

	BOOL GiveExcavationReward();
	void OnExcavationCommand_Ex( SP2Packet &rkPacket );
	void SetEquipExcavating_Ex( bool bEquip);
	// End. 2018-01-15 by bckim, 탐사 확장

public:
	void DeleteMedalItemPassedDate( bool bImmediately );
	void DeleteCustomMedalItemPassedDate( bool bImmediately );
	bool SendBillingAddMileage( int iPresentType, int iPresentValue1, int iPresentValue2, int iResellPeso, bool bPresent );

protected:
	void EquipSlotItem( SP2Packet &rkPacket );
	void ReleaseSlotItem( SP2Packet &rkPacket );

	void SendBilligLogOut();
	void SavePlayTimeLog();
public:
	inline void SetNeedSendPushStruct( bool bNeed ) { m_bNeedSendPushStruct = bNeed; }
	inline bool IsNeedSendPushStruct() const { return m_bNeedSendPushStruct; }

	inline void SetNeedSendPushStructIndex( int iIndex ) { m_iNeedSendPushStructMaxIndex = iIndex; }
	inline int GetNeedSendPushStructIndex() const { return m_iNeedSendPushStructMaxIndex; }

	inline void SetSendPushStructIndex( int iIndex ) { m_iSendPushStructIndex = iIndex; }
	inline int GetSendPushStructIndex() const { return m_iSendPushStructIndex; }

	inline void SetSendPushStructCheckTime( DWORD dwTime ) { m_dwSendPushStructCheckTime = dwTime; }
	inline DWORD GetSendPushStructCheckTime() const { return m_dwSendPushStructCheckTime; }

public:
	void AddPresentMemory( const ioHashString &szSendName, short iPresentType, int iPresentValue1, int iPresentValue2, int iPresentValue3, 
						   int iPresentValue4, short iPresentMent, CTime &rkLimitTime, short iPresentState );
	void SendPresentMemory();
	void LogoutMemoryPresentInsert();     // 유저 로그 아웃시에 저장

	// 청약
	void AddSubscriptionMemory( const ioHashString &szSubscriptionID, int iSubscriptionGold,
								short iPresentType, int iPresentValue1, int iPresentValue2,
								CTime &rkLimitTime );
	void SendSubscriptionMemory();
	void LogoutMemorySubscriptionInsert();		// 유저 로그 아웃시에 저장
public: //세션서버 관련
	void SendSessionLogin();
	void SendSessionLogout();

public: // InsertPlayTimeLOG
	void InsertLoginRecord(int recordType = 0);//LogDBClient::RCT_LOGIN
	void InsertLoginPcRoom(int recordType = 0);
	void InsertLogoutRecord();

public:
	bool IsShutDownUser() const { return m_bShutDownUser; }
	void SetShutDownUser(bool bShutDownUser) { m_bShutDownUser = bShutDownUser; }

	ioUserSelectShutDown &GetUserSelectShutDown();

public:
	DWORD GetFriendRecommendTableIndex(){ return m_FriendRecommendData.m_dwTableIndex; }
	
	bool GetPeerIP(char* remoteIP, const int size, int& remotePort); // 임시

	void EventProcessTime();

public:
	void UpdateRelativeGrade( DWORD dwUniqueCode );

public: //kyg Subscription 관련 
	void SetSubscriptionRetract(const DWORD dwIndex, const ioHashString& szSubscriptionID, int iGold, SP2Packet& kPacket, bool bDefaultGold=false );
	void SetRetractGold( const DWORD dwIndex, const ioHashString& szSubscriptionID, int iGold );
	
	int GetSubscriptionGold( const DWORD dwIndex, const ioHashString& szSubscriptionID );
	int GetRetractGold( const DWORD dwIndex, const ioHashString& szSubscriptionID );
	int GetDBRetractGold( const DWORD dwIndex, const ioHashString& szSubscriptionID );

public:
	void SendShutDown(SP2Packet& rkPacket);
	void SendSelectUserShutDown(SP2Packet& rkPacket);
	void OnNexonPCRoom(SP2Packet& rkPacket);

public:
	void OnPcRoomCharCreate( SP2Packet& rkPacket );

	ioCharacter* GetPCRoomChar( int iClassType );
	ioCharacter* CreatePCRoomChar( int iClassType );

	void AllocPCRoomChar( const PcRoomCharInfoVec& InfoVec );

	void DeletePCRoomCharBySlot();
	void DeleteExpiredChar( const PcRoomCharInfoVec& InfoVec );
	
public:
	void OnAttedanceCheck( SP2Packet& rkPacket );

public:
	void ClearSelectGashapon();
	const SelectGashaponValueVec& GetSelectGashapon();

	void OnBuySelectGashapon( SP2Packet &rkPacket );

//캐릭터 각성
protected:
	std::map < int, int > m_CharAwakeDataMap; // < 캐릭터Index, 종료 시간 >

public:
	void EraseCharAwakeDataMap( std::map < int, int >::iterator &iter, std::map< int, BYTE > &mAwakePassedDateMap );

	int GetCalcCharAwakeEndDate( int iAddDay );
	int GetCalcCharAwakeExtendDate( int iAddDay, int iEndDate );

	void SendAwakeFailPacket( BYTE chFailType );
	void SendAwakeExtendFailPacket( BYTE chFailType );

	void DeleteCharAwakePassedDate( );
	//void DeleteLoginAwakePassedDate( CHARACTER &char_data, const int iCharIndex, std::map<int, BYTE> & mAwakeMap);
	void SendCharAwakeEndPacket( std::map< int, BYTE > &mAwakeMap );

	void OnCharAwake( SP2Packet &rkPacket ); 
	void OnCharAwakeExtend( SP2Packet &rkPacket );

	void InsertAwakeDataMap( int iCharIndex, int iAwakeEndDate );

	bool CheckAwakePassedDate( int iAwakeEndDate ); 
	int GetINTtypeCurDate();

	//////Test
	void OnTestAwakeTimeSet( SP2Packet &rkPacket ); 

#if defined( SRC_OVERSEAS )
	// 일반 가챠 패키지 전부 보내기 (매크로)		JCLEE 140718
	void OnGetGashaAll( SP2Packet &rkPacket ); 
#endif

public:
	bool IsRightAwakeChange( const int iCharArray, const int iAwakeType );
	bool CharAwakeErrorCheck( int iCharArray, int iAwakeType, int iAwakeDay, int &iCode, int &iNeedCount );
	bool CharAwakeExtendErrorCheck( int iCharArray, int iAwakeDay, int &iMaterialCode, int &iMaterialNeedCount );
public:
	inline int GetUserIPType() { return m_iUserIPType; }

public:
	void AfterLoginProcess( bool bMove=false );
	void BeforeLogoutProcess();

public:
	//서버 이동시 100개씩 테이블 행 전송하기 위해서 호출( 한번에 치장 2천개씩 전송 )
	int GetDecoPageCount( );
	int GetExtraItemPageCount();

	//클라 상에서 안보이는 유저 정보 전송
	void OnPlayingUserDataInfo( SP2Packet &rkPacket ); 
	void FillUserCharInfo( SP2Packet &kPacket );

	void ModeExitRoom(bool bMovePenalty, int iPenaltyPeso);
	void SetRoomEntryType( bool bServerMove, BYTE& byEntryType );

	void LeaveAllRoom();

	//recv 시간 검증
	bool IsEnableInterval(const int ID, const DWORD dwInterval);

	//용병강화
	void TakeOffCharExtraItem(ioCharacter* pCharacter, int iEquipType);	//장착중인 아이템 해제

	void CustomItemSkinDelete(int iSlotIndex, BYTE byDeleteType);
	void PowerUpItemSkinDelete( ioUserExtraItem::EXTRAITEMSLOT& kSlot );
	
	//코스튬
	void OnCostumeBuy(SP2Packet &rkPacket );
	void OnCostumeChange( SP2Packet &rkPacket );
	void OnCostumeSell( SP2Packet &rkPacket );
	void OnCostumeDisassemble( SP2Packet &rkPacket );
	void DeleteCostumePassedDate();
	ioCharacter* FindCostumeEquipChar(int iCostumeIndex);
	void AddCostumeItem(int iIndex, DWORD dwCode, int iPeriodType, SYSTEMTIME &sysTime);
	void AddCostumeItem(int iIndex, DWORD dwCode, int iPeriodType, int iValue1, int iValue2);

	//악세사리
	void OnAccessorySell( SP2Packet &rkPacket );
	void OnAccessoryChange(SP2Packet &rkPacket );
	void AddAccessoryItem(int iIndex, DWORD dwCode, int iPeriodType, SYSTEMTIME &sysTime);
	void AddAccessoryItem(int iIndex, DWORD dwCode, int iPeriodType, int iValue1, int iValue2, int iValue3);
	void ReleaseAccessoryPassedDate();
	ioCharacter* FindAccessoryEquipChar(int iAccessoryIndex);

	void OnDecoSell( SP2Packet &rkPacket );

	//구매
	void CashItemBuyProcess(int iITemType, IntVec& vDataValue, DWORD dwPacketID);
	void PesoItemBuyProcess(int iITemType, int iCode, int iArray, DWORD dwPacketID);

	void BuyCostumeWithPeso(int iCode, int iPeso, int iPeriod );

	int GetItemCashValue( int iItemType, IntVec& vDataValue );
	int GetItemBuyPesoValue( int iItemType, int iCode, int iArray );
	int GetItemBuyPeriodValue( int iItemType, int iCode, int iArray );

	int GetOutPutType(int iItemType);
	bool SetItemBuyVariableValue(int iItemType, IntVec& vData, SP2Packet &rkPacket);

	//특별 상품 구매 처리
	void SendSpeialGoodsBuyResult(int iResult, int iItemType);
	void SpecialGoodsBuy(DWORD dwEtcType);
	void SpecialGoodsPresent(const int iPresentType, const int iBuyValue1, const int iBuyValue2, ioHashString szReceiverPublicID);

	void _OnSpecialGoodsBuy(int iBuyState, SP2Packet &rkPacket );
	bool SpecialGoodsCashProcess(int iItemType);
	void OnSpecialShopGoodsList(SP2Packet &rkPacket );

	void PresendCashProcess(const int iPresentType, const int iBuyValue1, const int iBuyValue2, ioHashString szReceiverPublicID);

	//미션 정보
	void OnMissionInfoRequest( SP2Packet &kPacket );
	void OnMissionCompensationRecv( SP2Packet &kPacket );
	void OnMissionTimeCheck( SP2Packet &kPacket );
	DWORD GetUserLoginTime() { return (DWORD)m_user_data.m_login_time.GetTime(); }
	void DoAdditiveMission(int iCount, int iUseType);
	
	void OnTestMissionSetDate( SP2Packet &kPacket );
	void OnTestMissionSetValue( SP2Packet &kPacket );

	//출석부
	void SetUserRollBookType(int iType);
	void OnRollBookRenewal( SP2Packet &rkPacket );
	void OnTestRollBookProgress( SP2Packet &rkPacket );

	//길드 출석
	void OnGuildAttend( SP2Packet &rkPacket );
	void OnRecvGuildAttendReward(SP2Packet &rkPacket);
	void SendGuildAttendReward(int iCount);
	void GetAttendStandardDate(SYSTEMTIME& sYesterDay, SYSTEMTIME& sToday);
	void OnRenewalGuildMemberAttendInfo(SP2Packet &rkPacket);

	//소지 하고 있는지 아이템인지 체크
	bool DoHaveAItem(const int iType, const DWORD dwCode, bool bPermanentItem, int iSearchingType);
	bool IsExistMedalItem(const DWORD dwCode, bool bPermanentItem, int iSearchingType);
	bool IsExistCostumeItem(const DWORD dwIndex, bool bPermanentItem, int iSearchingType);
	bool IsExistExtraItem(const DWORD dwIndex, bool bPermanentItem, int iSearchingType);
	bool IsExistMedalItemAsCount(const DWORD dwCode,  bool bPermanentItem, int iSearchingType, int iCount);

	bool DeleteItem(int iType, DWORD dwCode);

	void OnDaumAdultCheckRsp(SP2Packet &rkPacket );

	void LogCashItemBuyInfo(const int iItemPrice, const int iPreTotalCash, const int iPreRealCash, const int iCurTotalCash, const int iCurRealCash, const int iItemType, const int iItemCode, const int iItemCount);

public:
	User( SOCKET s=INVALID_SOCKET, DWORD dwSendBufSize=0, DWORD dwRecvBufSize=0 );
	virtual ~User();

public:
	// 라틴용 기간제 용병을 형식 변경 전에 값으로 저장해두는 변수 (DB 저장용)	JCLEE 140416
//	int	GetLimitSecForLatin(){ return m_iLimitSecForLatin; }

	

	//popup store
	bool m_bOnPopup;
	DWORD m_dwTotalMoney;
	DWORD m_dwMonthMoney;
	DWORD m_dwTotalPlayTime;
	std::vector<int> m_vecUsePopupIndex;	// 구매한 팝업 리스트
	std::vector<int> m_vecSendPopupIndex;		// 보낸 팝업 리스트
	
	//int m_iSendPopupIndex;
	void OnConnectPopupProcess();
	void CheckPopupStoreIndex();
	void SetUserSpentMoney( DWORD dwTotal, DWORD dwMonth, DWORD dwTotalPlayTime );
	void SetUserPopupIndex( std::vector<int>& vecPopupIndex );

	void AddUsePopupIndex( int iBuyPopupIndex );
	void OnPopupItemBuy(SP2Packet &rkPacket );


	//Cumulative Sale 
	struct CumulativeData 
	{
		int iType;
		DWORD dwItemCode;
		int	iBuyCnt;
		CumulativeData()
		{
			iType		= 0;
			dwItemCode	= 0;
			iBuyCnt		= 0;
		}
	}typedef CUMULATIVE_DATA;
	std::vector<CUMULATIVE_DATA> m_vecCumulativeBuyCnt;

	int GetCumulativeBuyCnt( int iType, DWORD dwCode );
	void UpdateCumulativeSaleItemCnt(int iType, DWORD dwCode );
	int GetCumulativeSaleItemDCPrice( int iType, DWORD dwCode, int iBuyCash );
	void DeleteCumulativeBuyCnt();
	
	// 몬스터 코인 치팅 방지 변수
	bool m_bUsedCoin;

	// rising gashapon
	int m_nRisingBuyCount;
	std::vector<int> m_vRisingGetIndex;
	void OnBuyRisingGashapon(SP2Packet &rkPacket );
	void OnInitRisingGashapon(SP2Packet &rkPacket );
	
	void InitRisingBuyCount() { m_nRisingBuyCount = 0; }
	int GetRisingBuyCount() const { return m_nRisingBuyCount; }
	void IncreaseRisingBuyCount() { m_nRisingBuyCount++; }

	std::vector<int> GetRisingGetIndex() const { return m_vRisingGetIndex; }
	void AddRisingGetIndex(int index) { m_vRisingGetIndex.push_back(index); IncreaseRisingBuyCount(); }
	void InitRisingGetIndex() { m_vRisingGetIndex.clear(); InitRisingBuyCount(); }

	int GetCountRSoldier();

	bool IsRSoldier(ioCharacter *pChar);

	void OnBillingTimeCashResult(SP2Packet &rkPacket );
	void _OnBillingPresentCash(SP2Packet &rkPacket);

	int GetCountOfSpecialSoldier(int iSpecialType);
	bool IsSpecialSoldier(ioCharacter *pChar, int iSpecialType, BOOL bCheckMortmain = TRUE );
	int GetSpecialSoldierType(int iClassType);
	int GetStartNumOfSpecialSoldier(int iSpecialType);
	int GetEndNumOfSpecialSoldier(int iSpecialType);

public:
	void EnterGuildRoom(const DWORD dwRoomIndex, const bool bInvite);

	void OnEnterGuildRoom(SP2Packet &kPacket );
	void OnConstructBlock(SP2Packet &kPacket );
	void OnRetrieveBlock(SP2Packet &kPacket );
	
	void ResultGuildRoomReq(SP2Packet &kPacket );

	void CreateGuildRoom(DWORD dwMapIndex);
	void SetPlazaInfoWithGuild(int& iMaxPlayer, int& iPlazaType);

	void OnConstructMode( SP2Packet &rkPacket );
	void OnSendGuildInvenInfo( SP2Packet &rkPacket );

	void OnTestCheckHousingInfo( SP2Packet &rkPacket );

	void SetFisheryBlockInfo(int val) { m_dwFisheryCode = val; }
	DWORD GetGuildFisheryCode() { return m_dwFisheryCode; }

	void ActiveUserGuildRoom();

	//개인 본부
	void OnJoinPersonalHQ(SP2Packet &rkPacket);
	void _OnJoinPersonalHQ( UserParent *pRequestUser, int iMapIndex, bool bInvited );
	
	void OnPersonalHQInviteList( SP2Packet &rkPacket );

	void OnPersonalHQJoinAgree( SP2Packet &rkPacket );
	void _OnPersonalHQJoinAgree( DWORD dwRoomIndex );

	void OnPersonalHQInfo( SP2Packet &rkPacket );
	void _OnPersonalHQInfo( UserParent *pRequestUser );

	void OnPersonalHQCommand( SP2Packet &rkPacket );

	void GuildConstructMode(BYTE byType);
	void PersonalHQConstructMode(BYTE byType);

	void GuildConstructBlock(SP2Packet &kPacket);
	void PersonalHQConstructBlock(SP2Packet &kPacket);

	void GuildRetrieveBlock(SP2Packet &kPacket);
	void PersonalHQRetrieveBlock(SP2Packet &kPacket);

	void AddPersonalInvenItem(const DWORD dwItemCode, const int iCount);
	void DecreasePersonalInvenItem(const DWORD dwItemCode, const int iCount);

	void OnReqPersonalHQInvenData(SP2Packet &kPacket);
	//기간제 캐쉬 박스
	void LoginSelectCashTable(CQueryResultData *query_data);
	void InsertCashTable(CQueryResultData *query_data);
	void UpdateCashTable(const DWORD dwCode, const int iResult, const DWORD dwReceiveDate, ioHashString& szBillingGUID);
	void RequestTimeCash(const DWORD dwCode, ioHashString& szBillingGUID, BOOL bFirst);
	
	//토너먼트 메크로
	void OnTestTournamentMacro(SP2Packet &rkPacket);
	//칭호
	void UpdateUserTitle(const DWORD dwCode, const __int64 iValue, const int iLevel, const BYTE byPremium, const BYTE byEquip, const BYTE byStatus, const BYTE byActionType);
	void UpdateUserTitleStatus();
	void SelectUserTitle(CQueryResultData *query_data);
	void OnTitleChange(SP2Packet &rkPacket);
	void OnSyncTimeEquipTitle(SP2Packet &rkPacket);
	void OnNewTitleConfirm(SP2Packet &rkPacket);
	bool FillEquipTitleData(SP2Packet &rkPacket);

	//여성화
	void GiveWomanGender(int iClassType);

	void DeleteExpiredPcRoomChar(int iClassType);
	
	
	
	//보너스 캐쉬
	void DeleteExpiredBonusCash();
	int GetAmountOfBonusCash();
	void InsertUserBonusCash(const DWORD dwIndex, const int iAmount, const DWORD dwExpirationDate);
	void UpdateUserBonusCash(BonusCashUpdateType eType, const DWORD dwStatus, const DWORD dwCashIndex, const int iAmount, const int iUsedAmount, const int iType, const int iValue1, const int iValue2, const char *szBillingGUID);
	void GivePCRoomRodAndBait();
	
	// 거래소 아이템 리스트 요청
	void ReqTradeItemList( SP2Packet &kPacket );

	// 오크통 개선
	// 오크통 게임 활성화/비활성화 flag
	bool GetOakBarrelOpen();

	// DB로 부터 오크통 정보를 받아와 메모리 저장.
	void SetOakBarrelData( BYTE byStep, BYTE byHole[OAK_BARREL_HOLE], DWORD dwTime, int iLimitSword, bool bInit = false );
	// DB로 부터 오크통 초기화 시간값을 받아와 메모리 저장
	void SetOakBarrelTimeData( DWORD dwTime );

	// S->C 로그인 시 오크통 정보 전송
	void SendOakBarrelData();
	// C->S 클라로 부터 받은 칼 사용 요청 패킷
	void OnOakBarrelUseSwordReq( SP2Packet &rkPacket );	
	// S->C 칼 사용 결과에대한 패킷
	void SendOakBarrelResult( bool bSuccess, BYTE byStep = 0 );
	// C->S 오크통 보상 받기
	void OnOakBarrelReward( SP2Packet &rkPacket );
	// S->C 오크통 보상 받기
	void SendOakBarrelReward( BYTE byStep );

	// 오크통 칼 일일 한도 수량 리필
	void CheckRefillLimitSword();

	// 오크통 칼 수량 by ETCItem
	int GetOakSword();

	// 오크통 칼 수량 증가
	void AddEtcOakSword( int iSword );

	// 오크통 진행 단계
	BYTE GetOakBarrelStep();

	// 오크통 칼 사용 일일 한도 수량
	void SetOakBarrelLimitSword( int iLimitSword );
	int GetOakBarrelLimitSword();

	void SetUserMatch( int iPlayCount, int iPoint, int iWinCount, int iMaxWinCount, int iMaxLoseCount, int iRankMMR );

	// 2018-06-18 by bckim, 카드 짝 마추기 이벤트 추가
	void CardMatching_GetInfo (SP2Packet &rkPacket );				// 처음 아이템 사용 및 기본 정보 셋팅 
	void CardMatching_StartSet (SP2Packet &rkPacket );				// 처음 아이템 사용 및 기본 정보 셋팅 
	void CardMatching_ConfirmCard_Single(SP2Packet &rkPacket );	
	void CardMatching_ConfirmCard (SP2Packet &rkPacket );			
	void CardMatching_End_Request (SP2Packet &rkPacket );			
	bool IsCardMatingState();	
	void CardMatching_TickProcess ();	
	// End. 2018-06-18 by bckim, 카드 짝 마추기 이벤트 추가

	// 2018-07-25 by bckim, 아레나 모드 추가
	void InitArenaModeCharList();		
	void Arena_Mode_CharCreate();	
	void Arena_Mode_CharDelete();
	ioCharacter* CreateArenaModeChar( int iClassType , BYTE byReinforceGrade );
	bool IsHaveArenaChar();
	// End. 2018-07-25 by bckim, 아레나 모드 추가

	// 2018-08-30 by bckim, 주사위 이벤트 추가
	bool GetProgressStateDice();
	void SetProgressStateDice( bool bFlag);
	//void SetDiceGameData( int iPosition, int iTrace[DICE_GAME_TRACE_DB], BYTE byBoradIndex, int iRewardIndex[DICE_GAME_REWARD_DB]);
	void SetDiceGameData( int iPosition, int *pArrayTrace, BYTE byBoradIndex, int *pArrayRewardIndex);

	void SendDiceGameData();
	void SendDiceGameData_Re();
	void Dice_Game_start(SP2Packet &rkPacket);
	void Dice_Game_ReStart(SP2Packet &rkPacket);
	void Dice_Game_throwDice(SP2Packet &rkPacket);
	int  RollDice();
	void Dice_Game_ExchangeRewardList(SP2Packet &rkPacket);
	void Dice_Game_ExchangeBoardList(SP2Packet &rkPacket);
	void Dice_Game_Complete_Done();
	void Dice_Game_End();
	// End. 2018-08-30 by bckim, 주사위 이벤트 추가

	// 일대일모드
	void OnSuccessionMatchRequest();
	void OnSuccessionMatchCancel();
	int  GetSuccessionMMR()				{ return m_UserMatch.GetMMR(); }
	int  GetSuccessionWinCount()		{ return m_UserMatch.GetWinCount(); }
	int  GetSuccessionMaxWinCount()		{ return m_UserMatch.GetMaxWinCount(); }
	void SuccessionModeWin()			{ m_UserMatch.SetWin(); }
	void SuccessionModeLose()			{ m_UserMatch.SetLose(); }
	void AddSuccessionPlayCount()		{ m_UserMatch.AddPlayCount(); }

	void AddSuccessionMMR( int iPoint )	{ m_UserMatch.AddMatchPoint( iPoint ); }
	void DelSuccessionMMR( int iPoint )	{ m_UserMatch.DelMatchPoint( iPoint ); }

	void AddSuccessionRankMMR( int iPoint )	{ m_UserMatch.AddRankMatchPoint( iPoint ); }
	void DelSuccessionRankMMR( int iPoint )	{ m_UserMatch.DelRankMatchPoint( iPoint ); }

	void SaveSuccessionData()			{ m_UserMatch.SaveData(); }
	void SendSuccessionData()			{ m_UserMatch.SendUserMatch(); }

	void SendSuccessionMatchCancel( bool bLogOut );

	void OnAccessoryCompose( SP2Packet &rkPacket );
	void OnAccessoryReinforce( SP2Packet &rkPacket );
	void OnExtraItemReinforce( SP2Packet &rkPacket );

	void OnSpiritCompose( SP2Packet &rkPacket );
	void OnSpiritDecompose( SP2Packet &rkPacket );
	void OnSpiritConversion( SP2Packet &rkPacket );
	void OnSpiritSell( SP2Packet &rkPacket );
	void SetModeRewardSpirit( int iSpiritCode, int iQuantity, ModeType eType );

	// 커스텀 메달
	int AddCustomMedalData( int iIndex, int iSlotIndex, int iCode );
	void OnSellCustomMedal( SP2Packet &rkPacket );
	void OnChangeCustomMedalState( SP2Packet &rkPacket );

	// 타임게이트
	void OnTimeGateRequest( SP2Packet &rkPacket );
	BOOL IsTimeGateOpen();

	void OnChangeCharCheck( SP2Packet &rkPacket );
	BOOL CheckUserHaveThisExtraItem(ioCharacter *rkChar, int iSlot, int iItemCode, int iItemReinforceLevel);
	BOOL CompareEquippedExtraItemInChangeCharCheck(ioCharacter *rkChar, int iSlot, int iItemCode, int iItemReinforceLevel);

public:
	BOOL IsTutorialUser() { return bTutorial; } 
	void CreateTutorialChar();

protected:
	//유영재 튜토리얼
	BOOL bTutorial;


	// 유저코인 관련 유저코인으로 레이드코인(티켓)관리함.
	ioUserCoin * GetUserCoin();
public:
	// 레이드 티켓 관련

	bool GetLastRaidTicketTime(CTime & outLastRefillTime);
	void UpdateRaidTicketTime();
	void CheckRefillRaidTicket(bool bLogin);
	int AddEtcRaidTicket(int iAddTicket);
	int GetRaidTicket();

public:
	// 좋아함
	void SendPresentTimeCash(int iType, int iValue );
	void SendCustomMedalData( CQueryResultData *query_data );
	void SendCustomMedalDataToClient( int iUserIndex, int iItemCode, int iPresentIndex, int iPresentSlotIndex, int iItemIndex );

public:

	void FillUserNetworkInfo( SP2Packet &rkPacket );
	void SetStealth( bool bStealth ) { m_bStealth = bStealth; };

	ioItem *m_pPreItem;		// 2018-03-15 by bckim, 깃발모드 추가	
};

#endif // !defined(AFX_USER_H__03E2797C_74A5_433B_B34D_2E60C0784CEF__INCLUDED_)
