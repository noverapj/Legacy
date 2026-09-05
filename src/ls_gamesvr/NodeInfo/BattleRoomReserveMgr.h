
#ifndef _BattleRoomReserveMgr_h_
#define _BattleRoomReserveMgr_h_

#include "UserNodeManager.h"
#include "BattleRoomNode.h"

class BattleRoomReserveNode
{
protected:
	ioHashString m_szPartyName;
	vUser        m_vList;
	int          m_iReserveSelectMode;

public:	
	void OnCreate();
	void OnDestroy();

public:
	void EnterUser( User *pUser );
	bool LeaveUser( User *pUser );
	void SetReserveSelectMode( int iSelectMode );

public:
	int  GetUserSize();
	bool IsEmpty(){ return m_vList.empty(); }
	
public:
	int  GetUserLevel();
	int  GetAverageLevel();
	int  GetReserveSelectMode(){ return m_iReserveSelectMode; }
	
public:
	bool CreateBattleRoom();
	void JoinBattleRoom( BattleRoomNode *pBattleRoom );
	
public:
	BattleRoomReserveNode();
	virtual ~BattleRoomReserveNode();
};

typedef vector< BattleRoomReserveNode * > vBRRNode;
typedef vBRRNode::iterator vBRRNode_iter;

#define MAX_RESERVE_BATTLEROOM          3000
class BattleRoomReserveMgr
{
	static BattleRoomReserveMgr *sg_Instance;
	vBRRNode    m_vBRRNode;
	MemPooler< BattleRoomReserveNode >	m_MemNode;

	int			m_iMaxPartyCreateUser;
public:
	static BattleRoomReserveMgr &GetInstance();
	static void ReleaseInstance();

public:
	void InitMemoryPool();
	void ReleaseMemoryPool();

public:
	void LoadInfo( const char *szFileName );
	bool CreateBattleRoom( BattleRoomReserveNode *pReserveNode );
	void JoinBattleRoom( BattleRoomReserveNode *pReserveNode, BattleRoomNode *pBattleRoom );
	int  GetReserveAllUser();

public:
	void ReserveBattleRoom( User *pUser, int iSearchTerm );
	BattleRoomReserveNode *DeleteReserveBattleRoom( User *pUser );
	void DeleteReserveBattleRoom( BattleRoomReserveNode *pReserveNode );
	BattleRoomReserveNode *SearchReserveBattleRoom( int iMyLevel, int iSearchTerm );

private:     	/* Singleton Class */
	BattleRoomReserveMgr();
	virtual ~BattleRoomReserveMgr();

};
#define g_BattleRoomReserveMgr    BattleRoomReserveMgr::GetInstance()

#endif