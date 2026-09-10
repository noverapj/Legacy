

#include "stdafx.h"

#include "ioObjectDoubleShotItem.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

ioObjectDoubleShotItem::ioObjectDoubleShotItem()
{
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
}

ioObjectDoubleShotItem::ioObjectDoubleShotItem( const ioObjectDoubleShotItem &rhs )
: ioObjectItem( rhs ),
  m_bEnableObjectDefense( rhs.m_bEnableObjectDefense ),
  m_dwProtectTime( rhs.m_dwProtectTime ),
  m_TargetEffect( rhs.m_TargetEffect ),
  m_fTargetRange( rhs.m_fTargetRange ),
  m_fTargetAngle( rhs.m_fTargetAngle ),
  m_fTargetRangeJump( rhs.m_fTargetRangeJump ),
  m_fTargetAngleJump( rhs.m_fTargetAngleJump ),
  m_TargetWoundType( rhs.m_TargetWoundType ),
  m_TargetWoundTypeJump( rhs.m_TargetWoundTypeJump ),
  m_JumpAttribute( rhs.m_JumpAttribute )
{
}

ioObjectDoubleShotItem::~ioObjectDoubleShotItem()
{
}

void ioObjectDoubleShotItem::LoadProperty( ioINILoader &rkLoader )
{
	ioObjectItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	m_bEnableObjectDefense = rkLoader.LoadBool_e( "enable_object_defense", false );

	m_dwProtectTime = (DWORD)rkLoader.LoadInt_e( "protect_time", 0 );

	rkLoader.LoadString_e( "target_effect", "", szBuf, MAX_PATH );
	m_TargetEffect = szBuf;

	m_fTargetRange = rkLoader.LoadFloat_e( "target_range", 0.0f );
	m_fTargetAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fTargetRangeJump = rkLoader.LoadFloat_e( "target_range_jump", 0.0f );
	m_fTargetAngleJump = rkLoader.LoadFloat_e( "target_angle_jump", 0.0f );

	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", 0 );
	m_TargetWoundTypeJump = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type_jump", 0 );

	LoadAttackAttribute_e( "jump_attack", m_JumpAttribute, rkLoader );
}

ioItem* ioObjectDoubleShotItem::Clone()
{
	return new ioObjectDoubleShotItem( *this );
}

void ioObjectDoubleShotItem::OnEquiped( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioObjectItem::OnEquiped( pOwner );

	m_AttackState = AS_NONE;

	m_fAirJumpPowerRate = 0.0f;

	m_dwEquipTime = FRAMEGETTIME();

	m_dwMotionEndTime = 0;
	m_dwJumpAttackMotionEndTime = 0;
	m_dwAttackStartTime = 0;

	m_TargetName[0].Clear();
	m_TargetName[1].Clear();

	m_iCurCombo = 0;
	m_iCurComboCount = 0;

	m_bReserveJumpAttack = false;

	m_iCurUseCount = 0;
}

ioObjectItem::ObjectSubType ioObjectDoubleShotItem::GetObjectSubType() const
{
	return OST_DOUBLE_SHOT;
}

void ioObjectDoubleShotItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ChangeToAttackFire( pOwner );
}

void ioObjectDoubleShotItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	switch( m_AttackState )
	{
	case AS_ATTACK:
		{
			if( m_dwMotionEndTime < FRAMEGETTIME() )
			{
				if( m_bReserveKey )
				{
					ChangeToAttackFire( pOwner );
				}
				else
				{
					ChangeToAttackEnd( pOwner );
				}
			}
		}
		break;
	}

	ProcessReserveKeyInput( pOwner );
}

void ioObjectDoubleShotItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	rkPacket >> m_bReserveJumpAttack;
}

bool ioObjectDoubleShotItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	ChangeToJumpAttackFire( pOwner, false );
	return false;
}

void ioObjectDoubleShotItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner )
		return;

	if( pOwner->IsNeedProcess() )
	{
		if( m_dwEnableReserveTime < FRAMEGETTIME() && pOwner->IsAttackKey() && !m_bReserveJumpAttack )
		{
			m_bReserveJumpAttack = true;

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << m_bReserveJumpAttack;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}

	CheckAirJump( pOwner );
}

void ioObjectDoubleShotItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	float fCurDamageRate, fCurForceRate, fCurBlowRate;
	float fCurWoundedTimeRate, fCurLiveTimeRate;

	fCurDamageRate = fCurForceRate = fCurBlowRate = FLOAT1;
	fCurWoundedTimeRate = fCurLiveTimeRate = FLOAT1;

	if( m_fCurChargeRate > 0.0f && !bJumpAttack )
	{
		float fCurMaxDamage = max( 0.0f, (m_fMaxDamageRate - FLOAT1) );
		float fCurMaxForce = max( 0.0f, (m_fMaxForceRate - FLOAT1) );
		float fCurMaxBlow = max( 0.0f, (m_fMaxBlowRate - FLOAT1) );

		float fCurMaxWoundedTime = max( 0.0f, (m_fMaxWoundedTimeRate - FLOAT1) );
		float fCurMaxLiveTime = max( 0.0f, (m_fMaxLiveTimeRate - FLOAT1) );

		fCurDamageRate += fCurMaxDamage * m_fCurChargeRate;
		fCurForceRate += fCurMaxForce * m_fCurChargeRate;
		fCurBlowRate += fCurMaxBlow * m_fCurChargeRate;

		fCurWoundedTimeRate += fCurMaxWoundedTime * m_fCurChargeRate;
		fCurLiveTimeRate += fCurMaxLiveTime * m_fCurChargeRate;

		pWeapon->SetOwnerChargeDamageRate( fCurDamageRate );
		pWeapon->SetOwnerChargeForceRate( fCurForceRate );
		pWeapon->SetOwnerChargeBlowRate( fCurBlowRate );

		pWeapon->SetOwnerChargeWoundedTimeRate( fCurWoundedTimeRate );
		pWeapon->SetOwnerChargeLiveTimeRate( fCurLiveTimeRate );
	}
	
	if( m_iCurComboCount >= 2 )
		m_iCurComboCount = 1;

	ioBaseChar *pOwner = pWeapon->GetOwner();
	if( pOwner )
	{
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_TargetName[m_iCurComboCount] );
		if( pTarget )
		{
			D3DXVECTOR3 vDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
			D3DXVec3Normalize( & vDir, &vDir );
			pWeapon->SetMoveDir( vDir );

			if( m_iCurComboCount == 0 )
				pTarget->AttachEffect( m_TargetEffect );
			if( m_iCurComboCount == 1 && m_TargetName[m_iCurComboCount-1] != m_TargetName[m_iCurComboCount] )
				pTarget->AttachEffect( m_TargetEffect );
		}
	}
	m_iCurComboCount++;
}

bool ioObjectDoubleShotItem::IsProtectState()
{
	if( m_dwEquipTime > 0 && m_dwEquipTime + m_dwProtectTime > FRAMEGETTIME() )
		return true;

	return false;
}

bool ioObjectDoubleShotItem::EnableReduceGaugeOnSwitchBuff()
{
	if( m_dwAttackStartTime == 0 )
		return false;

	return true;
}

bool ioObjectDoubleShotItem::IsOneTimeWeaponItem() const
{
	if( m_iMaxUseCount > 0 && m_iCurUseCount == m_iMaxUseCount && m_iCurComboCount == 2 )
		return true;

	return false;
}

bool ioObjectDoubleShotItem::IsEnableDash() const
{
	return false;
}

bool ioObjectDoubleShotItem::IsEnableDefense() const
{
	return m_bEnableObjectDefense;
}

void ioObjectDoubleShotItem::OnReleased( ioBaseChar *pOwner )
{
	ioObjectItem::OnReleased( pOwner );
}

bool ioObjectDoubleShotItem::CheckJumpShotState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwJumpAttackMotionEndTime < dwCurTime && m_bReserveJumpAttack )
	{
		ChangeToJumpAttackFire( pOwner, true );
		return true;
	}

	if( m_bReserveJumpAttack )
		return true;

	if( m_dwJumpAttackMotionEndTime > FRAMEGETTIME() )
		return true;

	return false;
}

void ioObjectDoubleShotItem::ChangeToAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ClearCancelInfo();

	pOwner->SetState( CS_ATTACK );
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_AttackState = AS_ATTACK;
	m_iCurUseCount++;
	m_iCurUseCount = min( m_iCurUseCount, m_iMaxUseCount );
	m_iCurComboCount = 0;

	DWORD dwCurTime = FRAMEGETTIME();

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( pAttribute->m_AttackAnimation );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/pAttribute->m_fAttackAniRate );

		m_dwMotionEndTime = dwCurTime;
		m_dwMotionEndTime += pGrp->GetAnimationFullTime(iAniID) * pAttribute->m_fAttackAniRate;

		m_dwEnableReserveTime = dwCurTime;
		m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * pAttribute->m_fAttackAniRate;
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	if( m_dwProtectTime > 0 && m_dwAttackStartTime == 0 )
		m_dwAttackStartTime = FRAMEGETTIME();

	m_bReserveKey = false;

	CheckActiveCount( pOwner );

	CheckTarget( pOwner );
}

void ioObjectDoubleShotItem::ChangeToAttackEnd( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_AttackState == AS_ATTACK_END )
		return;

	m_AttackState = AS_NONE;

	pOwner->SetState( CS_DELAY );
	pOwner->SetChargingState( false );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
}

void ioObjectDoubleShotItem::ChangeToJumpAttackFire( ioBaseChar *pOwner, bool bSend )
{
	if( !pOwner )
		return;

	m_iCurUseCount++;
	m_iCurUseCount = min( m_iCurUseCount, m_iMaxUseCount );
	m_iCurComboCount = 0;

	m_bReserveJumpAttack = false;
	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_JumpAttribute.m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioObjectDoubleShotItem::ChangeToJumpAttackFire - JumpAttack Animation Not Exist" );
		return;
	}

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	iAniID = pGrp->GetAnimationIdx( m_JumpAttribute.m_AttackAnimation );
	float fTimeRate = m_JumpAttribute.m_fAttackAniRate;

	pOwner->RefreshFireTimeList( iAniID, m_JumpAttribute.m_vWeaponInfoList, FTT_JUMP_ATTACK, fTimeRate, 0 );
	pOwner->IncreaseWeaponIndexBase();

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0, 0, true );

	m_dwJumpAttackMotionEndTime = dwCurTime;
	m_dwJumpAttackMotionEndTime += pGrp->GetAnimationFullTime(iAniID) * m_JumpAttribute.m_fAttackAniRate;
	
	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * m_JumpAttribute.m_fAttackAniRate;

	if( m_dwProtectTime > 0 && m_dwAttackStartTime == 0 )
		m_dwAttackStartTime = FRAMEGETTIME();

	SetAirJump( pOwner, iAniID, fTimeRate );

	CheckActiveCount( pOwner );

	CheckTarget( pOwner );
}

void ioObjectDoubleShotItem::ProcessReserveKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner )
	{
		m_bReserveKey = false;
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwMotionEndTime > 0 && m_dwMotionEndTime > dwCurTime )
	{
		if( m_dwEnableReserveTime > 0 && m_dwEnableReserveTime < dwCurTime && pOwner->IsAttackKey() )
			m_bReserveKey = true;
		else
			m_bReserveKey = false;
	}
}

void ioObjectDoubleShotItem::CheckTarget( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	m_TargetName[0].Clear();
	m_TargetName[1].Clear();

	BaseCharDistanceSqList vTargetList;
	vTargetList.clear();

	int iTargetCount = 0;

	float fPrevDistSq = 0.0f;
	if( pOwner->GetState() == CS_JUMP )
		fPrevDistSq = m_fTargetRangeJump * m_fTargetRangeJump;
	else
		fPrevDistSq = m_fTargetRange * m_fTargetRange;

	ioBaseChar *pTargetChar = pOwner;

	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);
		if( !pTarget )	continue;

		if( !pStage->IsCollisionCheckByMode() )
			continue;

		if( pOwner->GetCharName() == pTarget->GetCharName() )
			continue;

		if( pOwner->GetTeam() == pTarget->GetTeam() )
			continue;

		if( pTarget->IsSystemState() )
			continue;
			
		if( !pTarget->IsEnableTarget() )
			continue;


		if( pOwner->GetState() == CS_JUMP && !pTarget->CheckTargetWoundType( m_TargetWoundTypeJump ) )
			continue;

		if( pOwner->GetState() != CS_JUMP && !pTarget->CheckTargetWoundType( m_TargetWoundType ) )
			continue;

		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
		float fDistSq = D3DXVec3LengthSq( &vDiff );
		if( fDistSq < fPrevDistSq )
		{
			D3DXVECTOR3 vDir = vDiff;
			D3DXVec3Normalize( &vDir, &vDir );

			D3DXVECTOR3 vSearchDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vSearchDir, &vSearchDir );

			float fHalfCosine = 0.0f;

			if( pOwner->GetState() == CS_JUMP )
				fHalfCosine = cosf( DEGtoRAD( m_fTargetAngleJump ) * FLOAT05 );
			else
				fHalfCosine = cosf( DEGtoRAD( m_fTargetAngle ) * FLOAT05 );

			if( D3DXVec3Dot( &vSearchDir, &vDir ) >= fHalfCosine )
			{
				BaseCharDistanceSq kInfo;
				kInfo.m_Name = pTarget->GetCharName();
				kInfo.m_fDistanceSq = fDistSq;
				vTargetList.push_back( kInfo );

				iTargetCount++;
			}
		}
	}
	if( !vTargetList.empty() )
		std::sort( vTargetList.begin(), vTargetList.end(), BaseCharDistanceSqSort2() );

	if( !vTargetList.empty() )
	{
		int iSize = vTargetList.size();
		if( iSize > 1 )
		{
			m_TargetName[0] = vTargetList[0].m_Name;
			m_TargetName[1] = vTargetList[1].m_Name;
		}
		else
		{
			m_TargetName[0] = vTargetList[0].m_Name;
			m_TargetName[1] = vTargetList[0].m_Name;
		}
	}
}
