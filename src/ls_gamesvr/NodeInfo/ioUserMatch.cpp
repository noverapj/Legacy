#include "stdafx.h"
#include "ioUserMatch.h"
#include "MatchManager.h"
#include "../DataBase/DBClient.h"
#include "../QueryData/QueryResultData.h"

ioUserMatch::ioUserMatch()
	: m_iMMR(0), m_iPlayCount(0), m_iWinCount(0), m_iMaxWinCount(0), m_ctMatchReqTime(0)
{
}

ioUserMatch::~ioUserMatch()
{
}

void ioUserMatch::Initialize( User* pUser )
{
	m_pUser = pUser;
	m_iPlayCount = 0;
	m_iMMR = 0;
	m_iWinCount = 0;
	m_iMaxWinCount = 0;
	m_iMaxLoseCount = 0;
	m_iStartTier = 0;
	m_iEndTier = 0;
	m_iRankMMR = 0;
}

void ioUserMatch::SetUserMatch( int iPlayCount, int iPoint, int iWinCount, int iMaxWinCount, int iMaxLoseCount, int iRankMMR )
{
	m_iPlayCount = iPlayCount;
	m_iMMR = iPoint;
	m_iWinCount = iWinCount;
	m_iMaxWinCount = iMaxWinCount;
	m_iMaxLoseCount = iMaxLoseCount;
	m_iRankMMR = iRankMMR;
}

void ioUserMatch::SaveData()
{
	if( m_pUser == NULL )
		return;

	if( !m_pUser->IsConnectProcessComplete() )
		return;

	g_DBClient.OnUpdateUserMatch( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex(), m_iPlayCount, m_iMMR, m_iWinCount, m_iMaxWinCount, m_iMaxLoseCount, m_iRankMMR );
}

void ioUserMatch::FillMoveData( SP2Packet &rkPacket )
{
	PACKET_GUARD_VOID(rkPacket.Write(m_iPlayCount));
	PACKET_GUARD_VOID(rkPacket.Write(m_iMMR));
	PACKET_GUARD_VOID(rkPacket.Write(m_iWinCount));
	PACKET_GUARD_VOID(rkPacket.Write(m_iMaxWinCount));
	PACKET_GUARD_VOID(rkPacket.Write(m_iMaxLoseCount));
	PACKET_GUARD_VOID(rkPacket.Write(m_iStartTier));
	PACKET_GUARD_VOID(rkPacket.Write(m_iEndTier));
	PACKET_GUARD_VOID(rkPacket.Write(m_iRankMMR));
}

void ioUserMatch::ApplyMoveData( SP2Packet &rkPacket )
{
	PACKET_GUARD_VOID( rkPacket.Read(m_iPlayCount) );
	PACKET_GUARD_VOID( rkPacket.Read(m_iMMR) );
	PACKET_GUARD_VOID( rkPacket.Read(m_iWinCount) );
	PACKET_GUARD_VOID( rkPacket.Read(m_iMaxWinCount) );
	PACKET_GUARD_VOID( rkPacket.Read(m_iMaxLoseCount) );
	PACKET_GUARD_VOID( rkPacket.Read(m_iStartTier) );
	PACKET_GUARD_VOID( rkPacket.Read(m_iEndTier) );
	PACKET_GUARD_VOID( rkPacket.Read(m_iRankMMR) );
}

void ioUserMatch::SendUserMatch()
{
	if( !m_pUser )
		return;

	SP2Packet kPacket( STPK_SUCCESSION_DATA );
	PACKET_GUARD_VOID( kPacket.Write(m_iMMR) );
	PACKET_GUARD_VOID( kPacket.Write(m_iWinCount) );
	PACKET_GUARD_VOID( kPacket.Write(m_iMaxWinCount) );
	m_pUser->SendMessage( kPacket );
}

void ioUserMatch::SetReqTime()
{
	m_ctMatchReqTime = CTime::GetCurrentTime();
}

DWORD ioUserMatch::GetMatchDelayTime()
{
	CTime ctCurTime = CTime::GetCurrentTime();
	CTimeSpan delayTime = ctCurTime - m_ctMatchReqTime;

	return delayTime.GetTotalSeconds();
}

void ioUserMatch::AddMatchPoint( int iPoint )
{
	m_iMMR += iPoint;
}

void ioUserMatch::DelMatchPoint( int iPoint )
{
	m_iMMR -= iPoint;

	if( g_MatchManager.CheckMinimumMMR() && m_iMMR < g_MatchManager.GetMinimumMMR() )
		m_iMMR = g_MatchManager.GetMinimumMMR();
}

void ioUserMatch::AddRankMatchPoint( int iPoint )
{
	m_iRankMMR += iPoint;
}

void ioUserMatch::DelRankMatchPoint( int iPoint )
{
	m_iRankMMR -= iPoint;

	if( g_MatchManager.CheckMinimumMMR() && m_iMMR < g_MatchManager.GetMinimumMMR() )
		m_iMMR = g_MatchManager.GetMinimumMMR();
}

void ioUserMatch::SetTier( int iStart, int iEnd )
{
	m_iStartTier = iStart;
	m_iEndTier = iEnd;
}

void ioUserMatch::SetWin()
{
	m_iWinCount++;
	m_iMaxWinCount = max( m_iMaxWinCount, m_iWinCount );
	m_iMaxLoseCount = 0;
}

void ioUserMatch::SetLose()
{
	m_iWinCount = 0;
	m_iMaxLoseCount++;
}