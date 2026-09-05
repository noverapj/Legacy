#include "StdAfx.h"
#include "ioAccessoryPassiveSkill.h"

ioAccessoryPassiveSkill::ioAccessoryPassiveSkill(void)
{
	m_fAccessoryValue = 0.0f;
	m_dwAccessoryRecoveryTick = 0;
	m_iEquipType = -1;
}

ioAccessoryPassiveSkill::ioAccessoryPassiveSkill( const ioAccessoryPassiveSkill &rhs ):
ioPassiveSkill( rhs ),
m_AccValueType( rhs.m_AccValueType ),
m_fEnableCheckBuffPassiveGaugeRate( rhs.m_fEnableCheckBuffPassiveGaugeRate ),
m_fReducePassiveGaugeByCheckBuff( rhs.m_fReducePassiveGaugeByCheckBuff ),
m_CheckBuffList( rhs.m_CheckBuffList ),
m_WounderBuffList( rhs.m_WounderBuffList )
{
	m_fAccessoryValue = 0.0f;
	m_dwAccessoryRecoveryTick = 0;
	m_iEquipType = -1;
}

ioAccessoryPassiveSkill::~ioAccessoryPassiveSkill(void)
{
}

ioSkill* ioAccessoryPassiveSkill::Clone()
{
	return new ioAccessoryPassiveSkill( *this );
}

void ioAccessoryPassiveSkill::LoadProperty( ioINILoader &rkLoader )
{
	char szTitle[MAX_PATH], szBuff[MAX_PATH];
	ioPassiveSkill::LoadProperty( rkLoader );

	m_AccValueType = (AccValueType)rkLoader.LoadInt_e( "accessory_value_type", ACT_NONE );
	m_fEnableCheckBuffPassiveGaugeRate = rkLoader.LoadFloat_e( "enable_check_buff_passive_gauge", 0.0f );	
	m_fReducePassiveGaugeByCheckBuff = rkLoader.LoadFloat_e( "reduce_passive_gauge_by_check_buff", 0.0f );	

	int iCnt = rkLoader.LoadInt_e( "check_buff_cnt", 0 );
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "check_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuff, MAX_PATH );
		ioHashString szBuffName = szBuff;

		if( !szBuffName.IsEmpty() )
		{
			int iType = g_BuffMaker.GetBuffType( szBuffName );
			if( iType != BT_NONE )
			{
				ioBuffInfo kBuffInfo( szBuffName, iType );
				m_CheckBuffList.push_back( kBuffInfo );
			}
		}
	}

	iCnt = rkLoader.LoadInt_e( "wounder_buff_cnt", 0 );
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "wounder_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuff, MAX_PATH );
		ioHashString szBuffName = szBuff;

		if( !szBuffName.IsEmpty() )
		{
			int iType = g_BuffMaker.GetBuffType( szBuffName );
			if( iType != BT_NONE )
			{
				ioBuffInfo kBuffInfo( szBuffName, iType );
				m_WounderBuffList.push_back( kBuffInfo );
			}
		}
	}
}

bool ioAccessoryPassiveSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioPassiveSkill::OnSkillStart( pChar ) )
		return false;

	ioItem* pItem = GetOwnerItem();
	if ( !pItem )
		return false;
	m_iEquipType = pItem->GetType();
	if ( !COMPARE( m_iEquipType, ioItem::IT_RING, ioItem::IT_BRACELET + 1 ) )
		return false;
	m_fAccessoryValue = GetAccessoryRandomNum() / FLOAT1000;

	if ( m_bUsePassiveGauge )
	{
		float fMaxTime = m_fAccessoryValue * FLOAT1000;
		m_dwAccessoryRecoveryTick = (DWORD)( fMaxTime * m_fRecoveryValue / m_fMaxPassiveGauge );
	}
	return true;
}

void ioAccessoryPassiveSkill::CheckReducePassiveGaugeByCheckBuff( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	if ( m_fReducePassiveGaugeByCheckBuff <= 0.0f || m_fEnableCheckBuffPassiveGaugeRate <= 0.0f )
		return;

	if( m_CheckBuffList.empty() || m_WounderBuffList.empty() )
		return;

	float fRate = ( m_fCurPassiveGauge / m_fMaxPassiveGauge ) * FLOAT100;
	if ( fRate < m_fEnableCheckBuffPassiveGaugeRate )
		return;


	int iSize = m_CheckBuffList.size();
	bool bCheck = false;

	for( int i=0 ; i<iSize ; ++i )
	{
		if( pChar->HasBuff( ( pChar->GetCharName(), m_CheckBuffList[i].m_BuffName ) ) )
		{
			bCheck = true;
			break;
		}
	}

	if ( bCheck )
	{
		ioBuff *pBuff = pChar->AddNewBuff( m_WounderBuffList[i].m_BuffName,
			pChar->GetCharName(),
			GetOwnerItem()->GetName(),
			this );

		ReduceCurPassiveGauge( m_fReducePassiveGaugeByCheckBuff );
	}
}

void ioAccessoryPassiveSkill::UpdateRecoveryPassiveGauge( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( !m_bUsePassiveGauge ) return;
	if( m_bStopPassiveGaugeIOnSkipBuff && pChar->HasBuff( BT_SKIP_STUN ) )
		return;
	if( m_dwCheckZeroGaugeTime > 0 && m_dwCheckZeroGaugeTime + m_dwRecoveryDelay >= FRAMEGETTIME() )
		return;
	if( !pChar->GetUseEnableSkill( GetEquipSlotByCode(GetOwnerItem()->GetItemCode() ), SUT_NORMAL, true ) )
		return;

	int iCnt = m_RecoveryIgnoreBuffList.size();
	for( int i=0; i<iCnt; ++i )
	{
		ioBuff *pBuff = pChar->GetBuff( m_RecoveryIgnoreBuffList[i] );
		if( pBuff && pBuff->IsLive() )
		{
			return;
		}
	}

	iCnt = m_OwnerBuffList.size();
	for( int i=0; i < iCnt; ++i )
	{
		ioBuff *pBuff = pChar->GetBuff( m_OwnerBuffList[i].m_BuffName );
		if( pBuff && pBuff->OnReduceGauge() )
		{
			return;
		}
	}

	float fFrameGap = g_FrameTimer.GetCurFrameGap();

	float fRate = 0.0f;
	ioPlayStage *pStage = pChar->GetCreator();
	if( pStage && pStage->GetModeType() == MT_FIGHT_CLUB && m_dwFCModeRecoveryTick > 0 )
	{
		fRate = fFrameGap / m_dwFCModeRecoveryTick;
	}
	else if ( COMPARE( m_iEquipType, ES_RING, MAX_EQUIP_SLOT ) && m_dwAccessoryRecoveryTick > 0 )
	{
		fRate = fFrameGap / m_dwAccessoryRecoveryTick;
	}
	else
	{
		fRate = fFrameGap / m_dwRecoveryTick;
	}

	float fValue = m_fRecoveryValue * fRate;

	// 상태에 따른 보정
	if( pChar->GetState() == CS_RUN || pChar->GetState() == CS_DELAY ||
		pChar->GetState() == CS_SPECIAL_MOTION || pChar->GetState() == CS_ETCITEM_MOTION )
	{
		fValue *= m_fRecoveryPassiveGaugeRateOnDelayRun;
	}
	else
	{
		fValue *= m_fRecoveryPassiveGaugeRateOnEtc;
	}

	float fPreGauge = m_fCurPassiveGauge;

	m_fCurPassiveGauge += fValue;
	m_fCurPassiveGauge = min( m_fCurPassiveGauge, m_fMaxPassiveGauge );

	if( fPreGauge <= 0.0f && m_fCurPassiveGauge > 0.0f )
	{
		if( pChar->IsNeedSendNetwork() )
		{
			int iSlot = GetOwnerItem()->GetType() - 1;
			SP2Packet kPacket( CUPK_PASSIVE_GAUGE );
			kPacket << pChar->GetCharName();
			kPacket << iSlot;
			kPacket << m_fCurPassiveGauge;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioAccessoryPassiveSkill::CheckNPCAttackDamageRate( float& fDamage )
{
	if ( m_AccValueType != AVT_NPC_ATTACK_RATE )
		return;

	if ( m_fAccessoryValue <= 0.0f )
		return;

	fDamage *= m_fAccessoryValue;
}

void ioAccessoryPassiveSkill::CheckNPCWoundDamageRate( float& fDamage )
{
	if ( m_AccValueType != AVT_NPC_WOUNDE_RATE )
		return;

	if ( m_fAccessoryValue <= 0.0f )
		return;

	fDamage *= m_fAccessoryValue;
}
