#ifndef _LadderTeamNode_h_
#define _LadderTeamNode_h_

#include "UserParent.h"
#include "LadderTeamParent.h"
#include "../DataBase/LogDBClient.h"

#define MAX_LADDERTEAM_PLAYER           8

class Room;
class LadderTeamNode;
class LadderTeamSync
{
public:
	enum
	{
		/* 순서 중요 */
		/* 큰값을 대기중이라면 작은 값은 무시한다. */
		/* 값이 큰 정보가 작은 값의 정보를 포함한다.*/
		LTS_CHANGEINFO	= 0,
		LTS_CHANGERECORD= 1,
		LTS_CREATE		= 2,
		LTS_DESTROY		= 3,
		MAX_LTS,
	};

protected:
	LadderTeamNode *m_pCreator;

protected:
	DWORD m_dwUpdateTime;
	DWORD m_dwUpdateType;
	DWORD m_dwCheckTime[MAX_LTS];

public:
	void Update( DWORD dwUpdateType );
	void Process();

public:
	void SetCreator( LadderTeamNode *pCreator );
	void Init();

public:
	LadderTeamSync();
	virtual ~LadderTeamSync();
};

typedef struct tagLadderTeamUser
{
	DWORD		 m_dwUserIndex;
	ioHashString m_szPublicID;
	int          m_iGradeLevel;
	int          m_iAbilityLevel;
	int          m_iHeroMatchPoint;
	int          m_iLadderPoint;
	DWORD        m_dwGuildIndex;
	DWORD        m_dwGuildMark;

	// 음성 채팅 UDP 정보
	ioHashString m_szPublicIP;
	ioHashString m_szPrivateIP;
	ioHashString m_szTransferIP;
	int          m_iClientPort;
	int          m_iTransferPort;

	tagLadderTeamUser()
	{
		m_dwUserIndex	= 0;
		m_iGradeLevel	= 0;
		m_iAbilityLevel	= 0;
		m_iHeroMatchPoint    = 0;
		m_iLadderPoint  = 0;
		m_dwGuildIndex  = 0;
		m_dwGuildMark   = 0;
		m_iClientPort	= 0;
		m_iTransferPort = 0;
	}
}LadderTeamUser;
typedef vector< LadderTeamUser > vLadderTeamUser;
typedef vLadderTeamUser::iterator vLadderTeamUser_iter;
class LadderTeamUserSort
{
public:
	bool operator()( const LadderTeamUser &lhs , const LadderTeamUser &rhs ) const
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

class LadderTeamNode : public LadderTeamParent
{
protected:
	friend class LadderTeamSync;
	LadderTeamSync m_NodeSync;

	void SyncChangeInfo();
	void SyncChangeRecord();
	void SyncCreate();
	void SyncDestroy();

protected:
	DWORD		   m_dwIndex;	
	int            m_iCampType;
	Room          *m_pMatchRoom;

	// 매치 예약한 팀의 리스트
	struct MatchReserve
	{
		DWORD m_dwTeamIndex;
		DWORD m_dwReserveTime;
		MatchReserve()
		{
			m_dwTeamIndex = m_dwReserveTime = 0;
		}
	};
	typedef std::vector< MatchReserve > vMatchReserve;
	vMatchReserve m_MatchReserve;
	DWORD         m_dwMatchReserveIndex;
	
protected:
	vLadderTeamUser m_vUserNode;

	ioHashString   m_szTeamName;
	ioHashString   m_szTeamPW;
	ioHashString   m_OwnerUserID;
	DWORD          m_dwGuildIndex;
	DWORD          m_dwGuildMark;
	DWORD          m_dwJoinGuildIndex;

	int m_iMaxPlayer;
	int m_iSaveMaxPlayer;
	
	int m_iSelectMode;			//클라이언트에서 선택한 인덱스 값
	int m_iSelectMap;
	int	m_iPreSubType;
	int	m_iPreMapNum;
	ModeType m_PreModeType;

	bool m_bHeroMatchMode;
	// 승 - 무 - 패 - 연승 - 팀랭킹
	int m_iWinRecord;
	int m_iLoseRecord;
	int m_iVictoriesRecord;
	int m_iTeamRanking;

	//
	DWORD m_dwReserveTime;

	//
	DWORD m_dwLastMatchTeam;
	DWORD m_dwCurLastBattleTime;

	//Option
	bool m_bSearchLevelMatch;
	bool m_bSearchSameUser;
	bool m_bBadPingKick;

	//Search
	DWORD m_dwSearchMatchCurTime;
	int   m_iSearchMatchCurSec;
	int	  m_iSearchCount;
	
	//
	DWORD m_dwTeamState;

	void InitRecord();
	void InitData();	
	
public:
	void OnCreate();
	void OnDestroy();

public:
	virtual const DWORD GetIndex(){ return m_dwIndex; }
	virtual void EnterUser( const DWORD dwUserIndex, const ioHashString &szPublicID, const int iGradeLevel, const int iAbilityLevel, const int iHeroMatchPoint, const int iLadderPoint, const DWORD dwGuildIndex, const DWORD dwGuildMark,
							const ioHashString &szPublicIP, const ioHashString &szPrivateIP, const ioHashString &szTransferIP, const int iClientPort, const int iTransferPort );
	virtual bool LeaveUser( const DWORD dwUserIndex, const ioHashString &szPublicID );
	virtual void SendPacketTcp( SP2Packet &rkPacket, const DWORD dwUserIndex = 0 );
	virtual void SendPacketTcpUser( SP2Packet &rkPacket, const ioHashString &rkSenderName );
	virtual void SendPacketUdp( SP2Packet &rkPacket, const DWORD dwUserIndex = 0 );
	virtual void UserInfoUpdate( const DWORD dwUserIndex, const int iGradeLevel, const int iAbilityLevel, const int iHeroMatchPoint, const int iLadderPoint, const DWORD dwGuildIndex, const DWORD dwGuildMark, const int iClientPort, const ioHashString &szTransferIP, const int iTransferPort );
	virtual void UserUDPChange( const DWORD dwUserIndex, const ioHashString &szPublicID, const ioHashString &szPublicIP, const int iClientPort,
								const ioHashString &szPrivateIP, const ioHashString &szTransferIP, const int iTransferPort );

	virtual const bool IsOriginal(){ return true; }

	virtual int  GetCampType() const{ return m_iCampType; }
	virtual int	 GetJoinUserCnt() const;
	virtual int  GetMaxPlayer() const;
	virtual ioHashString GetTeamName() const { return m_szTeamName; }
	virtual ioHashString GetPW() const { return m_szTeamPW; }
	virtual DWORD GetGuildIndex() const;
	virtual DWORD GetGuildMark() const;
	virtual DWORD GetTeamState() const { return m_dwTeamState; }
	virtual bool  IsGuildTeam() const;
	virtual int   GetTeamRanking() const { return m_iTeamRanking; }
	virtual DWORD GetJoinGuildIndex() const { return m_dwJoinGuildIndex; }
	virtual bool IsFull() const;
	virtual bool IsEmptyUser();
	virtual bool IsSearchLevelMatch();
	virtual bool IsSearchSameUser();
	virtual bool IsBadPingKick();
	virtual bool IsSearching();
	virtual bool IsMatchPlay();
	virtual bool IsHeroMatchMode();

	virtual int  GetAbilityMatchLevel();
	virtual int  GetHeroMatchPoint();
	virtual int  GetTeamLevel();
	virtual int  GetWinRecord();
	virtual int  GetLoseRecord();
	virtual int  GetVictoriesRecord();
	virtual int  GetSelectMode() const;
	virtual int  GetSelectMap() const;
	virtual int  GetRankingPoint();
	virtual void UpdateRecord( TeamType ePlayTeam, TeamType eWinTeam );
	virtual void UpdateRanking( int iTeamRanking );	
	virtual void SetTeamState( DWORD dwState );

	// 매칭 & 룸 생성
	virtual void MatchRoomRequest( DWORD dwRequestIndex );
	virtual void MatchRoomRequestJoin( DWORD dwOtherTeamIndex, ModeType eModeType, int iModeSubNum, int iModeMapNum, int iCampType, int iTeamType );
	virtual void MatchReserveCancel();
	virtual void MatchPlayEndSync();

public:
	void SetCampType( int iCampType );
	void SetTeamName( const ioHashString &rkName );
	void SetTeamPW( const ioHashString &rkPW );
	void SetMaxPlayer( int iMaxPlayer );
	void SetSaveMaxPlayer( int iMaxPlayer );
	void SetJoinGuildIndex( DWORD dwJoinGuildIndex );
	void SelectMode( const int iModeIndex );
	void SelectMap( const int iMapIndex );
	void SetHeroMatchMode( bool bHeroMatchMode );

public:
	ioHashString GetOwnerName() const { return m_OwnerUserID; }
	void SetPreSelectModeInfo( const ModeType eModeType, const int iSubType, const int iMapNum );
	void GetPreSelectModeInfo( ModeType &eModeType, int &iSubType, int &iMapNum );
	bool IsReserveTimeOver();
	inline int GetSearchCount() { return m_iSearchCount; }
	inline void IncreaseSearchCount() { m_iSearchCount++; }
	inline void InitSearchCount() { m_iSearchCount = 0; }

public:
	void SetMatchReserve( DWORD dwTeamIndex );
	bool IsMatchReserve( DWORD dwTeamIndex );
	bool IsReMatchLimit( DWORD dwCheckTeam, DWORD dwLimitTime );

protected:
	void AddUser( const LadderTeamUser &kUser );
	bool RemoveUser( const DWORD dwUserIndex );
	void SelectNewOwner();
	ModeType GetSelectIndexToMode( int iModeIndex, int iMapIndex );
	bool LadderTeamKickOutUser( ioHashString &rkName );
	void CheckGuildTeam( bool bPlayEnd = false );

public:      // 매칭 & 룸 입장
	void MatchEnterRoom( Room *pRoom, DWORD dwMatchTeamIndex, int iCampType, int iTeamType );
	bool MatchRequstTeamEnterRoom( SP2Packet &rkPacket );
	void MatchReStartRoom( Room *pRoom, DWORD dwOtherTeamIndex, int iOtherSelectMode, int iOtherSelectMap );

public:
	void FillSyncCreate( SP2Packet &rkPacket );
	void FillLadderTeamInfo( SP2Packet &rkPacket );
	void FillUserInfo( SP2Packet &rkPacket, const LadderTeamUser &kUser );
	void FillUserList( SP2Packet &rkPacket );
	void FillUserIndex( SP2Packet &rkPacket );
	void FillUserIndex( DWORDVec &rkUserIndex );

public:
	const LadderTeamUser &GetUserNodeByArray( int iArray );
	UserParent *GetUserNode( const ioHashString &szPublicID );

public:        //패킷 처리
	virtual bool OnProcessPacket( SP2Packet &rkPacket, UserParent *pUser );
	virtual void OnLadderTeamInfo( UserParent *pUser );

protected:
	void OnVoiceInfo( SP2Packet &rkPacket, UserParent *pUser );
	void OnLadderTeamInvite( SP2Packet &rkPacket, UserParent *pUser );
	void OnMacroCommand( SP2Packet &rkPacket, UserParent *pUser );
	void OnVersusCancel();
	
public:
	void Process();

public:
	void SyncRealTimeCreate();

public:
	LadderTeamNode( DWORD dwIndex = 1 );
	virtual ~LadderTeamNode();
};

#endif