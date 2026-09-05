#include "../stdafx.h"
#include "./ThailandOTPnodemanager.h"
#include "../Network\GameServer.h"
#include "../Network/iocpHandler.h"
#include "../Local\ioLocalThailand.h"

extern CLog LOG;


ThailandOTPNodeManager *ThailandOTPNodeManager::sg_Instance = NULL;

ThailandOTPNodeManager::ThailandOTPNodeManager(void)
{
}

ThailandOTPNodeManager::~ThailandOTPNodeManager(void)
{
	m_vOTPNode.clear();
}

ThailandOTPNodeManager & ThailandOTPNodeManager::GetInstance()
{
	if( !sg_Instance )
		sg_Instance = new ThailandOTPNodeManager;

	return *sg_Instance;
}

void ThailandOTPNodeManager::ReleaseInstance()
{
	SAFEDELETE(sg_Instance);
}

void ThailandOTPNodeManager::InitMemoryPool()
{
	ioINILoader kLoader( "ls_config_billingsvr.ini" );
	kLoader.SetTitle( "MemoryPool" );
	int iMaxNode = kLoader.LoadInt( "ThailandOTPPool", 100 );

	kLoader.SetTitle( "Thailand Server Session" );
	int iSendBufferSize = kLoader.LoadInt( "SendBufferSize", MAX_BUFFER );	
	for(int i = 0;i < iMaxNode;i++)
	{
		ThailandOTPNode *pLoginNode = new ThailandOTPNode( INVALID_SOCKET, iSendBufferSize, MAX_BUFFER * 2 );
		if( !pLoginNode )
			continue;
		m_MemNode.Push( pLoginNode );
	}
	LOG.PrintTimeAndLog( 0, "Thailand Server Session SendBuffer : %d | %d", iSendBufferSize , iMaxNode );
}


void ThailandOTPNodeManager::ReleaseMemoryPool()
{
	vThailandOTPNode_iter iter, iEnd;	
	iEnd = m_vOTPNode.end();
	for(iter = m_vOTPNode.begin();iter != iEnd;++iter)
	{
		ThailandOTPNode *pLoginNode = *iter;
		if( !pLoginNode )
			continue;
		pLoginNode->OnDestroy();
		m_MemNode.Push( pLoginNode );
		
	}	
	m_vOTPNode.clear();
	m_MemNode.DestroyPool();
}

void ThailandOTPNodeManager::Process()
{
	for(vThailandOTPNode_iter iter = m_vOTPNode.begin(); iter != m_vOTPNode.end(); ++iter)
	{
		ThailandOTPNode *pLogin = *iter;
		if( !pLogin )
			continue;
		if( pLogin->IsDisconnectState() )
			continue;
		if( TIMEGETTIME() - pLogin->GetConnectTime() < ioLocalThailand::MAX_PACKET_ALIVE_TIME )
			continue;
		pLogin->SendUserMessage();
		pLogin->OnDestroy();
		LOG.PrintTimeAndLog( 0, "%s:%d:%x", __FUNCTION__, __LINE__, pLogin );
	}
}

ThailandOTPNode *ThailandOTPNodeManager::ConnectTo()
{
	ioINILoader kLoader( "ls_config_billingsvr.ini" );
	kLoader.SetTitle( "NETWORK" );

	char szServerIP[MAX_PATH];
	kLoader.LoadString( "ThailandOTPServerIP", "", szServerIP, MAX_PATH );

	int iSSPort = kLoader.LoadInt( "ThailandOTPServerPORT", 9000 );

	SOCKET socket = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( socket == INVALID_SOCKET )
	{
		LOG.PrintTimeAndLog( 0, "ThailandOTPNodeManager::ConnectTo fail socket %d[%s:%d]", GetLastError(), szServerIP, iSSPort );
		return NULL;
	}
	sockaddr_in serv_addr;
	serv_addr.sin_family		= AF_INET;
	serv_addr.sin_addr.s_addr	= inet_addr( szServerIP );
	serv_addr.sin_port			= htons( iSSPort );

	// non-block
	unsigned long arg = 1;
	ioctlsocket( socket, FIONBIO, &arg );

	int retval = ::connect( socket, (sockaddr*)&serv_addr, sizeof(serv_addr) );

	if( retval != 0 ) 
	{
		DWORD dwError = GetLastError();
		if( dwError != WSAEWOULDBLOCK )
		{
			LOG.PrintTimeAndLog( 0, "ThailandOTPNodeManager::ConnectTo fail connect %x:%d[%s:%d]", this, dwError, szServerIP, iSSPort );
			return NULL;
		}
	}

	// block
	arg = 0;
	ioctlsocket( socket, FIONBIO, &arg );

	if( retval != 0 )
	{
		// timeout
		struct timeval tv;
		tv.tv_sec  = 3;
		tv.tv_usec = 0;

		fd_set writedfds;    
		FD_ZERO(&writedfds);
		FD_SET(socket, &writedfds);
		
		retval = select(socket+1, NULL, &writedfds, NULL, &tv);
		
		if(retval == 0)
		{
			LOG.PrintTimeAndLog( 0, "%s Error 1 %d",  __FUNCTION__, GetLastError() );
			return NULL;
		}
		else if(retval == SOCKET_ERROR)
		{
			LOG.PrintTimeAndLog( 0, "%s Error 2 %d",  __FUNCTION__, GetLastError() );
			return NULL;
		}

		if(!FD_ISSET(socket, &writedfds))
		{
			LOG.PrintTimeAndLog( 0, "%s Error 3 %d",  __FUNCTION__, GetLastError() );
			return NULL;
		}
	}

	ThailandOTPNode *pNode = CreateNode( socket );
	return pNode;
}

ThailandOTPNode *ThailandOTPNodeManager::CreateNode( SOCKET s )
{
	ThailandOTPNode *newNode = (ThailandOTPNode*)m_MemNode.Pop();
	if( !newNode )
	{
		LOG.PrintTimeAndLog(0,"[error][goods]ThailandOTPNodeManager::CreateNode MemPool Zero!");
		return NULL;
	}

	g_iocp.AddHandleToIOCP((HANDLE)s,(DWORD)newNode);
	m_vOTPNode.push_back( newNode );

	newNode->SetSocket(s);
	newNode->OnCreate();

	return newNode;
}

void ThailandOTPNodeManager::RemoveNode( ThailandOTPNode *pLoginNode )
{
	if( !pLoginNode ) return;
	if( !pLoginNode->IsDisconnectState() )
	{
		pLoginNode->OnDestroy();
		LOG.PrintTimeAndLog( 0, "%s:%d:%x", __FUNCTION__, __LINE__, pLoginNode );
	}
}

void ThailandOTPNodeManager::Node_Destroy()
{
	if( m_vOTPNode.empty() ) return;

	vThailandOTPNode_iter iter = m_vOTPNode.begin();
	while( iter != m_vOTPNode.end() )
	{
		ThailandOTPNode *pLoginNode = *iter;
		if( pLoginNode->IsDisconnectState() )
		{
			iter = m_vOTPNode.erase( iter );
			m_MemNode.Push( pLoginNode );			
		}
		else
			iter++;
	}
}