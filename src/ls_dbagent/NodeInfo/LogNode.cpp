#include "../stdafx.h"
#include "../ThreadPool/ioThreadPool.h"
#include "../Network/DBServer.h"
#include "../Network/SP2Packet.h"
#include "../Network/ioPacketQueue.h"
#include "../Database/cQueryManager.h"
#include "LogNodeManager.h"
#include "LogNode.h"
#include <strsafe.h>


LogNode::LogNode( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize ) : CConnectNode( s, dwSendBufSize, dwRecvBufSize )
{
	ZeroMemory(m_IP,sizeof(m_IP));
	m_port = 0;
	m_daemonName = "";
}

LogNode::~LogNode(void)
{
}

void LogNode::OnCreate()
{
	CConnectNode::OnCreate();
	//SOCKET으로 IP 얻는다.
	SOCKADDR_IN sockaddrClient;

	int sockaddrinSize=sizeof(sockaddrClient);

	::getpeername(m_socket,(LPSOCKADDR)&sockaddrClient,&sockaddrinSize);
	SetClientAddr(sockaddrClient);
	m_tcpPort = 0;
}

void LogNode::OnDestroy()
{
	CConnectNode::OnDestroy();
}

void LogNode::SessionClose( BOOL safely/*=TRUE */ )
{
	if(IsActive())
	{
		CPacket packet(ITPK_CLOSE_SESSION);
		ReceivePacket( packet );
	}
}

void LogNode::ReceivePacket( CPacket &packet )
{ 
	g_RecvQueue.InsertQueue((DWORD)this, packet, PK_QUEUE_SESSION);
}

void LogNode::PacketParsing( CPacket &packet )
{
	SP2Packet& kPacket = reinterpret_cast<SP2Packet&>(packet);

	switch(packet.GetPacketID())
	{
	case ITPK_CLOSE_SESSION:
		{
			OnClose( (SP2Packet&)packet );
		}
		break;

	case LTPK_LOG:
		{
			OnParseLog(kPacket);
			return;
		}
		break;
	}
}

void LogNode::OnConnect( SP2Packet& kPacket, LOGMessageHeader &logHeader )
{
	char daemonName[MAX_DAMONE_NAME_SIZE];
	if(!kPacket.Read( sizeof(daemonName), daemonName )) return;

	m_tcpPort		= logHeader.m_logLevel;
	m_daemonName	= daemonName;

	GetPeerIP();

	LOG.PrintTimeAndLog(0,"LogNode:: OnConnect (%s)[%s:%d]",daemonName,m_IP,m_port);
	Debug("LogNode:: OnConnect (%s)[%s:%d]\n",daemonName,m_IP,m_port);

}

void LogNode::OnClose( SP2Packet &packet )
{
	CloseFiles("TCP DisConnected");
	OnDestroy();
	g_LogNodeManager->DelLogNode( this );
	DestoryFiles();
}

void LogNode::OnParseLog( SP2Packet& kPacket )
{
	LOGMessageHeader logHeader;
	if(!kPacket.Read(logHeader)) return;

	switch(logHeader.m_logMessageType)
	{
	case LOG_MESSAGE_TYPE_ON_CONNECT:
		{
			OnConnect(kPacket, logHeader);
		}
		break;

	case LOG_MESSAGE_TYPE_TERMINATE:
	case LOG_MESSAGE_TYPE_CLOSE:
		{
			OnLogFileCLose(kPacket);
		}
		break;

	case LOG_MESSAGE_TYPE_OPEN: //
		{
			OnLogFileOpen(kPacket);
		}
		break;

	case LOG_MESSAGE_TYPE_PROCESS:
		{
			OnLogProcess(kPacket, logHeader);
			return;
		}
		break;
	}
}

void LogNode::OnLogFileOpen( SP2Packet& kPacket )
{
	int categoryIndex = 0;
	char categoryName[MAX_CATEGORY_NAME_SIZE];
	SYSTEMTIME st;

	if(!kPacket.Read(categoryIndex))						return;
	if(!kPacket.Read(sizeof(categoryName), categoryName))	return;
	if(!kPacket.Read(st))									return;

	SetCategoryName(categoryIndex,categoryName);

	IsOpenFile(categoryIndex,st);
}

void LogNode::OnLogProcess( SP2Packet& kPacket, LOGMessageHeader &logHeader )
{
	LOGMessage logData;
	if(!kPacket.Read(logData)) return;

	CLog* logTmp = GetCLog(logData.categoryIndex);
	if(logTmp == NULL)	return;

	switch(logHeader.m_logRecordType)
	{
	case LOGRECORD_PRINTTIMEANDLOG: //여기는 디버그로그 레벨 정보 추가하여 할것
		{
			logTmp->PrintTimeAndLog(0,logData.m_logMessage);
		}
		break;

	case LOGRECORD_DEBUGLOG: //여긴 나중에 정보 더 추가하여 전송받아서 할것
		{
			logTmp->DebugLog(0,logData.m_fileLine,logData.m_logMessage);
		}
		break;

	case LOGRECORD_PRINTLOG:
		{
			logTmp->PrintLog(0,logData.m_logMessage);
		}
		break;

	case LOGRECORD_PRINTNOENTERLOG:
		{
			logTmp->PrintNoEnterLog(0,logData.m_logMessage);
		}
		break;
	}
}

void LogNode::OnLogFileCLose( SP2Packet& kPacket )
{
	int categoryIndex = 0;
	if(!kPacket.Read(categoryIndex)) return;

	CLog* logTmp = GetCLog(categoryIndex);
	if(logTmp)
		logTmp->CloseLog(); // 클로즈는 디스콘넥트가 됐을때만 
}

CLog* LogNode::IsOpenFile( int categoryIndex, SYSTEMTIME &st )
{
	CLOGMAP::iterator pos = m_logMap.find(categoryIndex);

	if(pos == m_logMap.end())
	{
		const char* categoryName = NULL;
		CLog* logTmp = NULL;
		char TimeLogName[MAX_PATH]="";

		logTmp = g_LogNodeManager->CreateCLog();
		categoryName = GetCategoryName(categoryIndex);
		SetLogFileName( TimeLogName,MAX_PATH,categoryName,st);

		m_logMap.insert(CLOGMAP::value_type(categoryIndex,logTmp));

		logTmp->OpenLog(0,TimeLogName,true);

		return logTmp;
	}
	else
	{
		const char* categoryName = NULL;
		CLog* logTmp =  (*pos).second;
		char TimeLogName[MAX_PATH]="";

		categoryName =  GetCategoryName(categoryIndex);
		SetLogFileName( TimeLogName,MAX_PATH,categoryName,st);

		logTmp->OpenLog(0,TimeLogName,true);
	}
	return NULL;
}

void LogNode::SetLogFileName( IN OUT char* TimeLogName, IN int nameSize, IN const char* categoryName, IN const SYSTEMTIME& st)
{
	char szCurTime[MAX_PATH] = "";

	StringCbPrintf(szCurTime, sizeof(szCurTime), "%04d%02d%02d", st.wYear, st.wMonth, st.wDay );
	memset(TimeLogName, 0, nameSize);

	StringCbPrintf(TimeLogName, nameSize, 
		"%s\\%s\\%s-%s_%d.log", m_daemonName.c_str(), categoryName,szCurTime,GetIP(),m_tcpPort);

	Debug("FileName:%s\n",TimeLogName);
}

CLog* LogNode::GetCLog( int categoryIndex )
{
	CLOGMAP::iterator pos = m_logMap.find(categoryIndex);

	if(pos != m_logMap.end())
	{
		CLog* logTmp = (*pos).second;
		return logTmp;
	}
	else
		return NULL;

}

void LogNode::CloseFiles(char* closeString)
{
	CLOGMAP::iterator pos = m_logMap.begin();
	
	while(pos != m_logMap.end())
	{
		CLog* logTmp = (*pos).second;
		logTmp->PrintTimeAndLog(0, "<<< --------------------  End File(:%s)-------------------- >>>",closeString);
		logTmp->CloseLog();
		pos++;
	
	}
}

void LogNode::DestoryFiles()
{
	CLOGMAP::iterator pos = m_logMap.begin();

	while(pos != m_logMap.end() )
	{
		CLog* logTmp = (*pos).second;
		g_LogNodeManager->PushCLog(logTmp);
		pos++;
	
	}
	m_logMap.clear();
}

const char* LogNode::GetCategoryName( int index )
{
	CATEGORYMAP::iterator pos = m_categoryMap.find(index);
	
	if(pos != m_categoryMap.end())
	{
		std::string& strTmp = (*pos).second;
		return strTmp.c_str();
	}
	return NULL;
}

void LogNode::SetCategoryName( const int categoryIndex, const char* categoryName )
{
	auto pos = m_categoryMap.insert(CATEGORYMAP::value_type(categoryIndex,categoryName));
	if(pos.second == false)
	{
		std::string& strTmp = (*pos.first).second;
		strTmp = categoryName;
	}
}

BOOL LogNode::SendPacket( SP2Packet &packet )
{
	return 0;
}

void LogNode::GetPeerIP()
{
	CConnectNode::GetPeerIP(m_IP,sizeof(m_IP),m_port);
}
