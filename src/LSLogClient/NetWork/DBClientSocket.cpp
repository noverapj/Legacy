#include "../stdafx.h"
#include "ioSP2TCPSocket.h"
#include ".\dbclientsocket.h"
#include "SP2Packet.h"
#include "../Util/cSerialize.h"
#include "../QueryData/QueryData.h"
#include "../QueryData/QueryResultData.h"
#include "../LSLogClientDlg.h"
#include <strsafe.h>

DBClientSocket::DBClientSocket()
{
}

DBClientSocket::~DBClientSocket()
{
	SAFEDELETE( m_pNS );
}

void DBClientSocket::ConnectProcess()
{
	LOG.PrintTimeAndLog( 0, "Server Connect" );

// DBAgent와 통신시에는 암호화 하지 않음.
/*	SAFEDELETE( m_pNS );
	ioINILoader kLoader( "LogClientInfo.ini" );
	kLoader.SetTitle( "security" );
	if( kLoader.LoadInt( "ON", 1 ) == 1 )
		SetNS( new ioTCPClientSecurity );
*/
}

void DBClientSocket::DisconnectProcess()
{
	LOG.PrintTimeAndLog( 0, "DBClientSocket::DisconnectProcess - Disconnected Server" );		
	SAFEDELETE( m_pNS );

}

bool DBClientSocket::CheckNS( CPacket &rkPacket )
{
	// DBAgent와 통신시에는 암호화 하지 않음.
	/*
	if( m_pNS == NULL )
	{
		LOG.PrintTimeAndLog( 0, "DBClientSocket::CheckNS ( m_pNS == NULL )" );
		return true;
	}

	ioTCPClientSecurity *pCS = (ioTCPClientSecurity*)m_pNS;

	if( !pCS->IsCheckSum( rkPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "DBClientSocket::CheckNS Check Sum Fail!! [%d : %d]", rkPacket.GetPacketID(), rkPacket.GetCheckSum() );
		return false;
	}

	if( !pCS->CheckState( rkPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "DBClientSocket::CheckNSType:%d State Not Same Client:%d, Server:%d", 
			rkPacket.GetPacketID(), rkPacket.GetState(), pCS->GetRcvState() );
		return false;
	}*/

	return true;
}

void DBClientSocket::ProcessPacket( CPacket &rkPacket )
{
	CQueryResultData query_data;
	SP2Packet &rkSP2Packet = (SP2Packet&)rkPacket;

	if(rkSP2Packet.GetPacketID() != DTPK_QUERY) return;
	
	rkSP2Packet>>query_data;
	switch(query_data.GetMsgType())
	{
	case DBAGENT_LOG_SET:
		OnResultInsertLog(&query_data);
		return;
	}

	return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DBClientSocket::ChangeForInsertDB( OUT CString &szConvert , int iMaxLength )
{
	// db에서 사용할 수 없는 글자를 변환
	szConvert.Replace("\'", "\"");
	szConvert.Replace("<", "&lt;");
	szConvert.Replace(">", "&gt;");
	szConvert.Replace("\r", "<br>");
	szConvert.Replace("\n", "<br>");

	// 글자 길이 체크
	if(iMaxLength < szConvert.GetLength())
		szConvert = szConvert.Left(szConvert.GetLength() - ( szConvert.GetLength() - iMaxLength ));
}

void DBClientSocket::OnInsertLog( CString szZoneName, CString szName,CString szContents, int iErrorNumber )
{
	if(szName.IsEmpty() || szContents.IsEmpty() || szZoneName.IsEmpty())
	{
		LOG.PrintTimeAndLog(0, "Error - OnInsertLog - empty");
		return;
	}

	ChangeForInsertDB(szZoneName, MAX_DB_LOG_ZONE_NAME_MINUS_ONE);
	ChangeForInsertDB(szName, MAX_DB_LOG_TYPE_MINUS_ONE);
	ChangeForInsertDB(szContents, MAX_DB_LOG_CONTENTS_MINUS_ONE);

	char szTitle[MAX_DB_LOG_TITLE]="";

	SYSTEMTIME st;
	GetLocalTime(&st);

	StringCbPrintf(szTitle, sizeof(szTitle), "%d_%s_%02d시%02d분%02d초", iErrorNumber, szName.GetBuffer(0), st.wHour, st.wMinute, st.wSecond);


	//static char str_query[MAX_DB_LOG_CONTENTS_MINUS_ONE + MAX_DB_LOG_TITLE + MAX_DB_LOG_TYPE_MINUS_ONE + MAX_PATH] = "";
	//memset(str_query, 0, sizeof(str_query));
	//StringCbPrintf(str_query, sizeof(str_query), "exec SC_ERROR_LOG_ADD '로사', '%s', '%d', '%s',"
	//	         , szZoneName.GetBuffer(0), iErrorNumber, szTitle);

	char szErrorNumber[MAX_PATH]="";
	StringCbPrintf( szErrorNumber, sizeof( szErrorNumber ), "[%d]", iErrorNumber );
	szContents.Insert(0, szErrorNumber);

	//StringCbCat(str_query, sizeof(str_query), "'");
	//StringCbCat(str_query, sizeof(str_query),  szContents.GetBuffer(0));
	//StringCbCat(str_query, sizeof(str_query),  "'");

	//CQueryData query_data;
	//query_data.SetData( 0, _RESULT_CHECK, DBAGENT_LOG_SET, _INSERTDB, str_query, (int)strlen(str_query), NULL, 0 );


	char servType[64] = "로사";
	char errNum[64] = {0};
	StringCbPrintf( errNum, sizeof(errNum), "%d", iErrorNumber );

	const int queryId = 1500;

	cSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Write( servType, strlen(servType), TRUE );
	v_FT.Write( szZoneName.GetBuffer(), szZoneName.GetLength(), TRUE );
	v_FT.Write( errNum, strlen(errNum), TRUE );
	v_FT.Write( szTitle, strlen(szTitle), TRUE );
	v_FT.Write( szContents.GetBuffer(), szContents.GetLength(), TRUE );

	CQueryData query_data;
	query_data.SetData( 
		0, 
		_RESULT_CHECK, 
		DBAGENT_LOG_SET, 
		_INSERTDB,
		queryId,
		v_FT,
		v_VT );

	SP2Packet sendPacket(DTPK_QUERY);
	sendPacket << query_data;
	if(!ioClientTCPSocket::SendLoop(sendPacket))
	{
		LOG.PrintTimeAndLog(0,"DB OnInsertLog Send Fail! :%d",GetLastError());
		return;
	}
}

void DBClientSocket::OnResultInsertLog(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog(0,"DB OnResultInsertLog Result FAILED! :%d",query_data->GetResultType());
	}
}