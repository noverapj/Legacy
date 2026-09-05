

#ifndef _DoubleCrownMode_h_
#define _DoubleCrownMode_h_

#include "Mode.h"
#include "DoubleCrownModeHelp.h"

class SP2Packet;
class User;

class DoubleCrownMode : public Mode
{
protected:
	DoubleCrownRecordList m_vRecordList;

	int m_iRedKillPoint;
	int m_iBlueKillPoint;

	float m_fRedKillPointRate;
	float m_fBlueKillPointRate;

	float m_fWinScoreConstant;

	float m_fScoreGapConst;
	float m_fScoreGapRateConst;
	float m_fLadderScoreGapConst;
	float m_fLadderScoreGapRateConst;

	float m_fDefaultCrownPoint;

	float m_fCurRedCrownPoint;
	float m_fCurBlueCrownPoint;

	DWORD m_dwDecreaseTickTime;
	DWORD m_dwCurDecreaseTickTime;

	float m_fDecreaseCrownPoint;
	float m_fCurRedDecreaseCrownPoint;
	float m_fCurBlueDecreaseCrownPoint;

	DWORD m_dwRoundEndContribute;
	bool m_bRoundEndContribute;

	DWORD m_dwBlueContribute;
	DWORD m_dwRedContribute;

	DWORD m_dwKingPingTime;
	DWORD m_dwKingPingCnt;

	DWORD m_dwBlueCheckKingPingTime;
	DWORD m_dwRedCheckKingPingTime;

protected:
	typedef std::vector<float> MemberBalanceVec;
	MemberBalanceVec m_MemberBalanceVec;

protected:
	struct PointBalance
	{
		DWORD m_dwStart;
		DWORD m_dwEnd;

		float m_fBalanceValue;

		PointBalance()
		{
			m_dwStart = 0;
			m_dwEnd   = 0;

			m_fBalanceValue = 0.0f;
		}
	};
	typedef std::vector<PointBalance> PointBalanceVec;
	PointBalanceVec m_PointBalanceVec;	

protected:
	Vector3Vec m_vBlueWearPosList;
	Vector3Vec m_vRedWearPosList;
	Vector3Vec m_vNoneWearPosList;

protected:
	ioHashString m_szBlueKingName;
	ioHashString m_szRedKingName;

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
	SyncEventTableList m_BlueSyncEventTableList;
	SyncEventTableList m_RedSyncEventTableList;

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
	
protected:
	void LoadKingPing( ioINILoader &rkLoader );
	void LoadBalance( ioINILoader &rkLoader );
	float GetMemberBalanceRate( TeamType eTeam );	
	float GetMemberBalanceRateByGap( int iGap );

	void LoadWearItem( ItemVector &rvItemList );
	void LoadWearPosList( ioINILoader &rkLoader, int iTeamType );
	void LoadWearPosList( ioINILoader &rkLoader, Vector3Vec& vVec );
	
	Vector3 GetRandomWearPos( bool bStartRound, int iTeamType );

public:
	virtual Vector3 GetRandomItemPos(ioItem *pItem = NULL);

public:
	void CheckDropCrown( ioItem *pItem );
	bool CheckPrePickCrown( ioItem *pItem, User *pUser );
	void CheckPickCrown( ioItem *pItem, User *pUser );
	void CheckCreateCrown( User *pUser );
	void CheckBadPingDropCrown( User *pUser );

	void SetTakeKing( TeamType Type, const ioHashString& szPublicID );	

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
	DoubleCrownRecord* FindDoubleCrownRecord( const ioHashString &rkName );
	DoubleCrownRecord* FindDoubleCrownRecord( User *pUser );

protected:
	virtual void OnEventSceneEnd( User *pSend, SP2Packet &rkPacket );
	virtual void SetRoundEndInfo( WinTeamType eWinTeam );
	virtual int GetCurTeamUserCnt( TeamType eTeam );

protected:
	void UpdateCurDecreaseCrownPoint( TeamType eTeam, int iPreCnt );

	void ProcessCrownPoint();

	TeamType    CheckCrownPointWinTeam();
	WinTeamType CheckCrownContributePointWinTeam();
	WinTeamType CheckCrownRandWinTeam();
	
protected:
	void SendRoundEndContribute();
	void SendRoundEndContributeResult();

public:	
	virtual void OnRoundEndContribute( User *pUser, SP2Packet &rkPacket );
	void OnDoubleCrownSyncRequest( User *pUser, SP2Packet &rkPacket );

public:
	const ioHashString& GetTeamKing( TeamType eTeam );

	void ProcessKingPing();
	void CheckKingPing( DWORD& dwCheckKingPingTime, TeamType eTeam );
	void BadPingDropCrown( User *pUser );

public:
	float CalcCrownPoint( SyncEventTableList& TableList, TeamType eTeam );

public:
	virtual bool ProcessTCPPacket( User *pSend, SP2Packet &rkPacket );

public:
	DoubleCrownMode( Room *pCreator );
	virtual ~DoubleCrownMode();
};

inline DoubleCrownMode* ToDoubleCrownMode( Mode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_DOBULE_CROWN )
		return NULL;

	return static_cast< DoubleCrownMode* >( pMode );
}

#endif

