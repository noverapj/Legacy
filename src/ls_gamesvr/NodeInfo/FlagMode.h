#ifndef _FlagMode_h_
#define _FlagMode_h_

#include "Mode.h"
#include "FlagModeHelp.h"

class SP2Packet;
class User;

class FlagMode : public Mode
{
protected:
	FlagRecordList m_vRecordList;

	float m_fWinScoreConstant;

	float m_fScoreGapConst;
	float m_fScoreGapRateConst;
	float m_fLadderScoreGapConst;
	float m_fLadderScoreGapRateConst;

	float m_fDefaultFlagPoint;

	float m_fCurRedCrownPoint;
	float m_fCurBlueCrownPoint;


	float m_fCurBlue_FlagPoint;
	float m_fCurRed_FlagPoint;

	ioItem *m_pItem;

	int		m_iFagRegenerationTime;
	float	m_fFlagKillPoint;
	float	m_fFlagTimePoint;
	int		m_iFlagTimePointMax;
	int		m_iFlagTimePointMin;	
	int		m_iFlagEnableZoneRange;
	int		m_iFlagReturnTerm;

	// End. 2018-03-15 by bckim, 깃발모드 추가
		
	DWORD m_dwCurDecreaseTickTime;

	DWORD m_dwRoundEndContribute;
	bool m_bRoundEndContribute;

	DWORD m_dwBlueContribute;
	DWORD m_dwRedContribute;

	DWORD m_dwFlagPingTime;
	DWORD m_dwFlagPingCnt;
	DWORD m_dwBlueCheckFlagPingTime;
	DWORD m_dwRedCheckFlagPingTime;

protected:
	typedef std::vector<float> MemberBalanceVec;
	MemberBalanceVec m_MemberBalanceVec;

protected:
	Vector3		m_vFlagPosition;			// 깃발 위치 
	Vector3		m_vFlagZone_Blue;
	Vector3		m_vFlagZone_Red;

	Vector3Vec m_vBlueWearPosList;
	Vector3Vec m_vRedWearPosList;
	Vector3Vec m_vNoneWearPosList;

protected:
	ioHashString	m_szFlagCaptureName;
	DWORD			m_dwRetentionTime;			// 소유 시간 

	int	m_iBlueUserGap;
	int	m_iRedUserGap;

protected:
	enum SyncEventType
	{
		SET_NONE,
		SET_CROWN_DROP,
		SET_CROWN_PICK,
		SET_USER_JOIN,
		SET_USER_LEAVE,
	};

	struct SyncEventTable
	{
		BYTE	eEventType;
		int		iTime;
		int		iValue;		

		SyncEventTable()
		{
			eEventType	= SET_NONE;
			iTime		= 0;
			iValue		= 0;			
		}
	};

	typedef std::vector<SyncEventTable> SyncEventTableList;
//	SyncEventTableList m_BlueSyncEventTableList;
//	SyncEventTableList m_RedSyncEventTableList;

protected:
	virtual void ProcessPlay();
	virtual void RestartMode();

	virtual void LoadRoundCtrlValue( ioINILoader &rkLoader );
	virtual void SendRoundResult( WinTeamType eWinTeam );

public:
	virtual void LoadINIValue();
	virtual void DestroyMode();

	virtual void AddNewRecord( User *pUser );
	virtual void RemoveRecord( User *pUser, bool bRoomDestroy = false );

	virtual void InitObjectGroupList();
	virtual const char* GetModeINIFileName() const;
	
	Vector3 m_vDropPos;								

	void FlagModeAddPoint(TeamType itype, User *pUser);

protected:
	void LoadFlagPing( ioINILoader &rkLoader );

	void LoadBasicFlag( ioINILoader &rkLoader );

	void LoadWearItem( ItemVector &rvItemList );
	void LoadWearPosList( ioINILoader &rkLoader, Vector3Vec& vVec );

	void LoadFlagPosition( ioINILoader &rkLoader);	
	void LoadFlagPosition( ioINILoader &rkLoader, Vector3& vVec );

public:			// 임시 
	//Vector3 GetRandomWearPos( bool bStartRound, int iTeamType );
	Vector3 GetFlagPosition();
	Vector3 GetFlagZonePositon(TeamType iTeam);	

public:
	virtual Vector3 GetRandomItemPos(ioItem *pItem = NULL);

public:
	void CheckCreateFlag( User *pUser );
	void CheckBadPingDropCrown( User *pUser );

	//void SetTakeKing( TeamType Type, const ioHashString& szPublicID );	
	

	void Set_Owner_Status(const ioHashString& szPublicID);
	void Set_Ownerless_Status();
	bool Is_Flag_Owner(const ioHashString& szPublicID);
	const ioHashString& Get_Current_Owner_PublicID();
	//const ioHashString& Get_Current_Owner_PublicID() const { return m_szFlagCaptureName; }
	DWORD GetRetentionTime();

	void SetFlagRegeneTime(int iFagRegenerationTime);
	void SetFlagKillPoint(float fKillPoint);
	void SetFlagTimePoint(float fTimePoint);
	void SetFlagTimePointMax(int iTimePointMax);
	void SetFlagTimePointMin(int iTimePointMin);
	void SetFlagEnableZoneRange(int iRange);
	void SetFlagReturnTerm(int iTerm);
	
	int	GetFlagRegeneTime();
	float GetFlagKillPoint();
	float GetFlagTimePoint();
	int GetFlagTimePointMax();
	int GetFlagTimePointMin();
	int GetFlagEnableZoneRange();
	int GetFlagReturnTerm();

	//DWORD Get
	// End. 2018-03-15 by bckim, 깃발모드 추가	

	

public:
	virtual void SetModeState( ModeState eState );
	virtual ModeType GetModeType() const;
	virtual void GetModeInfo( SP2Packet &rkPacket );
	virtual void GetExtraModeInfo( SP2Packet &rkPacket );
	virtual void GetModeHistory( SP2Packet &rkPacket );

	virtual void GetCharModeInfo( SP2Packet &rkPacket, const ioHashString &rkName, bool bDieCheck = false );

	virtual bool CheckRoundJoin( User *pSend );
	
	virtual void CheckUserLeaveEnd();
	virtual void CheckRoundEnd( bool bProcessCall );

	virtual void UpdateDropDieRecord( User *pDier, const ioHashString &szAttacker, const ioHashString &szBestAttacker );
	virtual void UpdateWeaponDieRecord( User *pDier, const ioHashString &szAttacker, const ioHashString &szBestAttacker );
	virtual void UpdateRoundRecord();

	float GetResultScoreGapValue( bool bLadderPoint, TeamType eWinTeam );

	virtual int GetRecordCnt() const;

	virtual TeamType GetNextTeamType();	

public:
	virtual int  GetUserKickVoteLimit( const ioHashString &szKickUserName );

public:
	virtual ModeRecord* FindModeRecord( const ioHashString &rkName );
	virtual ModeRecord* FindModeRecord( User *pUser );
	virtual ModeRecord* FindModeRecord( int iIdx );

protected:
	FlagRecord* FindFlagRecord( const ioHashString &rkName );
	FlagRecord* FindFlagRecord( User *pUser );

protected:
	virtual void OnEventSceneEnd( User *pSend, SP2Packet &rkPacket );
	virtual void SetRoundEndInfo( WinTeamType eWinTeam );
	virtual int GetCurTeamUserCnt( TeamType eTeam );

protected:
	void ProcessCrownPoint();

	TeamType    CheckFlagPointWinTeam();
	WinTeamType CheckFlagContributePointWinTeam();
	WinTeamType CheckFlagRandWinTeam();
	
protected:
	void SendRoundEndContribute();
	void SendRoundEndContributeResult();

public:	
	virtual void OnRoundEndContribute( User *pUser, SP2Packet &rkPacket );

	void OnFlagSyncRequest( User *pUser, SP2Packet &rkPacket );
	void OnFlagPointIdentify ( User *pUser, SP2Packet &rkPacket );	

public:
	void ProcessFlagPing();
	void CheckFlagPing( DWORD& dwCheckKingPingTime, TeamType eTeam );
	void BadPingDropFlag( User *pUser );

	void RecreateFlagPing();
	bool tick_start;
	DWORD flag_tick_count;

public:
	virtual bool ProcessTCPPacket( User *pSend, SP2Packet &rkPacket );


public:
	FlagMode( Room *pCreator );
	virtual ~FlagMode();
};

inline FlagMode* ToFlagMode( Mode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_FLAG )
		return NULL;

	return static_cast< FlagMode* >( pMode );
}
#endif

