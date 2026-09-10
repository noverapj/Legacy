

#include "stdafx.h"

#include "ioNpcChar.h"
#include "ioPlayStage.h"

#include "ioRunningGrapplingSkill.h"

#include "WeaponDefine.h"

ioRunningGrapplingSkill::ioRunningGrapplingSkill()
{
	ClearData();

	m_BuffSkill.clear();
	m_EnemyBuffSkill.clear();
}

ioRunningGrapplingSkill::ioRunningGrapplingSkill( const ioRunningGrapplingSkill &rhs )
: ioAttackSkill( rhs ),
  m_BuffSkill( rhs.m_BuffSkill ),
  m_EnemyBuffSkill( rhs.m_EnemyBuffSkill ),
  m_LoopEffect( rhs.m_LoopEffect ),
  m_LoopSound( rhs.m_LoopSound ),
  m_GrapLoopMotion( rhs.m_GrapLoopMotion ),
  m_fGrapLoopMotionRate( rhs.m_fGrapLoopMotionRate ),
  m_dwGrapLoopDuration( rhs.m_dwGrapLoopDuration ),
  m_EndAttackAttribute( rhs.m_EndAttackAttribute ),
  m_fTargetLoopGapRange( rhs.m_fTargetLoopGapRange ),
  m_fCharHeightRate( rhs.m_fCharHeightRate ),
  m_dwSkillLoopProtectTime( rhs.m_dwSkillLoopProtectTime ),
  m_dwRunLoopDuration( rhs.m_dwRunLoopDuration ),
  m_fRunSpeed( rhs.m_fRunSpeed ),
  m_dwRunRotateTime( rhs.m_dwRunRotateTime )
{
	ClearData();
}

ioRunningGrapplingSkill::~ioRunningGrapplingSkill()
{
	m_BuffSkill.clear();
	m_EnemyBuffSkill.clear();
}

void ioRunningGrapplingSkill::ClearData()
{
	m_SkillState = SS_NONE;

	m_dwRunLoopStartTime	= 0;
	m_dwRunLoopEndTime		= 0;

	m_dwGrapLoopStartTime	= 0;
	m_dwGrapLoopEndTime		= 0;

	m_LoopSoundID = 0;

	m_bTargetRot	= false;
	m_bLeftRot		= false;

	m_CurDirKey		= ioUserKeyInput::DKI_NONE;

	m_bSetHandMesh	= false;

	m_GrapplingTargetName.Clear();
}

void ioRunningGrapplingSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szTitle[MAX_PATH] = "";

	// Buff
	int iBuffCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	m_BuffSkill.reserve( iBuffCnt );
	for( int i=0; i<iBuffCnt; ++i )
	{
		wsprintf_e( szTitle, "owner_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
		m_BuffSkill.push_back( szBuf );
	}

	iBuffCnt = rkLoader.LoadInt_e( "enemy_buff_cnt", 0 );
	for( int i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szTitle, "enemy_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
		m_EnemyBuffSkill.push_back( szBuf );
	}

	// Effect/Sound for LoopAnimation
	rkLoader.LoadString_e( "attack_loop_effect", "", szBuf, MAX_PATH);
	m_LoopEffect = szBuf;
	
	rkLoader.LoadString_e( "attack_loop_sound", "", szBuf, MAX_PATH);
	m_LoopSound = szBuf;

	// Grappling Motion
	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_GrapLoopMotion = szBuf;
	m_fGrapLoopMotionRate	= rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );

	// Grappling Motion Time
	m_dwGrapLoopDuration	= (DWORD)rkLoader.LoadInt_e( "skill_loop_duration", 0 );

	// End Motion
	m_EndAttackAttribute.Init();
	LoadAttackAttribute_e( "skill_end_attack", m_EndAttackAttribute, rkLoader );

	m_fTargetLoopGapRange	= rkLoader.LoadFloat_e( "skill_target_loop_gap_range", 0.0f );
	m_fCharHeightRate		= rkLoader.LoadFloat_e( "char_height_rate", FLOAT05 );
	m_dwSkillLoopProtectTime= (DWORD)rkLoader.LoadInt_e( "skill_loop_protect_time", 0 );
	
	m_dwRunLoopDuration		= (DWORD)rkLoader.LoadInt_e( "run_duration", 0 );
	m_fRunSpeed				= (DWORD)rkLoader.LoadFloat_e( "run_speed", 0.0f );
	m_dwRunRotateTime		= (DWORD)rkLoader.LoadInt_e( "run_rotate_speed", 0 );
}

ioSkill* ioRunningGrapplingSkill::Clone()
{
	return new ioRunningGrapplingSkill( *this );
}

bool ioRunningGrapplingSkill::OnSkillStart( ioBaseChar *pChar )
{
	ClearData();

	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( false );

	return true;
}

void ioRunningGrapplingSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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
				CheckKeyInput( pChar );
				ProcessRotate( pChar );
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
				if( m_dwGrapLoopStartTime+g_App.GetGrapplingCheckDelay() < dwCurTime &&
					pTarget->GetState() != CS_GRAPPLING_WOUNDED_SKILL )
				{
					SetSkillEndState( pChar, false );
					return;
				}
			}
			else
			{
				SetSkillEndState( pChar, false );
				return;
			}

			if( m_dwGrapLoopEndTime < dwCurTime )
			{
				SetSkillEndState( pChar, true );
				return;
			}
		}
		break;
	case SS_END:
		break;
	}
}

void ioRunningGrapplingSkill::OnSkillEnd( ioBaseChar *pChar )
{
	RemoveEnemyBuff( pChar );

	ioAttackSkill::OnSkillEnd( pChar );

	pChar->EndEffect( m_LoopEffect );
	StopLoopSound();

	ClearData();

	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	// 스킬 중간 동작에 사용된 메쉬들 제거
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
}

void ioRunningGrapplingSkill::AttackSkillFire( ioBaseChar *pChar )
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

void ioRunningGrapplingSkill::SetSkillActionState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( m_SkillState != SS_RUN )	return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwWeaponBaseIndex);
	if( pWeapon )
	{
		pWeapon->SetLiveTime( 0 );
	}

	m_dwGrapLoopStartTime = FRAMEGETTIME();
	m_dwGrapLoopEndTime = m_dwGrapLoopStartTime;
	m_dwGrapLoopEndTime += m_dwGrapLoopDuration;

	m_SkillState = SS_ACTION;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	if( m_fGrapLoopMotionRate > 0.0f )
		fAniRate = m_fGrapLoopMotionRate;

	int iAniID = pGrp->GetAnimationIdx( m_GrapLoopMotion );//

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	pChar->SetCurMoveSpeed( 0 );

	// enemy buff
	AddEnemyBuff( pChar );
}

void ioRunningGrapplingSkill::AddEnemyBuff( ioBaseChar *pOwner )
{
	if( !pOwner )
		 return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
		if( pTarget )
		{
			int iEnemyBuffCnt = m_EnemyBuffSkill.size();
			for( int i=0; i < iEnemyBuffCnt; ++i )
			{
				pTarget->AddNewBuff( m_EnemyBuffSkill[i], "", "", this );
			}
		}
	}
}

void ioRunningGrapplingSkill::RemoveEnemyBuff( ioBaseChar *pOwner )
{
	if( !pOwner )
		 return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
		if( pTarget )
		{
			int iEnemyBuffCnt = m_EnemyBuffSkill.size();
			for( int i=0; i < iEnemyBuffCnt; ++i )
			{
				pTarget->RemoveBuff(  m_EnemyBuffSkill[i] );
			}
		}
	}
}

void ioRunningGrapplingSkill::SetSkillRunState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( m_SkillState != SS_NONE )	return;

	pChar->AttachEffect( m_LoopEffect );
	PlayLoopSound( pChar );

	m_SkillState = SS_RUN;

	m_dwRunLoopStartTime = FRAMEGETTIME();
	m_dwRunLoopEndTime = m_dwRunLoopStartTime;
	m_dwRunLoopEndTime += m_dwRunLoopDuration;

	AttackSkillFire( pChar );
}

void ioRunningGrapplingSkill::SetSkillRunEndState( ioBaseChar *pChar )
{
	pChar->SetCurMoveSpeed( 0 );
	pChar->EndEffect( m_LoopEffect );
	StopLoopSound();
	m_SkillState = SS_END;
}

void ioRunningGrapplingSkill::SetSkillEndState( ioBaseChar *pChar, bool bSuccess )
{
	if( !pChar ) return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	RemoveEnemyBuff( pChar );

	m_SkillState = SS_END;

	pChar->EndEffect( m_LoopEffect );
	StopLoopSound();

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
	
	int iBuffCnt = m_BuffSkill.size();
	for( int i=0; i < iBuffCnt; ++i)
	{
		pChar->AddNewBuff( m_BuffSkill[i], "", "", this );
	}
}

void ioRunningGrapplingSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
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
			pZone->SetGrapplingTargetName( m_GrapplingTargetName );
		}
	}

}

bool ioRunningGrapplingSkill::SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName )
{
	if( !pChar ) return false;
	if( m_SkillState != SS_RUN )		return false;

	// 충돌처리 후 체크된 타겟을 m_GrapplingTargetName로 지정한다.
	m_GrapplingTargetName = szTargetName;
	
	ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
	if( !pTarget ) return false;

	pChar->EndEffect( m_LoopEffect );
	StopLoopSound();

	SetSkillActionState( pChar );

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
	bool bAttackerNPC = false;
	if( ToNpcChar( pTarget ) )
	{
		bNPC = true;
	}

	if( ToNpcChar( pChar ) )
	{
		bAttackerNPC = true;
	}

	// local or 훈련소 or NPC
	if( !P2PNetwork::IsNetworkPlaying() || !pChar->GetCreator()->IsNetworkMode() || (bNPC || bAttackerNPC) )
	{
		pTarget->SetTargetRotToRotate( pChar->GetTargetRot(), true, false );
		pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
	}

	return true;
}

D3DXVECTOR3 ioRunningGrapplingSkill::GetGrapplingTargetPos( ioBaseChar *pChar )
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

bool ioRunningGrapplingSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
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

void ioRunningGrapplingSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar ) return;
	if( !pStage ) return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case SSC_GRAPPLING:
		{
			ioHashString szTargetName;
			rkPacket >> m_GrapplingTargetName;

			pChar->EndEffect( m_LoopEffect );
			StopLoopSound();

			SetSkillActionState( pChar );

			ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );

			if( pTarget && pTarget->IsNeedProcess() )
			{
				pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
			}
		}
		break;
	case SSC_ROTATE:
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

bool ioRunningGrapplingSkill::IsProtected( int iDefenseBreakType ) const
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

bool ioRunningGrapplingSkill::IsEnableReserve() const
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

void ioRunningGrapplingSkill::CheckKeyInput( ioBaseChar *pOwner )
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
			D3DXVECTOR3 vDir = pOwner->GetMoveDir();
			D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();

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
				kPacket << SSC_ROTATE;
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
				kPacket << SSC_ROTATE;
				kPacket << (int)m_CurDirKey;
				kPacket << m_bLeftRot;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

void ioRunningGrapplingSkill::ProcessRotate( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( m_dwRunRotateTime <= 0 ) return;
	if( m_bTargetRot ) return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwRunRotateTime;
	float fRotateAngle = 360.0f * fRate;

	// 목표에 도달체크 및 처리
	float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();

	float fCurYaw, fTargetYaw;
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

	fCurYaw = ioMath::ArrangeHead( fCurYaw );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

	float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
	if( fabs( fRotAngle ) < fRotateAngle )
	{
		m_bTargetRot = true;
		pOwner->SetTargetRotToRotate( qtRotate, true, false );
		return;
	}

	if( m_bLeftRot )
		fRotateAngle *= -1;

	fRotateAngle = ioMath::ArrangeHead( fRotateAngle );
	D3DXVECTOR3 vDir = pOwner->GetMoveDir();

	D3DXQUATERNION qtAngle;
	D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );

	vDir = qtAngle * vDir;
	ioMath::CalcDirectionQuaternion( qtAngle, -vDir );

	pOwner->SetTargetRotToRotate( qtAngle, true, false );
}

bool ioRunningGrapplingSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioRunningGrapplingSkill::PlayLoopSound( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	m_LoopSoundID = g_SoundMgr.PlaySound( m_LoopSound, pGrp->GetParentSceneNode(), PSM_LOOP | PSM_THREAD );
}

void ioRunningGrapplingSkill::StopLoopSound()
{
	g_SoundMgr.StopSound( m_LoopSound, m_LoopSoundID );
	//g_SoundMgr.StopSound( m_LoopSound, 0 );
}

bool ioRunningGrapplingSkill::IsCanSendControl() const
{
	switch( m_SkillState )
	{
	case SS_RUN:
		return false;
	}

	return true;
}

void ioRunningGrapplingSkill::SetCancelSkill( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	if( !pStage )
		return;

	ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
	if( pTarget )
	{
		pTarget->CheckSkillGrapplingCancelBuff( "", false );
	}
}

