

#ifndef _FarmingMode_h_
#define _FarmingMode_h_

#include "Mode.h"
#include "CatchModeHelp.h"

class SP2Packet;
class User;

class FarmingMode : public Mode
{
protected:
	CatchRecordList m_vRecordList;

	int m_iRedCatchBluePlayer;
	int m_iBlueCatchRedPlayer;
	
public:
	virtual void LoadINIValue();
	virtual void DestroyMode();

	virtual void AddNewRecord( User *pUser );
	virtual void RemoveRecord( User *pUser, bool bRoomDestroy = false );

protected:
	virtual void ProcessPlay();
	virtual void RestartMode();

public:
	virtual void UpdateDieState( User *pDier );
	virtual void UpdateUserDieTime( User *pDier );

public:
	virtual ModeType GetModeType() const;
	virtual void GetModeInfo( SP2Packet &rkPacket );
	virtual void GetModeHistory( SP2Packet &rkPacket );
	virtual void GetCharModeInfo( SP2Packet &rkPacket, const ioHashString &rkName, bool bDieCheck = false );
	virtual int GetRecordCnt() const;
	
	virtual const char* GetModeINIFileName() const;
	virtual TeamType GetNextTeamType();
	virtual void CheckRoundEnd( bool bProcessCall );

	virtual void InitObjectGroupList();

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
	CatchRecord* FindCatchRecord( const ioHashString &rkName );
	CatchRecord* FindCatchRecord( User *pUser );

protected:
	virtual void OnEventSceneEnd( User *pSend, SP2Packet &rkPacket );
	virtual void SetRoundEndInfo( WinTeamType eWinTeam );
	virtual int GetCurTeamUserCnt( TeamType eTeam );

public:
	virtual bool ProcessTCPPacket( User *pSend, SP2Packet &rkPacket );

protected:
	void OnPrisonerEscape( User *pUser, SP2Packet &rkPacket );
	void OnPrisonerDrop( User *pUser, SP2Packet &rkPacket );
	void OnPrisonerMode( User *pUser, SP2Packet &rkPacket );

	// 2019-07-03 by bckim, 파밍모드 추가
protected:
	typedef std::vector<CoordinateInfo> COORDINATEINFO;

	typedef boost::unordered_map<int, COORDINATEINFO> MAPSUPPLYITEMPOS;
	typedef std::map<int, int> CRITICALINFO;				//<확률, 배수>
	IORandom	m_ioRand;

	MAPSUPPLYITEMPOS m_mMapSupplyFirstItemPos_BLUE;
	MAPSUPPLYITEMPOS m_mMapSupplyFirstItemPos_RED;
	MAPSUPPLYITEMPOS m_mMapSupplyItemFixPos;				// 지역한정 좌표

	float Dispersion_Ratio;									// 아이템 드랍 포지션 범위 집중도 
	DWORD dwFirst_group_start_time;
	int iFirst_item_group_count;
	DWORD dwFirst_item_group[10];

	DWORD dwSecond_group_start_time;
	DWORD dwSecond_group_supply_item_term;
	int iSecond_item_group_count;
	DWORD dwSecond_item_group[10];	
	void GetCoorninateInfo(int iMapID, 	Vector3 &vPos, int iIndex );
	void GetFirstCoorninateInfo(int iMapID, Vector3& vPosition, TeamType team, int iIndex );

	void FirstOnCreateFieldItem();		
	void FirstOnCreateFieldItem_TEAM( TeamType team );		
	void OnCreateFieldItem( );			

	bool FirstItemDrop;
public:
	void LoadINISupplyItemBasicInfo();
	void LoadINISupplyItemPos();

	DWORD m_dwRoundTimeSendTime_TEMP;
	DWORD m_dwModeStartTime_TEMP;
	// End. 2019-07-03 by bckim, 파밍모드 추가

public:
	FarmingMode( Room *pCreator );
	virtual ~FarmingMode();
};

inline FarmingMode* ToFarmingMode( Mode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_FARMING )
		return NULL;

	return static_cast< FarmingMode* >( pMode );
}

#endif


