
#include "stdafx.h"

#include "ioNoviceGradeBenefitManager.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "ioNpcChar.h"

#include "BuffDefine.h"

template<> ioNoviceGradeBenefitManager* Singleton< ioNoviceGradeBenefitManager >::ms_Singleton = 0;

ioNoviceGradeBenefitManager::ioNoviceGradeBenefitManager()
{
	InitData();
}

ioNoviceGradeBenefitManager::~ioNoviceGradeBenefitManager()
{
	ClearData();
}

ioNoviceGradeBenefitManager& ioNoviceGradeBenefitManager::GetSingleton()
{
	return Singleton< ioNoviceGradeBenefitManager >::GetSingleton();
}

void ioNoviceGradeBenefitManager::ClearData()
{
	m_szBenefitBuffList.clear();
}

void ioNoviceGradeBenefitManager::InitData()
{
	m_szBenefitBuffList.clear();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioNoviceGradeBenefitManager::LoadProperty()
{
	char szBuf[MAX_PATH], szKey[MAX_PATH];
	ioINILoader_e kLoader( "config/sp2_novicegradebenefit.ini" );
	kLoader.SetTitle_e( "common" );
	m_nMinGrade = kLoader.LoadInt_e( "min_grade", 0 );
	m_nMaxGrade = kLoader.LoadInt_e( "max_grade", 0 );

	kLoader.SetTitle_e( "disable_mode_info" );
	int nDisableModeCnt = kLoader.LoadInt_e( "disable_mode_cnt", 0 );
	for( int i=0; i<nDisableModeCnt; ++i )
	{
		wsprintf_e( szBuf, "disable_mode%d", i+1 );
		int nDisableModeType = kLoader.LoadInt( szBuf, 0 );
		if ( nDisableModeType > 0 )
			m_vDisableMode.push_back( nDisableModeType );
	}

	kLoader.SetTitle_e( "benefit_buff_info" );
	m_szBenefitBuffList.clear();
	int nBuffCnt = kLoader.LoadInt_e( "benefit_buff_cnt", 0 );
	for( int i=0; i<nBuffCnt; i++ )
	{
		wsprintf_e( szKey, "benefit_buff%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_szBenefitBuffList.push_back( szBuf );
	}
}

void ioNoviceGradeBenefitManager::OnProcess( ioBaseChar *pOwner )
{
	if ( !CheckEnableBenefit( pOwner ) )
		return;
	
	if ( IsBenefitGrade( pOwner->GetLevel() ) )
		AddBenefitBuff( pOwner );
	else
		RemoveBenefitBuff( pOwner );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioNoviceGradeBenefitManager::CheckEnableBenefit( ioBaseChar *pOwner )
{
	if ( m_szBenefitBuffList.empty() )
		return false;

	if ( !P2PNetwork::IsNetworkPlaying() || !pOwner )
		return false;

	if ( ToNpcChar( pOwner ) )
		return false;

	for ( int i=0; i<(int)m_vDisableMode.size(); i++ )
	{
		if ( m_vDisableMode[i] == pOwner->GetCurModeType() )
			return false;
	}

	if( pOwner->GetState() == CS_DIE ||
		pOwner->GetState() == CS_VIEW ||
		pOwner->GetState() == CS_OBSERVER ||
		pOwner->GetState() == CS_LOADING ||
		pOwner->GetState() == CS_READY )
		return false;

	return true;
}

void ioNoviceGradeBenefitManager::AddBenefitBuff( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
		
	bool bAddBenefitBuff = true;
	int nBuffCnt = (int)m_szBenefitBuffList.size();
	for( int i=0; i<nBuffCnt; ++i )
	{
		if( pOwner->HasBuff( pOwner->GetCharName(), m_szBenefitBuffList[i] ) )
			bAddBenefitBuff = false;
	}

	if ( bAddBenefitBuff )
	{
		for( int i=0; i<nBuffCnt; ++i )
			pOwner->AddNewBuff( m_szBenefitBuffList[i], pOwner->GetCharName(), "", NULL );
		
		/*if ( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_NOVICE_GRADE_BENEFIT );
			kPacket << pOwner->GetCharName();
			kPacket << NGBS_Add_Benefit_Buff;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}*/
	}
}

void ioNoviceGradeBenefitManager::RemoveBenefitBuff( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	bool bRemoveBenefitBuff = false;
	int nBuffCnt = (int)m_szBenefitBuffList.size();
	for( int i=0; i<nBuffCnt; ++i )
	{
		if( pOwner->HasBuff( pOwner->GetCharName(), m_szBenefitBuffList[i] ) )
			bRemoveBenefitBuff = true;
	}

	if ( bRemoveBenefitBuff )
	{
		int nBuffCnt = (int)m_szBenefitBuffList.size();
		for( int i=0; i<nBuffCnt; ++i )
		{
			ioBuff *pRemoveBuff = pOwner->GetBuff( m_szBenefitBuffList[i], pOwner->GetCharName() );
			if( pRemoveBuff )
				pRemoveBuff->SetReserveEndBuff();
		}

		/*if ( pOwner->IsNeedSendNetwork() )
		{
		SP2Packet kPacket( CUPK_NOVICE_GRADE_BENEFIT );
		kPacket << pOwner->GetCharName();
		kPacket << NGBS_Remove_Benefit_Buff;
		P2PNetwork::SendToAllPlayingUser( kPacket );
		}*/		
	}
}

bool ioNoviceGradeBenefitManager::IsBenefitGrade( int nGrade )
{
	if ( COMPARE( nGrade, m_nMinGrade, m_nMaxGrade ) )
		return true;

	return false;
}