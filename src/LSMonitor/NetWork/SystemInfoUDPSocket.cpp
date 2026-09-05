#include "stdafx.h"
#include ".\systeminfoudpsocket.h"
#include "SystemInfoUDPMsg.h"

SystemInfoUDPSocket::SystemInfoUDPSocket(void)
{
}



void SystemInfoUDPSocket::EvaluateSocket( WPARAM wParam, LPARAM lParam )
{
	if (WSAGETSELECTEVENT(lParam) == FD_READ)
		UDPReceiveProcess(wParam);
}

bool SystemInfoUDPSocket::UDPReceiveProcess( WPARAM wParam )
{
	m_socket_object = wParam;
	CGenericRcvMsg       Generic;		// Define Generic Packet
	if (ReceiveLoop (wParam, Generic) == false)
	{
		LOG.PrintTimeAndLog(0, "SystemInfoUDPSocket : 연결이 끊어졌을 때 받음");
		return false;
	}

	switch (Generic.GetType())
	{
	case ANSWER_SYSTEM_INFO:	
		CANSWER_SYSTEM_INFO *msg1;
		msg1 = (CANSWER_SYSTEM_INFO *)&Generic;
		msg1->Process();
		break;
	case ANSWER_COPY:
		CANSWER_COPY *msg2;
		msg2 = ( CANSWER_COPY *) &Generic;
		msg2->Process();
		break;
	case ANSWER_DELETE:
		CANSWER_DELETE *msg3;
		msg3 = ( CANSWER_DELETE *) &Generic;
		msg3->Process();
		break;
	default:
		return false;
	}
	return true;
}
