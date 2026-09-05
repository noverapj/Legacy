#include "stdafx.h"
#include "ioTCPConnectNode.h"
#include "LOGPacket.h"
#include "ioLogQueue.h"
#include "../include/cSingleton.h"
#include "LogData.h"
#include "ioLogger.h"
#include "ioLogDataPool.h"
#include "../include/Log.h"

#pragma comment(lib, "ws2_32.lib")

namespace TCPNODE
{
	int Tokenizer(const std::string& str,const std::string& delimiters, std::vector<std::string>& tokens)
	{
		std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
		std::string::size_type pos = str.find_first_of(delimiters, lastPos);
		while (std::string::npos != pos || std::string::npos != lastPos)
		{
			tokens.push_back(str.substr(lastPos, pos - lastPos));
			lastPos = str.find_first_not_of(delimiters, pos);
			pos = str.find_first_of(delimiters, lastPos);
		}
		return tokens.size();
	}
}
using namespace TCPNODE;

ioTCPConnectNode::ioTCPConnectNode(void)
{
	InitData();
	InitMemoryPool();
}

ioTCPConnectNode::~ioTCPConnectNode(void)
{
}

void ioTCPConnectNode::InitData()
{
	WSADATA WOSAdata;
	if(WSAStartup(0x0002, &WOSAdata) != 0)
		return;

	SetSockState(DISCONNECTED);
	SetPort(0);
	SetSvrIpAddress("");
	SetSvrPort(0);

 
	SetPrintState(FALSE);
	m_logger = NULL;
	m_categoryIndex = 0;
	m_packet.Clear();
	m_recvIO.Init(16384*2+1);
	m_recvIO.InitRecvIO();
}

void ioTCPConnectNode::InitMemoryPool()
{
	m_contextPool.CreatePool(100,5000); //맥스값 어떻게 할지 정해야함 
}

int ioTCPConnectNode::InitSocket(int port)
{
	SetPort(port);
	ReadyToSocket(GetPort(), 1);
	return 0;
}

void ioTCPConnectNode::SetServerIP( const char* ipAddr, int port )
{
	m_svrPort = port;
	SetSvrIpAddress(ipAddr);

	memset(&m_svrAddr, 0, sizeof(m_svrAddr));

	m_svrAddr.sin_family = AF_INET;
	m_svrAddr.sin_port = htons(port);
	inet_pton(AF_INET, ipAddr, &m_svrAddr.sin_addr);
}

void ioTCPConnectNode::SetReserveData( CLogData* logData )
{
	BOOL bFind = FALSE;
	if(logData->GetType() == LOG_MESSAGE_TYPE_OPEN)
	{
		for(UINT i=0; i< m_reserveFiles.size(); ++i)
		{
			if(m_reserveFiles[i].m_instance == logData->GetInstance()) 
			{
				ReserveLogData& rLogData = m_reserveFiles[i];

				CopyMemory(&rLogData.m_st,logData->GetSt(),sizeof(rLogData.m_st));
				CopyMemory(rLogData.m_openFileName, logData->GetFileLine(), sizeof(rLogData.m_openFileName));

				bFind = TRUE;
				break;
			}
		}
		if(bFind == FALSE)
		{
			ReserveLogData rLogData;

			rLogData.m_instance = logData->GetInstance();
			rLogData.m_logRecordType = logData->GetRecordType();
			rLogData.m_logMessageType = logData->GetType();
			CopyMemory(&rLogData.m_st, logData->GetSt(), sizeof(rLogData.m_st));
			CopyMemory(rLogData.m_openFileName, logData->GetFileLine(), sizeof(rLogData.m_openFileName));
	
			m_reserveFiles.push_back(rLogData);
		}
	}
}

int ioTCPConnectNode::ReadyToSocket( int port, int makeNonBlocking )
{

	if(CreateSocket() == SOCKET_ERROR)
	{
		PrintTimeAndLog("Create Socekt Error");
		closesocket(m_sock);
	}
	
	if(BindSocket(port) == SOCKET_ERROR)
	{
		PrintTimeAndLog("bind() error (port number: %d): ", port);
		closesocket(GetSocketHandle());
		SetSockState(DISCONNECTED);
	}

	if (makeNonBlocking) 
	{
		if (MakeSocketNonBlocking(m_sock) == SOCKET_ERROR)
		{
			PrintTimeAndLog("failed to make non-blocking");
			closesocket(m_sock);
			SetSockState(DISCONNECTED);
			return -1;
		}
	}

	return m_sock;
}

int ioTCPConnectNode::BindSocket( int port )
{
	memset(&m_addr, 0, sizeof(m_addr));
	m_addr.sin_addr.s_addr = INADDR_ANY;
	m_addr.sin_family = AF_INET;

	if (bind(GetSocketHandle(), (struct sockaddr*)&m_addr, sizeof m_addr) != 0) 
		return SOCKET_ERROR;
	return 1;
}

int ioTCPConnectNode::CreateSocket()
{
	int reuseFlag = 1;
	m_sock = socket(AF_INET, SOCK_STREAM, 0);

	if (GetSocketHandle() < 0) return SOCKET_ERROR;

	if (setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR,
		(const char*)&reuseFlag, sizeof reuseFlag) < 0) 
		return SOCKET_ERROR;
	return 1;
}

int ioTCPConnectNode::MakeSocketNonBlocking( SOCKET sock )
{
	unsigned long arg = 1;
	if(ioctlsocket(sock, FIONBIO, &arg) == 0)
		return 1;
	else
		return SOCKET_ERROR;
}

void ioTCPConnectNode::SetSocketOption(const char* ipAddr, const int port, int timeOut, fd_set& set, timeval& tvout)
{
	SetServerIP(ipAddr,port);

	FD_ZERO(&set);
	tvout.tv_sec = 0; 
	tvout.tv_usec = timeOut*1000;
	FD_SET(GetSocketHandle(), &set);
}

BOOL ioTCPConnectNode::Connect( const char* ipAddr,const int port)
{
	int socketReturn = FALSE;;
	fd_set set;
	timeval tvout;

	SetSocketOption(ipAddr,port,100,set,tvout);

	if( connect(m_sock, (struct sockaddr *)&m_svrAddr, sizeof(m_svrAddr)) == SOCKET_ERROR )
		socketReturn = SOCKET_ERROR;
		
	
	if (socketReturn != 0) 
	{
		if ( WSAGetLastError() != WSAEINPROGRESS && WSAGetLastError() != WSAEWOULDBLOCK)
		{  
			SessionClose(-1);
			return socketReturn;
		}
		if (select(m_sock+1, NULL, &set, NULL, &tvout) <= 0)
		{
			SessionClose(-1);
			return socketReturn;
		}
	}
 
	socketReturn = AfterCreate();
	return socketReturn;
}

BOOL ioTCPConnectNode::AfterCreate()
{
	CreateIoCompletionPort((HANDLE)m_sock,g_logQueue->CompletionPort(),reinterpret_cast<ULONG_PTR>(this),0);
	if ( WaitForReceive() == SOCKET_ERROR )
		return FALSE;

	SetSockState(CONNECTED);
	SendOnConnect();

	SetPrintState(FALSE);
	PrintTimeAndLog("LogServer Connect (%s:%d)",m_ServerIP,m_svrPort);
	return TRUE;
}

int ioTCPConnectNode::WaitForReceive()
{
	ReadyToReceive();

	DWORD dwFlags = 0;
	DWORD dwBytes = 0;	
	int addrLen = sizeof(m_addr);

	int recvReturn =  WSARecv(GetSocketHandle(),
		m_recvIO.GetWsaBuf(),
		1,
		&dwBytes,
		&dwFlags,
		m_recvIO.GetOveralapped(),
		NULL);

	if(recvReturn == SOCKET_ERROR)
	{
		if(WSAGetLastError() != WSA_IO_PENDING)
		{
			SessionClose(-1);
		}
		recvReturn = 1;

		return recvReturn;
	}

	else 
		return recvReturn;
	
}

void ioTCPConnectNode::ReadyToReceive()
{
	m_recvIO.ReadyToReceive();
}

int ioTCPConnectNode::SendMessage( const char* buf,const int size )
{
	if(GetSockState() == CONNECTED)
	{
		int sendReturn = send(m_sock,buf,size,0);

		if(sendReturn == SOCKET_ERROR)
		{
			//로그처리 어떻게 할것인지 고민 
			PrintTimeAndLog("SendMessage Fail : %d",GetLastError());
		}
		return sendReturn;
	}
	return -1;
}

int ioTCPConnectNode::SendMessage( LOGPacket& pk )
{
	return SendMessage(pk.GetBuffer(),pk.GetBufferSize());
}
	
int ioTCPConnectNode::SendMessage( CLogData* logData )
{
	if(logData)
	{
		SetReserveData(logData);
		if(ExceptionSend(logData) == -1)
			return -1;

		LOGPacket kPacket(LTPK_LOG);
		LOGMessageHeader stHeader;

		stHeader.m_logMessageType = logData->GetType();
		stHeader.m_logRecordType = logData->GetRecordType();
		stHeader.m_logLevel = logData->GetLogLevel();
		kPacket << stHeader; 

		SetLoggerPacket(logData,kPacket);
		
		int rtVal = SendMessage(kPacket);

		if(rtVal != -1)	
		{
			logData->Init();
			g_logDataPool->Push(logData);
		}
		return rtVal;
	}
	return 0;
}

int ioTCPConnectNode::DispatchReceive( DWORD bytesTransferred )
{
	m_recvIO.AddBytesTransferred( bytesTransferred );

	while( m_recvIO.GetBytesTransferred() > 0 )
	{
		m_packet.SetBufferCopy( m_recvIO.GetBuffer(), m_recvIO.GetBytesTransferred() );
		if( (m_packet.IsValidPacket()) && (m_recvIO.GetBytesTransferred() >= (DWORD)m_packet.GetBufferSize()) )
		{
			int packetSize = m_packet.GetBufferSize();
			PacketParsing(reinterpret_cast<LOGBufferedContext*>(m_recvIO.GetOveralapped()));

			m_recvIO.AfterReceive( packetSize );
		}
		else 
			break;
	}

	return WaitForReceive();
}

void ioTCPConnectNode::SessionClose( int errCode )
{
	BOOL reuse = TRUE;

	::setsockopt(m_sock,SOL_SOCKET,SO_REUSEADDR,(TCHAR*)&reuse,sizeof(reuse));
	closesocket(m_sock);
	SetSockState(DISCONNECTED);

	PushOperation(FLAG_CLOSE);
}

void ioTCPConnectNode::PacketParsing( LOGBufferedContext* operationContext )
{
	switch(operationContext->m_flags)
	{
	case FLAG_CLOSE:
		{
			OnClose();
		}
		break;

	case FLAG_RECEIVE:
		{
			ParseReceivePacket(); 
		}
		break;

	case FLAG_CONNECT:
		{
			RequestConnect(operationContext);
		}
		break;

	case FLAG_SENDLOG:
		{
			SendLog(operationContext);
		}
		break;
	}

	m_contextPool.Push(operationContext); 
}



void ioTCPConnectNode::ParseReceivePacket()
{
	int packetID = m_packet.GetPacketID();

	switch(packetID)
	{
	case 0:
	default:
		break;

	}
}

void ioTCPConnectNode::OnClose()
{
	ReadyToSocket(GetPort(),1);

	Connect(m_ServerIP,m_svrPort);

	ProcessReserveData();
}

int ioTCPConnectNode::ExceptionSend( CLogData* logData )
{
	ioLogger* logger = reinterpret_cast<ioLogger*>(logData->GetInstance());
	if(logData->GetType() == LOG_MESSAGE_TYPE_SETCATEGORY)
		return -1;
	if(logData->GetCategoryName() == NULL)
		return -1;
	if(logger->GetOpenState() == FALSE)
	{
		if(logData->GetType() == LOG_MESSAGE_TYPE_OPEN) //이부분을 어떻게 해야할지 고민 
		{
			logger->SetOpenState(TRUE); 
			return 0;
		}
		else
			return -1;
	}
	return 0;
}

void ioTCPConnectNode::SendLog( LOGBufferedContext* operationContext )
{
	CLogData* logData = operationContext->m_logData;
	SendMessage(logData);
}

void ioTCPConnectNode::RequestConnect( LOGBufferedContext* operationContext )
{
	if(GetPort() == 0)
	{
		SetPort(operationContext->m_port);
		SetLogInstance(operationContext->GetLogInstance());
		strcpy_s(m_daemonName,operationContext->m_damoneName);
		SetServerIP(operationContext->m_ipAddress,operationContext->m_svrPort);
		
		ReadyToSocket(GetPort(),1);

		if(Connect(m_ServerIP,m_svrPort) == FALSE)
		{
			PrintTimeAndLog("RequestConnect Error");
		}
	}
	else
	{ 
		//	LOG.PrintTimeAndLog("Already Set TCP Mode ");
	}
}

void ioTCPConnectNode::SendOnConnect()
{	
	LOGPacket pk(LTPK_LOG);

	LOGMessageHeader logHeader;
	char daemonName[MAX_DAMONE_NAME_SIZE];

	logHeader.m_logLevel = GetPort();
	strcpy_s( daemonName, m_daemonName );
	logHeader.m_logMessageType = LOG_MESSAGE_TYPE_ONCONNECT;

	pk << logHeader;
	pk << daemonName;

	SendMessage(pk);
}

void ioTCPConnectNode::ProcessReserveData()
{
	for(UINT i=0; i< m_reserveFiles.size(); ++i)
	{
		CLogData* logData = g_logDataPool->Pop();

		if( logData == NULL ) return;

		ioLogger* logger = reinterpret_cast<ioLogger*>(m_reserveFiles[i].m_instance);
		if(logger) 
		{
			logData->Set(m_reserveFiles[i].m_instance, LOG_MESSAGE_TYPE_OPEN, 0, m_reserveFiles[i].m_openFileName);
			if(GetSockState() == DISCONNECTED && GetPrintState() == FALSE)
				logger->ExcuteOpen(logData);
			else
			{
				if( SendMessage(logData) == -1)
				{
					g_logDataPool->Push(logData);
					logData = NULL;
				}
			}

			logData->Init();
			logData->Set(m_reserveFiles[i].m_instance, LOG_MESSAGE_TYPE_PROCESS, 0,LOGRECORD_PRINTTIMEANDLOG, "<<< --------------------  LogServer Disconnect -------------------- >>>\r\n");
			if(GetSockState() == DISCONNECTED && GetPrintState() == FALSE) 
			{
				logger->ExcuteWrite(logData);
			}
		}
		else
			g_logDataPool->Push(logData);
	}
	SetPrintState(TRUE);
}

void ioTCPConnectNode::SetLoggerPacket( CLogData* logData, LOGPacket& kPacket )
{
	ioLogger* pLogger = (ioLogger*)logData->GetInstance(); //호출한 인스턴스 

	switch(logData->GetType())
	{
	case LOG_MESSAGE_TYPE_TERMINATE:
	case LOG_MESSAGE_TYPE_CLOSE:
		{
			int categoryIndex = pLogger->GetCategoryIndex();
			kPacket << categoryIndex;
		}
		break;

	case LOG_MESSAGE_TYPE_OPEN:
		{
			int categoryIndex = 0; 
			char categoryName[MAX_CATEGORY_NAME_SIZE];
			SYSTEMTIME st;

			categoryIndex = pLogger->GetCategoryIndex();
			strcpy_s(categoryName,pLogger->GetCategoryName());
			memcpy(&st,logData->GetSt(),sizeof(st));

			kPacket << categoryIndex;
			kPacket << categoryName;
			kPacket << st;
		}
		break;

	case LOG_MESSAGE_TYPE_PROCESS:
		{ 
			m_logMessage.Init();

			strcpy_s(m_logMessage.m_logMessage,logData->GetBuffer());
			strcpy_s(m_logMessage.m_fileLine,logData->GetFileLine());
			m_logMessage.m_categoryIndex	= pLogger->GetCategoryIndex();
			memcpy(&m_logMessage.m_st,logData->GetSt(),sizeof(m_logMessage.m_st));

			kPacket << m_logMessage;
		}
		break;
	}
}

void ioTCPConnectNode::PrintTimeAndLog( LPSTR fmt,... )
{
	if(m_logger)
	{
		CLogData* logData = g_logDataPool->Pop();

		char text[MAX_BUFFER_SIZE] = { 0, };

		va_list args;
		va_start( args, fmt );
		vsprintf_s( text, sizeof(text), fmt, args );
		va_end( args );

		logData->Set(m_logger, LOG_MESSAGE_TYPE_PROCESS, 0,LOGRECORD_PRINTTIMEANDLOG, text);
		m_logger->ExcuteWrite(logData);

		g_logDataPool->Push(logData);
	}
}

void ioTCPConnectNode::PushQueue(LOGBufferedContext* operationContext,int flag)
{
	operationContext->m_flags = flag;
	g_logQueue->Enqueue(reinterpret_cast<DWORD>(this),(DWORD)sizeof(this),(LPOVERLAPPED)operationContext->GetOveralapped());
}

void ioTCPConnectNode::PushOperation( int flag )
{
	LOGBufferedContext* operationContext = GetOveralpped();
	PushQueue(operationContext,flag);;
}

void ioTCPConnectNode::PushOperation( int flag, char* daemoneName, int port , int svrPort, char* ipAddr,ioLogger* log)
{
	LOGBufferedContext* operationContext = GetOveralpped();

	operationContext->m_port = port;
	operationContext->m_svrPort = svrPort;
	operationContext->m_logInstance = log;
	strcpy_s(operationContext->m_ipAddress,ipAddr);
	strcpy_s(operationContext->m_damoneName,daemoneName);

	PushQueue(operationContext,flag);
}

void ioTCPConnectNode::PushOperation( CLogData* logData )
{
	LOGBufferedContext* operationContext = GetOveralpped();

	operationContext->m_logData = logData;

	PushQueue(operationContext,FLAG_SENDLOG);
}
