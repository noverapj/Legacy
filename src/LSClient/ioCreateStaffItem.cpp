

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioTalisman.h"

#include "ioCreateStaffItem.h"

ioCreateStaffItem::ioCreateStaffItem()
{
	m_CreateState = CS_NONE;

	m_iCurCombo = 0;
	m_dwChargeStartTime = 0;
	m_dwCreateStartTime = 0;

	m_vTalismanList.clear();
}

ioCreateStaffItem::ioCreateStaffItem( const ioCreateStaffItem &rhs )
: ioWeaponItem( rhs ),
 m_PreGatherAnimation( rhs.m_PreGatherAnimation ),
 m_GatherAnimation( rhs.m_GatherAnimation ),
 m_EndGatherAnimation( rhs.m_EndGatherAnimation ),
 m_GatherEffect( rhs.m_GatherEffect ),
 m_fGaugePerAllDefense( rhs.m_fGaugePerAllDefense ), 
 m_dwDefenseTicTime( rhs.m_dwDefenseTicTime ),
 m_fSkillGaugeRate( rhs.m_fSkillGaugeRate ),
 m_iTalismanNum( rhs.m_iTalismanNum ),
 m_dwCreateTime( rhs.m_dwCreateTime ),
 m_iMaxTalisman( rhs.m_iMaxTalisman ),
 m_iCreateTalismanCnt( rhs.m_iCreateTalismanCnt ),
 m_iSkillNeedTalisman( rhs.m_iSkillNeedTalisman ),
 m_iDefenseWasteTalisman( rhs.m_iDefenseWasteTalisman ),
 m_iWoundedWasteTalisman( rhs.m_iWoundedWasteTalisman )
{
	m_CreateState = CS_NONE;

	m_iCurCombo = 0;
	m_dwChargeStartTime = 0;
	m_dwCreateStartTime = 0;

	m_vTalismanList.clear();
}

ioCreateStaffItem::~ioCreateStaffItem()
{
}

void ioCreateStaffItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "pre_gather_animation", "", szBuf, MAX_PATH );
	m_PreGatherAnimation = szBuf;
	rkLoader.LoadString_e( "gather_animation", "", szBuf, MAX_PATH );
	m_GatherAnimation = szBuf;
	rkLoader.LoadString_e( "gather_effect", "", szBuf, MAX_PATH );
	m_GatherEffect = szBuf;
	rkLoader.LoadString_e( "end_gather_animation", "", szBuf, MAX_PATH );
	m_EndGatherAnimation = szBuf;

	m_fGaugePerAllDefense = rkLoader.LoadFloat_e( "gauge_per_defense", 5.0f );
	m_dwDefenseTicTime = rkLoader.LoadInt_e( "defense_tic_time", 500 );
	m_fSkillGaugeRate = rkLoader.LoadFloat_e( "skill_gauge_rate", FLOAT1 );

	m_iTalismanNum = rkLoader.LoadInt_e( "create_talisman_num", 0 );
	m_dwCreateTime = rkLoader.LoadInt_e( "create_talisman_time", 0 );
	m_iMaxTalisman = rkLoader.LoadInt_e( "max_talisman_cnt", 0 );
	m_iCreateTalismanCnt = rkLoader.LoadInt_e( "create_talisman_cnt", 0 );
	m_iSkillNeedTalisman = rkLoader.LoadInt_e( "need_skill_talisman_cnt", 0 );
	m_iWoundedWasteTalisman = rkLoader.LoadInt_e( "waste_wound_talisman_cnt", 0 );
	m_iDefenseWasteTalisman = rkLoader.LoadInt_e( "waste_defense_talisman_cnt", 0 );

	m_vTalismanList.clear();
}

ioItem* ioCreateStaffItem::Clone()
{
	return new ioCreateStaffItem( *this );
}

void ioCreateStaffItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );

	ClearTalisman( pOwner );
}

void ioCreateStaffItem::OnReleased( ioBaseChar *pOwner )
{
	ioWeaponItem::OnReleased( pOwner );

	ClearTalisman( pOwner );
}

void ioCreateStaffItem::ClearTalisman( ioBaseChar *pOwner )
{
	int iCnt = m_vTalismanList.size();

	if( iCnt > 0 )
	{
		m_vTalismanList[0]->RemoveTalismanBuff( pOwner );

		for( int i=0; i < iCnt; ++i )
		{
			bool bDestroy = m_vTalismanList[i]->DestroyTalisman( pOwner );
			delete m_vTalismanList[i];
		}

		m_vTalismanList.clear();
	}
}

void ioCreateStaffItem::UpdateExtraData( ioBaseChar *pOwner )
{
	int iCurCnt = m_vTalismanList.size();

	if( iCurCnt > 0 )
	{
		if( pOwner->GetState() == CS_DIE ||
			pOwner->GetState() == CS_ENDMOTION )
			
		{
			ClearTalisman( pOwner );
			return;
		}
	}

	TalismanList vEndTalismanList;
	TalismanList::iterator iter = m_vTalismanList.begin();
	while( iter != m_vTalismanList.end()  )
	{
		ioTalisman *pTalisman = (*iter);
		if( !pTalisman )
		{
			++iter;
			continue;
		}

		if( pTalisman->GetEndTime() < FRAMEGETTIME() )
		{
			vEndTalismanList.push_back( pTalisman );
			iter = m_vTalismanList.erase( iter );
		}
		else
			++ iter;
	}

	int iDestroyCnt = vEndTalismanList.size();

	if( iDestroyCnt == 0 ) return;
	bool bRemoveBuff = false;
	if( iDestroyCnt == iCurCnt )
	{
		bRemoveBuff = vEndTalismanList[0]->RemoveTalismanBuff( pOwner );
	}

	for( int i=0; i < iDestroyCnt; ++i )
	{
		vEndTalismanList[i]->DestroyTalisman( pOwner );
		delete vEndTalismanList[i];
	}

	vEndTalismanList.clear();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << false;			//Create
		kPacket << bRemoveBuff;		//Buff
		kPacket << iDestroyCnt;		//Cnt
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCreateStaffItem::CheckWeaponDefenseState( ioBaseChar *pAttacker, ioBaseChar *pOwner )
{
	if( m_iDefenseWasteTalisman == 0 ) return;

	int iCnt = m_vTalismanList.size();
	if( iCnt == 0 ) return;

	int iDestroyCnt = min( iCnt, m_iDefenseWasteTalisman );

	TalismanList::iterator iter = m_vTalismanList.begin();

	bool bRemoveBuff = false;
	if( iCnt == iDestroyCnt )
		bRemoveBuff = (*iter)->RemoveTalismanBuff( pOwner );

	int iCurCnt = 0;
	while( iter != m_vTalismanList.end() && iCurCnt < iDestroyCnt )
	{
		(*iter)->DestroyTalisman( pOwner );
		delete (*iter);

		iter = m_vTalismanList.erase( iter );
		iCurCnt++;
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << false;			//Create
		kPacket << bRemoveBuff;		//Buff
		kPacket << iDestroyCnt;		//Cnt
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCreateStaffItem::CheckWoundedState( ioBaseChar *pOwner )
{
	if( m_iWoundedWasteTalisman == 0 ) return;

	int iCnt = m_vTalismanList.size();
	if( iCnt == 0 ) return;

	int iDestroyCnt = min( iCnt, m_iWoundedWasteTalisman );

	TalismanList::iterator iter = m_vTalismanList.begin();

	bool bRemoveBuff = false;
	if( iCnt == iDestroyCnt )
		bRemoveBuff = (*iter)->RemoveTalismanBuff( pOwner );

	int iCurCnt = 0;
	while( iter != m_vTalismanList.end() && iCurCnt < iDestroyCnt )
	{
		(*iter)->DestroyTalisman( pOwner );
		delete (*iter);

		iter = m_vTalismanList.erase( iter );
		iCurCnt++;
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << false;			//Create
		kPacket << bRemoveBuff;		//Buff
		kPacket << iDestroyCnt;		//Cnt
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioCreateStaffItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->EndEffect( m_GatherEffect );
}

void ioCreateStaffItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( iCurCombo == 0 )
	{
		m_CreateState = CS_CHARGING;

		m_dwChargeStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;

		pOwner->SetState( CS_ATTACK );
		pOwner->SetChargingState( true );

		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );
		
		int iAniID = pGrp->GetAnimationIdx( m_PreGatherAnimation );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
	}
	else
	{
		ioAttackableItem::SetNormalAttack( pOwner, iCurCombo );
		m_CreateState = CS_NONE;
	}
}

void ioCreateStaffItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_CreateState = CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_CreateState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCreateStaffItem::ChangeToCreateing( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetLoopAni( m_GatherAnimation, FLOAT100 );

	m_CreateState = CS_CREATEING;
	m_dwCreateStartTime = FRAMEGETTIME();
	pOwner->AttachEffect( m_GatherEffect );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_CreateState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCreateStaffItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_CreateState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		ioAttackableItem::CheckNormalAttack( pOwner );
		break;
	case CS_CREATEING:
		OnCreateing( pOwner );
		break;
	}
}

void ioCreateStaffItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwChargeStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			pOwner->EndEffect( m_GatherEffect );
			ChangeToCreateing( pOwner );
		}
	}
	else
	{
		pOwner->SetChargingState( false );
		ChangeToNormalAttack( pOwner );
	}
}

void ioCreateStaffItem::OnCreateing( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( !pOwner->IsAttackKeyDown() )	// ¹öÆ° ¶À
	{
		pOwner->SetChargingState( false );
		pOwner->SetState( CS_DELAY );

		m_CreateState = CS_NONE;
		m_dwCreateStartTime = 0;
		pOwner->EndEffect( m_GatherEffect );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)m_CreateState;
			kPacket << m_iCurCombo;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		return;
	}

	// Create
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwCreateStartTime + m_dwCreateTime < dwCurTime )
	{
		CheckCreateTalisman( pOwner );
		m_dwCreateStartTime = dwCurTime;
	}
}

void ioCreateStaffItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iCreateState;
	ioHashString szTarget;
	rkPacket >> iCreateState;
	rkPacket >> m_iCurCombo;

	switch( iCreateState )
	{
	case CS_NORMAL_ATTACK:
		ioAttackableItem::SetNormalAttack( pOwner, m_iCurCombo );
		m_CreateState = CS_NORMAL_ATTACK;
		break;
	case CS_CREATEING:
		pOwner->EndEffect( m_GatherEffect );
		ChangeToCreateing( pOwner );
		break;
	case CS_NONE:
		m_CreateState = CS_NONE;
		pOwner->EndEffect( m_GatherEffect );
		pOwner->SetState( CS_DELAY );
		break;
	}
}

void ioCreateStaffItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	bool bCreate, bBuff;
	int iCnt;
	rkPacket >> bCreate >> bBuff >> iCnt;

	if( bCreate )
		ApplyCreateTalisman( pOwner, bBuff, iCnt );
	else
		ApplyDestroyTalisman( pOwner, bBuff, iCnt );
}

ioWeaponItem::WeaponSubType ioCreateStaffItem::GetSubType() const
{
	return WST_CREATE_STAFF;
}

bool ioCreateStaffItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_CreateState == CS_CHARGING ||
			m_CreateState == CS_CREATEING )
			return true;
	}

	return false;
}

void ioCreateStaffItem::UpdateSkillGauge( ioBaseChar *pOwner, float fRate, bool bUpdate )
{
	if( m_dwTicCheckTime == 0 )
		return;

	float fRecoveryGauge = 0.0f;

	if( pOwner->GetState() == CS_RUN ||
		pOwner->GetState() == CS_DELAY ||
		pOwner->GetState() == CS_SPECIAL_MOTION ||
		pOwner->GetState() == CS_ETCITEM_MOTION || 
		IsCanUpdateGauge(pOwner) )
	{
		fRecoveryGauge = m_fDefaultGauge * pOwner->GetDelayRunGaugeRate();
	}
	else if( pOwner->GetState() == CS_DIE ||
			 pOwner->GetState() == CS_VIEW ||
			 pOwner->GetState() == CS_LOADING ||
			 pOwner->GetState() == CS_OBSERVER )
	{
		bUpdate = false;
	}
	else
	{
		fRecoveryGauge = m_fDefaultGauge * pOwner->GetEtcGaugeRate();
	}


	DWORD dwCurTime = FRAMEGETTIME();
	int iCurRecoveryTic = (int)pOwner->GetDefaultRecoveryGaugeTic();

	float fPreGauge = m_fCurSkillGauge;
	float fNewRecovery = fRecoveryGauge * fRate;

	int iTickGap = (int)( dwCurTime - m_dwTicCheckTime );
	if( iTickGap >= iCurRecoveryTic )
	{
		while( iTickGap >= iCurRecoveryTic )
		{
			if( bUpdate )
			{
				m_fCurSkillGauge = min( m_fCurSkillGauge+fNewRecovery, FLOAT100 );
			}

			iTickGap -= iCurRecoveryTic;
		}

		m_dwTicCheckTime = dwCurTime - iTickGap;
	}

//#ifndef SHIPPING 
//	if( Setting::ShowExtraInfo() )
//	{
//		float fMaxGauge = GetMaxSkillGauge( pOwner->GetCharName() );
//
//		if( fMaxGauge < FLOAT100 )
//		{
//			if( fPreGauge == 0.0f && m_fCurSkillGauge > 0.0f )
//			{
//				LOG.PrintTimeAndLog( 0, "SkillCheck - %s, MaxGauge: %f", GetSkillName().c_str(), fMaxGauge );
//				LOG.PrintTimeAndLog( 0, "SkillCheck - %s, RecoveryStart: %d", GetSkillName().c_str(), FRAMEGETTIME() );
//			}
//
//			if( fPreGauge < fMaxGauge && m_fCurSkillGauge >= fMaxGauge )
//			{
//				LOG.PrintTimeAndLog( 0, "SkillCheck - %s, RecoveryEnd: %d", GetSkillName().c_str(), FRAMEGETTIME() );
//			}
//		}
//	}
//#endif

	if( fRate > 0.0f )
		m_bUpdateEnableSkillGauge = bUpdate;
	else
		m_bUpdateEnableSkillGauge = false;
}

bool ioCreateStaffItem::IsEnableExtraGauge( ioBaseChar *pChar )
{
	if( m_bGaugeUpdateChargeAttack && IsChargeAttackState( pChar ) )
		return false;

	return true;
}

void ioCreateStaffItem::CheckCreateTalisman( ioBaseChar *pOwner )
{
	if( m_iCreateTalismanCnt == 0 ) return;

	int iCurCnt = m_vTalismanList.size();
	if( iCurCnt >= m_iMaxTalisman ) return;

	int iCreateCnt = min( (m_iMaxTalisman-iCurCnt), m_iCreateTalismanCnt );

	TalismanList vCreateList;
	for( int i=0; i < iCreateCnt; ++i )
	{
		ioTalisman *pTalisman = g_TalismanMaker.CreateTalisman( pOwner, m_iTalismanNum );
		if( pTalisman )
			vCreateList.push_back( pTalisman );
	}

	if( vCreateList.size() == 0 ) return;

	bool bAddBuff = false;
	if( iCurCnt == 0 )
	{
		bAddBuff = vCreateList[0]->AddTalismanBuff( pOwner );
	}

	for( int j=0; j < iCreateCnt; ++j )
	{
		m_vTalismanList.push_back( vCreateList[j] );
	}

	vCreateList.clear();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << true;			//Create
		kPacket << bAddBuff;		//Buff
		kPacket << iCreateCnt;		//CreateCnt
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCreateStaffItem::ApplyCreateTalisman( ioBaseChar *pOwner, bool bAddBuff, int iCreateCnt )
{
	if( !pOwner ) return;
	if( iCreateCnt == 0 ) return;

	int iCurCnt = m_vTalismanList.size();
	if( iCurCnt >= m_iMaxTalisman ) return;

	int iNewCreateCnt = min( (m_iMaxTalisman-iCurCnt), iCreateCnt );

	TalismanList vCreateList;
	for( int i=0; i < iNewCreateCnt; ++i )
	{
		ioTalisman *pTalisman = g_TalismanMaker.CreateTalisman( pOwner, m_iTalismanNum );
		if( pTalisman )
			vCreateList.push_back( pTalisman );
	}

	if( vCreateList.size() == 0 ) return;

	if( bAddBuff )
	{
		vCreateList[0]->AddTalismanBuff( pOwner );
	}

	iNewCreateCnt = vCreateList.size();
	for( int j=0; j < iNewCreateCnt; ++j )
	{
		m_vTalismanList.push_back( vCreateList[j] );
	}

	vCreateList.clear();
}

void ioCreateStaffItem::ApplyDestroyTalisman( ioBaseChar *pOwner, bool bRemoveBuff, int iDestroyCnt )
{
	if( !pOwner ) return;
	if( iDestroyCnt == 0 ) return;

	TalismanList vEndTalismanList;
	int iCurCnt = m_vTalismanList.size();
	if( iCurCnt == 0 ) return;

	int iNewDestroyCnt = min( iCurCnt, iDestroyCnt );

	TalismanList::iterator iter = m_vTalismanList.begin();
	if( bRemoveBuff )
	{
		(*iter)->RemoveTalismanBuff( pOwner );
	}

	int iCheckCnt = 0;
	while( iter != m_vTalismanList.end() && iCheckCnt < iNewDestroyCnt )
	{
		(*iter)->DestroyTalisman( pOwner );
		delete (*iter);

		iter = m_vTalismanList.erase( iter );
		iCheckCnt++;
	}
}

bool ioCreateStaffItem::IsCanFire( const ioBaseChar* pOwner, int iFireCnt, bool bNormalAttack ) const
{
	if( bNormalAttack )
		return true;

	if( m_iSkillNeedTalisman <= 0 )
		return true;


	int iCurCnt = m_vTalismanList.size();
	if( m_iSkillNeedTalisman > iCurCnt )
		return false;

	return true;
}

void ioCreateStaffItem::ReduceNeedGauge( float fUseGauge, ioBaseChar *pOwner )
{
	ioHashString szName;
	if( pOwner )
		szName = pOwner->GetCharName();

	float fSecoundGauge = m_fCurSkillGauge - GetMaxSkillGauge(szName);

	m_fCurSkillGauge -= fSecoundGauge;
	m_fCurSkillGauge -= fUseGauge;
	m_fCurSkillGauge = max( m_fCurSkillGauge, 0.0f );

	int iCurCnt = m_vTalismanList.size();
	if( iCurCnt == 0 ) return;

	int iDestroyCnt = min( iCurCnt, m_iSkillNeedTalisman );
	TalismanList::iterator iter = m_vTalismanList.begin();

	bool bRemoveBuff = false;
	if( iCurCnt == iDestroyCnt )
		bRemoveBuff = (*iter)->RemoveTalismanBuff( pOwner );

	int iCheckCnt = 0;
	while( iter != m_vTalismanList.end() && iCheckCnt < iDestroyCnt )
	{
		(*iter)->DestroyTalisman( pOwner );
		delete (*iter);

		iter = m_vTalismanList.erase( iter );
		iCheckCnt++;
	}
}

int ioCreateStaffItem::GetCurTalisman()
{
	return m_vTalismanList.size();
}

int ioCreateStaffItem::GetMaxBullet()
{
	return m_iMaxTalisman;
}

int ioCreateStaffItem::GetCurBullet()
{
	int iCurCnt = m_vTalismanList.size();
	return iCurCnt;
}
