

#include "stdafx.h"

#include "ioGlobalTimer.h"

template<> ioGlobalTimer* Singleton< ioGlobalTimer >::ms_Singleton = 0;

ioGlobalTimer::ioGlobalTimer()
{
	m_server_time	    = 0;
	m_current_time	    = 0;
	m_send_time		    = 0;
	m_recv_time         = 0;
	m_server_to_client_ping_time = 120000;       // 2분간 서버로부터 Ping이 안오면 연결 종료.

	m_server_delay_ms  = 0;
	m_packet_lose_cnt  = 0;
	m_packet_lose_send = 0;
	m_packet_lose_recv = 0;

	m_average_error    = 0;
	m_total_send_index = 0;

	m_bWorkPing = false;
}

ioGlobalTimer::~ioGlobalTimer()
{
}

ioGlobalTimer& ioGlobalTimer::GetSingleton()
{
	return Singleton< ioGlobalTimer >::GetSingleton();
}

int ioGlobalTimer::SetGlobalTime( SP2Packet &rkPacket )
{
	int delay_ms = REALGETTIME() - m_send_time; //패킷 지연 MS.
	DWORD recv_gap = delay_ms / 2; //Send - Recv 예상 소요시간.

	m_server_delay_ms = delay_ms;
	if( recv_gap <= MAX_SYNC_MS )
	{
		DWORD global_time = 0;
		rkPacket >> global_time;

		m_average_error = GetGlobalTime();
		m_server_time  = global_time + recv_gap;
		m_current_time = REALGETTIME();

		m_average_error = GetGlobalTime() - m_average_error;
	}

	// 패킷 소실 횟수 체크
	m_packet_lose_recv++;

	if( m_packet_lose_recv != m_packet_lose_send )
	{
		m_packet_lose_cnt += m_packet_lose_send - m_packet_lose_recv;	
		m_packet_lose_send = m_packet_lose_recv = 0;
	}
	m_recv_time = REALGETTIME();

	if( Help::CheckEnableTimeGapUser() )
	{
		LOG.PrintTimeAndLog( 0, "TC - Recv Sync: %d, %d, %d", m_recv_time, m_packet_lose_cnt, recv_gap );
	}

	return delay_ms;
}

DWORD ioGlobalTimer::GetGlobalTime() const
{
	if( m_server_time != 0 )
		return m_server_time + ( REALGETTIME() - m_current_time );

	return 0;
}

DWORD ioGlobalTimer::CalcGlobalGapTime( DWORD dwTime ) const
{
	DWORD dwGlobalTime = GetGlobalTime();
	if( dwGlobalTime > dwTime )
		return dwGlobalTime - dwTime;

	return 0;
}

void ioGlobalTimer::Process()
{
	if( g_MyInfo.GetPublicID().IsEmpty() )	// Login OK
		return;

	// 최초 or 9초에 한번 동기화
	if( m_send_time != 0 )
	{
		DWORD dwSendTimeGap = REALGETTIME() - m_send_time;
		if( dwSendTimeGap < SYNCTIME_DELAY )
			return;

		// 서버로부터 받은 시간 체크
		if( m_recv_time != 0 )
		{
			// CPU나 로딩으로 인한 시간 딜레이는 빼준다.
			m_recv_time = min( REALGETTIME(), m_recv_time + ( dwSendTimeGap - SYNCTIME_DELAY ) );
		}
	}

	SP2Packet kPacket( CUPK_SYNCTIME );
	kPacket << g_MyInfo.GetPublicID();
	kPacket << GetGlobalTime();
	kPacket << m_total_send_index;
	kPacket << m_bWorkPing;
	P2PNetwork::SendToServer( kPacket );

	m_send_time = REALGETTIME();
	m_packet_lose_send++;
	m_total_send_index++;

	m_bWorkPing = false;

	if( Help::CheckEnableTimeGapUser() )
	{
		LOG.PrintTimeAndLog( 0, "TC - Send Sync: %d", m_send_time );
	}
}

bool ioGlobalTimer::CheckServerToClientPing()
{
	if( m_send_time == 0 ) return true;
	if( m_recv_time == 0 ) return true;	
	
	DWORD dwRecvTimeGap = REALGETTIME() - m_recv_time;
	if( dwRecvTimeGap > m_server_to_client_ping_time )
	{
		m_recv_time = 0;
		return false;     // TCP 연결 종료
	}
	return true;
}