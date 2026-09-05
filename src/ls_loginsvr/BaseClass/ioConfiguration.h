#pragma once

#include "UserDefineType.h"
#include "../IPBlocker/ioIPBlocker.h"

class ioConfiguration
{
public:
	ioConfiguration(void);
	virtual ~ioConfiguration(void);

public:
	bool Init();

	void ReadLoadConfig();
	void SetDefault();
	void Tokenize(const string& str,vector<string>& tokens,	const string& delimiters = " ");
	BOOL CheckBlackList(char* IP);
	BOOL CheckWhiteList(char* IP);

public:
	int NWorkerCount() const				{ return m_workerCount; }
	void NWorkerCount(int val)				{ m_workerCount = val; }
	int NQueueFirstRecvQueue() const		{ return m_queueFirstRecvQueue; }
	void NQueueFirstRecvQueue(int val)		{ m_queueFirstRecvQueue = val; }
	std::string SIpAddr() const				{ return m_IpAddr; }
	void SIpAddr(std::string val)			{ m_IpAddr = val; }
	int NPort() const						{ return m_port; }
	void NPort(int val)						{ m_port = val; }
	BOOL NChannel() const					{ return m_port; }
	int NCMgrRecvSize() const				{ return m_connectMgrRecvSize; }
	void NCMgrRecvSize(int val)				{ m_connectMgrRecvSize = val; }
	int NCMgrSendSize() const				{ return m_cmgrSendSize; }
	void NCMgrSendSize(int val)				{ m_cmgrSendSize = val; }
	int NCMgrMaxPool() const				{ return m_cmgrMaxPool; }
	void NCMgrMaxPool(int val)				{ m_cmgrMaxPool = val; }
	int NGameServerMax() const				{ return m_vServerAddr.size(); }
	VIPADDR& ServerAddr()					{ return m_vServerAddr; }
	void ServerAddr(VIPADDR val)			{ m_vServerAddr = val; }
	int CheckerPassTime() const				{ return m_checkPassTime; }
	void CheckerPassTime(int val)			{ m_checkPassTime = val; }
	int MaxLogCount() const					{ return m_maxLogCount; }
	void MaxLogCount(int val)				{ m_maxLogCount = val; }
	int LogTime() const						{ return m_logTime; }
	void LogTime(int val)					{ m_logTime = val; }
	int NagleTime() const					{ return m_nagleTime; }
	void NagleTime(int val)					{ m_nagleTime = val; }
	int ConnectTime() const					{ return m_connectTime; }
	void ConnectTime(int val)				{ m_connectTime = val; }
	int ServerFullCount() const				{ return m_serverFullCount; }
	void ServerFullCount(int val)			{ m_serverFullCount = val; }
	int ClientGhostTime() const				{ return m_clientGhostTime; }
	void ClientGhostTime(int val)			{ m_clientGhostTime = val; }
	int ScanClientGhostTime() const			{ return m_checkGhostTime; }
	void ScanClientGhostTime(int val)		{ m_checkGhostTime = val; }
	int MaxWaitSecond() const				{ return m_maxWaitSecond; }
	void MaxWaitSecond(int val)				{ m_maxWaitSecond = val; }
	int UseSecurity() const					{ return m_useSecurity; }
	void UseSecurity(int val)				{ m_useSecurity = val; }
	int RequestServerInfoTime() const		{ return m_requestServerInfoTime; }
	void RequestServerInfoTime(int val)		{ m_requestServerInfoTime = val; }
	BOOL Allblock() const					{ return m_allblock; }
	void Allblock(BOOL val)					{ m_allblock = val; }
	int UserInfoMax() const					{ return m_userInfoMax; }
	void UserInfoMax(int val)				{ m_userInfoMax = val; }
	TCHAR* GetIP()							{ return m_IP; }
	TCHAR* GetLogFolder()					{ return m_logFolder; }
	void SetINI(TCHAR* iniFile)				{ m_iniFile = iniFile; }
	std::string* GetINI()					{ return &m_iniFile; }
	int CheckUserTimeout() const			{ return m_checkUserTimeout; }
	int CheckPingTime() const				{ return m_checkPingTime; }
	int GetPartitionTimer() const			{ return m_partitionTimer; }
	void SetPartition(int val)				{ m_partition = val; }
	int GetPartition()						{ return m_partition; }
	void SetWhiteList(BOOL whiteListOn)		{ m_whiteListOn = whiteListOn; }
	BOOL IsWhiteListOn()					{ return m_whiteListOn; }

private:
	TCHAR       m_IP[64];
	int			m_workerCount;
	int			m_queueFirstRecvQueue;
	std::string m_IpAddr;
	int			m_port;
	BOOL		m_channel;

	// default
	std::string m_iniFile;
	TCHAR		m_logFolder[256];

	// LoginServer Control
	BOOL		m_allblock;

	// ClinetNodeInfo
	int			m_connectMgrRecvSize;
	int			m_cmgrSendSize;
	int			m_cmgrMaxPool;
	int			m_maxWaitSecond;
	int         m_useSecurity;
	int			m_userInfoMax;

	// GameServerInfo
	VIPADDR		m_vServerAddr;
	int			m_connectTime;
	int			m_serverFullCount;
	int			m_requestServerInfoTime;

	// 검사데이타
	int			m_logTime;
	int			m_checkPassTime;
	int			m_maxLogCount;
	int			m_nagleTime;
	int			m_clientGhostTime;

	// 주기적 실행해야하는 것
	int			m_checkGhostTime;
	int			m_checkUserTimeout;
	int			m_checkPingTime;
	int			m_partitionTimer;

	// 서버 파티션
	int			m_partition;

	// IP블럭
	ioIPBlocker m_blackList;
	ioIPBlocker m_whiteList;
	BOOL		m_whiteListOn;
};


