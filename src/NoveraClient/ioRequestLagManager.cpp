
#include "stdafx.h"

#include "ioRequestLagManager.h"

#include "ioPlayStage.h"
#include "ioPlayMode.h"
#include "ioBaseChar.h"

template<> ioRequestLagManager* Singleton< ioRequestLagManager >::ms_Singleton = 0;

ioRequestLagManager::ioRequestLagManager()
{
	ClearData();
}

ioRequestLagManager::~ioRequestLagManager()
{
	ClearData();
}

ioRequestLagManager& ioRequestLagManager::GetSingleton()
{
	return Singleton< ioRequestLagManager >::GetSingleton();
}

void ioRequestLagManager::ClearData()
{
	m_szBuffList.clear();
	m_bCheckAvgFPS = true;
	m_fAvgFPS = 0.0f;
	m_bIsCheckLag = false;

	InitData();
}

void ioRequestLagManager::InitData()
{
	m_nRequestValue = 0;
	m_bStart = false;

	m_dwIncreaseTime = 0;
	m_dwIncreaseInputTime = 0;
	m_dwDecreaseTime = 0;
	m_dwAvgFPSTime = 0;

	m_fTotalFPS = 0.0f;
	m_nFPSCnt = 0;

	m_fHalfFPS = 0.0f;
	m_dwHalfFPSTime = 0;

	m_fFPSIncreased = 0.0f;

	m_dwBuffAddTime = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioRequestLagManager::LoadProperty()
{
	char szBuf[MAX_PATH], szKey[MAX_PATH];
	ioINILoader_e kLoader( "config/sp2_requestchar.ini" );
	kLoader.SetTitle_e( "lag_info" );
	
	m_bProcess = kLoader.LoadBool_e( "apply_fps_lag", false );

	//시간 설정
	m_dwCheckIncreaseTime = (DWORD)kLoader.LoadInt_e( "check_increase_time", 0);
	m_dwCheckIncreaseInputTime = (DWORD)kLoader.LoadInt_e( "check_increase_input_time", 0);
	m_dwCheckDecreaseTime = (DWORD)kLoader.LoadInt_e( "check_decrease_time", 0);
	m_dwCheckAvgFPSTime = (DWORD)kLoader.LoadInt_e( "check_avg_fps_time", 0);

	//비교값 설정
	m_nCheckValue = kLoader.LoadInt_e( "check_value", 0 );
	m_nIncreaseValue = kLoader.LoadInt_e( "increase_value", 0 );
	m_nIncreaseInputValue = kLoader.LoadInt_e( "increase_input_value", 0 );
	m_nDecreaseValue = kLoader.LoadInt_e( "decrease_value", 0 );

	m_fAvgFPSRate = kLoader.LoadFloat_e( "fps_rate", 0.01f );

	m_szBuffList.clear();

	int nBuffCnt = kLoader.LoadInt_e( "add_buff_cnt", 0 );
	for( int i=0; i<nBuffCnt; i++ )
	{
		wsprintf_e( szKey, "add_buff%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_szBuffList.push_back( szBuf );
	}

	int nDisableModeCnt = kLoader.LoadInt_e( "disable_mode_cnt", 0 );
	for( int i=0; i<nDisableModeCnt; ++i )
	{
		wsprintf_e( szBuf, "disable_mode%d", i+1 );
		int nDisableModeType = kLoader.LoadInt( szBuf, 0 );
		if ( nDisableModeType > 0 )
			m_vDisableMode.push_back( nDisableModeType );
	}
}

void ioRequestLagManager::OnProcess( ioPlayStage *pPlayStage )
{
	if ( !IsProcess() || !IsEnableFunc( pPlayStage ) )
	{
		InitData();

		m_bCheckAvgFPS = true;
		m_dwAvgFPSTime = FRAMEGETTIME();
		m_bIsCheckLag = false;
		return;
	}

	m_bIsCheckLag = true;

	float fCurFPS = 0.0f;
	float fSecFrame = g_FrameTimer.GetSecPerFrame();
	if ( fSecFrame > 0.0f )
		fCurFPS = FLOAT1/fSecFrame;

	AvgFPS( fCurFPS );

	IncreaseValue( fCurFPS );
	DecreaseValue();

	RenderFPSInfo( fCurFPS );

	if ( IsCheckValue() )
		RequestAction( pPlayStage );
}

void ioRequestLagManager::RenderFPSInfo( float fCurFPS )
{
	if ( !g_App.IsFPSRender() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_fAvgFPS > 0.0f && fCurFPS < ( m_fAvgFPS * FLOAT05 ) )
	{
		m_fHalfFPS = fCurFPS;
		m_dwHalfFPSTime = dwCurTime;
	}

	if ( m_dwHalfFPSTime > 0 && m_dwHalfFPSTime + 2000 < dwCurTime )
	{
		m_fHalfFPS = 0.0f;
		m_dwHalfFPSTime = 0;
	}

	if ( m_dwBuffAddTime > 0 && m_dwBuffAddTime + 5000 < dwCurTime )
		m_dwBuffAddTime = 0;
	
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_SHADOW );
	g_FontMgr.SetTextColor( 255, 255, 0 );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.PrintText( FLOAT1, 135.0f, 0.7f, "Avg FPS : %.1f, Req FPS : %.1f", m_fAvgFPS, (m_fAvgFPS * m_fAvgFPSRate) );
	g_FontMgr.PrintText( FLOAT1, 155.0f, 0.7f, "Half FPS : %.1f", m_fHalfFPS );
	g_FontMgr.PrintText( FLOAT1, 175.0f, 0.7f, "Lag Req Val : %d", m_nRequestValue );
	g_FontMgr.PrintText( FLOAT1, 195.0f, 0.7f, "Increased Val FPS : %.1f", m_fFPSIncreased );

	if ( m_dwBuffAddTime > 0 )
		g_FontMgr.PrintText( FLOAT1, 215.0f, 0.7f, "FPS Lag - Buff Add" );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioRequestLagManager::IsEnableFunc( ioPlayStage *pPlayStage )
{
	if ( !pPlayStage )
		return false;

	for ( int i=0; i<(int)m_vDisableMode.size(); i++ )
	{
		if ( pPlayStage->GetModeType() == m_vDisableMode[i] )
			return false;
	}
	
	ioPlayMode *pPlayMode = pPlayStage->GetPlayMode();
	if( !pPlayMode )
		return false;

	if( pPlayMode->GetModeState() != ioPlayMode::MS_PLAY )
		return false;

	ioHashString szCharName = g_MyInfo.GetPublicID();
	if ( !P2PNetwork::IsNetworkPlaying() )
		szCharName = "character1";
	
	ioBaseChar *pChar = pPlayStage->GetBaseChar( szCharName );
	if ( !pChar )
		return false;

	if( pChar->GetState() == CS_VIEW ||
		pChar->GetState() == CS_LOADING ||
		pChar->GetState() == CS_READY||
		pChar->GetState() == CS_OBSERVER )
		return false;

	return true;
}

void ioRequestLagManager::AvgFPS( float fCurFPS )
{
	/*
	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwAvgFPSTime + m_dwCheckAvgFPSTime < dwCurTime )
	{
		m_dwAvgFPSTime = dwCurTime;
		m_fTotalFPS = 0.0f;
		m_nFPSCnt = 0;
	}
	else
	{
		m_fTotalFPS += ( FLOAT1/g_FrameTimer.GetSecPerFrame() );
		m_nFPSCnt++;

		m_fAvgFPS = m_fTotalFPS / m_nFPSCnt;
	}*/

	if ( !m_bCheckAvgFPS )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwAvgFPSTime + m_dwCheckAvgFPSTime < dwCurTime )
		m_bCheckAvgFPS = false;
	else
	{
		m_fTotalFPS += fCurFPS;
		m_nFPSCnt++;

		m_fAvgFPS = m_fTotalFPS / m_nFPSCnt;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioRequestLagManager::IncreaseValue( float fCurFPS )
{	
	if ( m_fAvgFPS > 0.0f && fCurFPS < m_fAvgFPS * m_fAvgFPSRate )
	{
		DWORD dwCurTime = FRAMEGETTIME();
		if ( m_dwIncreaseTime + m_dwCheckIncreaseTime < dwCurTime )
		{
			m_nRequestValue += m_nIncreaseValue;
			m_fFPSIncreased = fCurFPS;
			m_dwIncreaseTime = dwCurTime;

			if ( !m_bStart )
			{
				m_dwDecreaseTime = dwCurTime;
				m_bStart = true;
			}
		}
	}
}

void ioRequestLagManager::DecreaseValue()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwDecreaseTime > 0 && m_dwDecreaseTime + m_dwCheckDecreaseTime < dwCurTime )
	{
		m_nRequestValue -= m_nDecreaseValue;
		m_nRequestValue = max( m_nRequestValue, 0 );
		m_dwDecreaseTime = dwCurTime;

		if ( m_nRequestValue <= 0 )
			InitData();
	}
}

void ioRequestLagManager::IncreaseValueInput()
{
	if ( !m_bIsCheckLag )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwIncreaseInputTime + m_dwCheckIncreaseInputTime < dwCurTime )
	{
		m_nRequestValue += m_nIncreaseInputValue;
		m_dwIncreaseInputTime = dwCurTime;

		if ( !m_bStart )
		{
			m_dwDecreaseTime = dwCurTime;
			m_bStart = true;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioRequestLagManager::IsCheckValue()
{
	if ( m_bStart && m_nRequestValue >= m_nCheckValue )
	{
		InitData();
		return true;
	}
	return false;
}

void ioRequestLagManager::RequestAction( ioPlayStage *pPlayStage )
{
	if ( !pPlayStage )
		return;

	ioHashString szCharName = g_MyInfo.GetPublicID();
	if ( !P2PNetwork::IsNetworkPlaying() )
		szCharName = "character1";

	ioBaseChar *pChar = pPlayStage->GetBaseChar( szCharName );
	if ( !pChar )
		return;

	if( pChar->GetState() == CS_DIE )
		return;

	m_dwBuffAddTime = FRAMEGETTIME();

	AddLagActionBuff( pChar );

	if ( pChar->IsNeedProcess() && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_LAG_ACTION );
		kPacket << szCharName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioRequestLagManager::AddLagActionBuff( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	for ( int i=0; i<(int)m_szBuffList.size(); i++ )
		pChar->AddNewBuff( m_szBuffList[i], pChar->GetCharName(), "", NULL );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioRequestLagManager::IsProcess()
{
	return m_bProcess;
}

void ioRequestLagManager::SetProcess( bool bProcess )
{
	m_bProcess = bProcess;

	if ( m_bProcess )
	{
		InitData();

		m_bCheckAvgFPS = true;
		m_dwAvgFPSTime = FRAMEGETTIME();
	}
}