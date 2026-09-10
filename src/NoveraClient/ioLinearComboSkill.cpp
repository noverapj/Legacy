

#include "stdafx.h"

#include "ioLinearComboSkill.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioLinearComboSkill::ioLinearComboSkill()
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

	m_dwAreMapEffectID = -1;

	m_nCurEndCombo = 0;
	m_dwAreaWeaponIndex = 0;
}

ioLinearComboSkill::ioLinearComboSkill( const ioLinearComboSkill &rhs )
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
 m_vDestroyDummyCharList( rhs.m_vDestroyDummyCharList ),
 m_DummyCharList( rhs.m_DummyCharList ),
 m_DummyCharEffect( rhs.m_DummyCharEffect ),
 m_RetreatAttack( rhs.m_RetreatAttack ),
 m_iMaxRetreatCnt( rhs.m_iMaxRetreatCnt ),
 m_bUseChangeDirection( rhs.m_bUseChangeDirection ),
 m_bCheckJumpNoDrop( rhs.m_bCheckJumpNoDrop ),
 m_bUseEndJumpPower(rhs.m_bUseEndJumpPower),
 m_stAreaMapEffect( rhs.m_stAreaMapEffect ),
 m_fAreaMapEffectLookOffset( rhs.m_fAreaMapEffectLookOffset ),
 m_bUseEndCombo( rhs.m_bUseEndCombo ),
 m_nMaxEndCombo( rhs.m_nMaxEndCombo ),
 m_bUseAreaWeapon( rhs.m_bUseAreaWeapon ),
 m_stAreaWeaponName( rhs.m_stAreaWeaponName )
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

	m_dwAreMapEffectID = -1;

	m_nCurEndCombo = 0;
	m_dwAreaWeaponIndex = 0;
}

ioLinearComboSkill::~ioLinearComboSkill()
{
	m_vDestroyDummyCharList.clear();
	m_DummyCharList.clear();
}

void ioLinearComboSkill::LoadProperty( ioINILoader &rkLoader )
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

	//
	rkLoader.LoadString_e( "dummy_char_effect", "", szBuf, MAX_PATH );
	m_DummyCharEffect = szBuf;

	m_DummyCharList.clear();
	m_vDestroyDummyCharList.clear();

	Help::LoadDummyCharCreateDestroyInfo( rkLoader, m_DummyCharList, m_vDestroyDummyCharList );

	// retreat
	LoadAttackAttribute_e( "retreat_attack", m_RetreatAttack, rkLoader );

	m_iMaxRetreatCnt = rkLoader.LoadInt_e( "retreat_max_cnt", 0 );

	m_bUseChangeDirection = rkLoader.LoadBool_e( "use_change_direction", true );
	m_bCheckJumpNoDrop = rkLoader.LoadBool_e( "check_jump_no_drop", false );
	m_bUseEndJumpPower = rkLoader.LoadBool_e( "use_end_jump_power", false );

	//
	rkLoader.LoadString_e( "area_map_effect", "", szBuf, MAX_PATH );
	m_stAreaMapEffect = szBuf;
	m_fAreaMapEffectLookOffset = rkLoader.LoadFloat_e( "area_map_effect_look_offset", 0.f );

	m_bUseEndCombo = rkLoader.LoadBool_e( "use_end_combo", false );
	m_nMaxEndCombo = rkLoader.LoadInt_e( "max_end_combo", 0 );

	//////////////////////////////////////////////////////////////////////////
	m_bUseAreaWeapon = rkLoader.LoadBool_e( "use_area_weapon", false );
	rkLoader.LoadString_e( "area_weapon_name", "", szBuf, MAX_PATH );
	m_stAreaWeaponName = szBuf;

}

ioSkill* ioLinearComboSkill::Clone()
{
	return new ioLinearComboSkill( *this );
}

bool ioLinearComboSkill::OnSkillStart( ioBaseChar *pChar )
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
	m_dwAreaWeaponIndex = 0;

	m_iCurRetreatCnt = 0;
	m_SkillState = SS_NONE;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_nCurEndCombo = 0;

	InitExtraAniJump();

	m_bUsedEndOtherAttack = false;
	m_bUsedComboList = false;
	m_bSetAttackFire = false;

	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	SetCameraBuff( pChar );

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

	if( m_SkillUseType == SUT_JUMP && m_bCheckJumpNoDrop )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
		pChar->SetForcePowerAmt( 0.0f );
	}

	if( !m_stAreaMapEffect.IsEmpty() )
	{
		D3DXVECTOR3 vScale = pChar->GetWorldScale();
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
		D3DXVECTOR3 vLook = pChar->GetTargetRot() * D3DXVECTOR3( 0.f, 0.f, -FLOAT1 );
		D3DXVec3Normalize( &vLook, &vLook );

		vPos = vPos + vLook* m_fAreaMapEffectLookOffset;

		ioMapEffect *pMapEffect = pChar->CreateMapEffectBySkill( m_stAreaMapEffect, vPos, vScale );
		if( pMapEffect )
		{
			//pMapEffect->SetWorldOrientation( pChar->GetWorldOrientation() );
			m_dwAreMapEffectID = pMapEffect->GetUniqueID();
		}
	}

	return true;
}

void ioLinearComboSkill::AttackSkillFire( ioBaseChar *pChar )
{
	ioAttackSkill::AttackSkillFire( pChar );

	SetSkillActionState( pChar );
	m_bSetAttackFire = true;
	m_SkillState = SS_ACTION;
}

void ioLinearComboSkill::SetSkillActionState( ioBaseChar *pChar )
{
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}

void ioLinearComboSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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

				if( m_bUseAreaWeapon )
					CreateAreaWeapon( pChar );
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
	}
}

void ioLinearComboSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );
	
	EndCameraBuff( pChar );

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
	{
		g_SoundMgr.StopSound( m_SkillSoundName, m_SkillSoundID );
		//g_SoundMgr.StopSound( m_SkillSoundName, 0 );
	}

	if( m_dwAreMapEffectID != -1 )
	{
		pChar->DestroyMapEffect( m_dwAreMapEffectID );
		m_dwAreMapEffectID = -1;
	}

	DestroyAreaWeapon( pChar );
}

bool ioLinearComboSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioLinearComboSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
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

void ioLinearComboSkill::SetNextLinearCombo( ioBaseChar *pChar, bool bOther )
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

	if( pChar->IsNeedProcess() && pChar->IsSettedDirection() && m_bUseChangeDirection )
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

	//
	if( !m_bUsedComboList )
		g_DummyCharMgr.DestroyDummyCharByNameList( pChar, m_vDestroyDummyCharList );

	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	vDummyCharCreateInfoList vCreateList;
	vCreateList.clear();

	if( pStage && pChar->IsNeedProcess() && !m_bUsedComboList && !m_DummyCharList.empty() )
	{
		D3DXVECTOR3 vCreatePos = pChar->GetWorldPosition();

		if( !m_DummyCharEffect.IsEmpty() )
			pChar->CreateMapEffectBySkill( m_DummyCharEffect, vCreatePos );

		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );

		D3DXVECTOR3 vSideDir;
		D3DXVec3Cross( &vSideDir, &ioMath::UNIT_Y, &vDir );
		D3DXVec3Normalize( &vSideDir, &vSideDir );

		int iSize = m_DummyCharList.size();
		for( int i=0; i < iSize; ++i )
		{
			ioHashString szDummyCharName = m_DummyCharList[i].m_DummyCharName;
			int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
			float fOffset = m_DummyCharList[i].m_fDummyCharOffset;
			float fSideOffset = m_DummyCharList[i].m_fDummyCharSideOffset;
			float fStartAngle = m_DummyCharList[i].m_fDummyCharStartAngle;

			D3DXVECTOR3 vTargetPos = vCreatePos + (fOffset * vDir);
			vTargetPos += fSideOffset * vSideDir;

			ioDummyChar *pDummy = pStage->CreateDummyChar( szDummyCharName, iCurIndex,
														   pChar->GetCharName(),
														   vTargetPos,
														   fStartAngle,
														   0, true );

			if( pDummy )
			{
				pDummy->SetControlSkillName( GetName() );

				DummyCharCreateInfo kInfo;
				kInfo.m_DummyCharName = szDummyCharName;
				kInfo.m_iDummyCharIdx = iCurIndex;
				kInfo.m_fDummyCharStartAngle = fStartAngle;
				kInfo.m_vTargetPos = vTargetPos;

				vCreateList.push_back( kInfo );
			}
		}
	}

	m_bUsedComboList = true;

	int iDummyCnt = vCreateList.size();

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << m_SkillState;
		kPacket << m_iCurLinearCombo;
		kPacket << (int)m_CurDirKey;
		kPacket << bOther;

		kPacket << iDummyCnt;
		for( int i=0; i < iDummyCnt; ++i )
		{
			kPacket << vCreateList[i].m_DummyCharName;
			kPacket << vCreateList[i].m_iDummyCharIdx;
			kPacket << vCreateList[i].m_fDummyCharStartAngle;
			kPacket << vCreateList[i].m_vTargetPos;
		}

		if( IsUseAutoTargetValue() )
		{
			pChar->FillAutoTargetInfo( kPacket );
		}
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
		
	if ( !m_bUseEndCombo )
	{
		m_iCurLinearCombo++;
	}
	else
	{
		//엔드콤보사용시
		if ( bOther )
			m_nCurEndCombo++;
		else
			m_iCurLinearCombo++;
	}
}

void ioLinearComboSkill::CheckLinearCombo( ioBaseChar *pChar )
{
	if( !pChar )
		return;
	
	if( m_bUsedEndOtherAttack && !m_bUseEndCombo )
		return;

	if( m_dwPreDelayStartTime != 0 )
		return;

	const AttributeList &rkComboList = GetComboList();

	if( !COMPARE( m_iCurLinearCombo, 0, (int)rkComboList.size() ) || 
		m_bUseEndCombo && !COMPARE( m_nCurEndCombo, 0, m_nMaxEndCombo ) )
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
				if( pChar->IsAttackKey() )
				{
					SetNextLinearCombo( pChar, false );
					return;
				}
				else if( m_bUseEndOtherAttack && pChar->IsDefenseKey() )
				{				
					m_bUsedEndOtherAttack = true;
					SetNextLinearCombo( pChar, true );
					return;
				}
			}
		}
	}
}

void ioLinearComboSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar )
		return;

	int iState;
	rkPacket >> iState;

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

			for( int i=0; i < iDummyCnt; ++i )
			{
				int iCurIndex;
				ioHashString szDummyCharName;
				float fStartAngle;
				D3DXVECTOR3 vTargetPos;

				rkPacket >> szDummyCharName >> iCurIndex;
				rkPacket >> fStartAngle;
				rkPacket >> vTargetPos;

				ioDummyChar *pDummy = pStage->CreateDummyChar( szDummyCharName, iCurIndex,
															   pChar->GetCharName(),
															   vTargetPos,
															   fStartAngle,
															   0, false );

				if( pDummy )
					pDummy->SetControlSkillName( GetName() );
			}

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
	}
}

bool ioLinearComboSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	if( m_dwMotionEndTime + dwActionStop >= FRAMEGETTIME() )
		return false;

	if( m_bCheckJumpNoDrop )
		return false;

	return true;
}

bool ioLinearComboSkill::IsEnableReserve() const
{
	if( m_dwPreDelayStartTime != 0 )
		return false;

	if( m_dwEnableReserveTime == 0 || m_dwMotionStartTime >= m_dwEnableReserveTime )
		return false;

	if( m_dwEnableReserveTime < FRAMEGETTIME() )
		return true;

	return false;
}

bool ioLinearComboSkill::IsProtected( int iDefenseBreakType ) const
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

const AttributeList& ioLinearComboSkill::GetComboList()
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

AttackAttribute ioLinearComboSkill::GetEndAttribute()
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

bool ioLinearComboSkill::IsJumpState() const
{
	if( !m_bNoGravityCombo )
		return false;

	return m_bUsedComboList;
}

bool ioLinearComboSkill::IsNoDropState() const
{
	if( !m_bNoGravityCombo )
		return false;

	if( m_bCheckJumpNoDrop && m_SkillUseType == SUT_JUMP )
		return true;

	return m_bUsedComboList;
}

bool ioLinearComboSkill::IsCanSendControl() const
{
	if( m_bSetAttackFire )
		return false;

	return true;
}

bool ioLinearComboSkill::CheckEnableRetreatState( ioBaseChar *pChar )
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

void ioLinearComboSkill::SetRetreatState( ioBaseChar *pChar )
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

bool ioLinearComboSkill::IsEnableDashCheck( ioBaseChar *pChar )
{
	switch( m_SkillState )
	{
	case SS_ACTION:
	case SS_COMBO:
		return true;
	}

	return false;
}

bool ioLinearComboSkill::IsAttackEndState() const
{
	if( m_bCheckJumpNoDrop )
	{
		if( m_dwPreDelayStartTime > 0 )
			return false;

		if( m_dwMotionEndTime >= FRAMEGETTIME() )
			return false;

		return true;
	}
	else 
		return __super::IsAttackEndState();
}


void ioLinearComboSkill::CreateAreaWeapon( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( m_stAreaWeaponName.IsEmpty() ) return;

	if( m_dwAreaWeaponIndex != 0 )
	{
		if( g_AreaWeaponMgr.FindAttachedAreaWeapon( m_dwAreaWeaponIndex ) )
		{
			LOG.PrintTimeAndLog( 0, "ioLinearComboSkill::CreateAreaWeapon - Create Fail! Already Exist: %d",
				m_dwAreaWeaponIndex );

			return;
		}
	}

	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
	D3DXQUATERNION qtRot = D3DXQUATERNION( 0.0f, 0.0f, 0.0f, FLOAT1 );
	ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.CreateAreaWeapon( pChar->GetCharName(),
		pChar->GetCharName(),
		GetName(),
		m_stAreaWeaponName,
		vPos,
		qtRot,
		ioAreaWeapon::CT_ATTACHED );


	if( pAreaWeapon )
	{
		m_dwAreaWeaponIndex = pAreaWeapon->GetAreaWeaponIndex();
	}
}

void ioLinearComboSkill::DestroyAreaWeapon( ioBaseChar *pOwner )
{
	if( m_dwAreaWeaponIndex )
		g_AreaWeaponMgr.DestroyAreaWeapon( m_dwAreaWeaponIndex );

	m_dwAreaWeaponIndex = 0;
}