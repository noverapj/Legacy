#pragma once

#include "UserParent.h"
#include "CopyNodeParent.h"
#include <boost/unordered/unordered_map.hpp>

class UserCopyNode : public UserParent,
					 public CopyNodeParent					 
{
protected:
	// 기본 정보
	DWORD		 m_dwUserIndex;
	DWORD        m_dwDBAgentID;
	ioHashString m_szPrivateID;
	ioHashString m_szPublicID;
	int          m_iCampType;
	int			 m_iGradeLevel;
	int          m_iUserPos;
	int          m_iKillDeathLevel;
	int          m_iLadderPoint;
	bool         m_bSafetyLevel;
	ModeType     m_eModeType;
	bool         m_bDeveloper;
	int          m_iUserRank;
	DWORD        m_dwPingStep;
	DWORD        m_dwGuildIndex;
	DWORD        m_dwGuildMark;
	DWORDVec     m_vBestFriend;

	bool		 m_bShuffleGlobalSearch;
	DWORD		 m_dwLadderMatchTime;

	int		m_iBattleMode_Order;		// 2019-02-14 by bckim, 배틀 모드 추가

protected:
	void InitData();

public:
	virtual void OnCreate( ServerNode *pCreator );
	virtual void OnDestroy();

public:
	virtual DWORD GetUserIndex() const { return m_dwUserIndex; }
	virtual DWORD GetUserDBAgentID() const { return m_dwDBAgentID; }
	virtual const DWORD GetAgentThreadID() const { return m_szPrivateID.GetHashCode(); }
	virtual const ioHashString& GetPrivateID() const { return m_szPrivateID; }
	virtual const ioHashString& GetPublicID() const { return m_szPublicID; }
	virtual int GetUserCampPos() const { return m_iCampType; }
	virtual int GetGradeLevel() { return m_iGradeLevel; }
	virtual int GetUserPos() { return m_iUserPos; }
	virtual int GetKillDeathLevel() { return m_iKillDeathLevel; }
	virtual int GetLadderPoint(){ return m_iLadderPoint; }
	virtual bool IsSafetyLevel() { return m_bSafetyLevel; }
	virtual const bool IsUserOriginal(){ return false; }
	virtual bool RelayPacket( SP2Packet &rkPacket );
	virtual ModeType GetModeType(){ return m_eModeType; }
	virtual const bool IsDeveloper(){ return m_bDeveloper; }
	virtual int GetUserRanking(){ return m_iUserRank; }
	virtual DWORD GetPingStep(){ return m_dwPingStep; }
	virtual bool  IsGuild();
	virtual DWORD GetGuildIndex();
	virtual DWORD GetGuildMark();
	virtual bool IsBestFriend( DWORD dwUserIndex );
	virtual void GetBestFriend( DWORDVec &rkUserIndexList );
	virtual bool IsShuffleGlboalSearch();

public:
	void SetUserIndex( DWORD dwIndex ){ m_dwUserIndex = dwIndex; }

public:
	void ApplySyncCreate( SP2Packet &rkPacket );
	void ApplySyncUpdate( SP2Packet &rkPacket );
	void ApplySyncPos( SP2Packet &rkPacket );
	void ApplySyncGuild( SP2Packet &rkPacket );
	void ApplySyncCamp( SP2Packet &rkPacket );
	void ApplySyncPublicID( SP2Packet &rkPacket );
	void ApplySyncBestFriend( SP2Packet &rkPacket );
	void ApplyUserNode( UserParent *pUser );
	void ApplySyncShuffle( SP2Packet &rkPacket );
	void SetLadderMatchTime( DWORD dwMatchTime ) { m_dwLadderMatchTime = dwMatchTime; }
	DWORD GetLadderMatchTime() { return m_dwLadderMatchTime; }

	// 2019-02-14 by bckim, 배틀 모드 추가
	void SetBattleModeOrder( int iBattleModeOrder ) { m_iBattleMode_Order = iBattleModeOrder; }
	int GetBattleModeOrder() { return m_iBattleMode_Order; }
	// End. 2019-02-14 by bckim, 배틀 모드 추가

	
public:
	UserCopyNode();
	virtual ~UserCopyNode();
};

typedef boost::unordered_map<DWORD, UserCopyNode*> uUserCopyNode;
typedef uUserCopyNode::iterator uUserCopyNode_iter;

typedef boost::unordered_map<std::string, DWORD> uUserCopyNodeTable;
typedef uUserCopyNodeTable::iterator uUserCopyNodeTable_iter;