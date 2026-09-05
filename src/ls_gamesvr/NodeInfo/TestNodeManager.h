#pragma once
#include "../Util/Singleton.h"

class CQueryResultData;
class TestNode;
class UserBot;

typedef std::vector<TestNode*> TestNodeVec;

enum BOTTYPE
{
	BOT_USER		= 1,
};

enum USERBOTTYPE
{
	BOT_TURNAMENT	= 1,
};

class ioTestNodeManager : public Singleton< ioTestNodeManager >
{
protected:
	typedef std::map< ioHashString, TestNode*> TestNodeMap;
	TestNodeMap m_TestNodeMap;

protected:
	bool m_bLogPrint;
	bool m_bTestServer;

public:
	void LoadINI();
	
	void ServerDownAllSave();
	void DestroyNode();

public:	
	inline bool IsTestServer(){ return m_bTestServer; }
	int IsLogPrint();
	
public:
	TestNode* GetNode( const ioHashString& szGUID );
	void GetNodeList( TestNodeVec& DestTourVec );

public:
	UserBot* CreateUserBotTemplete( ioINILoader &rkLoader, int iSubType );

public:
	void OnUpdate();	

public:
	static ioTestNodeManager& GetSingleton();

public:	
	ioTestNodeManager( bool bTestServer );
	~ioTestNodeManager();
};

#define g_TestNodeMgr ioTestNodeManager::GetSingleton()

////////////////////////////////////////////////////////////////////////////////////////////

class TestNode
{
	friend class ioTestNodeManager;

private:
	ioHashString m_szGUID;
	ioHashString m_szClassName;

protected:
	BOTTYPE m_Type;
	int     m_SubType;

public:
	virtual bool LoadINI( ioINILoader &rkLoader );	

private:
	void SetType( BOTTYPE Type ){ m_Type = Type; }
	void SetSubType( int SubType ){ m_SubType = SubType; }

public:
	virtual BOTTYPE GetType(){ return m_Type; }
	virtual int GetSubType(){ return m_SubType; }

public:
	inline const ioHashString& GetGUID() const { return m_szGUID; }

public:
	virtual void OnCreate();
	virtual bool OnUpdate();
	virtual void OnDestroy();

protected:
	TestNode();
	~TestNode();
};

////////////////////////////////////////////////////////////////////////////////////////////

class UserBot : public TestNode
{
public:
	enum USERLOADSTATE
	{
		ULS_NONE,
		ULS_LOADDB,
		ULS_LOADDB_DONE,
		ULS_LOADDB_FAIL,
	};

private:
	ioHashString m_szPrivateID;
	USERLOADSTATE m_State;

protected:
	DWORD m_dwUserID;
	DWORD m_dwDBAgentID;
	ioHashString m_szGUID;

protected:
	int  m_iDBCampPos;
	bool m_bUserConnect;

public:
	virtual bool LoadINI( ioINILoader &rkLoader );

public:
	static UserBot* ToUserBot( TestNode *pNode );

public:
	inline bool IsUserConnect(){ return m_bUserConnect; }

public:
	const ioHashString& GetPrivateID(){ return m_szPrivateID; }
	const DWORD GetAgentThreadID() const { return m_szPrivateID.GetHashCode(); }
	const DWORD GetDBAgentID() const { return m_dwDBAgentID; }
	const DWORD GetUserIndex() const { return m_dwUserID; }

	const int GetDBCampPos() const { return m_iDBCampPos; }
	void SetDBCampPos( int iCampPos ){ m_iDBCampPos = iCampPos; }

public:
	virtual void OnCreate();
	virtual bool OnUpdate();
	virtual void OnDestroy();

public:
	void ApplyDBUserData( CQueryResultData *query_data );

	void SetState( USERLOADSTATE State ){ m_State = State; }
	UserBot::USERLOADSTATE GetState(){ return m_State; }
	bool IsActive();

public:
	UserBot();
	~UserBot();
};

////////////////////////////////////////////////////////////////////////////////////////////

class TournamentBot : public UserBot
{
public:
	enum TOURNAMENTBOTTYPE
	{
		TBT_NONE,
		TBT_TOURNAMENT,	//´ëÈ¸Âü°¡º¿
		TBT_CHEER,		//´ëÈ¸ÀÀ¿øº¿
	};


	enum TOURNAMENTSTATE
	{
		TS_NONE,
		TS_JOIN,
		TS_TEAM_GET,
		TS_TEAM_GET_OK,
		TS_TEAM_GET_FAIL,		
		TS_NOT_EXIST_TEAM,
	};

	enum TOURNAMENTCHEERSTATE
	{
		TCS_NONE,
		TCS_CHEER_TRY,
		TCS_RESULT_GET,		
		TCS_CHEER_DECISION_OK,
		TCS_CHEER_DECISION_FAIL,
	};

protected:
	TOURNAMENTBOTTYPE m_TourBotType;
	TOURNAMENTSTATE   m_State;
	TOURNAMENTCHEERSTATE m_CheerState;

protected:
	DWORD		 m_dwTeamIndex;
	int			 m_iLadderPoint;
	BYTE		 m_CampPos;
	ioHashString m_szTeamName;

	DWORD		 m_dwTryCount;
	DWORD		 m_dwJoinCheckTime;
	DWORD		 m_dwCheerCheckTime;	

	DWORD		 m_dwCurrCheckTime;

protected:	
	DWORD  m_dwPrevTournamentStartDate;

protected:
	DWORD m_dwCheerTeam;

public:
	virtual bool LoadINI( ioINILoader &rkLoader );

public:
	static TournamentBot* ToTournamentBot( TestNode *pNode );
	static TournamentBot* GetTournamentBot( DWORD dwUserIdx );

public:
	virtual void OnCreate();
	virtual bool OnUpdate();
	virtual void OnDestroy();

protected:
	void ProcessTournamentJoinState( DWORD dwTourIdx );
	void ProcessCheerTryState( DWORD dwTourIdx );

protected:
	void CampJoinTry( DWORD dwTourIdx );
	void TournamentJoinTry( DWORD dwTourIdx );
	void TournamentJoinCheck( DWORD dwTourIdx );
	void TournamentReStartCheck( DWORD dwTourIdx );
	
	void TournamentCheerNone( DWORD dwTourIdx );
	void TournamentCheerTry( DWORD dwTourIdx );

public:
	void ApplyTeamCreateOk( DWORD dwTeamIndex );
	void ApplyTeamCreateFail();
	void ApplyUserTeamIndex( CQueryResultData *query_data );
	void ApplyCheerDecisionOK( SP2Packet &rkPacket );
	void ApplyCheerDecisionFail( int iResult );

public:
	TournamentBot();
	~TournamentBot();
};