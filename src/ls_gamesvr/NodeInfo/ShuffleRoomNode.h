#ifndef _ShuffleRoomNode_h_
#define _ShuffleRoomNode_h_

#include "UserParent.h"
#include "ShuffleRoomParent.h"
#include "../DataBase/LogDBClient.h"

class Room;
class ShuffleRoomNode;
class Mode;

class ShuffleRoomSync
{
public:
	enum
	{
		/* 순서 중요 */
		/* 큰값을 대기중이라면 작은 값은 무시한다. */
		/* 값이 큰 정보가 작은 값의 정보를 포함한다.*/
		BRS_PLAY		= 0,
		BRS_CHANGEINFO	= 1,
		BRS_CREATE		= 2,
		BRS_DESTROY		= 3,
		MAX_BRS,
	};

protected:
	ShuffleRoomNode *m_pCreator;

protected:
	DWORD m_dwUpdateTime;
	DWORD m_dwUpdateType;
	DWORD m_dwCheckTime[MAX_BRS];

public:
	void Update( DWORD dwUpdateType );
	void Process();

public:
	void SetCreator( ShuffleRoomNode *pCreator );
	void Init();

public:
	ShuffleRoomSync();
	virtual ~ShuffleRoomSync();
};

typedef struct tagShuffleRoomUser
{
	DWORD		 m_dwUserIndex;
	ioHashString m_szPublicID;
	int          m_iGradeLevel;
	int          m_iAbilityLevel;

	// 음성 채팅 UDP 정보
	ioHashString m_szPublicIP;
	ioHashString m_szPrivateIP;
	ioHashString m_szTransferIP;
	int          m_iClientPort;
	int          m_iTransferPort;

	// 전투룸 입장 후 부터 사용
	TeamType     m_eTeamType;

	// 로그를 위한 변수
	int          m_iServerRelayCount;
	tagShuffleRoomUser()
	{
		m_dwUserIndex	= 0;
		m_iGradeLevel	= 0;
		m_iAbilityLevel	= 0;
		m_iClientPort	= 0;
		m_iTransferPort = 0;
		m_eTeamType     = TEAM_NONE;
		m_iServerRelayCount = 0;
	}

	const bool IsNATUser() const
	{
		if( strcmp( m_szPublicIP.c_str(), m_szPrivateIP.c_str() ) == 0 )
			return false;
		return true;
	}
}ShuffleRoomUser;
typedef vector< ShuffleRoomUser > vShuffleRoomUser;
typedef vShuffleRoomUser::iterator vShuffleRoomUser_iter;

class ShuffleRoomUserSort
{
public:
	bool operator()( const ShuffleRoomUser &lhs , const ShuffleRoomUser &rhs ) const
	{
		if( lhs.m_iGradeLevel > rhs.m_iGradeLevel )
		{
			return true;
		}
		else if( lhs.m_iGradeLevel == rhs.m_iGradeLevel )
		{
			int iCmpValue = _stricmp( lhs.m_szPublicID.c_str(), rhs.m_szPublicID.c_str() );	
			if( iCmpValue < 0 )
				return true;
		}
		return false;
	}
};

struct ShuffleRoomInfo
{
	int m_iModeIdx;
	int m_iSubIdx;
	int m_iMapIdx;
	ShuffleRoomInfo()
	{
		m_iModeIdx= -1;
		m_iSubIdx = -1;
		m_iMapIdx = -1;
	}
	ShuffleRoomInfo( int iMode, int iSub, int iMap )
	{
		m_iModeIdx= iMode;
		m_iSubIdx = iSub;
		m_iMapIdx = iMap;
	}
};
typedef std::vector<ShuffleRoomInfo> vShuffleRoomInfo;

class ShuffleRoomNode : public ShuffleRoomParent
{
protected:
	friend class ShuffleRoomSync;
	ShuffleRoomSync m_NodeSync;
	void SyncPlay();
	void SyncChangeInfo();
	void SyncCreate();
	void SyncDestroy();

protected:
	DWORD		   m_dwIndex;	

protected:
	vShuffleRoomUser m_vUserNode;

	ioHashString m_szRoomName;
	ioHashString m_OwnerUserID;
	int			 m_iMaxPlayerBlue; 
	int			 m_iMaxPlayerRed; 
	Room         *m_pShuffleRoom;

	// 승 패 연승
	int          m_iBlueWin;
	int          m_iBlueLose;
	int          m_iBlueVictories;
	int          m_iRedWin;
	int          m_iRedLose;
	int          m_iRedVictories;

	// 보스 모드 보스 유저
	ioHashString m_szBossName;

	// 강시 모드 최초 강시 유저
	ioHashString m_szGangsiName;

//	int    m_iCurShufflePhase; //parent class에 동일한 이름으로 정의되어 있음
	int    m_iMaxShufflePhase;

	vShuffleRoomInfo m_vShuffleModeList;

protected:
	struct PhaseRecord
	{
		float m_fContribtuePer;
		DWORD m_dwPlayTime;

		PhaseRecord()
		{
			m_fContribtuePer = 0;
			m_dwPlayTime     = 0;
		}
	};
	typedef std::map< int, PhaseRecord > PhaseRecordMap;

	struct ShuffleRecord
	{
		ioHashString m_szPublicID;
		int m_iUserIndex;		
		PhaseRecordMap m_PhaseRecordMap;

		ShuffleRecord()
		{
			m_iUserIndex  = 0;
			
		}
	};
	typedef std::vector<ShuffleRecord> vShuffleRecord;
	vShuffleRecord m_vShuffleRecord;

protected:
	struct PlayPointInfo
	{
		int iPlayPoint;
		int iWinningPoint;
		int iConsecutivePoint;
		int iAwardPoint;

		ioHashString szUserName;
	};
	typedef std::vector<PlayPointInfo> PlayPointInfoVec;
	PlayPointInfoVec m_vPlayPointInfoVec;

protected:
	DWORD m_dwReserveTime;

protected:
	void InitRecord();
	void InitData();
	void InitShuffleMode();
	
public:
	void OnCreate();
	void OnDestroy();

public:
	virtual const DWORD GetIndex(){ return m_dwIndex; }
	virtual void EnterUser( const DWORD dwUserIndex, const ioHashString &szPublicID, const int iGradeLevel, const int iAbilityLevel,
							const ioHashString &szPublicIP, const ioHashString &szPrivateIP, const ioHashString &szTransferIP, const int iClientPort, const int iTransferPort );
	virtual bool LeaveUser( const DWORD dwUserIndex, const ioHashString &szPublicID );
	virtual bool ReJoinTry( const DWORD dwUserIndex, const ioHashString &szPublicID );
	virtual void SendPacketTcp( SP2Packet &rkPacket, const DWORD dwUserIndex = 0 );
	virtual void SendPacketTcpUser( SP2Packet &rkPacket, const ioHashString &rkSenderName );
	virtual void SendPacketUdp( SP2Packet &rkPacket, const DWORD dwUserIndex = 0 );
	virtual void UserInfoUpdate( const DWORD dwUserIndex, const int iGradeLevel, const int iAbilityLevel, const int iClientPort, const ioHashString &szTransferIP, const int iTransferPort );
	virtual void UserUDPChange( const DWORD dwUserIndex, const ioHashString &szPublicID, const ioHashString &szPublicIP, const int iClientPort,
								const ioHashString &szPrivateIP, const ioHashString &szTransferIP, const int iTransferPort );
	virtual void UserP2PRelayInfo( const DWORD dwUserIndex, const DWORD dwRelayUserIndex, bool bRelay );

	virtual const bool IsOriginal(){ return true; }

	virtual int	 GetJoinUserCnt() const;
	virtual int  GetPlayUserCnt();
	virtual int  GetMaxPlayer() const;
	virtual int  GetMaxPlayerBlue() const;
	virtual int  GetMaxPlayerRed() const;
	virtual ioHashString GetName() const { return m_szRoomName; }
	virtual ioHashString GetOwnerName() const { return m_OwnerUserID; }
	virtual bool IsShuffleModePlaying();

	virtual bool IsFull();
	virtual bool IsEmptyShuffleRoom();
	virtual int  GetPlayModeType();
	virtual int  GetAbilityMatchLevel();
	virtual int  GetRoomLevel();
	virtual int  GetSortLevelPoint( int iMyLevel );

public:
	virtual void SyncPlayEnd( bool bAutoStart );
	void SyncRealTimeCreate();
	
public:
	void ShuffleEnterRandomTeam();
	void ShuffleEnterRandomTeam( UserRankInfoList &rkUserRankInfoList );
	void CreateShuffle( SP2Packet &rkPacket );
	void CheckShuffleJoin( ShuffleRoomUser &kCheckUser );

	TeamType CreateTeamType();

protected:
	void AddUser( const ShuffleRoomUser &kUser );
	bool RemoveUser( const DWORD dwUserIndex );
	void SelectNewOwner();

public:
	bool KickOutUser( const ioHashString &rkName, BYTE eKickType = RoomParent::RLT_NORMAL );
	bool IsShuffleTeamChangeOK( TeamType eChangeTeam );

public:
	TeamType ChangeTeamType( const ioHashString &rkName, TeamType eTeam );

	void UpdateRecord( TeamType eWinTeam );
	bool IsChangeEnterSyncData();

public:
	void CreateBoss();
	const ioHashString &GetBossName();
	void ClearBossName();

public:
	void CreateGangsi();
	const ioHashString &GetGangsiName();
	void ClearGangsi();
	
public:
	void FillShuffleRoomInfo( SP2Packet &rkPacket );	
	void FillUserInfo( SP2Packet &rkPacket, const ShuffleRoomUser &rkUser );
	void FillSyncCreate( SP2Packet &rkPacket );
	void FillRecord( SP2Packet &rkPacket );

public:
	ShuffleRoomUser &GetUserNodeByIndex( DWORD dwUserIndex );
	const ShuffleRoomUser &GetUserNodeByArray( int iArray );
	UserParent *GetUserNode( const ioHashString &szPublicID );
	TeamType GetUserTeam( const ioHashString &szPublicID );
	int      GetUserTeamCount( TeamType eTeam );
	int      GetMaxPlayer( TeamType eTeam );

public:        //패킷 처리
	virtual bool OnProcessPacket( SP2Packet &rkPacket, UserParent *pUser );

protected:
	void OnMacroCommand( SP2Packet &rkPacket, UserParent *pUser );
	void OnVoiceInfo( SP2Packet &rkPacket, UserParent *pUser );
	void OnShuffleRoomCommand( SP2Packet &rkPacket, UserParent *pUser );		

public:
	void Process();

public:
	bool HasModeList();	
	bool ShuffleRoomReadyGo();
	void EnterShuffleRoom( Room *pRoom );
	bool IncreaseShufflePhase();
	void SetShufflePhaseEnd();
	bool IsShufflePhaseEnd();
	void RestartShuffleMode();

	void KickOutModeError();
	void KickOutHighLevelUser();
	void InitShuffleRecord();
	
	void AddModeContributeByShuffle( const ioHashString& szPublicID, int iUserIndex, float fContributePer, DWORD dwPlayTime );
	void AddModePlayTimeByShuffle( const ioHashString& szPublicID, int iUserIndex, float fContributePer, DWORD dwPlayTime );

	inline int GetShuffleRecordCnt() { return static_cast<int>(m_vShuffleRecord.size()); }

	//오늘의 모드 보상
	void FinalRoundhufflePlayPoint( Mode* pMode, ModeRecord* pRecord );
	
	//보너스 모드 보상
	void CheckShuffleReward( User *pUser, int iStarCnt, int& iStarByCalcBonusCount, float& iBonusPercent );

	void SendShuffleReward( User *pUser, int iCnt );

	bool IsShuffleRoom();

	void SendShufflePhase( User *pUser );
	void SendShufflePhaseAllUser();

public:
	int GetKillDeathLevelAverage( User *pUser );

public:
	bool IsReserveTimeOver();
	bool HasUser( const ioHashString& szPublicName );

public:
	ShuffleRoomNode( DWORD dwIndex = 1 );
	virtual ~ShuffleRoomNode();
};

#endif