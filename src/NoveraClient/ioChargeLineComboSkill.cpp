

#include "stdafx.h"

#include "ioChargeLineComboSkill.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"


ioChargeLineComboSkill::ioChargeLineComboSkill()
{
	m_SkillState = SS_NONE;
	m_bSetHandMesh = false;
	m_bUsedEndOtherAttack = false;

	m_iCurLinearCombo = 0;
	m_dwNextComboTime = 0;
	m_dwCurSkillProtectTime = 0;

	m_bUsedComboList = false;
	m_bSetAttackFire = false;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_iCurRetreatCnt = 0;
}

ioChargeLineComboSkill::ioChargeLineComboSkill( const ioChargeLineComboSkill &rhs )
	: ioAttackSkill( rhs ),
	m_vComboList( rhs.m_vComboList ),
	m_vJumpComboList( rhs.m_vJumpComboList ),
	m_vDownComboList( rhs.m_vDownComboList ),
	m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	m_bAutoComboAction( rhs.m_bAutoComboAction ),
	m_ProtectCheckType( rhs.m_ProtectCheckType ),
	m_bUseEndOtherAttack( rhs.m_bUseEndOtherAttack ),
	m_EndOtherAttack( rhs.m_EndOtherAttack ),
	m_JumpEndOtherAttack( rhs.m_JumpEndOtherAttack ),
	m_DownEndOtherAttack( rhs.m_DownEndOtherAttack ),
	m_bNoGravityCombo( rhs.m_bNoGravityCombo ),
	m_RetreatAttack( rhs.m_RetreatAttack ),
	m_iMaxRetreatCnt( rhs.m_iMaxRetreatCnt ),

	m_dwMaxChargeTime( rhs.m_dwMaxChargeTime ),
	m_dwMaxGatheringTime( rhs.m_dwMaxGatheringTime ),
	m_stGatheringAni( rhs.m_stGatheringAni ),
	m_fGatheringAniRate( rhs.m_fGatheringAniRate ),
	m_GatherAttack1( rhs.m_GatherAttack1 ),
	m_GatherAttack2( rhs.m_GatherAttack2 ),
	m_GatherAttackList( rhs.m_GatherAttackList ),
	m_dwGatherTic( rhs.m_dwGatherTic )
{
	m_SkillState = SS_NONE;
	m_bSetHandMesh = false;
	m_bUsedEndOtherAttack = false;

	m_iCurLinearCombo = 0;
	m_dwNextComboTime = 0;
	m_dwCurSkillProtectTime = 0;

	m_bUsedComboList = false;
	m_bSetAttackFire = false;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_iCurRetreatCnt = 0;
}

ioChargeLineComboSkill::~ioChargeLineComboSkill()
{
}

void ioChargeLineComboSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_ProtectCheckType = (ProtectCheckType)rkLoader.LoadInt_e( "protect_check_type", PCT_TIME );

	//
	int iComboCnt = rkLoader.LoadInt_e( "linear_combo_cnt", 0 );
	m_vComboList.clear();

	if( iComboCnt > 0 )
	{
		m_vComboList.reserve( iComboCnt );

		for( int i=0; i < iComboCnt; ++i )
		{
			AttackAttribute kComboAttack;

			wsprintf_e( szKey, "linear_combo%d", i+1 );
			LoadAttackAttribute( szKey, kComboAttack, rkLoader );

			m_vComboList.push_back( kComboAttack );
		}
	}

	int iJumpComboCnt = rkLoader.LoadInt_e( "jump_linear_combo_cnt", 0 );
	m_vJumpComboList.clear();

	if( iJumpComboCnt > 0 )
	{
		m_vJumpComboList.reserve( iJumpComboCnt );

		for( int i=0; i < iJumpComboCnt; ++i )
		{
			AttackAttribute kComboAttack;

			wsprintf_e( szKey, "jump_linear_combo%d", i+1 );
			LoadAttackAttribute( szKey, kComboAttack, rkLoader );

			m_vJumpComboList.push_back( kComboAttack );
		}
	}

	int iDownComboCnt = rkLoader.LoadInt_e( "down_linear_combo_cnt", 0 );
	m_vDownComboList.clear();

	if( iDownComboCnt > 0 )
	{
		m_vDownComboList.reserve( iDownComboCnt );

		for( int i=0; i < iDownComboCnt; ++i )
		{
			AttackAttribute kComboAttack;

			wsprintf_e( szKey, "down_linear_combo%d", i+1 );
			LoadAttackAttribute( szKey, kComboAttack, rkLoader );

			m_vDownComboList.push_back( kComboAttack );
		}
	}

	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	m_bAutoComboAction = rkLoader.LoadBool_e( "auto_combo_action", false );

	//
	m_bUseEndOtherAttack = rkLoader.LoadBool_e( "use_end_other_attack", false );

	LoadAttackAttribute_e( "end_other_attack", m_EndOtherAttack, rkLoader );

	LoadAttackAttribute_e( "jump_end_other_attack", m_JumpEndOtherAttack, rkLoader );

	LoadAttackAttribute_e( "down_end_other_attack", m_DownEndOtherAttack, rkLoader );

	//
	m_bNoGravityCombo = rkLoader.LoadBool_e( "no_gravity_combo", false );

	// retreat
	LoadAttackAttribute_e( "retreat_attack", m_RetreatAttack, rkLoader );

	m_iMaxRetreatCnt = rkLoader.LoadInt_e( "retreat_max_cnt", 0 );

	//////////////////////////////////////////////////////////////////////////
	m_dwMaxChargeTime = (DWORD)rkLoader.LoadInt_e( "max_charge_time", 0 );
	m_dwMaxGatheringTime = (DWORD)rkLoader.LoadInt_e( "max_gather_time", 0 );
	m_dwGatherTic = (DWORD)rkLoader.LoadInt_e( "gather_tic", 0 );

	rkLoader.LoadString_e( "gathering_ani", "", szBuf, MAX_PATH );
	m_stGatheringAni = szBuf;
	m_fGatheringAniRate = rkLoader.LoadFloat_e( "gathering_ani_rate ", FLOAT1 );

	int iCnt = rkLoader.LoadInt_e( "gathering_charge_attack_cnt", 0 );
	m_GatherAttackList.clear();

	if( iCnt > 0 )
	{
		m_GatherAttackList.reserve( iCnt );

		for( int i=0; i < iCnt; ++i )
		{
			AttackAttribute kAttack;

			wsprintf_e( szKey, "gathering_charge_attack_combo%d", i+1 );
			LoadAttackAttribute( szKey, kAttack, rkLoader );

			m_GatherAttackList.push_back( kAttack );
		}
	}

}

ioSkill* ioChargeLineComboSkill::Clone()
{
	return new ioChargeLineComboSkill( *this );
}

bool ioChargeLineComboSkill::OnSkillStart( ioBaseChar *pChar )
{
	m_bSetHandMesh = false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_iCurLinearCombo = 0;
	m_dwNextComboTime = 0;
	m_dwEnableReserveTime = 0;
	m_dwFireMotionEndTime = 0;

	m_iCurRetreatCnt = 0;
	m_SkillState = SS_NONE;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	InitExtraAniJump();

	m_bUsedEndOtherAttack = false;
	m_bUsedComboList = false;
	m_bSetAttackFire = false;

	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	if( !m_SkillSoundName.IsEmpty() && pChar )
	{
		ioEntityGroup *pGrp = pChar->GetGroup();
		m_SkillSoundID = g_SoundMgr.PlaySound( m_SkillSoundName, pGrp->GetParentSceneNode() );
	}

	m_dwCurSkillProtectTime = 0;
	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = FRAMEGETTIME() + m_dwSkillProtectTime;

	m_stTargetName.Clear();
	ioChargeComboBowItem *pCCBI = ToChargeComboBowItem( GetOwnerItem() );
	if( pCCBI )
		m_stTargetName = pCCBI->GetTargetName();

	return true;
}

void ioChargeLineComboSkill::AttackSkillFire( ioBaseChar *pChar )
{
	ioAttackSkill::AttackSkillFire( pChar );

	SetSkillActionState( pChar );
	m_bSetAttackFire = true;
	m_SkillState = SS_ACTION;
}

void ioChargeLineComboSkill::SetSkillActionState( ioBaseChar *pChar )
{
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}

void ioChargeLineComboSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;

				AttackSkillFire( pChar );
				EndPreDelayEffect( pChar );
			}
		}
		break;
	case SS_ACTION:
		{
			CheckExtraAniJump( pChar );
			CheckAniRotate( pChar );

			if( CheckEnableRetreatState( pChar ) )
			{
				SetRetreatState( pChar );
				return;
			}
			CheckLinearCombo( pChar );
		}
		break;
	case SS_CHARGE:
		{
			CheckKeyRot(pChar);

			CheckExtraAniJump( pChar );
			CheckAniRotate( pChar );
			if( pChar->IsAttackKeyDown() )
			{
				if( dwCurTime > m_dwChargeStartTime + m_dwMaxChargeTime )
				{
					ChangeGatheringState(pChar);
					return;
				}
			}
			else
			{
				SetNextLinearCombo(pChar, false);
			}
		}
		break;
	case SS_COMBO:
		{
			CheckExtraAniJump( pChar );
			CheckAniRotate( pChar );

			if( CheckEnableRetreatState( pChar ) )
			{
				SetRetreatState( pChar );
				return;
			}
			CheckLinearCombo( pChar );
		}
		break;
	case SS_RETREAT:
		{
			CheckExtraAniJump( pChar );
			CheckAniRotate( pChar );
			CheckLinearCombo( pChar );
		}
		break;
	case SS_GATHERING:
		{
			CheckKeyRot(pChar);

			CheckExtraAniJump( pChar );
			CheckAniRotate( pChar );

			int nTic = (dwCurTime-m_dwGatheringStartTime) / m_dwGatherTic;

			if( pChar->IsNeedProcess() )
			{
				if( pChar->IsAttackKeyDown() )
				{
					if( dwCurTime > (m_dwGatheringStartTime + m_dwMaxGatheringTime) )
					{
						ChangeGatherAttack(pChar, nTic);
						return;
					}
				}
				else
				{
					ChangeGatherAttack(pChar, nTic);
				}
			}
		}
		break;
	case SS_GATHER_ATTACK:
		{
			CheckExtraAniJump( pChar );
			CheckAniRotate( pChar );
		}
		break;
	}
}

void ioChargeLineComboSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	if( GetOwnerItem() )
		GetOwnerItem()->ClearCurChargeRateForSkill();

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}


	if( !m_HandMesh.IsEmpty() )
	{
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	}
	m_bSetHandMesh = false;
	m_bUsedEndOtherAttack = false;
	m_bUsedComboList = false;
	m_bSetAttackFire = false;

	m_iCurLinearCombo = 0;
	m_dwEnableReserveTime = 0;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	pChar->SetSkillMoveEnable( false, false );

	if( !m_SkillSoundName.IsEmpty() )
		g_SoundMgr.StopSound( m_SkillSoundName, m_SkillSoundID );
}

bool ioChargeLineComboSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioChargeLineComboSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	SetChargingRateForWeapon( pWeapon );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	ioTraceWeaponType2 *pTraceWeaponType2 = ToTraceWeaponType2( pWeapon );
	ioChargeComboBowItem *pCCBI = ToChargeComboBowItem( GetOwnerItem() );

	if( pCCBI )
	{
		if( !m_stTargetName.IsEmpty() )
		{
			D3DXVECTOR3 vTargetDir = pCCBI->ReCalcMoveDir( pWeapon->GetOwner(), pWeapon, true );
			if( pTraceWeaponType2 )
			{
				D3DXQUATERNION qtRot;
				ioMath::CalcDirectionQuaternion( qtRot, -vTargetDir );
				pTraceWeaponType2->SetTraceTarget( m_stTargetName );
				pTraceWeaponType2->SetMoveDir( vTargetDir );

				vTargetDir.y = 0;
				ioMath::CalcDirectionQuaternion( qtRot, -vTargetDir );
				pWeapon->GetOwner()->SetTargetRotToRotate( qtRot, true );
			}
		}
	}

	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( false, true );
		}
	}
}


void ioChargeLineComboSkill::ChangeChargeState( ioBaseChar *pChar )
{
	m_SkillState = SS_CHARGE;

	pChar->SetSkillMoveEnable( false, true );
}


void ioChargeLineComboSkill::CheckKeyRot( ioBaseChar *pChar )
{
	if( pChar->IsNeedProcess() && pChar->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();
		if( m_CurDirKey == eNewDirKey )
			return;

		m_CurDirKey = eNewDirKey;
		pChar->SetTargetRotToDirKey( eNewDirKey );

		//send packet
		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SS_GATHERING_ROT;
			kPacket << (int)m_CurDirKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}


void ioChargeLineComboSkill::SetNextLinearCombo( ioBaseChar *pChar, bool bOther )
{
	if( !pChar ) return;

	m_dwEnableReserveTime = 0;
	pChar->ClearSkillReserveKey();

	const AttributeList &rkComboList = GetComboList();

	int iMaxCombo = rkComboList.size();
	if( !COMPARE( m_iCurLinearCombo, 0, iMaxCombo ) )
		return;

	m_SkillState = SS_COMBO;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	if( pChar->IsNeedProcess() && pChar->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();
		m_CurDirKey = eNewDirKey;
		pChar->SetTargetRotToDirKey( eNewDirKey );
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	AttackAttribute kAttack = rkComboList[m_iCurLinearCombo];

	if( bOther )
		kAttack = GetEndAttribute();

	int iAniID = pGrp->GetAnimationIdx( kAttack.m_AttackAnimation );
	if( iAniID == -1 )	return;

	pChar->InitActionStopDelay( true );

	if( m_bNoGravityCombo )
	{
		pChar->SetGravityAmt( 0.0f );
		pChar->SetForcePowerAmt( 0.0f );
		pChar->SetCurJumpPowerAmt( 0.0f );
	}

	float fTimeRate = kAttack.m_fAttackAniRate;
	DWORD dwPreDelay = kAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( IsUseAutoTargetValue() )
	{
		pChar->SetAutoTarget( ATT_SKILL );
	}

	if( m_bRefreshFireTimeList )
	{
		if( m_iCurLinearCombo == 0 )
			m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex+m_iCurLinearCombo,
			iAniID,
			kAttack.m_vWeaponInfoList,
			FTT_SKILL_ATTACK,
			fTimeRate,
			dwPreDelay );
	}

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + kAttack.m_dwEndDelay;

	DWORD dwFireStartTime;
	dwFireStartTime = dwCurTime;

	dwFireStartTime += dwPreDelay;
	dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	float fAttackEnd = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( fAttackEnd > 0.0f )
		m_dwAttackEndTime = dwCurTime + fAttackEnd;
	else
		m_dwAttackEndTime = 0;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	if( kAttack.m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		float fWaitTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
		float fEndTimeRate = kAttack.m_fAttackEndAniRate;

		pGrp->SetActionAni( kAttack.m_AttackEndAnimation,
			0.0f, FLOAT100,
			1.0f, FLOAT1/fEndTimeRate,
			fWaitTime, 0.0f, true );

		int iEndAniID = pGrp->GetAnimationIdx( kAttack.m_AttackEndAnimation );
		m_dwMotionEndTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndTimeRate;
	}

	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	SetExtraAniJump( pChar, iAniID, fTimeRate );

	//
	DWORD dwTrackingTime = dwCurTime + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	if( IsUseAutoTargetValue() )
	{
		pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}

	//
	pChar->SetReservedSliding( kAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_bUsedComboList = true;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << m_SkillState;
		kPacket << m_iCurLinearCombo;
		kPacket << (int)m_CurDirKey;
		kPacket << bOther;

		if( IsUseAutoTargetValue() )
		{
			pChar->FillAutoTargetInfo( kPacket );
		}
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_iCurLinearCombo++;
}


void ioChargeLineComboSkill::ChangeGatheringState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();

	DWORD dwCurTime = FRAMEGETTIME();

	m_SkillState = SS_GATHERING;
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_stGatheringAni );
	if( iAniID == -1 )	
		return;

	pChar->InitActionStopDelay( true );
	
	float fTimeRate = m_fGatheringAniRate;

	pGrp->SetLoopAni( iAniID, FLOAT100 );
	pChar->SetSkillMoveEnable( false, true );

	m_dwGatheringStartTime = dwCurTime;

	//Send Packet
	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << m_SkillState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioChargeLineComboSkill::ChangeGatherAttack( ioBaseChar *pChar, int nTic )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();

	DWORD dwCurTime = FRAMEGETTIME();

	m_SkillState = SS_GATHER_ATTACK;
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( m_GatherAttackList.size() == 0 )
		return;

	if( (int)m_GatherAttackList.size() <= nTic )
		nTic = m_GatherAttackList.size() - 1;

	int iAniID = pGrp->GetAnimationIdx( m_GatherAttackList[nTic].m_AttackAnimation );
	if( iAniID == -1 )	
		return;

	pChar->InitActionStopDelay( true );

	float fTimeRate = m_GatherAttackList[nTic].m_fAttackAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0 );

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex+m_iCurLinearCombo+1,
		iAniID,
		m_GatherAttackList[nTic].m_vWeaponInfoList,
		FTT_SKILL_ATTACK,
		fTimeRate,
		0 );

	//
	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	if( IsUseAutoTargetValue() )
	{
		pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}

	pChar->ClearSkillReserveKey();

	//
	pChar->SetReservedSliding( m_GatherAttackList[nTic].m_vForceInfoList, iAniID, fTimeRate, 0 );

	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	//Send Packet
	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << m_SkillState;
		kPacket << nTic;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioChargeLineComboSkill::CheckLinearCombo( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_bUsedEndOtherAttack )
		return;

	if( m_dwPreDelayStartTime != 0 )
		return;

	const AttributeList &rkComboList = GetComboList();

	if( !COMPARE( m_iCurLinearCombo, 0, (int)rkComboList.size() ) )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_bAutoComboAction )
	{
		if( m_dwEnableReserveTime > 0 && m_dwEnableReserveTime < dwCurTime )
		{
			if( pChar->IsNeedProcess() )
			{
				bool bReserveSkill = false;
				int iSkillNum = pChar->GetSkillKeyInput();
				if( iSkillNum >= 0 )
				{
					bReserveSkill = true;
				}

				if( !bReserveSkill )
				{
					if( m_bUseEndOtherAttack && pChar->IsDefenseKey() )
					{
						pChar->SetSkillMoveEnable( false, true );

						m_bUsedEndOtherAttack = true;
						SetNextLinearCombo( pChar, true );
						return;
					}
				}
			}
		}

		if( m_dwMotionEndTime < dwCurTime )
		{
			pChar->SetSkillMoveEnable( false, true );

			if( pChar->IsNeedProcess() )
			{
				SetNextLinearCombo( pChar, false );
			}
		}

		return;
	}

	if( m_dwEnableReserveTime == 0 )
		return;

	if( m_dwEnableReserveTime < dwCurTime )
	{
		pChar->SetSkillMoveEnable( false, true );

		if( pChar->IsNeedProcess() )
		{
			bool bReserveSkill = false;
			int iSkillNum = pChar->GetSkillKeyInput();
			if( iSkillNum >= 0 )
			{
				bReserveSkill = true;
			}

			if( !bReserveSkill )
			{
				if( pChar->IsAttackKeyDown() )
				{
					m_dwChargeStartTime = dwCurTime;
					ChangeChargeState(pChar);
					return;
				}
				else if( m_bUseEndOtherAttack && pChar->IsDefenseKeyDown() )
				{				
					m_bUsedEndOtherAttack = true;
					m_dwChargeStartTime = dwCurTime;
					ChangeChargeState(pChar);
					return;
				}
			}
		}
	}
}

void ioChargeLineComboSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar )
		return;

	int iState;
	rkPacket >> iState;

	int nTic;

	switch( iState )
	{
	case SS_COMBO:
		{
			rkPacket >> m_iCurLinearCombo;

			int iCurKey;
			rkPacket >> iCurKey;
			pChar->SetTargetRotToDirKey( (ioUserKeyInput::DirKeyInput)iCurKey );

			bool bOther;
			rkPacket >> bOther;

			SetNextLinearCombo( pChar, bOther );

			int iDummyCnt;
			rkPacket >> iDummyCnt;

			if( IsUseAutoTargetValue() )
			{
				pChar->ApplyAutoTargetInfo( rkPacket );
			}
		}
		break;
	case SS_RETREAT:
		{
			rkPacket >> m_iCurLinearCombo;
			rkPacket >> m_iCurRetreatCnt;

			int iCurKey;
			rkPacket >> iCurKey;
			pChar->SetTargetRotToDirKey( (ioUserKeyInput::DirKeyInput)iCurKey );

			SetRetreatState( pChar );
		}
		break;
	case SS_GATHERING:
			ChangeGatheringState(pChar);
		break;
	case SS_GATHERING_ROT:
		{	
			int nCurKey;
			rkPacket >>	nCurKey;
			m_CurDirKey = (ioUserKeyInput::DirKeyInput)nCurKey;
			pChar->SetTargetRotToDirKey( m_CurDirKey );
		}
		break;
	case SS_GATHER_ATTACK:
		{
			rkPacket >> nTic;
			ChangeGatherAttack(pChar, nTic );
		}
		break;
	}
}

bool ioChargeLineComboSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	if( m_dwMotionEndTime + dwActionStop >= FRAMEGETTIME() )
		return false;

	if( m_SkillState == SS_CHARGE || m_SkillState == SS_GATHERING )
		return false;

	return true;
}

bool ioChargeLineComboSkill::IsEnableReserve() const
{
	if( m_SkillState == SS_CHARGE || m_SkillState == SS_GATHERING )
		return true;

	if( m_dwPreDelayStartTime != 0 )
		return false;

	if( m_dwEnableReserveTime == 0 || m_dwMotionStartTime >= m_dwEnableReserveTime )
		return false;

	if( m_dwEnableReserveTime < FRAMEGETTIME() )
		return true;

	return false;
}

bool ioChargeLineComboSkill::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{
		if( iDefenseBreakType == m_ProtectCancelType )   // 스킬 무적 파괴
			return false;
	}

	DWORD dwCurTime = FRAMEGETTIME();

	if( IsPreDelayProtect() )
	{
		if( m_dwPreDelayStartTime != 0 )
			return true;
	}

	if( m_dwPreDelayStartTime == 0 )
	{
		switch( m_ProtectCheckType )
		{
		case PCT_TIME:
			if( m_dwCurSkillProtectTime > dwCurTime )
				return true;
			break;
		case PCT_ANI:
			if( m_iCurLinearCombo == 0 &&
				m_dwFireMotionEndTime > 0 &&
				m_dwFireMotionEndTime > dwCurTime )
			{
				return true;
			}
			break;
		}
	}

	return false;
}

const AttributeList& ioChargeLineComboSkill::GetComboList()
{
	switch( m_SkillUseType )
	{
	case SUT_NORMAL:
		return m_vComboList;
	case SUT_DOWN:
		if( !m_vDownComboList.empty() )
			return m_vDownComboList;
		return m_vComboList;
	case SUT_JUMP:
		if( !m_vJumpComboList.empty() )
			return m_vJumpComboList;
		return m_vComboList;
	}

	return m_vComboList;
}

AttackAttribute ioChargeLineComboSkill::GetEndAttribute()
{
	switch( m_SkillUseType )
	{
	case SUT_NORMAL:
		return m_EndOtherAttack;
	case SUT_DOWN:
		if( !m_DownEndOtherAttack.m_AttackAnimation.IsEmpty() )
			return m_DownEndOtherAttack;
		return m_EndOtherAttack;
	case SUT_JUMP:
		if( !m_JumpEndOtherAttack.m_AttackAnimation.IsEmpty() )
			return m_JumpEndOtherAttack;
		return m_EndOtherAttack;
	}

	return m_EndOtherAttack;
}

bool ioChargeLineComboSkill::IsJumpState() const
{
	if( !m_bNoGravityCombo )
		return false;

	return m_bUsedComboList;
}

bool ioChargeLineComboSkill::IsNoDropState() const
{
	if( !m_bNoGravityCombo )
		return false;

	return m_bUsedComboList;
}

bool ioChargeLineComboSkill::IsCanSendControl() const
{
	if( m_bSetAttackFire )
		return false;

	return true;
}

bool ioChargeLineComboSkill::CheckEnableRetreatState( ioBaseChar *pChar )
{
	if( !pChar )
		return false;

	if( m_iMaxRetreatCnt <= 0 )
		return false;

	if( !pChar->IsDirKeyDoubleClick() )
		return false;

	if( m_iCurLinearCombo >= m_iMaxRetreatCnt )
		return false;

	if( m_iCurRetreatCnt < m_iMaxRetreatCnt )
		return true;

	return false;
}

void ioChargeLineComboSkill::SetRetreatState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	m_SkillState = SS_RETREAT;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	if( pChar->IsNeedProcess() && pChar->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();
		m_CurDirKey = eNewDirKey;
		pChar->SetTargetRotToDirKey( eNewDirKey );
	}

	int iAniID = pGrp->GetAnimationIdx( m_RetreatAttack.m_AttackAnimation );
	if( iAniID == -1 )	return;

	pChar->InitActionStopDelay( true );

	float fTimeRate = m_RetreatAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_RetreatAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
		iAniID,
		m_RetreatAttack.m_vWeaponInfoList,
		FTT_SKILL_ATTACK,
		fTimeRate,
		dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + m_RetreatAttack.m_dwEndDelay;

	DWORD dwFireStartTime;
	dwFireStartTime = dwCurTime;

	dwFireStartTime += dwPreDelay;
	dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	float fAttackEnd = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( fAttackEnd > 0.0f )
		m_dwAttackEndTime = dwCurTime + fAttackEnd;
	else
		m_dwAttackEndTime = 0;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	if( m_RetreatAttack.m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		float fWaitTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
		float fEndTimeRate = m_RetreatAttack.m_fAttackEndAniRate;

		pGrp->SetActionAni( m_RetreatAttack.m_AttackEndAnimation,
			0.0f, FLOAT100,
			1.0f, FLOAT1/fEndTimeRate,
			fWaitTime, 0.0f, true );

		int iEndAniID = pGrp->GetAnimationIdx( m_RetreatAttack.m_AttackEndAnimation );
		m_dwMotionEndTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndTimeRate;
	}

	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );
	SetExtraAniJump( pChar, iAniID, fTimeRate );
	//
	pChar->SetReservedSliding( m_RetreatAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << m_SkillState;
		kPacket << m_iCurLinearCombo;
		kPacket << m_iCurRetreatCnt;
		kPacket << (int)m_CurDirKey;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	pChar->ClearDirDoubleClick();
	m_iCurRetreatCnt++;
}

bool ioChargeLineComboSkill::IsEnableDashCheck( ioBaseChar *pChar )
{
	switch( m_SkillState )
	{
	case SS_ACTION:
	case SS_COMBO:
		return true;
	}

	return false;
}

