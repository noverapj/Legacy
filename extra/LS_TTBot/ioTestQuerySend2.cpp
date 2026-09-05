#include "StdAfx.h"
#include "Protocol.h"
#include "network/SP2Packet.h"
#include "ioTestQuerySend2.h"
#include "LS_TTBotDlg.h"

ioTestQuerySend2::ioTestQuerySend2(void)
{
	Init();
}


ioTestQuerySend2::~ioTestQuerySend2(void)
{
	Destroy();
}

void ioTestQuerySend2::Init()
{
	AddState(TESTSTATE_NONE, TESTSTATE_CONNECT);
	AddState(TESTSTATE_CONNECT, TESTSTATE_SEND);
	AddState(TESTSTATE_SEND, TESTSTATE_SEND);	
 
	//m_iDBAgentThreadID = 0;
}

void ioTestQuerySend2::Destroy()
{
	LOG("disconnected, %s", m_IP);
}

void ioTestQuerySend2::Run()
{
	int newState = GetNextState();
	switch(newState)
	{
	case TESTSTATE_CONNECT :
		if(DoConnect())
			LOG("connected, %s", m_IP);
		else
			LOG("connect failed");
		break;

	case TESTSTATE_SEND:
		{
			/////////////////////////////////////////////////////////////////////////
			// Test 2
			const int queryId = 2003;

			cSerialize v_FT;
			vVALUETYPE v_VT;

			v_FT.Write( (uint32)30 );
			v_FT.Write( (uint32) rand() % 35000 );

			// 기본 정보
			v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				 //길드 인덱스
			v_VT.push_back( GetValueType( vChar, GUILD_NAME_NUM_PLUS_ONE ) );    //길드 이름
			v_VT.push_back( GetValueType( vChar, GUILD_TITLE_NUMBER_PLUS_ONE ) );//길드 소개
			v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );               //길드 마크
			v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );               //길드 포인트
			v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );               //길드 인원 제한
			v_VT.push_back(GetValueType(vTimeStamp,sizeof(DBTIMESTAMP)));		 //길드 생성일
			v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );               //길드 현재 인원
			v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );               //길드 금일 획득 포인트
			v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );               //길드 레벨
			v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );               //길드 진영

			// 길드 전적
			v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) ); //승
			v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) ); //패
			v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) ); //킬
			v_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) ); //데스

			CQueryData query_data;
			query_data.SetData( 
				++m_iDBAgentThreadID, 
				_RESULT_CHECK, 
				DBAGENT_GUILD_INFO_GET, 
				_SELECTEX1DB,
				queryId, 
				v_FT,
				v_VT );

			SP2Packet kPacket( DTPK_QUERY );
			kPacket << query_data;
			for( int i = 0 ; i < 20 ; ++i )
			{
				m_socketTCP.SendPacket( kPacket );
			}
		}
		break;

	case TESTSTATE_DISCONNECT :
		if(DoDisconnect())
			LOG("disconnected, %s", m_IP);
		else
			LOG("disconnected failed");
		break;

	default:
		return;
	}


	m_current = newState;
}

ValueType ioTestQuerySend2::GetValueType(VariableType nType,int len)
{
	ValueType vt;
	vt.type = nType;
	vt.size = len;
	return vt;
}
