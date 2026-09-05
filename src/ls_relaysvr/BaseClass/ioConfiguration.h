#pragma once


#include "UserDefineType.h"

class ioINILoader;

class ioConfiguration
{
public:
	ioConfiguration(void);
	virtual ~ioConfiguration(void);

public://Fuction
	bool Init();

public:
	void ClientInfoLoad( ioINILoader &kLoader );
	void ProcessCheckerInfoLoad( ioINILoader &kLoader );
	void CommonInfoLoad( ioINILoader &kLoader );
	void ServerConnectInfoLoad( ioINILoader &kLoader );
	void AcceptorInfoLoad( const char* szINI );

public:
	DWORD StrToDwordIP(char* publicIP);
	void ReadLoadConfig();
	void SetDefault();
	void Tokenize(const string& str,vector<string>& tokens,	const string& delimiters = " ");
	void GetMyIP(std::string &ipAddr);
	bool GetLocalIpAddress();

public://get/set fuction
	int GetWorkerCount() const					{ return m_workercount; }
	void SetNWorkerCount(int val)				{ m_workercount = val; }
	int GetQueueFirstRecvQueue() const			{ return m_queueFirstRecvQueue; }
	void SetQueueFirstRecvQueue(int val)		{ m_queueFirstRecvQueue = val; }
	std::string GetSIpAddr() const				{ return m_IpAddr; }
	void SetSIpAddr(std::string val)			{ m_IpAddr = val; }
	int GetPort() const							{ return m_port; }
	void SetPort(int val)						{ m_port = val; }
	int GetConnectMgrRecvSize() const			{ return m_connectmgrrecvsize; }
	void SetConnectMgrRecvSize(int val)			{ m_connectmgrrecvsize = val; }
	int GetConnectMgrSendSize() const			{ return m_cmgrsendsize; }
	void SetConnectMgrSendSize(int val)			{ m_cmgrsendsize = val; }
	int GetConnectMgrMaxPoolSize() const		{ return m_cmgrmaxpool; }
	void SetConnectMgrMaxPool(int val)			{ m_cmgrmaxpool = val; }
	int GetGameServerMaxSize() const			{ return m_serverAddrs.size(); }
	ConnectIPAddrs& ServerAddr()						{ return m_serverAddrs; }
	void ServerAddr(ConnectIPAddrs val)				{ m_serverAddrs = val; }
	int GetMaxLogCount() const					{ return m_maxlogcount; }
	void SetMaxLogCount(int val)				{ m_maxlogcount = val; }
	int GetLogTime() const						{ return m_logtime; }
	void SetLogTime(int val)					{ m_logtime = val; }
	int GetNagleTime() const					{ return nagleTime; }
	void SetNagleTime(int val)					{ nagleTime = val; }
	int GetConnectTime() const					{ return m_connecttime; }
	void SetConnectTime(int val)				{ m_connecttime = val; }
	int GetServerFullCount() const				{ return m_serverfullcount; }
	void SetServerFullCount(int val)			{ m_serverfullcount = val; }
	int GetRoomMaxCount() const					{ return m_roomMaxCount; }
	void SetRoomMaxCount(int val)				{ m_roomMaxCount = val; }
	int GetUseSecurity() const					{ return m_usesecurity; }
	void SetUseSecurity(int val)				{ m_usesecurity = val; }
	int GetUserInfoMax() const					{ return m_userInfoMax; }
	void SetUserInfoMax(int val)				{ m_userInfoMax = val; }
	TCHAR* GetIP()								{ return m_IP; }
	TCHAR* GetLogFolder()						{ return m_logFolder; }
	void SetINI(TCHAR* iniFile)					{ m_iniFile = iniFile; }
	std::string* GetINI()						{ return &m_iniFile; }
	char* GetPublicIP()							{ return m_publicIP;}
	char* GetPrivateIP()						{ return m_privateIP;}
	DWORD   GetDWIP()							{ return m_dwIP;}
	int GetRelayServerRoomMaxCount() const		{ return m_relayServerRoomMaxCount; }
	void SetRelayServerRoomMaxCount(int val)	{ m_relayServerRoomMaxCount = val; }
	int GetPrivateIPFirstByte() const			{ return m_privateIPFirstByte; }
	void SetPrivateIPFirstByte(int val)			{ m_privateIPFirstByte = val; }
	std::vector<int>& GetUdpPorts()				{ return m_udpPorts; }
	void SetUdpPorts(std::vector<int>& val)		{ m_udpPorts = val; }

private:
	TCHAR m_IP[64];
	int m_workercount;
	int m_queueFirstRecvQueue;
	std::string m_IpAddr;
	int m_port;
	DWORD m_dwIP;
	int m_privateIPFirstByte;
	std::vector<int> m_udpPorts;
	/************************************************************************/
	/* default																*/
	/************************************************************************/
	std::string m_iniFile;
	TCHAR m_logFolder[256];
	/************************************************************************/
	/* MainIP                                                                     */
	/************************************************************************/
	char m_publicIP[STR_IP_MAX];
	char m_privateIP[STR_IP_MAX];
	/************************************************************************/
	/* RelayServer Control                                                  */
	/************************************************************************/
	BOOL m_allblock;
	/************************************************************************/
	/* ClinetNodeInfo                                                       */
	/************************************************************************/
	int	m_connectmgrrecvsize;
	int	m_cmgrsendsize;
	int	m_cmgrmaxpool;
	int	m_maxwaitsecond;
	int m_usesecurity;
	/************************************************************************/
	/* GameServerInfo                                                                      */
	/************************************************************************/
	ConnectIPAddrs m_serverAddrs;
	int m_connecttime;
	int m_serverfullcount;
	int m_requestserverinfotime;
	int m_roomMaxCount;
	int m_userInfoMax;
	/************************************************************************/
	/* RelayServerInfo                                                                     */
	/************************************************************************/
	int m_relayServerRoomMaxCount;
	/************************************************************************/
	/* ioProcess / Packet checker                                                                     */
	/************************************************************************/
	int m_logtime;
	int m_chckerpasstime;
	int m_maxlogcount;
	/************************************************************************/
	/* nagle                                                                     */
	/************************************************************************/
	int nagleTime;
	/************************************************************************/
	/* 주기적 실행해야하는 것                                                                      */
	/************************************************************************/
	int m_clientghosttime;
	int m_scanclientghosttime;
};

