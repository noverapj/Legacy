#include "stdafx.h"
#include "ioINILoaderAU.h"
#include "NetWork/ioClientSocket.h"
#include "NetWork/Protocol.h"
#include "AutoUpgradeDlg.h"
#include "ioBalancer.h"


ioBalancer::ioBalancer(void) : m_socket(NULL), m_state(BALANCER_STATE_NONE), m_serverId(0), m_serverPort(0)
{
	Init();
}

ioBalancer::~ioBalancer(void)
{
	Destroy();
}

void ioBalancer::Init()
{
	ZeroMemory(m_serverIP, sizeof(m_serverIP));
	ZeroMemory(m_INI, sizeof(m_INI));
}

void ioBalancer::Destroy()
{
	if(m_socket)
	{
		m_socket->CloseClientTCP();
		delete m_socket;
		m_socket = NULL;
	}
}

BOOL ioBalancer::LoadINI()
{
	CString szINIPath = "info\\";
	szINIPath += m_INI;

	ioINILoaderAU kLoader( (LPCTSTR)szINIPath );
	kLoader.SetTitle("login");

	ZeroMemory(m_serverIP, sizeof(m_serverIP));
	kLoader.LoadString("login_ip", "", m_serverIP, sizeof(m_serverIP));

	m_serverPort = kLoader.LoadInt("login_port", 0);
	return TRUE;
}

void ioBalancer::Begin(TCHAR* INI)
{
	ZeroMemory(m_INI, sizeof(m_INI));
	strcpy_s(m_INI, sizeof(m_INI), INI);

	Thread::Begin();
}

void ioBalancer::Run()
{
	__try
	{
		SetState(BALANCER_STATE_CONNECTING);

		LoadINI();

		Connect();

		while(TRUE)
		{
			Sleep(10000);
		}
	}
	__except(1)
	{
		SendLOG( _T("ioBalancer except"), 209);
	}
}

void ioBalancer::SendLOG(const char* log, int number)
{
	CAutoUpgradeDlg *pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();
	if(pDlg == NULL) return;

	pDlg->SendLOG(log, number);
}

LONG ioBalancer::Evaluate(WPARAM wParam, LPARAM lParam)
{
	if(m_socket)
	{
		return m_socket->EvaluateSocket(wParam, lParam);
	}
	return 0;
}

void ioBalancer::Connect()
{
	m_socket = new ioClientSocket( WM_LOGINSOCKET );
	if(m_socket->InitClientTCP( GetHandle(), m_socket->GetProcID(), m_serverIP, m_serverPort ))
	{
	}
	else
	{
		SetState(BALANCER_STATE_CONNECTFAILED);
	}

	// 게임서버 정보
	TCHAR loginId[128] = _T("youngdie");
	int port = 12345;

	SP2Packet kPacket( LSTPK_CONNECT_CLIENT ); // 게임서버 정보를 받아오는 패킷
	kPacket << loginId << port;

	int tryCount = 0;
	while(TRUE)
	{
		if(!m_socket->IsConnected())
		{
			Sleep(100);
			continue;
		}

		SetState(BALANCER_STATE_CONNECTED);
		m_socket->SendLoop( kPacket );
		break;
	}
}

void ioBalancer::Disconnect()
{
	if(m_socket)
	{
		m_socket->CloseClientTCP();
	}
}

void ioBalancer::GenerateID(TCHAR* IP, const int port)
{
	char copyIP[MAX_PATH]="";
	strcpy_s(copyIP, IP);

	unsigned char bytesArray[MAX_PATH];
	memset(bytesArray, 0 , MAX_PATH);

	int loopCount = 0;
	char *next_token = NULL;
	while (loopCount < 100)
	{
		char *pos = NULL;
		if( loopCount == 0)
			pos = strtok_s(copyIP, ".", &next_token);
		else
			pos = strtok_s(NULL, ".", &next_token);

		if(pos == NULL)
			break;

		// IP의 4개의 구역을 한바이트씩 분리해서 넣는다
		bytesArray[loopCount]=atoi(pos);

		loopCount++;
		if(loopCount >= sizeof(__int64))
			break;
	}

	m_serverId = 0;
	memcpy(&bytesArray[sizeof(int)], &port, sizeof(int));
	memcpy(&m_serverId, bytesArray, sizeof(__int64));
}

void ioBalancer::RecvPacket(SP2Packet &rkPacket)
{
	switch(rkPacket.GetPacketID())
	{
	case LSTPK_SVR_STATUS_RESPONSE : 
		{
			int serverPort = 0;
			TCHAR serverIP[64];

			ZeroMemory(serverIP, sizeof(serverIP));

			// 게임서버에서 받아온 서버와 아이피
			rkPacket >> serverIP >> serverPort;
			GenerateID(serverIP, serverPort);

			// 로그인서버로부터 응답
			SetState(BALANCER_STATE_SUCCESS);

			//LOG.PrintTimeAndLog(0, "ioBalancer::LSTPK_SVR_STATUS_RESPONSE (%s:%d)", serverIP, serverPort);
			LOG.PrintTimeAndLog(0, "Response login");
			// 연결종료
			Disconnect();
		}
		break;

	case LSPTK_LS_LOGINSVR_CONNECT :
		{
		}
		break;

	case LSPTK_TICKET_RESPONSE :
		{
			// 대기열의 경우 티켓번호를 부여받는다
			int ticket;
			rkPacket >> ticket;
		}
		break;

	case LSPTK_TIMEOUT_CLOSE_REQUEST :
		{
			// 타임아웃으로 인한 종료
			SetState(BALANCER_STATE_CLOSE);
		}
		break;

	case LSPTK_ERROR :
		{
			// 로그인 서버로부터의 에러
			int errCode = 0;
			rkPacket >> errCode;

			switch(errCode)
			{
			case LS_ERR_SERVERMAX:
				break;
			case LS_ERR_WAITLISTMAX:
				break;
			case LS_ERR_TIMEOUT:
				break;
			case LS_ERR_SERVERALLBLOCK:
				break;
			case LS_ERR_UNKNOWNTYPE:
				break;
			case LS_ERR_NOSERVER:
				break;
			}
			SetState(BALANCER_STATE_ERROR);
		}
		break;

	case LSPTK_PING : // 핑
		break;

	default:
		SetState(BALANCER_STATE_ERROR);
		break;
	}
}

void ioBalancer::GetADDR(std::string& serverId)
{
	char szBuffer[64];
	sprintf_s(szBuffer, "%I64d", m_serverId);
	serverId = szBuffer;
}
