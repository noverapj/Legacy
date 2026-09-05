#pragma once
#include "User.h"

class ioUserMatch
{
public:
	ioUserMatch();
	virtual ~ioUserMatch();

protected:

	User*	m_pUser;

	int		m_iPlayCount;
	int		m_iMMR;
	int		m_iWinCount;	// ¿¬½Â
	int		m_iMaxWinCount;	// ÃÖ´ë¿¬½Â
	int		m_iMaxLoseCount;
	CTime	m_ctMatchReqTime;

	int		m_iStartTier;
	int		m_iEndTier;
	int		m_iRankMMR;		// ·©Å·¿ë MMR

public:

	void	Initialize( User* pUser );
	void	SaveData();
	void	FillMoveData( SP2Packet &rkPacket );
	void	ApplyMoveData( SP2Packet &rkPacket );

	void	SetUserMatch( int iPlayCount, int iPoint, int iWinCount, int iMaxWinCount, int iMaxLoseCount, int iRankMMR );

	void	SendUserMatch();

	void	AddPlayCount()		{ m_iPlayCount++; }

	int		GetMMR()			{ return m_iMMR; }
	int		GetWinCount()		{ return m_iWinCount; }
	int		GetMaxWinCount()	{ return m_iMaxWinCount; }
	int		GetMaxLoseCount()	{ return m_iMaxLoseCount; }

	void	AddMatchPoint( int iPoint );
	void	DelMatchPoint( int iPoint );

	void	AddRankMatchPoint( int iPoint );
	void	DelRankMatchPoint( int iPoint );


	void	SetWin();
	void	SetLose();

	void	SetTier( int iStart, int iEnd );
	int		GetStartTier()	{ return m_iStartTier; }
	int		GetEndTier()	{ return m_iEndTier; }

	void	SetReqTime();
	DWORD	GetMatchDelayTime();
};