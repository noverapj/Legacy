

#ifndef _HiddenkingMode_h_
#define _HiddenkingMode_h_

#include "Mode.h"
#include "Item.h"
#include "HiddenkingModeHelp.h"

class SP2Packet;
class User;
class ioItem;

class HiddenkingMode : public Mode
{
protected:
	HiddenkingRecordList m_vRecordList;

	int m_iBlueTeamScore;
	int m_iRedTeamScore;
	int m_iTotalBlueScore;
	int m_iTotalRedScore;

	int m_iNeedPoint;

	int m_iCrownRate;
	
	DWORD m_dwWinTeamScoreTime;
	DWORD m_dwLoseTeamScoreTime;
	DWORD m_dwDrawScoreTime;

	DWORD m_dwBlueTakeKingTime;
	DWORD m_dwRedTakeKingTime;

	DWORD m_dwRemainTime;
	DWORD m_dwGivePointTime;
	float m_fRemainTimeRate;

	float m_fManyPeopleRate;
	float m_fDecreaseScoreTimeRate;

	DWORD m_dwScoreGaugeConstValue;
	float m_fScoreGaugeMaxRate;
	float m_fScoreGaugeDethTimeRate;;

	Vector3Vec m_vWearPosList;
	ioHashString m_szNameOfKing;

	DWORD m_dwCheckKingPingTime;

	DWORD m_dwKingPingTime;
	int m_iKingPingCnt;

public:
	virtual void LoadINIValue();
	virtual void DestroyMode();

	virtual void AddNewRecord( User *pUser );
	virtual void RemoveRecord( User *pUser, bool bRoomDestroy = false );

protected:
	virtual void LoadRoundCtrlValue( ioINILoader &rkLoader );
	
protected:
	virtual void ProcessReady();
	virtual void ProcessPlay();
	virtual void RestartMode();

	void ProcessKingPing();

public:
	virtual void InitObjectGroupList();
	virtual Vector3 GetRandomItemPos(ioItem *pItem = NULL);

protected:
	void LoadWearItem( ItemVector &rvItemList);
	void LoadWearPosList(ioINILoader &rkLoader );
	Vector3 GetRandomWearPos( bool bStartRound );

protected:
	virtual void SetRoundEndInfo( WinTeamType eTeam );
	virtual int GetCurTeamUserCnt( TeamType eTeam );

	virtual void OnEventSceneEnd( User *pSend, SP2Packet &rkPacket );
	
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
	HiddenkingRecord* FindKingRecord( User *pUser );
	HiddenkingRecord* FindKingRecord( const ioHashString &rkName );

protected:
	void ClearKing();
	
	void SetScore(TeamType eTeam, bool bCrown = false );
	void SetTakeKingTime( TeamType eKingTeam );

	DWORD GetPointTime( TeamType eKingTeam );
	void CheckGivePoint();
	TeamType CheckManyPeopleTeam();

	DWORD CheckDecreasePointTimeByPeopleCnt();

public:
	void CheckDropCrown(ioItem *pItem);
	void CheckPickCrown(ioItem *pItem, User *pUser);
	void CheckCreateCrown(User *pUser);

	void BadPingDropCrown( User *pUser );

	// ¸ðµå : 09.06.03
	//void SendScore( TeamType eTeam, bool bRoundEnd = false );
	void SendScore( bool bRoundEnd = false );

	inline const ioHashString& GetKingName() { return m_szNameOfKing; }

public:
	HiddenkingMode( Room *pCreator );
	virtual ~HiddenkingMode();
};

inline HiddenkingMode* ToKingMode( Mode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_KING )
		return NULL;

	return static_cast< HiddenkingMode* >( pMode );
}

#endif

