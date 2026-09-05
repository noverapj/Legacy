#include "StdAfx.h"
#include "ioJumpAttackBuff.h"

#include "ioBaseChar.h"


ioJumpAttackBuff::ioJumpAttackBuff(void)
{
}

ioJumpAttackBuff::ioJumpAttackBuff( const ioJumpAttackBuff &rhs ):
ioBuff( rhs ),
m_fNeedGauge( rhs.m_fNeedGauge ),
m_ExtraAttack( rhs.m_ExtraAttack ),
m_fEndJumpPower( rhs.m_fEndJumpPower ),
m_fAimAngle( rhs.m_fAimAngle ),
m_fMaxRange( rhs.m_fMaxRange ),
m_fMinRange( rhs.m_fMinRange ),
m_bEnableDownAim( rhs.m_bEnableDownAim )
{

}

ioJumpAttackBuff::~ioJumpAttackBuff(void)
{
}

ioBuff* ioJumpAttackBuff::Clone()
{
	return new ioJumpAttackBuff( *this );
}

void ioJumpAttackBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", 0.0f );
	LoadAttackAttribute_e( "extra_attack", m_ExtraAttack, rkLoader );

	m_fEndJumpPower = rkLoader.LoadFloat_e( "end_jump_power", 0.0f );

	m_fAimAngle = rkLoader.LoadFloat_e( "aim_angle", 0.0f );
	m_fMaxRange = rkLoader.LoadFloat_e( "aim_max_range", 0.0f );
	m_fMinRange = rkLoader.LoadFloat_e( "aim_min_range", 0.0f );
	m_bEnableDownAim = rkLoader.LoadBool_e( "enable_down_aim", false );

}

void ioJumpAttackBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	m_dwMotionEndTime = 0;
	m_dwFireStartTime = 0;
}

bool ioJumpAttackBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	m_dwMotionEndTime = 0;
	m_dwFireStartTime = 0;
	return true;
}

void ioJumpAttackBuff::ProcessBuff( float fTimePerSec )
{
	if ( !GetOwner() || !m_pOwner )
	{
		SetReserveEndBuff();
		return;
	}

	ioPlayStage *pStage = m_pOwner->GetCreator();
	if ( !pStage )
	{
		SetReserveEndBuff();
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_iOperationType )
	{
	case OT_SWITCH:
		{
			float fCurTicGauge = GetCurTicGauge();
			int iTickGap = GetCurTicGap();
			int iCurTicTime = GetCurTicTime();
			if( iTickGap >= iCurTicTime )
			{
				m_dwCheckTime = dwCurTime - ( iTickGap - iCurTicTime );

				ioObjectItem *pObject = m_pOwner->GetObject();
				if( pObject && !pObject->EnableReduceGaugeOnSwitchBuff() && pObject->GetItemCode() == m_dwCreateWeaponItemCode )
					break;

				if( fCurTicGauge != 0.0f )
				{
					if( !IsRightOwnerItem() )
					{
						SetReserveEndBuff();
						return;
					}

					if( m_pOwner->ReduceCurSkillGaugeBySkill( m_ItemName, fCurTicGauge ) <= 0.0f )
					{
						if( m_pOwner->GetState() != CS_BUFF_ACTION )
						{
							SetReserveEndBuff();
							return;
						}
					}
				}
			}

			CheckCallingProcess();
		}
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}

	if ( m_pOwner->IsNeedProcess() && CheckEnableJumpAttackState( m_pOwner ) )
	{
		SetActionState( m_pOwner, pStage );
		return;
	}

	if ( m_dwFireStartTime && m_dwFireStartTime < dwCurTime )
	{
		m_dwFireStartTime = 0;
		CreateWeapon( m_pOwner );
	}

	if ( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
	{
		SetActionEndState( m_pOwner );
		return;
	}
}

bool ioJumpAttackBuff::CheckEnableJumpAttackState( ioBaseChar *pOwner )
{
	/*if ( !pOwner || !m_pOwnerItem )
		return false;*/
	if ( !pOwner || m_ItemName.IsEmpty() )
		return false;

	if ( !pOwner->IsAttackKey() )
		return false;

	if ( pOwner->GetState() != CS_JUMP )
		return false;

	if ( pOwner->GetUsedBuffJump() != BJUT_CUR )
		return false;

	ioItem *pOwnerItem = pOwner->GetEquipedItem( m_ItemName );
	if( !pOwnerItem )
		return false;

	//int iSlot = m_pOwnerItem->GetType() - 1;
	int iSlot = pOwnerItem->GetType() - 1;
	ioSkill *pSkill = pOwner->GetEquipedSkill( iSlot );
	ioPassiveSkill *pPassiveSkill = ToPassiveSkill(pSkill);

	if ( !pPassiveSkill )
		return false;

	if( GetSkillName().IsEmpty() ||  pPassiveSkill->GetName() != GetSkillName() )
		return false;

	if ( !pPassiveSkill->UsePassiveGauge() || pPassiveSkill->GetCurPassiveGauge() <= m_fNeedGauge )
		return false;

	return true;
}

void ioJumpAttackBuff::SetActionState( ioBaseChar* pChar, ioPlayStage *pStage )
{
	//if ( !pChar || !pStage || !m_pOwnerItem )
	if ( !pChar || !pStage || m_ItemName.IsEmpty() )
		return;

	ioItem *pOwnerItem = pChar->GetEquipedItem( m_ItemName );
	if( !pOwnerItem )
		return;

	//int iSlot = m_pOwnerItem->GetType() - 1;
	int iSlot = pOwnerItem->GetType() - 1;
	ioPassiveSkill *pPassiveSkill = ToPassiveSkill( pChar->GetEquipedSkill( iSlot ) );

	if ( !pPassiveSkill )
		return;

	pPassiveSkill->SetCurPassiveGauge( 0.0f );

	if ( pChar->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = ioUserKeyInput::DKI_NONE;
		if ( pChar->IsSettedDirection() )
		{
			eNewDirKey = pChar->CheckCurDirKey();
			pChar->SetTargetRotToDirKey( eNewDirKey );
			pChar->SetWorldOrientation( pChar->GetTargetRot() );
		}
	}

	pChar->SetState( CS_BUFF_ACTION );

	SetActionAni( pChar, m_ExtraAttack );

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << static_cast<BYTE>(BS_ATTACK);
		kPacket << pChar->GetWorldPosition();
		kPacket << pChar->GetWorldOrientation();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioJumpAttackBuff::SetActionAni( ioBaseChar* pChar, const AttackAttribute& Attri )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( Attri.m_AttackAnimation );
	if( iAniID == -1 )	
		return;

	float fTimeRate  = Attri.m_fAttackAniRate;
	DWORD dwPreDelay = Attri.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = m_dwFireStartTime = dwCurTime;

	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + Attri.m_dwEndDelay;

	m_dwFireStartTime += dwPreDelay + pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	// Col Skip
	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	pChar->SetReservedSliding( Attri.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
}

void ioJumpAttackBuff::SetActionEndState( ioBaseChar *pChar )
{
	m_dwMotionEndTime = 0;
	if ( pChar->IsNeedProcess() )
	{
		if ( pChar->IsFloating() )
			pChar->SetSKillEndJumpState( m_fEndJumpPower, false );
		else
			pChar->SetState( CS_DELAY, false );
	}
	
}

void ioJumpAttackBuff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	BYTE btState;
	rkPacket >> btState;

	if ( btState == BS_ATTACK )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;

		pOwner->SetWorldPosition( vPos );
		pOwner->SetTargetRotAndMoveDirChange( qtRot );

		SetActionState( pOwner, pStage );
	}
}

void ioJumpAttackBuff::CreateWeapon( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	D3DXVECTOR3 vCurPos = pChar->GetMidPositionByRate();
	D3DXVECTOR3 vCurDir = pChar->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	DWORD dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	ioHashString szTarget;
	pChar->CheckAimedTargetByBow( m_fAimAngle, m_fMinRange, m_fMaxRange, szTarget, m_bEnableDownAim );

	if ( !szTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = pChar->GetBaseChar( szTarget );
		if ( pTarget )
		{
			vCurDir = pTarget->GetWorldPosition() - pChar->GetWorldPosition();
			D3DXVec3Normalize( &vCurDir, &vCurDir );
		}
	}

	WeaponInfoList WeaponList = m_ExtraAttack.m_vWeaponInfoList;
	for ( int i=0; i<(int)WeaponList.size() ; ++i )
	{
		FireTime kFireTime;
		kFireTime.dwStart = FRAMEGETTIME();
		kFireTime.iAttributeIdx = WeaponList[i].m_iWeaponIdx;
		kFireTime.iResistanceIdx = WeaponList[i].m_iResistanceIdx;
		kFireTime.szWoundedAni  = WeaponList[i].m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration = WeaponList[i].m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni = WeaponList[i].m_bWoundedLoopAni;
		kFireTime.dwWeaponIdx = dwWeaponBaseIndex++;

		pChar->ExtendFireExplicit( kFireTime, vCurPos, vCurDir, "" );
	}
}
