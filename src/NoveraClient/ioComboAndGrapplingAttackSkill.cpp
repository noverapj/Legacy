#include "StdAfx.h"
#include "ioComboAndGrapplingAttackSkill.h"

#include "ioBaseChar.h"
#include "ioNpcChar.h"
#include "ioPlayStage.h"
#include "ioZoneEffectWeapon.h"
#include "ioNoneTraceDummy2.h"


ioComboAndGrapplingAttackSkill::ioComboAndGrapplingAttackSkill(void)
{
}

ioComboAndGrapplingAttackSkill::ioComboAndGrapplingAttackSkill( const ioComboAndGrapplingAttackSkill &rhs ):
ioAttackSkill( rhs ),
m_fTargetRange( rhs.m_fTargetRange ),
m_fTargetAngle( rhs.m_fTargetAngle ),
m_fTargetUpHeight( rhs.m_fTargetUpHeight ),
m_vNoneTargetOffset( rhs.m_vNoneTargetOffset ),
m_vTeloportTargetOffset( rhs.m_vTeloportTargetOffset ),
m_szCheckBuff( rhs.m_szCheckBuff ),
m_ComboList( rhs.m_ComboList ),
m_fCharJumpPower( rhs.m_fCharJumpPower ),
m_fCharHeightRate( rhs.m_fCharHeightRate ),
m_GrapplingLoopMotion( rhs.m_GrapplingLoopMotion ),
m_fGrapplingLoopRate( rhs.m_fGrapplingLoopRate ),
m_dwGrapplingLoopRotateTime( rhs.m_dwGrapplingLoopRotateTime ),
m_LandAttackAttribute( rhs.m_LandAttackAttribute ),
m_fTargetGapRange( rhs.m_fTargetGapRange ),
m_szDummyCharName( rhs.m_szDummyCharName ),
m_fDummyHRotAngle( rhs.m_fDummyHRotAngle ),
m_vDummyStartOffset( rhs.m_vDummyStartOffset ),
m_vDummyComboAttackCodeList( rhs.m_vDummyComboAttackCodeList ),
m_iDummyGrapplingAttackCode( rhs.m_iDummyGrapplingAttackCode ),
m_iDummyLandingAttackCode( rhs.m_iDummyLandingAttackCode ),
m_vDummyJumppingOffset( rhs.m_vDummyJumppingOffset )
{

}

ioComboAndGrapplingAttackSkill::~ioComboAndGrapplingAttackSkill(void)
{
}

ioSkill* ioComboAndGrapplingAttackSkill::Clone()
{
	return new ioComboAndGrapplingAttackSkill( *this );
}

void ioComboAndGrapplingAttackSkill::ClearData()
{
	m_SkillState = SS_NONE;
	m_GrapplingTargetName.Clear();
	m_iCurLinearCombo = 0;
	m_szAttackTarget.Clear();
	m_iDummyIndex = -1;
}

void ioComboAndGrapplingAttackSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );
	
	char szBuf[MAX_PATH], szKey[MAX_PATH];

	m_fTargetAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fTargetRange = rkLoader.LoadFloat_e( "target_max_range", 0.0f );
	m_fTargetUpHeight = rkLoader.LoadFloat_e( "target_up_height", 0.0f );

	m_vNoneTargetOffset.x = rkLoader.LoadFloat_e( "none_target_offset_x", 0.0f );
	m_vNoneTargetOffset.y = rkLoader.LoadFloat_e( "none_target_offset_y", 0.0f );
	m_vNoneTargetOffset.z = rkLoader.LoadFloat_e( "none_target_offset_z", 0.0f );

	m_vTeloportTargetOffset.x = rkLoader.LoadFloat_e( "target_teloport_offset_x", 0.0f );
	m_vTeloportTargetOffset.y = rkLoader.LoadFloat_e( "target_teloport_offset_y", 0.0f );
	m_vTeloportTargetOffset.z = rkLoader.LoadFloat_e( "target_teloport_offset_z", 0.0f );


	//linear combo
	int iComboCnt = rkLoader.LoadInt_e( "linear_combo_cnt", 0 );
	m_ComboList.clear();

	if( iComboCnt > 0 )
	{
		m_ComboList.reserve( iComboCnt );

		for( int i=0; i < iComboCnt; ++i )
		{
			AttackAttribute kComboAttack;

			wsprintf_e( szKey, "linear_combo%d", i+1 );
			LoadAttackAttribute( szKey, kComboAttack, rkLoader );

			m_ComboList.push_back( kComboAttack );
		}
	}

	rkLoader.LoadString_e( "check_buff", "", szBuf, MAX_PATH );
	m_szCheckBuff = szBuf;

	m_fCharJumpPower = rkLoader.LoadFloat_e( "skill_jump_power", 0.0f );
	m_fCharHeightRate = rkLoader.LoadFloat_e( "char_height_rate", FLOAT05 );

	rkLoader.LoadString_e( "grappling_loop_motion", "", szBuf, MAX_PATH );
	m_GrapplingLoopMotion = szBuf;

	m_fGrapplingLoopRate = rkLoader.LoadFloat_e( "grappling_loop_motion_rate", FLOAT1 );
	m_fTargetGapRange = rkLoader.LoadFloat_e( "target_gap_range", 0.0f );
	m_dwGrapplingLoopRotateTime = rkLoader.LoadInt_e( "grappling_loop_rotate_speed", 0 );

	m_LandAttackAttribute.Init();
	LoadAttackAttribute_e( "land_attack", m_LandAttackAttribute, rkLoader );

	rkLoader.LoadString_e( "dummy_char_name", "", szBuf, MAX_PATH );
	m_szDummyCharName = szBuf;
	m_vDummyStartOffset.x = rkLoader.LoadFloat_e( "dummy_char_start_offset_x", 0.0f );
	m_vDummyStartOffset.y = rkLoader.LoadFloat_e( "dummy_char_start_offset_y", 0.0f );
	m_vDummyStartOffset.z = rkLoader.LoadFloat_e( "dummy_char_start_offset_z", 0.0f );
	m_fDummyHRotAngle = rkLoader.LoadFloat_e( "dummy_h_rot_angle", 0 );

	m_vDummyJumppingOffset.x = rkLoader.LoadFloat_e( "dummy_char_jump_offset_x", 0.0f );
	m_vDummyJumppingOffset.y = rkLoader.LoadFloat_e( "dummy_char_jump_offset_y", 0.0f );
	m_vDummyJumppingOffset.z = rkLoader.LoadFloat_e( "dummy_char_jump_offset_z", 0.0f );
	
	m_vDummyComboAttackCodeList.clear();
	if( iComboCnt > 0 )
	{
		m_vDummyComboAttackCodeList.reserve( iComboCnt );

		for( int i=0; i < iComboCnt; ++i )
		{
			int iComboNum;
			wsprintf_e( szKey, "dummy_combo_attack_code%d", i+1 );
			iComboNum = rkLoader.LoadInt( szKey, 0 );
			m_vDummyComboAttackCodeList.push_back( iComboNum );
		}
	}

	m_iDummyGrapplingAttackCode = rkLoader.LoadInt_e( "dummy_grappling__attack_code", 0 );
	m_iDummyLandingAttackCode = rkLoader.LoadInt_e( "dummy_landing_attack_code", 0 );
}

bool ioComboAndGrapplingAttackSkill::OnSkillStart( ioBaseChar *pOwner )
{
	if ( !ioAttackSkill::OnSkillStart( pOwner ) )
		return false;

	m_bSetHandMesh = false;
	SetCameraBuff( pOwner );
	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
		pOwner->ShowWeaponItemMesh( false );

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pOwner->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}	

	if( !m_bReduceGauge )
	{
		pOwner->ReduceNeedGaugeBySkill( this, GetNeedGauge(pOwner->GetCharName()) );
		m_bReduceGauge = true;
	}

	ClearData();

	return true;
}

void ioComboAndGrapplingAttackSkill::OnProcessState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if( !pOwner || !pStage )
		return;

	//DWORD dwCurTime = FRAMEGETTIME();
	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < FRAMEGETTIME() )
			{
				m_dwPreDelayStartTime = 0;

				EndPreDelayEffect( pOwner );
				if ( pOwner->IsNeedProcess() )
					SetTeleportAttackState( pOwner, pStage );
			}
		}
		break;
	case SS_TELEPORT_ATTACK:
		{
			if ( m_dwMotionEndTime < FRAMEGETTIME() )
				SetEndState();
		}
		break;
	case SS_LINEAR_COMBO:
		{
			//D키 입력 공격으로 할 것인지 자동으로 할것인지 상의 필요
			if( m_dwMotionEndTime < FRAMEGETTIME() )
			{
				if ( m_iCurLinearCombo < (int)m_ComboList.size() )
					SetLinearComboState( pOwner );
				else
					SetEndState();
			}
		}
		break;
	case SS_GRAPPLING_LOOP:
		{
			DWORD dwCurTime = FRAMEGETTIME();
			// Target 유효 체크
			ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
			if( pTarget )
			{
				if( m_dwMotionStartTime+g_App.GetGrapplingCheckDelay() < dwCurTime &&
					pTarget->GetState() != CS_GRAPPLING_WOUNDED_SKILL )
				{
					SetEndState();
					return;
				}
			}
			else
			{
				SetEndState();
				return;
			}

			if( m_fCharJumpPower > 0.0f )
			{
				if( pOwner->IsNeedProcess() && CheckLanding( pOwner ) )
				{
					SetLandAttackState( pOwner );
					return;
				}
			}
			else if( m_dwMotionEndTime < dwCurTime )
			{
				SetEndState();
				return;
			}

			ProcessLoopDir( pOwner );
		}
		break;
	case SS_LAND_ATTACK:
		{
			if( m_dwMotionEndTime < FRAMEGETTIME() )
			{
				SetEndState();
				return;
			}
		}
	}
	
	ProcessDummyState( pOwner );
}

void ioComboAndGrapplingAttackSkill::SetTeleportAttackState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if( !pOwner || !pStage )
		return;

	m_SkillState = SS_TELEPORT_ATTACK;
	if ( pOwner->IsNeedProcess() )
	{
		FindAttTarget( pOwner, pStage );

		ioBaseChar* pTarget = pOwner->GetBaseChar( m_szAttackTarget );
		if ( pTarget )
		{
			D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
			D3DXVECTOR3 vNewPos = pTarget->GetWorldOrientation() * m_vTeloportTargetOffset;
			vNewPos += vTargetPos;

			pOwner->SetWorldPosition( vNewPos );

			D3DXVECTOR3 vDir = vTargetPos - vNewPos;
			D3DXVec3Normalize( &vDir, &vDir );
			pOwner->SetTargetRotToDir( vDir, true );
		}
		else
		{
			D3DXVECTOR3 vNewPos = pOwner->GetWorldOrientation() * m_vNoneTargetOffset;
			vNewPos += pOwner->GetWorldPosition();
			pOwner->SetWorldPosition( vNewPos );
		}
	}
	
	SetActionAni( pOwner, GetAttribute() );
	CreateDummyChar( pOwner, pStage );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << SST_TELEPORT_ATTACK;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioComboAndGrapplingAttackSkill::FindAttTarget( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if( !pOwner || !pStage )
		 return;

	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_fTargetRange * m_fTargetRange;
	float fCurGap = -FLOAT1;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);
		if( pTarget == pOwner )
			continue;

		if( pOwner->GetCharName() == pTarget->GetCharName() )
			continue;

		if( pOwner->GetTeam() == pTarget->GetTeam() )
			continue;

		if( pTarget->IsSystemState() )
			continue;

		if( !pTarget->IsEnableTarget() ) 
			continue;

		if( pTarget->GetExperienceMode() != EMS_NONE )
			continue;

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( fCurAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = pOwner->GetTargetRot();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );
			D3DXVECTOR3 vDiff2 = vDiff;
			vDiff2.y = 0.0f;
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				continue;
		}

		// 높이 체크
		float fHeightGap = pTarget->GetWorldPosition().y - pTarget->GetBottomHeight();
		if( fHeightGap > 0.0f && fHeightGap > m_fTargetUpHeight )
			continue;

		// 거리 체크
		if( fDiffRangeSq < fTargetRangeSq )
		{
			if( fCurGap == -FLOAT1 )
			{
				fCurGap = fDiffRangeSq;
				m_szAttackTarget = pTarget->GetCharName();
			}
			else if( fCurGap >= 0.0f && fDiffRangeSq < fCurGap )
			{
				fCurGap = fDiffRangeSq;
				m_szAttackTarget = pTarget->GetCharName();
			}
		}
	}
}

void ioComboAndGrapplingAttackSkill::SetActionAni( ioBaseChar *pOwner, const AttackAttribute& rkAttri )
{
	if ( !pOwner )
		return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );

	int iAniID = pGrp->GetAnimationIdx( rkAttri.m_AttackAnimation );
	if( iAniID == -1 )
		return;

	float fTimeRate = rkAttri.m_fAttackAniRate;
	if ( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = rkAttri.m_dwPreDelay;

	if( !rkAttri.m_vWeaponInfoList.empty() )
	{
		pOwner->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
			iAniID,
			rkAttri.m_vWeaponInfoList,
			FTT_SKILL_ATTACK,
			fTimeRate,
			dwPreDelay );
	}

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + rkAttri.m_dwEndDelay;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwFireStartTime = dwCurTime + dwPreDelay;
	dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	float fAttackEnd = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( fAttackEnd > 0.0f )
		m_dwAttackEndTime = dwCurTime + fAttackEnd;
	else
		m_dwAttackEndTime = 0;

	// Col Skip
	pOwner->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	m_dwFireMotionEndTime = pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;
	if ( m_dwFireMotionEndTime > 0 )
		m_dwFireMotionEndTime += dwCurTime + dwPreDelay;

	m_dwEnableReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if ( m_dwEnableReserveTime > 0 )
		m_dwEnableReserveTime += dwPreDelay + dwCurTime;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->SetReservedSliding( rkAttri.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwTrackingTime = pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" );
	if( IsUseAutoTargetValue() )
	{
		pOwner->SetAutoTarget( ATT_SKILL );
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}


void ioComboAndGrapplingAttackSkill::SetEndState()
{
	m_SkillState = SS_END;
	m_dwAttackEndTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += 100;
}

void ioComboAndGrapplingAttackSkill::SetNextSkillState( ioBaseChar *pWounder, ioWeapon *pWeapon /*= NULL */ )
{
	if ( !pWounder || !pWeapon )
		return;
	ioBaseChar *pAttacker = pWeapon->GetOwner();
	if ( !pAttacker )
		return;

	if( pWounder->IsNeedProcess() )
	{
		if( !pWounder->HasBuff( pAttacker->GetCharName(), m_szCheckBuff ) )
		{
			ioItem *pItem = GetOwnerItem();
			if ( !pItem || pItem->GetOwnerName().IsEmpty())
				return;

			SetEndState();
			if ( pWounder->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pItem->GetOwnerName();
				kPacket << GetName();
				kPacket << (int)SST_END;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return;
		}

		ioItem *pItem = GetOwnerItem();
		if ( !pItem || pItem->GetOwnerName().IsEmpty())
			return;

		SetLinearComboState( pAttacker );
		if ( pWounder->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pItem->GetOwnerName();
			kPacket << GetName();
			kPacket << (int)SST_LINEAR_COMBO;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioComboAndGrapplingAttackSkill::SetLinearComboState( ioBaseChar *pOwner )
{
	m_SkillState = SS_LINEAR_COMBO;

	if ( !COMPARE( m_iCurLinearCombo, 0, (int)m_ComboList.size() ))
		return;
	if( m_iCurLinearCombo == 0 )
		m_dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();
	SetActionAni( pOwner, m_ComboList[m_iCurLinearCombo] );

	if ( COMPARE( m_iCurLinearCombo, 0, (int)m_vDummyComboAttackCodeList.size() ))
		DummyAttackState( pOwner, m_vDummyComboAttackCodeList[m_iCurLinearCombo] );
	m_iCurLinearCombo++;
}

void ioComboAndGrapplingAttackSkill::OnSkillEnd( ioBaseChar *pOwner )
{
	ioAttackSkill::OnSkillEnd( pOwner );

	DestroyDummyChar( pOwner );

	// 기본무기 장착
	if( m_bUseItemMesh )
		pOwner->ShowWeaponItemMesh( true );

	// 스킬 중간 동작에 사용된 메쉬들 제거
	if( !m_HandMesh.IsEmpty() )
		pOwner->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	m_bSetHandMesh = false;

	pOwner->SetSkillMoveEnable( false, false );

	EndCameraBuff( pOwner );

	ClearData();
}

bool ioComboAndGrapplingAttackSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if ( m_SkillState == SS_END )
		return true;

	return false;
}

void ioComboAndGrapplingAttackSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	if( pWeapon && pWeapon->CheckGrapplingState() == GPT_WRESTLING && !m_GrapplingTargetName.IsEmpty() )
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
	else if( pWeapon && pWeapon->CheckGrapplingState() == GPT_WRESTLING_ALL && !m_GrapplingTargetName.IsEmpty() )
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
	else if( pWeapon && pWeapon->CheckGrapplingState() == GPT_NO_CHANGE && !m_GrapplingTargetName.IsEmpty() )
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
	else if( pWeapon && pWeapon->CheckGrapplingState() == GPT_NO_CHANGE2 && !m_GrapplingTargetName.IsEmpty() )
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
	else if( pWeapon && pWeapon->CheckGrapplingState() == GPT_EXCEPT && !m_GrapplingTargetName.IsEmpty() )
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pOwner = pZone->GetOwner();
		if( pOwner )
		{
			D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( true, true );
		}
	}
}

bool ioComboAndGrapplingAttackSkill::SetSkillGrapplingState( ioBaseChar *pOwner, const ioHashString &szTargetName )
{
	if( !pOwner )
		return false;
	if( m_SkillState != SS_LINEAR_COMBO )
		return false;

	m_GrapplingTargetName = szTargetName;

	SetGrapplingLoopState( pOwner );
	DummyAttackState( pOwner, m_iDummyGrapplingAttackCode );
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << SST_GRAPPLING_LOOP;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << szTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_GrapplingTargetName );
	if( !pTarget ) return false;

	// for NPC
	bool bNPC = false;
	bool bAttackerNPC = false;
	if( ToNpcChar( pTarget ) )
	{
		bNPC = true;
	}

	if( ToNpcChar( pOwner ) )
	{
		bAttackerNPC = true;
	}

	// local or 훈련소 or NPC
	if( !P2PNetwork::IsNetworkPlaying() || !pOwner->GetCreator()->IsNetworkMode() || (bNPC || bAttackerNPC) )
	{
		D3DXQUATERNION qtCharRot = pOwner->GetTargetRot();
		pTarget->SetTargetRotToRotate( qtCharRot, true, false );
		pTarget->SetSkillGrapplingState( pOwner->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
	}

	return true;
}

void ioComboAndGrapplingAttackSkill::SetGrapplingLoopState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	if( m_SkillState != SS_LINEAR_COMBO )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_SkillState = SS_GRAPPLING_LOOP;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	if( m_fGrapplingLoopRate > 0.0f )
		fAniRate = m_fGrapplingLoopRate;

	int iAniID = pGrp->GetAnimationIdx( m_GrapplingLoopMotion );

	if( m_fCharJumpPower > 0.0f )
	{
		pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );
		pOwner->SetJumpPower( m_fCharJumpPower );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );
		m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fAniRate;
	}
}

bool ioComboAndGrapplingAttackSkill::CheckLanding( ioBaseChar *pOwner )
{
	if( !pOwner ) return true;

	float fHeightGap = 0.0f;
	if( pOwner->CheckGhostState() )
		fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetPreHeightForGhostState();
	else
		fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();

	if( fHeightGap > 0.0f || !pOwner->IsDownState() )
	{
		return false;
	}

	return true;
}

void ioComboAndGrapplingAttackSkill::ProcessLoopDir( ioBaseChar *pOwner )
{
	if( m_dwGrapplingLoopRotateTime == 0 )
		return;

	// 캐릭터 회전
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	float fRate = (fTimePerSec*1000) / m_dwGrapplingLoopRotateTime;
	float fRotateAngle = 360.0f * fRate;

	fRotateAngle = ioMath::ArrangeHead( fRotateAngle );

	D3DXQUATERNION	qtNewRot;
	D3DXQuaternionRotationYawPitchRoll( &qtNewRot, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );

	D3DXQUATERNION qtCharRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vDir = qtCharRot * D3DXVECTOR3( 0.0, 0.0, -FLOAT1 );
	vDir = qtNewRot * vDir;

	ioMath::CalcDirectionQuaternion( qtNewRot, -vDir );
	pOwner->SetTargetRotToRotate( qtNewRot, true, false );
}

void ioComboAndGrapplingAttackSkill::SetLandAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	m_SkillState = SS_LAND_ATTACK;

	SetActionAni( pOwner, m_LandAttackAttribute );

	DummyAttackState( pOwner, m_iDummyLandingAttackCode );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << SST_LAND_ATTACK;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

D3DXVECTOR3 ioComboAndGrapplingAttackSkill::GetGrapplingTargetPos( ioBaseChar *pOwner )
{
	if( m_fTargetGapRange > 0.0f )
	{
		D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate( m_fCharHeightRate );
		D3DXVECTOR3 vDir = pOwner->GetMoveDir();

		if( m_fTargetGapRange > 0.0f )
			vPos += m_fTargetGapRange * vDir;

		return vPos;
	}

	return ioMath::VEC3_ZERO;
}

void ioComboAndGrapplingAttackSkill::CreateDummyChar( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if ( !pOwner || !pStage )
		return;

	if( m_szDummyCharName.IsEmpty() ) 
		return;

	float fAngleH = ioMath::ArrangeHead( m_fDummyHRotAngle );
	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngleH) );

	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	vDir = qtRot * vDir;
	D3DXVec3Normalize( &vDir, &vDir );
	qtRot = Help::ConvertDirToQuaternion( vDir );
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + ( qtRot * m_vDummyStartOffset );

	int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
	m_iDummyIndex = iCurIndex;

	ioDummyChar *pDummy = pStage->CreateDummyChar( m_szDummyCharName, m_iDummyIndex,
		pOwner->GetCharName(),
		vPos,
		0.0f,
		0, true );
}

void ioComboAndGrapplingAttackSkill::DestroyDummyChar( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	g_DummyCharMgr.DestroyDummyCharByName( pOwner, m_szDummyCharName );
	m_iDummyIndex = -1;
}

void ioComboAndGrapplingAttackSkill::DummyAttackState( ioBaseChar *pOwner, int iAttackCode )
{
	if ( !pOwner )
		return;

	if ( iAttackCode== 0 )
		return;

	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_iDummyIndex );
	if ( !pDummy )
		return;

	ioNoneTraceDummy2 *pNoneTraceDummy2 = ToNoneTraceDummy2( pDummy );
	if ( !pNoneTraceDummy2 )
		return;

	pNoneTraceDummy2->SetAttackState( iAttackCode );
}

void ioComboAndGrapplingAttackSkill::ProcessDummyState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_iDummyIndex );
	if ( !pDummy )
		return;

	float fAngleH = ioMath::ArrangeHead( m_fDummyHRotAngle );
	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngleH) );

	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	vDir = qtRot * vDir;
	D3DXVec3Normalize( &vDir, &vDir );
	qtRot = Help::ConvertDirToQuaternion( vDir );
	pDummy->SetWorldOrientation( qtRot );
	
	D3DXVECTOR3 vPos = ioMath::VEC3_ZERO;
	if ( m_SkillState == SS_TELEPORT_ATTACK ||  m_SkillState == SS_LINEAR_COMBO )
		vPos = pOwner->GetWorldPosition() + ( qtRot * m_vDummyStartOffset );
	else
		vPos = pOwner->GetWorldPosition() + ( qtRot * m_vDummyJumppingOffset );

	pDummy->SetWorldPosition( vPos );
	
}

void ioComboAndGrapplingAttackSkill::ApplyExtraSkillInfo( ioBaseChar *pOwner, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;
	if( !pStage )
		return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case SST_TELEPORT_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotToRotate( qtRot, true, false );

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );

			SetTeleportAttackState( pOwner, pStage );
		}
		break;
	case SST_LINEAR_COMBO:
		SetLinearComboState( pOwner );
		break;
	case SST_END:
		SetEndState();
		break;
	case SST_GRAPPLING_LOOP:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotToRotate( qtRot, true, false );

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );
			rkPacket >> m_GrapplingTargetName;

			SetGrapplingLoopState( pOwner );
			DummyAttackState( pOwner, m_iDummyGrapplingAttackCode );

			ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
			if( pTarget && pTarget->IsNeedProcess() )
				pTarget->SetSkillGrapplingState( pOwner->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
		}
		break;
	case SST_LAND_ATTACK:
		SetLandAttackState( pOwner );
		break;
	}
}
