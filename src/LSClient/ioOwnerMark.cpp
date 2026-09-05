#include "stdafx.h"

#include "ioOwnerMark.h"

ioOwnerMark::ioOwnerMark(void)
{
	m_pOwnerMark   = NULL;
	m_TeamType	   = TEAM_NONE;
	m_dwFrameTimer = FRAMEGETTIME();
	m_iFrame       = 0;
}

ioOwnerMark::~ioOwnerMark(void)
{
	ClearResource();
}

void ioOwnerMark::ClearResource()
{
	SAFEDELETE( m_pOwnerMark );
}

bool ioOwnerMark::IsResouceOK() const
{
	if( !m_pOwnerMark )
		return false;

	return true;
}

void ioOwnerMark::SetTeam( TeamType eTeam )
{
	if( m_TeamType == eTeam )	return;

	m_TeamType = eTeam;

	ClearResource();

	switch( eTeam )
	{
	case TEAM_BLUE:
		m_pOwnerMark		 = g_UIImageSetMgr.CreateImage( "BaseImage010", "Itsme_Blue" );
		break;
	case TEAM_RED:
		m_pOwnerMark		 = g_UIImageSetMgr.CreateImage( "BaseImage010", "Itsme_Red" );
		break;
	default:
		m_pOwnerMark		 = g_UIImageSetMgr.CreateImage( "BaseImage010", "Itsme_Blue" );
		break;
	}
}

void ioOwnerMark::Update()
{
	if( !IsResouceOK() )
		return;

	enum { DELAY_TIME = 50, };
	if( FRAMEGETTIME() - m_dwFrameTimer >= DELAY_TIME )
	{
		m_dwFrameTimer = FRAMEGETTIME();
		m_iFrame++;
		if( m_iFrame == MAX_FRAME )
			m_iFrame = 0;
	}
}

void ioOwnerMark::Render( float fXPos, float fYPos )
{
	if( !IsResouceOK() )	
		return;

	if( !COMPARE( m_iFrame, 0, MAX_FRAME ) )
		return;
	
	enum { FRAME_WIDTH = 36, FRAME_HEIGHT = 35, };
	int iWidthStart = m_iFrame*FRAME_WIDTH;
	m_pOwnerMark->RenderWidthCut( fXPos - iWidthStart, fYPos, iWidthStart, iWidthStart+FRAME_WIDTH  );
}