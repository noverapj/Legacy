

#include "../stdafx.h"
#include "../ioINILoader.h"
#include "ioSP2TCPSocket.h"
#include "SP2Packet.h"
#include "../LSLogClientDlg.h"
#include "Protocol.h"
#include "../Version.h"


ioSP2TCPSocket::ioSP2TCPSocket()
{
}

ioSP2TCPSocket::~ioSP2TCPSocket()
{
	SAFEDELETE( m_pNS );
}

void ioSP2TCPSocket::ConnectProcess()
{
	LOG.PrintTimeAndLog( 0, "Server Connect" );

	SAFEDELETE( m_pNS );
	ioINILoader kLoader( "LogClientInfo.ini" );
	kLoader.SetTitle( "security" );
	if( kLoader.LoadInt( "ON", 1 ) == 1 )
		SetNS( new ioTCPClientSecurity );

	SP2Packet kPacket(CTPK_CONNECT);
	kPacket << RIGHT_MSG_CHECK_VALUE;
	SendLoop(kPacket);
}

void ioSP2TCPSocket::DisconnectProcess()
{
	LOG.PrintTimeAndLog( 0, "ioSP2TCPSocket::DisconnectProcess - Disconnected Server" );		
	SAFEDELETE( m_pNS );

	CLSLogClientDlg *pDlg = (CLSLogClientDlg*)AfxGetApp()->GetMainWnd();
	if(pDlg)
	{
		CString szTitle;
		szTitle.Format("LSLogClient version:%d (REMOTE:%s:%d) %s", FILEVER, pDlg->GetIP().c_str(), pDlg->GetPort(), "Disconnect");
		pDlg->SetWindowText(szTitle);
	}
}

bool ioSP2TCPSocket::CheckNS( CPacket &rkPacket )
{
	if( m_pNS == NULL )
	{
		LOG.PrintTimeAndLog( 0, "ioSP2TCPSocket::CheckNS ( m_pNS == NULL )" );
		return true;
	}
	
	ioTCPClientSecurity *pCS = (ioTCPClientSecurity*)m_pNS;

	if( !pCS->IsCheckSum( rkPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "ioSP2TCPSocket::CheckNS Check Sum Fail!! [%d : %d]", rkPacket.GetPacketID(), rkPacket.GetCheckSum() );
		return false;
	}
	
	if( !pCS->CheckState( rkPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "ioSP2TCPSocket::CheckNSType:%d State Not Same Client:%d, Server:%d", 
								 rkPacket.GetPacketID(), rkPacket.GetState(), pCS->GetRcvState() );
		return false;
	}
	
	return true;
}

void ioSP2TCPSocket::ProcessPacket( CPacket &rkPacket )
{
	if(AfxGetApp())
	{
		CLSLogClientDlg *pWnd = (CLSLogClientDlg*)AfxGetApp()->GetMainWnd();
		if(pWnd)
			pWnd->ProcessPacket( (SP2Packet&)rkPacket );
	}
}
//////////////////////////////////////////////////////////////////////////
ioTCPClientSecurity::ioTCPClientSecurity()
{
	m_SndState.InitState();
	m_RcvState.InitState();
}

ioTCPClientSecurity::~ioTCPClientSecurity()
{

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

