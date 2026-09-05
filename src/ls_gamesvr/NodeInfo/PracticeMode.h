#pragma once
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max

#include <boost/date_time/posix_time/posix_time.hpp>

#pragma pop_macro("min")
#pragma pop_macro("max")

#include "Mode.h"
#include "../BoostPooler.h"

class SP2Packet;
class User;

class PracticeMode: public Mode, public BoostPooler<PracticeMode>
{
protected:
	std::vector< ModeRecord > m_vRecordList;

	int m_iPracticeIndex;
	boost::posix_time::ptime m_PracticeStartTime;

public:
	virtual void InitMode();
	virtual void AddNewRecord( User *pUser );
	virtual void RemoveRecord( User *pUser, bool bRoomDestroy = false );
	virtual void GetCharModeInfo( SP2Packet &rkPacket, const ioHashString &szName, bool bDieCheck = false ){};
	virtual int  GetRecordCnt() const;
	virtual int GetCurTeamUserCnt( TeamType eTeam ){return 1;};
	virtual TeamType GetNextTeamType(){return TEAM_NONE;};
	virtual ModeRecord* FindModeRecord( const ioHashString &rkName );
	virtual ModeRecord* FindModeRecord( User *pUser );
	virtual ModeRecord* FindModeRecord( int iIdx );
	virtual ModeRecord* FindModeRecordIgnoreCase( const ioHashString &rkName );
	
	virtual void InitObjectGroupList();

	virtual const char* GetModeINIFileName() const;

public:
	virtual int  GetUserKickVoteLimit( const ioHashString &szKickUserName ){return 0;};

protected:
	virtual void ProcessReady();
	virtual void ProcessPlay();
	virtual void ProcessRevival();

public:
	virtual bool ProcessTCPPacket( User *pSend, SP2Packet &rkPacket );
	virtual bool CheckTCPPacket( SP2Packet &rkPacket );

protected:
	void OnPrisonerEscape( User *pUser, SP2Packet &rkPacket );
	void OnPrisonerDrop( User *pUser, SP2Packet &rkPacket );
	void OnPrisonerMode( User *pUser, SP2Packet &rkPacket );
	void OnSelectRegular( User *pUser, SP2Packet &rkPacket );

public:
	void OnPractice_GameStart( User *pUser, SP2Packet &rkPacket );
	void OnPracticeResult( User *pUser, SP2Packet &rkPacket );


public:
	void SetPracticeIndex( int iIndex ) { m_iPracticeIndex = iIndex; }
	int GetPracticeIndex()	{	return m_iPracticeIndex;		}

	void SetBoostPracticeStartTime( boost::posix_time::ptime PracticeStartTime ){	m_PracticeStartTime = PracticeStartTime;	}
	boost::posix_time::ptime GetBoostPracticeStartTime(){	return m_PracticeStartTime;	}

protected:
	int SetRegularSoldierItem( int iSlot, OUT int &iItemCode );

public:
	PracticeMode( Room *pCreator );
	virtual ~PracticeMode();

	inline ModeType GetModeType() const
	{
		return MT_PRACTICE;
	}
};

inline PracticeMode* ToPracticeMode( Mode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_PRACTICE )
		return NULL;

	return static_cast< PracticeMode* >( pMode );
}
