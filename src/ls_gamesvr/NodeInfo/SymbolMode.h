

#ifndef _SymbolMode_h_
#define _SymbolMode_h_

#include "Mode.h"
#include "SymbolModeHelp.h"

class SP2Packet;
class User;

typedef struct _RecoverGauge
{
	float m_fWinGauge;
	float m_fDrawGauge;
	float m_fLoseGauge;

	_RecoverGauge()
	{
		m_fWinGauge = 0.0f;
		m_fDrawGauge = 0.0f;
		m_fLoseGauge = 0.0f;
	}
} RecoverGauge;

class SymbolMode : public Mode
{
protected:
	SymbolList m_vSymbolList;
	SymbolRecordList m_vRecordList;

	bool m_bNoLimiteSymbol;

	int m_iBlueSymbolCnt;
	int m_iRedSymbolCnt;

	IntVec m_SymbolLimitsCharCntList;
	int m_iMinVictoryActiveSymbolCnt;

	// For Gauge
	DWORD m_dwRecoverTic;
	DWORD m_dwTicCheckTime;

	DWORD m_dwScoreGaugeConstValue;
	float m_fScoreGaugeMaxRate;
	float m_fScoreGaugeDethTimeRate;

	typedef std::vector< RecoverGauge > RecoverGaugeList;
	RecoverGaugeList m_RecoverGaugeList;

	float m_fMaxGauge;
	float m_fRedGauge;
	float m_fBlueGauge;
	float m_fCurRecoverGauge;
	float m_fDecreaseScoreTimeRate;

	TeamType m_AscendancyTeam;

public:
	virtual void LoadINIValue();
	virtual void DestroyMode();

	virtual void AddNewRecord( User *pUser );
	virtual void RemoveRecord( User *pUser, bool bRoomDestroy = false );

protected:
	virtual void ProcessPlay();
	virtual void RestartMode();

	virtual void OnEventSceneEnd( User *pSend, SP2Packet &rkPacket );
	virtual void SetRoundEndInfo( WinTeamType eWinTeam );

	virtual int GetCurTeamUserCnt( TeamType eTeam );

public:
	virtual ModeType GetModeType() const;
	virtual void GetModeInfo( SP2Packet &rkPacket );
	virtual void GetModeHistory( SP2Packet &rkPacket );
	virtual void GetCharModeInfo( SP2Packet &rkPacket, const ioHashString &szName, bool bDieCheck = false );
	virtual int GetRecordCnt() const;

	virtual const char* GetModeINIFileName() const;
	virtual TeamType GetNextTeamType();
	virtual void CheckRoundEnd( bool bProcessCall );
	virtual void CheckUserLeaveEnd();

	virtual void InitObjectGroupList();

	virtual void SendScoreGauge();

	virtual bool CheckRoundJoin( User *pSend );
	
public:
	virtual void UpdateRoundRecord();

public:
	virtual int  GetUserKickVoteLimit( const ioHashString &szKickUserName );

public:
	virtual ModeRecord* FindModeRecord( const ioHashString &rkName );
	virtual ModeRecord* FindModeRecord( User *pUser );
	virtual ModeRecord* FindModeRecord( int iIdx );

protected:
	SymbolRecord* FindSymbolRecord( User *pUser );
	SymbolRecord* FindSymbolRecord( const ioHashString &rkName );

public:
	virtual bool ProcessTCPPacket( User *pSend, SP2Packet &rkPacket );

protected:
	void RestoreSymbolList();
	void SetReSymbolAcitvity();

	void SetScore( TeamType eTeam, bool bLast=false );
	void SendScore(TeamType eTeam, bool bLast=false );

	int GetActiveSymbolCnt();

	// ¸ðµå : 09.06.03
	int GetTeamSymbolCnt( TeamType eTeam );

	void UpdateSymbolGauge();

	bool IsEnableDie( int iSymbolIdx, TeamType eAttackTeam );
	TeamType CheckCurWinTeam();

	float CheckDecreaseGaugeByPeopleCnt();

	void ProcessGauge();

	void OnSymbolDie( User *pUser, SP2Packet &rkPacket );
	void OnSymbolMax( User *pUser, SP2Packet &rkPacket );
	void OnSymbolDamaged( User *pUser, SP2Packet &rkPacket );

public:
	SymbolMode( Room *pCreator );
	virtual ~SymbolMode();
};

inline SymbolMode* ToSymbolMode( Mode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_SYMBOL )
		return NULL;

	return static_cast< SymbolMode* >( pMode );
}

#endif

