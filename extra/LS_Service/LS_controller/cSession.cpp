#include "stdafx.h"
#include "cSession.h"
#include "../include/IIocpContext.h"


void tinet_ntoa(uint32 IP, TCHAR* buffer)
{
	SOCKADDR_IN sockAddr;
	sockAddr.sin_addr.s_addr = IP;

#ifdef UNICODE
	char szIP[16];
	strcpy_s(szIP, sizeof(szIP), inet_ntoa(sockAddr.sin_addr));

	uint32 length = MultiByteToWideChar(CP_ACP, 0, szIP, sizeof(szIP)-1, NULL, NULL);
	MultiByteToWideChar(CP_ACP, 0, szIP, sizeof(szIP)-1, buffer, length);
	buffer[length] = NULL;
#else
	strcpy_s(buffer, sizeof(buffer), inet_ntoa(sockAddr.sin_addr));
#endif
}

void cSession::OnConnect()
{
	TCHAR szIP[64];
	tinet_ntoa(GetContext()->GetIP(), szIP);

	TransferEnd();
	Debug(_T("@家南 立加(%s)\n"), szIP);
}

void cSession::OnDisconnect()
{
	TransferEnd();
	Debug(_T("@家南 立加秦力\n"));
}

BOOL cSession::TransferBegin(const TCHAR* file, const uint32 length)
{
	if(!m_transfer)
	{
		m_transfer = TRUE;
		if(length > G_MAXFILESIZE)
			m_buffer.Resize(G_MAXFILESIZE*2);
		else
			m_buffer.Resize(length);

		m_transferLength = 0;
		ZeroMemory(m_file, sizeof(m_file));
		CopyMemory(m_file, file, sizeof(m_file));
		return TRUE;
	}
	return FALSE;
}

BOOL cSession::TransferDo(const uint8* buffer, const uint32 length)
{
	if(m_transfer)
	{
		m_transferLength += length;
		return m_buffer.Append(buffer, length);
	}
	return FALSE;
}

void cSession::TransferWrite()
{
	if(m_transfer)
	{
		m_buffer.Erase();
	}
}

void cSession::TransferEnd()
{
	m_transfer = FALSE;
	m_buffer.Erase();
	ZeroMemory(m_file, sizeof(m_file));
}

