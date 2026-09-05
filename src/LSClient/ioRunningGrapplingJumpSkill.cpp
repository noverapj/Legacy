

#include "stdafx.h"

#include "ioNpcChar.h"
#include "ioPlayStage.h"

#include "WeaponDefine.h"

#include "ioRunningGrapplingJumpSkill.h"

ioRunningGrapplingJumpSkill::ioRunningGrapplingJumpSkill()
{
	ClearData();
}

ioRunningGrapplingJumpSkill::ioRunningGrapplingJumpSkill( const ioRunningGrapplingJumpSkill &rhs )
: ioAttackSkill( rhs ),
  m_LoopEffect( rhs.m_LoopEffect ),
  m_LoopSound( rhs.m_LoopSound ),
  m_GrapLoopMotion( rhs.m_GrapLoopMotion ),
  m_fGrapLoopMotionRate( rhs.m_fGrapLoopMotionRate ),
  m_EndAttackAttribute( rhs.m_EndAttackAttribute ),
  m_fTargetLoopGapRange( rhs.m_fTargetLoopGapRange ),
  m_fCharHeightRate( rhs.m_fCharHeightRate ),
  m_dwSkillLoopProtectTime( rhs.m_dwSkillLoopProtectTime ),
  m_dwRunLoopDuration( rhs.m_dwRunLoopDuration ),
  m_fRunSpeed( rhs.m_fRunSpeed ),
  m_dwRunRotateTime( rhs.m_dwRunRotateTime ),
  m_fCharJumpPower( rhs.m_fCharJumpPower ),
  m_dwJumpMoveSpeed( rhs.m_dwJumpMoveSpeed ),
  m_dwJumpMoveRotateSpeed( rhs.m_dwJumpMoveRotateSpeed ),
  m_dwJumpLoopRotateTime( rhs.m_dwJumpLoopRotateTime )
{
	ClearData();
}

ioRunningGrapplingJumpSkill::~ioRunningGrapplingJumpSkill()
{
}

void ioRunningGrapplingJumpSkill::ClearData()
{
	m_SkillState = SS_NONE;

	m_dwRunLoopStartTime	= 0;
	m_dwRunLoopEndTime		= 0;

	m_LoopSoundID = 0;

	m_bTargetRot	= false;
	m_bLeftRot		= false;

	m_CurDirKey		= ioUserKeyInput::DKI_NONE;
	m_vCurMoveDir	= ioMath::VEC3_ZERO;
	m_vCharMoveDir	= ioMath::VEC3_ZERO;

	D3DXQuaternionIdentity( &m_qtCurRotate );

	m_bSetHandMesh	= false;

	m_GrapplingTargetName.Clear();
}

int ioRunningGrapplingJumpSkill::GetSkillState()
{
	return m_SkillState;
}

void ioRunningGrapplingJumpSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szTitle[MAX_PATH] = "";

	// Effect/Sound for LoopAnimation
	rkLoader.LoadString_e( "attack_loop_effect", "", szBuf, MAX_PATH);
	m_LoopEffect = szBuf;
	
	rkLoader.LoadString_e( "attack_loop_sound", "", szBuf, MAX_PATH);
	m_LoopSound = szBuf;

	// Grappling Motion
	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_GrapLoopMotion = szBuf;
	m_fGrapLoopMotionRate	= rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );

	// End Motion
	m_EndAttackAttribute.Init();
	LoadAttackAttribute_e( "skill_end_attack", m_EndAttackAttribute, rkLoader );

	m_fTargetLoopGapRange	= rkLoader.LoadFloat_e( "skill_target_loop_gap_range", 0.0f );
	m_fCharHeightRate		= rkLoader.LoadFloat_e( "char_height_rate", FLOAT05 );
	m_dwSkillLoopProtectTime= (DWORD)rkLoader.LoadInt_e( "skill_loop_protect_time", 0 );
	
	m_dwRunLoopDuration		= (DWORD)rkLoader.LoadInt_e( "run_duration", 0 );
	m_fRunSpeed				= rkLoader.LoadFloat_e( "run_speed", 0.0f );
	m_dwRunRotateTime		= (DWORD)rkLoader.LoadInt_e( "run_rotate_speed", 0 );

	m_fCharJumpPower		= rkLoader.LoadFloat_e( "jump_power", FLOAT1 );
	m_dwJumpMoveSpeed		= (DWORD)rkLoader.LoadInt_e( "jump_move_speed", 0 );
	m_dwJumpMoveRotateSpeed	= (DWORD)rkLoader.LoadInt_e( "jump_move_rotate_speed", 0 );
	m_dwJumpLoopRotateTime	= (DWORD)rkLoader.LoadInt_e( "jump_loop_rotate_speed", 0 );
}

ioSkill* ioRunningGrapplingJumpSkill::Clone()
{
	return new ioRunningGrapplingJumpSkill( *this );
}

bool ioRunningGrapplingJumpSkill::OnSkillStart( ioBaseChar *pChar )
{
	ClearData();

	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( false );

	return true;
}

void ioRunningGrapplingJumpSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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
				EndPreDelayEffect( pChar );
				SetSkillRunState( pChar );
			}
		}
		break;
	case SS_RUN:
		{
			if( m_dwRunLoopEndTime < dwCurTime )
			{
				SetSkillRunEndState( pChar );
				return;
			}
			else
			{				
				pChar->SetCurMoveSpeed( m_fRunSpeed );
			}
		}
		break;
	case SS_ACTION:
		{
			// Target 유효 체크
			ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
			if( pTarget )
			{
				if( m_dwRunLoopEndTime+g_App.GetGrapplingCheckDelay() < dwCurTime &&
					pTarget->GetState() != CS_GRAPPLING_WOUNDED_SKILL )
				{
					pChar->SetCurMoveSpeed( 0 );
					m_SkillState = SS_JUMP;
					return;
				}
			}
			else
			{
				pChar->SetCurMoveSpeed( 0 );
				m_SkillState = SS_JUMP;
				return;
			}

			CheckKeyInput( pChar );
			ProcessRotateOnJump( pChar );

			if( CheckLanding( pChar ) )
			{
				SetSkillEndState( pChar );
				return;
			}

			ProcessLoopDir( pChar );
			ProcessMoveDir( pChar, pStage );
		}
		break;
	case SS_END:
		break;
	}
}

void ioRunningGrapplingJumpSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	pChar->EndEffect( m_LoopEffect );
	StopLoopSound( m_LoopSound, m_LoopSoundID );

	ClearData();

	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	// 스킬 중간 동작에 사용된 메쉬들 제거
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
}

void ioRunningGrapplingJumpSkill::AttackSkillFire( ioBaseChar *pChar )
{
	if( !pChar ) return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )	return;

	float fTimeRate  = GetAttackTimeRate();
	DWORD dwPreDelay = GetAttribute().m_dwPreDelay;

	if( m_bRefreshFireTimeList )
	{
		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
											iAniID,
											GetAttribute().m_vWeaponInfoList,
											FTT_SKILL_ATTACK,
											fTimeRate,
											dwPreDelay );
	}

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	pChar->AttachEffect( m_SkillEffectName );

	pChar->SetReservedSliding( GetAttribute().m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}	

	m_GrapplingTargetName.Clear();

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}

bool ioRunningGrapplingJumpSkill::SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName )
{
	if( !pChar ) return false;
	if( m_SkillState != SS_RUN )		return false;

	// 충돌처리 후 체크된 타겟을 m_GrapplingTargetName로 지정한다.
	m_GrapplingTargetName = szTargetName;
	
	ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
	if( !pTarget ) return false;

	pChar->EndEffect( m_LoopEffect );
	StopLoopSound( m_LoopSound, m_LoopSoundID );

	SetSkillActionState( pChar );

	// 점프중 이동을 위한 디렉션과 행렬 저장.
	D3DXVECTOR3 vDir;
	vDir = pTarget->GetWorldPosition() - pChar->GetWorldPosition();
	vDir.y = 0.0f;
	D3DXVec3Normalize( &m_vCharMoveDir, &vDir );
	m_qtCurRotate = pChar->GetTargetRot();

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_GRAPPLING;
		kPacket << szTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	// for NPC
	bool bNPC = false;
	if( ToNpcChar( pTarget ) )
	{
		bNPC = true;
	}

	// local or 훈련소 or NPC
	if( !P2PNetwork::IsNetworkPlaying() || !pChar->GetCreator()->IsNetworkMode() || bNPC )
	{
		pTarget->SetTargetRotToRotate( pChar->GetTargetRot(), true, false );
		pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
	}

	return true;
}

D3DXVECTOR3 ioRunningGrapplingJumpSkill::GetGrapplingTargetPos( ioBaseChar *pChar )
{
	if( m_fTargetLoopGapRange > 0.0f )
	{
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( m_fCharHeightRate );
		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );

		if( m_fTargetLoopGapRange > 0.0f )
			vPos += m_fTargetLoopGapRange * vDir;

		return vPos;
	}

	return ioMath::VEC3_ZERO;
}

void ioRunningGrapplingJumpSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar ) return;
	if( !pStage ) return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case SSC_GRAPPLING:
		{
			rkPacket >> m_GrapplingTargetName;

			SetSkillActionState( pChar );

			ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );

			if( pTarget && pTarget->IsNeedProcess() )
			{
				pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
			}
		}
		break;
	case SSC_RUN_ROTATE:
	case SSC_JUMP_ROTATE:
		{
			int iDir;
			rkPacket >> iDir;
			rkPacket >> m_bLeftRot;

			DWORD dwCurTime = FRAMEGETTIME();
			int iAniID = 0;

			if( iDir == ioUserKeyInput::DKI_NONE )
			{
				m_CurDirKey = ioUserKeyInput::DKI_NONE;
			}
			else
			{
				m_bTargetRot = false;

				switch( iDir )
				{
				case ioUserKeyInput::DKI_UP:
					m_CurDirKey = ioUserKeyInput::DKI_UP;
					break;
				case ioUserKeyInput::DKI_RIGHTUP:
					m_CurDirKey = ioUserKeyInput::DKI_RIGHTUP;
					break;
				case ioUserKeyInput::DKI_RIGHT:
					m_CurDirKey = ioUserKeyInput::DKI_RIGHT;
					break;
				case ioUserKeyInput::DKI_RIGHTDOWN:
					m_CurDirKey = ioUserKeyInput::DKI_RIGHTDOWN;
					break;
				case ioUserKeyInput::DKI_DOWN:
					m_CurDirKey = ioUserKeyInput::DKI_DOWN;
					break;
				case ioUserKeyInput::DKI_LEFTDOWN:
					m_CurDirKey = ioUserKeyInput::DKI_LEFTDOWN;
					break;
				case ioUserKeyInput::DKI_LEFT:
					m_CurDirKey = ioUserKeyInput::DKI_LEFT;
					break;
				case ioUserKeyInput::DKI_LEFTUP:
					m_CurDirKey = ioUserKeyInput::DKI_LEFTUP;
					break;
				default:
					m_CurDirKey = ioUserKeyInput::DKI_NONE;
					break;
				}
			}
		}
		break;
	}

}

void ioRunningGrapplingJumpSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	pWeapon->SetLiveTime( m_dwRunLoopDuration );

	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	if( pWeapon && pWeapon->CheckGrapplingState() == GPT_WRESTLING && !m_GrapplingTargetName.IsEmpty() )
	{
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
	}
	else if( pWeapon && pWeapon->CheckGrapplingState() == GPT_WRESTLING_ALL && !m_GrapplingTargetName.IsEmpty() )
	{
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
	}

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
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

bool ioRunningGrapplingJumpSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_NONE:
	case SS_RUN:
	case SS_ACTION:
		return false;
	case SS_END:
		if( m_dwMotionEndTime + dwActionStop < dwCurTime )
		{
			return true;
		}
		return false;
	}

	return false;
}

bool ioRunningGrapplingJumpSkill::IsProtected( int iDefenseBreakType ) const
{
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

	switch( m_SkillState )
	{
	case SS_NONE:
		return false;
		break;
	case SS_RUN:
	case SS_ACTION:
	case SS_END:
		if( m_dwRunLoopStartTime+m_dwSkillLoopProtectTime < dwCurTime )
			return false;

		return true;
	}

	return false;
}

bool ioRunningGrapplingJumpSkill::IsEnableReserve() const
{
	if( m_dwPreDelayStartTime != 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_END:
		if( m_dwEnableReserveTime == 0 || m_dwMotionStartTime >= m_dwEnableReserveTime )
			return false;

		if( m_dwEnableReserveTime < FRAMEGETTIME() )
			return true;
		break;
	}

	return false;
}

bool ioRunningGrapplingJumpSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioRunningGrapplingJumpSkill::CheckKeyInput( ioBaseChar *pOwner )
{
	
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

	if( pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != m_CurDirKey )
		{
			float fYawD = 180.0f + 45.0f * ( eNewDirKey - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			D3DXVECTOR3 vTargetDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVECTOR3 vDir;
			D3DXQUATERNION qtCurRot;

			switch( m_SkillState )
			{
			case SS_RUN:
				{
					vDir = pOwner->GetMoveDir();
					qtCurRot = pOwner->GetTargetRot();
				}
				break;
			case SS_ACTION:
				{
					vDir = m_vCurMoveDir;
					qtCurRot = m_qtCurRotate;
				}
				break;
			default:
				{
					vDir = ioMath::VEC3_ZERO;
					D3DXQuaternionIdentity( &qtCurRot );
				}
			}

			m_bTargetRot = false;
			m_bLeftRot = false;
			if( D3DXVec3Dot( &vTargetDir, &vDir ) > -FLOAT1 )
			{
				float fCurYaw, fTargetYaw;
				fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
				fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

				fCurYaw = ioMath::ArrangeHead( fCurYaw );
				fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

				float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
				if( fRotAngle < 0.0f )
					m_bLeftRot = true;
			}

			m_CurDirKey = eNewDirKey;

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pOwner->GetCharName();
				kPacket << GetName();
				kPacket << SSC_RUN_ROTATE;
				kPacket << (int)m_CurDirKey;
				kPacket << m_bLeftRot;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	else
	{
		if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
		{
			m_CurDirKey = ioUserKeyInput::DKI_NONE;

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pOwner->GetCharName();
				kPacket << GetName();
				kPacket << SSC_RUN_ROTATE;
				kPacket << (int)m_CurDirKey;
				kPacket << m_bLeftRot;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

bool ioRunningGrapplingJumpSkill::CheckLanding( ioBaseChar *pChar )
{
	if( !pChar ) return true;

	DWORD dwCurTime = FRAMEGETTIME();

	float fHeightGap = 0.0f;
	if( pChar->CheckGhostState() )
	{
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetPreHeightForGhostState();
	}
	else
	{
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetBottomHeight();
	}
	
	if( !pChar->CheckLandingInfo( fHeightGap ) )
		return false;
	//if( fHeightGap > 0.0f || !pChar->IsDownState() )	return false;

	return true;
}

void ioRunningGrapplingJumpSkill::SetSkillRunState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( m_SkillState != SS_NONE )	return;

	pChar->AttachEffect( m_LoopEffect );
	m_LoopSoundID = PlayLoopSound( pChar, m_LoopSound );

	m_SkillState = SS_RUN;

	m_dwRunLoopStartTime = FRAMEGETTIME();
	m_dwRunLoopEndTime = m_dwRunLoopStartTime;
	m_dwRunLoopEndTime += m_dwRunLoopDuration;

	AttackSkillFire( pChar );
}

void ioRunningGrapplingJumpSkill::SetSkillRunEndState( ioBaseChar *pChar )
{
	pChar->SetCurMoveSpeed( 0 );
	pChar->EndEffect( m_LoopEffect );
	StopLoopSound( m_LoopSound, m_LoopSoundID );
	m_SkillState = SS_END;
}

void ioRunningGrapplingJumpSkill::SetSkillActionState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( m_SkillState != SS_RUN )	return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	// 이동중 타겟이 잡혔을 경우 Weapon의 LiveTime을 0으로 셋팅.
	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwWeaponBaseIndex);
	if( !pWeapon )	return;
	
	pWeapon->SetLiveTime( 0 );

	m_SkillState = SS_ACTION;

	pChar->SetJumpPower( m_fCharJumpPower );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	if( m_fGrapLoopMotionRate > 0.0f )
		fAniRate = m_fGrapLoopMotionRate;

	int iAniID = pGrp->GetAnimationIdx( m_GrapLoopMotion );//

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	pChar->SetCurMoveSpeed( 0 );
}

void ioRunningGrapplingJumpSkill::SetSkillEndState( ioBaseChar *pChar )
{
	pChar->SetCurMoveSpeed( 0 );

	if( !pChar ) return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	m_SkillState = SS_END;

	pChar->EndEffect( m_LoopEffect );
	StopLoopSound( m_LoopSound, m_LoopSoundID );

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;

	if( m_EndAttackAttribute.m_fAttackAniRate > 0.0f )
		fAniRate = m_EndAttackAttribute.m_fAttackAniRate;
	
	int iAniID = pGrp->GetAnimationIdx( m_EndAttackAttribute.m_AttackAnimation );
	
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fAniRate;

	m_dwFireMotionEndTime = dwCurTime;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fAniRate;

	// Key Reserve
	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" );

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
										iAniID,
										m_EndAttackAttribute.m_vWeaponInfoList,
										FTT_SKILL_ATTACK,
										fAniRate,
										m_EndAttackAttribute.m_dwPreDelay );
}

void ioRunningGrapplingJumpSkill::ProcessRotateOnJump( ioBaseChar *pChar )
{
	if( !pChar ) return;

	if( pChar->IsSettedDirection() )
	{
		float fFrameGap = g_FrameTimer.GetCurFrameGap();
		float fRate = fFrameGap / m_dwJumpMoveRotateSpeed;
		float fRotateAngle = 360.0f * fRate;

		float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );

		float fYawR = DEGtoRAD( fYawD );

		D3DXQUATERNION qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

		float fCurYaw, fTargetYaw;
		fCurYaw = RADtoDEG( ioMath::QuaterToYaw( m_qtCurRotate ) );
		fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

		fCurYaw = ioMath::ArrangeHead( fCurYaw );
		fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

		float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );

		if( fabs( fRotAngle ) < fRotateAngle )
		{			
			m_vCharMoveDir = qtRotate * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
			m_qtCurRotate = qtRotate;
			return;
		}

		if( m_bLeftRot )
			fRotateAngle *= -1;

		fRotateAngle = ioMath::ArrangeHead( fRotateAngle );

		D3DXQUATERNION	qtAngle;
		D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );

		m_vCharMoveDir = qtAngle * m_vCharMoveDir;
		D3DXVec3Normalize( &m_vCharMoveDir, &m_vCharMoveDir );

		ioMath::CalcDirectionQuaternion( qtAngle, -m_vCharMoveDir );
		m_qtCurRotate = qtAngle;
	}
}

void ioRunningGrapplingJumpSkill::ProcessLoopDir( ioBaseChar *pChar )
{
	if( m_dwJumpLoopRotateTime == 0 )
		return;

	// 캐릭터 회전
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	float fRate = (fTimePerSec*1000) / m_dwJumpLoopRotateTime;
	float fRotateAngle = 360.0f * fRate;

	fRotateAngle = ioMath::ArrangeHead( fRotateAngle );

	D3DXQUATERNION	qtNewRot;
	D3DXQuaternionRotationYawPitchRoll( &qtNewRot, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );

	D3DXQUATERNION qtCharRot = pChar->GetTargetRot();
	D3DXVECTOR3 vDir = qtCharRot * D3DXVECTOR3( 0.0, 0.0, -FLOAT1 );
	vDir = qtNewRot * vDir;

	ioMath::CalcDirectionQuaternion( qtNewRot, -vDir );
	pChar->SetTargetRotToRotate( qtNewRot, true, false );
}

void ioRunningGrapplingJumpSkill::ProcessMoveDir( ioBaseChar *pChar, ioPlayStage *pStage )
{
	
	if( !pChar ) return;
	if( !pStage ) return;
	if( m_dwJumpMoveSpeed == 0.0f ) return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt = m_vCharMoveDir * m_dwJumpMoveSpeed * fDivideTimePerSec;
		if( vMoveAmt != ioMath::VEC3_ZERO )
		{
			vTotalMove += vMoveAmt;
			bCurMove = true;
		}
	}

	if( bCurMove )
	{
		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( pChar, &vTotalMove, bCol ) )
		{
			pChar->Translate( vTotalMove );
		}
	}

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	if( pChar->CheckAxisLimit( &vPos ) )
		pChar->SetWorldPosition( vPos );
}

UniqueObjID ioRunningGrapplingJumpSkill::PlayLoopSound( ioBaseChar *pChar, ioHashString &szSoundName )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	
	UniqueObjID eSoundID = g_SoundMgr.PlaySound( szSoundName, pGrp->GetParentSceneNode(), PSM_LOOP | PSM_THREAD );
	return eSoundID;
}

void ioRunningGrapplingJumpSkill::StopLoopSound( ioHashString &szSoundName, UniqueObjID eSoundID )
{
	g_SoundMgr.StopSound( szSoundName, eSoundID );
	//g_SoundMgr.StopSound( szSoundName, 0 );
}



