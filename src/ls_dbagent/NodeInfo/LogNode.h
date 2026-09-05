#pragma once

#include <unordered_map>

class CConnectNode;
class SP2Packet;

class LogNode : public CConnectNode
{
public:
	LogNode( SOCKET s=INVALID_SOCKET, DWORD dwSendBufSize=0, DWORD dwRecvBufSize=0 );
	virtual ~LogNode(void);

public:
	virtual void OnCreate();
	virtual void OnDestroy();	
	virtual int  GetConnectType(){ return 0; }

public:
	virtual void SessionClose( BOOL safely=TRUE );
	virtual void ReceivePacket( CPacket &packet );	
	virtual void PacketParsing( CPacket &packet );

protected:
	void OnConnect( SP2Packet& kPacket, LOGMessageHeader &logHeader );
	void OnClose( SP2Packet &packet );
	void OnParseLog( SP2Packet& kPacket );
	void OnLogFileOpen( SP2Packet& kPacket );
	void OnLogProcess( SP2Packet& kPacket, LOGMessageHeader &logHeader );
	void OnLogFileCLose(SP2Packet& kPacket);

public:
	CLog* IsOpenFile( int categoryIndex, SYSTEMTIME &st );
	void SetLogFileName( IN OUT char* TimeLogName, IN int nameSize, IN const char* categoryName, IN const SYSTEMTIME& st);
	CLog* GetCLog(int categoryIndex);
	void CloseFiles(char* closeString);
	void DestoryFiles();
	const char* GetCategoryName(int index);
	void SetCategoryName(const int categoryIndex, const char* categoryName);
 
public:
	BOOL SendPacket( SP2Packet &packet );
	void GetPeerIP();
	const char* GetIP()	{ return m_IP; }
	const int GetPort()	{ return m_port; }

protected:
	typedef std::unordered_map<int,CLog*> CLOGMAP;
	typedef std::unordered_map<int,std::string> CATEGORYMAP;
	CLOGMAP m_logMap;
	CATEGORYMAP m_categoryMap;
	char m_IP[64];
	int m_port;
	int m_tcpPort;
	std::string m_daemonName;
	 
};

