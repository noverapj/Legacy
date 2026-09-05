#include "../stdafx.h"
#include ".\ioclientsocket.h"
#include "SP2Packet.h"
#include "..\AutoUpgradeDlg.h"

ioClientSocket::ioClientSocket( DWORD dwProcID ) : ioClientTCPSocket()
{
	m_dwProcID = dwProcID;
}

ioClientSocket::~ioClientSocket()
{
	SAFEDELETE( m_pNS );
}

void ioClientSocket::ConnectProcess()
{
	LOG.PrintTimeAndLog( 0, "Server Connect" );

	SAFEDELETE( m_pNS );
	SetNS( new ioTCPClientSecurity );

	if(AfxGetApp())
	{
		CAutoUpgradeDlg* pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();
		if(pDlg)
		{
			switch(m_dwProcID)
			{
			case WM_CLIENTSOCKET:
				pDlg->SetLoginNetworkState( NS_CONNECTED );
				break;
			case WM_CLIENTOTPSOCKET:
				pDlg->SetOTPNetworkState( NS_CONNECTED );
				break;
			case WM_LOGINSOCKET:
				break;
			case WM_MIGRATIONSOCKET:
				pDlg->SetMigrationNetworkState( NS_CONNECTED );
				break;

			}
		}
	}
}

void ioClientSocket::DisconnectProcess()
{
	LOG.PrintTimeAndLog( 0, "ioSP2TCPSocket::DisconnectProcess - Disconnected Server" );
	SAFEDELETE( m_pNS );	
}

bool ioClientSocket::CheckNS( CPacket &rkPacket )
{
	if( m_pNS == NULL )
	{
		LOG.PrintTimeAndLog( 0, "ioFileSocket::CheckNS ( m_pNS == NULL )" );
		return true;
	}

	ioTCPClientSecurity *pCS = (ioTCPClientSecurity*)m_pNS;

	if( !pCS->IsCheckSum( rkPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "ioFileSocket::CheckNS Check Sum Fail!! [%d : %d]", rkPacket.GetPacketID(), rkPacket.GetCheckSum() );
		return false;
	}

	if( !pCS->CheckState( rkPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "ioFileSocket::CheckNSType:%d State Not Same Client:%d, Server:%d", 
			rkPacket.GetPacketID(), rkPacket.GetState(), pCS->GetRcvState() );
		return false;
	}

	return true;
}

void ioClientSocket::ProcessPacket( CPacket &rkPacket )
{
	if(AfxGetApp())
	{
		CAutoUpgradeDlg* pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();
		if(pDlg)
		{
			switch(m_dwProcID)
			{
			case WM_CLIENTSOCKET :
				pDlg->LoginDlgRecvPacket( (SP2Packet&)rkPacket );
				break;
			case WM_CLIENTOTPSOCKET :
				pDlg->OTPDlgRecvPacket( (SP2Packet&)rkPacket );
				break;
			case WM_LOGINSOCKET :
				pDlg->BalancerRecvPacket( (SP2Packet&)rkPacket );
				break;
			case WM_MIGRATIONSOCKET:
				pDlg->MigrationDlgRecvPacket( (SP2Packet&)rkPacket );
				break;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
ioTCPClientSecurity::ioTCPClientSecurity()
{
	InitState();
}

ioTCPClientSecurity::~ioTCPClientSecurity()
{

}

void ioTCPClientSecurity::InitState()
{
	m_SndState.InitState();
	m_RcvState.InitState();
}

// Packet CheckSum
void ioTCPClientSecurity::EncryptMsg( CPacket &rkPacket )
{
	// Check Sum
	DWORD dwResult = MakeDigest(  (BYTE*)rkPacket.GetBuffer(), rkPacket.GetBufferSize()  );
	rkPacket.SetCheckSum( dwResult );

	// Encryption
	BYTE *pSrc;
	pSrc = (BYTE*)rkPacket.GetBuffer() + PK_CKSUM_ADDR;
	Encrypt( pSrc, rkPacket.GetBufferSize() - PK_CKSUM_ADDR );		
}

void ioTCPClientSecurity::DecryptMsg( CPacket &rkPacket )
{
	// Decryption
	BYTE *pSrc;
	pSrc = (BYTE*)rkPacket.GetBuffer() + PK_CKSUM_ADDR;
	Decrypt( pSrc, rkPacket.GetBufferSize() - PK_CKSUM_ADDR );
}

bool ioTCPClientSecurity::IsCheckSum( CPacket &rkPacket )
{
	//Decrypt
	DecryptMsg( rkPacket );

	DWORD	dwTemp = rkPacket.GetCheckSum();   //임시 저장.
	rkPacket.SetCheckSum( 0 );

	DWORD dwResult = MakeDigest( (BYTE*)rkPacket.GetBuffer(), rkPacket.GetBufferSize() );

	rkPacket.SetCheckSum( dwTemp );

	// Check Sum
	return ( dwResult == rkPacket.GetCheckSum() );
}

// Packet Replay
int ioTCPClientSecurity::GetSndState()
{
	return m_SndState.GetState();
}

void ioTCPClientSecurity::UpdateSndState()
{
	m_SndState.UpdateState();
}

int ioTCPClientSecurity::GetRcvState()
{
	return m_RcvState.GetState();
}

void ioTCPClientSecurity::UpdateRcvState()
{
	m_RcvState.UpdateState();
}

bool ioTCPClientSecurity::CheckState( CPacket &rkPacket )
{
	if( GetRcvState() == MAGIC_TOKEN_FSM )
	{
		m_RcvState.SetState( rkPacket.GetState() );
		m_SndState.SetState( rkPacket.GetState() );		
		UpdateSndState();
	}
	else
	{
		if( GetRcvState() != rkPacket.GetState() )
			return false;
	}

	UpdateRcvState();	
	return true;
}

void ioTCPClientSecurity::PrepareMsg( CPacket &rkPacket )
{
	rkPacket.SetState( GetSndState() );

	EncryptMsg( rkPacket );

	UpdateSndState();
}

void ioTCPClientSecurity::CompletionMsg( CPacket &rkPacket )
{	
}