// DBClient.cpp: implementation of the DBClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DBClient.h"

//#include "../Window.h"
#include "../MainProcess.h"
#include "../Shutdown.h"
#include "../QueryData/QueryResultData.h"
#include "../Network/GameServer.h"
#include "../Network/iocpHandler.h"
#include "../Network/ioPacketQueue.h"
#include "../NodeInfo/ioCharacter.h"
#include "../NodeInfo/ioAward.h"
#include "../NodeInfo/ioClassExpert.h"
#include "../NodeInfo/ioUserGrowthLevel.h"
#include "../NodeInfo/ioItemInfoManager.h"
#include "../NodeInfo/Room.h"
#include "../NodeInfo/BattleRoomNode.h"
#include "../NodeInfo/ioEtcItem.h"
#include "../NodeInfo/ServerNodeManager.h"
#include "../MainServerNode/MainServerNode.h"
#include "../NodeInfo/ioItemInitializeControl.h"
#include "../NodeInfo/GuildRewardManager.h"
#include "../util/cSerialize.h"
#include "LogDBClient.h"
#include "../EtcHelpFunc.h"
#include "../Local/ioLocalParent.h"
#include <strsafe.h>

DBAgentNode::DBAgentNode( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize ) : CConnectNode( s, dwSendBufSize, dwRecvBufSize )
{
	Init();
}

DBAgentNode::~DBAgentNode()
{
	Destroy();
}

void DBAgentNode::Init()
{
	m_iDBAgentIndex	= 0;
	m_iConnectPort	= 0;
	m_szConnectIP.Clear();
}

void DBAgentNode::Destroy()
{

}

void DBAgentNode::OnCreate()
{
	CConnectNode::OnCreate();
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][gameDB]Node create : [%s] [%d]", m_szConnectIP.c_str(), m_iConnectPort );
}

void DBAgentNode::OnDestroy()
{
	CConnectNode::OnDestroy();
	g_CriticalError.CheckDBAgentServerDisconnect( m_szConnectIP.c_str(), m_iConnectPort );
}

bool DBAgentNode::CheckNS( CPacket &rkPacket )
{
	return true;             //네트웍 감시 필요없다.
}

int DBAgentNode::GetConnectType()
{
	return CONNECT_TYPE_GAMEDB_SERVER;
}

void DBAgentNode::SessionClose(BOOL safely)
{
	g_CriticalError.CheckDBAgentServerExceptionDisconnect( GetConnectIP(), GetConnectPort(), GetLastError() );

	if( !g_App.IsTestZone() )
	{
		CPacket packet( DTPK_CLOSE );
		ReceivePacket( packet );
	}
}

void DBAgentNode::ReceivePacket( CPacket &packet )
{
	switch( packet.GetPacketID() )
	{
	case DTPK_QUERY:
		g_RecvQueue.InsertQueue( 0, packet, PK_QUEUE_QUERY ); //유저가 보내지 않았으므로 key_value가 0이다
		break;

	case DTPK_CLOSE:
		g_RecvQueue.InsertQueue( (DWORD)this, packet, PK_QUEUE_SESSION ); // 디비에이전트 노드가 삭제됨
		break;

	default:
		break;
	}
}

void DBAgentNode::PacketParsing( CPacket &packet )
{
	SP2Packet &kPacket = (SP2Packet&)packet;

	switch( packet.GetPacketID() )
	{
	case DTPK_CLOSE:
		OnClose( kPacket );
		return;

	default :
		break;
	}

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"ERROR : UserNodeManager에서 처리할 패킷이 DBClient로 왔다 : x%x", packet.GetPacketID() );
}

void DBAgentNode::OnClose(SP2Packet &packet)
{
	g_DBClient.OnCloseDBAgent( GetIndex() );

	OnDestroy();
}

void DBAgentNode::SetConnectInfo( const int iIndex, const ioHashString &szIP, int iPort )
{
	m_iDBAgentIndex	= iIndex;
	m_szConnectIP	= szIP;
	m_iConnectPort	= iPort;
}

void DBAgentNode::Reconnect()
{
	SOCKET socket = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( socket == INVALID_SOCKET )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DBClient::Reconnect failed %d[%s:%d]", GetLastError(), m_szConnectIP, m_iConnectPort );
		return;
	}

	sockaddr_in serv_addr;
	serv_addr.sin_family		= AF_INET;
	serv_addr.sin_addr.s_addr	= inet_addr( m_szConnectIP.c_str() );
	serv_addr.sin_port			= htons( m_iConnectPort );
	if( ::connect( socket, (sockaddr*)&serv_addr, sizeof(serv_addr) ) != 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DBClient::Reconnect failed %d[%s:%d]", GetLastError(), m_szConnectIP, m_iConnectPort );
		return;
	}

	g_iocp.AddHandleToIOCP( (HANDLE)socket, (DWORD)this );
	OnCreate();
	AfterCreate();

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DBClient::Reconnect success %d[%s:%d]", GetIndex(), m_szConnectIP, m_iConnectPort );
}

//////////////////////////////////////////////////////////////////////////
DBClient *DBClient::sg_Instance = NULL;
DBClient::DBClient()
{
	m_dwCurrentTime   = 0;
}

DBClient::~DBClient()
{
	DestroyDBAgentMap();
}

DBClient &DBClient::GetInstance()
{
	if(sg_Instance == NULL)
	{
		sg_Instance = new DBClient;
	}
	return *sg_Instance;
}

void DBClient::ReleaseInstance()
{		
	SAFEDELETE(sg_Instance);
}

void DBClient::DestroyDBAgentMap()
{
	DBAgentNodeMap::iterator iCreate;
	for( iCreate = m_DBAgentMap.begin() ; iCreate != m_DBAgentMap.end() ; ++iCreate )
	{
		DBAgentNode *pNode = iCreate->second;
		if( pNode )
		{
			pNode->OnDestroy();
			SAFEDELETE( pNode );
		}
	}
	m_DBAgentMap.clear();
}

DWORD DBClient::GameServerAgentID()
{
	// 2013-01-14 신영욱, 게임서버 32대가 하나의 디비에이전트를 사용, 16개의 스레드가 2개씩 담당
	int iDBAgentIndex = (g_ServerNodeManager.GetServerIndex() - 1) / 32;
	iDBAgentIndex =  (iDBAgentIndex >= GetTotalNodeSize()) ? 0 : iDBAgentIndex;

	//Debug("@DBAgent Index : %d\r\n", iDBAgentIndex);
	return iDBAgentIndex;
}

DWORD DBClient::GameServerThreadID()
{
	//return rand() % 10000;

	// 2013-01-14 신영욱, 고른 분포를 위해 prime number로 변경
	return rand() % 997;
}

bool DBClient::ConnectTo()
{
	if(m_DBAgentInfos.empty())
		return false;

	DestroyDBAgentMap();

	ioINILoader kLoader( "ls_config_game.ini" );
	kLoader.SetTitle( "DBAgent Session" );
	int iSendBufferSize = kLoader.LoadInt( "SendBufferSize", MAX_BUFFER );
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][gamdDB]Session sendBuffer : [%d]", iSendBufferSize );

	int index = 0;
	for(auto i = m_DBAgentInfos.begin(); i != m_DBAgentInfos.end(); i++)
	{
		//
		SOCKET socket = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
		if( socket == INVALID_SOCKET )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DBClient::ConnectTo socket fail %d[%s:%d]", GetLastError(), (*i).first.c_str(),(*i).second );
			return false;
		}

		sockaddr_in serv_addr;
		serv_addr.sin_family		= AF_INET;
		serv_addr.sin_addr.s_addr	= inet_addr((*i).first.c_str() );
		serv_addr.sin_port			= htons( (*i).second );
		if( ::connect( socket, (sockaddr*)&serv_addr, sizeof(serv_addr) ) != 0 )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DBClient::ConnectTo connect fail %d[%s:%d]", GetLastError(), (*i).first.c_str(), (*i).second );
			return false;
		}
		//
		DBAgentNode *pAgentNode = new DBAgentNode( socket, iSendBufferSize, MAX_BUFFER * 3 );
		pAgentNode->SetConnectInfo( index, (*i).first.c_str(), (*i).second );
		g_iocp.AddHandleToIOCP( (HANDLE)socket, (DWORD)pAgentNode );
		pAgentNode->OnCreate();
		pAgentNode->AfterCreate();
		m_DBAgentMap.insert( DBAgentNodeMap::value_type( index, pAgentNode ) );
		index++;
	}

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][gameDB]Agent connection count : [%d]", (int)m_DBAgentMap.size() );
	
	m_dwCurrentTime = TIMEGETTIME();
	return true;
}

ValueType DBClient::GetValueType( VariableType nType, int len )
{
	ValueType vt;
	vt.type = nType;
	vt.size = len;
	return vt;
}

void DBClient::OnPing()
{
	const int queryId = 1; // 실제로는 존재하지 않는 쿼리

	DWORD dwCurrent = TIMEGETTIME();

	CQueryData query_data;
	query_data.SetReturnData( &dwCurrent, sizeof(DWORD) );

	cSerialize v_FT;
	vVALUETYPE v_VT;

	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(), 
		_RESULT_CHECK, 
		DBAGENT_GAME_PINGPONG, 
		_UPDATEDB,
		queryId, 
		v_FT, 
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;

	DBAgentNodeMap::iterator iter;
	for(iter = m_DBAgentMap.begin();iter != m_DBAgentMap.end() ; ++iter )
	{
		DBAgentNode *pNode = iter->second;
		if( !pNode->IsActive() ) continue;
		if( !pNode->SendMessage( kPacket, TRUE ) )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnPing Send Fail!(%d) :%d", (DWORD)iter->first, GetLastError() );
		}
	}
}

void DBClient::OnClassPriceInfo()
{
	static int iCurMinute = 0;
	iCurMinute++;

	if( iCurMinute > 60 )
	{
		iCurMinute = 0;

		if( !g_MainServer.IsActive() )
			OnSelectItemBuyCnt();
	}
}

void DBClient::ProcessTime()
{
	if( TIMEGETTIME() - m_dwCurrentTime < 60000 ) 
		return; // 1분 확인

	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	OnTestZoneReconnect();
	OnClassPriceInfo();

	m_dwCurrentTime = TIMEGETTIME();
}

void DBClient::ProcessPing()
{
	OnPing();
}

bool DBClient::SendMessage( DWORD dwAgentServerID, SP2Packet &rkPacket )
{
	g_ProcessChecker.DBServerSendMessage( rkPacket.GetPacketID(), rkPacket.GetBufferSize() );

	DBAgentNodeMap::iterator iter = m_DBAgentMap.find( dwAgentServerID );
	if( iter != m_DBAgentMap.end() )
	{
		DBAgentNode *pNode = iter->second;
		if( pNode && pNode->IsActive() )
		{
			return pNode->SendMessage( rkPacket, TRUE );
		}
	}

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DBClient::SendMessage : Agent Send Failed(%d) Random Send", dwAgentServerID );
	for(iter = m_DBAgentMap.begin();iter != m_DBAgentMap.end() ; ++iter )
	{
		DBAgentNode *pNode = iter->second;
		if( pNode && pNode->IsActive() )
		{
			return pNode->SendMessage( rkPacket, TRUE );
		}
	}
	return false;
}

void DBClient::OnTestZoneReconnect()
{
	if( !g_App.IsTestZone() )
		return;

	bool bReconnect = false;
	DBAgentNodeMap::iterator iter;
	for(iter = m_DBAgentMap.begin();iter != m_DBAgentMap.end() ; ++iter )
	{
		DBAgentNode *pNode = iter->second;
		if( !pNode->IsActive() )
		{
			bReconnect = true;
			break;
		}
	}

	if( bReconnect )
		ConnectTo();
}

void DBClient::OnReconnect()
{
	for(DBAgentNodeMap::iterator it = m_DBAgentMap.begin(); it != m_DBAgentMap.end() ; ++it )
	{
		DBAgentNode *pNode = it->second;
		if( !pNode->IsActive() )
		{
			pNode->Reconnect();
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// SEND
//////////////////////////////////////////////////////////////////////////
void DBClient::OnCloseDBAgent( const int iIndex )
{
	Debug( "# OnClose DBAgent, Index(%d)\r\n", iIndex );
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "# OnClose DBAgent, Index(%d)", iIndex );

	g_App.Shutdown(SHUTDOWN_DBAGENT, 3);

	/*
	// 이 서버가 디비에이전트가 죽었을 경우 종료한다
	if(GameServerAgentID() == iIndex )
	{
		g_App.Shutdown(SHUTDOWN_DBAGENT);
	}
	else // 해당 디비에이전트를 사용하는 유저만 종료시킨다
	{
		g_UserNodeManager.AllUserDisconnect( iIndex );
	}
	*/
}

void DBClient::OnUpdateUserCount( const int64 iServerIndex, int iUserCount )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//wsprintf(str_query,"exec game_server_connection_count_save %d, %d", dwServerIndex, iUserCount );

	//const int queryId = 1;

	//cSerialize v_FT;
	//vVALUETYPE v_VT;

	//v_FT.Write( iServerIndex );
	//v_FT.Write( iUserCount );

	//CQueryData query_data;	
	//query_data.SetData( 
	//	//g_App.GetGameServerName().GetHashCode(), 
	//	g_ServerNodeManager.GetServerIndex(),
	//	_RESULT_CHECK, 
	//	DBAGENT_USER_COUNT_UPD, 
	//	_UPDATEDB,
	//	queryId,
	//	v_FT,
	//	v_VT);

	//SP2Packet kPacket( DTPK_QUERY );
	//kPacket << query_data;
	//if( !SendMessage( GameServerAgentID(), kPacket ) )
	//{
	//	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdateUserCount Send Fail! :%d - %d", GetLastError(), queryId );
	//	return;
	//}
}

void DBClient::OnUpdateUserMoveServerID( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const int64 serverId )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s(str_query,"exec game_login_serverid_save %d, %s", dwUserIndex, szServerID.c_str() );

	const int queryId = 2;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( serverId );

	CQueryData query_data;	
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_USER_MOVE_SERVER_ID_UPD, 
		_UPDATEDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdateUserMoveServerID Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateServerOn( const int64 iServerIndex )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s(str_query,"exec game_server_connection_on %d", iServerIndex );

	const int queryId = 3;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( iServerIndex );

	CQueryData query_data;	
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_CHECK, 
		DBAGENT_SERVER_ON_UPD, 
		_UPDATEDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( GameServerAgentID(), kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdateServerOn Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnDeleteGameServerInfo( const __int64 gameServerId )
{
	if(0 == gameServerId)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Error - OnDeleteGameServerInfo - GameServerID is empty");
		return;
	}
	//char str_query[MAX_QUERY_SIZE] = "";
	//wsprintf(str_query,"exec game_server_delete %s", szGameServerID.c_str());

	const int queryId = 4;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( gameServerId );

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_CHECK,
		DBAGENT_SERVER_INFO_DEL,
		_DELETEDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( GameServerAgentID(), kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnDeleteGameServerInfo Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectSecondEncryptKey()
{
	//char str_query[MAX_QUERY_SIZE] = "exec game_encode_get_fix_key";

	const int queryId = 5;

	cSerialize v_FT;
	vVALUETYPE v_VT;
	v_VT.push_back(GetValueType(vChar,LOGIN_KEY_PLUS_ONE));          //2번째 암호키

	CQueryData query_data;	
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_CHECK,
		DBAGENT_SERVER_SECOND_KEY_GET,
		_SELECTDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( GameServerAgentID(), kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectSecondEncryptKey Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectItemBuyCnt()
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_hero_price_get_data" );

	const int queryId = 6;
	// MainServer iQueryID = 2001; 과 같음

	cSerialize v_FT;
	vVALUETYPE v_VT;
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));         //용병 번호
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));         //가격
	//for(int i = 0;i < ioItemPriceManager::DB_LOAD_COUNT;i++)
	//{
	//	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));         //가격
	//}

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_CHECK, 
		DBAGENT_ITEM_BUYCNT_SET, 
		_SELECTDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	kPacket << query_data;
	if( !SendMessage( GameServerAgentID(), kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectItemBuyCnt Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectUserLoginInfo( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szPrivateID )
{
	if( szPrivateID.IsEmpty() )	
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Error - OnSelectUserLoginInfo - szID is empty");
		return;
	}

	if( !g_App.IsRightID( szPrivateID.c_str() ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Error - OnSelectUserLoginInfo - szID is Error");
		return;
	}

	//char str_query[MAX_QUERY_SIZE] = "";
	//wsprintf(str_query,"exec game_login_get_data '%s'", szPrivateID.c_str());

	const int queryId = 7;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( szPrivateID.c_str(), szPrivateID.Length(), TRUE );

	v_VT.push_back(GetValueType(vChar,ID_NUM_PLUS_ONE));          //유저 아이디
	v_VT.push_back(GetValueType(vChar,LOGIN_KEY_PLUS_ONE));       //로그인 키
	v_VT.push_back(GetValueType(vINT64,sizeof(__int64)));         //게임서버ID
	v_VT.push_back(GetValueType(vTimeStamp,sizeof(DBTIMESTAMP))); //로그인 키 생성 날짜,시간.
#ifdef __OHTG_LOGIN_IP_CHECK__
	v_VT.push_back(GetValueType(vChar,LOGIN_KEY_PLUS_ONE));          //IP
#endif //__OHTG_LOGIN_IP_CHECK__

	CQueryData query_data;	
	query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	query_data.SetReturnData( szPrivateID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetData( 
		dwThreadID,
		_RESULT_CHECK,
		DBAGENT_USER_LOGIN_INFO_GET,
		_SELECTDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectUserLoginInfo Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectUpdateUserLoginInfo( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szPrivateID , const int64 serverId )
{
	if( szPrivateID.IsEmpty() )	
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Error - OnSelectUpdateUserLoginInfo - szID is empty");
		return;
	}
	if( 0 == serverId ) 
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Error - OnSelectUpdateUserLoginInfo - GameServerID is empty");
		return;
	}

	//char str_query[MAX_QUERY_SIZE] = "";
	//wsprintf(str_query,"exec game_login_save '%s', %s", szPrivateID.c_str() , szGameServerID.c_str());

	const int queryId = 8;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( szPrivateID.c_str(), szPrivateID.Length(), TRUE );
	v_FT.Write( serverId );
	v_VT.push_back(GetValueType(vChar,ID_NUM_PLUS_ONE));          //유저 아이디
	v_VT.push_back(GetValueType(vINT64,sizeof(__int64)));         //게임서버ID

	CQueryData query_data;	
	query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	query_data.SetReturnData( szPrivateID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetData( 
		dwThreadID,
		_RESULT_CHECK,
		DBAGENT_USER_LOGIN_INFO_GET_UPD,
		_SELECTDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectUpdateUserLoginInfo Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateUserLogout( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szPrivateID )
{	
	if( szPrivateID.IsEmpty() )	
	{
		//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Error - OnUpdateUserLogout - UserID is Zero");
		return;
	}

	char szGUID[LOGIN_KEY_PLUS_ONE]="";
	Help::GetGUID( szGUID, sizeof( szGUID ) );

	//char str_query[MAX_QUERY_SIZE] = "";
	//wsprintf( str_query,"exec game_login_init_user '%s', '%s'", szPrivateID.c_str(), szGUID );

	const int queryId = 9;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( szPrivateID.c_str(), szPrivateID.Length(), TRUE );
	v_FT.Write( szGUID, LOGIN_KEY_PLUS_ONE, TRUE );

	CQueryData query_data;	
	query_data.SetData( 
		dwThreadID,
		_RESULT_CHECK,
		DBAGENT_USER_LOGOUT_UPD,
		_UPDATEDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdateUserLogout Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateAllUserLogout( const __int64 gameServerId )
{
	if(0 == gameServerId)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Error - OnUpdateAllUserLogout - GameServerID is empty");
		return;
	}

	//char str_query[MAX_QUERY_SIZE] = "";
	//wsprintf(str_query,"exec game_login_init_server %s", szGameServerID.c_str());

	const int queryId = 10;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( gameServerId );

	CQueryData query_data;	
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_CHECK,
		DBAGENT_USER_ALL_LOGOUT_UPD,
		_UPDATEDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( GameServerAgentID(), kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdateAllUserLogout Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectUserData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szPrivateID )
{
	if( szPrivateID.IsEmpty() )	return;

	//char str_query[MAX_QUERY_SIZE] = "";
	//wsprintf(str_query,"exec game_game_get_data '%s'", szPrivateID.c_str() );

	const int queryId = 11;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( szPrivateID.c_str(), szPrivateID.Length(), TRUE );
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));             //유저 고유 인덱스
	v_VT.push_back(GetValueType(vChar,ID_NUM_PLUS_ONE));          //유저 아이디
	v_VT.push_back(GetValueType(vChar,ID_NUM_PLUS_ONE));          //유저 닉네임
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));             //훈련하기
	v_VT.push_back(GetValueType(vINT64,sizeof(__int64)));         //유저 머니
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));             //접속횟수
	v_VT.push_back(GetValueType(vTimeStamp,sizeof(DBTIMESTAMP))); //접속 날짜.
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));             //계급
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));             //계급 구간 경험치
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));			  //유저 이벤트 타입
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));             //유저랭킹
	v_VT.push_back(GetValueType(vSHORT,sizeof(short)));           //가입 타입 ( 임시, 정식, 임시만료) // db smallint
	v_VT.push_back(GetValueType(vTimeStamp,sizeof(DBTIMESTAMP))); //가입 날짜.
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));             //진영 타입
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));             //누적 래더 포인트
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));             //시즌 래더 포인트
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));             //진영 랭킹
	v_VT.push_back(GetValueType(vSHORT,sizeof(short)));           //채널링 타입 // db smallint
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && pLocal->IsChannelingID() )
	{
		v_VT.push_back(GetValueType(vChar,CHANNELING_USER_ID_NUM_PLUS_ONE)); //채널링사의 유저 ID
		v_VT.push_back(GetValueType(vChar,CHANNELING_USER_NO_NUM_PLUS_ONE)); //채널링사의 유저 NO
	}
	v_VT.push_back(GetValueType(vSHORT,sizeof(short)));           //차단타입
	v_VT.push_back(GetValueType(vTimeStamp,sizeof(DBTIMESTAMP))); //차단시간
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));             //낚시레벨
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));             //낚시 구간 경험치
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));             //리필 시간
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));             //유물레벨
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));             //유물 구간 경험치
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));             //영웅전 누적 경험치
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));             //영웅전 경험치
	v_VT.push_back(GetValueType(vSHORT,sizeof(short)));             //수련장

	CQueryData query_data;
	query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	query_data.SetReturnData( szPrivateID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetData( 
		dwThreadID,
		_RESULT_CHECK,
		DBAGENT_USER_DATA_GET,
		_SELECTDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectUserData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateUserData( const DWORD dwAgentID, const DWORD dwThreadID, const USERDATA &user_data, const UserRelativeGradeData &user_relative_grade_data, DWORD dwConnectTime )
{
	//char szLimitDate[MAX_PATH] = "";
	//CTime curTime = CTime::GetCurrentTime();
	//wsprintf( szLimitDate,"\'%d-%d-%d %d:%d:%d\'", curTime.GetYear(), curTime.GetMonth(), curTime.GetDay(), curTime.GetHour(), curTime.GetMinute(), curTime.GetSecond() );

	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s(str_query, "exec game_game_save %d, %d, %I64d, %d, %s, %d, %d, %d, %d, %d, %d, %d, %d, %d", 
	//									user_data.m_user_idx, 
	//									user_data.m_user_state, 
	//									user_data.m_money, 
	//									user_data.m_connect_count, 
	//									szLimitDate,
	//									user_data.m_grade_level,
	//									user_data.m_grade_exp,
	//									TIMEGETTIME() - dwConnectTime,
	//									user_data.m_camp_position,
	//									user_data.m_fishing_level,
	//									user_data.m_fishing_exp,
	//									user_data.m_refill_data,
	//									user_data.m_iExcavationLevel,
	//									user_data.m_iExcavationExp );

	SYSTEMTIME sysTime;
	GetLocalTime( &sysTime );

	const int queryId = 12;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( user_data.m_user_idx );
	v_FT.Write( user_data.m_user_state );
	v_FT.Write( user_data.m_money );
	v_FT.Write( user_data.m_connect_count );
	v_FT.Write( (uint8*)&sysTime, sizeof(SYSTEMTIME), TRUE );
	v_FT.Write( user_data.m_grade_level );
	v_FT.Write( user_data.m_grade_exp );
	v_FT.Write( TIMEGETTIME() - dwConnectTime );
	v_FT.Write( user_data.m_camp_position );
	v_FT.Write( user_data.m_fishing_level );
	v_FT.Write( user_data.m_fishing_exp );
	v_FT.Write( user_data.m_refill_data );
	v_FT.Write( user_data.m_iExcavationLevel );
	v_FT.Write( user_data.m_iExcavationExp );
	v_FT.Write( user_relative_grade_data.m_init_code );

	CQueryData query_data;                                                    
	query_data.SetData( 
		dwThreadID,
		_RESULT_CHECK,
		DBAGENT_USER_DATA_UPD,
		_UPDATEDB,
		queryId,
		v_FT,
		v_VT );
	
	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdateUserData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateUserLadderPoint( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD &dwUserIndex, const int &iAccumulationLadderPoint, const int &iLadderPoint )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s(str_query, "exec game_game_renpoint_save %d, %d, %d", dwUserIndex, iAccumulationLadderPoint, iLadderPoint );

	const int queryId = 13;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( iAccumulationLadderPoint );
	v_FT.Write( iLadderPoint );

	CQueryData query_data;                                                    
	query_data.SetData( 
		dwThreadID,
		_RESULT_CHECK,
		DBAGENT_USER_LADDER_POINT_UPD,
		_UPDATEDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdateUserLadderPoint Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateUserHeroExpert( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD &dwUserIndex, const int &iHeroExpert )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s(str_query, "exec game_game_heroexp_save %d, %d", dwUserIndex, iHeroExpert );

	const int queryId = 14;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( iHeroExpert );

	CQueryData query_data;                                                    
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_HERO_EXPERT_UPD, 
		_UPDATEDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdateUserHeroExpert Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnInitUserHeroExpert( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD &dwUserIndex )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s(str_query, "exec game_game_heroexp_init %d", dwUserIndex );

	const int queryId = 15;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );

	CQueryData query_data;                                                    
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_HERO_EXPERT_INIT, 
		_UPDATEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnInitUserHeroExpert Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnLoginSelectControlKeys( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_config_keyboard_get_data %d", dwUserIdx );

	const int queryId = 16;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );
	v_VT.push_back( GetValueType( vChar, MAX_CONTROL_KEYS_PLUS_ONE ) );

	CQueryData query_data;
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	//query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwUserIdx, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_CONTROL_KEYS_GET, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnLoginSelectControlKeys Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnLoginSelectAllAwardData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx )
{
	if( szID.IsEmpty() )	return;

	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_award_get_list %d", dwUserIdx );

	const int queryId = 17;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//시상테이블 인덱스
	for(int i = 0;i < ioAward::MAX_SLOT;i++)
	{
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//Category
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//Count
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//Point
	}

	CQueryData query_data;
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	//query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwUserIdx, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_AWARD_DATA_GET, 
		_SELECTEX1DB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnLoginSelectAllAwardData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnLoginSelectAwardExpert( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_award_get_level %d", dwUserIdx );

	const int queryId = 18;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//Award Level
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//Award Exp

	CQueryData query_data;
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	//query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwUserIdx, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_AWARD_EXPERT_GET, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnLoginSelectAwardExpert Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnLoginSelectAllClassExpert( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_class_get_list %d", dwUserIdx );

   	const int queryId = 19;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );					//인덱스
	for(int i = 0;i < ioClassExpert::MAX_SLOT;i++)
	{
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//타입
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//레벨
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		    	//경험치
		v_VT.push_back( GetValueType( vChar, sizeof(BYTE) ) );				//용병강화도
	}	

	CQueryData query_data;
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	//query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwUserIdx, sizeof(int) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_CLASS_EXPERT_DATA_GET, 
		_SELECTEX1DB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnLoginSelectAllClassExpert Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnLoginSelectUserRecord( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_record_battle_get_data %d", dwUserIdx );

	const int queryId = 20;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );

	// 전투 전적
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) ); //승
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) ); //패
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) ); //킬
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) ); //데스	

	// 래던 전적
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) ); //승
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) ); //패
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) ); //킬
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) ); //데스

	// 영웅전 전적
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) ); //승
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) ); //패
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) ); //킬
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) ); //데스

	// 영웅전 시즌 전적
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) ); //승
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) ); //패
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) ); //킬
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) ); //데스

	CQueryData query_data;
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	//query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwUserIdx, sizeof(int) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_USER_RECORD_GET, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnLoginSelectUserRecord Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnLoginSelectAllEtcItemData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx )
{
	if( szID.IsEmpty() )	return;

	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_item_etc_get_data_list %d", dwUserIdx );

	const int queryId = 21;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//인덱스
	for(int i = 0;i < ioInventory::MAX_SLOT;i++)
	{
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//ItemType
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//ItemValue1
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//ItemValue2
	}

	CQueryData query_data;
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	//query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwUserIdx, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_ETC_ITEM_DATA_GET, 
		_SELECTEX1DB,	
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnLoginSelectAllEtcItemData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnLoginSelectAllExtraItemData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx )
{
	if( szID.IsEmpty() )	return;

	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_item_extra_get_list %d", dwUserIdx );

	const int queryId = 22;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );

	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//인벤 인덱스

	for(int i = 0;i < ioUserExtraItem::MAX_SLOT;i++)
	{
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//ItemCode
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//Reinforce
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//SlotIndex
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//TradeType
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//PeriodType
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//MaleCustom
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//FemaleCustom
		v_VT.push_back( GetValueType( vSHORT, sizeof(short) ) );			//FailExp
		v_VT.push_back( GetValueType( vTimeStamp, sizeof(DBTIMESTAMP) ) );	//LimiteDate
	}

	CQueryData query_data;
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	//query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwUserIdx, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_EXTRAITEM_DATA_GET, 
		_SELECTEX1DB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnLoginSelectAllExtraItemData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnLoginSelectAllPetData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx )
{
	if( szID.IsEmpty() )	return;

	const int queryId = 177; //수정 필요

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );

	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//펫 인덱스
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//펫 코드
	v_VT.push_back( GetValueType( vChar, sizeof(char) ) );		//펫 렝크
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//펫 레벨
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//펫 경험치
	v_VT.push_back( GetValueType( vChar, sizeof(char) ) );		//장착 여부
#if defined( DIVIDE_PET ) 
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//장착 영웅
#endif

	
	CQueryData query_data;
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	//query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwUserIdx, sizeof(int) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_PET_DATA_GET, 
		_SELECTEX1DB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnLoginSelectAllPetData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnLoginSelectAllQuestCompleteData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx )
{
	if( szID.IsEmpty() )	return;

	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_questcomplete_get_list %d", dwUserIdx );

	const int queryId = 23;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );			
	for(int i = 0;i < ioQuest::MAX_SLOT;i++)
	{
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//IndexData
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//DateData
	}

	CQueryData query_data;
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	//query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwUserIdx, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_QUEST_COMPLETE_DATA_GET, 
		_SELECTEX1DB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnLoginSelectAllQuestCompleteData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnLoginSelectAllQuestData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx )
{
	if( szID.IsEmpty() )	return;

	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_quest_get_list %d", dwUserIdx );

	const int queryId = 24;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );			
	for(int i = 0;i < ioQuest::MAX_SLOT;i++)
	{
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//IndexData
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//ValueData
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );              //MagicData
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//DateData
	}

	CQueryData query_data;
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	//query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwUserIdx, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_QUEST_DATA_GET, 
		_SELECTEX1DB,	
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnLoginSelectAllQuestData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnLoginSelectAllCharData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx )
{
	if( szID.IsEmpty() )	
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][login] user id is empty" );
		return;
	}

	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_character_get_list %d", dwUserIdx );

	const int queryId = 25;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));				//캐릭터 인덱스
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));				//Class Type
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));				//종족
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));				//성별
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));				//수염
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));				//얼굴
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));				//머리
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));				//피부색
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));				//머리색
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));				//장신구
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));				//속옷
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));				//아이템 종류 1
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));				//아이템 종류 2
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));				//아이템 종류 3
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));				//아이템 종류 4
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));               //용병 슬롯 위치

	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));               //용병 남은 기간
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));               //용병 기간 타입
	v_VT.push_back(GetValueType(vSHORT,sizeof(short)));             //용병 대표 타입
	v_VT.push_back(GetValueType(vSHORT,sizeof(short)));             //용병 대여 타입
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));               //용병 대여 시간

	v_VT.push_back(GetValueType(vChar,sizeof(char)));               //용병 각성 타입
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));               //용병 각성 남은 시간

	CQueryData query_data;
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	//query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwUserIdx, sizeof(int) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_CHAR_DATA_ALL_GET, 
		_SELECTEX1DB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnLoginSelectAllCharData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnLoginSelectAllInvenData( const int iPrevDecoIndex, const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx )
{
	if( szID.IsEmpty() )	return;

	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_item_get_data_list %d", dwUserIdx );
	//game_item_get_data_list_dev
	
	const int queryId = 26;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( iPrevDecoIndex );
	v_FT.Write( dwUserIdx );
	v_FT.Write( DB_DECO_SELECT_COUNT );

	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//인벤 인덱스

	for(int i = 0;i < ioInventory::MAX_SLOT;i++)
	{
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//ItemType
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//ItemCode
	}

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIdx, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_INVEN_DATA_GET, 
		_SELECTEX1DB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnLoginSelectAllInvenData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnLoginSelectAllGrowth( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_ability_get_list %d", dwUserIdx );

	const int queryId = 27;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );					//인덱스

	for(int i = 0;i < ioUserGrowthLevel::MAX_SLOT;i++)
	{
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//타입

		for( int j=0; j < MAX_CHAR_GROWTH; ++j )							//캐릭터 성장요소별 레벨
		{
			v_VT.push_back( GetValueType( vChar, sizeof(char) ) );
		}

		for( int k=0; k < MAX_ITEM_GROWTH; ++k )							//아이템 성장요소별 레벨
		{
			v_VT.push_back( GetValueType( vChar, sizeof(char) ) );
		}

		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//TimeGrowthSlot
		v_VT.push_back( GetValueType( vTimeStamp, sizeof(DBTIMESTAMP) ) );	//LimiteDate
	}

	CQueryData query_data;
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	//query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwUserIdx, sizeof(int) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_GROWTH_DATA_GET, 
		_SELECTEX1DB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnLoginSelectAllGrowth Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnLoginSelectAllMedalItemData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx )
{
	if( szID.IsEmpty() )	return;

	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_item_medal_get_list %d", dwUserIdx );

	const int queryId = 28;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//인벤 인덱스
	for(int i = 0;i < ioUserExtraItem::MAX_SLOT;i++)
	{
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//ItemType
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//EquipClass
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//PeriodType
		v_VT.push_back( GetValueType( vTimeStamp, sizeof(DBTIMESTAMP) ) );	//LimiteDate
	}

	CQueryData query_data;
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	//query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwUserIdx, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_MEDALITEM_DATA_GET, 
		_SELECTEX1DB,	
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnLoginSelectAllMedalItemData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnLoginSelectAllExMedalSlotData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx )
{
	if( szID.IsEmpty() )	return;

	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_item_medal_extend_get_list %d", dwUserIdx );

	const int queryId = 29;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//인벤 인덱스
	for( int i=0; i<ioUserExpandMedalSlot::MAX_SLOT; i++ )
	{
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//Class
		v_VT.push_back( GetValueType( vChar, sizeof(char) ) );				//Medal Slot Number
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//PeriodType
	}

	CQueryData query_data;
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	//query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwUserIdx, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_EXPAND_MEDAL_SLOT_DATA_GET, 
		_SELECTEX1DB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnLoginSelectAllExMedalSlotData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnLoginSelectAllEventData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx )
{
	if( szID.IsEmpty() )	return;

	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_event_get_data %d", dwUserIdx );

	const int queryId = 30;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG))); // index
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG))); // event type
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG))); // value 1				
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG))); // value 2				

	CQueryData query_data;
	query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	//query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwUserIdx, sizeof(int) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_EVENT_DATA_ALL_GET, 
		_SELECTEX1DB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnLoginSelectAllEventData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnLoginSelectAllAlchemicData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx )
{
	if( szID.IsEmpty() )	return;

	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_piece_get_list %d", dwUserIdx );

	const int queryId = 168;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	//
	v_FT.Write( dwUserIdx );

	//
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//인덱스

	for( int i=0; i < ioAlchemicInventory::MAX_DB_SLOT; ++i )
	{
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//code
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//count
	}

	CQueryData query_data;
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	//query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwUserIdx, sizeof(DWORD) );
	
	query_data.SetData( dwThreadID, _RESULT_CHECK,
						DBAGENT_ALCHEMIC_DATA_GET, _SELECTEX1DB,
						queryId,
						v_FT,
						v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnLoginSelectAllAlchemicData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnInsertCharData( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, const CHARACTER &kCharInfo )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//wsprintf( str_query,"exec game_character_add %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d", 
	//			       dwUserIndex, kCharInfo.m_class_type, kCharInfo.m_kindred, kCharInfo.m_sex, kCharInfo.m_beard,
	//				   kCharInfo.m_face, kCharInfo.m_hair, kCharInfo.m_skin_color, kCharInfo.m_hair_color, kCharInfo.m_accessories, kCharInfo.m_underwear,
	//				   kCharInfo.m_extra_item[0], kCharInfo.m_extra_item[1], kCharInfo.m_extra_item[2], kCharInfo.m_extra_item[3], kCharInfo.m_iLimitSecond,
	//				   (int)kCharInfo.m_ePeriodType, kCharInfo.m_sLeaderType, kCharInfo.m_sRentalType, kCharInfo.m_dwRentalMinute );

	const int queryId = 31;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( kCharInfo.m_class_type );
	v_FT.Write( kCharInfo.m_kindred );
	v_FT.Write( kCharInfo.m_sex );
	v_FT.Write( kCharInfo.m_beard );
	v_FT.Write( kCharInfo.m_face );
	v_FT.Write( kCharInfo.m_hair );
	v_FT.Write( kCharInfo.m_skin_color );
	v_FT.Write( kCharInfo.m_hair_color );
	v_FT.Write( kCharInfo.m_accessories );
	v_FT.Write( kCharInfo.m_underwear );
	v_FT.Write( kCharInfo.m_extra_item[0] );
	v_FT.Write( kCharInfo.m_extra_item[1] );
	v_FT.Write( kCharInfo.m_extra_item[2] );
	v_FT.Write( kCharInfo.m_extra_item[3] );
	v_FT.Write( kCharInfo.m_iLimitSecond );
	v_FT.Write( (int)kCharInfo.m_ePeriodType );
	v_FT.Write( kCharInfo.m_sLeaderType );
	v_FT.Write( kCharInfo.m_sRentalType );
	v_FT.Write( kCharInfo.m_dwRentalMinute );

    CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_CHAR_DATA_SET, 
		_INSERTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnInsertCharData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectCharIndex( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, int user_index, int iMsgResult, int iSelectCount /*= 1 */, int iLogType /*= -1*/, int iBuyPrice /*= 0*/ )
{
	if( szID.IsEmpty() ) return;

	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s(str_query,"exec game_character_get_self_index %d, %d", user_index, iSelectCount );

	const int queryId = 32;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( user_index );
	v_FT.Write( iSelectCount );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//Char INDEX

	CQueryData query_data;
	query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	//query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &user_index, sizeof(int) );
	query_data.SetReturnData( &iMsgResult, sizeof(int) );
	query_data.SetReturnData( &iLogType, sizeof(int) );
	query_data.SetReturnData( &iBuyPrice, sizeof(int) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_CHAR_INDEX_GET, 
		_SELECTEX1DB, 
		queryId,
		v_FT,
		v_VT );
	
	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectCharIndex Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectCharData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, int char_index, int iMsgResult, int iLogType /*= -1*/, int iBuyPrice /*= 0*/ )
{
	//if( szID.IsEmpty() )	return;
	
	//char str_query[MAX_QUERY_SIZE] = "";
	//wsprintf( str_query,"exec game_character_get_data %d", char_index );

   	const int queryId = 33;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( char_index );
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));				//User INDEX
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));				//Class Type
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));				//종족
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));				//성별
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));				//수염
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));				//얼굴
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));				//머리
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));				//피부색
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));				//머리색
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));				//장신구
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));				//속옷
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));				//아이템 종류 1
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));				//아이템 종류 2
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));				//아이템 종류 3
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));				//아이템 종류 4
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));               //용병 슬롯 위치
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));               //용병 남은 기간
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));               //용병 기간 타입
	v_VT.push_back(GetValueType(vSHORT,sizeof(short)));             //용병 대표 타입
	v_VT.push_back(GetValueType(vSHORT,sizeof(short)));             //용병 대여 타입
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));               //용병 대여 시간
	v_VT.push_back(GetValueType(vChar,sizeof(char)));				//용병 각성 타입
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));				//용병 각성 종료 Date

	CQueryData query_data;
	query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	//query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );
	//query_data.SetReturnData( &dwUserIndex, sizeof(int) );
	query_data.SetReturnData( &iMsgResult, sizeof(int) );
	query_data.SetReturnData( &char_index, sizeof(int) );
	query_data.SetReturnData( &iLogType, sizeof(int) );
	query_data.SetReturnData( &iBuyPrice, sizeof(int) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_CHAR_DATA_GET, 
		_SELECTDB,
		queryId,
		v_FT,
		v_VT );
	
	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectCharData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateCharData( const DWORD dwAgentID, const DWORD dwThreadID, ioCharacter &rkChar )
{
	const CHARACTER &rkCharInfo = rkChar.GetCharInfo();

	//char str_query[MAX_QUERY_SIZE] = "";
	////기본 정보
	//sprintf_s( str_query, "exec game_character_sava %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d", 
	//					rkChar.GetCharIndex(), rkCharInfo.m_class_type, rkCharInfo.m_kindred, rkCharInfo.m_sex,
	//					rkCharInfo.m_beard, rkCharInfo.m_face, rkCharInfo.m_hair, rkCharInfo.m_skin_color, rkCharInfo.m_hair_color,
	//					rkCharInfo.m_accessories, rkCharInfo.m_underwear, rkCharInfo.m_extra_item[0], rkCharInfo.m_extra_item[1], rkCharInfo.m_extra_item[2], 
	//					rkCharInfo.m_extra_item[3], rkCharInfo.m_iSlotIndex, rkCharInfo.m_iLimitSecond, (int)rkCharInfo.m_ePeriodType, rkCharInfo.m_sLeaderType, rkCharInfo.m_sRentalType, rkCharInfo.m_dwRentalMinute );	

	const int queryId = 34;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( rkChar.GetCharIndex() );
	v_FT.Write( rkCharInfo.m_class_type );
	v_FT.Write( rkCharInfo.m_kindred );
	v_FT.Write( rkCharInfo.m_sex );
	v_FT.Write( rkCharInfo.m_beard );
	v_FT.Write( rkCharInfo.m_face );
	v_FT.Write( rkCharInfo.m_hair );
	v_FT.Write( rkCharInfo.m_skin_color );
	v_FT.Write( rkCharInfo.m_hair_color );
	v_FT.Write( rkCharInfo.m_accessories );
	v_FT.Write( rkCharInfo.m_underwear );
	v_FT.Write( rkCharInfo.m_extra_item[0] );
	v_FT.Write( rkCharInfo.m_extra_item[1] );
	v_FT.Write( rkCharInfo.m_extra_item[2] );
	v_FT.Write( rkCharInfo.m_extra_item[3] );
	v_FT.Write( rkCharInfo.m_iSlotIndex );
	v_FT.Write( rkCharInfo.m_iLimitSecond );
	v_FT.Write( (int)rkCharInfo.m_ePeriodType );
	v_FT.Write( rkCharInfo.m_sLeaderType );
	v_FT.Write( rkCharInfo.m_sRentalType );
	v_FT.Write( rkCharInfo.m_dwRentalMinute );
	v_FT.Write( rkCharInfo.m_chAwakeType );
	v_FT.Write( rkCharInfo.m_iAwakeLimitTime );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_CHAR_DATA_UPD, 
		_UPDATEDB,
		queryId,
		v_FT,
		v_VT );
	
	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdateCharData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnDeleteCharData( const DWORD dwAgentID, const DWORD dwThreadID, int char_index )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s(str_query,"exec game_character_delete %d",char_index);
	
	const int queryId = 35;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( char_index );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_CHAR_DATA_DEL, 
		_DELETEDB, 
		queryId,
		v_FT,
		v_VT );
	
	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnDeleteCharData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnDeleteCharLimitDate( const DWORD dwAgentID, const DWORD dwThreadID, int user_index )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s(str_query,"exec game_character_delete_limitdate %d",user_index);

	const int queryId = 36;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( user_index );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_CHAR_DATE_LIMIT_DATE_DEL, 
		_DELETEDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnDeleteCharLimitDate Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateCharRegDate( const DWORD dwAgentID, const DWORD dwThreadID, int iCharIndex )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s(str_query,"exec game_character_regdate_save %d",iCharIndex);

	const int queryId = 37;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( iCharIndex );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_CHAR_REG_DATE_UPD, 
		_UPDATEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdateCharRegDate Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateCharRentalTime( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwCharIndex, DWORD dwTime )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s(str_query,"game_character_rentaltime_save %d, %d", dwCharIndex, dwTime );

	const int queryId = 166;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwCharIndex );
	v_FT.Write( dwTime );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_CHAR_RENTAL_TIME_UPD, 
		_UPDATEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdateCharRentalTime Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnInsertCharRentalHistory( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwRentUserIdx, DWORD dwLoanUserIdx, int iClassType )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_character_rental_history %d, %d, %d", dwRentUserIdx, dwLoanUserIdx, iClassType );

	const int queryId = 38;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwRentUserIdx );
	v_FT.Write( dwLoanUserIdx );
	v_FT.Write( iClassType );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_CHAR_RENTAL_HISTORY_SET, 
		_INSERTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnInsertCharRentalHistory Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectCharRentalHistory( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, DWORD dwRentUserIdx, const ioHashString &rkRequestID, int iClassType )
{
	CTimeSpan cGapTime( 0, Help::GetBestFriendCharRentalDelayHour(), 0, 0 );
	CTime kLimitTime = CTime::GetCurrentTime() - cGapTime;
	//char szLimitDate[MAX_PATH] = "";
	//sprintf_s( szLimitDate, "\'%d-%d-%d %d:%d:%d\'", kLimitTime.GetYear(), kLimitTime.GetMonth(), kLimitTime.GetDay(), kLimitTime.GetHour(), 
	//					  kLimitTime.GetMinute(), kLimitTime.GetSecond() );

	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_character_rental_list %d, %s", dwRentUserIdx, szLimitDate );

	SYSTEMTIME sysTime;
	kLimitTime.GetAsSystemTime( sysTime );

   	const int queryId = 39;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwRentUserIdx );
	v_FT.Write( (uint8*)&sysTime, sizeof(sysTime), TRUE );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );					// 빌려준 유저 인덱스
	v_VT.push_back( GetValueType( vTimeStamp, sizeof(DBTIMESTAMP) ) );      // 기록 날짜

	CQueryData query_data;
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwRentUserIdx, sizeof(int) );
	query_data.SetReturnData( rkRequestID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &iClassType, sizeof(int) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_CHAR_RENTAL_HISTORY_GET, 
		_SELECTEX1DB, 
		queryId,
		v_FT,
		v_VT );
	
	SP2Packet kPacket(DTPK_QUERY);
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectCharRentalHistory Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnInsertClassExpert( 
								   const DWORD dwAgentID, 
								   const DWORD dwThreadID, 
								   const ioHashString &szUserGUID, 
								   const ioHashString &szID, 
								   DWORD dwUserIdx, 
								   std::vector<int>& contents )
{
	if( szID.IsEmpty() ) return;

	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_class_add %d%s", dwUserIdx, szContent );

	const int queryId = 40;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );
	for( std::vector<int>::iterator it = contents.begin() ; it != contents.end() ; ++it )
	{
		v_FT.Write( *it );
	}

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_CLASS_EXPERT_DATA_SET, 
		_INSERTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnInsertClassExpert Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}

	OnSelectClassExpertIndex( dwAgentID, dwThreadID, szUserGUID, szID, dwUserIdx );
}

void DBClient::OnSelectClassExpertIndex( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_class_get_self_index %d", dwUserIdx );

	const int queryId = 41;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//Class Expert INDEX

	CQueryData query_data;
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	//query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwUserIdx, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_CLASS_EXPERT_DATA_NEW_INDEX, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectClassExpertIndex Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateClassExpert( const DWORD dwAgentID, const DWORD dwThreadID, int user_index, cSerialize& v_FT  )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_class_save %d%s", dwClassInfoIdx, szContent );

	const int queryId = 42;

	vVALUETYPE v_VT;

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_CLASS_EXPERT_DATA_UPD, 
		_UPDATEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdateClassExpert Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnInsertInvenData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx, std::vector<int>& contents, bool bBuyCash, int iBuyPrice, int iLogType )
{
	if( szID.IsEmpty() ) return;

	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_item_add %d%s", dwUserIdx, szContent );

	const int queryId = 43;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );
	for( std::vector<int>::iterator it = contents.begin() ; it != contents.end() ; ++it )
	{
		v_FT.Write( *it );
	}

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_INVEN_DATA_SET, 
		_INSERTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnInsertInvenData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
	OnSelectInvenIndex( dwAgentID, dwThreadID, szUserGUID, szID, dwUserIdx, bBuyCash, iBuyPrice, iLogType );
}

void DBClient::OnSelectInvenIndex( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx, bool bBuyCash, int iBuyPrice, int iLogType )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_item_get_self_index %d", dwUserIdx );

	const int queryId = 44;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//Item INDEX

	CQueryData query_data;
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	//query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwUserIdx, sizeof(DWORD) );
	query_data.SetReturnData( &bBuyCash, sizeof( bool ) );
	query_data.SetReturnData( &iBuyPrice, sizeof( int ) );
	query_data.SetReturnData( &iLogType, sizeof( int ) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_INVEN_DATA_CREATE_INDEX, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectInvenIndex Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateInvenData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szID, DWORD dwInvenIdx, std::vector<int>& contents )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_item_save %d%s", dwInvenIdx, szContent );

	const int queryId = 45;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwInvenIdx );
	for( std::vector<int>::iterator it = contents.begin() ; it != contents.end() ; ++it )
	{
		v_FT.Write( *it );
	}

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_INVEN_DATA_UPD, 
		_UPDATEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdateInvenData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnInsertEtcItemData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx, std::vector<int>& contents, bool bBuyCash, int iBuyPrice )
{
	if( szID.IsEmpty() ) return;

	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_item_etc_add %d%s", dwUserIdx, szContent );

	const int queryId = 46;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );
	for( std::vector<int>::iterator it = contents.begin() ; it != contents.end() ; ++it )
	{
		v_FT.Write( *it );
	}

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_ETC_ITEM_DATA_SET, 
		_INSERTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnInsertEtcItemData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
	OnSelectEtcItemIndex( dwAgentID, dwThreadID, szUserGUID, szID, dwUserIdx, bBuyCash, iBuyPrice );
}

void DBClient::OnSelectEtcItemIndex( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx, bool bBuyCash /*= false*/, int iBuyPrice /*= 0 */ )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_item_etc_get_self_index %d", dwUserIdx );

	const int queryId = 47;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//Item INDEX

	CQueryData query_data;
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	//query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwUserIdx, sizeof( int ) );
	query_data.SetReturnData( &bBuyCash, sizeof( bool ) );
	query_data.SetReturnData( &iBuyPrice, sizeof( int ) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_ETC_ITEM_DATA_CREATE_INDEX, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectEtcItemIndex Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateEtcItemData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szID, DWORD dwInvenIdx, std::vector<int>& contents )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_item_etc_save %d%s", dwInvenIdx, szContent );

	const int queryId = 48;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwInvenIdx );
	for( std::vector<int>::iterator it = contents.begin() ; it != contents.end() ; ++it )
	{
		v_FT.Write( *it );
	}

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_ETC_ITEM_DATA_UPD, 
		_UPDATEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdateEtcItemData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnInsertExtraItemData( 
									 const DWORD dwAgentID, 
									 const DWORD dwThreadID, 
									 const ioHashString &szUserGUID, 
									 const ioHashString &szID, 
									 DWORD dwUserIdx, 
									 cSerialize& v_FT, 
									 bool bBuyCash, int iBuyPrice, int iLogType, int iMachineCode, int iPeriodTime )
{
	if( szID.IsEmpty() ) return;

	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_item_extra_add %d%s", dwUserIdx, szContent );

	const int queryId = 49;

	vVALUETYPE v_VT;

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_EXTRAITEM_DATA_SET, 
		_INSERTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnInsertExtraItemData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
	OnSelectExtraItemIndex( dwAgentID, dwThreadID, szUserGUID, szID, dwUserIdx, bBuyCash, iBuyPrice, iLogType, iMachineCode, iPeriodTime );
}

void DBClient::OnSelectExtraItemIndex( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx, bool bBuyCash, int iBuyPrice, int iLogType, int iMachineCode, int iPeriodTime )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_item_extra_get_self_index %d", dwUserIdx );

	const int queryId = 50;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//Item INDEX

	CQueryData query_data;
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	//query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwUserIdx, sizeof( int ) );
	query_data.SetReturnData( &bBuyCash, sizeof( bool ) );
	query_data.SetReturnData( &iBuyPrice, sizeof( int ) );
	query_data.SetReturnData( &iLogType, sizeof( int ) );
	query_data.SetReturnData( &iMachineCode, sizeof( int ) );
	query_data.SetReturnData( &iPeriodTime, sizeof( int ) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_EXTRAITEM_DATA_CREATE_INDEX, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectExtraItemIndex Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateExtraItemData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szID, DWORD dwInvenIdx, cSerialize& v_FT  )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_item_extra_save %d%s", dwInvenIdx, szContent );

	const int queryId = 51;

	vVALUETYPE v_VT;
	
	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_EXTRAITEM_DATA_UPD, 
		_UPDATEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdateExtraItemData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

//
void DBClient::OnUpdateAwardExpert( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIdx, int iAwardLevel, int iAwardExp )
{
	char str_query[MAX_QUERY_SIZE] = "";
	sprintf_s( str_query, "exec game_award_level_save %d, %d, %d", dwUserIdx, iAwardLevel, iAwardExp );

	const int queryId = 52;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );
	v_FT.Write( iAwardLevel );
	v_FT.Write( iAwardExp );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_AWARD_EXPERT_UPD, 
		_UPDATEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdateAwardExpert Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnInsertAwardData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx, std::vector<int>& contents )
{
	if( szID.IsEmpty() ) return;

	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_award_add %d%s", dwUserIdx, szContent );

	const int queryId = 53;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );
	for( std::vector<int>::iterator it = contents.begin() ; it != contents.end() ; ++it )
	{
		v_FT.Write( *it );
	}

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_AWARD_DATA_SET, 
		_INSERTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnInsertAwardData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
	OnSelectAwardIndex( dwAgentID, dwThreadID, szUserGUID, szID, dwUserIdx );
}

void DBClient::OnSelectAwardIndex( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx )
{
	char str_query[MAX_QUERY_SIZE] = "";
	sprintf_s( str_query, "exec game_award_get_self_index %d", dwUserIdx );

	const int queryId = 54;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//Char INDEX

	CQueryData query_data;
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	//query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwUserIdx, sizeof(int) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_AWARD_DATA_CREATE_INDEX, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectAwardIndex Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateAwardData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szID, DWORD dwInvenIdx, std::vector<int>& contents )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_award_save %d%s", dwInvenIdx, szContent );

	const int queryId = 55;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwInvenIdx);
	for( std::vector<int>::iterator it = contents.begin() ; it != contents.end() ; ++it )
	{
		v_FT.Write( *it );
	}

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_AWARD_DATA_UPD, 
		_UPDATEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdateAwardData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnInsertGrowth( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx, cSerialize& v_FT )
{
	if( szID.IsEmpty() ) return;

	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_ability_add %d%s", dwUserIdx, szContent );

	const int queryId = 56;

	vVALUETYPE v_VT;

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_GROWTH_DATA_SET, 
		_INSERTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnInsertGrowth Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
	OnSelectGrowthIndex( dwAgentID, dwThreadID, szUserGUID, szID, dwUserIdx );
}

void DBClient::OnSelectGrowthIndex( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_ability_get_self_index %d", dwUserIdx );

	const int queryId = 57;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//Growth INDEX

	CQueryData query_data;
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	//query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwUserIdx, sizeof(int) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_GROWTH_DATA_NEW_INDEX, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectGrowthIndex Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateGrowth( const DWORD dwAgentID, const DWORD dwThreadID, int user_index, DWORD dwClassInfoIdx, cSerialize& v_FT )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_ability_save %d%s", dwClassInfoIdx, szContent );

	const int queryId = 58;

	//cSerialize v_FT;
	vVALUETYPE v_VT;

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_GROWTH_DATA_UPD, 
		_UPDATEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdateGrowth Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}


void DBClient::OnInsertFishData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx, cSerialize& v_FT )
{
	if( szID.IsEmpty() ) return;

	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_fish_add %d%s", dwUserIdx, szContent );

	const int queryId = 59;

	vVALUETYPE v_VT;

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_FISH_DATA_SET, 
		_INSERTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnInsertFishData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
	OnSelectFishDataIndex( dwAgentID, dwThreadID, szUserGUID, szID, dwUserIdx );
}

void DBClient::OnSelectFishDataIndex( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx )
{
	char str_query[MAX_QUERY_SIZE] = "";
	sprintf_s( str_query, "exec game_fish_get_self_index %d", dwUserIdx );

	const int queryId = 60;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//Growth INDEX

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIdx, sizeof(int) );
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	//query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_FISH_DATA_NEW_INDEX, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectFishDataIndex Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectAllFishData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD user_index )
{
	char str_query[MAX_QUERY_SIZE] = "";
	sprintf_s( str_query, "exec game_fish_get_data_list %d", user_index );

	const int queryId = 61;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( user_index );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );					//인덱스

	for(int i = 0;i < ioUserFishingItem::MAX_SLOT;i++)
	{
		v_VT.push_back( GetValueType( vChar, sizeof(char) ) );				//타입
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//Array
	}

	CQueryData query_data;
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	//query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &user_index, sizeof(int) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_FISH_DATA_GET, 
		_SELECTEX1DB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectAllFishData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateFishData( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIdx, DWORD dwClassInfoIdx, cSerialize& v_FT )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_fish_save %d%s", dwClassInfoIdx, szContent );

	const int queryId = 62;

	vVALUETYPE v_VT;

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_FISH_DATA_UPD, 
		_UPDATEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdateFishData Send Fail! :%d - %d", GetLastError(), queryId );
	}
}

void DBClient::OnSelectFriendList( const DWORD dwAgentID, const DWORD dwThreadID, int iFriendIDX, const DWORD dwUserIndex, const ioHashString &szPublicID, int iSelectCount )
{
	if( szPublicID.IsEmpty() )	return;

	//char szContent[MAX_PATH] = "";
	//sprintf_s( szContent, "%d, %d, %d", iFriendIDX, dwUserIndex, iSelectCount );

	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_friend_list_asc %s", szContent );

	const int queryId = 63;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( iFriendIDX );
	v_FT.Write( dwUserIndex );
	v_FT.Write( iSelectCount );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				 //테이블 인덱스
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				 //친구 인덱스
	v_VT.push_back( GetValueType( vChar, ID_NUM_PLUS_ONE ) );            //친구 닉네임
	v_VT.push_back( GetValueType( vTimeStamp, sizeof(DBTIMESTAMP) ) );   //친구 수락 시간
	v_VT.push_back( GetValueType( vLONG, sizeof( LONG ) ) );			// 보낸갯수
	v_VT.push_back( GetValueType( vLONG, sizeof( LONG ) ) );			// 보낸 시간
	v_VT.push_back( GetValueType( vLONG, sizeof( LONG ) ) );			// 받은갯수
	v_VT.push_back( GetValueType( vLONG, sizeof( LONG ) ) );			// 받은 시간
	v_VT.push_back( GetValueType( vLONG, sizeof( LONG ) ) );			// 받기전갯수

	CQueryData query_data;
	query_data.SetReturnData( szPublicID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_FRIEND_LIST_GET, 
		_SELECTEX1DB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectFriendList Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectFriendRequestList( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwTableIndex, const DWORD dwUserIndex, int iSelectCount )
{
	if( dwTableIndex == 0 ) return;

	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_friend_join_list %d, %d, %d", dwTableIndex, dwUserIndex, iSelectCount );

	const int queryId = 64;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwTableIndex );
	v_FT.Write( dwUserIndex );
	v_FT.Write( iSelectCount );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				 //테이블 인덱스
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				 //친구 인덱스
	v_VT.push_back( GetValueType( vChar, ID_NUM_PLUS_ONE ) );            //친구 닉네임

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_FRIEND_REQUEST_LIST_GET,
		_SELECTEX1DB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectFriendRequestList Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectFriendApplication( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, const ioHashString &szFriendID )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_friend_join_in %d, '%s'", dwUserIndex, szFriendID.c_str() );

	const int queryId = 65;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( szFriendID.c_str(), szFriendID.Length(), TRUE );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				 //결과

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( szFriendID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_FRIEND_APPLICATION_GET, 
		_SELECTDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectFriendApplication Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectUserIDCheck( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szPublicID, const ioHashString &szUserPublicID )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_friend_check_member '%s'", szUserPublicID.c_str() );

	const int queryId = 66;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( szUserPublicID.c_str(), szUserPublicID.Length(), TRUE );
	v_VT.push_back( GetValueType( vChar, ID_NUM_PLUS_ONE ) );	//유저 아이디
	v_VT.push_back( GetValueType( vChar, ID_NUM_PLUS_ONE ) );	//유저 닉네임
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//유저 어카운트 인덱스	

	CQueryData query_data;
	query_data.SetReturnData( szPublicID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_FRIEND_USERID_CHECK_GET, 
		_SELECTDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectUserIDCheck Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnDeleteFriendRequest( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwTableIndex )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s(str_query,"exec game_friend_join_delete %d", dwTableIndex );

	const int queryId = 67;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwTableIndex );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_FRIEND_REQUEST_DELETE_DEL, 
		_DELETEDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnDeleteFriendRequest Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectInsertFriend( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const ioHashString &szUserName, const DWORD dwTableIndex, const DWORD dwFriendIndex, const ioHashString &szFriendName )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s(str_query,"exec game_friend_join_app %d, %d", dwUserIndex, dwFriendIndex );

	const int queryId = 68;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( dwFriendIndex );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				 //결과

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( szUserName.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwTableIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwFriendIndex, sizeof(DWORD) );
	query_data.SetReturnData( szFriendName.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_FRIEND_INSERT_GET, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectInsertFriend Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnDeleteFriend( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const ioHashString &szUserID, const ioHashString &szFriendPublicID )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s(str_query,"exec game_friend_delete %d,'%s'", dwUserIndex, szFriendPublicID.c_str() );

	const int queryId = 69;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( szFriendPublicID.c_str(), szFriendPublicID.Length(), TRUE );

	CQueryData query_data;
	query_data.SetReturnData( szUserID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( szFriendPublicID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_FRIEND_DELETE_DEL, 
		_DELETEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnDeleteFriend Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectFriendDeveloperInsert( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const ioHashString &szUserName )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s(str_query,"exec game_friend_dev_add %d,'%s'", dwUserIndex, szUserName.c_str() );

	const int queryId = 70;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( szUserName.c_str(), szUserName.Length(), TRUE );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				 //결과

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( szUserName.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_FRIEND_DEVELOPER_INSERT_GET, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectFriendDeveloperInsert Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectBestFriendList( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_friend_best_list %d", dwUserIndex );

	const int queryId = 71;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );			//절친 테이블 인덱스
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );			//절친 유저 인덱스
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );			//절친 상태
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );			//절친 시간값

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_BEST_FRIEND_LIST_GET, 
		_SELECTEX1DB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectBestFriendList Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnDeleteBestFirendTable( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwTableIndex )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s(str_query,"exec game_friend_best_delete %d", dwTableIndex );

	const int queryId = 72;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwTableIndex );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_BEST_FRIEND_DEL, 
		_DELETEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnDeleteBestFirendList Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateBestFriendList( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwTableIndex, const DWORD dwState, const DWORD dwMagicDate )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_friend_best_save %d, %d, %d", dwTableIndex, dwState, dwMagicDate );

	const int queryId = 73;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwTableIndex );
	v_FT.Write( dwState );
	v_FT.Write( dwMagicDate );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_BEST_FRIEND_LIST_UPD, 
		_UPDATEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdateBestFriendList Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnInsertBestFriendAdd( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwFriendIndex )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_friend_best_add %d, %d", dwUserIndex, dwFriendIndex );

	const int queryId = 74;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( dwFriendIndex );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_BEST_FRIEND_ADD_SET, 
		_INSERTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnInsertBestFriendAdd Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
	OnSelectBestFriendAddResult( dwAgentID, dwThreadID, dwUserIndex, dwFriendIndex );
}

void DBClient::OnSelectBestFriendAddResult( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwFriendIndex )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_friend_best_add_self_index %d, %d", dwUserIndex, dwFriendIndex );

	const int queryId = 75;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( dwFriendIndex );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );			//절친 테이블 인덱스 - 0이면 회원 탈퇴한 유저

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwFriendIndex, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_BEST_FRIEND_ADD_RESULT_GET, 
		_SELECTDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectBestFriendAddResult Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateUserRecord( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, std::vector<int>& contents )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_record_battle_save %d%s", dwUserIndex, szContent );

	const int queryId = 76;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex);
	for( std::vector<int>::iterator it = contents.begin() ; it != contents.end() ; ++it )
	{
		v_FT.Write( *it );
	}

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_USER_RECORD_UPD, 
		_UPDATEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdateUserRecord Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnInsertQuestData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx, std::vector<int>& contents )
{
	if( szID.IsEmpty() ) return;

	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_quest_add %d%s", dwUserIdx, szContent );

	const int queryId = 77;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );
	for( std::vector<int>::iterator it = contents.begin() ; it != contents.end() ; ++it )
	{
		v_FT.Write( *it );
	}

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_QUEST_DATA_SET, 
		_INSERTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnInsertQuestData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
	OnSelectQuestIndex( dwAgentID, dwThreadID, szUserGUID, szID, dwUserIdx );
}

void DBClient::OnSelectQuestIndex( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_quest_get_self_index %d", dwUserIdx );

	const int queryId = 78;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		// INDEX

	CQueryData query_data;
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	//query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwUserIdx, sizeof(int) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_QUEST_DATA_CREATE_INDEX, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectQuestIndex Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateQuestData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szID, DWORD dwInvenIdx, std::vector<int>& contents )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_quest_save %d%s", dwInvenIdx, szContent );

	const int queryId = 79;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwInvenIdx );
	for( std::vector<int>::iterator it = contents.begin() ; it != contents.end() ; ++it )
	{
		v_FT.Write( *it );
	}

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_QUEST_DATA_UPD, 
		_UPDATEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	
	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdateQuestData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnDeleteQuestData( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwInvenIdex )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_quest_delete %d", dwInvenIdex );

	const int queryId = 80;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwInvenIdex );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_NAUGHT, 
		DBAGENT_QUEST_DATA_DEL, 
		_DELETEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnDeleteQuestData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnInsertQuestCompleteData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx, std::vector<int>& contents )
{
	if( szID.IsEmpty() ) return;

	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_questcomplete_add %d%s", dwUserIdx, szContent );

	const int queryId = 81;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );
	for( std::vector<int>::iterator it = contents.begin() ; it != contents.end() ; ++it )
	{
		v_FT.Write( *it );
	}

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_QUEST_COMPLETE_DATA_SET, 
		_INSERTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnInsertQuestCompleteData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
	OnSelectQuestCompleteIndex( dwAgentID, dwThreadID, szUserGUID, szID, dwUserIdx );
}

void DBClient::OnSelectQuestCompleteIndex( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_questcomplete_get_self_index %d", dwUserIdx );

	const int queryId = 82;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		// INDEX

	CQueryData query_data;
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	//query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwUserIdx, sizeof(int) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_QUEST_COMPLETE_DATA_CREATE_INDEX, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectQuestCompleteIndex Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateQuestCompleteData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szID, DWORD dwInvenIdx, std::vector<int>& contents )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_questcomplete_save %d%s", dwInvenIdx, szContent );

	const int queryId = 83;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwInvenIdx );
	for( std::vector<int>::iterator it = contents.begin() ; it != contents.end() ; ++it )
	{
		v_FT.Write( *it );
	}

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK,
		DBAGENT_QUEST_COMPLETE_DATA_UPD,
		_UPDATEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdateQuestCompleteData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnInsertQuestWebAlarm( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szID, DWORD dwUserIdx, int iGradeLevel, DWORD dwQuestIndex, char *szPublicIP )
{
	if( szID.IsEmpty() ) return;

	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_quest_board_add %d, '%s', %d, %d, '%s'", dwUserIdx, szID.c_str(), iGradeLevel, dwQuestIndex, szPublicIP );

	const int queryId = 84;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );
	v_FT.Write( szID.c_str(), szID.Length(), TRUE );
	v_FT.Write( iGradeLevel );
	v_FT.Write( dwQuestIndex );
	v_FT.Write( szPublicIP, strlen(szPublicIP), TRUE );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_QUEST_WEB_ALARM_SET, 
		_INSERTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnInsertQuestWebAlarm Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnInsertTrial( const DWORD dwAgentID, const DWORD dwThreadID, User *pReportUser, const ioHashString &szCriminalUserID , const ioHashString &rszChat, const ioHashString &rszChatType, const ioHashString &rszBattleRoomUserInfo, const ioHashString &rszCriminalIP , const ioHashString &rszReason, int iChatType , DWORD dwChannelIndex )
{
	if( !pReportUser ) return;

	LogDBClient::GameType eGameType = g_LogDBClient.GetGameType( pReportUser->GetPlayingMode(), pReportUser->GetMyRoom() );
	
	Room *pRoom = pReportUser->GetMyRoom();
	DWORD dwPlayTime  = 0;
	int   iBlueWinCnt = 0;
	int   iRedWinCnt  = 0;
	int   iModeSubNum = 0;
	int   iModeMapNum = 0;
	std::string szRoomInfo;

	szRoomInfo = rszChatType.c_str();
	szRoomInfo += "<br><br>";

	if( pRoom ) 
	{
		DWORD dwModeStartTime = pRoom->GetModeStartTime();
		if( dwModeStartTime != 0)
			dwPlayTime  = TIMEGETTIME() - dwModeStartTime;
		iBlueWinCnt = pRoom->GetBlueWinCnt();
		iRedWinCnt  = pRoom->GetRedWinCnt();
		iModeSubNum = pRoom->GetModeSubNum();
		iModeMapNum = pRoom->GetModeMapNum();

		if( eGameType == LogDBClient::GT_SQUARE )
		{
			szRoomInfo +=  "Room info";
			szRoomInfo += "<br><br>Room name : ";
			szRoomInfo += pRoom->GetRoomName().c_str();
			szRoomInfo += "<br>Room master : ";
			szRoomInfo += pRoom->GetMasterName().c_str();
			szRoomInfo += "<br>Team : ";
			szRoomInfo += Help::GetTeamTypeString( pReportUser->GetTeam() ).c_str();

			char szTemp[MAX_PATH];
			ZeroMemory( szTemp, sizeof(szTemp) );
			StringCbPrintf( szTemp, sizeof(szTemp), "<br>Round time : %u<br>Round score : %d(blue)/%d(red)", 
				dwPlayTime,
				iBlueWinCnt,
				iRedWinCnt);
			szRoomInfo += szTemp;

			szRoomInfo += "<br>User : ";
			for (int i = 0; i < pRoom->GetJoinUserCnt() ; i++)
			{
				User *pInRoomUser = pRoom->GetUserNodeByArray( i );
				if( pInRoomUser )
				{
					szRoomInfo += pInRoomUser->GetPublicID().c_str();
					szRoomInfo += "(";
					szRoomInfo += Help::GetTeamTypeString( pInRoomUser->GetTeam()).c_str();
					szRoomInfo += "), ";
				}
			}
		}
	}

	if( pReportUser->IsBattleRoom() )
	{
		// my
		BattleRoomParent *pBattleRoom = pReportUser->GetMyBattleRoom();
		if( pBattleRoom && pBattleRoom->IsOriginal() )
		{
			BattleRoomNode *pOriginal = (BattleRoomNode *)pBattleRoom;
			szRoomInfo += "<br><br><br>My party info";
			szRoomInfo += "<br><br>Party name : ";
			szRoomInfo += pOriginal->GetName().c_str();
			szRoomInfo += "<br>Party master : ";
			szRoomInfo += pOriginal->GetOwnerName().c_str();
			szRoomInfo += "<br>Team : ";
			szRoomInfo += Help::GetTeamTypeString( pReportUser->GetTeam() ).c_str();

			char szTemp[MAX_PATH];
			ZeroMemory( szTemp, sizeof(szTemp) );
			StringCbPrintf( szTemp, sizeof(szTemp), "<br>Round time : %u<br>Round score : %d(blue)/%d(red)", 
							dwPlayTime,
							iBlueWinCnt,
							iRedWinCnt );
			szRoomInfo += szTemp;

			szRoomInfo += "<br>User : ";
			szRoomInfo += rszBattleRoomUserInfo.c_str();
		}
	}

	std::string szResultChat;
	Help::GetSafeTextWriteDB( szResultChat, rszChat );
	std::string szResultReason;
	Help::GetSafeTextWriteDB( szResultReason, rszReason );

	//static char str_query[4096] = "";
	//ZeroMemory(str_query, sizeof( str_query ) );
	//StringCbPrintf(str_query, sizeof(str_query), "exec game_fame_give_point  %d,%d,%d,  %d,%d,'%s','%s',  '%s','%s','%s',  '%s','%s','%s'" ,
	//	eGameType, 
	//	pReportUser->GetPlayingMode(), 
	//	iModeSubNum, 

	//	iModeMapNum, 
	//	pReportUser->GetUserIndex(),
	//	pReportUser->GetPublicID().c_str(),
	//	pReportUser->GetPublicIP(), 

	//	pReportUser->GetPrivateIP(), 
	//	szCriminalUserID.c_str(), 
 //       rszCriminalIP.c_str(),

	//	szRoomInfo.c_str(),
	//	szResultChat.c_str(),
	//	szResultReason.c_str() );

	//size_t iSize = 0;
	//StringCbLength(str_query, sizeof( str_query ) , &iSize );

	//if( iSize == 0)
	//{
	//	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"%s - Query over! :%s:%s:%d",__FUNCTION__,
	//		                pReportUser->GetPublicID().c_str(), szCriminalUserID.c_str(), GetLastError() );
	//	return;
	//}

	const int queryId = 85;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( eGameType );
	v_FT.Write( pReportUser->GetPlayingMode() );
	v_FT.Write( iModeSubNum );
	v_FT.Write( iModeMapNum );
	v_FT.Write( pReportUser->GetUserIndex() );
	v_FT.Write( pReportUser->GetPublicID().c_str(), pReportUser->GetPublicID().Length(), TRUE );
	v_FT.Write( pReportUser->GetPublicIP(), strlen(pReportUser->GetPublicIP()), TRUE );
	v_FT.Write( pReportUser->GetPrivateIP(), strlen(pReportUser->GetPrivateIP()), TRUE );
	v_FT.Write( szCriminalUserID.c_str(), szCriminalUserID.Length(), TRUE );
	v_FT.Write( rszCriminalIP.c_str(), rszCriminalIP.Length(), TRUE );
	v_FT.Write( szRoomInfo.c_str(), szRoomInfo.length(), TRUE );
	v_FT.Write( szResultChat.c_str(), szResultChat.length(), TRUE );
	v_FT.Write( szResultReason.c_str(), szResultReason.length(), TRUE );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_TRIAL_SET, 
		_INSERTDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"%s - Send Fail! :%d",__FUNCTION__, GetLastError());
		return;
	}
}

void DBClient::OnUpdateEventData( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwIndex , int iValue1, int iValue2 )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_event_save %d, %d, %d", dwIndex , iValue1, iValue2);

	const int queryId = 86;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwIndex );
	v_FT.Write( iValue1 );
	v_FT.Write( iValue2 );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_EVENT_DATA_UPD, 
		_UPDATEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdateEventData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnInsertEventData( const DWORD dwAgentID, const DWORD dwThreadID, const char *szUserGUID, DWORD dwUserIndex, int iValue1, int iValue2, int iEventType )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_event_add %d, %d, %d, %d", dwUserIndex, iValue1, iValue2, iEventType );

	const int queryId = 87;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( iValue1 );
	v_FT.Write( iValue2 );
	v_FT.Write( iEventType );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_EVENT_DATA_SET, 
		_INSERTDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnInsertEventData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}

	OnSelectEventIndex( dwAgentID, dwThreadID, szUserGUID, dwUserIndex, iEventType );
}

void DBClient::OnSelectEventIndex( const DWORD dwAgentID, const DWORD dwThreadID, const char *szUserGUID, DWORD dwUserIndex, int iEventType )
{
	if( szUserGUID == NULL )	
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectEventData Error - User GUID is NULL. :%d :%d", dwUserIndex, iEventType );
		return;
	}

	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_event_get_createIDX %d, %d", dwUserIndex, iEventType );

	const int queryId = 88;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( iEventType );
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG))); // index

	CQueryData query_data;
	query_data.SetReturnData( szUserGUID, USER_GUID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwUserIndex, sizeof(int) );
	query_data.SetReturnData( &iEventType, sizeof(int) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_EVENT_INDEX_GET, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectEventIndex Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnInsertEventLog( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, int iAddPeso )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_event_openbeta_coin_log %d, %d", dwUserIndex, iAddPeso );

	const int queryId = 89;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( iAddPeso );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_EVENT_LOG_SET, 
		_INSERTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnInsertEventLog Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateGuildEtcItemDelete( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, int iFieldNum, DWORD dwTableIndex )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_guild_use_item '%d', '%u'", iFieldNum, dwTableIndex );

	char fieldNum[64], tableIndex[64];
	sprintf_s( fieldNum, "%d", iFieldNum );
	sprintf_s( tableIndex, "%u", dwTableIndex );

	const int queryId = 90;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( fieldNum, strlen(fieldNum), TRUE );
	v_FT.Write( tableIndex, strlen(tableIndex), TRUE );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_GUILD_ETC_ITEM_DELETE_UPD, 
		_UPDATEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdateGuildEtcItemDelete Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectCreateGuild( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, const ioHashString &szGuildName, const ioHashString &szGuildTitle, 
								    const int iGuildMark, DWORD dwTableIndex, int iFieldNum,  const int iGuildMaxEntry )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_guild_create_guild %d, '%s', '%s', %d, %d", dwUserIndex, szGuildName.c_str(), szGuildTitle.c_str(),
	//																		 iGuildMark, iGuildMaxEntry );

	const int queryId = 91;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( szGuildName.c_str(), szGuildName.Length(), TRUE );
	v_FT.Write( szGuildTitle.c_str(), szGuildTitle.Length(), TRUE );
	v_FT.Write( iGuildMark );
	v_FT.Write( iGuildMaxEntry );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		// RESULT

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwTableIndex, sizeof(DWORD) );
	query_data.SetReturnData( &iFieldNum, sizeof(int) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_CREATE_GUILD_GET, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectCreateGuild Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectCreateGuildReg( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_guild_user_get_data %d", dwUserIndex );

	const int queryId = 92;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );					// 길드 인덱스
	v_VT.push_back( GetValueType( vChar, GUILD_NAME_NUM_PLUS_ONE ) );		// 길드 이름
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );					// 길드 마크

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_CREATE_GUILD_REG_GET, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectCreateGuild Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectCreateGuildInfo( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_guild_login_get_data %d", dwUserIndex );

	const int queryId = 93;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				// GUILD INDEX
	v_VT.push_back( GetValueType( vChar, GUILD_NAME_NUM_PLUS_ONE ) );	// GUILD NAME
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				// GUILD MARK
	v_VT.push_back( GetValueType( vChar, GUILD_POS_NUM_PLUS_ONE ) );	// GUILD POSITION
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				// GUILD EVENT
	v_VT.push_back( GetValueType( vTimeStamp, sizeof(DBTIMESTAMP) ) );	// 출석 보상 받은 날
	v_VT.push_back( GetValueType( vTimeStamp, sizeof(DBTIMESTAMP) ) );	// 길드 랭크 보상 받은 날
	v_VT.push_back( GetValueType( vTimeStamp, sizeof(DBTIMESTAMP) ) );	// 길드 가입날
	v_VT.push_back( GetValueType( vChar, sizeof(char) ) );	// 길드 룸 open 여부.

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_CREATE_GUILD_INFO_GET, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectCreateGuildInfo Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateCreateGuildFailPeso( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, int iAddPeso )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_guild_fail_add_peso %d, %d", dwUserIndex, iAddPeso );

	const int queryId = 94;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( iAddPeso );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_CREATE_GUILD_FAIL_PESO_UPD, 
		_UPDATEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdateCreateGuildFailPeso Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectUserGuildInfo( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_guild_login_get_data %d", dwUserIdx );

	const int queryId = 95;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				// GUILD INDEX
	v_VT.push_back( GetValueType( vChar, GUILD_NAME_NUM_PLUS_ONE ) );	// GUILD NAME
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				// GUILD MARK
	v_VT.push_back( GetValueType( vChar, GUILD_POS_NUM_PLUS_ONE ) );	// GUILD POSITION
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				// GUILD EVENT
	v_VT.push_back( GetValueType( vTimeStamp, sizeof(DBTIMESTAMP) ) );	// 출석 보상 받은 날
	v_VT.push_back( GetValueType( vTimeStamp, sizeof(DBTIMESTAMP) ) );	// 길드 랭크 보상 받은 날
	v_VT.push_back( GetValueType( vTimeStamp, sizeof(DBTIMESTAMP) ) );	// 길드 가입날
	v_VT.push_back( GetValueType( vChar, sizeof(char) ) );	// 길드 룸 open 여부.

	CQueryData query_data;
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	//query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwUserIdx, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_USER_GUILD_INFO_GET, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectUserGuildInfo Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectEntryDelayGuildList( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_guild_join_my_list %d", dwUserIndex );

	const int queryId = 96;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				// GUILD INDEX

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_ENTRY_DELAY_GUILD_LIST_GET, 
		_SELECTEX1DB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectEntryDelayGuildList Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnDeleteEntryDelayGuildList( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_guild_join_self_init %d", dwUserIndex );

	const int queryId = 97;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_GUILD_DELAY_GUILD_LIST_DEL, 
		_DELETEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnDeleteEntryDelayGuildList Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnDeleteGuildMemberDelete( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, DWORD dwGuildIndex )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_guild_member_out_delete %d, %d", dwUserIndex, dwGuildIndex );

	const int queryId = 98;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( dwGuildIndex );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_GUILD_MEMBER_DELETE_DEL, 
		_DELETEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnDeleteGuildMemberDelete Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateGuildMemberEvent( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, DWORD dwGuildIndex )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_guild_msg_init %d, %d", dwUserIndex, dwGuildIndex );

	const int queryId = 99;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( dwGuildIndex );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_GUILD_MEMBER_EVENT_UPD, 
		_UPDATEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdateGuildMemberEvent Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectGuildEntryDelayMember( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, DWORD dwGuildIndex )
{	
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_guild_join_get_list %d", dwGuildIndex );
	
	const int queryId = 100;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwGuildIndex );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				// TABLE INDEX
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				// USER INDEX
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				// USER LEVEL
	v_VT.push_back( GetValueType( vChar, ID_NUM_PLUS_ONE ) );			// USER NICK

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwGuildIndex, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_GUILD_ENTRY_DELAY_MEMBER_GET, 
		_SELECTEX1DB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectGuildEntryDelayMember Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}	
}

void DBClient::OnSelectGuildMemberList( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, DWORD dwGuildIndex, DWORD dwGuildJoinUser )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_guild_member_get_list %d", dwGuildIndex );

	const int queryId = 101;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwGuildIndex );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				// TABLE INDEX
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				// USER INDEX
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				// USER LEVEL
	v_VT.push_back( GetValueType( vChar, ID_NUM_PLUS_ONE ) );			// USER NICK
	v_VT.push_back( GetValueType( vChar, GUILD_POS_NUM_PLUS_ONE ) );	// USER POSITION
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				// USER CAMP POINT
	
	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwGuildIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwGuildJoinUser, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_GUILD_MEMBER_LIST_GET, 
		_SELECTEX1DB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectGuildMemberList Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectGuildMemberListEx( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, DWORD dwGuildIndex, DWORD dwGuildJoinUser, bool bMemberUpdateCheck )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_guild_member_get_list2 %d", dwGuildIndex );

	const int queryId = 102;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwGuildIndex );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				// TABLE INDEX
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				// USER INDEX
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				// USER LEVEL
	v_VT.push_back( GetValueType( vChar, ID_NUM_PLUS_ONE ) );			// USER NICK
	v_VT.push_back( GetValueType( vChar, GUILD_POS_NUM_PLUS_ONE ) );	// USER POSITION
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				// USER CAMP POINT

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwGuildIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwGuildJoinUser, sizeof(DWORD) );
	query_data.SetReturnData( &bMemberUpdateCheck, sizeof(bool) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_GUILD_MEMBER_LIST_EX_GET, 
		_SELECTEX1DB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectGuildMemberListEx Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectGuildMarkBlockInfo( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szDeveloperID, DWORD dwGuildIndex, DWORD dwGuildMark )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_guild_member_get_list3 %d", dwGuildIndex );

	const int queryId = 103;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwGuildIndex );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				// USER INDEX

	CQueryData query_data;
	query_data.SetReturnData( szDeveloperID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwGuildIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwGuildMark, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_GUILD_MARK_BLOCK_INFO, 
		_SELECTEX1DB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectGuildMarkBlockInfo Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateGuildTitle( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwGuildIndex, const ioHashString &szGuildTitle )
{
	std::string szResultTitle;
	Help::GetSafeTextWriteDB( szResultTitle, szGuildTitle );        //DB에 들어가서는 안될 문자를 변경한다

	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_guild_intro_save %d, '%s'", dwGuildIndex, szResultTitle.c_str() );

	const int queryId = 104;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwGuildIndex );
	v_FT.Write( szResultTitle.c_str(), szResultTitle.length(), TRUE );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_GUILD_TITLE_UPD, 
		_UPDATEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdateGuildTitle Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectGuildNameChange( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, DWORD dwGuildIndex, const ioHashString &szGuildName, DWORD dwTableIndex, int iFieldNum )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_guild_change_guildname %d, '%s'", dwGuildIndex, szGuildName.c_str() );

	const int queryId = 105;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwGuildIndex );
	v_FT.Write( szGuildName.c_str(), szGuildName.Length(), TRUE );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		// RESULT

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwGuildIndex, sizeof(DWORD) );
	query_data.SetReturnData( szGuildName.c_str(), GUILD_NAME_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwTableIndex, sizeof(DWORD) );
	query_data.SetReturnData( &iFieldNum, sizeof(int) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_GUILD_NAME_CHANGE_GET, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectGuildNameChange Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectGuildEntryApp( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, DWORD dwGuildIndex, DWORD dwMasterIndex, DWORD dwSecondMasterIndex )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_guild_join_in %d, %d", dwUserIndex, dwGuildIndex );

	const int queryId = 106;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( dwGuildIndex );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				// 결과

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwGuildIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwMasterIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwSecondMasterIndex, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_GUILD_ENTRY_APP_GET, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectGuildEntryApp Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectGuildEntryAppMasterGet( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, DWORD dwGuildIndex )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_guild_member_get_masters %d", dwGuildIndex );

	const int queryId = 107;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwGuildIndex );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				// 길드장/부길드장 인덱스

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwGuildIndex, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_GUILD_ENTRY_APP_MASTER_GET, 
		_SELECTEX1DB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectGuildEntryAppMasterGet Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnDeleteGuildEntryCancel( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, DWORD dwGuildIndex )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_guild_join_self_delete %d, %d", dwUserIndex, dwGuildIndex );

	const int queryId = 108;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( dwGuildIndex );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_GUILD_ENTRY_CANCEL_DEL, 
		_DELETEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnDeleteGuildEntryCancel Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectGuildEntryAgree( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, DWORD dwEntryUserIndex, DWORD dwGuildIndex )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_guild_join_app %d, %d", dwEntryUserIndex, dwGuildIndex );

	const int queryId = 109;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwEntryUserIndex );
	v_FT.Write( dwGuildIndex );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				// 결과

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwGuildIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwEntryUserIndex, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_GUILD_ENTRY_AGREE_GET, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectGuildEntryAgree Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnDeleteGuildEntryRefuse( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwTableIndex )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_guild_join_delete %d", dwTableIndex );

	const int queryId = 110;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwTableIndex );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_GUILD_ENTRY_REFUSE_DEL, 
		_DELETEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnDeleteGuildEntryRefuse Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectGuildEntryAgreeUserGuildInfo( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_guild_login_get_data %d", dwUserIndex );

	const int queryId = 111;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				// GUILD INDEX
	v_VT.push_back( GetValueType( vChar, GUILD_NAME_NUM_PLUS_ONE ) );	// GUILD NAME
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				// GUILD MARK
	v_VT.push_back( GetValueType( vChar, GUILD_POS_NUM_PLUS_ONE ) );	// GUILD POSITION
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				// GUILD EVENT
	v_VT.push_back( GetValueType( vTimeStamp, sizeof(DBTIMESTAMP) ) );	// 출석 보상 받은 날
	v_VT.push_back( GetValueType( vTimeStamp, sizeof(DBTIMESTAMP) ) );	// 길드 랭크 보상 받은 날
	v_VT.push_back( GetValueType( vTimeStamp, sizeof(DBTIMESTAMP) ) );	// 길드 가입날
	v_VT.push_back( GetValueType( vChar, sizeof(char) ) );	// 길드 룸 open 여부.

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_GUILD_ENTRY_AGREE_USER_INFO_GET, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectGuildEntryAgreeUserGuildInfo Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnDeleteGuildLeaveUser( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, DWORD dwGuildIndex )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_guild_member_self_out %d, %d", dwUserIndex, dwGuildIndex );

	const int queryId = 112;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( dwGuildIndex );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_GUILD_LEAVE_USER_DEL, 
		_DELETEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnDeleteGuildLeaveUser Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateGuildMasterChange( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, DWORD dwTargetIndex, DWORD dwGuildIndex )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_guild_change_master %d, %d, %d", dwUserIndex, dwTargetIndex, dwGuildIndex );

	const int queryId = 113;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( dwTargetIndex );
	v_FT.Write( dwGuildIndex );

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof( DWORD ) );
	query_data.SetReturnData( &dwTargetIndex, sizeof( DWORD ) );
	query_data.SetReturnData( &dwGuildIndex, sizeof( DWORD ) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_GUILD_MASTER_CHANGE_UPD, 
		_UPDATEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdateGuildMasterChange Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateGuildPositionChange( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, DWORD dwTargetIndex, DWORD dwGuildIndex, const ioHashString &szTargetID, const ioHashString &szPosition )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_guild_position_save %d, %d, '%s'", dwTargetIndex, dwGuildIndex, szPosition.c_str() );

	const int queryId = 114;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwTargetIndex );
	v_FT.Write( dwGuildIndex );
	v_FT.Write( szPosition.c_str(), szPosition.Length(), TRUE );

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof( DWORD ) );
	query_data.SetReturnData( &dwTargetIndex, sizeof( DWORD ) );
	query_data.SetReturnData( &dwGuildIndex, sizeof( DWORD ) );
	query_data.SetReturnData( szTargetID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( szPosition.c_str(), GUILD_POS_NUM_PLUS_ONE );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_GUILD_POSITION_CHANGE_UPD, 
		_UPDATEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdateGuildPositionChange Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnDeleteGuildKickOut( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwTableIndex, DWORD dwGuildIndex )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_guild_member_out %d, %d", dwTableIndex, dwGuildIndex );

	const int queryId = 115;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwTableIndex );
	v_FT.Write( dwGuildIndex );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_GUILD_KICK_OUT_DEL, 
		_DELETEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnDeleteGuildKickOut Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectGuildSimpleData( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, const ioHashString &szGuildUserID )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_guild_user_nick_get_data '%s'", szGuildUserID.c_str() );
	
	const int queryId = 116;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( szGuildUserID.c_str(), szGuildUserID.Length(), TRUE );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				// GUILD INDEX
	
	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( szGuildUserID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_GUILD_SIMPLE_DATA_GET, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSeleteGuildSimpleData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateGuildMarkChange( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwGuildIndex, DWORD dwGuildMark )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_guild_mark_save %d, %d", dwGuildIndex, dwGuildMark );

	const int queryId = 117;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwGuildIndex );
	v_FT.Write( dwGuildMark);

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_GUILD_MARK_CHANGE_UPD, 
		_UPDATEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdateGuildMarkChange Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnInsertGuildMarkChangeKeyValue( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, DWORD dwGuildIndex, __int64 iMyMoney, int iChangeMoney, DWORD dwGuildMark, DWORD dwNewGuildMark )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_guild_mark_log_add %d, %d, %I64d, %d, %d, %d", dwGuildIndex, dwUserIndex, iMyMoney, iChangeMoney, dwGuildMark, dwNewGuildMark );

	const int queryId = 118;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwGuildIndex );
	v_FT.Write( dwUserIndex );
	v_FT.Write( iMyMoney );
	v_FT.Write( iChangeMoney );
	v_FT.Write( dwGuildMark );
	v_FT.Write( dwNewGuildMark );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_GUILD_MARK_CHANGE_KEY_VALUE_SET, 
		_INSERTDB, 
		queryId,
		v_FT,
		v_VT );
	
	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnInsertGuildMarkChangeKeyValue Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
	OnSelectGuildMarkChangeKeyValue( dwAgentID, dwThreadID, dwUserIndex );
}

void DBClient::OnSelectGuildMarkChangeKeyValue( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_guild_mark_log_get_self_idx %d", dwUserIndex );

	const int queryId = 119;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				// KEY INDEX

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_GUILD_MARK_CHANGE_KEY_VALUE_GET, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectGuildMarkChangeKeyValue Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateGuildMarkChangeKeyValue( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwKeyIndex )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_guild_mark_log_delete %d", dwKeyIndex );

	const int queryId = 120;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwKeyIndex );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_GUILD_MARK_CHANGE_KEY_VALUE_UPD, 
		_UPDATEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdateGuildMarkChangeKeyValue Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectGuildUserLadderPointADD( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, DWORD dwGuildIndex, bool bPlus )
{
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "OnSelectGuildUserLadderPointADD 사용하지 않는 쿼리 실행됨!!" );

	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_get_ladderpoint %d", dwUserIndex );

	const int queryId = 121;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				// Ladder Point

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwGuildIndex, sizeof(DWORD) );
	query_data.SetReturnData( &bPlus, sizeof(bool) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_GUILD_USER_LADDER_POINT_ADD_GET, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectGuildUserLadderPointADD Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectCampSeasonBonus( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_region_get_compen %d", dwUserIndex );

	const int queryId = 122;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );       // 보너스 인덱스
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );       // 블루진영 포인트
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );       // 블루진영 보너스 포인트
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );       // 블루진영 유효 유저 수
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );       // 레드진영 포인트
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );       // 레드진영 보너스 포인트
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );       // 레드진영 유효 유저 수
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );       // 내 진영 타입
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );       // 내 진영 포인트
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );       // 내 진영 랭크

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_CAMP_SEASON_BONUS_GET, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectCampSeasonBonus Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnDeleteCampSeasonBonus( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwBonusIndex, DWORD dwUserIndex, const ioHashString &rkUserNick, int iSeasonBonus )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_region_compen_delete %d, %d, '%s', %d", dwBonusIndex, dwUserIndex, rkUserNick.c_str(), iSeasonBonus );

	const int queryId = 123;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwBonusIndex );
	v_FT.Write( dwUserIndex );
	v_FT.Write( rkUserNick.c_str(), rkUserNick.Length(), TRUE );
	v_FT.Write( iSeasonBonus );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_CAMP_SEASON_BONUS_DEL, 
		_DELETEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnDeleteCampSeasonBonus Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnInsertPresentData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szSendName, const ioHashString &szRecvName, short iPresentType, 
								    int iPresentValue1, int iPresentValue2, int iPresentValue3, int iPresentValue4, short iPresentMent, CTime &rkLimitTime, short iPresentState )
{
	//char szLimitDate[MAX_PATH] = "";
	//sprintf_s( szLimitDate,"\'%d-%d-%d %d:%d:%d\'", rkLimitTime.GetYear(), rkLimitTime.GetMonth(), rkLimitTime.GetDay(), rkLimitTime.GetHour(), 
	//											 rkLimitTime.GetMinute(), rkLimitTime.GetSecond() );
	

	SYSTEMTIME sysTime;

	if( iPresentType == PRESENT_BONUS_CASH )
	{
		
		CTimeSpan cPresentGapTime( 7, 0, 0, 0 );
		CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
		CTime tempTime = g_ItemInitControl.CheckPresentFixedLimitDate( iPresentType, iPresentValue1, kPresentTime );
		tempTime.GetAsSystemTime( sysTime );

	}
	else
	{	CTime tempTime = g_ItemInitControl.CheckPresentFixedLimitDate( iPresentType, iPresentValue1, rkLimitTime );
		tempTime.GetAsSystemTime( sysTime );
	}

	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_present_add '%s', '%s', %d, %d, %d, %d, %d, %d, %s, %d", 
	//	szSendName.c_str(), szRecvName.c_str(), 
	//	iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, iPresentMent, szLimitDate, iPresentState );
	
	const int queryId = 124;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( szSendName.c_str(), szSendName.Length(), TRUE );
	v_FT.Write( szRecvName.c_str(), szRecvName.Length(), TRUE );
	v_FT.Write( iPresentType );
	v_FT.Write( iPresentValue1 );
	v_FT.Write( iPresentValue2 );
	v_FT.Write( iPresentValue3 );
	v_FT.Write( iPresentValue4 );
	v_FT.Write( iPresentMent );
	v_FT.Write( (uint8*)&sysTime, sizeof(sysTime), TRUE );
	v_FT.Write( iPresentState );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY,
		DBAGENT_PRESENT_DATA_SET, 
		_INSERTDB, 
		queryId, 
		v_FT, 
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnInsertPresentData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnInsertPresentDataLog( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szSendName, const ioHashString &szRecvName, short iPresentType, 
								    int iPresentValue1, int iPresentValue2, int iPresentValue3, int iPresentValue4, short iPresentMent, CTime &rkLimitTime, short iPresentState )
{
	//char szLimitDate[MAX_PATH] = "";
	//sprintf_s( szLimitDate,"\'%d-%d-%d %d:%d:%d\'", rkLimitTime.GetYear(), rkLimitTime.GetMonth(), rkLimitTime.GetDay(), rkLimitTime.GetHour(), 
	//											 rkLimitTime.GetMinute(), rkLimitTime.GetSecond() );

	SYSTEMTIME sysTime;

	if( iPresentType == PRESENT_BONUS_CASH )
	{

		CTimeSpan cPresentGapTime( 7, 0, 0, 0 );
		CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
		CTime tempTime = g_ItemInitControl.CheckPresentFixedLimitDate( iPresentType, iPresentValue1, kPresentTime );
		tempTime.GetAsSystemTime( sysTime );

	}
	else
	{	
		CTime tempTime = g_ItemInitControl.CheckPresentFixedLimitDate( iPresentType, iPresentValue1, rkLimitTime );
		tempTime.GetAsSystemTime( sysTime );
	}
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_present_add '%s', '%s', %d, %d, %d, %d, %d, %d, %s, %d", 
	//	szSendName.c_str(), szRecvName.c_str(), 
	//	iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, iPresentMent, szLimitDate, iPresentState );
	
	const int queryId = 2056;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( szSendName.c_str(), szSendName.Length(), TRUE );
	v_FT.Write( szRecvName.c_str(), szRecvName.Length(), TRUE );
	v_FT.Write( iPresentType );
	v_FT.Write( iPresentValue1 );
	v_FT.Write( iPresentValue2 );
	v_FT.Write( iPresentValue3 );
	v_FT.Write( iPresentValue4 );
	v_FT.Write( iPresentMent );
	v_FT.Write( (uint8*)&sysTime, sizeof(sysTime), TRUE );
	v_FT.Write( iPresentState );
	v_VT.push_back( GetValueType( vLONG, sizeof( LONG ) ) ); // UserIndex를 받는다

	CQueryData query_data;
	query_data.SetReturnData( &iPresentType, sizeof(short) );
	query_data.SetReturnData( &iPresentValue1, sizeof(int) );
	query_data.SetReturnData( &iPresentValue2, sizeof(int) );
	query_data.SetReturnData( &iPresentValue3, sizeof(int) );
	query_data.SetReturnData( &iPresentValue4, sizeof(int) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK,
		DBAGENT_PRESENT_DATA_LOG_SET, 
		_INSERTDB, 
		queryId, 
		v_FT, 
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnInsertPresentData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnInsertPresentDataByUserIndex( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwSendUserIndex, DWORD dwRecvUserIndex, short iPresentType, int iPresentValue1, int iPresentValue2, int iPresentValue3, int iPresentValue4, short iPresentMent, CTime &rkLimitTime, short iPresentState )
{
	//char szLimitDate[MAX_PATH] = "";
	//sprintf_s( szLimitDate,"\'%d-%d-%d %d:%d:%d\'", rkLimitTime.GetYear(), rkLimitTime.GetMonth(), rkLimitTime.GetDay(), rkLimitTime.GetHour(), 
	//	     rkLimitTime.GetMinute(), rkLimitTime.GetSecond() );
	
	SYSTEMTIME sysTime;

	if( iPresentType == PRESENT_BONUS_CASH )
	{

		CTimeSpan cPresentGapTime( 7, 0, 0, 0 );
		CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
		CTime tempTime = g_ItemInitControl.CheckPresentFixedLimitDate( iPresentType, iPresentValue1, kPresentTime );
		tempTime.GetAsSystemTime( sysTime );

	}
	else
	{	
		CTime tempTime = g_ItemInitControl.CheckPresentFixedLimitDate( iPresentType, iPresentValue1, rkLimitTime );
		tempTime.GetAsSystemTime( sysTime );
	}



	__int64 iTempValue	= 0;
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_present_add_idx  %d, %d, %d, %d, %d, %d, %d, %d, %s, %d", 
	//		dwSendUserIndex, dwRecvUserIndex, iPresentType,
	//	    iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, iPresentMent, szLimitDate, iPresentState );

	const int queryId = 125;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwSendUserIndex );
	v_FT.Write( dwRecvUserIndex );
	v_FT.Write( iPresentType );
	v_FT.Write( iPresentValue1 );
	v_FT.Write( iPresentValue2 );
	v_FT.Write( iPresentValue3 );
	v_FT.Write( iPresentValue4 );
	v_FT.Write( iPresentMent );
	v_FT.Write( (uint8*)&sysTime, sizeof(sysTime), TRUE );
	v_FT.Write( iPresentState );
	v_FT.Write( iTempValue );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_PRESENT_DATA_BY_USERINDEX_SET, 
		_INSERTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnInsertPresentDataByUserIndex Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectPresentData( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, DWORD dwPresentIndex, DWORD dwSelectCount )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_present_get_list %d, %d, %d", dwPresentIndex, dwUserIndex, dwSelectCount );

	const int queryId = 126;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwPresentIndex );
	v_FT.Write( dwUserIndex );
	v_FT.Write( dwSelectCount );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				 //선물 인덱스
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				 //선물 보낸 유저인덱스
	v_VT.push_back( GetValueType( vChar, ID_NUM_PLUS_ONE ) );            //선물 보낸 유저 닉네임
	v_VT.push_back( GetValueType( vSHORT, sizeof(short) ) );             //선물 타입
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );               //선물 변수1
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );               //선물 변수2
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );               //선물 변수3
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );               //선물 변수4
	v_VT.push_back( GetValueType( vSHORT, sizeof(short) ) );             //선물 멘트 타입
	v_VT.push_back( GetValueType( vSHORT, sizeof(short) ) );             //선물 알림 타입
	v_VT.push_back( GetValueType( vTimeStamp, sizeof(DBTIMESTAMP) ) );   //선물 만료 시간

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_PRESENT_DATA_GET, 
		_SELECTEX1DB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectPresentData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdatePresentData( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwPresentIndex, short iPresentState )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_present_save %d, %d", dwPresentIndex, iPresentState );

	const int queryId = 127;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwPresentIndex );
	v_FT.Write( iPresentState );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_PRESENT_DATA_UPD, 
		_UPDATEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdatePresentData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnAllDeletePresentData( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_present_delete_all %d", dwUserIndex );

	const int queryId = 128;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_PRESENT_ALL_DELETE_DEL, 
		_DELETEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnAllDeletePresentData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnDeletePresent( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwPresentIndex )
{
	const int queryId = 2093;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwPresentIndex );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGNET_PRESENT_DEL, 
		_DELETEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnDeletePresent Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectUserIndexAndPresentCnt( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, DWORD dwUserIndex, const ioHashString &rszRecvPublicID, short iPresentType, int iBuyValue1, int iBuyValue2 )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_present_count '%s'", rszRecvPublicID.c_str() );

	const int queryId = 129;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( rszRecvPublicID.c_str(), rszRecvPublicID.Length(), TRUE );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				 //선물 받을 유저인덱스
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );               //선물 받을 유저의 현재 선물 갯수
	v_VT.push_back( GetValueType( vChar, ID_NUM_PLUS_ONE ) );            //선물 받을 유저의 PrivateID

	CQueryData query_data;
	query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( &iPresentType, sizeof(short) );
	query_data.SetReturnData( &iBuyValue1, sizeof(int) );
	query_data.SetReturnData( &iBuyValue2, sizeof(int) );
	query_data.SetReturnData( rszRecvPublicID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_USER_INDEX_AND_PRESENT_CNT_GET, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectUserIndexAndPresentCnt Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnPresentInsertByPrivateID( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szSendName, const ioHashString &szRecvName, short iPresentType, int iPresentValue1, int iPresentValue2, int iPresentValue3, int iPresentValue4, short iPresentMent, CTime &rkLimitTime, short iPresentState, int iIsPublicIDState )
{
	//
	CTime tempTime = g_ItemInitControl.CheckPresentFixedLimitDate( iPresentType, iPresentValue1, rkLimitTime );

	SYSTEMTIME sysTime;
	tempTime.GetAsSystemTime( sysTime );

	const int queryId = 2094;

	cSerialize v_FT;
	vVALUETYPE v_VT;
	BYTE byIDType = iIsPublicIDState;

	v_FT.Write( szSendName.c_str(), szSendName.Length(), TRUE );
	v_FT.Write( szRecvName.c_str(), szRecvName.Length(), TRUE );
	v_FT.Write( byIDType );
	v_FT.Write( iPresentType );
	v_FT.Write( iPresentValue1 );
	v_FT.Write( iPresentValue2 );
	v_FT.Write( iPresentValue3 );
	v_FT.Write( iPresentValue4 );
	v_FT.Write( iPresentMent );
	v_FT.Write( (uint8*)&sysTime, sizeof(sysTime), TRUE );
	v_FT.Write( iPresentState );

	v_VT.push_back( GetValueType( vLONG, sizeof( LONG ) ) ); // UserIndex를 받는다

	CQueryData query_data;
	query_data.SetReturnData( &iPresentType, sizeof(short) );
	query_data.SetReturnData( &iPresentValue1, sizeof(int) );
	query_data.SetReturnData( &iPresentValue2, sizeof(int) );
	query_data.SetReturnData( &iPresentValue3, sizeof(int) );
	query_data.SetReturnData( &iPresentValue4, sizeof(int) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK,
		DBAGENT_PRESENT_ADD_BY_PRIVATE, 
		_INSERTDB, 
		queryId, 
		v_FT, 
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnInsertPresentData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectUserEntry( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, const DWORD dwUserIndex )
{
	//char szQuery[MAX_PATH] = "";
	//sprintf_s( szQuery, "exec game_member_namecheck_get_data %d", dwUserIndex );

	const int queryId = 130;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_VT.push_back( GetValueType(vSHORT,sizeof(short)) ); // db smallint

	CQueryData query_data;
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwUserIndex, sizeof(int) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_USER_ENTRY_GET, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectUserEntry Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectUserExist( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, const ioHashString &szFindPublicID  )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_member_get_count '%s'", szFindPublicID.c_str() );

	const int queryId = 131;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( szFindPublicID.c_str(), szFindPublicID.Length(), TRUE );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) ); // 존재유무 ( 0:없다. / 1이상:존재한다. )

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_USER_EXIST_GET, 
		_SELECTDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectUserExist Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectPublicIDExist( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIdx, const ioHashString &szNewPublicID )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//StringCbPrintf( szQuery, MAX_QUERY_SIZE, "exec game_member_get_userNickname_count '%s'", szNewPublicID.c_str() );

	const int queryId = 132;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( szNewPublicID.c_str(), szNewPublicID.Length(), TRUE );
	v_VT.push_back( GetValueType( vLONG, sizeof( LONG ) ) ); // 존재유무 ( 0 : 없다 / 1이상 : 존재한다. )

	CQueryData query_data;
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwUserIdx, sizeof(int) );
	query_data.SetReturnData( szNewPublicID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_PUBLIC_ID_EXIST_GET, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectPublicIDExist Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdatePublicIDAndEtcItem( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, const ioHashString &rszPublicID, const ioHashString &rszNewPublicID, DWORD dwEtcItemIndex, int iEtcItemFieldCnt, const ioHashString &rszPublicIP )
{
	// 차후에 범용적으로 선언할것
	enum 
	{
		SOLDIER = 1,
		DECO    = 2,
		ETC     = 3,
	};

	//char szQuery[MAX_QUERY_SIZE] = "";
	//StringCbPrintf( szQuery, MAX_QUERY_SIZE, "exec game_member_change_userNickname %d, '%s', '%s', %d, %d, %d, '%d', '%s'", 
	//	dwUserIndex, rszPublicID.c_str(), rszNewPublicID.c_str(), ETC, ioEtcItem::EIT_ETC_CHANGE_ID, dwEtcItemIndex, iEtcItemFieldCnt, rszPublicIP.c_str() );

	char etcItemIndex[64], etcItemFieldCnt[64];
	StringCbPrintf( etcItemIndex, sizeof(etcItemIndex), "%d", dwEtcItemIndex );
	StringCbPrintf( etcItemFieldCnt, sizeof(etcItemFieldCnt), "%d", iEtcItemFieldCnt );

	const int queryId = 133;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( rszPublicID.c_str(), rszPublicID.Length(), TRUE );
	v_FT.Write( rszNewPublicID.c_str(), rszNewPublicID.Length(), TRUE );
	v_FT.Write( (int)ETC );
	v_FT.Write( (int)ioEtcItem::EIT_ETC_CHANGE_ID );
	v_FT.Write( etcItemIndex, strlen(etcItemIndex), TRUE );
	v_FT.Write( etcItemFieldCnt, strlen(etcItemFieldCnt), TRUE );
	v_FT.Write( rszPublicIP.c_str(), rszPublicIP.Length(), TRUE );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_PUBLIC_ID_ETC_ITEM_UPD, 
		_UPDATEDB, 
		queryId, 
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdatePublicIDAndEtcItem Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectChangedPublicID( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, const ioHashString &rszNewPublicID )
{
	char szQuery[MAX_QUERY_SIZE] = "";
	StringCbPrintf( szQuery, MAX_QUERY_SIZE, "exec game_get_nickname %d", dwUserIndex );

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_VT.push_back(GetValueType(vChar,ID_NUM_PLUS_ONE)); // 변경이 완료된 public id

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof( DWORD ) );
	query_data.SetReturnData( rszNewPublicID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_CHANGED_PUBLIC_ID_GET, 
		_SELECTDB, 
		134,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB %s Send Fail! :%d", __FUNCTION__,  GetLastError() );
		return;
	}
}

void DBClient::OnSelectBlockType( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const DWORD dwUserIndex )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_get_limitType %d", dwUserIndex );

	const int queryId = 135;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_VT.push_back(GetValueType(vSHORT,sizeof(short)));           //차단타입
	v_VT.push_back(GetValueType(vTimeStamp,sizeof(DBTIMESTAMP))); //차단시간

	CQueryData query_data;
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwUserIndex, sizeof( DWORD ) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_BLOCK_TYPE_GET, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectBlockType Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectMemberConut( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szPrivateID )
{
	if( szPrivateID.IsEmpty() )	
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Error - OnSelectMemberConut - szID is empty");
		return;
	}

	if( !g_App.IsRightID( szPrivateID.c_str() ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Error - OnSelectMemberConut - szID is Error");
		return;
	}

	//char str_query[MAX_QUERY_SIZE] = "";
	//wsprintf(str_query,"exec game_member_count '%s'", szPrivateID.c_str());

	const int queryId = 136;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( szPrivateID.c_str(), szPrivateID.Length(), TRUE );
	v_VT.push_back( GetValueType( vLONG, sizeof( LONG ) ) ); // userMemberDB count / 1이상이어야 함.

	CQueryData query_data;	
	query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	query_data.SetReturnData( szPrivateID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetData( 
		dwThreadID,
		_RESULT_CHECK,
		DBAGENT_MEMBER_COUNT_GET,
		_SELECTDB,
		136,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectMemberConut Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnInsertMember( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szPrivateID, const ioHashString &szPublicID )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_member_add '%s', '%s'", szPrivateID.c_str(), szPublicID.c_str() );

	const int queryId = 137;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( szPrivateID.c_str(), szPrivateID.Length(), TRUE );
	v_FT.Write( szPublicID.c_str(), szPublicID.Length(), TRUE );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_MEMBER_SET, 
		_INSERTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnInsertMember Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectFirstPublicIDExist( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, const ioHashString &szPublicID, const ioHashString &szUserGUID, const ioHashString &szNewPublicID )
{/*
	char szQuery[MAX_QUERY_SIZE] = "";
	StringCbPrintf( szQuery, MAX_QUERY_SIZE, "exec game_member_get_userNickname_count '%s'", szNewPublicID.c_str() );*/

	const int queryId = 138;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( szNewPublicID.c_str(), szNewPublicID.Length(), TRUE );
	v_VT.push_back( GetValueType( vLONG, sizeof( LONG ) ) ); // 존재유무 ( 0 : 없다 / 1이상 : 존재한다. )

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof( DWORD ) );
	query_data.SetReturnData( szPublicID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	query_data.SetReturnData( szNewPublicID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_FIRST_PUBLIC_ID_EXIST_GET, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectFirstPublicIDExist Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateFirstPublicID( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, const ioHashString &szNewPublicID )
{
	/*char szQuery[MAX_QUERY_SIZE] = "";
	StringCbPrintf( szQuery, MAX_QUERY_SIZE, "exec game_create_nickname %d, '%s'", dwUserIndex, szNewPublicID.c_str() );*/

	const int queryId = 139;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( szNewPublicID.c_str(), szNewPublicID.Length(), TRUE );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY,
		DBAGENT_FIRST_PUBLIC_ID_UPD, 
		_UPDATEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdateFirstPublicID Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectChangedFirstPublicID( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, const ioHashString &szPublicID, const ioHashString &szUserGUID, const ioHashString &szNewPublicID )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//StringCbPrintf( szQuery, MAX_QUERY_SIZE, "exec game_get_nickname %d", dwUserIndex );

	const int queryId = 140;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_VT.push_back(GetValueType(vChar,ID_NUM_PLUS_ONE)); // 변경이 완료된 public id

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof( DWORD ) );
	query_data.SetReturnData( szPublicID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	query_data.SetReturnData( szNewPublicID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_FIRST_PUBLIC_ID_CHANGED_ID_GET, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectChangedFirstPublicID Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateControlKeys( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, const ControlKeys &rkControlKeys )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//StringCbPrintf( szQuery, MAX_QUERY_SIZE, "exec game_config_keyboard_save %d, '%s'", dwUserIndex, rkControlKeys.m_szControlKeys );

	const int queryId = 141;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( rkControlKeys.m_szControlKeys, strlen(rkControlKeys.m_szControlKeys), TRUE );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_CONTROL_KEYS_UPD, 
		_UPDATEDB, 
		141,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdateControlKeys Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnInsertMedalItemData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx, cSerialize& v_FT, int iLogType, int iLimitTime )
{
	if( szID.IsEmpty() ) return;

	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_item_medal_add %d%s", dwUserIdx, szQueryAgument );

	const int queryId = 142;

	vVALUETYPE v_VT;

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_MEDALITEM_DATA_SET, 
		_INSERTDB, 
		queryId, 
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnInsertMedalItemData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
	OnSelectMedalItemIndex( dwAgentID, dwThreadID, szUserGUID, szID, dwUserIdx, iLogType, iLimitTime );
}

void DBClient::OnSelectMedalItemIndex( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx, int iLogType, int iLimitTime )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_item_medal_get_self_index %d", dwUserIdx );

	const int queryId = 143;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//Item INDEX

	CQueryData query_data;
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	//query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwUserIdx, sizeof(int) );
	query_data.SetReturnData( &iLogType, sizeof( int ) );
	query_data.SetReturnData( &iLimitTime, sizeof( int ) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_MEDALITEM_DATA_CREATE_INDEX, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectMedalItemIndex Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateMedalItemData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szID, DWORD dwInvenIdx, cSerialize& v_FT )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_item_medal_save %d%s", dwInvenIdx, szQueryAgument );

	const int queryId = 144;

	vVALUETYPE v_VT;

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_MEDALITEM_DATA_UPD, 
		_UPDATEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdateMedalItemData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnInsertExMedalSlotData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx, cSerialize& v_FT, int iLogType )
{
	if( szID.IsEmpty() ) return;

	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_item_medal_extend_add %d%s", dwUserIdx, szQueryAgument );

	const int queryId = 145;

	vVALUETYPE v_VT;

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_EXPAND_MEDAL_SLOT_DATA_SET, 
		_INSERTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		return;
	}
	OnSelectExMedalSlotIndex( dwAgentID, dwThreadID, szUserGUID, szID, dwUserIdx, iLogType );
}

void DBClient::OnSelectExMedalSlotIndex( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx, int iLogType )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_item_medal_extend_get_self_index %d", dwUserIdx );

	const int queryId = 146;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//Item INDEX

	CQueryData query_data;
// 	query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
// 	query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwUserIdx, sizeof(int) );
	query_data.SetReturnData( &iLogType, sizeof( int ) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_EXPAND_MEDAL_SLOT_DATA_CREATE_INDEX, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectExMedalSlotIndex Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateExMedalSlotData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szID, DWORD dwInvenIdx, cSerialize& v_FT )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_item_medal_extend_save %d%s", dwInvenIdx, szQueryAgument );

	const int queryId = 147;

	vVALUETYPE v_VT;

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_EXPAND_MEDAL_SLOT_DATA_UPD, 
		_UPDATEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdateExMedalSlotData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectHeroData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_ranking_hero_get_data %d", dwUserIdx );

	const int queryId = 148;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//유저 칭호
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//유저 랭킹

	for(int i = 0;i < HERO_SEASON_RANK_MAX;i++)
	{
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );			//시즌 랭킹
	}

	CQueryData query_data;
	query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwUserIdx, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_HERO_DATA_GET, 
		_SELECTDB,	
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectHeroData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectHeroTop100Data( int iMinNumber, int iMaxNumber )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_ranking_hero_topN %d, %d", iMinNumber, iMaxNumber );

	const int queryId = 149;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( iMinNumber );
	v_FT.Write( iMaxNumber );
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));     //유저 인덱스
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));     //유저 레벨
	v_VT.push_back(GetValueType(vChar,ID_NUM_PLUS_ONE));  //유저 아이디
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));     //유저 칭호
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));     //유저 승
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));     //유저 패
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));     //유저 진영
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));     //유저 영웅전 경험치

	CQueryData query_data;
	query_data.SetReturnData( &iMinNumber, sizeof(int) );
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_CHECK, 
		DBAGENT_HERO_TOP100_GET, 
		_SELECTEX1DB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( GameServerAgentID(), kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectHeroTop100Data Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectItemCustomUniqueIndex( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_item_extra_get_customidx %d", dwUserIdx );

	const int queryId = 150;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//고유번호

	CQueryData query_data;
	query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwUserIdx, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_ITEM_CUSTOM_UNIQUE_INDEX, 
		_SELECTDB,	
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectItemCustomUniqueIndex Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

// 임시 : 거래소
void DBClient::OnSelectCreateTrade( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, const ioHashString &rkUserNick, const char *szPublicIP, DWORD dwItemType, 
								    DWORD dwItemMagicCode, DWORD dwItemValue, __int64 iItemPrice, DWORD dwRegisterPeriod, 
									DWORD dwItemMaleCustom, DWORD dwItemFemaleCustom )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_trade_add %d, %d, %d, %d, %I64d, %d, %d, '%s', %d", dwUserIndex, dwItemType, dwItemMagicCode, dwItemValue, 
	//																			     iItemPrice, dwItemMaleCustom, dwItemFemaleCustom, szPublicIP, dwRegisterPeriod );

	const int queryId = 151;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( dwItemType );
	v_FT.Write( dwItemMagicCode );
	v_FT.Write( dwItemValue );
	v_FT.Write( iItemPrice );
	v_FT.Write( dwItemMaleCustom );
	v_FT.Write( dwItemFemaleCustom );
	v_FT.Write( szPublicIP, strlen(szPublicIP), TRUE );
	v_FT.Write( dwRegisterPeriod );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_TRADE_CREATE, 
		_INSERTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectCreateTrade Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}

	OnSelectCreateTradeIndex( dwAgentID, dwThreadID, dwUserIndex, rkUserNick, szPublicIP, dwItemType, dwItemMagicCode, dwItemValue, iItemPrice, dwRegisterPeriod, dwItemMaleCustom, dwItemFemaleCustom );
}

void DBClient::OnSelectCreateTradeIndex( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, const ioHashString &rkUserNick, const char *szPublicIP,
										 DWORD dwItemType, DWORD dwItemMagicCode, DWORD dwItemValue, __int64 iItemPrice,
										 DWORD dwRegisterPeriod, DWORD dwItemMaleCustom, DWORD dwItemFemaleCustom  )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_trade_get_self_index %d", dwUserIndex );

	CTime curTime = CTime::GetCurrentTime();
	DBTIMESTAMP dts;
	curTime.GetAsDBTIMESTAMP( dts );

	const int queryId = 152;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_VT.push_back( GetValueType( vLONG, sizeof(DWORD) ) );		// tradeindex

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );					// 등록자 인덱스
	query_data.SetReturnData( rkUserNick.c_str(), ID_NUM_PLUS_ONE );			// 등록자 닉네임
	query_data.SetReturnData( &dwItemType, sizeof(DWORD) );						// 아이템 타입
	query_data.SetReturnData( &dwItemMagicCode, sizeof(DWORD) );				// 코드
	query_data.SetReturnData( &dwItemValue, sizeof(DWORD) );					// value
	query_data.SetReturnData( &dwItemMaleCustom, sizeof(DWORD) );				// 남자 장비 스킨
	query_data.SetReturnData( &dwItemFemaleCustom, sizeof(DWORD) );				// 여자 장비 스킨
	query_data.SetReturnData( &iItemPrice, sizeof(__int64) );					// 거래가격
	query_data.SetReturnData( &dwRegisterPeriod, sizeof(DWORD) );				// 등록기간
	query_data.SetReturnData( &dts, sizeof(DBTIMESTAMP) );						// 등록시간
	query_data.SetReturnData( szPublicIP, IP_NUM_PLUS_ONE );					// 등록 ip
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_TRADE_CREATE_INDEX, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectCreateTradeIndex Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnTradeItemComplete( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwBuyUserIndex, DWORD dwRegisterUserIndex, const ioHashString &rkUserNick, DWORD dwTradeIndex, 
								    DWORD dwItemType, DWORD dwItemMagicCode, DWORD dwItemValue, DWORD dwItemMaleCustom, DWORD dwItemFemaleCustom, __int64 iItemPrice )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_trade_delete %d", dwTradeIndex );

	const int queryId = 153;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwTradeIndex );

	CQueryData query_data;
	query_data.SetReturnData( &dwBuyUserIndex, sizeof(DWORD) );					// 구매자 인덱스
	query_data.SetReturnData( &dwTradeIndex, sizeof(DWORD) );					// 인덱스
	query_data.SetReturnData( &dwRegisterUserIndex, sizeof(DWORD) );			// 등록자 인덱스
	query_data.SetReturnData( rkUserNick.c_str(), ID_NUM_PLUS_ONE );			// 등록자 닉네임
	query_data.SetReturnData( &dwItemType, sizeof(DWORD) );						// 아이템 타입
	query_data.SetReturnData( &dwItemMagicCode, sizeof(DWORD) );				// 코드
	query_data.SetReturnData( &dwItemValue, sizeof(DWORD) );					// value
	query_data.SetReturnData( &dwItemMaleCustom, sizeof(DWORD) );				// 남자 장비 스킨
	query_data.SetReturnData( &dwItemFemaleCustom, sizeof(DWORD) );				// 여자 장비 스킨
	query_data.SetReturnData( &iItemPrice, sizeof(__int64) );					// 거래가격
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_TRADE_COMPLETE, 
		_DELETEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnTradeItemComplete Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnTradeItemCancel( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwRegisterUserIndex, const ioHashString &rkUserNick, DWORD dwTradeIndex, DWORD dwItemType, 
								  DWORD dwItemMagicCode, DWORD dwItemValue, DWORD dwItemMaleCustom, DWORD dwItemFemaleCustom, __int64 iItemPrice )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_trade_delete %d", dwTradeIndex );

	const int queryId = 153;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwTradeIndex );

	CQueryData query_data;
	query_data.SetReturnData( &dwTradeIndex, sizeof(DWORD) );					// 인덱스
	query_data.SetReturnData( &dwRegisterUserIndex, sizeof(DWORD) );			// 등록자 인덱스
	query_data.SetReturnData( rkUserNick.c_str(), ID_NUM_PLUS_ONE );			// 등록자 닉네임
	query_data.SetReturnData( &dwItemType, sizeof(DWORD) );						// 아이템 타입
	query_data.SetReturnData( &dwItemMagicCode, sizeof(DWORD) );				// 코드
	query_data.SetReturnData( &dwItemValue, sizeof(DWORD) );					// value
	query_data.SetReturnData( &dwItemMaleCustom, sizeof(DWORD) );				// 남자 장비 스킨
	query_data.SetReturnData( &dwItemFemaleCustom, sizeof(DWORD) );				// 여자 장비 스킨
	query_data.SetReturnData( &iItemPrice, sizeof(__int64) );					// 거래가격
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_TRADE_CANCEL, 
		_DELETEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnTradeItemCancel Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnInsertSoldierPriceCollectedBuyTime( int iClassType, __int64 iClassTime )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_log_class_buytime %d, %I64d", iClassType, iClassTime );

	const int queryId = 155;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( iClassType );
	v_FT.Write( iClassTime );

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		DBAGENT_SOLDIER_PRICE_COLLECTED_BUYTIME, 
		_INSERTDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( GameServerAgentID(), kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnInsertSoldierPriceCollectedBuyTime Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnInsertSoldierPriceCollectedPlayTime( int iGradeLevel, __int64 iPlayTime )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_log_level_playtime %d, %I64d", iGradeLevel, iPlayTime );

	const int queryId = 156;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( iGradeLevel );
	v_FT.Write( iPlayTime );

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		DBAGENT_SOLDIER_PRICE_COLLECTED_PLAYTIME, 
		_INSERTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( GameServerAgentID(), kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnInsertSoldierPriceCollectedPlayTime Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnDBServerTest()
{
	ioINILoader kLoader( "config/dba_test_ini.ini" );
	kLoader.SetTitle( "common" );
	int iMaxIndex = kLoader.LoadInt( "max_index", MAX_BUFFER );
	if( iMaxIndex < GetNodeSize() )
		return;

	int i = 0;
	DWORDVec vIndexList;
	for(i = 0;i < iMaxIndex;i++)
	{
		char szKey[MAX_PATH] = "";
		sprintf_s( szKey, "user_index%d", i + 1 );
		DWORD dwIndex = kLoader.LoadInt( szKey, 0 );
		if( dwIndex == 0 ) continue;

		vIndexList.push_back( dwIndex );
	}

	if( (int)vIndexList.size() < GetNodeSize() )
		return;

	io_random_shuffle( vIndexList.begin(), vIndexList.end() );
	int iDBASendCnt = ( (int)vIndexList.size() ) / GetNodeSize();
	for(i = 0;i < GetNodeSize();i++)
	{
		for(int k = 0;k < iDBASendCnt;k++)
		{
			if( vIndexList.empty() ) break;

			DWORD dwUserIndex = vIndexList[0];
			char szTempGUID[USER_GUID_NUM_PLUS_ONE]="";
			Help::GetGUID( szTempGUID, sizeof(szTempGUID) );

			char szTempID[ID_NUM_PLUS_ONE] = "";
			sprintf_s( szTempID, "-T%d", i );
			OnLoginSelectControlKeys( i, i, szTempGUID, szTempID, dwUserIndex );
			OnLoginSelectAllAwardData( i, i, szTempGUID, szTempID, dwUserIndex );
			OnLoginSelectAwardExpert( i, i, szTempGUID, szTempID, dwUserIndex );
			OnLoginSelectAllClassExpert( i, i, szTempGUID, szTempID, dwUserIndex );
			OnLoginSelectUserRecord( i, i, szTempGUID, szTempID, dwUserIndex );
			OnLoginSelectAllEtcItemData( i, i, szTempGUID, szTempID, dwUserIndex );
			OnLoginSelectAllExtraItemData( i, i, szTempGUID, szTempID, dwUserIndex );	
			OnLoginSelectAllQuestCompleteData( i, i, szTempGUID, szTempID, dwUserIndex );
			OnLoginSelectAllQuestData( i, i, szTempGUID, szTempID, dwUserIndex );
			OnLoginSelectAllCharData( i, i, szTempGUID, szTempID, dwUserIndex );
			OnLoginSelectAllInvenData( 0, i, i, szTempGUID, szTempID, dwUserIndex );
			OnLoginSelectAllGrowth( i, i, szTempGUID, szTempID, dwUserIndex );
			OnLoginSelectAllMedalItemData( i, i, szTempGUID, szTempID, dwUserIndex );
			OnLoginSelectAllEventData( i, i, szTempGUID, szTempID, dwUserIndex );
			OnLoginSelectAllAlchemicData( i, i, szTempGUID, szTempID, dwUserIndex );
			
			vIndexList.erase( vIndexList.begin() );		
		}
		OnDBServerTestLastQuery( i, i );	
		Sleep( 1 );
	}
}

void DBClient::OnDBServerTestLastQuery( const DWORD dwAgentID, const DWORD dwThreadID )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_member_get_count 'aaaa'" );

	const int queryId = 131;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( "aaaa", 4, TRUE );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) ); // 존재유무 ( 0:없다. / 1이상:존재한다. )

	CQueryData query_data;
	DWORD dwCurrentTime = TIMEGETTIME();
	query_data.SetReturnData( &dwAgentID, sizeof( DWORD ) );
	query_data.SetReturnData( &dwCurrentTime, sizeof( DWORD ) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_TEST_SERVER_DELAY, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnDBServerTestLastQuery Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectHeadquartersDataCount( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query,"exec game_hq_get_count %d", dwUserIdx );

	const int queryId = 157;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) ); // 결과

	CQueryData query_data;
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );		
	//query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );	
	query_data.SetReturnData( &dwUserIdx, sizeof(DWORD) );			
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_HEADQUARTERS_DATA_COUNT, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectHeadquartersDataCount Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnInsertHeadquartersData( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIdx )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//wsprintf(str_query,"exec game_hq_add %d", dwUserIdx );

	const int queryId = 158;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_HEADQUARTERS_DATA_SET, 
		_INSERTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnInsertHeadquartersData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectHeadquartersData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query,"exec game_hq_get_data %d", dwUserIdx );

	const int queryId = 159;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );
	for(int i = 0;i < MAX_DISPLAY_CNT;i++)
	{
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );  // 용병 인덱스
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );  // XPos
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );  // ZPos
	}
	v_VT.push_back( GetValueType( vSHORT, sizeof(short) ) );    // Lock

	CQueryData query_data;
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );		
	//query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );	
	query_data.SetReturnData( &dwUserIdx, sizeof(DWORD) );			
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_HEADQUARTERS_DATA_GET, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectHeadquartersData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateHeadquartersData( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, const UserHeadquartersOption &rkData )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_hq_save %d, %s", dwUserIndex, szContent );

	const int queryId = 160;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	for(int i = 0;i < MAX_DISPLAY_CNT;i++)
	{
		v_FT.Write( rkData.m_dwCharacterIndex[i] );
		v_FT.Write( rkData.m_iCharacterXPos[i] );
		v_FT.Write( rkData.m_iCharacterZPos[i] );
	}
	v_FT.Write( rkData.m_sLock );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_HEADQUARTERS_DATA_UPD, 
		_UPDATEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdateHeadquartersData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectUserBirthDate( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szPrivateID )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query,"exec game_get_age_from_id '%s'", szPrivateID.c_str() );

	const int queryId = 161;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( szPrivateID.c_str(), szPrivateID.Length(), TRUE );
	v_VT.push_back( GetValueType( vChar, USER_BIRTH_DATE_PLUS_ONE ) );  // 주민번호 앞자리
	v_VT.push_back( GetValueType( vChar, sizeof(BYTE) ) );             // 주민번호 뒷자리 첫번째 숫자
	v_VT.push_back(GetValueType( vSHORT,sizeof(short)));			  //채널링 타입 // db smallint

	CQueryData query_data;
	query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );		
	query_data.SetReturnData( szPrivateID.c_str(), ID_NUM_PLUS_ONE );			
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_USER_BIRTH_DATE_GET, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectUserBirthDate Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}


void DBClient::OnSelectUserSelectShutDown( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szPrivateID )
{
	SYSTEMTIME sysTime;
	CTime CurrentTime = CTime::GetCurrentTime();
	CurrentTime.GetAsSystemTime( sysTime );

	int iServiceCode = 201125;
	const int queryId = 167;

	cSerialize v_FT;
	vVALUETYPE v_VT;
	v_FT.Write( szPrivateID.c_str(), szPrivateID.Length(), TRUE );
	v_FT.Write( iServiceCode );
	v_FT.Write( (uint8*)&sysTime, sizeof(sysTime), TRUE );
	v_VT.push_back( GetValueType( vChar, sizeof( char ) ) );    
	v_VT.push_back( GetValueType( vTimeStamp,sizeof(DBTIMESTAMP) ));

	CQueryData query_data;
	query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );		
	query_data.SetReturnData( szPrivateID.c_str(), ID_NUM_PLUS_ONE );			
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_USER_SELECT_SHUT_DOWN_GET, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog(0,"DB OnSelectUserSelectShutDown Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectFriendRecommendData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query,"exec game_event_chuchun_get_data %d", dwUserIdx );

	const int queryId = 162;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) ); // 테이블 인덱스
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) ); // 추천한 친구 인덱스

	CQueryData query_data;
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );		
	//query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );	
	query_data.SetReturnData( &dwUserIdx, sizeof(DWORD) );			
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGNET_FRIEND_RECOMMEND_GET, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectFriendRecommendData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateFriendRecommendData( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwTableIndex )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_event_chuchun_save %d", dwTableIndex );

	const int queryId = 163;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwTableIndex );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGNET_FRIEND_RECOMMEND_UPD, 
		_UPDATEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdateFriendRecommendData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectDisconnectCheck( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szPrivateID )
{
	if( szPrivateID.IsEmpty() )	
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Error - OnSelectDisconnectCheck - szID is empty");
		return;
	}

	//char str_query[MAX_QUERY_SIZE] = "";
	//wsprintf(str_query,"exec game_login_get_data '%s'", szPrivateID.c_str());

	const int queryId = 164;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( szPrivateID.c_str(), szPrivateID.Length(), TRUE );
	v_VT.push_back(GetValueType(vChar,ID_NUM_PLUS_ONE));          //유저 아이디
	v_VT.push_back(GetValueType(vChar,LOGIN_KEY_PLUS_ONE));       //로그인 키
	v_VT.push_back(GetValueType(vINT64,sizeof(__int64)));         //게임서버ID
	v_VT.push_back(GetValueType(vTimeStamp,sizeof(DBTIMESTAMP))); //로그인 키 생성 날짜,시간.

	CQueryData query_data;	
	query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	query_data.SetReturnData( szPrivateID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetData( 
		dwThreadID,
		_RESULT_CHECK,
		DBAGENT_DISCONNECT_CHECK_GET,
		_SELECTDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectDisconnectCheck Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

// 2020-06-05 by bckim, 하드시리얼 유저 제재
void DBClient::OnSelectUserRestrictionbyHddSerial(  User *pUser, const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &sHddSerialString )
{
	if( pUser == NULL )
		return;

	if( pUser->GetUserIndex() == 0 )
		return;

	if( pUser->GetPublicID().IsEmpty() )
		return;

	if( pUser->GetPublicIP() == NULL )
		return;

	if( sHddSerialString.IsEmpty() )	
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Error - OnSelectUserRestrictionbyHddSerial - sHddSerialString is empty");
		return;
	}
	
	const int iQueryID = 2207;
	// 2207=game_block_hdd_serial_check INT STR 

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"!!!!!!!! DB DBAGENT_DISCONNECT_HDD_SERIAL_CHECK_GET START!!!!!!!!!" );

	cSerialize v_FT;
	vVALUETYPE v_VT;

	char szHddSerialString[MAX_PATH]="";
	StringCbCopyN( szHddSerialString, sizeof( szHddSerialString ), sHddSerialString.c_str(), MAX_PATH );	// 2019-03-05 by bckim, 유저 UAC rsHddSerialString 추가

	DWORD dwUserIdx = pUser->GetUserIndex();
	int istrLen = strlen(szHddSerialString);

	v_FT.Write( dwUserIdx );
	v_FT.Write( szHddSerialString, strlen(szHddSerialString), TRUE);
	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) );             // result

	CQueryData query_data;	
	query_data.SetReturnData( &dwUserIdx, sizeof(DWORD) );			
	query_data.SetReturnData( &istrLen, sizeof(int) );			
	query_data.SetReturnData( szHddSerialString, istrLen );

	query_data.SetData( 
		dwThreadID,
		_RESULT_CHECK,
		DBAGENT_DISCONNECT_HDD_SERIAL_CHECK_GET,
		_SELECTDB,
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	kPacket << query_data;
	SendMessage( dwAgentID, kPacket );
}
// End. 2020-06-05 by bckim, 하드시리얼 유저 제재

// alchemic
void DBClient::OnInsertAlchemicData( const DWORD dwAgentID, const DWORD dwThreadID,
									 const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx,
									 cSerialize& v_FT )
{
	if( szID.IsEmpty() ) return;

	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_piece_add %d%s", dwUserIdx, szContent );

	const int queryId = 169;

	vVALUETYPE v_VT;

	CQueryData query_data;
	query_data.SetData( dwThreadID, _RESULT_CHECK, DBAGENT_ALCHEMIC_DATA_SET, 
						_INSERTDB, queryId, v_FT, v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnInsertAlchemicData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}

	OnSelectAlchemicIndex( dwAgentID, dwThreadID, szUserGUID, szID, dwUserIdx );
}

void DBClient::OnSelectAlchemicIndex( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_piece_self_index %d", dwUserIdx );

	const int queryId = 170;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	//
	v_FT.Write( dwUserIdx );

	//
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//Item INDEX

	CQueryData query_data;
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	//query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwUserIdx, sizeof(int) );
	query_data.SetData( dwThreadID, _RESULT_CHECK,
						DBAGENT_ALCHEMIC_DATA_CREATE_INDEX, _SELECTDB, 
						queryId, v_FT, v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectAlchemicIndex Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateAlchemicData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szID, DWORD dwInvenIdx, cSerialize& v_FT  )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query, "exec game_piece_save %d%s", dwInvenIdx, szContent );

	const int queryId = 171;

	vVALUETYPE v_VT;
	
	CQueryData query_data;
	query_data.SetData( dwThreadID, _RESULT_CHECK,
						DBAGENT_ALCHEMIC_DATA_UPD, _UPDATEDB,
						queryId,
						v_FT,
						v_VT );

	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdateAlchemicData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnInsertTournamentTeamCreate( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, 
											 const DWORD dwTourIndex, const ioHashString &rkTeamName, BYTE MaxPlayer, int iLadderPoint, BYTE CampPos )
{
	// game_league_team_add INT STR INT INT8 INT INT8
	const int queryId = 2021;
		//
	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwTourIndex );
	v_FT.Write( rkTeamName.c_str(), rkTeamName.Length(), TRUE );
	v_FT.Write( dwUserIndex );
	v_FT.Write( MaxPlayer );
	v_FT.Write( iLadderPoint );
	v_FT.Write( CampPos );

	v_VT.push_back(GetValueType(vLONG,sizeof(int))); // 결과

	CQueryData query_data;
	query_data.SetReturnData( &dwAgentID, sizeof(DWORD) );
	query_data.SetReturnData( &dwThreadID, sizeof(DWORD) );
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwTourIndex, sizeof(DWORD) );

	query_data.SetData( dwThreadID, _RESULT_CHECK, DBAGENT_TOURNAMENT_TEAM_CREATE_GET, _SELECTDB, queryId, v_FT, v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnInsertTournamentTeamCreate Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "OnInsertTournamentTeamCreate Send" );
}

void DBClient::OnSelectTournamentTeamIndex( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwTourIndex )
{
	// game_league_team_self_index 리그인덱스, 팀장인덱스

	const int queryId = 2022;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwTourIndex );
	v_FT.Write( dwUserIndex );

	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) ); //Team Index

	CQueryData query_data;
	query_data.SetReturnData( &dwAgentID, sizeof(DWORD) );
	query_data.SetReturnData( &dwThreadID, sizeof(DWORD) );
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwTourIndex, sizeof(DWORD) );
	query_data.SetData( dwThreadID, _RESULT_CHECK, DBAGENT_TOURNAMENT_TEAM_INDEX_GET, _SELECTDB, queryId, v_FT, v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectTournamentTeamIndex Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "OnSelectTournamentTeamIndex Send" );
}

void DBClient::OnSelectTournamentTeamList( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const DWORD dwUserIndex, const DWORD dwTeamIndex, const int iSelectCount )
{
	//> game_league_my_team_list 가져올개수, 팀인덱스, 유저인덱스

	const int queryId = 2023;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( iSelectCount );
	v_FT.Write( dwTeamIndex );
	v_FT.Write( dwUserIndex );

	// - 팀인덱스, 리그인덱스, 팀이름, 팀장인덱스, 리그포지션, 응원포인트, 토너먼트위치, 진영포인트, 진영타입
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );
	v_VT.push_back( GetValueType( vChar, TOURNAMENT_TEAM_NAME_NUM_PLUS_ONE ) );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );
	v_VT.push_back( GetValueType( vSHORT, sizeof(SHORT) ) );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );
	v_VT.push_back( GetValueType( vChar, sizeof(char) ) );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );
	v_VT.push_back( GetValueType( vChar, sizeof(char) ) );

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );		
	query_data.SetData( dwThreadID, _RESULT_CHECK, DBAGENT_TOURNAMENT_TEAM_LIST_GET, _SELECTEX1DB, queryId, v_FT, v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectTournamentTeamList Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "OnSelectTournamentTeamList Send" );
}

void DBClient::OnSelectTournamentCreateTeamData( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwTeamIndex )
{
	// game_league_team_get_data 팀인덱스

	const int queryId = 2026;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwTeamIndex );

	//- 리그인덱스, 팀이름, 팀장인덱스, 리그포지션, 팀맥스카운트, 응원포인트, 토너먼트위치, 진영포인트, 진영타입
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );
	v_VT.push_back( GetValueType( vChar, TOURNAMENT_TEAM_NAME_NUM_PLUS_ONE ) );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );
	v_VT.push_back( GetValueType( vSHORT, sizeof(SHORT) ) );
	v_VT.push_back( GetValueType( vChar, sizeof(char) ) );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );
	v_VT.push_back( GetValueType( vChar, sizeof(char) ) );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );
	v_VT.push_back( GetValueType( vChar, sizeof(char) ) );

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwTeamIndex, sizeof(DWORD) );
	query_data.SetData( dwThreadID, _RESULT_CHECK, DBAGENT_TOURNAMENT_CREATE_TEAM_DATA_GET, _SELECTDB, queryId, v_FT, v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectTournamentCreateTeamData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "OnSelectTournamentCreateTeamData Send" );
}

void DBClient::OnSelectTournamentTeamMember( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const DWORD dwUserIndex, const DWORD dwTeamIndex )
{
	// game_league_team_member_list 팀인덱스

	const int queryId = 2028;
	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwTeamIndex );

	// - 테이블인덱스, 유저인덱스, 닉네임, 레벨, 진영포인트, 길드인덱스
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );
	v_VT.push_back( GetValueType( vChar, ID_NUM_PLUS_ONE ) );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwTeamIndex, sizeof(DWORD) );
	query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );		
	query_data.SetData( dwThreadID, _RESULT_CHECK, DBAGENT_TOURNAMENT_TEAM_MEMBER_GET, _SELECTEX1DB, queryId, v_FT, v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectTournamentTeamMember Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "OnSelectTournamentTeamMember Send" );
}

void DBClient::OnSelectTournamentTeamAppList( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwTourIndex, const DWORD dwTeamIndex )
{
	// game_league_team_member_app_list INT

	const int queryId = 2030;	
	
	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwTeamIndex );

	// - 테이블인덱스, 유저인덱스, 닉네임, 레벨, 길드인덱스, 진영 타입
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );
	v_VT.push_back( GetValueType( vChar, ID_NUM_PLUS_ONE ) );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );
	v_VT.push_back( GetValueType( vChar, sizeof(BYTE) ) );

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwTourIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwTeamIndex, sizeof(DWORD) );
	query_data.SetData( dwThreadID, _RESULT_CHECK, DBAGENT_TOURNAMENT_TEAM_APP_LIST_GET, _SELECTEX1DB, queryId, v_FT, v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectTournamentTeamAppList Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "OnSelectTournamentTeamAppList Send" );
}

void DBClient::OnInsertTournamentTeamAppAdd( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwTourIndex, const DWORD dwTeamIndex, const DWORD dwMasterIndex )
{
	// game_league_team_member_app_add INT INT INT

	const int queryId = 2031;
	
	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( dwTourIndex );
	v_FT.Write( dwTeamIndex );
	
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );  // 결과

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwTourIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwTeamIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwMasterIndex, sizeof(DWORD) );

	query_data.SetData( dwThreadID, _RESULT_CHECK, DBAGENT_TOURNAMENT_TEAM_APP_ADD_SET, _SELECTDB, queryId, v_FT, v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnInsertTournamentTeamAppAdd Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "OnInsertTournamentTeamAppAdd Send" );
}

void DBClient::OnDeleteTournamentTeamAppDel( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwTableIndex )
{
	// game_league_team_member_app_del INT

	const int queryId = 2032;

	cSerialize v_FT;
	v_FT.Write( dwTableIndex );

	vVALUETYPE v_VT;

	CQueryData query_data;
	query_data.SetData( dwThreadID, _RESULT_DESTROY, DBAGENT_TOURNAMENT_TEAM_APP_DEL, _DELETEDB, queryId, v_FT,	v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnDeleteTournamentTeamAppDel Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "OnDeleteTournamentTeamAppDel Send" );
}

void DBClient::OnUpdateTournamentTeamAppReg( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwRegUserIndex, const DWORD dwTourIndex, const DWORD dwTeamIndex, const DWORD dwTableIndex, BYTE CampPos )
{
	// game_league_team_member_app_reg INT INT INT INT8

	const int queryId = 2033;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwRegUserIndex );
	v_FT.Write( dwTourIndex );
	v_FT.Write( dwTeamIndex );
	v_FT.Write( CampPos );

	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );  // 결과

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwTourIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwTeamIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwRegUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwTableIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwAgentID, sizeof(DWORD) );
	query_data.SetReturnData( &dwThreadID, sizeof(DWORD) );

	query_data.SetData( dwThreadID, _RESULT_CHECK, DBAGENT_TOURNAMENT_TEAM_APP_REG_UPD, _SELECTDB, queryId, v_FT, v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdateTournamentTeamAppReg Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "OnUpdateTournamentTeamAppReg Send" );
}

void DBClient::OnDeleteTournamentTeamAppList( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, DWORD dwTourIndex )
{
	// game_league_team_member_app_list_del INT INT

	const int queryId = 2034;

	cSerialize v_FT;
	v_FT.Write( dwUserIndex );
	v_FT.Write( dwTourIndex );

	vVALUETYPE v_VT;

	CQueryData query_data;
	query_data.SetData( dwThreadID, _RESULT_DESTROY, DBAGENT_TOURNAMENT_TEAM_APP_LIST_DEL, _DELETEDB, queryId, v_FT,	v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnDeleteTournamentTeamAppList Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "OnDeleteTournamentTeamAppList Send" );
}

void DBClient::OnSelectTournamentTeamAppAgreeMember( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwTourIndex, const DWORD dwTeamIndex, const DWORD dwAppUserIndex )
{
	// game_league_team_member_list 팀인덱스

	const int queryId = 2028;
	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwTeamIndex );

	// - 테이블인덱스, 유저인덱스, 닉네임, 레벨, 진영포인트, 길드인덱스
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );
	v_VT.push_back( GetValueType( vChar, ID_NUM_PLUS_ONE ) );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwTourIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwTeamIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwAppUserIndex, sizeof(DWORD) );
	query_data.SetData( dwThreadID, _RESULT_CHECK, DBAGENT_TOURNAMENT_TEAM_APP_AGREE_MEMBER, _SELECTEX1DB, queryId, v_FT, v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectTournamentTeamAppAgreeMember Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "OnSelectTournamentTeamAppAgreeMember Send" );
}

void DBClient::OnSelectTournamentAppAgreeTeam( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwTeamIndex )
{
	// game_league_team_get_data 팀인덱스

	const int queryId = 2026;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwTeamIndex );

	//- 리그인덱스, 팀이름, 팀장인덱스, 리그포지션, 팀맥스카운트, 응원포인트, 토너먼트위치, 진영포인트, 진영타입
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );
	v_VT.push_back( GetValueType( vChar, TOURNAMENT_TEAM_NAME_NUM_PLUS_ONE ) );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );
	v_VT.push_back( GetValueType( vSHORT, sizeof(SHORT) ) );
	v_VT.push_back( GetValueType( vChar, sizeof(char) ) );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );
	v_VT.push_back( GetValueType( vChar, sizeof(char) ) );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );
	v_VT.push_back( GetValueType( vChar, sizeof(char) ) );

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwTeamIndex, sizeof(DWORD) );
	query_data.SetData( dwThreadID, _RESULT_CHECK, DBAGENT_TOURNAMENT_TEAM_APP_AGREE_TEAM, _SELECTDB, queryId, v_FT, v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectTournamentAppAgreeTeam Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "OnSelectTournamentAppAgreeTeam Send" );
}

void DBClient::OnDeleteTournamentTeamMember( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwTourIndex, const DWORD dwTeamIndex, const DWORD dwTableIndex, const DWORD dwMemberIndex )
{
	// game_league_team_member_delete INT INT

	const int queryId = 2035;

	cSerialize v_FT;
	v_FT.Write( dwTableIndex );
	v_FT.Write( dwMemberIndex );

	vVALUETYPE v_VT;
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );           // 결과 : 음수값일 경우 에러 !!! 양수일 때 진영 포인트

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwMemberIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwTourIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwTeamIndex, sizeof(DWORD) );
	query_data.SetData( dwThreadID, _RESULT_CHECK, DBAGENT_TOURNAMENT_TEAM_MEMBER_DEL, _SELECTDB, queryId, v_FT,	v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnDeleteTournamentTeamMember Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "OnDeleteTournamentTeamMember Send" );
}

void DBClient::OnUpdateUserCampPosition( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, int iCampPosition )
{
	//  game_factiontype_save 유저인덱스, 진영타입

	const int queryId = 2037;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( iCampPosition );
	
	CQueryData query_data;
	query_data.SetData( dwThreadID, _RESULT_NAUGHT, DBAGENT_USER_CAMP_POSITION_UPD, _UPDATEDB, queryId, v_FT, v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdateUserCampPosition Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectTournamentHistoryList( DWORD dwCount, DWORD dwStartIndex )
{
	// 2040=game_league_history_list INT INT
	const int queryId = 2040;
	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwCount );
	v_FT.Write( dwStartIndex );

	// - 인덱스, 리그타이틀, 리그싲작날짜, 팀명, 진영타입, 진영명
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );
	v_VT.push_back( GetValueType( vChar, TOURNAMENT_TITLE_NUM_PLUS_ONE ) );	
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );
	v_VT.push_back( GetValueType( vChar, TOURNAMENT_TEAM_NAME_NUM_PLUS_ONE ) );	
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );
	v_VT.push_back( GetValueType( vChar, TOURNAMENT_CAMP_NAME_NUM_PLUS_ONE ) );	

	CQueryData query_data;
	query_data.SetData( GameServerThreadID(), _RESULT_CHECK, DBAGENT_TOURNAMENT_HISTORY_LIST, _SELECTEX1DB, queryId, v_FT, v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( GameServerAgentID(), kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectTournamentHistoryList Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "OnSelectTournamentHistoryList Send" );
}

void DBClient::OnSelectTournamentHistoryUserList( DWORD dwHistoryIndex )
{
	// 2041=game_league_winner_info INT
	const int queryId = 2041;

	cSerialize v_FT;
	v_FT.Write( dwHistoryIndex );

	vVALUETYPE v_VT;
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );      // 유저 인덱스
	v_VT.push_back( GetValueType( vChar, ID_NUM_PLUS_ONE ) );	// 닉네임
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );      // 레벨
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );      // 용병타입
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );      // 성별
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );      // 얼굴
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );      // 머리
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );      // 피부색 
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );      // 머리색 
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );      // 속옷 
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );      // 무기 
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );      // 갑옷 
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );      // 투구 
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );      // 망토 

	CQueryData query_data;
	query_data.SetReturnData( &dwHistoryIndex, sizeof(DWORD) );
	query_data.SetData( GameServerThreadID(), _RESULT_CHECK, DBAGENT_TOURNAMENT_HISTORY_USER_LIST, _SELECTEX1DB, queryId, v_FT,	v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( GameServerAgentID(), kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectTournamentHistoryUserList Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "OnSelectTournamentHistoryUserList Send" );
}

void DBClient::OnSelectTournamentReward( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const DWORD dwUserIndex )
{
	// 2042=game_league_get_reward INT
	const int queryId = 2042;

	cSerialize v_FT;
	v_FT.Write( dwUserIndex );

	vVALUETYPE v_VT;
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );      // 테이블 인덱스
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );      // 리그 시작 날짜
	v_VT.push_back( GetValueType( vChar, sizeof(BYTE) ) );      // 최종 달성 라운드
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );      // 유저진영정보
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );      // 우승팀 진영 정보
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );      // 진영보상페소
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );      // 진영 랭킹
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );      // 진영 획득 포인트

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	query_data.SetData( dwThreadID, _RESULT_CHECK, DBAGENT_TOURNAMENT_REWARD_DATA, _SELECTDB, queryId, v_FT, v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectTournamentReward Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "OnSelectTournamentReward Send" );
}

void DBClient::OnDeleteTournamentReward( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwTableIndex )
{
	// game_league_reward_del INT
	const int queryId = 2043;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwTableIndex );
	
	CQueryData query_data;
	query_data.SetData( dwThreadID, _RESULT_DESTROY, DBAGENT_TOURNAMENT_REWARD_DATA_DELETE, _DELETEDB, queryId, v_FT,	v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnDeleteTournamentReward Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "OnDeleteTournamentReward Send" );
}

void DBClient::OnInsertTournamentCustomAdd( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const ioHashString &szPublicID, DWORD dwUseEtcItem,
											DWORD dwStartDate, DWORD dwEndDate, uint8 Type, uint8 State, const ioHashString &rkTourName, uint16 MaxRound, DWORD dwBannerL, DWORD dwBannerS, int iModeBattleType,
											uint8 MaxPlayer, uint8 RoundType, DWORD dwAppDate, DWORD dwDelayDate, DWORD dwRoundDate1,  DWORD dwRoundDate2,  DWORD dwRoundDate3, DWORD dwRoundDate4, DWORD dwRoundDate5,
											DWORD dwRoundDate6, DWORD dwRoundDate7, DWORD dwRoundDate8, DWORD dwRoundDate9, DWORD dwRoundDate10 )
{
	// 2044=game_league_user_list_add INT INT INT INT8 INT8 STR INT16 INT INT INT INT8 INT8 INT INT INT INT INT INT INT INT INT INT INT INT
	const int queryId = 2044;

	cSerialize v_FT;
	v_FT.Write( dwUserIndex );										// 주최자 인덱스
	v_FT.Write( dwStartDate );										// 대회 시작 시간
	v_FT.Write( dwEndDate );										// 대회 종료 시간
	v_FT.Write( Type );												// 대회 타입
	v_FT.Write( State );											// 대회 상태
	v_FT.Write( rkTourName.c_str(), rkTourName.Length(), TRUE );	// 대회명
	v_FT.Write( MaxRound );                                         // 대회 라운드
	v_FT.Write( dwBannerL );                                        // 대회 배너 ( Large )
	v_FT.Write( dwBannerS );										// 대회 배너 ( Small )
	v_FT.Write( iModeBattleType );									// 대회 모드 BMT_
	v_FT.Write( MaxPlayer );										// 대회 팀 최대 인원
	v_FT.Write( RoundType );                                        // 대회 경기 방식 ( 수동 & 자동 )
	v_FT.Write( dwAppDate );                                        // 팀 모집 기간
	v_FT.Write( dwDelayDate );										// 팀 배정(대기) 기간
	v_FT.Write( dwRoundDate1 );										// 1라운드 시간
	v_FT.Write( dwRoundDate2 );										// 2라운드 시간
	v_FT.Write( dwRoundDate3 );										// 3라운드 시간
	v_FT.Write( dwRoundDate4 );										// 4라운드 시간
	v_FT.Write( dwRoundDate5 );										// 5라운드 시간
	v_FT.Write( dwRoundDate6 );										// 6라운드 시간
	v_FT.Write( dwRoundDate7 );										// 7라운드 시간
	v_FT.Write( dwRoundDate8 );										// 8라운드 시간
	v_FT.Write( dwRoundDate9 );										// 9라운드 시간
	v_FT.Write( dwRoundDate10 );									// 10라운드 시간

	DWORD dwDummyRoundDate11 = 0;
	v_FT.Write( dwDummyRoundDate11 );								// 더미 시간

	vVALUETYPE v_VT;
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );			// 토너먼트 인덱스

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwUseEtcItem, sizeof(DWORD) );
	query_data.SetReturnData( szPublicID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetData( dwThreadID, _RESULT_CHECK, DBAGENT_TOURNAMENT_CUSTOM_DATA_ADD, _SELECTDB, queryId, v_FT,	v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnInsertTournamentCustomAdd Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "OnInsertTournamentCustomAdd Send" );
}

void DBClient::OnSelectTournamentCustomReward( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, DWORD dwUserIdx )
{
	// 2060=game_league_user_reward_get_data INT
	const int queryId = 2060;

	cSerialize v_FT;
	v_FT.Write( dwUserIdx );

	vVALUETYPE v_VT;
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );					// 테이블 인덱스
	v_VT.push_back( GetValueType( vChar, ID_NUM_PLUS_ONE ) );				// 주최자 닉네임
	v_VT.push_back( GetValueType( vChar, TOURNAMENT_TITLE_NUM_PLUS_ONE ) );	// 리그명
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );					// 라운드
	v_VT.push_back( GetValueType( vSHORT, sizeof(short) ) );				// 맥스라운드
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );					// 보상1
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );					// 보상2
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );					// 보상3
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );					// 보상4

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIdx, sizeof(DWORD) );
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );	
	query_data.SetData( dwThreadID, _RESULT_CHECK, DBAGENT_TOURNAMENT_CUSTOM_REWARD_GET, _SELECTEX1DB, queryId, v_FT,	v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectTournamentCustomReward Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "OnSelectTournamentCustomReward Send" );
}

void DBClient::OnDeleteTournamentCustomReward( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwTableIndex )
{
	//2061=game_league_user_reward_del INT
	const int queryId = 2061;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwTableIndex );

	CQueryData query_data;
	query_data.SetData( dwThreadID, _RESULT_DESTROY, DBAGENT_TOURNAMENT_CUSTOM_REWARD_DEL, _DELETEDB, queryId, v_FT,	v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnDeleteTournamentCustomReward Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "OnDeleteTournamentCustomReward Send" );
}

void DBClient::OnInsertEventMarble( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, const ioHashString &szPublicID, int iChannelingType )
{
	const int queryId = 172; // log_event_marble_add INT STR INT

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( szPublicID.c_str(), szPublicID.Length(), TRUE );
	v_FT.Write( iChannelingType );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_EVENT_MARBLE_SET, 
		_INSERTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnInsertEventMarble Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

//////////////////////////////////////////////////////////////////////////
void DBClient::OnSelectCloverInfoRequest( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const ioHashString &szUserGUID )
{
	if( dwUserIndex == 0 || szUserGUID.IsEmpty() )
		return;

	const int queryId = 2052;	// game_clover_info INT

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_VT.push_back( GetValueType( vLONG, sizeof( LONG ) ) );		// 보낼수 있는 클로버 갯수
	v_VT.push_back( GetValueType( vLONG, sizeof( LONG ) ) );		// 마지막 충전 시간.
	v_VT.push_back( GetValueType( vSHORT, sizeof( short ) ) );		// 남은 시간.

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof( DWORD ) );
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_CLOVER_INFO_GET, 
		_SELECTEX1DB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( ! SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnCloverInfoRequest Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateCloverInfo( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const int iCloverCount, const int iLastChargeTime, const short sRemainTime )
{
	const int queryId = 2053;	//game_clover_info_update INT INT INT

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( iCloverCount );
	v_FT.Write( iLastChargeTime );
	v_FT.Write( sRemainTime );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_CLOVER_INFO_SET, 
		_UPDATEDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	if( ! SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdateCloverInfo Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateFriendCloverInfo( const DWORD dwAgentID, const DWORD dwThreadID, const int iTableIndex, const int iSendCount, const int iSendDate, const int iReceiveCount, const int iReceiveDate, const int iBReceiveCount )
{
	const int queryId = 2054;	//game_friend_clover_info_update INT INT INT INT INT INT

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( iTableIndex );
	v_FT.Write( iSendCount );
	v_FT.Write( iSendDate );
	v_FT.Write( iReceiveCount );
	v_FT.Write( iReceiveDate );
	v_FT.Write( iBReceiveCount );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_FRIEND_CLOVER_INFO_SET, 
		_UPDATEDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );

	kPacket << query_data;
	if( ! SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdateCloverInfo Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateFriendReceiveCloverInfo( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwFriendIndex, const int iDate, const int iSendCount )
{
	const int queryId = 2055;	// game_clover_friend_receive_save INT INT INT

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( dwFriendIndex );
	v_FT.Write( iDate );
	v_FT.Write( iSendCount );
	v_VT.push_back( GetValueType( vLONG, sizeof( LONG ) ) );	// index
	v_VT.push_back( GetValueType( vLONG, sizeof( LONG ) ) );	// friendAccIDX
	v_VT.push_back( GetValueType( vLONG, sizeof( LONG ) ) );	// receive Clover Date
	v_VT.push_back( GetValueType( vLONG, sizeof( LONG ) ) );	// receive B Clover Cnt

	CQueryData query_data;
	query_data.SetReturnData( &dwFriendIndex, sizeof( DWORD ) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_FRIEND_RECEIVE_CLOVER_SET, 
		_SELECTDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( ! SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdateCloverInfo Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

//! 빙고
void DBClient::OnInsertBingoNumber( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const BYTE byNumber[][ ioBingo::MAX ] )
{
	const int queryId = 2070;	// game_event_bingo_number_add INT INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );

	for( int i = 0 ; i < ioBingo::MAX ; ++i )
	{
		for( int j = 0 ; j < ioBingo::MAX ; ++j )
		{
			v_FT.Write( byNumber[ i ][ j ] );
		}
	}

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_BINGO_NUMBER_SET, 
		_INSERTDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( ! SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdateCloverInfo Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnInsertBingoPresent( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const BYTE byPresent[ ioBingo::PRESENT_COUNT ] )
{
	const int queryId = 2071;	// game_event_bingo_present_add INT INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );

	for( int i = 0 ; i < ioBingo::PRESENT_COUNT ; ++i )
	{
		v_FT.Write( byPresent[ i ] );
	}

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_BINGO_PRESENT_SET, 
		_INSERTDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( ! SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdateCloverInfo Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectBingoNumber( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex )
{
	const int queryId = 2072;	// game_event_bingo_number_get INT

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );

	for( int i = 0 ; i < ioBingo::MAX ; ++i )
	{
		for( int j = 0 ; j < ioBingo::MAX ; ++j )
		{
			v_VT.push_back( GetValueType( vChar, sizeof( BYTE ) ) );
		}
	}

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof( DWORD ) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_BINGO_NUMBER_GET, 
		_SELECTEX1DB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( ! SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdateCloverInfo Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectBingoPresent( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex )
{
	const int queryId = 2073;	// game_event_bingo_present_get INT

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );

	for( int i = 0 ; i < ioBingo::PRESENT_COUNT ; ++i )
	{
		v_VT.push_back( GetValueType( vChar, sizeof( BYTE ) ) );
	}

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof( DWORD ) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_BINGO_PRESENT_GET, 
		_SELECTEX1DB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( ! SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdateCloverInfo Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateBingoNumber( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const BYTE byNumber[][ ioBingo::MAX ] )
{
	const int queryId = 2074;	// game_event_bingo_number_save INT INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );

	for( int i = 0 ; i < ioBingo::MAX ; ++i )
	{
		for( int j = 0 ; j < ioBingo::MAX ; ++j )
		{
			v_FT.Write( byNumber[ i ][ j ] );
		}
	}

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_BINGO_NUMBER_UPD, 
		_UPDATEDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	if( ! SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdateBingoNumber Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateBingoPresent( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const BYTE byPresent[ ioBingo::PRESENT_COUNT ] )
{
	const int queryId = 2075;	// game_event_bingo_present_save INT INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );

	for( int i = 0 ; i < ioBingo::PRESENT_COUNT ; ++i )
	{
		v_FT.Write( byPresent[ i ] );
	}

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_BINGO_PRESENT_UPD, 
		_UPDATEDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( ! SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdateBingoPresent Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectRelativeGradeInfo( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex )
{
	const int queryId = 2080;	// game_relative_reward_get_data INT

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_VT.push_back( GetValueType( vLONG, sizeof( LONG ) ) );	// relative grade backup exp
	v_VT.push_back( GetValueType( vLONG, sizeof( LONG ) ) );	// relative grade init time
	v_VT.push_back( GetValueType( vChar, sizeof( char ) ) );	// is recv reward ( y/n )

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof( DWORD ) );
	query_data.SetData( dwThreadID, _RESULT_CHECK, DBAGENT_RELATIVE_GRADE_INFO_GET, _SELECTDB, queryId, v_FT, v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( ! SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectRelativeGradeInfo Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateRelativeGradeReward( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, bool bReward )
{
	const int queryId = 2081;	// game_relative_reward_save INT INT

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( (int8)bReward );

	CQueryData query_data;
	query_data.SetData( dwThreadID, _RESULT_DESTROY, DBAGENT_RELATIVE_GRADE_REWARD_UPD, _UPDATEDB, queryId, v_FT, v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( ! SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdateRelativeGradeReward Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnInsertRelativeGradeTable( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex )
{
	const int queryId = 2083;	// game_relative_add INT

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof( DWORD ) );
	query_data.SetData( dwThreadID, _RESULT_DESTROY, DBAGENT_RELATIVE_GRADE_TABLE_SET, _INSERTDB, queryId, v_FT, v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( ! SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnInsertRelativeGradeTable Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnInsertTournamentCheerDecision( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwTourIndex, const DWORD dwTeamIndex )
{
	const int queryId = 2084;	// game_league_cheer_add INT INT INT

	cSerialize v_FT;
	v_FT.Write( dwUserIndex );
	v_FT.Write( dwTourIndex );
	v_FT.Write( dwTeamIndex );

	vVALUETYPE v_VT;
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );
	
	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwTourIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwTeamIndex, sizeof(DWORD) );
	query_data.SetData( dwThreadID, _RESULT_CHECK, DBAGENT_TOURNAMENT_CHEER_DECISION, _INSERTDB, queryId, v_FT, v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Send Fail! :%d - %d", __FUNCTION__, GetLastError(), queryId );
		return;
	}
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Send", __FUNCTION__ );
}

void DBClient::OnSelectTournamentCheerList( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const DWORD dwUserIndex, const int iSelectCount, const DWORD dwTableIdx )
{
	//> game_league_cheer_list 유저인덱스, 가져올개수, 테이블인덱스

	const int queryId = 2085;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( iSelectCount );
	v_FT.Write( dwTableIdx );

	// 대회인덱스, 팀인덱스, 테이블 인덱스
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );
	
	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	query_data.SetData( dwThreadID, _RESULT_CHECK, DBAGENT_TOURNAMENT_CHEER_LIST_GET, _SELECTEX1DB, queryId, v_FT, v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Send Fail! :%d - %d", __FUNCTION__, GetLastError(), queryId );
		return;
	}
	//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Send", __FUNCTION__ );
}

void DBClient::OnSelectTournamentCheerReward( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const DWORD dwUserIndex, DWORD dwCheerType )
{
	//2086=game_league_get_reward_cheer INT
	const int queryId = 2086;

	cSerialize v_FT;
	v_FT.Write( dwUserIndex );
	v_FT.Write( dwCheerType );

	vVALUETYPE v_VT;
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );						// 테이블 인덱스
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );						// 보상 응원 페소

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwCheerType, sizeof(DWORD) );
	query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	query_data.SetData( dwThreadID, _RESULT_CHECK, DBAGENT_TOURNAMENT_CHEER_REWARD_DATA, _SELECTDB, queryId, v_FT, v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectTournamentCheerReward Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}	
}

void DBClient::OnDeleteTournamentCheerReward( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwTableIdx )
{
	// 2087=game_league_reward_cheer_del INT
	const int queryId = 2087;

	vVALUETYPE v_VT;
	cSerialize v_FT;
	v_FT.Write( dwTableIdx );

	CQueryData query_data;	
	query_data.SetData( dwThreadID, _RESULT_DESTROY, DBAGENT_TOURNAMENT_CHEER_REWARD_DATA_DELETE, _DELETEDB, queryId, v_FT,	v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectTournamentCheerRewardDelete Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

// Pirate Roulette Reset / Start
void DBClient::OnInsertPirateRouletteNumber( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex )
{
	const int queryId = 2099;	// game_event_PirateRoulette_number_add INT
	const int MaxHP = ioPirateRoulette::ROULETTE_HP_MAX;
	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( MaxHP );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_PIRATEROULETTE_NUMBER_INSERT, 
		_INSERTDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( ! SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnInsertPirateRouletteNumber Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}
// Pirate Roulette Reward Received Info.
void DBClient::OnInsertPirateRoulettePresent( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex )
{
	const int queryId = 2102;	// game_event_PirateRoulette_present_add INT

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_PIRATEROULETTE_PRESENT_INSERT, 
		_INSERTDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( ! SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnInsertPirateRoulettePresent Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectPirateRouletteNumber( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex )
{
	const int queryId = 2100;	// game_event_PirateRoulette_number_get INT

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );

	v_VT.push_back( GetValueType( vLONG, sizeof(LONG)));
	for( int i = 0 ; i < ioPirateRoulette::ROULETTE_BOARD_MAX; ++i )
	{
		v_VT.push_back( GetValueType( vChar, sizeof(BYTE) ) );
	}

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof( DWORD ) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_PIRATEROULETTE_NUMBER_GET, 
		_SELECTEX1DB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( ! SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectPirateRouletteNumber Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectPirateRoulettePresent( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex )
{
	const int queryId = 2103;	// game_event_PirateRoulette_number_get INT

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );

	for( int i = 0 ; i < ioPirateRoulette::ROULETTE_PRESENT_MAX; ++i )
	{
		v_VT.push_back( GetValueType( vChar, sizeof( BYTE ) ) );
	}

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof( DWORD ) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_PIRATEROULETTE_PRESENT_GET, 
		_SELECTEX1DB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( ! SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectPirateRoulettePresent Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdatePirateRouletteNumber( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwHP, const BYTE byNumber[ioPirateRoulette::ROULETTE_BOARD_MAX] )
{
	const int queryId = 2101;	// game_event_PirateRoulette_number_save INT INT INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( dwHP );
	for( int i = 0 ; i < ioPirateRoulette::ROULETTE_BOARD_MAX; ++i )
	{
		v_FT.Write( byNumber[i]);
	}

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_PIRATEROULETTE_NUMBER_UPDATE, 
		_UPDATEDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	if( ! SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdatePirateRouletteNumber Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdatePirateRoulettePresent( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const BYTE byPresent[ioPirateRoulette::ROULETTE_PRESENT_MAX] )
{
	const int queryId = 2104;	// game_event_PirateRoulette_number_save INT INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	for( int i = 0 ; i < ioPirateRoulette::ROULETTE_PRESENT_MAX; ++i )
	{
		v_FT.Write( byPresent[i]);
	}

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_PIRATEROULETTE_PRESENT_UPDATE, 
		_UPDATEDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	if( ! SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdatePirateRouletteNumber Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::AddDBAgentInfo( TCHAR* ipaddr, int port )
{
	std::pair<std::string,int> prdata;
	prdata.first = ipaddr;
	prdata.second = port;

	m_DBAgentInfos.push_back(prdata);

}

bool DBClient::IsDBAgentActive( int index )
{
	DBAgentNodeMap::iterator iter = m_DBAgentMap.find( index );
	if( iter == m_DBAgentMap.end() )
		return false;

	DBAgentNode *pNode = iter->second;
	if( pNode )
		return pNode->IsActive();

	return false;
}

void DBClient::OnInsertSubscriptionData( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex,
										 const ioHashString &szSubscriptionID, int iSubscriptionGold, int iBonusCash, short iPresentType, int iPresentValue1, int iPresentValue2,
										 short iSubscriptionState, CTime &rkLimitTime )
{
	//char szLimitDate[MAX_PATH] = "";
	//sprintf_s( szLimitDate,"\'%d-%d-%d %d:%d:%d\'", rkLimitTime.GetYear(), rkLimitTime.GetMonth(), rkLimitTime.GetDay(), rkLimitTime.GetHour(), 
	//											 rkLimitTime.GetMinute(), rkLimitTime.GetSecond() );
	CTime tempTime = rkLimitTime;

	SYSTEMTIME sysTime;
	tempTime.GetAsSystemTime( sysTime );

	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_sbox_add %d, %d, %d, %d, %d, %d, '%s', %s", 
	// dwUserIndex, iPresentType, iPresentValue1, iPresentValue2, iSubscriptionGold
	// iSubscriptionState, szSubscriptionID, szLimitDate );
	
	const int queryId = 173;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( iPresentType );
	v_FT.Write( iPresentValue1 );
	v_FT.Write( iPresentValue2 );
	v_FT.Write( iSubscriptionGold );
	v_FT.Write( iBonusCash );
	v_FT.Write( iSubscriptionState );
	v_FT.Write( szSubscriptionID.c_str(), szSubscriptionID.Length(), TRUE );
	v_FT.Write( (uint8*)&sysTime, sizeof(sysTime), TRUE );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY,
		DBAGENT_SUBSCRIPTION_DATA_SET, 
		_INSERTDB, 
		queryId, 
		v_FT, 
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnInsertSubscriptionData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectSubscriptionData( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, DWORD dwDBIndex, DWORD dwSelectCount )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_sbox_get_list %d, %d, %d", dwDBIndex, dwUserIndex, dwSelectCount );

	const int queryId = 175;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwDBIndex );
	v_FT.Write( dwUserIndex );
	v_FT.Write( dwSelectCount );

	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//상품 인덱스
	v_VT.push_back( GetValueType( vSHORT, sizeof(short) ) );			//상품 타입
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//상품 변수1
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//상품 변수2
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//상품 구매가격
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//상품 구매 보너스 캐쉬 사용 액.
	v_VT.push_back( GetValueType( vSHORT, sizeof(short) ) );			//상품 상태
	v_VT.push_back( GetValueType( vChar, 64 ) );						//상품 거래번호
	v_VT.push_back( GetValueType( vTimeStamp, sizeof(DBTIMESTAMP) ) );	//상품 만료 시간

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_SUBSCRIPTION_DATA_GET,
		_SELECTEX1DB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSelectSubscriptionData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSubscriptionDataDelete( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwDBIndex )
{
	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_sbox_delete %d", dwDBIndex );

	const int queryId = 176;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwDBIndex );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_SUBSCRIPTION_DATA_DEL,
		_DELETEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSubscriptionDataDelete Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSubseriptionDataUpdate( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwDBIndex,
										 short iPresentType, int iValue1, int iValue2,
										 const ioHashString &szSubscriptionID, int iSubscriptionGold, int iBonusCash, short iSubscriptionState, CTime &rkLimitTime )
{
	//char szLimitDate[MAX_PATH] = "";
	//sprintf_s( szLimitDate,"\'%d-%d-%d %d:%d:%d\'", rkLimitTime.GetYear(), rkLimitTime.GetMonth(), rkLimitTime.GetDay(), rkLimitTime.GetHour(), 
	//		

	//char szQuery[MAX_QUERY_SIZE] = "";
	//sprintf_s( szQuery, "exec game_sbox_save %d, %d, %d, %d, %d, %d, '%s', %s", dwDBIndex, iPresentType, iValue1, iValue2,
	//																iSubscriptionGold, iSubscriptionState, szSubscriptionID, szLimitDate );

	CTime tempTime = rkLimitTime;

	SYSTEMTIME sysTime;
	tempTime.GetAsSystemTime( sysTime );

	const int queryId = 174;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwDBIndex );
	v_FT.Write( iPresentType );
	v_FT.Write( iValue1 );
	v_FT.Write( iValue2 );
	v_FT.Write( iSubscriptionGold );
	v_FT.Write( iBonusCash );
	v_FT.Write( iSubscriptionState );
	v_FT.Write( szSubscriptionID.c_str(), szSubscriptionID.Length(), TRUE );
	v_FT.Write( (uint8*)&sysTime, sizeof(sysTime), TRUE );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_SUBSCRIPTION_DATA_UPDATE,
		_UPDATEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnSubseriptionDataUpdate Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnInsertAttendanceRecord( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const CTime& tConnectTime )
{
	const int queryId = 2090;	// game_attendance_add INT

	SYSTEMTIME sysTime;
	tConnectTime.GetAsSystemTime( sysTime );
		
	cSerialize v_FT;	
	v_FT.Write( dwUserIndex );
	v_FT.Write( (uint8*)&sysTime, sizeof(sysTime), TRUE );

	vVALUETYPE v_VT;

	CQueryData query_data;
	query_data.SetData( dwThreadID, _RESULT_DESTROY, DBAGENT_ATTENDANCE_ADD, _INSERTDB, queryId, v_FT, v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Send Fail! :%d - %d", __FUNCTION__, GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectAttendanceRecord( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex )
{
	const int queryId = 2091;	// game_attendance_list INT
	
	cSerialize v_FT;
	v_FT.Write( dwUserIndex );

	vVALUETYPE v_VT;
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//테이블 인덱스
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//유저인덱스
	v_VT.push_back( GetValueType( vTimeStamp, sizeof(DBTIMESTAMP) ) );	//저장 시간

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );

	query_data.SetData( dwThreadID, _RESULT_CHECK, DBAGENT_ATTENDANCE_LIST_GET, _SELECTDB, queryId, v_FT, v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Send Fail! :%d - %d", __FUNCTION__, GetLastError(), queryId );
		return;
	}
}

void DBClient::OnDeleteAttendanceRecord( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex )
{
	const int queryId = 2092;	// game_attendance_delete INT

	cSerialize v_FT;	
	v_FT.Write( dwUserIndex );

	vVALUETYPE v_VT;

	CQueryData query_data;
	query_data.SetData( dwThreadID, _RESULT_DESTROY, DBAGENT_ATTENDANCE_DELETE, _DELETEDB, queryId, v_FT, v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Send Fail! :%d - %d", __FUNCTION__, GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdatePetData(  const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szID, DWORD dwInvenIdx, cSerialize& v_FT )
{
	const int queryId = 180; 

	vVALUETYPE v_VT;

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_PET_DATA_UPD,
		_UPDATEDB, 
		queryId,
		v_FT,
		v_VT );

	
	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdatePeData Send Fail! :%s - %d - %d", szID.c_str(), GetLastError(), queryId );
		return;
	}
}

bool DBClient::OnInsertPetData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szID ,cSerialize& v_FT, CQueryData &query_data )
{
	const int queryId = 178;

	vVALUETYPE v_VT;
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//생성된 펫 인덱스
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//생성된 펫 코드
	v_VT.push_back( GetValueType( vChar, sizeof(char) ) );		//생성된 펫 랭크
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//생성된 펫 레벨
#if defined( DIVIDE_PET ) 
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//생성된 펫 착용 영웅
#endif
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_PET_DATA_SET,
		_UPDATEDB, 
		queryId,
		v_FT,
		v_VT );

	
	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_bool( kPacket.Write(query_data) );

	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnDeletePetData Send Fail! :%s - %d - %d", szID.c_str(), GetLastError(), queryId );
		return false;
	}
	return true;
}

bool DBClient::OnDeletePetData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szID, DWORD dwInvenIdx, cSerialize& v_FT )
{
	const int queryId = 179;

	vVALUETYPE v_VT;

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_PET_DATA_UPD,
		_UPDATEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_bool( kPacket.Write(query_data) );

	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnDeletePetData Send Fail! :%s - %d - %d", szID.c_str(), GetLastError(), queryId );
		return false;
	}
	return true;
}

void DBClient::OnLoginSelectCostumeData( const int iPrecCostumeIndex, const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx )
{
	// game_costume_get_list 
	if( szID.IsEmpty() )	return;

	const int queryID = 181;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( iPrecCostumeIndex );
	v_FT.Write( dwUserIdx );
	v_FT.Write( DB_COSTUME_SELECT_COUNT );

	v_VT.push_back( GetValueType(vLONG, sizeof(LONG) ) );				//코스튬 테이블 인덱스
	v_VT.push_back( GetValueType(vLONG, sizeof(LONG) ) );				//코스튬 코드
	v_VT.push_back( GetValueType(vChar, sizeof(BYTE) ) );				//코스튬 기간 타입
	v_VT.push_back( GetValueType(vTimeStamp, sizeof(DBTIMESTAMP)) );	//사용가능 시간
	v_VT.push_back( GetValueType(vLONG, sizeof(LONG) ) );				//장착 용병 번호

	CQueryData query_data;
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	//query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwUserIdx, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_COSTUME_DATA_GET,
		_UPDATEDB,
		queryID,
		v_FT,
		v_VT );
	
	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnLoginSelectCostumeData Send Fail! :%d - %d", GetLastError(), queryID );
		return;
	}
}

void DBClient::OnInsertCostumeData(const DWORD dwAgentID, const DWORD  dwThreadID, const DWORD dwUserIndex, const DWORD dwCode, const BYTE byPeriodType, 
																			const SYSTEMTIME& sysTime, const BYTE byInsertType, int iValue1, int iValue2, int iValue3)
{
	//game_costume_add
	const int queryID = 182;

	cSerialize v_FT;
	vVALUETYPE v_VT;
	int iClassType = 0;
	
	v_FT.Write( dwUserIndex );
	v_FT.Write( dwCode );
	v_FT.Write( byPeriodType );
	v_FT.Write( (uint8*)(&sysTime), sizeof(SYSTEMTIME), TRUE );
	v_FT.Write( iClassType );

	v_VT.push_back( GetValueType(vLONG, sizeof(LONG) ) );				//코스튬 테이블 인덱스

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwCode, sizeof(DWORD) );
	query_data.SetReturnData( &byPeriodType, sizeof(BYTE) );
	query_data.SetReturnData( &sysTime, sizeof(SYSTEMTIME) );
	query_data.SetReturnData( &byInsertType, sizeof(BYTE) );		//1 : 구매 ,			2: 선물 받기
	query_data.SetReturnData( &iValue1, sizeof(int) );				//1 : 상품코드			2: 선물함index
	query_data.SetReturnData( &iValue2, sizeof(int) );				//1 : 기간(몇일짜리)	2: 슬롯index
	query_data.SetReturnData( &iValue3, sizeof(int) );				//1 : 구매 가격			2: 0

	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_COSTUME_DATA_INSERT,
		_UPDATEDB,
		queryID,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnLoginSelectCostumeData Send Fail! :%d - %d", GetLastError(), queryID );
		return;
	}
}

void DBClient::OnUpdateCostumeData(const DWORD dwAgentID, const DWORD  dwThreadID,const DWORD dwUserIndex, const DWORD dwCostumeIdx, const DWORD dwCode, 
																						const BYTE byPeriodType, const SYSTEMTIME& sysTime, const int iClassType)
{
	//game_costume_save
	const int queryID = 183;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwCostumeIdx );
	v_FT.Write( dwCode );
	v_FT.Write( byPeriodType );
	v_FT.Write( (uint8*)(&sysTime), sizeof(SYSTEMTIME), TRUE );
	v_FT.Write( iClassType );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_COSTUME_DATA_UPDATE,
		_UPDATEDB,
		queryID,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnLoginSelectCostumeData Send Fail! :%d - %d", GetLastError(), queryID );
		return;
	}
}

void DBClient::OnDeleteCostumeData(const DWORD dwAgentID, const DWORD  dwThreadID,const DWORD dwUserIndex, const DWORD dwCostumeIdx)
{
	//game_costume_delete
	const int queryID = 184;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwCostumeIdx );
	v_FT.Write( dwUserIndex );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_COSTUME_DATA_DELETE,
		_UPDATEDB,
		queryID,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnLoginSelectCostumeData Send Fail! :%d - %d", GetLastError(), queryID );
		return;
	}
}

void DBClient::OnLoginSelectAccessoryData( const int iPrecAccessoryIndex, const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx )
{
	// game_accessory_get_list 
	if( szID.IsEmpty() )	return;

	const int queryID = 217;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( iPrecAccessoryIndex );
	v_FT.Write( dwUserIdx );
	v_FT.Write( DB_ACCESSORY_SELECT_COUNT );

	v_VT.push_back( GetValueType(vLONG, sizeof(LONG) ) );				//악세사리 테이블 인덱스
	v_VT.push_back( GetValueType(vLONG, sizeof(LONG) ) );				//악세사리 코드
	v_VT.push_back( GetValueType(vChar, sizeof(BYTE) ) );				//악세사리 기간 타입
	v_VT.push_back( GetValueType(vTimeStamp, sizeof(DBTIMESTAMP)) );	//사용가능 시간
	v_VT.push_back( GetValueType(vLONG, sizeof(LONG) ) );				//악세사리 능력치
	v_VT.push_back( GetValueType(vLONG, sizeof(LONG) ) );				//장착 용병 번호
	v_VT.push_back( GetValueType(vLONG, sizeof(LONG) ) );				//악세사리 합성코드
	v_VT.push_back( GetValueType(vLONG, sizeof(LONG) ) );				//악세사리 합성 능력치

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIdx, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_ACCESSORY_DATA_GET,
		_UPDATEDB,
		queryID,
		v_FT,
		v_VT );
	
	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnLoginSelectAccessoryData Send Fail! :%d - %d", GetLastError(), queryID );
		return;
	}
}
void DBClient::OnUpdateAccessoryData(const DWORD dwAgentID, const DWORD  dwThreadID,const DWORD dwUserIndex, const DWORD dwAccessoryIdx, const DWORD dwCode, const BYTE byPeriodType, const SYSTEMTIME& sysTime, const int iClassType, const int iAccessoryValue, const int iComposeCode, const int iComposeValue)
{
	//game_accessory_set_save
	const int queryID = 218;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwAccessoryIdx );
	v_FT.Write( dwUserIndex );
	v_FT.Write( dwCode );
	v_FT.Write( byPeriodType );
	v_FT.Write( (uint8*)(&sysTime), sizeof(SYSTEMTIME), TRUE );
	v_FT.Write( iAccessoryValue );
	v_FT.Write( iClassType );
	v_FT.Write( iComposeCode );
	v_FT.Write( iComposeValue );
	
	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_ACCESSORY_DATA_UPDATE,
		_UPDATEDB,
		queryID,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdateAccessoryData Send Fail! :%d - %d", GetLastError(), queryID );
		return;
	}
}


void DBClient::OnDeleteAccessoryData(const DWORD dwAgentID, const DWORD  dwThreadID,const DWORD dwUserIndex, const DWORD dwAccessoryIdx)
{
	//game_accessory_set_delete INT INT
	const int queryID = 219;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwAccessoryIdx );
	v_FT.Write( dwUserIndex );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_ACCESSORY_DATA_DELETE,
		_UPDATEDB,
		queryID,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnDeleteAccessoryData Send Fail! :%d - %d", GetLastError(), queryID );
		return;
	}
}

void DBClient::OnInsertAccessoryData(const DWORD dwAgentID, const DWORD  dwThreadID,const DWORD dwUserIndex, const DWORD dwCode, const BYTE byPeriodType, const SYSTEMTIME& sysTime, const BYTE byInsertType, int iValue1, int iValue2, int iValue3 )
{
	//game_accessory_set_add
	const int queryID = 220;

	cSerialize v_FT;
	vVALUETYPE v_VT;
	int iClassType = 0;
	
	v_FT.Write( dwUserIndex );
	v_FT.Write( dwCode );
	v_FT.Write( byPeriodType );
	v_FT.Write( (uint8*)(&sysTime), sizeof(SYSTEMTIME), TRUE );
	v_FT.Write( iValue3 );
	v_FT.Write( iClassType );

	v_VT.push_back( GetValueType(vLONG, sizeof(LONG) ) );			//악세사리 테이블 인덱스

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwCode, sizeof(DWORD) );
	query_data.SetReturnData( &byPeriodType, sizeof(BYTE) );
	query_data.SetReturnData( &sysTime, sizeof(SYSTEMTIME) );
	query_data.SetReturnData( &byInsertType, sizeof(BYTE) );		//1 : 구매 ,			2: 선물 받기
	query_data.SetReturnData( &iValue1, sizeof(int) );				//1 : 상품코드			2: 선물함index
	query_data.SetReturnData( &iValue2, sizeof(int) );				//1 : 기간(몇일짜리)	2: 선물함 슬롯index
	query_data.SetReturnData( &iValue3, sizeof(int) );				//1 : 구매 가격			2: 악세사리 능력치

	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_ACCESSORY_DATA_INSERT,
		_UPDATEDB,
		queryID,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnInsertAccessoryData Send Fail! :%d - %d", GetLastError(), queryID );
		return;
	}
}

void DBClient::OnLoginSelectMissionData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx )
{
	//game_mission_get_data
	const int queryID = 185;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIdx );
	v_VT.push_back( GetValueType(vChar, sizeof(BYTE)) );				//Type
	v_VT.push_back( GetValueType(vLONG, sizeof(LONG) ) );				//Code
	v_VT.push_back( GetValueType(vLONG, sizeof(LONG) ) );				//Value
	v_VT.push_back( GetValueType(vChar, sizeof(BYTE)) );				//State

	CQueryData query_data;
	//query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	//query_data.SetReturnData( szID.c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwUserIdx, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_MISSION_DATA_GET,
		_UPDATEDB,
		queryID,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnLoginSelectCostumeData Send Fail! :%d - %d", GetLastError(), queryID );
		return;
	}
}

void DBClient::OnLoginSelectRollBookData( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex )
{
	//game_attend_get_data
	const int queryID = 191;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_VT.push_back( GetValueType(vLONG, sizeof(LONG)) );				//해당 출석부 누적 출석 수
	v_VT.push_back( GetValueType(vLONG, sizeof(LONG) ) );				//보상 테이블
	v_VT.push_back( GetValueType( vTimeStamp, sizeof(DBTIMESTAMP) ) );	//출석한 시간

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_ROLLBOOK_DATA_GET,
		_UPDATEDB,
		queryID,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnLoginSelectRollBookData Send Fail! :%d - %d", GetLastError(), queryID );
		return;
	}
}

void DBClient::OnUpdateMissionData(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, BYTE byMissionType, const DWORD dwMissionCode, const int iMissionValue, const BYTE byState)
{
	//game_mission_set_data
	const int queryID = 186;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	byMissionType += 1;	//DB에는 미션 타입이 1부터 저장.

	v_FT.Write( dwUserIndex );
	v_FT.Write( byMissionType );
	v_FT.Write( dwMissionCode );
	v_FT.Write( iMissionValue );
	v_FT.Write( byState );
	v_VT.push_back( GetValueType(vLONG, sizeof(LONG) ) );		//Return;

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_MISSION_DATA_UPDATE,
		_UPDATEDB,
		queryID,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnLoginSelectCostumeData Send Fail! :%d - %d", GetLastError(), queryID );
		return;
	}
}

void DBClient::OnInitMissionData(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, BYTE byMissionType)
{
	//game_mission_set_init
	const int queryID = 187;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	byMissionType += 1;	//DB에는 미션 타입이 1부터 저장.

	v_FT.Write( dwUserIndex );
	v_FT.Write( byMissionType );
	v_VT.push_back( GetValueType(vLONG, sizeof(LONG) ) );		//Return;

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_MISSION_DATA_INIT,
		_UPDATEDB,
		queryID,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnLoginSelectCostumeData Send Fail! :%d - %d", GetLastError(), queryID );
		return;
	}
}

void DBClient::OnUpdateRollBookData(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const int iTableIndex, const int iAttendCount)
{
	//game_attend_set_data
	const int queryID = 192;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( iAttendCount );
	v_FT.Write( iTableIndex );
	v_VT.push_back( GetValueType(vLONG, sizeof(LONG) ) );				//Return;
	v_VT.push_back( GetValueType( vTimeStamp, sizeof(DBTIMESTAMP) ) );	//출석한 시간

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( &iTableIndex, sizeof(DWORD) );
	query_data.SetReturnData( &iAttendCount, sizeof(DWORD) );

	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_ROLLBOOK_DATA_UPDATE,
		_UPDATEDB,
		queryID,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdateRollBookData Send Fail! :%d - %d", GetLastError(), queryID );
		return;
	}
}

void DBClient::OnSelectGuildAttendanceInfo(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwGuildIndex, const DWORD dwUserIndex, SYSTEMTIME& sSelectDate, int iSelectType)
{
	//game_guild_attend_get_list
	const int queryID = 196;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	CTime cGetTime(sSelectDate.wYear, sSelectDate.wMonth, sSelectDate.wDay, g_GuildRewardMgr.GetRenewalHour(), sSelectDate.wMinute, 0);
	DWORD dwGetTime = cGetTime.GetTime();
	
	v_FT.Write( (uint8*)&sSelectDate, sizeof(SYSTEMTIME), TRUE );
	v_FT.Write( dwGuildIndex );
	v_VT.push_back( GetValueType(vLONG, sizeof(LONG) ) );				//출석한 유저 인덱스;

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( &iSelectType, sizeof(int) );
	query_data.SetReturnData( &dwGetTime, sizeof(DWORD) );

	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_GUILD_ATTENDANCE_MEMBER_GET,
		_UPDATEDB,
		queryID,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdateRollBookData Send Fail! :%d - %d", GetLastError(), queryID );
		return;
	}
}

void DBClient::OnInsertUserGuildAttendanceInfo(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwGuildIndex, const DWORD dwUserIndex)
{
	//game_guild_attend_set_add
	const int queryID = 195;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwGuildIndex );
	v_FT.Write( dwUserIndex );
	v_VT.push_back( GetValueType(vLONG, sizeof(LONG) ) );				//Return;

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_GUILD_USER_ATTENDANCE_INFO_INSERT,
		_UPDATEDB,
		queryID,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdateRollBookData Send Fail! :%d - %d", GetLastError(), queryID );
		return;
	}
}

void DBClient::OnUpdateGuildAttendanceRewardDate(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex)
{
	//game_guild_reward_set_attend
	const int queryID = 193;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );

	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_GUILD_ATTEND_REWARD_UPDATE,
		_UPDATEDB,
		queryID,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][dbclient]Guild reward date update fail : [%d] [%d]", GetLastError(), queryID );
		return;
	}
}

void DBClient::OnUpdateGuildRankRewardDate(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex)
{
	//game_guild_reward_set_guildRank
	const int queryID = 194;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );

	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_GUILD_RANK_REWARD_UPDATE,
		_UPDATEDB,
		queryID,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][dbclient]Guild reward date update fail : [%d] [%d]", GetLastError(), queryID );
		return;
	}
}

void DBClient::OnSelectSpentCash( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIndex )
{
	if( dwUserIndex == 0 ) return;

	//char str_query[MAX_QUERY_SIZE] = "";
	//wsprintf(str_query,"exec game_spentGold_get_data '%d'", dwUserIndex );

	const int queryId = 2095;
	
	cSerialize v_FT;
	v_FT.Write( dwUserIndex );

	vVALUETYPE v_VT;
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));


	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetData( dwThreadID, _RESULT_CHECK, DBAGENT_GAME_SPENT_MONEY_GET, _SELECTDB, queryId, v_FT, v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Send Fail! :%d - %d", __FUNCTION__, GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectPopupIndex( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIndex )
{
	if( dwUserIndex == 0 ) return;

	//char str_query[MAX_QUERY_SIZE] = "";
	//wsprintf(str_query,"exec game_popStore_get_data '%d'", dwUserIndex );

	const int queryId = 2097;

	cSerialize v_FT;
	v_FT.Write( dwUserIndex );

	vVALUETYPE v_VT;
	v_VT.push_back(GetValueType(vLONG,sizeof(LONG)));

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetData( dwThreadID, _RESULT_CHECK, DBAGENT_GAME_POPUP_INDEX_GET, _SELECTEX1DB, queryId, v_FT, v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Send Fail! :%d - %d", __FUNCTION__, GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateSpentCash( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, int iCash )
{
	if( dwUserIndex == 0 ) return;

	const int queryId = 2096;
	//2096=game_spentGold_set_add 
	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( iCash );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_GAME_SPENT_MONEY_SET, 
		_INSERTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdateSpentCash Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnInsertPopupIndex( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, int iPopupIndex )
{
	if( dwUserIndex == 0 ) return;

	const int queryId = 2098;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( iPopupIndex );

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_GAME_POPUP_INDEX_SET, 
		_INSERTDB, 
		queryId,
		v_FT,
		v_VT );
	
	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnInsertPopupIndex Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnLoginSelectExtraItemData( const int iPrevExraIndex, const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIdx )
{
	const int queryId = 22; //game_item_extra_get_list

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( iPrevExraIndex );
	v_FT.Write( dwUserIdx );
	v_FT.Write( DB_EXTRAITEM_SELECT_COUNT );

	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//인벤 인덱스

	for(int i = 0;i < ioUserExtraItem::MAX_SLOT;i++)
	{
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//ItemCode
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//Reinforce
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//SlotIndex
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//TradeType
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//PeriodType
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//MaleCustom
		v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				//FemaleCustom
		v_VT.push_back( GetValueType( vSHORT, sizeof(short) ) );			//FailExp
		v_VT.push_back( GetValueType( vTimeStamp, sizeof(DBTIMESTAMP) ) );	//LimiteDate
	}

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIdx, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_EXTRAITEM_DATA_GET,
		_SELECTEX1DB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnLoginSelectAllExtraItemData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectUserChannelingKeyValue( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szPrivateID, const ioHashString &szUserGUID, const int iChannelingType  )
{
	//char str_query[MAX_QUERY_SIZE] = "";
	//sprintf_s( str_query,"exec game_channel_get_cp_userID '%s'", szPrivateID.c_str() );

	const int queryId = 198;	//변경 필요

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( szPrivateID.c_str(), szPrivateID.Length(), TRUE );
	v_VT.push_back( GetValueType( vChar, CHANNELING_KEY_VALUE_PLUS_ONE ) );  // 주민번호 앞자리

	CQueryData query_data;
	query_data.SetReturnData( szUserGUID.c_str(), USER_GUID_NUM_PLUS_ONE );		
	query_data.SetReturnData( szPrivateID.c_str(), ID_NUM_PLUS_ONE );	
	query_data.SetReturnData( &iChannelingType, sizeof(int) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_USER_CHANNELING_KEY_VALUE_GET, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectUserBirthDate Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectGuildBlocksInfos(const DWORD dwUserIndex, const DWORD dwGuildIndex, const DWORD dwRoomIndex, int iPage)
{
	//[game_guildHQ_get_data]
	const int queryId = 199;	//변경 필요

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwGuildIndex );
	v_FT.Write( (BYTE)1 );
	v_FT.Write( iPage );
	v_FT.Write( DB_BUILT_BLOCK_ITEM_SELECT_COUNT );
	
	v_VT.push_back( GetValueType(vINT64,sizeof(__int64)) );         //index
	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) );             //code
	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) );             //XZ
	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) );             //Y
	v_VT.push_back( GetValueType(vChar, sizeof(char)) );			//direction
	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) );             //Score

	CQueryData query_data;

	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwGuildIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwRoomIndex, sizeof(DWORD) );
	query_data.SetReturnData( &iPage, sizeof(int) );

	query_data.SetData( 
		dwGuildIndex, 
		_RESULT_CHECK, 
		DBAGENT_GUILD_BLOCK_INFOS_GET, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( 0, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectGuildBlocksInfos Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnRetrieveOrDeleteBlock(const DWORD dwGuildIndex, const DWORD dwRoomIndex, const __int64 dwItemIndex, const BYTE byState, const ioHashString &szPublicID, const DWORD dwUserIndex, const DWORD dwItemCode)
{
	//[game_guildHQ_set_status]

	const int queryId = 200;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	BYTE byInfo	= byState;
	if( GBT_EXCEPTION == byState )
		byInfo	= GBT_RETRIEVE;

	v_FT.Write( dwGuildIndex );
	v_FT.Write( dwUserIndex );
	v_FT.Write( dwItemIndex );
	v_FT.Write( dwItemCode );
	v_FT.Write( byInfo );

	CQueryData query_data;

	query_data.SetReturnData( &dwGuildIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwRoomIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwItemIndex, sizeof(__int64) );
	query_data.SetReturnData( &byState, sizeof(BYTE) );
	query_data.SetReturnData( szPublicID.c_str(), ID_NUM_PLUS_ONE );

	query_data.SetData( 
		dwGuildIndex, 
		_RESULT_CHECK, 
		DBAGENT_GUILD_BLOCK_RETRIEVE_DELETE, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( 0, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectGuildBlocksInfos Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnConstructOrMoveBlock(const DWORD dwGuildIndex, const DWORD dwRoomIndex, const ioHashString &szPublicID, const __int64 dwItemIndex, const DWORD dwItemCode, const BYTE byState, const int iXZ, const int iY, const BYTE byDirection, const DWORD dwUserIndex )
{
	//[[game_guildHQ_set_fit]]

	const int queryId = 201;	//변경 필요

	cSerialize v_FT;
	vVALUETYPE v_VT;

	BYTE byInfo	= byState;

	if( GBT_DEFAULT_CONSTRUCT == byInfo  )
		byInfo	= GBT_CONSTRUCT;

	v_FT.Write( dwGuildIndex );
	v_FT.Write( dwUserIndex );
	v_FT.Write( dwItemIndex );
	v_FT.Write( dwItemCode );
	v_FT.Write( iXZ );
	v_FT.Write( iY );
	v_FT.Write( byDirection );
	v_FT.Write( byInfo );
	
	if( GBT_CONSTRUCT == byState || GBT_DEFAULT_CONSTRUCT == byState )
		v_VT.push_back( GetValueType(vINT64, sizeof(__int64)) );	//설치된 아이템 index select 

	CQueryData query_data;

	query_data.SetReturnData( &dwGuildIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwRoomIndex, sizeof(DWORD) );
	query_data.SetReturnData( &byState, sizeof(BYTE) );
	query_data.SetReturnData( &dwItemIndex, sizeof(__int64) );
	query_data.SetReturnData( &dwItemCode, sizeof(DWORD) );
	query_data.SetReturnData( &iXZ, sizeof(int) );
	query_data.SetReturnData( &iY, sizeof(int) );
	query_data.SetReturnData( &byDirection, sizeof(BYTE) );
	query_data.SetReturnData( szPublicID.c_str(), ID_NUM_PLUS_ONE );

	query_data.SetData( 
		dwGuildIndex, 
		_RESULT_CHECK, 
		DBAGENT_GUILD_BLOCK_CONSTRUCT_MOVE, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( 0, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectGuildBlocksInfos Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectGuildInvenVersion(const DWORD dwUserIndex, const DWORD dwGuildIndex, const int iRequestType)
{
	//game_guildHQ_ver_get_info
	const int queryId = 202;	//변경 필요

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwGuildIndex );
	v_VT.push_back( GetValueType(vINT64, sizeof(__int64)) );	//version

	CQueryData query_data;

	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwGuildIndex, sizeof(DWORD) );
	query_data.SetReturnData( &iRequestType, sizeof(int) );

	query_data.SetData( 
		dwGuildIndex, 
		_RESULT_CHECK, 
		DBAGENT_GUILD_INVEN_VERSION_GET, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( 0, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectGuildInvenVersion Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectGuildInvenInfo(const DWORD dwUserIndex, const DWORD dwGuildIndex, const int iRequestType, const __int64 iRequestVer)
{
	//game_guildHQ_get_data
	const int queryId = 199;	//변경 필요

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwGuildIndex );
	v_FT.Write( (BYTE)0 );	//인벤 정보
	v_FT.Write( 0 );	//시작 페이지.
	v_FT.Write( DB_GUILD_IN_SELCET_COUNT );	//한번에 Select 수

	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) );             //아이템 코드
	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) );             //아이템 수량

	CQueryData query_data;

	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwGuildIndex, sizeof(DWORD) );
	query_data.SetReturnData( &iRequestType, sizeof(int) );
	query_data.SetReturnData( &iRequestVer, sizeof(__int64) );

	query_data.SetData( 
		dwGuildIndex, 
		_RESULT_CHECK, 
		DBAGENT_GUILD_INVEN_INFO_GET, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( 0, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectGuildBlocksInfos Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnAddGuildBlockItem(const DWORD dwUserIndex, const DWORD dwGuildIndex, const DWORD dwItemCode, const int iAddType)
{
	//game_guildHQ_set_add
	const int queryId = 203;	//변경 필요

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwGuildIndex );
	v_FT.Write( dwItemCode );	//인벤 정보

	CQueryData query_data;

	query_data.SetReturnData( &dwGuildIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwItemCode, sizeof(DWORD) );
	query_data.SetReturnData( &iAddType, sizeof(int) );

	query_data.SetData( 
		dwGuildIndex, 
		_RESULT_CHECK, 
		DBAGENT_GUILD_BLOCK_ADD, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( 0, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnAddGuildBlockItem Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnDefaultConstructGuildBlock(const DWORD dwGuildIndex, const DWORD dwItemCode, const int iXZ, const int iY, BYTE byDirection)
{
	//game_guildHQ_set_default_add
	const int queryId = 204;	//변경 필요

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwGuildIndex );
	v_FT.Write( dwItemCode );
	v_FT.Write( iXZ );
	v_FT.Write( iY );
	v_FT.Write( byDirection );

	CQueryData query_data;

	query_data.SetData( 
		dwGuildIndex, 
		_RESULT_CHECK, 
		DBAGENT_GUILD_BLOCK_DEFAULT_CONSTRUCT, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( 0, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnDefaultConstructGuildBlock Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnPersonalHQConstructBlock(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwRoomIndex, const ioHashString &szPublicID, const __int64 iItemIndex, const DWORD dwItemCode, const int iXZ, const int iY, const BYTE byDirection, const DWORD dwUserIndex )
{
	//game_personalHQ_set_fit
	const int queryId = 209;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	BYTE byInfo	= GBT_CONSTRUCT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( iItemIndex );
	v_FT.Write( dwItemCode );
	v_FT.Write( iXZ );
	v_FT.Write( iY );
	v_FT.Write( byDirection );
	v_FT.Write( byInfo );
	
	v_VT.push_back( GetValueType(vINT64, sizeof(__int64)) );	//설치된 아이템 index select 

	CQueryData query_data;

	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwRoomIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwItemCode, sizeof(DWORD) );
	query_data.SetReturnData( &iXZ, sizeof(int) );
	query_data.SetReturnData( &iY, sizeof(int) );
	query_data.SetReturnData( &byDirection, sizeof(BYTE) );
	query_data.SetReturnData( szPublicID.c_str(), ID_NUM_PLUS_ONE );

	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_PERSONAL_HQ_CONSTRUCT, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectGuildBlocksInfos Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnPersonalHQRetrieveBlock(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwRoomIndex, const __int64 dwItemIndex, const BYTE byState, const ioHashString &szPublicID, const DWORD dwUserIndex, const DWORD dwItemCode)
{
	//game_personalHQ_set_status
	const int queryId = 208;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	BYTE byInfo	= byState;
	if( GBT_EXCEPTION == byState )
		byInfo	= GBT_RETRIEVE;

	v_FT.Write( dwUserIndex );
	v_FT.Write( dwItemIndex );
	v_FT.Write( dwItemCode );
	v_FT.Write( byInfo );

	CQueryData query_data;

	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwRoomIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwItemIndex, sizeof(__int64) );
	query_data.SetReturnData( &dwItemCode, sizeof(DWORD) );
	query_data.SetReturnData( &byState, sizeof(BYTE) );
	query_data.SetReturnData( szPublicID.c_str(), ID_NUM_PLUS_ONE );

	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_PERSONAL_HQ_RETRIEVE, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectGuildBlocksInfos Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::IsExistPersonalHQInfo(const DWORD dwAgentID, const DWORD dwThreadID,const DWORD dwUserIndex, int iType, int iValue1, int iValue2)
{
	//game_personalHQ_get_user_info
	const int queryId = 207;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );

	v_VT.push_back( GetValueType(vLONG, sizeof(LONG)) );             //존재 여부

	CQueryData query_data;

	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwAgentID, sizeof(DWORD) );
	query_data.SetReturnData( &dwThreadID, sizeof(DWORD) );

	query_data.SetReturnData( &iType, sizeof(int) );
	query_data.SetReturnData( &iValue1, sizeof(int) );
	query_data.SetReturnData( &iValue2, sizeof(int) );

	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_PERSONAL_HQ_IS_EXIST, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB IsExistPersonalHQInfo Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnAddPersonalHQBlockItem(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwItemCode, const int iCount, BOOL bEnd)
{
	//game_personalHQ_set_add
	const int queryId = 206;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( dwItemCode );

	CQueryData query_data;

	query_data.SetReturnData( &dwUserIndex, sizeof(dwUserIndex) );
	query_data.SetReturnData( &dwItemCode, sizeof(dwItemCode) );
	query_data.SetReturnData( &iCount, sizeof(iCount) );
	query_data.SetReturnData( &bEnd, sizeof(bEnd) );

	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_PERSONAL_HQ_BLOCK_ADD, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnAddPersonalHQBlockItem Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectPersonalInvenInfo(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex)
{
	//game_personalHQ_get_data
	const int queryId = 205;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( (BYTE)0 );	//인벤 정보
	v_FT.Write( 0 );	//시작 페이지.
	v_FT.Write( DB_GUILD_IN_SELCET_COUNT );	//한번에 Select 수

	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) );             //아이템 코드
	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) );             //아이템 수량

	CQueryData query_data;

	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );

	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_PERSONAL_HQ_INVEN_INFO_GET, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectPersonalInvenInfo Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectPersonalBlocksInfo(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwRoomIndex, const DWORD dwUserIndex,  int iPage)
{
	//game_personalHQ_get_data
	const int queryId = 205;	//변경 필요

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( (BYTE)1 );
	v_FT.Write( iPage );
	v_FT.Write( DB_BUILT_BLOCK_ITEM_SELECT_COUNT );
	
	v_VT.push_back( GetValueType(vINT64,sizeof(__int64)) );         //index
	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) );             //code
	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) );             //XZ
	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) );             //Y
	v_VT.push_back( GetValueType(vChar, sizeof(char)) );			//direction
	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) );             //Score

	CQueryData query_data;

	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( &iPage, sizeof(int) );
	query_data.SetReturnData( &dwRoomIndex, sizeof(int) );
	query_data.SetReturnData( &dwThreadID, sizeof(int) );
	query_data.SetReturnData( &dwAgentID, sizeof(int) );

	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_PERSONAL_HQ_BLOCKS_INFO_GET, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectPersonalBlocksInfo Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnDefaultConstructPersonalBlock(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwItemCode, const int iXZ, const int iY, BYTE byDirection)
{
	//game_personalHQ_set_default_add
	const int queryId = 210;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write(dwUserIndex);
	v_FT.Write(dwItemCode);
	v_FT.Write(iXZ);
	v_FT.Write(iY);
	v_FT.Write(byDirection);

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_PERSONAL_HQ_DEFAULT_CONSTRUCT, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnDefaultConstructPersonalBlock Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectTimeCashTable(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex)
{
	const int queryId = 214;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write(dwUserIndex);
	
	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) );             //code
	v_VT.push_back( GetValueType(vTimeStamp,sizeof(DBTIMESTAMP)) ); //StartDate
	v_VT.push_back( GetValueType(vTimeStamp,sizeof(DBTIMESTAMP)) ); //EndDate
	v_VT.push_back( GetValueType(vTimeStamp,sizeof(DBTIMESTAMP)) ); //Recently receive date

	CQueryData query_data;

	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );

	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_TIME_CASH_TABLE_GET, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectTimeCashTable Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnInsertTimeCashTable(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const ioHashString& szGUID, const DWORD dwCode, const DWORD dwEndDate)
{
	//[game_timeCash_set_add]
	const int queryId = 216;

	CTime cEndDate(dwEndDate);

	SYSTEMTIME sysTime = {0,0,0,0,0,0,0,0};
	sysTime.wYear = cEndDate.GetYear();
	sysTime.wMonth = cEndDate.GetMonth();
	sysTime.wDay = cEndDate.GetDay();
	sysTime.wHour = cEndDate.GetHour();
	sysTime.wMinute = cEndDate.GetMinute();

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write(dwUserIndex);
	v_FT.Write(dwCode);
	v_FT.Write( (uint8*)(&sysTime), sizeof(SYSTEMTIME), TRUE );

	v_VT.push_back( GetValueType(vTimeStamp,sizeof(DBTIMESTAMP)) ); //Update Date
	v_VT.push_back( GetValueType(vTimeStamp,sizeof(DBTIMESTAMP)) ); //Over Date
	v_VT.push_back( GetValueType(vTimeStamp,sizeof(DBTIMESTAMP)) ); //Start Date

	CQueryData query_data;
	
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( szGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwCode, sizeof(DWORD) );
	query_data.SetReturnData( &dwEndDate, sizeof(DWORD) );

	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_TIME_CASH_TABLE_INSERT, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnInsertTimeCashTable Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateTimeCashTable(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const ioHashString& szGUID, const DWORD dwCode)
{
	//game_timeCash_set_cash (update)
	const int queryId = 215;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write(dwUserIndex);
	v_FT.Write(dwCode);

	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) );             //Result
	v_VT.push_back( GetValueType(vTimeStamp,sizeof(DBTIMESTAMP)) ); //Update Date

	CQueryData query_data;

	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( szGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwCode, sizeof(DWORD) );

	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_TIME_CASH_TABLE_UPDATE, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdateTimeCashTable Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectTitlesInfo(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex)
{
	//game_title_get_data
	const int queryId = 211;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write(dwUserIndex);
	
	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) );             //code
	v_VT.push_back( GetValueType(vINT64,sizeof(__int64)) );         //value
	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) );             //level
	v_VT.push_back( GetValueType(vChar,sizeof(char)) );             //premium
	v_VT.push_back( GetValueType(vChar, sizeof(char)) );			//equip
	v_VT.push_back( GetValueType(vChar, sizeof(char)) );			//status

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );

	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_TITLE_SELECT_TITLE, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectTitlesInfo Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateTitleStatus(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex)
{
	//game_title_set_status
	const int queryId = 212;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write(dwUserIndex);
	

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );

	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_TITLE_UPDATE_STATUS, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdateTitleStatus Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnInsertOrUpdateTitleInfo(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwCode, const __int64 iValue, const int iLevel, const BYTE byPremium, const BYTE byEquip, const BYTE byStatus, const BYTE byActionType)
{
	//game_title_set_add
	const int queryId = 213;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write(dwUserIndex);
	v_FT.Write(dwCode);
	v_FT.Write(iValue);
	v_FT.Write(iLevel);
	v_FT.Write(byPremium);
	v_FT.Write(byEquip);
	v_FT.Write(byStatus);

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(dwUserIndex) );
	query_data.SetReturnData( &byActionType, sizeof(byActionType) );
	query_data.SetReturnData( &dwCode, sizeof(dwCode) );
	query_data.SetReturnData( &iValue, sizeof(iValue) );
	query_data.SetReturnData( &iLevel, sizeof(iLevel) );
	query_data.SetReturnData( &byPremium, sizeof(byPremium) );
	query_data.SetReturnData( &byEquip, sizeof(byEquip) );
	query_data.SetReturnData( &byStatus, sizeof(byStatus) );

	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_TITLE_INSERT_OR_UPDATE, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnInsertOrUpdateTitleInfo Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnInsertBonusCash(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const int iAmount, CTime& cEndDate)
{
	//game_gold_set_add
	const int queryId = 221;

	DWORD dwEndDate	   = cEndDate.GetTime();

	SYSTEMTIME sysTime = {0,0,0,0,0,0,0,0};
	sysTime.wYear = cEndDate.GetYear();
	sysTime.wMonth = cEndDate.GetMonth();
	sysTime.wDay = cEndDate.GetDay();
	sysTime.wHour = cEndDate.GetHour();
	sysTime.wMinute = cEndDate.GetMinute();

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write(dwUserIndex);
	v_FT.Write(iAmount);
	v_FT.Write(iAmount);
	v_FT.Write((BYTE)1);
	v_FT.Write( (uint8*)(&sysTime), sizeof(SYSTEMTIME), TRUE );
	
	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) );             //index

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(dwUserIndex) );
	query_data.SetReturnData( &iAmount, sizeof(iAmount) );
	query_data.SetReturnData( &dwEndDate, sizeof(dwEndDate) );

	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_BONUS_CASH_INSERT, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnInsertBonusCash Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateBonusCash(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwCashIndex, const int iAmount, BonusCashUpdateType eType, int iBuyItemType, int iBuyValue1, int iBuyValue2, const ioHashString& szGUID)
{
	//game_gold_set_data
	const int queryId = 222;
	
	cSerialize v_FT;
	vVALUETYPE v_VT;
	BYTE byType	= eType;

	v_FT.Write(dwCashIndex);
	v_FT.Write(dwUserIndex);
	//v_FT.Write(byType);
	v_FT.Write(iAmount);
	
	v_VT.push_back( GetValueType(vLONG, sizeof(LONG)) );             //status
	v_VT.push_back( GetValueType(vLONG, sizeof(LONG)) );             //table index
	v_VT.push_back( GetValueType(vLONG, sizeof(LONG)) );             //table amount
	v_VT.push_back( GetValueType(vLONG, sizeof(LONG)) );			 //used amount

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(dwUserIndex) );
	query_data.SetReturnData( &byType, sizeof(byType) );
	query_data.SetReturnData( &dwCashIndex, sizeof(dwCashIndex) );
	query_data.SetReturnData( &iAmount, sizeof(iAmount) );
	query_data.SetReturnData( &iBuyItemType, sizeof(iBuyItemType) );
	query_data.SetReturnData( &iBuyValue1, sizeof(iBuyValue1) );
	query_data.SetReturnData( &iBuyValue2, sizeof(iBuyValue2) );
	query_data.SetReturnData( szGUID.c_str(), USER_GUID_NUM_PLUS_ONE );
	
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_BONUS_CASH_UPDATE, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdateBonusCash Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}


void DBClient::OnSelectBonusCash(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwSelectType)
{
	//game_gold_get_data
	const int queryId = 223;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write(dwUserIndex);
	v_FT.Write((BYTE)1);

	v_VT.push_back( GetValueType(vLONG, sizeof(LONG)) );             //table index
	v_VT.push_back( GetValueType(vLONG, sizeof(LONG)) );             //total amount
	v_VT.push_back( GetValueType(vLONG, sizeof(LONG)) );             //remaining amount
	v_VT.push_back( GetValueType(vTimeStamp,sizeof(DBTIMESTAMP)) );  //expiration Date
	
	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(dwUserIndex) );
	query_data.SetReturnData( &dwSelectType, sizeof(dwSelectType) );

	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_BONUS_CASH_SELECT, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectBonusCash Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectExpiredBonusCash(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex)
{
	//game_gold_get_used_list
	const int queryId = 225;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write(dwUserIndex);
	v_FT.Write((BYTE)1);
	v_FT.Write((short)30);

	v_VT.push_back( GetValueType(vLONG, sizeof(LONG)) );             //table index
	v_VT.push_back( GetValueType(vLONG, sizeof(LONG)) );             //total amount
	v_VT.push_back( GetValueType(vLONG, sizeof(LONG)) );             //remaining amount
	v_VT.push_back( GetValueType(vTimeStamp,sizeof(DBTIMESTAMP)) ); //expiration Date
	
	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(dwUserIndex) );

	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_EXPIRED_BONUS_CASH_SELECT, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectBonusCash Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnDeleteExpirationBonusCash(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex)
{
	//game_gold_set_delete
	const int queryId = 224;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write(dwUserIndex);
	v_FT.Write((BYTE)1);

	CQueryData query_data;

	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_BONUS_CASH_EXPIRATION_DELETE, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectBonusCash Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnLoginSelectUserCoin( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex )
{
	//2105=game_coin_get_data DATETIME INT8
	const int queryId = 2105;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write(dwUserIndex);

	v_VT.push_back( GetValueType( vTimeStamp, sizeof(DBTIMESTAMP) ) );	// 코인 지급시간
	v_VT.push_back( GetValueType(vChar, sizeof(char)) );			// 코인타입

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(dwUserIndex) );

	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_USERCOIN_SELECT, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectUserCoin Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}

}

void DBClient::OnInsertUserCoin( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const BYTE byCoinType )
{
	//2106=game_coin_set_add INT INT8
	const int queryId = 2106;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write(dwUserIndex);
	v_FT.Write(byCoinType);

	v_VT.push_back( GetValueType( vTimeStamp, sizeof(DBTIMESTAMP) ) );	// 코인 지급시간

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(dwUserIndex) );
	query_data.SetReturnData( &byCoinType, sizeof(byCoinType) );

	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_USERCOIN_INSERT, 
		_INSERTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnInsertUserCoin Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}

}

void DBClient::OnUpdateUserCoin( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const BYTE byCoinType )
{
	//2107=game_coin_set_updatedate INT INT8
	const int queryId = 2107;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write(dwUserIndex);
	v_FT.Write(byCoinType);

	v_VT.push_back( GetValueType( vTimeStamp, sizeof(DBTIMESTAMP) ) );	// 코인 지급시간

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(dwUserIndex) );
	query_data.SetReturnData( &byCoinType, sizeof(byCoinType) );

	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_USERCOIN_UPDATE, 
		_UPDATEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdateUserCoin Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}

}

// 2018-06-18 by bckim, 카드 짝 마추기 이벤트 추가
void DBClient::OnSelectCardMatchingData( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex )
{
	/*		// 2201=game_cardmatching_get_data INT
	CREATE PROCEDURE [dbo].[game_cardmatching_get_data]          
	@ACCOUNT_IDX INT          
	AS            
	SET NOCOUNT ON;          
	SET LOCK_TIMEOUT 10000;          

	SELECT [MissionType],[MissionMark1],[MissionMark2],[Mark1],[Mark2],[UpdateTime]
	FROM [dbo].[userCardMatchingDB] WITH (NOLOCK)          
	WHERE [AccountIDX] = @ACCOUNT_IDX   
	*/

	if( dwUserIndex == 0 ) 
		return;

	const int queryId = 2201;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );

	v_VT.push_back( GetValueType(vChar,sizeof(char)) );					// [MissionType]
	v_VT.push_back( GetValueType(vChar, sizeof(char)) );				// [MissionMark1]
	v_VT.push_back( GetValueType(vChar, sizeof(char)) );				// [MissionMark2]
	v_VT.push_back( GetValueType(vChar, sizeof(char)) );				// [Mark1]
	v_VT.push_back( GetValueType(vChar, sizeof(char)) );				// [Mark2]
	v_VT.push_back( GetValueType( vTimeStamp, sizeof(DBTIMESTAMP) ) );	// [UpdateTime]	

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(dwUserIndex) );

	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_CARD_MATCHING_DATA_SELECT, 
		_SELECTDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectCardMatchingData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[CARD_MATCHING] DB OnSelectCardMatchingData Send ClentIDX[%d]", dwUserIndex );

}

void DBClient::OnUpdateCardMatchingData( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const BYTE byUpdateType, const BYTE byCardMissionType , const BYTE byCardMissionMark1, const BYTE byCardMissionMark2, const BYTE byCardMark1, const BYTE byCardMark2 )
{

	// const BYTE byCardMissionType , const BYTE byCardMissionMark1, const BYTE byCardMissionMark2, const BYTE byCardMark1, const BYTE byCardMark2 );

	// 2202=game_cardmatching_update_data INT INT8 INT8 INT8 INT8 INT8
	if( dwUserIndex == 0 ) 
		return;

	const int queryId = 2202;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( byUpdateType );
	v_FT.Write( byCardMissionType );	
	v_FT.Write( byCardMissionMark1 );
	v_FT.Write( byCardMissionMark2 );
	v_FT.Write( byCardMark1 );
	v_FT.Write( byCardMark2 );
	
	v_VT.push_back( GetValueType(vChar,sizeof(char)) );		
	v_VT.push_back( GetValueType(vChar,sizeof(char)) );		
	v_VT.push_back( GetValueType(vChar,sizeof(char)) );
	v_VT.push_back( GetValueType(vChar,sizeof(char)) );
	v_VT.push_back( GetValueType(vChar,sizeof(char)) );
	v_VT.push_back( GetValueType(vChar,sizeof(char)) );
	
	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(dwUserIndex) );
	query_data.SetReturnData( &byUpdateType, sizeof(byUpdateType) );

	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_CARD_MATCHING_DATA_UPDATE, 
		_UPDATEDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( ! SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdateCardMatchingData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}
// End. 2018-06-18 by bckim, 카드 짝 마추기 이벤트 추가

// 2018-08-30 by bckim, 주사위 이벤트 추가  
// 뱀주사위 데이터 로드
void DBClient::OnSelectDiceGameData( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex )	
{
	// 2204=game_dice_game_get_data INT
	if( dwUserIndex == 0 ) 
		return;

	const int queryId = 2204;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	/*
	DICE_GAME_TRACE_DB = 5,
	DICE_GAME_REWARD_DB = 10,
	DICE_GAME_SLOT_COUNT = 90,		// 90개가 넘으면 안됨. 16 * DICE_GAME_TRACE_DB = 90
	*/ 
	
	v_VT.push_back( GetValueType(vLONG,sizeof(int)) );				// 포지션 
	for( int i = 0; i < DICE_GAME_TRACE_DB; ++i )	
		v_VT.push_back(GetValueType(vLONG,sizeof(int)));			// 트레이스 16*5

	v_VT.push_back( GetValueType(vChar, sizeof(char)) );			// 보드 인덱스 

	for( int i = 0; i < DICE_GAME_REWARD_DB; ++i )
		v_VT.push_back(GetValueType(vLONG,sizeof(int)));			// 보상 10 

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(dwUserIndex) );

	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_DICE_GAME_DATA_SELECT, 
		_SELECTDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectDiceGameData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

// 뱀주사위 업데이트
void DBClient::OnUpdateDiceGameData( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const int iPosition, const int iTraceInfo[DICE_GAME_TRACE_DB], const BYTE byBoradIndex ,const int iRewardIndex[DICE_GAME_REWARD_DB])
//void DBClient::OnUpdateDiceGameData( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const BYTE byOakStep, const BYTE byHole[OAK_BARREL_HOLE], const int iLimitSword )
{
	// 2205=game_dice_game_data_update INT INT INT INT INT INT INT INT8 INT INT INT INT INT INT INT INT INT INT
	if( dwUserIndex == 0 ) 
		return;

	const int queryId = 2205;
	
	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( iPosition );											// 오크통 진행 단계
	for( int i = 0; i < DICE_GAME_TRACE_DB; ++i )			
		v_FT.Write( iTraceInfo[i] );	
	v_FT.Write( byBoradIndex );		
	for( int i = 0; i < DICE_GAME_REWARD_DB; ++i )			
		v_FT.Write( iRewardIndex[i] );	

	
	v_VT.push_back( GetValueType(vLONG,sizeof(int)) );		
	
	for( int i = 0; i < DICE_GAME_TRACE_DB; ++i )			
		v_VT.push_back( GetValueType(vLONG,sizeof(int)) );			
	
	v_VT.push_back( GetValueType(vChar,sizeof(char)));		
	
	for( int i = 0; i < DICE_GAME_REWARD_DB; ++i )			
		v_VT.push_back( GetValueType(vLONG,sizeof(int)) );			


	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(dwUserIndex) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_DICE_GAME_DATA_UPDATE, 
		_UPDATEDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( ! SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdateDiceGameData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}
// End. 2018-08-30 by bckim, 주사위 이벤트 추가

// 로그인 시 받아오는 오크통 데이터
void DBClient::OnSelectOakBarrelData( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex )
{
	// 226=game_oakbarrel_get_data INT
	if( dwUserIndex == 0 ) 
		return;

	const int queryId = 226;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );

	v_VT.push_back( GetValueType(vChar,sizeof(char)) );					// 오크통 진행 단계
	for( int i = 0; i < OAK_BARREL_HOLE; ++i )
		v_VT.push_back( GetValueType(vChar, sizeof(char)) );			// 오크통 12개 구멍 상태
	v_VT.push_back( GetValueType( vTimeStamp, sizeof(DBTIMESTAMP) ) );	// 오크통 칼 일일 사용 한도 수량 초기화 시각
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				// 오크통 칼 일일 사용 한도 수량

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(dwUserIndex) );

	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_OAK_BARREL_DATA_SELECT, 
		_SELECTDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectOakBarrelData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}


// 오크통 데이터 업데이트
void DBClient::OnUpdateOakBarrelData( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const BYTE byUpdateType, const BYTE byOakStep, const BYTE byHole[OAK_BARREL_HOLE], const int iLimitSword )
{
	// 227=game_oakbarrel_data_update INT INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT8 INT
	if( dwUserIndex == 0 ) 
		return;

	const int queryId = 227;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( byOakStep );											// 오크통 진행 단계
	for( int i = 0; i < OAK_BARREL_HOLE; ++i )			
		v_FT.Write( byHole[i] );										// 오크통 12개 구멍 상태
	v_FT.Write( iLimitSword );											// 오크통 칼 일일 사용 한도 수량

	v_VT.push_back( GetValueType(vChar,sizeof(char)) );					// 오크통 진행 단계
	for( int i = 0; i < OAK_BARREL_HOLE; ++i )
		v_VT.push_back( GetValueType(vChar, sizeof(char)) );			// 오크통 12개 구멍 상태
	v_VT.push_back( GetValueType( vTimeStamp, sizeof(DBTIMESTAMP) ) );	// 오크통 칼 일일 사용 한도 수량 초기화 시각
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				// 오크통 칼 일일 사용 한도 수량

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(dwUserIndex) );
	query_data.SetReturnData( &byUpdateType, sizeof(byUpdateType) );

	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_OAK_BARREL_DATA_UPDATE, 
		_UPDATEDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( ! SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdateOakBarrelData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}


// 오크통 칼 일일 사용 한도 수량 초기화 시각 업데이트
void DBClient::OnUpdateOakBarrelTimeData( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex )
{
	// 228=game_oakbarrel_set_updatedate INT
	if( dwUserIndex == 0 ) 
		return;

	const int queryId = 228;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );

	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );					// 결과값 (성공:0 실패:-1)
	v_VT.push_back( GetValueType( vTimeStamp, sizeof(DBTIMESTAMP) ) );		// 오크통 칼 일일 사용 한도 수량 초기화 시각

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );

	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_OAK_BARREL_TIME_UPDATE, 
		_UPDATEDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( ! SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdateOakBarrelTimeData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}


// 오크통 칼 일일 사용 한도 수량 초기화 업데이트
void DBClient::OnUpdateOakBarrelLimitSwordInit( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const int iLmitSwordMax )
{
	// 229=game_oakbarrel_set_init INT INT
	const int queryId = 229;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( iLmitSwordMax );

	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );					// 초기화 수량

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(dwUserIndex) );

	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_OAK_BARREL_LIMIT_SWORD_INIT, 
		_UPDATEDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( ! SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdateOakBarrelLimitSwordInit Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectUserMatch( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex )
{
	// 230=game_matchmode_get_data INT
	if( dwUserIndex == 0 ) 
		return;

	const int queryId = 230;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	
	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) ); // 테이블이 존재하는 가?
	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) ); // 플레이카운트
	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) ); // MMR
	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) ); // 현재연승
	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) ); // 최대연승
	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) ); // 최대패배
	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) ); // 랭킹 MMR

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(dwUserIndex) );

	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_USER_MATCH_DATA_SELECT, 
		_SELECTDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectUserMatch Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateUserMatch( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const int iPlayCount, const int iMMR, const int iWinCnt, const int iMaxWinCnt, const int iMaxLoseCnt, const int iRankMMR )
{
	// 231=game_matchmode_set_data INT INT INT INT INT
	if( dwUserIndex == 0 ) 
		return;

	const int queryId = 231;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( iPlayCount );
	v_FT.Write( iMMR );
	v_FT.Write( iWinCnt );
	v_FT.Write( iMaxWinCnt );
	v_FT.Write( iMaxLoseCnt );
	v_FT.Write( iRankMMR );

	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) );

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( &iPlayCount, sizeof(int) );
	query_data.SetReturnData( &iMMR, sizeof(int) );
	query_data.SetReturnData( &iWinCnt, sizeof(int) );
	query_data.SetReturnData( &iMaxWinCnt, sizeof(int) );
	query_data.SetReturnData( &iMaxLoseCnt, sizeof(int) );
	query_data.SetReturnData( &iRankMMR, sizeof(int) );

	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_USER_MATCH_DATA_UPDATE, 
		_UPDATEDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( ! SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdateUserMatch Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectUserSpiritData( const DWORD dwAgentID, const DWORD dwThreadID, const int TableIndex, const DWORD dwUserIndex, const int iCount )
{
	if( dwUserIndex == 0 ) 
		return;

	// 2112=game_spirit_get_data INT INT INT
	const int queryId = 2112;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( TableIndex );
	v_FT.Write( dwUserIndex );
	v_FT.Write( iCount );

	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) ); // 테이블
	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) ); // 타입
	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) ); // 개수

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(dwUserIndex) );

	query_data.SetData( dwThreadID, _RESULT_CHECK, DBAGENT_USER_SPIRIT_SELECT, _SELECTDB, queryId, v_FT, v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectOakBarrelData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnInsertUserSpiritData( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const int iSpiritCode, const int iQuantity )
{
	// 2113=game_spirit_set_add INT INT INT
	if( dwUserIndex == 0 ) 
		return;

	const int queryId = 2113;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( iSpiritCode );
	v_FT.Write( iQuantity );

	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) );

	CQueryData query_data;
	query_data.SetData( dwThreadID, _RESULT_CHECK, DBAGENT_USER_SPIRIT_INSERT, _UPDATEDB, queryId, v_FT, v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( ! SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnInsertSpiritData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnUpdateUserSpiritData( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const int iSpiritCode, const int iQuantity )
{
	// 2114=game_spirit_set_data INT INT INT
	if( dwUserIndex == 0 ) 
		return;

	const int queryId = 2114;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( iSpiritCode );
	v_FT.Write( iQuantity );

	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) );

	CQueryData query_data;
	query_data.SetData( dwThreadID, _RESULT_CHECK, DBAGENT_USER_SPIRIT_UPDATE, _UPDATEDB, queryId, v_FT, v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( ! SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdateUserSpiritData Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

// 커스텀 메달
void DBClient::OnSelectCustomMedal( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex )
{
	// 236=game_custom_medal_get_data INT
	if( dwUserIndex == 0 ) 
		return;

	const int queryId = 236;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	
	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) ); // idx
	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) ); // Medalcode
	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) ); // BasicGrowth1
	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) ); // BasicGrowth2
	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) ); // BasicGrowth3
	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) ); // BasicGrowth4
	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) ); // SkillGrowth1
	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) ); // SkillGrowth2
	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) ); // SkillGrowth3
	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) ); // SkillGrowth4
	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) ); // SetCharacterNum
	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) ); // LimitType
	v_VT.push_back( GetValueType( vTimeStamp, sizeof(DBTIMESTAMP) ) );	//LimiteDate
	
	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(dwUserIndex) );

	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_USER_CUSTOM_MEDAL_SELECT, 
		_SELECTDB, 
		queryId, 
		v_FT, 
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectCustomMedal Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnInsertCustomMedal( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const int iItemCode, const int iGrowth1
	, const int iGrowth2, const int iGrowth3, const int iGrowth4, const int iGrowth5, const int iGrowth6, const int iGrowth7, const int iGrowth8,
	const int iClass, const int iLimitType,SYSTEMTIME sysTime, int iPresentIndex, int iSlotIndex )
{
	// 237=game_custom_medal_set_add  @ACCOUNT_IDX  INT INT INT INT INT INT INT INT INT INT INT INT DATETIME

	const int queryId = 237;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write(dwUserIndex);
	v_FT.Write(iItemCode);
	v_FT.Write(iGrowth1);
	v_FT.Write(iGrowth2);
	v_FT.Write(iGrowth3);
	v_FT.Write(iGrowth4);
	v_FT.Write(iGrowth5);
	v_FT.Write(iGrowth6);
	v_FT.Write(iGrowth7);
	v_FT.Write(iGrowth8);
	v_FT.Write(iClass);
	v_FT.Write(iLimitType);
	v_FT.Write( (uint8*)(&sysTime), sizeof(SYSTEMTIME), TRUE );

	v_VT.push_back( GetValueType(vLONG,sizeof(LONG)) ); // 커스텀 메달 인덱스 

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(dwUserIndex) );
	query_data.SetReturnData( &iItemCode, sizeof(iItemCode) );
	query_data.SetReturnData( &iPresentIndex, sizeof(iPresentIndex) );
	query_data.SetReturnData( &iSlotIndex, sizeof(iSlotIndex) );
	
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_USER_CUSTOM_MEDAL_INSERT, 
		_INSERTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnInsertCustomMedal Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}

}

void DBClient::OnUpdateCustomMedalState( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const int iItemCode, const int iItemIndex
	, const int iClass, const bool bState )
{
	// 238=game_custom_medal_set_charaternum INT INT INT
	const int queryId = 238;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( iItemIndex );
	v_FT.Write( dwUserIndex );
	v_FT.Write( iClass );
	
	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_USER_CUSTOM_MEDAL_STATUS_UPDATE, 
		_UPDATEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnDeleteCustomMedal Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnDeleteCustomMedal( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD iItemIndex )
{
	// 239=game_custom_medal_set_delete INT INT
	const int queryId = 239;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( iItemIndex );
	v_FT.Write( dwUserIndex );
	

	CQueryData query_data;
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_USER_CUSTOM_MEDAL_DELETE, 
		_DELETEDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnDeleteCustomMedal Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}

void DBClient::OnSelectTimeGate( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex )
{
	if( dwUserIndex == 0 ) 
		return;

	//240=game_timegate_get_data INT
	const int queryId = 240;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );

	v_VT.push_back( GetValueType(vTimeStamp,sizeof(DBTIMESTAMP)) );  // 마지막 오픈시간

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(dwUserIndex) );

	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_USER_TIME_GATE_SELECT, 
		_SELECTDB, 
		queryId, 
		v_FT, 
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnSelectTimeGate Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
	
}

void DBClient::OnUpdateTimeGate( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex )
{	
	if( dwUserIndex == 0 ) 
		return;

	//241=game_timegate_set_data INT 
	const int queryId = 241;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	
	v_VT.push_back( GetValueType(vTimeStamp,sizeof(DBTIMESTAMP)) );  // 마지막 오픈시간

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(dwUserIndex) );

	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_USER_TIME_GATE_UPDATE, 
		_SELECTDB, 
		queryId,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdateTimeGate Send Fail! :%d - %d", GetLastError(), queryId );
		return;
	}
}


void DBClient::OnLoginSelectPracticeData( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex )
{
	// game_practice_get_list
	const int iQueryID = 811;

	cSerialize v_FT;
	vVALUETYPE v_VT;
	
	v_FT.Write( dwUserIndex );
	
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//수련장 인덱스
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//수련장 등급
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//수련장 카운드
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//수련장 시간
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//수련장 랭킹
	
	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(int) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_PRACTICE_LIST_GET, 
		_SELECTEX1DB,
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnLoginSelectPracticeData Send Fail! :%d - %d", GetLastError(), iQueryID );
		return;
	}	
}
/*
void DBClient::OnLoginSelectPracticeData( const DWORD dwUserIndex )
{
	// game_practice_get_list
	const int iQueryID = 811;

	Reset(iQueryID);

	m_FT.Write( dwUserIndex );

	m_Query.SetReturnData( &dwUserIndex, sizeof(DWORD) );

	m_Query.SetData( 
		dwUserIndex, 
		DBAGENT_PRACTICE_LIST_GET,  
		iQueryID,
		m_FT );

	//SendMessage();
}

void DBClient::OnUpdatePracticeData( const DWORD dwUserIndex, const DWORD dwPracticeID, const DWORD dwPracticeGrade )
{
	// game_practice_set_data

	const int iQueryID = 812;

	Reset(iQueryID);

	m_FT.Write( dwUserIndex );
	m_FT.Write( dwPracticeID );
	m_FT.Write( dwPracticeGrade );

	m_Query.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	m_Query.SetReturnData( &dwPracticeID, sizeof(DWORD) );
	m_Query.SetReturnData( &dwPracticeGrade, sizeof(DWORD) );

	m_Query.SetData( 
		dwUserIndex, 
		DBAGENT_PRACTICE_SET_DATA,  
		iQueryID,
		m_FT );

	//SendMessage();
}
*/

bool DBClient::OnUpdatePracticeAdmission( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex
	, const DWORD dwPracticeID, const DWORD dwPracticeGrade, const DWORD dwPracticeCount, const DWORD dwPracticeTime, const DWORD dwPracticeRank, int iMoney )
{
	// game_practice_set_data

	const int iQueryID = 812;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Reset();
	v_FT.Write( dwUserIndex );
	v_FT.Write( dwPracticeID );
	v_FT.Write( dwPracticeGrade );
	v_FT.Write( dwPracticeCount );
	v_FT.Write( dwPracticeTime );
	v_FT.Write( dwPracticeRank );
	
	v_VT.push_back( GetValueType(vLONG, sizeof(LONG) ) );		//Return;	
	
	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(dwUserIndex) );
	query_data.SetReturnData( &dwPracticeID, sizeof(dwPracticeID) );
	query_data.SetReturnData( &dwPracticeGrade, sizeof(dwPracticeGrade) );
	query_data.SetReturnData( &dwPracticeCount, sizeof(dwPracticeCount) );
	query_data.SetReturnData( &dwPracticeTime, sizeof(dwPracticeTime) );
	query_data.SetReturnData( &dwPracticeRank, sizeof(dwPracticeRank) );
	query_data.SetReturnData( &iMoney, sizeof(iMoney) );

	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_PRACTICE_SET_ADMISSION,
		_SELECTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_bool( kPacket.Write(query_data) );

	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdatePracticeData Send Fail! : %d - %d", GetLastError(), iQueryID );
		return false;
	}
	return true;
}

bool DBClient::OnUpdatePracticeTime( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex
	, const DWORD dwPracticeID, const DWORD dwPracticeGrade, const DWORD dwCurrentGrade, const DWORD dwPracticeCount, const DWORD dwPracticeTime, const DWORD dwPracticeRank )
{
	// game_practice_set_data

	const int iQueryID = 813;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Reset();
	v_FT.Write( dwUserIndex );
	v_FT.Write( dwPracticeID );
	v_FT.Write( dwPracticeGrade );
	v_FT.Write( dwPracticeCount );
	v_FT.Write( dwPracticeTime );
	v_FT.Write( dwPracticeRank );
	
	v_VT.push_back( GetValueType(vLONG, sizeof(LONG) ) );		//Return;	
	
	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(dwUserIndex) );
	query_data.SetReturnData( &dwPracticeID, sizeof(dwPracticeID) );
	query_data.SetReturnData( &dwPracticeGrade, sizeof(dwPracticeGrade) );
	query_data.SetReturnData( &dwPracticeCount, sizeof(dwPracticeCount) );
	query_data.SetReturnData( &dwPracticeTime, sizeof(dwPracticeTime) );
	query_data.SetReturnData( &dwPracticeRank, sizeof(dwPracticeRank) );
	query_data.SetReturnData( &dwCurrentGrade, sizeof(dwCurrentGrade) );

	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_PRACTICE_SET_TIME,
		_SELECTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );

	PACKET_GUARD_bool( kPacket.Write(query_data) );

	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdatePracticeTime Send Fail! : %d - %d", GetLastError(), iQueryID );
		return false;
	}
	return true;
}


void DBClient::OnPracticeRankList( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex )
{
	// game_practice_set_data

	const int iQueryID = 815;
	
	cSerialize v_FT;
	vVALUETYPE v_VT;
	
	v_FT.Write( dwUserIndex );
	
	v_VT.push_back( GetValueType( vChar, TIMESTRING ) );        //시작시간
	v_VT.push_back( GetValueType( vChar, TIMESTRING ) );        //종료시간
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//수련장 인덱스
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//수련장 랭크
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//수련장 선물타입
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//수련장 선물아이디
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//수련장 선물갯수
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//수련장 선물타입
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//수련장 선물아이디
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//수련장 선물갯수
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//수련장 선물타입
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//수련장 선물아이디
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//수련장 선물갯수
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//수련장 선물타입
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//수련장 선물아이디
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//수련장 선물갯수
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//수련장 선물타입
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//수련장 선물아이디
	v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );		//수련장 선물갯수
	
	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(int) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_PRACTICE_RANK_LIST_GET, 
		_SELECTEX1DB,
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnLoginSelectPracticeData Send Fail! :%d - %d", GetLastError(), iQueryID );
		return;
	}	
}


void DBClient::OnInitPracticeData(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwPracticeID, const DWORD dwPracticeCount)
{
	//game_practice_Init_count
	const int queryID = 814;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( dwPracticeID );
	v_FT.Write( dwPracticeCount );

	v_VT.push_back( GetValueType(vLONG, sizeof(LONG) ) );		//Return;	

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_PRACTICE_INIT_DATA,
		_UPDATEDB,
		queryID,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnInitPracticeData Send Fail! :%d - %d", GetLastError(), queryID );
		return;
	}
}


void DBClient::OnPCRoomPlaytime_Add(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwPCBangNumber)
{
	//game_practice_Init_count
	const int queryID = 830;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( dwUserIndex );
	v_FT.Write( dwPCBangNumber );

	v_VT.push_back( GetValueType(vLONG, sizeof(LONG) ) );		//Return;	

	CQueryData query_data;
	query_data.SetReturnData( &dwUserIndex, sizeof(DWORD) );
	query_data.SetReturnData( &dwPCBangNumber, sizeof(DWORD) );
	query_data.SetData( 
		dwThreadID, 
		_RESULT_CHECK, 
		DBAGENT_PCBANG_PLAY_TIME,
		_UPDATEDB,
		queryID,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );
	if( !SendMessage( dwAgentID, kPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnInitPracticeData Send Fail! :%d - %d", GetLastError(), queryID );
		return;
	}
}

void DBClient::Reset(const int iQueryID)
{
	m_iQueryID = iQueryID;

	m_FT.Reset();
	m_Query.Clear();
}
