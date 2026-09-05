
#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioHakumenItem.h"

#include "WeaponDefine.h"

ioHakumenItem::ioHakumenItem()
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_CurDefenseAttack.Init();
	m_fCurGauge = 0.0f;

	m_DefenseState = DS_NONE;

	m_bJumpDefenseState = false;
}

ioHakumenItem::ioHakumenItem( const ioHakumenItem &rhs )
	: ioWeaponItem( rhs ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_DefenseAttack( rhs.m_DefenseAttack ),
	m_fMaxGauge( rhs.m_fMaxGauge ),
	m_fNeedGauge( rhs.m_fNeedGauge ),
	m_fIncreaseGauge( rhs.m_fIncreaseGauge ),
	m_fDecreaseGauge( rhs.m_fDecreaseGauge )
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_CurDefenseAttack.Init();
	m_fCurGauge = 0.0f;

	m_DefenseState = DS_NONE;
}

ioHakumenItem::~ioHakumenItem()
{
}

void ioHakumenItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	int iMaxCombo = GetMaxCombo();
	m_AttackReadyAniList.clear();
	for( int i=0; i < iMaxCombo; ++i )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	LoadDefenseInfo( rkLoader );

	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", FLOAT100 );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );

	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_gauge", FLOAT1 );
	m_fDecreaseGauge = rkLoader.LoadFloat_e( "decrease_gauge", 2.0f );
}

void ioHakumenItem::LoadDefenseInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];
	m_DefenseAttack.Init();


	rkLoader.LoadString_e( "defense_ready_motion", "", szBuf, MAX_PATH );
	m_DefenseAttack.m_stDefenseReadyMotion = szBuf;
	m_DefenseAttack.m_fDefenseReadyMotionRate = rkLoader.LoadFloat_e( "defense_ready_motion_rate", FLOAT1 );


	rkLoader.LoadString_e( "defense_wait_motion", "", szBuf, MAX_PATH );
	m_DefenseAttack.m_stDefenseMotion = szBuf;
	m_DefenseAttack.m_fDefenseMotionRate = rkLoader.LoadFloat_e( "defense_wait_motion_rate", FLOAT1 );

	wsprintf_e( szKey, "defense_attack" );
	LoadAttackAttribute( szKey, m_DefenseAttack.m_DefenseAttack, rkLoader );

	m_DefenseAttack.m_fEndJumpAmt = rkLoader.LoadFloat_e( "defense_jump_amt", 0.0f );
}

ioItem* ioHakumenItem::Clone()
{
	return new ioHakumenItem( *this );
}

ioWeaponItem::WeaponSubType ioHakumenItem::GetSubType() const
{
	return WST_HAKUMEN;
}

void ioHakumenItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	ClearState( pOwner );

	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = CS_NONE;
		m_iCurCombo = iCurCombo;

		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );
	}
	else
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAniList[iCurCombo] );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_ChargeState = CS_CHARGING;
		m_dwAttackStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );

		if( m_bChargeAutoTarget )
		{
			DWORD dwTrackingTime = FRAMEGETTIME();
			dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
			pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
		else
		{
			pOwner->StopAutoTargetTracking();
		}
	}
}

void ioHakumenItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_NORMAL_ATTACK:

		if( pOwner->IsNeedProcess() )
		{
			CheckCancelReserve( pOwner );

			if( ProcessCancel(pOwner) )
				return;
		}

		ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	}
}

void ioHakumenItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )	return;

	if( IsEnableGauge() && pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			SetDefenseState( pOwner, m_DefenseAttack, true );
			//DefenseAttackFire( pOwner );
			return;
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioHakumenItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	//
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		if( !m_bChargeAutoTarget )
		{
			pOwner->SetAutoTarget( ATT_NORMAL );
		}

		pOwner->SetNormalAttackByAttribute( *pAttribute );
		m_szCurAni = pAttribute->m_AttackAnimation;

		m_dwCancelCheckTime = dwCurTime;
		m_dwInputCancelCheckTime = dwCurTime;
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHakumenItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	switch( iState )
	{
	case CS_NONE:
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );

		m_ChargeState = CS_NONE;
		break;
	case CS_NORMAL_ATTACK:
		if( !m_bChargeAutoTarget )
			pOwner->ApplyAutoTargetInfo( rkPacket );
		ChangeToNormalAttack( pOwner );
		break;
	}
}

void ioHakumenItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case SST_FLY_ATTACK_START:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRot( qtRot );

//			ChangeFlyAttack( pOwner );
		}
		break;
	case SST_DEFENSE_STATE:
		{
			CheckDefenseState( pOwner, rkPacket );
		}
		break;
	}
}

void ioHakumenItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

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

bool ioHakumenItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_CHARGING:
			return true;
		}
	}

	return false;
}

void ioHakumenItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	m_ChargeState = CS_NONE;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	pOwner->ClearDirDoubleClick();

	m_fCurChargeRate = 0.0f;
}

void ioHakumenItem::SetDefenseState( ioBaseChar *pOwner, const HakumenDefenseInfo &rkDefenseAttackInfo, bool bSendNet )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( pOwner->GetState() == CS_JUMP )
		m_bJumpDefenseState = true;

	if( pOwner->IsNeedProcess() )
	{
		D3DXQUATERNION qtRot = pOwner->GetTargetRot();
		pOwner->SetTargetRotToRotate( qtRot, true );
	}

	pOwner->SetState( CS_HAKUMEN_DEFENSE );

	m_CurDefenseAttack = rkDefenseAttackInfo;

	ChangeDefenseStart( pOwner );

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_DEFENSE_STATE;
		kPacket << m_DefenseState;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fCurGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHakumenItem::ChangeDefenseStart( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurDefenseAttack.m_stDefenseReadyMotion );
	float fTimeRate  = m_CurDefenseAttack.m_fDefenseReadyMotionRate;
	DWORD dwPreDelay = 0;
	DWORD dwEndDelay = 0;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	m_DefenseState = DS_START;

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate  + dwPreDelay;

	DecreaseGauge();

	pOwner->ClearReservedSliding();
}

void ioHakumenItem::ChangeDefenseDelay( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurDefenseAttack.m_stDefenseMotion );
	float fTimeRate  = m_CurDefenseAttack.m_fDefenseMotionRate;
	DWORD dwPreDelay = 0;
	DWORD dwEndDelay = 0;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	m_DefenseState = DS_DELAY;

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate  + dwPreDelay;
}

void ioHakumenItem::DefenseAttackFire( ioBaseChar *pOwner, bool bLandAttack  )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitForcePower();
	pOwner->SetJumpPower( 0 );
	pOwner->SetAirJumpPower( 0 );
	pOwner->SetCurMoveSpeed( 0 );

	pOwner->ClearReservedSliding();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_DefenseState = DS_DEFENSE_ATTACK;

	int iAniID = pGrp->GetAnimationIdx( m_CurDefenseAttack.m_DefenseAttack.m_AttackAnimation );

	float fTimeRate  = m_CurDefenseAttack.m_DefenseAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_CurDefenseAttack.m_DefenseAttack.m_dwPreDelay;
	DWORD dwEndDelay = m_CurDefenseAttack.m_DefenseAttack.m_dwEndDelay;

	pOwner->RefreshFireTimeList( iAniID, m_CurDefenseAttack.m_DefenseAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate  + dwPreDelay;


	pOwner->SetReservedSliding( m_CurDefenseAttack.m_DefenseAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );


	//////////////////////////////////////////////////////////////////////////
	m_vAniRotateInfoList.clear();

	AniEventConstPtrList vAniRotateList;
	pGrp->GetAnimationEventConstPtrList( iAniID, "ani_rotate", vAniRotateList );

	AniEventConstPtrList::iterator iter_r = vAniRotateList.begin();
	while( iter_r != vAniRotateList.end() )
	{
		AniRotateInfo kInfo;

		kInfo.m_dwTime = (*iter_r)->fEventTime * fTimeRate;

		if( kInfo.m_dwTime > 0 )
			kInfo.m_dwTime += dwCurTime;
		else
			kInfo.m_dwTime = 0;

		if( !(*iter_r)->szSubInfo.IsEmpty() )
		{
			const StringVector &vParam = ioStringConverter::Split( (*iter_r)->szSubInfo.c_str(), " " );
			if( vParam.size() == 1 )
			{
				kInfo.m_fAngle = ioStringConverter::ParseFloat( vParam[0].c_str() );
			}
		}

		m_vAniRotateInfoList.push_back(kInfo);

		++iter_r;
	}

	if( !m_vAniRotateInfoList.empty() )
	{
		m_iCurAniRotate = 0;
		m_dwAniRotateTime = m_vAniRotateInfoList[m_iCurAniRotate].m_dwTime;
		m_fAniRotateAngle = m_vAniRotateInfoList[m_iCurAniRotate].m_fAngle;
	}
	//////////////////////////////////////////////////////////////////////////

	//send packet
	if( pOwner->IsNeedProcess() && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_DEFENSE_STATE;
		kPacket << m_DefenseState;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fCurGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHakumenItem::ProcessDefenseState( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_DefenseState )
	{
	case DS_START:
		if( m_dwMotionEndTime < dwCurTime )
			ChangeDefenseDelay(pOwner);
		break;

	case DS_DELAY:
		if( m_dwMotionEndTime < dwCurTime )
			ChangeDefenseEnd(pOwner);
		break;

	case DS_DEFENSE_ATTACK:
		if( m_dwMotionEndTime < dwCurTime )
			ChangeDefenseEnd(pOwner);

		CheckAniRotate( pOwner );
		break;

	case DS_END:
		break;
	}
}

void ioHakumenItem::CheckDefenseState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iFlyState;
	rkPacket >> iFlyState;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	switch( iFlyState )
	{
	case DS_START:
		{
			float fCurGauge;
			rkPacket >> fCurGauge;

			SetCurGauge( fCurGauge );

			pOwner->SetTargetRotToRotate( qtRot, true );

			SetDefenseState( pOwner, m_DefenseAttack, false );
		}
		break;
	case DS_DEFENSE_ATTACK:
		{
			float fCurGauge;
			rkPacket >> fCurGauge;

			SetCurGauge( fCurGauge );

			pOwner->SetTargetRotToRotate( qtRot, true );

			DefenseAttackFire( pOwner );
		}
		break;
	}
}

int ioHakumenItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioHakumenItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioHakumenItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioHakumenItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

void ioHakumenItem::InitGauge()
{
	m_fCurGauge = 0.0f;
}

void ioHakumenItem::MaxGauge()
{
	m_fCurGauge = m_fMaxGauge;
}

float ioHakumenItem::GetCurGauge()
{
	return m_fCurGauge;
}

void ioHakumenItem::SetCurGauge( float fGauge )
{
	m_fCurGauge = fGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

bool ioHakumenItem::IsEnableGauge()
{
	if( m_fCurGauge >= m_fNeedGauge )
		return true;

	return false;
}

void ioHakumenItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	// check owner state
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	if( pOwner->IsSystemState() )
		return;

	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
		if( m_fCurGauge < m_fMaxGauge )
		{
			float fGauge = m_fIncreaseGauge * fTimePerSec;

			m_fCurGauge += fGauge;
			m_fCurGauge = min( m_fCurGauge, m_fMaxGauge );
		}
		return;
	}
}

void ioHakumenItem::ClearState( ioBaseChar *pOwner )
{
	m_DefenseState = DS_NONE;

	m_CurDefenseAttack.Init();

	m_dwMotionEndTime = 0;

	m_bJumpDefenseState = false;
}

void ioHakumenItem::ChangeDefenseEnd( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_DefenseState = DS_END;
	
	pOwner->InitForcePower();
	pOwner->SetJumpPower( 0 );
	pOwner->SetAirJumpPower( 0 );
	pOwner->SetCurMoveSpeed( 0 );

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	//if( pOwner && pOwner->IsNeedProcess() )
	{
		pOwner->SetExtendAttackEndJump( m_CurDefenseAttack.m_fEndJumpAmt, m_fExtendLandingRate, false, false  );
		pOwner->ClearDirDoubleClick();
	}
}

void ioHakumenItem::CheckWeaponDefenseState( ioBaseChar *pAttacker, ioBaseChar *pOwner )
{
	if( m_DefenseState != DS_DELAY )
		return;

	DefenseAttackFire(pOwner);
}

void ioHakumenItem::CheckAniRotate( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( m_vAniRotateInfoList.empty() ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwAniRotateTime > 0 && m_dwAniRotateTime < dwCurTime )
	{
		if( m_fAniRotateAngle > 0.0f )
		{
			D3DXQUATERNION qtRot;
			D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(m_fAniRotateAngle) );
			D3DXVECTOR3 vMoveDir = qtRot * pChar->GetMoveDir();

			D3DXQUATERNION qtTargetRot;
			ioMath::CalcDirectionQuaternion( qtTargetRot, -vMoveDir );

			pChar->SetTargetRotAndMoveDirChange( qtTargetRot );
		}

		m_iCurAniRotate++;

		int iSize = m_vAniRotateInfoList.size();
		if( COMPARE(m_iCurAniRotate, 0, iSize) )
		{
			m_dwAniRotateTime = m_vAniRotateInfoList[m_iCurAniRotate].m_dwTime;
			m_fAniRotateAngle = m_vAniRotateInfoList[m_iCurAniRotate].m_fAngle;
		}
		else
		{
			m_dwAniRotateTime = 0;
			m_fAniRotateAngle = 0.0f;

		}
	}
}

bool ioHakumenItem::IsJumpDefenseState()
{ 
	return m_bJumpDefenseState; 
}

void ioHakumenItem::DecreaseGauge()
{
	m_fCurGauge -= m_fDecreaseGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

void ioHakumenItem::IncreaseChangeDamage( float fDamage, int iType, bool bBuff )
{
	if( iType == SSGT_HAKUMEN || iType == SSGT_NONE )
	{
		m_fCurGauge += fDamage;
		m_fCurGauge = max( 0.0f, m_fCurGauge );
		m_fCurGauge = min( m_fCurGauge, m_fMaxGauge );
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioHakumenDefenseSpecialState::ioHakumenDefenseSpecialState()
{
}

ioHakumenDefenseSpecialState::~ioHakumenDefenseSpecialState()
{
}

void ioHakumenDefenseSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioHakumenDefenseSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioHakumenItem *pHakumenItem = ToHakumenItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pHakumenItem )
		pHakumenItem->ClearState( pOwner );
}

void ioHakumenDefenseSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioHakumenItem* pHakumenItem = ToHakumenItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pHakumenItem )
		pHakumenItem->ProcessDefenseState( pOwner, 0 );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioHakumenDefenseSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioHakumenDefenseSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ioHakumenItem *pHakumenItem = ToHakumenItem( pOwner->GetWeapon() );
	if( !pHakumenItem )
		return false;
	
	if( !pHakumenItem->IsJumpDefenseState() )
		return false;

	return true;
}