// LogDBClient.cpp: implementation of the LogDBClient class.
//
//////////////////////////////////////////////////////////////////////

#include "../stdafx.h"
#include "LogDBClient.h"

#include "../MainProcess.h"
#include "../QueryData/QueryResultData.h"
#include "../Network/GameServer.h"
#include "../Network/iocpHandler.h"
#include "../Network/ioPacketQueue.h"
#include "../NodeInfo/ServerNodeManager.h"
#include <strsafe.h>
#include "../ioProcessChecker.h"
#include "../Local/ioLocalJapan.h"
#include "../NodeInfo/GoodsManager.h"


extern CLog LOG;
LogDBClient *LogDBClient::sg_Instance = NULL;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern BOOL tokenize(const std::string str, const std::string& delimiters, std::vector<std::string>& tokens);

LogDBClient::LogDBClient( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize ) : CConnectNode( s, dwSendBufSize, dwRecvBufSize )
{
	m_dwCurrentTime   = 0;
}

LogDBClient::~LogDBClient()
{

}

LogDBClient &LogDBClient::GetInstance()
{
	if(sg_Instance == NULL)
	{
		ioINILoader kLoader( "../global_define.ini" );
		kLoader.SetTitle( "GameLogDB Session" );
		int iSendBufferSize = kLoader.LoadInt( "SendBufferSize", MAX_BUFFER );	
		sg_Instance = new LogDBClient( INVALID_SOCKET, iSendBufferSize, MAX_BUFFER * 2 );

		LOG.PrintTimeAndLog( 0, "GameLogDB Session SendBuffer : %d", iSendBufferSize );
	}
	return *sg_Instance;
}

void LogDBClient::ReleaseInstance()
{
	SAFEDELETE(sg_Instance);
}

bool LogDBClient::ConnectTo()
{
	ioINILoader kLoader( "../global_define.ini" );
	kLoader.SetTitle( "DBA_Log" );

	char szValue[MAX_PATH];
	kLoader.LoadString( "1", "", szValue, MAX_PATH );

	std::string values = szValue;
	std::string delimiter = ":";
	std::vector<std::string> tokens;
	tokenize(values, delimiter, tokens);
	if(tokens.size() != 2) return false;

	ioHashString ServerIP;
	int iSSPort = 0;

	ServerIP = tokens[0].c_str();
	iSSPort= atoi(tokens[1].c_str());

	if( strcmp( ServerIP.c_str(), "" ) == 0 )
	{
		LOG.PrintTimeAndLog( 0, "[warning][logdb]LogDBClient::ConnectTo IP is empty" );
		return false;
	}

	 
	if( iSSPort == -1 )
	{
		LOG.PrintTimeAndLog( 0, "[warning][logdb]LogDBClient::ConnectTo PORT is empty" );
		return false;
	}


	SOCKET socket = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( socket == INVALID_SOCKET )
	{
		LOG.PrintTimeAndLog( 0, "[warning][logdb]LogDBClient::ConnectTo socket %d[%s:%d]", GetLastError(), ServerIP.c_str(), iSSPort );
		return false;
	}

	sockaddr_in serv_addr;
	serv_addr.sin_family		= AF_INET;
	serv_addr.sin_addr.s_addr	= inet_addr( ServerIP.c_str() );
	serv_addr.sin_port			= htons( iSSPort );
	if( ::connect( socket, (sockaddr*)&serv_addr, sizeof(serv_addr) ) != 0 )
	{
		LOG.PrintTimeAndLog( 0, "[warning][logdb]LogDBClient::ConnectTo connect %d[%s:%d]", GetLastError(), ServerIP.c_str(), iSSPort );
		return false;
	}

	g_iocp.AddHandleToIOCP( (HANDLE)socket, (DWORD)this );
	CConnectNode::SetSocket( socket );

	OnCreate();	
	if( !AfterCreate() )
	{
		LOG.PrintTimeAndLog( 0, "%s Error",__FUNCTION__);
	}
	LOG.PrintTimeAndLog( 0, "[info][logdb]OnConnect (IP:%s PORT:%d RESULT:%d)", ServerIP.c_str(), iSSPort, 0 );
	return true;
}

void LogDBClient::OnCreate()
{
	CConnectNode::OnCreate();

	m_dwCurrentTime = TIMEGETTIME();
}

void LogDBClient::OnDestroy()
{
	CConnectNode::OnDestroy();
	LOG.PrintTimeAndLog( 0, "[warning][logdb]LogDBClient : Disconnect" );
}

bool LogDBClient::SendMessage( CPacket &rkPacket )
{
	g_ProcessChecker.LogDBSendMessage( rkPacket.GetBufferSize() );
	return CConnectNode::SendMessage( rkPacket, TRUE);
}

bool LogDBClient::CheckNS( CPacket &rkPacket )
{
	return true;             //네트웍 감시 필요없다.
}

int LogDBClient::GetConnectType()
{
	return CONNECT_TYPE_LOGDB_SERVER;
}

void LogDBClient::SessionClose( BOOL safely )
{
	if(IsActive())
	{
		CPacket packet(ITPK_CLOSE_SESSION);
		ReceivePacket( packet );
	}
}
void LogDBClient::ReceivePacket( CPacket &packet )
{
	g_RecvQueue.InsertQueue( (DWORD)this, packet, PK_QUEUE_SESSION );
	g_ProcessChecker.LogDBRecvMessage( packet.GetBufferSize() );
}

void LogDBClient::OnClose( SP2Packet &packet )
{
	OnDestroy();
}

void LogDBClient::ProcessTime()
{
	if(TIMEGETTIME() - m_dwCurrentTime < 60000) return; // 1분 확인
	
	if( !IsActive() )
	{
		ConnectTo();
	}
	m_dwCurrentTime = TIMEGETTIME();
	OnPing();
}

void LogDBClient::PacketParsing( CPacket &packet )
{
	switch(packet.GetPacketID())
	{
	case ITPK_CLOSE_SESSION:
		OnClose( (SP2Packet&)packet );
		break;
	}
}

ValueType LogDBClient::GetValueType(VariableType nType,int len)
{
	ValueType vt;
	vt.type = nType;
	vt.size = len;
	return vt;
}

void LogDBClient::OnPing()
{
	SP2Packet kPacket(DTPK_QUERY_PING); // 리턴값 없음.
	if( !SendMessage( kPacket ) )
	{
		LOG.PrintTimeAndLog(0,"LogDB OnPing Send Fail! :%d",GetLastError());
		return;
	}
}
//////////////////////////////////////////////////////////////////////////
// SEND
//////////////////////////////////////////////////////////////////////////
void LogDBClient::OnInsertBillingServerError( ChannelingType eChannelingType, int iRet, const char *szDesc )
{
/*	char str_query[MAX_PATH] = "";
	memset(str_query, 0, sizeof(str_query));

	StringCbPrintf(str_query, sizeof(str_query), "exec log_error_billing_add %d, %d, '%s'", (int) eChannelingType, iRet, szDesc );

	CQueryData query_data;
	query_data.SetData( 0, _RESULT_NAUGHT, DBAGENT_CONNECT_USER_SET, _INSERTDB, str_query, (int)strlen(str_query), NULL, 0 );

	SP2Packet sendPacket(DTPK_QUERY);
	sendPacket<<query_data;
	if(!SendMessage(sendPacket))
	{
		LOG.PrintTimeAndLog(0,"LogDBClient::OnInsertConnectUser Send Fail! :%d",GetLastError());
		return;
	}*/
}

void LogDBClient::OnInsertJapanMileageLog( DWORD dwUserIndex, const ioHashString sPrivateID, const ioHashString sPublicID, const ioHashString sPublicIP, int iPresentType, int iValue1, int iValue2, bool bPresent, char *szLogNum, int iAddMileage, int iResultValue )
{
/*	if( !szLogNum )
	{
		LOG.PrintTimeAndLog(0,"%s szLogNum == NULL." , __FUNCTION__ );
		return;
	}

	DWORD dwGoodsNo = 0;
	ioHashString szGoodsName;
	g_GoodsMgr.GetGoodsInfoPresentMileage( iPresentType, iValue1, iValue2, bPresent, dwGoodsNo, szGoodsName );

	char szGoodsNo[MAX_PATH]="";
	StringCbPrintf( szGoodsNo, sizeof( szGoodsNo ), "%d", dwGoodsNo );

	char str_query[MAX_PATH*2] = "";
	memset(str_query, 0, sizeof(str_query));
	StringCbPrintf(str_query, sizeof(str_query), "exec log_item_mileage_add '%s', %d, '%s', %d, %d, %d, %d, %d, '%s', %d, '%s', '%s'", sPrivateID.c_str(), dwUserIndex, sPublicID.c_str(), iPresentType, iValue1, iValue2, iAddMileage, iResultValue, szLogNum, JMLT_ADD, sPublicIP.c_str(), szGoodsNo );

	CQueryData query_data;
	query_data.SetData( 0, _RESULT_NAUGHT, DBAGENT_MILEAGE_LOG_SET, _INSERTDB, str_query, (int)strlen(str_query), NULL, 0 );

	SP2Packet sendPacket(DTPK_QUERY);
	sendPacket<<query_data;
	if(!SendMessage(sendPacket))
	{
		LOG.PrintTimeAndLog(0,"%d Send Fail! :%d", __FUNCTION__, GetLastError());
		return;
	}*/
}
//////////////////////////////////////////////////////////////////////////
