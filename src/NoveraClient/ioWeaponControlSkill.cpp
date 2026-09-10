
#include "stdafx.h"

#include "ioWeaponControlSkill.h"


#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "WeaponDefine.h"

ioWeaponControlSkill::ioWeaponControlSkill()
{
	m_ControlState = RS_NONE;

	m_dwLoopStartTime = 0;
	m_bSetHandMesh = false;

	m_dwFireStartTime = 0;
	m_dwEndAniEndTime = 0;
}

ioWeaponControlSkill::ioWeaponControlSkill( const ioWeaponControlSkill &rhs )
: ioNormalSkill( rhs ),
 m_AttackAttribute( rhs.m_AttackAttribute ),
 m_LoopAnimation( rhs.m_LoopAnimation ),
 m_dwLoopTime( rhs.m_dwLoopTime ),
 m_StartHandMesh( rhs.m_StartHandMesh ),
 m_LoopHandMesh( rhs.m_LoopHandMesh ),
 m_EndHandMesh( rhs.m_EndHandMesh ),
 m_dwLoopProtectDuration( rhs.m_dwLoopProtectDuration ),
 m_EndAnimation( rhs.m_EndAnimation ),
 m_fEndAniRate( rhs.m_fEndAniRate ),
 m_dwTargetOffset( rhs.m_dwTargetOffset ),
 m_dwCreateOffset( rhs.m_dwCreateOffset ),
 m_iStopSkillKey( rhs.m_iStopSkillKey )
{
	m_ControlState = RS_NONE;

	m_dwLoopStartTime = 0;
	m_bSetHandMesh = false;

	m_dwFireStartTime = 0;
	m_dwEndAniEndTime = 0;

	m_szTargetName.Clear();
	m_szControlBuffName.Clear();
}

ioWeaponControlSkill::~ioWeaponControlSkill()
{
}

void ioWeaponControlSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	// Animation
	rkLoader.LoadString_e( "loop_animation", "", szBuf, MAX_PATH );
	m_LoopAnimation = szBuf;
	m_dwLoopTime = rkLoader.LoadInt_e( "loop_duration", 0 );
	m_dwLoopProtectDuration = rkLoader.LoadInt_e( "loop_protect_duration", 0 );

	rkLoader.LoadString_e( "end_animation", "", szBuf, MAX_PATH );
	m_EndAnimation = szBuf;
	m_fEndAniRate = rkLoader.LoadFloat_e( "end_ani_rate", FLOAT1 );

	LoadAttackAttribute_e( "control_attack", m_AttackAttribute, rkLoader );

	// HandMesh
	rkLoader.LoadString_e( "start_hand_mesh", "", szBuf, MAX_PATH );
	m_StartHandMesh = szBuf;
	rkLoader.LoadString_e( "loop_hand_mesh", "", szBuf, MAX_PATH );
	m_LoopHandMesh = szBuf;
	rkLoader.LoadString_e( "end_hand_mesh", "", szBuf, MAX_PATH );
	m_EndHandMesh = szBuf;

	m_dwTargetOffset = rkLoader.LoadInt_e( "target_offset", 0 );
	m_dwCreateOffset = rkLoader.LoadInt_e( "create_offset", 0 );
	m_iStopSkillKey = rkLoader.LoadInt_e( "stop_skill_key", -1 );
}

ioSkill* ioWeaponControlSkill::Clone()
{
	return new ioWeaponControlSkill( *this );
}

bool ioWeaponControlSkill::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{
		if( iDefenseBreakType == m_ProtectCancelType )   // 스킬 무적 파괴
			return false;
	}

	if( IsPreDelayProtect() )
	{
		if( m_dwPreDelayStartTime != 0 )
			return true;
	}

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_ControlState == RS_ACTION )
		return true;

	if( m_ControlState == RS_LOOP )
	{
		if( m_dwLoopProtectDuration == 0 )
			return true;

		if( m_dwProtectTime > dwCurTime )
			return true;
	}
	
	return false;
}

bool ioWeaponControlSkill::IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg )
{
	ioAttackableItem *pAttackable = ToAttackableItem( GetOwnerItem() );
	if( pAttackable )
	{
		if( !pAttackable->IsCanFire( pChar ) )
			return false;
	}

	if( !CheckNeedGauge( pChar, bHelpMsg ) )
		return false;

	return true;
}

bool ioWeaponControlSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_ControlState != RS_END )
		return false;

	if( m_dwEndAniEndTime > 0 && m_dwEndAniEndTime < FRAMEGETTIME() )
		return true;

	return false;
}

bool ioWeaponControlSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !HasSkillMotion() )
		return false;
	if( !pChar )
		return false;

	pChar->SetSkillProtection();

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	m_ControlState = RS_NONE;
	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	m_dwMotionStartTime = 0;
	m_dwLoopStartTime = 0;
	m_dwFireStartTime = 0;
	m_dwEndAniEndTime = 0;

	m_vCreatePos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	m_dwProtectTime = 0;
	m_bSetHandMesh = false;

	m_bReduceGauge = false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	DWORD dwPreDelayTime = GetPreDelayTime();
	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		m_dwPreDelayStartTime = 0;
		EndPreDelayEffect( pChar );
		SetActionState( pChar );
	}
	else
	{
		DWORD dwTrackingTime = 0;
		ioEntityGroup *pGrp = pChar->GetGroup();
		if( pGrp )
		{
			ioHashString szPreDelay = GetPreDelayAniName();

			int iAniID = pGrp->GetAnimationIdx( szPreDelay );
			if( iAniID != -1 )
				dwTrackingTime = pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" );
		}

		DWORD dwPreDelay = GetPreDelayTime();

		if( dwTrackingTime > 0 )
			dwTrackingTime += FRAMEGETTIME();
		else
			dwTrackingTime = FRAMEGETTIME() + dwPreDelay;

		if( IsUseAutoTargetValue() )
		{
			pChar->SetAutoTarget( ATT_SKILL );
			pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
	}

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	SetSkillNameChatBubble( pChar );

	return true;
}

void ioWeaponControlSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwPreDelayTime = GetPreDelayTime();

	switch( m_ControlState )
	{
	case RS_NONE:
		if( m_dwPreDelayStartTime > 0 && m_dwPreDelayStartTime + dwPreDelayTime < dwCurTime )
		{
			m_dwPreDelayStartTime = 0;
			EndPreDelayEffect( pChar );
			SetActionState( pChar );
		}
		break;
	case RS_ACTION:
		if( m_dwMotionStartTime > 0 && m_dwMotionEndTime < dwCurTime )
		{
			m_dwMotionStartTime = 0;
			SetLoopState( pChar, pStage );
		}
		
		// SetLoopState가 발생했을때도 한다
		if( m_dwFireStartTime > 0 && m_dwFireStartTime < dwCurTime )
		{
			m_dwFireStartTime = 0;

			m_vCreatePos = pChar->GetMidPositionByRate();
			m_vCreatePos += pChar->GetMoveDir() * m_dwCreateOffset;
			pChar->SkillFireExplicit( m_AttackAttribute.m_vWeaponInfoList.front(),
									  GetName(),
									  m_dwWeaponBaseIndex,
									  m_vCreatePos );
		}
		break;
	case RS_LOOP:
		{
			int iSkillNum = pChar->GetSkillKeyInput();
			if( iSkillNum >= 0 && m_iStopSkillKey == iSkillNum )
			{
				SetEndState( pChar );
				return;
			}
		}
		if( m_dwLoopStartTime > 0 && m_dwLoopStartTime+m_dwLoopTime < dwCurTime )
		{
			m_dwLoopStartTime = 0;
			SetEndState( pChar );
		}
		else if( !CheckControlWeapon( pChar ) )
		{
			m_dwLoopStartTime = 0;
			SetEndState( pChar );
		}
		else if( ( pChar->IsOwnerChar() || !P2PNetwork::IsNetworkPlaying() ) && !m_szTargetName.IsEmpty() )
		{
			if( pChar->IsAttackKey() )
			{
				CreateControlWeaponAgain( pChar );
			}
			else
			{
				ioBaseChar *pTarget = pChar->GetBaseChar( m_szTargetName );
				if( pTarget && !pTarget->IsJumpingState() )
				{
					int iTargetState = pTarget->GetState();
					if( iTargetState != CS_RUN && iTargetState != CS_DELAY )
						return;

					ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();
					bool bJump = pChar->IsJumpKey();
					SetControlMoveInfo( pChar, eNewDirKey, bJump );
				}
			}
		}
		break;
	case RS_END:
		break;
	}
}

void ioWeaponControlSkill::SetControlMoveInfo( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eKey, bool bJump )
{
	if( !pOwner )
		return;

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTargetName );
	if( !pTarget )
		return;

	bool bChange = false;
	if( m_PrevKey != eKey || bJump )
		bChange = true;

	if( eKey != ioUserKeyInput::DKI_NONE )
		pTarget->SetTargetRotToDirKey( eKey );

	int iState = CS_DELAY;
	if( bJump )
		iState = CS_JUMP;
	else if( eKey != ioUserKeyInput::DKI_NONE )
		iState = CS_RUN;

	m_PrevKey = eKey;
	pOwner->SetTargetControlState( iState );

	if( bChange && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << SS_MOVE;
		kPacket << (int)m_PrevKey;
		kPacket << bJump;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioWeaponControlSkill::OnSkillEnd( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pOwner->ShowWeaponItemMesh( true );
	}
	if( !m_szTargetName.IsEmpty() )
	{
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTargetName );
		m_szTargetName.Clear();
		if( pTarget )
			pTarget->RemoveBuff( m_szControlBuffName );
	}
	// 스킬 중간 동작에 사용된 메쉬들 제거
	m_bSetHandMesh = false;

	if( !m_StartHandMesh.IsEmpty() )
		pOwner->RestoreSkillWeaponMesh( this, m_StartHandMesh, m_bVisibleEquipMesh );

	if( !m_LoopHandMesh.IsEmpty() )
		pOwner->RestoreSkillWeaponMesh( this, m_LoopHandMesh, m_bVisibleEquipMesh );

	if( !m_EndHandMesh.IsEmpty() )
		pOwner->RestoreSkillWeaponMesh( this, m_EndHandMesh, m_bVisibleEquipMesh );

	// Ani
	m_dwMotionStartTime = 0;
	m_dwLoopStartTime = 0;
	m_dwEndAniEndTime = 0;

	ioNormalSkill::OnSkillEnd( pOwner );

	g_WeaponMgr.CheckSkillDependencyWeaponDead( pOwner->GetCharName(), GetName() );
}

void ioWeaponControlSkill::StartPreDelayEffect( ioBaseChar *pChar )
{
	ioSkill::StartPreDelayEffect( pChar );
}

void ioWeaponControlSkill::EndPreDelayEffect( ioBaseChar *pChar )
{
	ioSkill::EndPreDelayEffect( pChar );
}

void ioWeaponControlSkill::SetActionState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	m_dwMotionStartTime = FRAMEGETTIME();
	m_dwMotionEndTime = m_dwMotionStartTime;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	return;

	float fAniRate = GetSkillMotionRate();

	float fTimeRate = fAniRate * m_fExtraAniRate;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iCurSkillAniID )*fTimeRate;

	DWORD dwCreateTime = pGrp->GetAnimationEventTime_e( iCurSkillAniID, "fire_s" ) * fTimeRate;
	m_dwFireStartTime = m_dwMotionStartTime + dwCreateTime;

	m_ControlState = RS_ACTION;

	pChar->AttachEffect( m_SkillEffectName );

	// 사용 아이템 처리
	if( m_bUseItemMesh && !m_StartHandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_StartHandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
}

void ioWeaponControlSkill::SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )
		return;

	if( m_LoopAnimation.IsEmpty() )
	{
		m_dwLoopStartTime = 0;
		return;
	}

	if( m_bSetHandMesh )
	{
		if( !m_StartHandMesh.IsEmpty() )
		{
			pChar->RestoreSkillWeaponMesh( this, m_StartHandMesh, m_bVisibleEquipMesh );
		}
		m_bSetHandMesh = false;
	}

	// 사용 아이템 처리
	if( m_bUseItemMesh && !m_LoopHandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_LoopHandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( m_LoopAnimation, FLOAT100 );

	m_dwLoopStartTime = FRAMEGETTIME();
	m_dwProtectTime = m_dwLoopStartTime + m_dwLoopProtectDuration;

	m_ControlState = RS_LOOP;
}

void ioWeaponControlSkill::SetEndState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	m_ControlState = RS_END;
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwEndAniEndTime = dwCurTime;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	int iCurSkillAniID = pGrp->GetAnimationIdx( m_EndAnimation );
	if( iCurSkillAniID == -1 )	return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fEndAniRate );

	m_dwEndAniEndTime += pGrp->GetAnimationFullTime( iCurSkillAniID ) * m_fEndAniRate;

	if( m_bSetHandMesh )
	{
		if( !m_LoopHandMesh.IsEmpty() )
		{
			pChar->RestoreSkillWeaponMesh( this, m_LoopHandMesh, m_bVisibleEquipMesh );
		}
		m_bSetHandMesh = false;
	}

	// 사용 아이템 처리
	if( m_bUseItemMesh && !m_EndHandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_EndHandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	g_WeaponMgr.CheckSkillDependencyWeaponDead( pChar->GetCharName(), GetName() );
}

void ioWeaponControlSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar || !pStage )
		return;

	int iState;
	rkPacket >> iState;
	switch( iState )
	{
	case SS_SET:
		{
			ioHashString szTargetName, szBuffName;
			rkPacket >> szTargetName >> szBuffName;
			SetControlBuffInfo( pChar, szTargetName, szBuffName );
		}
		break;
	case SS_MOVE:
		{
			int iKey;
			bool bJump;
			rkPacket >> iKey >> bJump;
			SetControlMoveInfo( pChar, (ioUserKeyInput::DirKeyInput)iKey, bJump );
		}
		break;
	case SS_WEAPON:
		CreateControlWeaponAgain( pChar );
		break;
	case SS_END:
		SetEndState( pChar );
		break;
	}
}

const D3DXVECTOR3& ioWeaponControlSkill::GetWeaponPos( ioBaseChar *pChar )
{
	if( !pChar )
		return m_vCreatePos;

	if( m_dwWeaponBaseIndex < 0 )
		return pChar->GetWorldPosition();

	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwWeaponBaseIndex );
	if( ToBombMissileWeapon2( pWeapon ) || ToBombMissileWeapon4( pWeapon ) || ToBombMissileWeapon5( pWeapon ) )
	{
		return pWeapon->GetPosition();
	}
	if( !m_szTargetName.IsEmpty() )
	{
		ioBaseChar *pTarget = pChar->GetBaseChar( m_szTargetName );
		if( pTarget )
			return pTarget->GetWorldPosition();
	}
	return m_vCreatePos;
}

bool ioWeaponControlSkill::CheckControlWeapon( ioBaseChar *pChar )
{
	if( !pChar->IsOwnerChar() && P2PNetwork::IsNetworkPlaying() )
		return true;

	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwWeaponBaseIndex );
	if( ToBombMissileWeapon2( pWeapon ) || ToBombMissileWeapon4( pWeapon ) || ToBombMissileWeapon5( pWeapon ) || !m_szTargetName.IsEmpty() )
		return true;

	return false;
}

bool ioWeaponControlSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

bool ioWeaponControlSkill::IsWeaponControl( ioBaseChar *pChar )
{
	switch( m_ControlState )
	{
	case RS_LOOP:
	case RS_END:
		if( CheckControlWeapon( pChar ) )
			return true;
		break;
	}

	return false;
}

bool ioWeaponControlSkill::UseActiveCount( ioBaseChar *pOwner )
{
	if( m_ControlState != RS_LOOP )
		return false;

	if( !pOwner )
		return false;

	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwWeaponBaseIndex );
	ioBombMissileWeapon4 *pBombM4 = ToBombMissileWeapon4( pWeapon );
	if( pBombM4 && pBombM4->GetMaxExplosionCnt() > 0 )
		return true;

	ioBombMissileWeapon5 *pBombM5 = ToBombMissileWeapon5( pWeapon );
	if( pBombM5 )
		return false;

	return false;
}

int ioWeaponControlSkill::GetMaxActiveCount( ioBaseChar *pOwner )
{
	if( m_ControlState != RS_LOOP )
		return 0;

	if( !pOwner )
		return 0;

	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwWeaponBaseIndex );
	ioBombMissileWeapon4 *pBombM4 = ToBombMissileWeapon4( pWeapon );
	if( pBombM4 )
		return pBombM4->GetMaxExplosionCnt();

	ioBombMissileWeapon5 *pBombM5 = ToBombMissileWeapon5( pWeapon );
	if( pBombM5 )
		return pBombM5->GetMaxExplosionCnt();

	return 0;
}

int ioWeaponControlSkill::GetCurActiveCount( ioBaseChar *pOwner )
{
	if( m_ControlState != RS_LOOP )
		return 0;

	if( !pOwner )
		return 0;

	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwWeaponBaseIndex );
	ioBombMissileWeapon4 *pBombM4 = ToBombMissileWeapon4( pWeapon );
	if( pBombM4 )
		return pBombM4->GetCurExplosionCnt();

	ioBombMissileWeapon5 *pBombM5 = ToBombMissileWeapon5( pWeapon );
	if( pBombM5 )
		return pBombM5->GetCurExplosionCnt();

	return 0;
}

bool ioWeaponControlSkill::SetControlBuffInfo( ioBaseChar *pOwner, ioHashString szTargetName, ioHashString szControlBuffName )
{
	if( m_ControlState != RS_LOOP )
		return false;

	m_szTargetName = szTargetName;
	m_szControlBuffName = szControlBuffName;
	m_PrevKey = ioUserKeyInput::DKI_NONE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << SS_SET;
		kPacket << m_szTargetName;
		kPacket << m_szControlBuffName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	return true;
}

void ioWeaponControlSkill::ClearControlBuffInfo( ioBaseChar *pOwner )
{
	if( m_szTargetName.IsEmpty() )
		return;

	m_szTargetName.Clear();
	m_szControlBuffName.Clear();
	m_PrevKey = ioUserKeyInput::DKI_NONE;

	SetEndState( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << SS_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioWeaponControlSkill::CreateControlWeaponAgain( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_vCreatePos = pOwner->GetMidPositionByRate();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTargetName );
	if( pTarget )
	{
		m_vCreatePos = pTarget->GetMidPositionByRate();
		m_vCreatePos -= pTarget->GetMoveDir() * m_dwTargetOffset;

		m_szTargetName.Clear();
		pTarget->RemoveBuff( m_szControlBuffName );
	}
	m_dwWeaponBaseIndex++;
	pOwner->SkillFireExplicit( m_AttackAttribute.m_vWeaponInfoList.front(),
								GetName(),
								m_dwWeaponBaseIndex,
								m_vCreatePos );

	m_szTargetName.Clear();
	m_szControlBuffName.Clear();
	m_PrevKey = ioUserKeyInput::DKI_NONE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << SS_WEAPON;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}