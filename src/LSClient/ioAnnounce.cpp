

#include "stdafx.h"

#include "ioAnnounce.h"
#include "ioComplexStringPrinter.h"
#include "ioAnnounceLevel.h"



ioAnnounce::ioAnnounce()
{
	m_dwStartTime = FRAMEGETTIME();
	m_dwEndTime = m_dwStartTime;
}

ioAnnounce::~ioAnnounce()
{
}

void ioAnnounce::SetLiveTime( DWORD dwLiveTime )
{
	m_dwEndTime = m_dwStartTime + dwLiveTime;
}

DWORD ioAnnounce::GetStartTime() const
{
	return m_dwStartTime;
}

bool ioAnnounce::IsLive() const
{
	if( m_dwEndTime >= FRAMEGETTIME() )
		return true;

	return false;
}

//------------------------------------------------------


ioGameReadyAnnounce::ioGameReadyAnnounce()
{
	m_eOwnerTeam = TEAM_BLUE;
	m_iCurRound  = 1;
	m_iHalfRound = 5;
}

ioGameReadyAnnounce::~ioGameReadyAnnounce()
{
}

void ioGameReadyAnnounce::SetAnnounce( TeamType eOwnerTeam, int iCurRound, int iHalfRound )
{
	m_eOwnerTeam = eOwnerTeam;
	m_iCurRound  = iCurRound;
	m_iHalfRound = iHalfRound;
}

int ioGameReadyAnnounce::GetLevel() const
{
	return ANN_GAME_READY;
}

void ioGameReadyAnnounce::Render()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwEndTime < dwCurTime )	return;

	DWORD dwGapTime = m_dwEndTime - dwCurTime;
	int iSecond = (int)dwGapTime / 1000;

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	float fXPos = Setting::HalfWidth();
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( fXPos, 220.0f, 1.25f, STR(1) );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );

	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	kPrinter.AddTextPiece( 0.75f, STR(2) );

	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPieceWithYOffset( FLOAT1, -4.0f, STR(7), iSecond + 1 );

	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	kPrinter.AddTextPiece( 0.75f, STR(3) );

	kPrinter.PrintFullText( fXPos, 257.0f, TAT_CENTER );

	if( m_eOwnerTeam != TEAM_NONE && m_iCurRound == m_iHalfRound )
	{
		kPrinter.ClearList();

		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );

		if( m_eOwnerTeam == TEAM_BLUE )
		{
			kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
			kPrinter.AddTextPiece( 0.75f, STR(4) );
		}
		else if( m_eOwnerTeam == TEAM_RED )
		{
			kPrinter.SetTextColor( TCT_DEFAULT_RED );
			kPrinter.AddTextPiece( 0.75f, STR(5) );
		}

		kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
		kPrinter.AddTextPiece( 0.75f, STR(6) );

		kPrinter.PrintFullText( fXPos, 281.0f, TAT_CENTER );
	}
}


//-------------------------------------------

ioNormalAnnounce::ioNormalAnnounce()
{
	m_iLevel = ANN_DEFAULT;
	m_dwTitleColor = TCT_DEFAULT_GRAY;
}

ioNormalAnnounce::~ioNormalAnnounce()
{
}

void ioNormalAnnounce::SetAnnounce( const char *szTitle,
									const char *szAnnounce,
									DWORD dwTitleColor )
{
	strcpy( m_szTitle, szTitle );
	strcpy( m_szAnnounce, szAnnounce );
	m_dwTitleColor = dwTitleColor;
}

void ioNormalAnnounce::SetLevel( int iLevel )
{
	m_iLevel = iLevel;
}

int ioNormalAnnounce::GetLevel() const
{
	return m_iLevel;
}

void ioNormalAnnounce::Render()
{
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	float fXPos = Setting::HalfWidth();
	g_FontMgr.SetTextColor( m_dwTitleColor );
	g_FontMgr.PrintText( fXPos, 220.0f, 1.25f, m_szTitle );

	g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	g_FontMgr.PrintText( fXPos, 257.0f, 0.75f, m_szAnnounce );
}

//----------------------------------------------------

ioLevelupAnnounce::ioLevelupAnnounce()
{
	for( int i=0 ; i<4 ; i++ )
	{
		m_Stat[i] = 0;
	}
}

ioLevelupAnnounce::~ioLevelupAnnounce()
{
}

void ioLevelupAnnounce::SetAnnounce( int iStr, int iDex, int iInt, int iVit )
{
	m_Stat[0] = iStr;
	m_Stat[1] = iDex;
	m_Stat[2] = iInt;
	m_Stat[3] = iVit;
}

int ioLevelupAnnounce::GetLevel() const
{
	return ANN_LEVELUP;
}

void ioLevelupAnnounce::Render()
{
	enum { MAX_STAT_TEXT = 4, };
	char szStatText[MAX_STAT_TEXT][MAX_PATH];
	StringCbCopy( szStatText[0], sizeof( szStatText[0] ), STR(7) );
	StringCbCopy( szStatText[1], sizeof( szStatText[1] ), STR(8) );
	StringCbCopy( szStatText[2], sizeof( szStatText[2] ), STR(9) );
	StringCbCopy( szStatText[3], sizeof( szStatText[3] ), STR(10) );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	float fXPos = Setting::HalfWidth();
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( fXPos, 220.0f, 1.25f, STR(1) );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );

	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	kPrinter.AddTextPiece( 0.75f, STR(2) );

	bool bFirst = true;
	for( int i=0 ; i<4 ; i++ )
	{
		int iCurStat = m_Stat[i];
		if( iCurStat == 0 )	continue;

		kPrinter.SetTextColor( TCT_DEFAULT_WHITE );

		if( !bFirst )
		{
			kPrinter.AddTextPiece( 0.75f, STR(3), szStatText[i] );
		}
		else
		{
			kPrinter.AddTextPiece( 0.75f, STR(4), szStatText[i] );
		}
			
		if( iCurStat > 0 )
		{
			kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
		}
		else
		{
			kPrinter.SetTextColor( TCT_DEFAULT_RED );
		}
			
		kPrinter.AddTextPiece( 0.75f, STR(5), iCurStat );
		
		kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
		kPrinter.AddTextPiece( 0.75f, STR(6) );

		bFirst = false;
	}

	kPrinter.PrintFullText( fXPos, 257.0f, TAT_CENTER );
}

//----------------------------------------------------

ioGetItemAnnounce::ioGetItemAnnounce()
{
}

ioGetItemAnnounce::~ioGetItemAnnounce()
{
}

void ioGetItemAnnounce::SetAnnounce( const ioItem *pItem )
{
	m_ItemName  = pItem->GetName();
	m_SkillName = pItem->GetSkillName();
}

int ioGetItemAnnounce::GetLevel() const
{
	return ANN_GET_ITEM;
}

void ioGetItemAnnounce::Render()
{
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	float fXPos = Setting::HalfWidth();
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( fXPos, 220.0f, 1.25f, STR(1) );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );

	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	kPrinter.AddTextPiece( 0.75f, STR(2), m_ItemName.c_str() );
	if( !m_SkillName.IsEmpty() )
	{
		kPrinter.AddTextPiece( 0.75f, m_SkillName.c_str() );
	}

	kPrinter.PrintFullText( fXPos, 257.0f, TAT_CENTER );
}

//---------------------------------------------------

ioRevivalAnnounce::ioRevivalAnnounce()
{
}

ioRevivalAnnounce::~ioRevivalAnnounce()
{
}

int ioRevivalAnnounce::GetLevel() const
{
	return ANN_REVIVAL;
}

void ioRevivalAnnounce::Render()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwEndTime < dwCurTime )	return;

	DWORD dwGapTime = m_dwEndTime - dwCurTime;
	int iSecond = (int)dwGapTime / 1000;

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	float fXPos = Setting::HalfWidth();
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( fXPos, 220.0f, 1.25f, STR(1) );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );

	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	kPrinter.AddTextPiece( 0.75f, STR(2) );

	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPieceWithYOffset( FLOAT1, -4.0f, STR(5), iSecond + 1 );

	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	kPrinter.AddTextPiece( 0.75f, STR(3) );

	kPrinter.SetTextColor( TCT_DEFAULT_RED );
	kPrinter.AddTextPiece( 0.75f, "X" );

	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	kPrinter.AddTextPiece( 0.75f, STR(4) );

	kPrinter.PrintFullText( fXPos, 257.0f, TAT_CENTER );
}

//--------------------------------------------------------------------------
ioDefenseSymbolAnnounce::ioDefenseSymbolAnnounce()
{
	m_bDefenseMode = true;
}

ioDefenseSymbolAnnounce::~ioDefenseSymbolAnnounce()
{
}

void ioDefenseSymbolAnnounce::SetAnnounce( bool bDefenseMode )
{
	m_bDefenseMode = bDefenseMode;
}

int ioDefenseSymbolAnnounce::GetLevel() const
{
	return ANN_DEFENSE_SYMBOL;
}

void ioDefenseSymbolAnnounce::Render()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwEndTime < dwCurTime )	return;

	DWORD dwGapTime = m_dwEndTime - dwCurTime;
	int iSecond = (int)dwGapTime / 1000;

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	float fXPos = Setting::HalfWidth();
	g_FontMgr.SetTextColor( TCT_DEFAULT_RED );

	if( m_bDefenseMode )
	{
		g_FontMgr.PrintText( fXPos, 220.0f, 1.25f, STR(1), iSecond );
	
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		g_FontMgr.PrintText( fXPos, 257.0f, 0.75f, STR(2) );
	}
	else
	{
		g_FontMgr.PrintText( fXPos, 220.0f, 1.25f, STR(3), iSecond );

		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		g_FontMgr.PrintText( fXPos, 257.0f, 0.75f, STR(4) );
	}
}

//--------------------------------------------------------------------------
ioExitPrisonerMode::ioExitPrisonerMode()
{
	m_bExitPrisonerMode = true;
}

ioExitPrisonerMode::~ioExitPrisonerMode()
{
}

void ioExitPrisonerMode::SetAnnounce( bool bExitPrisonerMode )
{
	m_bExitPrisonerMode = bExitPrisonerMode;
}

int ioExitPrisonerMode::GetLevel() const
{
	return ANN_EXIT_PRISONERMODE;
}

void ioExitPrisonerMode::Render()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwEndTime < dwCurTime )	return;

	DWORD dwGapTime = m_dwEndTime - dwCurTime;
	int iSecond = (int)dwGapTime / 1000;

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	float fXPos = Setting::HalfWidth();
	g_FontMgr.SetTextColor( TCT_DEFAULT_RED );

	if( m_bExitPrisonerMode )
	{
		g_FontMgr.PrintText( fXPos, 220.0f, 1.25f, STR(1), iSecond );
	
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		g_FontMgr.PrintText( fXPos, 257.0f, 0.75f, STR(2) );
	}
	else
	{
		g_FontMgr.PrintText( fXPos, 220.0f, 1.25f, STR(3), iSecond );

		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		g_FontMgr.PrintText( fXPos, 257.0f, 0.75f, STR(4) );
	}
}

//--------------------------------------------------------------------------
ioTimeLimitAnnounce::ioTimeLimitAnnounce()
{
}

ioTimeLimitAnnounce::~ioTimeLimitAnnounce()
{
}

int ioTimeLimitAnnounce::GetLevel() const
{
	return ANN_ROUND_TIMELIMIT;
}

void ioTimeLimitAnnounce::Render()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwEndTime < dwCurTime )	return;

	DWORD dwGapTime = m_dwEndTime - dwCurTime;
	int iSecond = (int)dwGapTime / 1000;

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	float fXPos = Setting::HalfWidth();

	g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
	g_FontMgr.PrintText( fXPos, 220.0f, 1.25f, STR(1), iSecond );
	
	g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	g_FontMgr.PrintText( fXPos, 257.0f, 0.75f, STR(2) );
}
