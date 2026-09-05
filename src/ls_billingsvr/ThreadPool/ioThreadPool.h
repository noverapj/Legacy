#ifndef _ioThreadPool_h_
#define _ioThreadPool_h_

#include "../Channeling/ioChannelingNodeParent.h"
#include "../LoginManager/LoginManager.h"
#include "../../include/MemPooler.h"

enum ThreadPoolType 
{
	TPT_NONE  = 0,
	TPT_CHANNELING = 1,
	TPT_LOCAL = 2,
};

class ioData : public SuperParent
{
public:
	enum ProcessType 
	{
		PT_NONE						  = 0,
		PT_GET_CASH					  = 1,
		PT_OUTPUT_CASH				  = 2,
		PT_LOGIN					  = 3,
		PT_CANCEL_CASH				  = 4,
		PT_FILL_CASH_URL			  = 5,
		PT_SUBSCRIPTION_RETRACT		  = 6,
		PT_QUEST_COMPLEATE			  = 7,
		PT_PRESENT					  = 8,
		PT_SUBSCRIPTION_RETRACT_CHECK = 9,
		PT_PRESENT_CASH				  =  10,
	};

protected:
	/*
	uint64_t	 iNexonSN = 0;
	
	*/
	ProcessType  m_eProcessType;
	bool         m_bEmpty;
	bool         m_bSetUserMouse;
	WORD		 m_wCancelFlag;
	int          m_iChannelingType;
	int          m_iServerPort;
	int          m_iItemPayAmt;
	int          m_iItemType;
	int          m_iUserLevel;
	int			 m_iIntID;
	int			 m_iReqNum;
	int          m_iItemValueList[MAX_ITEM_VALUE];
	

	DWORD		 m_dwServerNo; 
	DWORD		 m_dwCountryByIP;
	DWORD		 m_dwWin;
	DWORD		 m_dwDraw;
	DWORD		 m_dwLose;
	DWORD		 m_giveup;
	DWORD		 m_dwGold;
	DWORD        m_dwExp;
	DWORD		 m_dwGender;
	DWORD		 m_dwExitCode;
	DWORD        m_dwUserIndex;
	DWORD        m_dwReturnMsgType;
	DWORD        m_dwGoodsNo;
	DWORD        m_dwIndex;
	
	ioHashString m_PrivateIP;
	ioHashString m_ConnectTime;
	ioHashString m_DisconnectTime;
	ioHashString m_Country;
	ioHashString m_szBillingGUID;
	ioHashString m_szPrivateID;
	ioHashString m_szPublicID;
	ioHashString m_szUserNo;
	ioHashString m_sServerIP;
	ioHashString m_szUserIP;
	ioHashString m_szGoodsName;
	ioHashString m_szEncodePW; 
	ioHashString m_szUserMacAddress;
	ioHashString m_szUserKey;
	ioHashString m_szNexonUserNo;
	
	ioHashString	m_szAuthInfo;
	DWORD			m_dwSessionID;

	ioHashString m_szChargeNo;
	
	//hr
	ioHashString m_szQuestKey;
	
	//jal
	__int64		 m_iVCode;

	int m_iGameServerPort;
	ioHashString m_szReceivePrivateID;
	ioHashString m_szReceivePublicID;
	DWORD m_dwRecvUserIndex;

	TwoOfINTVec m_vBonusCashForConsume;

public:
	//get
	int GetChannelingType() const { return m_iChannelingType; }
	const ioHashString &GetBillingGUID() const { return m_szBillingGUID; }
	DWORD GetUserIndex() const { return m_dwUserIndex; }
	const ioHashString &GetPrivateID() const { return m_szPrivateID; }
	const ioHashString &GetPublicID() const { return m_szPublicID; }
	bool GetSetUserMouse() const { return m_bSetUserMouse; }
	const ioHashString &GetUserNo() const { return m_szUserNo; }
	const ioHashString &GetServerIP() const { return m_sServerIP; }
	int   GetServerPort() const { return m_iServerPort; }
	const ioHashString &GetUserIP() const { return m_szUserIP; }
	int GetItemPayAmt() const { return m_iItemPayAmt; }
	int GetItemType() const { return m_iItemType; }
	int GetItemValue( int iArray ) const;
	DWORD GetGoodsNo() const { return m_dwGoodsNo; }
	const ioHashString &GetGoodsName() const { return m_szGoodsName; }
	DWORD GetReturnMsgType() const { return m_dwReturnMsgType; }
	const ioHashString GetEncodePW() const { return m_szEncodePW; }
	int GetUserLevel() const { return m_iUserLevel; }
	const ioHashString GetUserMacAddress() const { return m_szUserMacAddress; }
	const ioHashString GetUserKey() const { return m_szUserKey; }
	const ioHashString GetAuthInfo() const { return m_szAuthInfo; }
	DWORD GetSessionID() const { return m_dwSessionID; }
	const ioHashString GetChargeNo() const { return m_szChargeNo; }
	WORD GetCancelFlag() const { return m_wCancelFlag; }
	DWORD GetIndex() const { return m_dwIndex; }
	__int64 GetUserVCode() const { return m_iVCode; }
	int GetUserIntID() const { return m_iIntID; }
	int GetUserReqNum() const { return m_iReqNum; }
	const ioHashString &GetConnectTime() const { return m_ConnectTime; }
	const ioHashString &GetDisConnectTime() const { return m_DisconnectTime; }
	DWORD GetServerNo() const { return m_dwServerNo; }
	const ioHashString &GetCountry() const { return m_Country; }
	DWORD GetCountryByIP() const { return m_dwCountryByIP; }
	const ioHashString &GetPrivateIP() const { return m_PrivateIP; }
	DWORD GetWin() const { return m_dwWin; }
	DWORD GetDraw() const { return m_dwDraw; }
	DWORD GetLose() const { return m_dwLose; }
	DWORD GetGiveup() const { return m_giveup; }
	DWORD GetGold() const { return m_dwGold; }
	DWORD GetExp() const { return m_dwExp; }
	DWORD GetGender() const { return m_dwGender; }
	DWORD GetExitCode() const { return m_dwExitCode; }	
	
	const ioHashString &GetQuestKey() const { return m_szQuestKey; }

	int GetGameServerPort() const { return m_iGameServerPort; }
	const ioHashString &GetReceivePrivateID() const { return m_szReceivePrivateID; }
	const ioHashString &GetReceivePublicID()  const { return m_szReceivePublicID; }
	DWORD GetRecvUserIndex() const { return m_dwRecvUserIndex; }
	void GetBonusCashInfo(TwoOfINTVec& vInfo) const { vInfo = m_vBonusCashForConsume; }

	//set
	void SetChannelingType(int iChannelingType) { m_iChannelingType = iChannelingType; }
	void SetBillingGUID( const ioHashString &rsBillingGUID) { m_szBillingGUID = rsBillingGUID; }
	void SetUserIndex(DWORD dwUserIndex) { m_dwUserIndex = dwUserIndex; }
	void SetPrivateID( const ioHashString &rsPrivateID) { m_szPrivateID = rsPrivateID; }
	void SetPublicID( const ioHashString &rsPublicID) { m_szPublicID = rsPublicID; }
	void SetSetUserMouse(bool bSetUserMouse) { m_bSetUserMouse = bSetUserMouse; }
	void SetUserNo( const ioHashString &rsUserNo) { m_szUserNo = rsUserNo; }
	void SetServerIP( const ioHashString &rsServerIP) { m_sServerIP = rsServerIP; }
	void SetServerPort(int iServerPort) { m_iServerPort = iServerPort; }
	void SetUserIP( const ioHashString &rUserIP) { m_szUserIP = rUserIP; }
	void SetItemPayAmt(int iItemPayAmt) { m_iItemPayAmt = iItemPayAmt; }
	void SetItemType(int iItemType) { m_iItemType = iItemType; }
	void SetItemValueList(int iItemValueList[MAX_ITEM_VALUE]);
	void SetGoodsNo(DWORD dwGoodsNo) { m_dwGoodsNo = dwGoodsNo; }
	void SetGoodsName( const ioHashString &rsGoodsName) { m_szGoodsName = rsGoodsName; }
	void SetReturnMsgType( DWORD dwReturnMsgType ) { m_dwReturnMsgType = dwReturnMsgType; }
	void SetEncodePW( const ioHashString &rszEncodePW) { m_szEncodePW = rszEncodePW; }
	void SetUserLevel( int iUserLevel ) { m_iUserLevel = iUserLevel; }
	void SetUserMacAddress( const ioHashString &rszUserMacAddress) { m_szUserMacAddress = rszUserMacAddress; }
	void SetUserKey( const ioHashString &rszUserKey) { m_szUserKey = rszUserKey; }
	void SetAuthInfo( const ioHashString &rszAuthInfo) { m_szAuthInfo= rszAuthInfo; }
	void SetSessionID(DWORD dwSessionID) { m_dwSessionID = dwSessionID; }
	void SetChargeNo( const ioHashString& val) { m_szChargeNo = val; }
	void SetCancelFlag(WORD val) { m_wCancelFlag = val; }
	void SetIndex(DWORD val) { m_dwIndex = val; }
	void SetUserVCode(__int64 VCode) { m_iVCode = VCode; }
	void SetUserIntID(int intID) { m_iIntID = intID; }
	void SetUserReqNum(int reqNum) { m_iReqNum = reqNum; }	
	
	void  SetConnectTime(ioHashString & connectTime) { m_ConnectTime = connectTime; }
	void  SetDisConnectTime(ioHashString &disConnectTime) { m_DisconnectTime = disConnectTime; }
	void  SetServerNo(int serverNo)  { m_dwServerNo = serverNo; }
	void  SetCountry(ioHashString &country) { m_Country = country; }
	void  SetCountryByIP(int countryByIP)  { m_dwCountryByIP = countryByIP; }
	void  SetPrivateIP(ioHashString &privateIP) { m_PrivateIP = privateIP; }
	void  SetWin(int win)  { m_dwWin = win; }
	void  SetDraw(int draw) { m_dwDraw = draw; }
	void  SetLose(int lose) { m_dwLose = lose; }
	void  SetGiveup(int giveup) { m_giveup = giveup; }
	void  SetGold(int gold) { m_dwGold = gold; }
	void  SetExp(int exp) { m_dwExp = exp; }
	void  SetGender(int gender) { m_dwGender = gender; }
	void  SetExitCode(int exitCode) { m_dwExitCode = exitCode; }	
	
	void  SetQuestKey(ioHashString & rszQuestKey) { m_szQuestKey = rszQuestKey; }

	void SetGameServerPort(int val) { m_iGameServerPort = val; }
	void SetReceivePrivateID(ioHashString& val) { m_szReceivePrivateID = val; }
	void SetReceivePublicID(ioHashString& val) { m_szReceivePublicID = val; }
	void SetRecvUserIndex(DWORD val) { m_dwRecvUserIndex = val; }
	const ioHashString& GetNexonUserNo() const { return m_szNexonUserNo; }
	void SetNexonUserNo(ioHashString& val) { m_szNexonUserNo = val; }

	void AddBonusCashInfoForConsume(const int iIndex, const int iValue);

public:
	void Clear();
	bool IsEmpty() const;
	void SetEmpty( bool bEmpty ) { m_bEmpty = bEmpty; }

	ioData::ProcessType GetProcessType() const { return m_eProcessType; }
	void SetProcessType(ioData::ProcessType eProcessType) { m_eProcessType = eProcessType; }

public:
	ioData& operator=( const ioData &rhs );

public:
	ioData();
	virtual ~ioData();
};

///////////////////////////////////////////////////////////////////////////
class ioBillThread : public Thread
{
protected:
	HANDLE m_hKillEvent;
	HANDLE m_hWaitEvent;
    
protected:
	bool   m_bStopThread;
	bool   m_bActiveThread;
	int    m_iQueryProcessCount;

	ThreadPoolType m_eThreadPoolType;

	typedef std::vector< ioData * >  ioDataVec;
	ioDataVec						 m_vData;
	MemPooler<ioData>				 m_MemNode;
	LoginManager					 m_LoginMgr;

protected:
	void ProcessLoop();
	void ProcessWeb( const ioData &kData );

	void InitMemoryPool();
	void ReleaseMemoryPool();

public:
	void OnCreate();
	void OnDestroy();

public:
	virtual void Run();

public:
	inline bool IsThreadStop(){ return m_bStopThread; }
	inline bool IsActiveThread(){ return m_bActiveThread; }

	int  GetNodeSize(){ return m_vData.size(); }
	int  RemainderNode(){ return m_MemNode.GetCount(); }


	virtual int GetLoginNodeSize(){ return m_LoginMgr.GetLoginNodeSize(); }
	virtual int RemainderLoginNode(){ return m_LoginMgr.RemainderLoginNode(); }

public:
	bool SetData( const ioData &rData );
	void SetThreadPoolType(ThreadPoolType eThreadPoolType) { m_eThreadPoolType = eThreadPoolType; }

public:
	ioBillThread();
	virtual ~ioBillThread();
};
//////////////////////////////////////////////////////////////////////////
class ioThreadPool : public SuperParent
{
private:
	static ioThreadPool *sg_Instance;

public:
	static ioThreadPool &GetInstance();
	static void ReleaseInstance();

protected:
	typedef std::vector< ioBillThread * > ioThreadVec;
	ioThreadVec m_vThread;

	ThreadPoolType m_eThreadPoolType;

protected:
	void Clear();
 
public:
	virtual void Initialize();

public:
	void SetData( const ioData &rData );

public:
	int GetActiveThreadCount();
	int GetThreadCount() { return m_vThread.size(); }

	int  GetNodeSize();
	int  RemainderNode();

	int  GetLoginNodeSize();
	int  RemainderLoginNode();

	void SetThreadPoolType(ThreadPoolType eThreadPoolType) { m_eThreadPoolType = eThreadPoolType; }

private:
	ioThreadPool();
	virtual ~ioThreadPool();
};
#define g_ThreadPool ioThreadPool::GetInstance()

#endif