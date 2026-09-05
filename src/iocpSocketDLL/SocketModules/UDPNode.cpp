#include "../iocpSocketDLL.h"
#include "UDPNode.h"
#include "ioUDPSecurity.h"


UDPNode::UDPNode(void)
{
	m_contextpoolCount = 0;
}

UDPNode::~UDPNode(void)
{
	ReleaseMemoryPool();
}

void UDPNode::InitMemory( int contextCount, int recvSize , int packetCount)
{
	m_contextPool.CreatePool(contextCount,50000,TRUE);
	m_bufferPool.init(packetCount);

	m_iRecvSize = recvSize;
}

void UDPNode::InitMemory( int contextCount, int recvSize , int seed_64,int seed_128, int seed_256, int seed_1024, int seed_2048 )
{
	m_contextPool.CreatePool(contextCount,50000,TRUE);
	m_bufferPool.init(seed_64, seed_128,seed_256, seed_1024, seed_2048);

	m_iRecvSize = recvSize;
}

BOOL UDPNode::SendMessage(const char* ip,int port, CPacket& rkPacket ) 
{ 
	CPacket sendPacket;
	sendPacket = rkPacket;
	if( m_pNS )
		m_pNS->PrepareMsg( sendPacket );
	BOOL rtval =  SendMessage(ip,port,sendPacket.GetBuffer(),sendPacket.GetBufferSize());
	if( m_pNS )
		m_pNS->CompletionMsg( sendPacket );
	return rtval;
}


BOOL UDPNode::SendMessageByDWORDIP( DWORD dwIp, int port, CPacket& rkPacket )
{
	CPacket sendPacket;
	sendPacket = rkPacket;
	if( m_pNS )
		m_pNS->PrepareMsg( sendPacket );
	/************************************************************************/
	/* S                                                                     */
	/************************************************************************/
	sockaddr_in clientaddr;
	int iAddrLen = sizeof(clientaddr);

	ZeroMemory(&clientaddr,sizeof(sockaddr_in));
	clientaddr.sin_family	= AF_INET;
	clientaddr.sin_addr.s_addr = (htonl((long)dwIp));
	clientaddr.sin_port = htons(port);

	BOOL rtval = SendMessageByAddr(clientaddr,sendPacket.GetBuffer(),sendPacket.GetBufferSize());
	//////////////////////////////////////////////////////////////////////////
	if( m_pNS )
		m_pNS->CompletionMsg( sendPacket );
	return rtval;
}

BOOL UDPNode::SendCurrentPortMessage(const char* ip,int port, int index, CPacket& rkPacket)
{
	CPacket sendPacket;
	sendPacket = rkPacket;
	if( m_pNS )
		m_pNS->PrepareMsg( sendPacket );
	BOOL rtval =  SendMessage(ip,port,sendPacket.GetBuffer(),sendPacket.GetBufferSize(),index);
	if( m_pNS )
		m_pNS->CompletionMsg( sendPacket );
	return rtval;
}
BOOL UDPNode::SendMessage(const char* ip,int port, const char* buffer, const int size, int index)
{
	sockaddr_in clientaddr;
	int iAddrLen = sizeof(clientaddr);

	SetIptoSockAddr(clientaddr, ip, port);

	IOBufferedContext* ov = NULL;
	ov = ReadyToSend(size, buffer);

	if(ov == NULL)
	{
		return FALSE;
	}

	SOCKET sendSocket;
	sendSocket = GetSendSocket(index);


	DWORD dwFlags = 0, dwCount = 1;

	int retval = WSASendTo(sendSocket,
		&ov->m_stContext.m_wsaBuffer,
		dwCount,
		&ov->m_dwBytesTransfer,
		dwFlags,
		reinterpret_cast<sockaddr*>(&clientaddr),
		iAddrLen,
		reinterpret_cast<WSAOVERLAPPED*>(&ov->m_stContext.m_wsaOverlapped),
		NULL);

	if ((retval == SOCKET_ERROR) && (WSA_IO_PENDING != (WSAGetLastError())))
	{
		m_contextPool.Push(ov);
		m_bufferPool.Push(ov->m_stContext.m_wsaBuffer.buf,ov->m_stContext.m_wsaBuffer.len);
		return FALSE;
	}

	return TRUE;
}

BOOL UDPNode::SendMessageByAddr( sockaddr_in& clientaddr,const char* buffer, const int size, int index /*= -1*/ )
{
	IOBufferedContext* ov = NULL;
	ov = ReadyToSend(size, buffer);
	int iAddrLen = sizeof(clientaddr);
	if(ov == NULL)
	{
		return FALSE;
	}

	SOCKET sendSocket;
	sendSocket = GetSendSocket(index);


	DWORD dwFlags = 0, dwCount = 1;

	int retval = WSASendTo(sendSocket,
		&ov->m_stContext.m_wsaBuffer,
		dwCount,
		&ov->m_dwBytesTransfer,
		dwFlags,
		reinterpret_cast<sockaddr*>(&clientaddr),
		iAddrLen,
		reinterpret_cast<WSAOVERLAPPED*>(&ov->m_stContext.m_wsaOverlapped),
		NULL);

	if ((retval == SOCKET_ERROR) && (WSA_IO_PENDING != (WSAGetLastError())))
	{
		m_contextPool.Push(ov);
		m_bufferPool.Push(ov->m_stContext.m_wsaBuffer.buf,ov->m_stContext.m_wsaBuffer.len);
		return FALSE;
	}

	return TRUE;
}

void UDPNode::CloseConnection(int index)
{
	if(index >= (int) m_recvInfos.size())
		return;

	if(INVALID_SOCKET != m_recvInfos[index]->fd)
	{
		::closesocket(m_recvInfos[index]->fd);
	}
}

void UDPNode::ExceptionClose( int lastError, int index )
{
	if(IsActive())
	{
		PrintTimeAndLog(0,"UDPNode ExceptionClose index:%d Err:%d\n",index,lastError);
		SessionClose(index);
	}
}

const std::vector<int>* UDPNode::GetUDP_port()
{
	return &(m_iPorts);
}

const char* UDPNode::GetPublicIP()
{
	return const_cast<char*>(m_strIP.c_str());
}

BOOL UDPNode::Dispatch( DWORD bytesTransferred, OVERLAPPED *ov, CPacket& packet )
{
	if(ov == NULL)
	{
		PrintTimeAndLog(0,"Error UDP Node DIspatch Error");
		return FALSE;
	}
	IOBufferedContext* lpIOBufferedContext = reinterpret_cast<IOBufferedContext*>(ov);
	int index = FindIndextoOv(ov);
	if( lpIOBufferedContext->m_flags == ASYNCFLAG_RECEIVE )
	{
		DispatchReceive( packet, bytesTransferred ,index );
		return TRUE;
	}
	else if( lpIOBufferedContext->m_flags == ASYNCFLAG_SEND )
	{
		BufferFlush(lpIOBufferedContext);		 
	}
	return TRUE;
}

void UDPNode::DispatchReceive( CPacket& packet, DWORD bytesTransferred, int i )
{
	m_recvInfos[i]->recvio.AddBytesTransferred( bytesTransferred );
	int ncount =0;
	if( m_recvInfos[i]->recvio.GetBytesTransferred() > 0 )
	{
		packet.SetBufferCopy(m_recvInfos[i]->recvio.GetBuffer(), m_recvInfos[i]->recvio.GetBytesTransferred());
		if( (packet.IsValidPacket() == true) && 
			(m_recvInfos[i]->recvio.GetBytesTransferred() >= static_cast<DWORD>(packet.GetBufferSize())))
		{
			int packetSize = packet.GetBufferSize();
			ioUDPSecurity* pns = reinterpret_cast<ioUDPSecurity*>(m_recvInfos[i]->m_pns);

			pns->RcvPeerInfo(&m_recvInfos[i]->addr);

			if( bytesTransferred >= 1024 )
			{
				PrintTimeAndLog(0,"[UDPNode::DispatchReceive] Big Size UDP Packet :0x%x::size:%d",packet.GetPacketID(),packetSize); 
			}

			if( !CheckNS( packet , i) ) 
			{
				PrintTimeAndLog(0,"[UDPNode] checkns fail :%d::size:%d",packet.GetPacketID(),packetSize); 
			}
			else //인증 성공시에만 리시브 패킷함  
				ReceivePacket( packet ,i);
			m_recvInfos[i]->recvio.AfterReceive( packetSize );
		}	 
		else
		{
			//PrintTimeAndLog(0,"UDPDispatchError[size:%d]", bytesTransferred);//AE±aE­
			m_recvInfos[i]->recvio.InitRecvIO(); 
		}
	}
	WaitForPacketReceive(i);
}

void UDPNode::SetNS( NetworkSecurity *pNS ,int index)
{
	if(index > (int)m_recvInfos.size())
		return;

	m_recvInfos[index]->m_pns = pNS;
}

void UDPNode::SetNS( NetworkSecurity *pNS )
{
	m_pNS = pNS;
}

BOOL UDPNode::CheckNS( CPacket &rkPacket, int index)
{
	if(index > (int)m_recvInfos.size())
		return FALSE;

	NetworkSecurity* ns = m_recvInfos[index]->m_pns;
	if( !ns ) 
		return TRUE;
	if( !ns->IsCheckSum( rkPacket ) )
		return FALSE; 
	if( !ns->CheckState( rkPacket ) )
		return FALSE;
	return TRUE;
}

BOOL UDPNode::WaitForPacketReceive(int index)
{
	if((int)m_recvInfos.size() <= index)
		return FALSE;

	CReceiveIO& recvIO = m_recvInfos[index]->recvio;
	if( m_recvInfos[index]->fd == INVALID_SOCKET ) return FALSE;

	if( !recvIO.Receivable() )
	{
		CPacket packet;
		packet.SetBufferCopy( recvIO.GetBuffer(), recvIO.GetBytesTransferred() );
		PrintTimeAndLog( 0, "Session WaitForPacketReceive Buffer Overflow[%lu,%d]!!!", packet.GetPacketID(), packet.GetBufferSize() );
		recvIO.InitRecvIO();
	}

	DWORD dwFlags = 0;
	DWORD dwBytes = 0;	

	recvIO.ReadyToReceive();
	int iAddrLen = sizeof(m_recvInfos[index]->addr);

	int iLoopCount = 0;
	while(1)
	{
		int nResult = ::WSARecvFrom( 
			m_recvInfos[index]->fd,
			recvIO.GetWsaBuf(), 
			1, 
			&dwBytes, 
			&dwFlags,
			reinterpret_cast<sockaddr*>(&m_recvInfos[index]->addr),
			&iAddrLen,
			recvIO.GetOveralapped(), 
			NULL );


		if(nResult != SOCKET_ERROR) 
			break;

		DWORD dwError = ::WSAGetLastError();
		if( dwError == WSA_IO_PENDING) 
			break;

		PrintTimeAndLog( 0, "UDP Session WaitForPacketReceive FAILED:%lu, Repeat : %d", dwError, ++iLoopCount );
		Sleep(1);
	}
	return TRUE;
}

void UDPNode::SetAddr(std::string ip, std::vector<int>& ports)
{
	m_strIP = ip;
	m_iPorts.resize(ports.size());
	std::copy(ports.begin(), ports.end(), m_iPorts.begin());
}

void UDPNode::SetSockInfo(std::vector<SOCKINFOPAIR>& socketPairs, SOCKET& sendSocket)
{   
	for(int i=0; i<(int)socketPairs.size();++i)
	{
		UDPIoInfo* stdata = new UDPIoInfo;
		stdata->fd = socketPairs[i].first;
		memcpy(&stdata->addr ,&socketPairs[i].second,sizeof(stdata->addr));
		stdata->recvio.Init(m_iRecvSize);
		stdata->recvio.InitRecvIO();

		m_recvInfos.push_back(stdata);

		WaitForPacketReceive(i);
		SetNetworkSecurity(i);
	}
}

int UDPNode::FindIndextoOv( WSAOVERLAPPED* ov )
{
	for(int i=0; i<(int)m_recvInfos.size();++i)
	{
		if(m_recvInfos[i]->recvio.GetOveralapped() == ov)
			return i;
	}
	return -1;
}

void UDPNode::BufferFlush( IOBufferedContext* lpIOBufferedContext )
{
	m_bufferPool.Push(lpIOBufferedContext->m_stContext.m_wsaBuffer.buf,lpIOBufferedContext->m_stContext.m_wsaBuffer.len);
	m_contextPool.Push(lpIOBufferedContext);
}

void UDPNode::ReleaseMemoryPool()
{
	m_contextPool.DestroyPool();
	m_bufferPool.DestroyBufferPool();
	for(unsigned int i=0; i<m_recvInfos.size();++i)
	{
		delete m_recvInfos[i];
	}
	m_recvInfos.clear();
}

void UDPNode::SetIptoSockAddr( sockaddr_in &clientaddr, const char* ip, int port )
{
	ZeroMemory(&clientaddr,sizeof(sockaddr_in));
	clientaddr.sin_family	= AF_INET;
	inet_pton(AF_INET, ip, &clientaddr.sin_addr);
	clientaddr.sin_port = htons(port);
}

IOBufferedContext* UDPNode::ReadyToSend(  const int size, const char* buffer )
{
	IOBufferedContext* ov = m_contextPool.Pop();
	if(ov == NULL)
	{
		IncremnetContextpoolCount();
		PrintTimeAndLog(0, "UDPNode ov Contextpool NULL\n");
		return NULL;
	}

	ov->m_stContext.m_wsaBuffer.buf = m_bufferPool.Get(size);

	if(ov->m_stContext.m_wsaBuffer.buf == NULL)
	{
		if(ov)
			m_contextPool.Push(ov);

		return NULL;
	}
	memcpy(ov->m_stContext.m_wsaBuffer.buf,buffer,size);

	ov->m_stContext.m_wsaBuffer.len = size;
	ov->m_flags						= ASYNCFLAG_SEND;
	ov->m_dwBytesTransfer			= 0;	

	memset(&ov->m_stContext.m_wsaOverlapped,0,sizeof(ov->m_stContext.m_wsaOverlapped));
	return ov;
}

SOCKET UDPNode::GetSendSocket( int index )
{
	if(index == -1 )
		return m_recvInfos[0]->fd;
	else
		return m_recvInfos[index]->fd;
}
