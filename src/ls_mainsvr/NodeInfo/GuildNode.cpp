#include "../stdafx.h"
//#include "../Window.h"

#include "../QueryData/QueryResultData.h"
#include "../DataBase/DBClient.h"

#include "GuildNodeManager.h"
#include "GuildNode.h"

GuildNode::GuildNode()
{
	InitData();
}

GuildNode::~GuildNode()
{	
}

void GuildNode::InitData()
{
	m_dwGuildIndex	= 0;            //길드 고유 인덱스
	m_dwGuildMark	= 0;            //길드 마크 업데이트 인덱스
	m_dwGuildRank	= 0;            //길드 랭킹
	m_dwGuildRegDate= 0;            //길드 생성 일자
	m_dwGuildPoint	= 0;            //길드 포인트 GP
	m_dwCurGuildPoint= 0;            //금일 획득한 길드 포인트
	m_dwGuildJoinUser = 0;          //현재 길드 인원
	m_dwGuildLevel  = 0;			//길드 레벨
	m_szGuildName.Clear();			//길드 이름.
	m_szGuildTitle.Clear();			//길드 소개.
	m_Record.Init();                //길드 전적
	m_bSaveReg = false;             //길드 등록
}

void GuildNode::CreateGuild( SP2Packet &rkPacket, DWORD dwDefaultLevel )
{
	rkPacket >> m_dwGuildIndex >> m_szGuildName >> m_szGuildTitle >> m_dwGuildMark >> m_dwGuildMaxEntry >> m_dwGuildRegDate;

	// Default Setting
	m_dwGuildLevel     = dwDefaultLevel;
	m_dwGuildJoinUser  = 1;
}

BOOL GuildNode::CreateGuild( CQueryResultData *pQueryData )
{
	// 기본 정보
	if(!pQueryData->GetValue( m_dwGuildIndex ))					return FALSE;	//길드 인덱스
	if(!pQueryData->GetValue( m_szGuildName, GUILD_NAME_NUM_PLUS_ONE ))			return FALSE;	//길드 이름
	if(!pQueryData->GetValue( m_szGuildTitle, GUILD_TITLE_NUMBER_PLUS_ONE ))		return FALSE;	//길드 소개
	if(!pQueryData->GetValue( m_dwGuildMark ))					return FALSE;	//길드 마크
	if(!pQueryData->GetValue( m_dwGuildPoint ))					return FALSE;   //길드 포인트
	if(!pQueryData->GetValue( m_dwGuildMaxEntry ))				return FALSE;	//길드 인원 제한

	DBTIMESTAMP dts;
	if(!pQueryData->GetValue( (char*)&dts, sizeof(DBTIMESTAMP) ))				return FALSE;	//길드 생성일
	m_dwGuildRegDate = (dts.year * 10000) + (dts.month * 100) + dts.day;

	if(!pQueryData->GetValue( m_dwGuildJoinUser ))				return FALSE;	//길드 인원
	if(!pQueryData->GetValue( m_dwCurGuildPoint ))				return FALSE;	//금일 획득 포인트
	if(!pQueryData->GetValue( m_dwGuildLevel ))					return FALSE;	//길드 레벨

	// 전적 정보
	if(!pQueryData->GetValue( m_Record.m_iWin ))					return FALSE;
	if(!pQueryData->GetValue( m_Record.m_iLose ))				return FALSE;
	if(!pQueryData->GetValue( m_Record.m_iKill ))				return FALSE;
	if(!pQueryData->GetValue( m_Record.m_iDeath ))				return FALSE;
	return TRUE;
}

DWORD GuildNode::GetLimitMaxEntry()
{
	//return max( g_GuildNodeManager.GetDefaultGuildMaxEntry() + ( m_dwGuildLevel * g_GuildNodeManager.GetGuildLevelUPEntryGap() ), g_GuildNodeManager.GetDefaultGuildMaxEntry() );
	return g_GuildNodeManager.GetDefaultGuildMaxEntry();
}

void GuildNode::AddGuildRecord( int iWin, int iLose, int iKill, int iDeath )
{	
	m_Record.m_iWin += iWin;
	m_Record.m_iLose+= iLose;
	m_Record.m_iKill+= iKill;
	m_Record.m_iDeath+= iDeath;
	m_Record.m_bChange = true;
    g_GuildNodeManager.UpdateGuildNode( this );
}

void GuildNode::AddGuildPoint( int iCurGP )
{
	m_dwCurGuildPoint += iCurGP;
	m_dwGuildPoint = max( 0, (int)m_dwGuildPoint + iCurGP );

	g_GuildNodeManager.UpdateGuildNode( this );
	LOG.PrintTimeAndLog( 0, "AddGuildPoint : %s : %d-%d-%d", GetGuildName().c_str(), m_dwGuildPoint, m_dwCurGuildPoint, iCurGP );
}

void GuildNode::Save()
{
	m_bSaveReg = false;
	LOG.PrintTimeAndLog( 0, "Guild Save : %s - %d - %d - %d - %d - %d", m_szGuildName.c_str(), m_dwGuildRank, m_dwGuildPoint, m_dwCurGuildPoint, m_dwGuildLevel, m_dwGuildMaxEntry );
	// 정보 변경 업데이트
	g_DBClient.OnUpdateGuildRanking( m_dwGuildIndex, m_dwGuildRank, m_dwGuildPoint, m_dwCurGuildPoint, m_dwGuildLevel, m_dwGuildMaxEntry );

	// 전적 변경 업데이트
	if( m_Record.m_bChange )
	{
		g_DBClient.OnUpdateGuildRecord( m_dwGuildIndex, m_Record.m_iWin, m_Record.m_iLose, m_Record.m_iKill, m_Record.m_iDeath );
		m_Record.m_bChange = false;
	}
}

void GuildNode::SetGuildMaxEntry( DWORD dwMaxEntry )
{
	m_dwGuildMaxEntry = min( dwMaxEntry, GetLimitMaxEntry() );
	g_GuildNodeManager.UpdateGuildNode( this, true );
}

void GuildNode::SetGuildJoinUser( DWORD dwJoinUserCount )
{
	m_dwGuildJoinUser = dwJoinUserCount;	

	DWORD dwPrevMaxEntry = m_dwGuildMaxEntry;
	DWORD dwLimitMaxEntry= GetLimitMaxEntry();
	if( m_dwGuildJoinUser >= dwLimitMaxEntry )
	{
		m_dwGuildMaxEntry = min( m_dwGuildMaxEntry, m_dwGuildJoinUser );
	}
	else
	{
		m_dwGuildMaxEntry = max( m_dwGuildMaxEntry, m_dwGuildJoinUser );
	}
	if( dwPrevMaxEntry != m_dwGuildMaxEntry )
		g_GuildNodeManager.UpdateGuildNode( this, true );
}

void GuildNode::SetGuildLevel( DWORD dwGuildLevel, DWORD dwGuildMaxEntry )
{
	if( m_dwGuildLevel == dwGuildLevel ) return;

	// 저장 요청
	g_GuildNodeManager.UpdateGuildNode( this );

	// 레벨 변경
	m_dwGuildLevel = dwGuildLevel;

	// 유저가 길드 최대인원을 건드렸다면 최대 인원이 허용되는 범위에서 변경하지 않는다.
	if( GetGuildMaxEntry() % g_GuildNodeManager.GetGuildLevelUPEntryGap() == 0 || 
		GetGuildMaxEntry() > dwGuildMaxEntry )        
	{
		m_dwGuildMaxEntry = dwGuildMaxEntry;
	}
	m_dwGuildMaxEntry = max( m_dwGuildMaxEntry, m_dwGuildJoinUser );
}

void GuildNode::ChangeGuildName( const ioHashString &rkNewName )
{
	LOG.PrintTimeAndLog( 0, "ChangeGuildName(%d) : ( %s - > %s )", GetGuildIndex(), GetGuildName().c_str(), rkNewName.c_str() );
	m_szGuildName = rkNewName;
}

float GuildNode::GetGuildBonus()
{
	return g_GuildNodeManager.GetGuildLevelBonus( GetGuildLevel() );
}