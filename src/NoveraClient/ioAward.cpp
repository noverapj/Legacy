
#include "stdafx.h"

#include "ioAward.h"

ioAward::ioAward()
{
	Initialize();	
}

ioAward::~ioAward()
{
	
}

void ioAward::Initialize()
{
	m_iAwardLevel = 0;
	m_iAwardExp   = 0;
}

void ioAward::ApplyAwardData( SP2Packet &rkPacket )
{
	Initialize();

	rkPacket >> m_iAwardLevel >> m_iAwardExp;
}

bool ioAward::AddAwardExp( int iExp )
{
	if( iExp == 0 ) return false;

	int iNextExp = g_LevelMgr.GetNextAwardupExp( GetAwardLevel() );
	m_iAwardExp += iExp;
	if( iNextExp <= m_iAwardExp )
	{
		m_iAwardLevel++;
		//
		int iRemainExp = m_iAwardExp - iNextExp;
		m_iAwardExp = 0;
		if( iRemainExp > 0 )
			AddAwardExp( iRemainExp );			
		return true;
	}
	return false;
}

int ioAward::GetAwardLevel()
{
	return m_iAwardLevel;
}

int ioAward::GetAwardExp()
{
	return m_iAwardExp;
}