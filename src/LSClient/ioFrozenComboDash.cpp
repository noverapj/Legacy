#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioFrozenComboDash.h"

#include "ItemDefine.h"
#include "WeaponDefine.h"


ioFrozenComboDash::ioFrozenComboDash()
{
	m_DashState					= CDS_NONE;
	m_dwChargingStartTime		= 0;
	m_dwFireTime				= 0;
	m_iCurComboCnt				= 0;
	m_dwCancelCheckTime			= 0;
	m_dwActionEndTime			= 0;
	m_dwPreReserveEnableTime	= 0;
	m_bSetCombo					= false;
	m_fCurChargeRate			= 0.0f;
	m_fInputEventAnimationRate	= FLOAT1;
	m_bCancelChargingAttack		= false;

	m_szInputEventAnimation.Clear();
}

ioFrozenComboDash::ioFrozenComboDash( const ioFrozenComboDash &rhs )
: ioExtendDash( rhs ),
m_AttackAttribute( rhs.m_AttackAttribute ),
m_ExtendAttackList( rhs.m_ExtendAttackList ),
m_dwChargingTime( rhs.m_dwChargingTime ),
m_szGatheringAni( rhs.m_szGatheringAni ),
m_fChargingSpeedRate( rhs.m_fChargingSpeedRate ),
m_ChargingAniList( rhs.m_ChargingAniList )
{
	m_DashState					= CDS_NONE;
	m_dwChargingStartTime		= 0;
	m_dwFireTime				= 0;
	m_iCurComboCnt				= 0;
	m_dwCancelCheckTime			= 0;
	m_dwActionEndTime			= 0;
	m_dwPreReserveEnableTime	= 0;
	m_bSetCombo					= false;
	m_fCurChargeRate			= 0.0f;
	m_fInputEventAnimationRate	= FLOAT1;
	m_bCancelChargingAttack		= false;

	m_szInputEventAnimation.Clear();
}

ioExtendDash* ioFrozenComboDash::Clone()
{
	return new ioFrozenComboDash( *this );
}

ioFrozenComboDash::~ioFrozenComboDash()
{
}

DashType ioFrozenComboDash::GetType() const
{
	return DT_FROZEN_COMBO;
}

void ioFrozenComboDash::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendDash::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_dwChargingTime = rkLoader.LoadInt_e( "dash_charge_time", 0 );
	m_fChargingSpeedRate = rkLoader.LoadFloat_e( "dash_charge_speed_rate", 0.0f );
	
	rkLoader.LoadString_e( "gathering_dash_animation", "", szBuf, MAX_PATH );
	m_szGatheringAni = szBuf;
	
	m_AttackAttribute.Init();
	LoadAttackAttribute_e( "dash_attack", m_AttackAttribute, rkLoader );

	// 차지 동작
	int iMaxChargeAni = rkLoader.LoadInt_e( "charging_dash_ani_cnt", 0 );
	for( int i=0; i < iMaxChargeAni; ++i )
	{
		wsprintf_e( szBuf, "charging_dash_ani%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_ChargingAniList.push_back( szBuf );
	}

	// Extend combo : +D~ 1타 20Gauge, 2타~8타 5Gauge ( (20 * 1타)  + (5 * 7타) = 55 )
	int iMax = rkLoader.LoadInt_e( "extend_dash_max_cnt", 0 );
	for( int i=0; i < iMax; ++i )
	{
		AttackAttribute eAttribute;

		wsprintf_e( szBuf, "extend_dash_attack%d", i+1);
		LoadAttackAttribute( szBuf, eAttribute, rkLoader );

		m_ExtendAttackList.push_back( eAttribute );
	}
}

bool ioFrozenComboDash::StartDash( ioBaseChar *pOwner )
{
	if( !pOwner )	{	return false;	}

	m_bSetCombo					= false;
	m_iCurComboCnt				= 0;
	m_dwChargingStartTime		= 0;
	m_dwActionEndTime			= 0;
	m_dwPreReserveEnableTime	= 0;
	m_DashState					= CDS_NONE;
	m_dwCancelCheckTime			= 0;
	m_fCurChargeRate			= 0.0f;
	m_bCancelChargingAttack		= false;

	pOwner->NormalAttackOver( false );
	pOwner->InitExtendAttackTagInfo();
	pOwner->SetNoWoundState( false );
	pOwner->SetState( CS_EXTEND_DASH );

	ChangeToCharging( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << CDS_NONE;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return true;
}

void ioFrozenComboDash::ChangeToCharging( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( m_ChargingAniList.empty() )
		return;

	ioHashString szCurChargingAni;

	int iChargingCnt = m_ChargingAniList.size();
	int iCurIndex = m_iCurComboCnt;

	if( m_bSetCombo )
		iCurIndex = m_iCurComboCnt+1;

	if( COMPARE( iCurIndex, 0, iChargingCnt ) )
	{
		szCurChargingAni = m_ChargingAniList[iCurIndex];
	}
	else
	{
		szCurChargingAni = m_ChargingAniList[0];
	}

	int iAniID = pGrp->GetAnimationIdx( szCurChargingAni );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( szCurChargingAni, FLOAT100 );

	m_dwChargingStartTime = FRAMEGETTIME();

	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;
	m_dwPreReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;	
	m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;	

	m_DashState = CDS_CHARGE;
}

void ioFrozenComboDash::ProcessDash( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_DashState )
	{
	case CDS_CHARGE:
		ProcessCharging( pOwner );
		break;
	case CDS_GATHERING:
		break;
	case CDS_NORMAL_ATTACK:
		ProcessNormalComboDash( pOwner );
		break;
	case CDS_EXTEND_COMBO:
		ProcessExtendComboDash( pOwner );
		break;
	}
}

void ioFrozenComboDash::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwChargingStartTime+m_dwChargingTime < dwCurTime )
		{
			// 게이지 검사 20, 5 
			if( CheckEnableExtendComboState( pOwner ) )
			{
				SetExtendComboState( pOwner );				
			}
			else
			{
				pOwner->SetState( CS_DELAY );
			}
			return;
		}
	}
	else
	{
		// 대시 1타에서는 일반 공격으로 간다.
		if( m_bSetCombo ) 
		{
			ioObjectItem *pItem = pOwner->GetObject();

			if( ToDrinkItem( pItem ) )
			{
				pOwner->SetDrinkState();
			}
			else
			{
				if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
				{
					pOwner->SetNormalAttack( 0 );
				}
			}
			return;
			//pOwner->SetState( CS_DELAY );
			//return;
		}
		ChangeToNormalAttack( pOwner );
		//LOG.PrintTimeAndLog( 0, "ioFrozenComboDash::ProcessCharging - ChangeToNormalAttack");
		return;
	}
}

void ioFrozenComboDash::ProcessGathering( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( !pOwner->IsAttackKeyDown() )
	{
		ChangeToExtendAttack( pOwner );
	}
}

bool ioFrozenComboDash::CheckEnableExtendComboState( ioBaseChar *pOwner )
{
	if( NULL == pOwner )	{	return false;	}

	if( m_ExtendAttackList.empty() )	{	return false;	}

	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioFrozenItem* pFrozenItem = ToFrozenItem( pItem );
	if( pFrozenItem )
	{
		// 처음 콤보 시작
		int iCurBullet = pFrozenItem->GetCurBullet();
		int iNeedBullet = pFrozenItem->GetNeedBullet();
		int iAddNeedBullet = pFrozenItem->GetAddNeedBullet();

		if( (0 == m_iCurComboCnt) && (iCurBullet >= iNeedBullet) )
		{
			// 게이지 사용 20
			pFrozenItem->CheckDecreaseGauge();
			return true;
		}
		else if( (0 < m_iCurComboCnt) && (iCurBullet >= iAddNeedBullet) )
		{
			// 추가 게이지 5
			pFrozenItem->CheckAddDecreaseGauge();
			return true;
		}
	}

	return false;
}

void ioFrozenComboDash::SetExtendComboState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	int iMaxCnt = m_ExtendAttackList.size();
	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	int nCurComboCnt = 0;
	nCurComboCnt = m_iCurComboCnt;

	const AttackAttribute &rkAttribute = m_ExtendAttackList[nCurComboCnt];

	int iAniID = pGrp->GetAnimationIdx( rkAttribute.m_AttackAnimation );
	m_szInputEventAnimation = rkAttribute.m_AttackAnimation;
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioFrozenComboDash::SetExtendComboState - %s Animation is Not Exist",
								rkAttribute.m_AttackAnimation.c_str() );
		return;
	}

	float fTimeRate = rkAttribute.m_fAttackAniRate;
	m_fInputEventAnimationRate = fTimeRate;
	DWORD dwPreDelay = rkAttribute.m_dwPreDelay;

	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;
	m_dwPreReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );

	pOwner->RefreshFireTimeList( iAniID, rkAttribute.m_vWeaponInfoList, FTT_EXTEND_DASH_ATTACK, fTimeRate, dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetNoWoundState( false );

	pOwner->SetAutoTarget( ATT_DASH );
	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, (float)dwPreDelay, true );

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwDuration = dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + rkAttribute.m_dwEndDelay;

	DWORD dwEndTime = pGrp->GetAnimationEventTime_e( iAniID, "landing" );
	if( dwEndTime == 0 )
	{
		dwEndTime = pGrp->GetAnimationFullTime( iAniID );
	}

	int iEndAniID = pGrp->GetAnimationIdx( rkAttribute.m_AttackEndAnimation );
	if( iEndAniID == -1 )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

		CheckKeyReserveTime( pOwner, iAniID, fTimeRate );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		float fReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
		if( fReserveTime > 0.0f )
		{
			m_dwPreReserveEnableTime = dwCurTime + fReserveTime;
		}

		float fWaitTime = dwEndTime * fTimeRate + dwPreDelay;
		float fEndTimeRate = rkAttribute.m_fAttackEndAniRate;

		pGrp->SetActionAni( iEndAniID, 0.0f, FLOAT100, FLOAT1, FLOAT1/fEndTimeRate, fWaitTime, 0.0f, true );

		CheckKeyReserveTime( pOwner, iEndAniID, fEndTimeRate );
		if( m_dwReserveEnableTime > 0 )
		{
			m_dwReserveEnableTime = dwDuration;
		}

		dwDuration += pGrp->GetAnimationFullTime( iEndAniID ) * fEndTimeRate + rkAttribute.m_dwEndDelay;
	}

	m_dwCancelCheckTime = dwCurTime;
	m_dwActionStartTime = dwCurTime;
	m_dwActionEndTime = dwCurTime + dwDuration;

	pOwner->SetReservedSliding( rkAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	DWORD dwTrackingTime = dwCurTime + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_dwFireTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	m_bSetCombo = true;
	m_DashState = CDS_EXTEND_COMBO;	

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << CDS_EXTEND_COMBO;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	// send 이후
	m_iCurComboCnt++;
}

void ioFrozenComboDash::ProcessExtendComboDash( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwEndTime = m_dwActionEndTime + pOwner->GetActionStopTotalDelay();

	if( dwEndTime > dwCurTime )
	{
		if( m_dwReserveEnableTime > 0 && m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			// 예약체크
			int iMaxCnt = m_ExtendAttackList.size();
			if( m_bSetCombo && m_iCurComboCnt >= iMaxCnt )
			{
				CheckExtraReserve( pOwner );
			}
			else
			{
				CheckExtendReserve( pOwner );
			}
			return;
		}
	}
	else
	{
		ProcessKeyReserve( pOwner );
	}
}

void ioFrozenComboDash::ProcessNormalComboDash( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwEndTime = m_dwActionEndTime + pOwner->GetActionStopTotalDelay();

	// 대시 1타에서 D~ 시 캔슬 D~ 공격이 발생 되게 한다. 
	if( CheckAniDash_Input_Speical_Dash( pOwner ) )
		return;

	if( dwEndTime > dwCurTime )
	{
		if( m_dwReserveEnableEndTime == 0 && m_dwReserveEnableTime > 0 && m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			// 예약체크
			int iMaxCnt = pOwner->GetExtraDashAttackMax();
			if( m_bSetCombo && m_iCurComboCnt >= iMaxCnt )
			{
				CheckExtraReserve( pOwner );
			}
			else
			{
				CheckKeyReserve( pOwner );
			}
			return;
		}
		else if( m_dwReserveEnableEndTime > 0 && m_dwReserveEnableTime > 0 && 
			m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime &&
			m_dwReserveEnableEndTime + pOwner->GetActionStopTotalDelay() >= dwCurTime )
		{
			// 예약체크
			int iMaxCnt = pOwner->GetExtraDashAttackMax();
			if( m_bSetCombo && m_iCurComboCnt >= iMaxCnt )
			{
				CheckExtraReserve( pOwner );
			}
			else
			{
				CheckKeyReserve( pOwner );
			}			
			return;
		}
	}
	else
	{
		ProcessKeyReserve( pOwner );
		return;
	}
}

void ioFrozenComboDash::ChangeToGathering( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	int iAniID = pGrp->GetAnimationIdx( m_szGatheringAni );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetLoopAni( iAniID, FLOAT100 );

	m_DashState = CDS_GATHERING;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << (int)m_DashState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioFrozenComboDash::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	const AttackAttribute *pAttr = pOwner->GetAttackAttribute( AT_DASH_ATTACK );
	if( !pAttr )
		return;

	// 1타에서 D~ 시 캔슬 D~ 공격이 발생 되게 한다. 
	m_szInputEventAnimation = pAttr->m_AttackAnimation;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioFrozenComboDash::ChangeToNormalAttack - %s Animation is Not Exist",
								pAttr->m_AttackAnimation.c_str() );
		return;
	}

	float fTimeRate = pAttr->m_fAttackAniRate;
	m_fInputEventAnimationRate = fTimeRate;

	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;
	m_dwPreReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->RefreshFireTimeList( iAniID, pAttr->m_vWeaponInfoList, FTT_EXTEND_DASH_ATTACK, fTimeRate, pAttr->m_dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetNoWoundState( false );

	pOwner->SetState( CS_EXTEND_DASH );
	pOwner->SetAutoTarget( ATT_DASH );
	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, (float)pAttr->m_dwPreDelay, true );

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwDuration = pAttr->m_dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + pAttr->m_dwEndDelay;

	m_dwCancelCheckTime = dwCurTime;
	m_dwActionStartTime= dwCurTime;
	m_dwActionEndTime = dwCurTime + dwDuration;

	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, pAttr->m_dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, pAttr->m_dwPreDelay );

	DWORD dwTrackingTime = FRAMEGETTIME() + pAttr->m_dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );	

	m_bSetCombo = true;
	m_bCancelChargingAttack = true;	// 일반공격에서 D~ 시 캔슬 D~ 공격이 발생 되게 한다. 
	m_DashState = CDS_NORMAL_ATTACK;

	CheckKeyReserveTime( pOwner, iAniID, pAttr->m_fAttackAniRate );
	pOwner->InitExtendAttackTagInfo();
	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, pAttr->m_dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, pAttr->m_dwPreDelay );
	

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << CDS_NORMAL_ATTACK;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioFrozenComboDash::ChangeToExtendAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
}

void ioFrozenComboDash::ExtendDashEnd( ioBaseChar *pOwner )
{
	m_DashState					= CDS_NONE;
	m_bSetCombo					= false;
	m_iCurComboCnt				= 0;
	m_dwChargingStartTime		= 0;
	m_bCancelChargingAttack		= false;
	m_dwActionEndTime			= 0;
	m_dwReserveEnableTime		= 0;
	m_dwPreReserveEnableTime	= 0;
	m_iSkillInput				= -1;
	m_bEnableDefenseState		= false;
	m_bEnableAttackState		= false;
	m_bEnableJumpState			= false;	
	m_eSpecialDashCurrDirkey	= ioUserKeyInput::DKI_NONE;

	if( pOwner )
	{
		pOwner->InitExtendAttackTagInfo();
	}
}

void ioFrozenComboDash::ApplyDashState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurComboCnt;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	pOwner->SetTargetRotToRotate( qtRot, true, false );

	switch( iState )
	{
	case CDS_NONE:
		StartDash( pOwner );
		break;
	case CDS_CHARGE:
		ChangeToCharging( pOwner );
		break;
	case CDS_NORMAL_ATTACK:
		ChangeToNormalAttack( pOwner );
		break;
	case CDS_EXTEND_COMBO:
		SetExtendComboState( pOwner );
		break;
	}
}

void ioFrozenComboDash::ApplyDash( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case DST_STATE:
		ApplyDashState( pOwner, rkPacket );
		break;
	case DST_KEY:
		break;
	}
}

void ioFrozenComboDash::CheckExtraReserve( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	bool bReserveSkill = false;
	int iSkillNum = pOwner->GetSkillKeyInput();
	if( iSkillNum >= 0 )
	{
		m_iSkillInput = iSkillNum;
		bReserveSkill = true;
	}

	if( pOwner->IsDefenseKeyDown() && !m_bEnableDefenseState && !bReserveSkill )
	{
		m_bEnableDefenseState = true;
		m_bEnableAttackState = false;
		m_bEnableJumpState = false;		
		m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsJumpKey() && !m_bEnableJumpState && !bReserveSkill )
	{
		m_bEnableJumpState = true;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;		
		m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsAttackKey() && !m_bEnableAttackState && !bReserveSkill )
	{
		m_bEnableAttackState = true;
		m_bEnableDefenseState = false;
		m_bEnableJumpState = false;
		m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

		m_iSkillInput = -1;
	}
}

void ioFrozenComboDash::CheckExtendReserve( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	bool bReserveSkill = false;
	int iSkillNum = pOwner->GetSkillKeyInput();
	if( iSkillNum >= 0 )
	{
		m_iSkillInput = iSkillNum;
		bReserveSkill = true;
	}

	if( pOwner->IsDefenseKeyDown() && !m_bEnableDefenseState && !bReserveSkill )
	{
		m_bEnableDefenseState = true;
		m_bEnableAttackState = false;
		m_bEnableJumpState = false;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsJumpKey() && !m_bEnableJumpState && !bReserveSkill )
	{
		m_bEnableJumpState = true;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;
		m_iSkillInput = -1;
	}
	else if( pOwner->IsAttackKey() && !m_bEnableAttackState && !bReserveSkill )
	{
		ioUserKeyInput::DirKeyInput eCurKey = pOwner->CheckCurDirKey();

		if( pOwner->IsSettedDirection() )
		{
			float fYawD = 180.0f + 45.0f * ( eCurKey - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, true, false );
		}

		return;
	}
	else if( pOwner->IsAttackKeyDown() && !m_bEnableAttackState && !bReserveSkill )
	{
		if( m_dwChargingStartTime+m_dwChargingTime < dwCurTime )
		{
			m_DashState = CDS_CHARGE;
			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_EXTEND_DASH );
				kPacket << pOwner->GetCharName();
				kPacket << (int)GetType();
				kPacket << pOwner->GetHP().m_fCurValue;
				kPacket << DST_STATE;
				kPacket << CDS_CHARGE;
				kPacket << m_iCurComboCnt;
				kPacket << pOwner->GetTargetRot();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return;
		}
	}
}

// 1타에서 D~ 시 캔슬 D~ 공격이 발생 되게 한다.
bool ioFrozenComboDash::CheckAniDash_Input_Speical_Dash( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	if( m_szInputEventAnimation.IsEmpty() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	int iAniID = pGrp->GetAnimationIdx( m_szInputEventAnimation );		
	DWORD dwInputSpecialDashTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" );
	if( 0 < dwInputSpecialDashTime )
	{
		dwInputSpecialDashTime *= m_fInputEventAnimationRate;
		dwInputSpecialDashTime += m_dwActionStartTime;		
	}

	if( 0 < dwInputSpecialDashTime && dwCurTime > dwInputSpecialDashTime )
	{
		ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
		ioFrozenItem* pFrozenItem = ToFrozenItem( pItem );
		int nCurBullet = pFrozenItem->GetCurBullet();
		int nNeedBullet = pFrozenItem->GetNeedBullet();
		if( pFrozenItem && pOwner->IsAttackKeyDown() && nCurBullet >= nNeedBullet )
		{
			ChangeToCharging( pOwner );
			LOG.PrintTimeAndLog( 0, "ioFrozenComboDash::CheckAniDash_Input_Speical_Dash - pOwner->IsAttackKeyDown() : %d", pOwner->IsAttackKeyDown() );
			return true;
		}
	}

	return false;
}

void ioFrozenComboDash::CheckKeyReserve( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	bool bReserveSkill = false;
	int iSkillNum = pOwner->GetSkillKeyInput();
	if( iSkillNum >= 0 )
	{
		m_iSkillInput = iSkillNum;
		bReserveSkill = true;
	}

	bool bIsAttackKey = pOwner->IsAttackKey();
	if( bIsAttackKey )
	{
		LOG.PrintTimeAndLog( 0, "ioFrozenComboDash::CheckKeyReserve IsAttackKey: %d ", bIsAttackKey );
	}
	bool bEnableAttackState = m_bEnableAttackState;
	if( bEnableAttackState )
	{
		LOG.PrintTimeAndLog( 0, "ioFrozenComboDash::CheckKeyReserve m_bEnableAttackState: %d",  bEnableAttackState);
	}
	bool bIsAttackKeyDown = pOwner->IsAttackKeyDown();
	if( bIsAttackKeyDown )
	{
		LOG.PrintTimeAndLog( 0, "ioFrozenComboDash::CheckKeyReserve IsAttackKeyDown: %d",  bIsAttackKeyDown);
	}

	if( pOwner->IsDefenseKeyDown() && !m_bEnableDefenseState && !bReserveSkill )
	{
		m_bEnableDefenseState = true;
		m_bEnableAttackState = false;
		m_bEnableJumpState = false;		
		m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsAttackKey() && !m_bEnableAttackState && !bReserveSkill )
	{
		m_bEnableAttackState = true;
		m_bEnableDefenseState = false;
		m_bEnableJumpState = false;
		m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsJumpKey() && !m_bEnableJumpState && !bReserveSkill )
	{
		m_bEnableJumpState = true;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;		
		m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;
		m_iSkillInput = -1;
	}
}

void ioFrozenComboDash::ProcessKeyReserve( ioBaseChar *pOwner )
{
	// 공격키가 눌러져 있으면 공격 상태 처리
	if( pOwner->IsAttackKeyDown() && 
		!pOwner->IsJumpKeyDown() &&
		!pOwner->IsDefenseKeyDown() &&
		!pOwner->IsChatModeState(true) &&
		!pOwner->IsFloating() )
	{
		ioObjectItem *pItem = pOwner->GetObject();

		if( ToDrinkItem( pItem ) )
		{
			pOwner->SetDrinkState();
		}
		else
		{
			if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				pOwner->SetNormalAttack( 0 );
			}
		}
		return;
	}

	// 필살기 예약되었으면 필살기 발동
	ioUFCItem *pUFCItem = ToUFCItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pUFCItem && pUFCItem->CheckOnHand() && m_iSkillInput > 0 )
	{
		pUFCItem->ChangeToSpecialAttackFire( pOwner, ioUFCItem::SS_CHARGE_A + m_iSkillInput );
		m_iSkillInput = -1;
		return;
	}
	if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
	{
		m_iSkillInput = -1;
		return;
	}

	// 방어
	if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
	{
		pOwner->SetDefenseState();
		return;
	}

	// 점프
	if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
	{
		pOwner->SetJumpState();
		return;
	}

	// 공격
	if( m_bEnableAttackState )
	{
		ioObjectItem *pItem = pOwner->GetObject();

		if( ToDrinkItem( pItem ) )
		{
			pOwner->SetDrinkState();
		}
		else
		{
			if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				pOwner->SetNormalAttack( 0 );
			}
		}
		return;
	}

	// 대기
	pOwner->ClearAttackFireTimeAndSkill();
	pOwner->ClearReservedSliding();
	pOwner->SetState( CS_DELAY );
}

void ioFrozenComboDash::SetExtraInfoToWeapon( ioBaseChar *pOwner, ioPlayStage *pStage, ioWeapon *pWeapon, const ioHashString &szName )
{
	if( !pWeapon ) return;

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );

	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( false, false );
		}
	}
}