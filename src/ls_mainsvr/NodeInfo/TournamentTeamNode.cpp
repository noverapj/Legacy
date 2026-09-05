#include "../stdafx.h"

#include "../EtcHelpFunc.h"
#include "../DataBase/DBClient.h"
#include "TournamentTeamNode.h"

TournamentTeamNode::TournamentTeamNode( DWORD dwTourIndex, DWORD dwTeamIndex, ioHashString szTeamName,DWORD dwOwnerIndex,int iLadderPoint ,BYTE CampPos, BYTE MaxPlayer ) :
m_dwTourIndex( dwTourIndex ),
m_dwTeamIndex( dwTeamIndex ),
m_szTeamName( szTeamName ),
m_dwOwnerIndex( dwOwnerIndex ),
m_Position( 0 ),
m_StartPosition( 0 ),
m_MaxPlayer( MaxPlayer ),
m_iCheerPoint( 0 ),
m_TourPos( 0 ),
m_iLadderPoint( iLadderPoint ),
m_CampPos( CampPos ),
m_bDropEntry( false )
{
	InitDataChange();
	LOG.PrintTimeAndLog( 0, "CreateTournamentTeam %d - %d - %s - %d - %d - %d - %d - %d - %d - %d", m_dwTourIndex, m_dwTeamIndex, m_szTeamName.c_str(),
							m_dwOwnerIndex, (int)m_Position, (int)m_MaxPlayer, m_iCheerPoint, (int)m_TourPos, m_iLadderPoint, (int)m_CampPos );
}

TournamentTeamNode::TournamentTeamNode( DWORD dwTourIndex, DWORD dwTeamIndex, ioHashString szTeamName,DWORD dwOwnerIndex,int iLadderPoint ,BYTE CampPos, BYTE MaxPlayer, int iCheerPoint, SHORT Position, SHORT StartPosition, BYTE TourPos ) :
m_dwTourIndex( dwTourIndex ),
m_dwTeamIndex( dwTeamIndex ),
m_szTeamName( szTeamName ),
m_dwOwnerIndex( dwOwnerIndex ),
m_Position( Position ),
m_StartPosition( StartPosition ),
m_MaxPlayer( MaxPlayer ),
m_iCheerPoint( iCheerPoint ),
m_TourPos( TourPos ),
m_iLadderPoint( iLadderPoint ),
m_CampPos( CampPos ),
m_bDropEntry( false )
{
	InitDataChange();
	LOG.PrintTimeAndLog( 0, "CreateTournamentTeam DB %d - %d - %s - %d - %d - %d - %d - %d - %d - %d - %d - %d - %d - %d", m_dwTourIndex, m_dwTeamIndex, m_szTeamName.c_str(),
							m_dwOwnerIndex, (int)m_Position, (int)m_MaxPlayer, m_iCheerPoint, (int)m_TourPos, m_iLadderPoint, (int)m_CampPos, m_iCheerPoint, (int)m_Position, (int)m_StartPosition, (int)m_TourPos );	
}

TournamentTeamNode::~TournamentTeamNode()
{

}

void TournamentTeamNode::InitDataChange()
{
	m_dwChangeTimer = 0;
	m_dwSaveTime    = 0;
}

void TournamentTeamNode::SetDataChange()
{
	if( m_dwChangeTimer == 0 || m_dwSaveTime == 0 )
	{
		m_dwChangeTimer = TIMEGETTIME();
		m_dwSaveTime    = ( rand() % 50000 ) + 10000;
	}
}

bool TournamentTeamNode::IsDataSave()
{
	if( m_dwSaveTime == 0 ) return false;
	if( m_dwChangeTimer == 0 ) return false;

	return true;
}

bool TournamentTeamNode::IsDataSaveTimeCheck()
{
	if( m_dwSaveTime == 0 ) return false;
	if( m_dwChangeTimer == 0 ) return false;

	DWORD dwGapTime = TIMEGETTIME() - m_dwChangeTimer;
	if( dwGapTime > m_dwSaveTime )
		return true;
	return false;
}

void TournamentTeamNode::FillTeamInfo( SP2Packet &rkPacket )
{
	rkPacket << GetTourIndex() << GetTeamIndex() << GetTeamName() << GetOwnerIndex() << GetPosition();
	rkPacket << GetMaxPlayer() << GetCheerPoint() << GetTourPos() << GetLadderPoint() << GetCampPos();
}

void TournamentTeamNode::SetLadderPointAdd( int iLadderPoint )
{
	if( iLadderPoint == 0 ) return;

	m_iLadderPoint = max( 0, m_iLadderPoint + iLadderPoint );	
	
	SetDataChange();
}

void TournamentTeamNode::SetTourPos( BYTE TourPos )
{
	m_TourPos = TourPos;

	SetDataChange();
}

void TournamentTeamNode::SetPosition( SHORT Position )
{
	m_Position = Position;

	SetDataChange();
}

void TournamentTeamNode::SetStartPosition( SHORT StartPosition )
{
	m_StartPosition = StartPosition;

	SetDataChange();
}

void TournamentTeamNode::SaveData()
{
	InitDataChange();
	g_DBClient.OnUpdateTournamentTeamPointSave( GetTeamIndex(), GetPosition(), GetStartPosition(), GetTourPos(), GetLadderPoint() );
	LOG.PrintTimeAndLog( 0, "TournamentTeamNode::SaveData : %d - %d - %s - %d - %d - %d - %d", GetTeamIndex(), GetOwnerIndex(), GetTeamName().c_str(), (int)GetPosition(), (int)GetStartPosition(), (int)GetTourPos(), GetLadderPoint() );
}