#include "StdAfx.h"
#include "Protocol.h"
#include "network/SP2Packet.h"
#include "ioTestQuerySend1.h"

#include "LS_TTBotDlg.h"

//extern int m_iDBAgentThreadID;

ioTestQuerySend1::ioTestQuerySend1(void)
{
	Init();
}


ioTestQuerySend1::~ioTestQuerySend1(void)
{
	Destroy();
}

void ioTestQuerySend1::Init()
{
	AddState(TESTSTATE_NONE, TESTSTATE_CONNECT);
	AddState(TESTSTATE_CONNECT, TESTSTATE_SEND);
	AddState(TESTSTATE_SEND, TESTSTATE_SEND);	
}

void ioTestQuerySend1::Destroy()
{
	LOG("disconnected, %s", m_IP);
}

void ioTestQuerySend1::Run()
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
			// Test 1
			for( int i = 0 ; i < 10 ; ++i )
			{
				// Query
				const int queryId = 2002;

				CQueryData query_data;
				cSerialize v_FT;
				vVALUETYPE v_VT;

				query_data.SetData( 
					++m_iDBAgentThreadID, 
					_RESULT_CHECK, 
					DBAGENT_TOTAL_REG_USER_SET, 
					_SELECTDB,
					queryId, 
					v_FT, 
					v_VT );

				SP2Packet kPacket( DTPK_QUERY );
				kPacket << query_data;

				m_socketTCP.SendPacket( kPacket );
			}
		}
		break;

	default:
		return;
	}


	m_current = newState;
}

ValueType ioTestQuerySend1::GetValueType(VariableType nType,int len)
{
	ValueType vt;
	vt.type = nType;
	vt.size = len;
	return vt;
}
