#include "StdAfx.h"
#include "ioTargetMoveComboAttackSkill.h"
#include "ioPlayStage.h"
#include "ioNpcChar.h"
#include "WeaponDefine.h"

ioTargetMoveComboAttackSkill::ioTargetMoveComboAttackSkill(void)
{
	ClearData();
}

ioTargetMoveComboAttackSkill::ioTargetMoveComboAttackSkill( const ioTargetMoveComboAttackSkill &rhs ) :
ioAttackSkill( rhs ),
	//move state
m_fRunSpeed( rhs.m_fRunSpeed ),
m_fMaxMoveDist( rhs.m_fMaxMoveDist ),
	//next action state
m_NextActionAnimation( rhs.m_NextActionAnimation ),
m_fNextActionAniRate( rhs.m_fNextActionAniRate ),
	//linear state
m_ComboList( rhs.m_ComboList ),
m_bUseLinearAutoTargetValue( rhs.m_bUseLinearAutoTargetValue ),
m_fLinearAutoTargetAngle( rhs.m_fLinearAutoTargetAngle ),
m_fLinearAutoTargetMinRange( rhs.m_fLinearAutoTargetMinRange ),
m_fLinearAutoTargetMaxRange( rhs.m_fLinearAutoTargetMaxRange ),
m_bLinearAutoTargetEnableDownAim( rhs.m_bLinearAutoTargetEnableDownAim ),
	//grapple state
m_GrappleAttribute( rhs.m_GrappleAttribute ),
m_GrapplingAnimation( rhs.m_GrapplingAnimation ),
m_fGrapplingAniRate( rhs.m_fGrapplingAniRate ),
m_LandAttackAttribute( rhs.m_LandAttackAttribute ),
m_fCharHeightRate( rhs.m_fCharHeightRate ),
m_fTargetGapRange( rhs.m_fTargetGapRange ),
m_fGrapplingJumpPower( rhs.m_fGrapplingJumpPower ),
	//end state
m_EndAttackAttribute( rhs.m_EndAttackAttribute ),
	//protect time
m_dwSkillProtectTime( rhs.m_dwSkillProtectTime )
{
	ClearData();
}

ioTargetMoveComboAttackSkill::~ioTargetMoveComboAttackSkill(void)
{
}

void ioTargetMoveComboAttackSkill::ClearData()
{
	m_SkillState = SS_NONE;

	m_fCurMoveDist = 0.0f;
	m_iCurLinearCombo = 0;
	m_dwEnableReserveTime = 0;
	m_GrapplingTargetName.Clear();
	m_dwSkillProtectStartTime = 0;
	m_SkillEndState = SES_NONE;
	m_iLastAttack = 0;
	m_dwMoveAttackWeaponIndex = 0;
	m_bIsJumpPower = false;
	m_bSetHandMesh = false;
}

ioSkill* ioTargetMoveComboAttackSkill::Clone()
{
	return new ioTargetMoveComboAttackSkill( *this );
}

void ioTargetMoveComboAttackSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	//speed
	m_fRunSpeed = rkLoader.LoadFloat_e( "move_speed", 0.0f );
	m_fMaxMoveDist = rkLoader.LoadFloat_e( "max_range", 0.0f );

	//next action motion
	rkLoader.LoadString_e( "next_action_motion", "", szBuf, MAX_PATH );	
	m_NextActionAnimation = szBuf;
	m_fNextActionAniRate = rkLoader.LoadFloat_e( "next_action_ani_rate", 0.0f );
	
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
	m_bUseLinearAutoTargetValue = rkLoader.LoadBool_e( "use_linear_auto_target_value", false );
	m_fLinearAutoTargetAngle = rkLoader.LoadFloat_e( "linear_auto_target_angle", 0.0f );
	m_fLinearAutoTargetMinRange = rkLoader.LoadFloat_e( "linear_auto_target_min_range", 0.0f );
	m_fLinearAutoTargetMaxRange = rkLoader.LoadFloat_e( "linear_auto_target_max_range", 0.0f );
	m_bLinearAutoTargetEnableDownAim = rkLoader.LoadBool_e( "linear_auto_target_enable_down_target", false );

	//grapple
	m_GrappleAttribute.Init();
	LoadAttackAttribute_e( "grapple_attack", m_GrappleAttribute, rkLoader );
	rkLoader.LoadString_e( "grappling_motion", "", szBuf, MAX_PATH );	
	m_GrapplingAnimation = szBuf;
	m_fGrapplingAniRate = rkLoader.LoadFloat_e( "grappling_motion_rate", 0.0f );
	m_LandAttackAttribute.Init();
	LoadAttackAttribute_e( "land_attack", m_LandAttackAttribute, rkLoader );
	m_fCharHeightRate = rkLoader.LoadFloat_e( "char_height_rate", FLOAT05 );
	m_fTargetGapRange = rkLoader.LoadFloat_e( "skill_target_gap_range", 0.0f );
	m_fGrapplingJumpPower = rkLoader.LoadFloat_e( "grappling_jump_power", 0.0f );

	//end
	m_EndAttackAttribute.Init();
	LoadAttackAttribute_e( "skill_end_attack", m_EndAttackAttribute, rkLoader );

	//protect time
	m_dwSkillProtectTime = rkLoader.LoadInt_e( "skill_protect_time", 0 );
}

bool ioTargetMoveComboAttackSkill::OnSkillStart( ioBaseChar *pChar )
{
	if ( !pChar )
		return false;

	ioAttackSkill::OnSkillStart( pChar );

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( false );

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	SetCameraBuff( pChar );

	if( !m_bReduceGauge )
	{
		float fCurGauge = pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	ClearData();

	// m_GrappleAttribute가 있다면
	if ( !m_GrappleAttribute.m_AttackAnimation.IsEmpty() )
		m_iLastAttack++;
	return true;
}

void ioTargetMoveComboAttackSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	// 스킬 중간 동작에 사용된 메쉬들 제거
	m_bSetHandMesh = false;
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	EndCameraBuff( pChar );

	ClearData();
}

bool ioTargetMoveComboAttackSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if ( m_SkillState == SS_END && m_dwMotionEndTime < FRAMEGETTIME() )
		return true;
	return false;
}

void ioTargetMoveComboAttackSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar || !pStage )
		return;

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
				SetMoveAttackState( pChar, pStage );
			}
		}
		break;
	case SS_MOVEATTACK:
		{
			if ( m_dwMotionEndTime < dwCurTime )
				SetEndState( pChar, SES_ATTACK );

			if( !CheckMove( pChar ) )
				SetEndState( pChar, SES_ATTACK );

			if ( m_fCurMoveDist >= m_fMaxMoveDist )
				SetEndState( pChar, SES_ATTACK );
		}
		break;
	case SS_NEXTACTION:
		{
			if ( m_dwMotionEndTime < dwCurTime )
				SetEndState( pChar, SES_NONE );

			if ( m_dwEnableReserveTime > 0 && m_dwEnableReserveTime < dwCurTime )
			{
				if( pChar->IsNeedProcess() )
				{
					if( pChar->IsAttackKey() )
					{
						SetNextLinearCombo( pChar );
						return;
					}
				}
			}
			CheckExtraAniJump( pChar );
		}
		break;
	case SS_LINEAR:
		{
			if( m_dwMotionEndTime < dwCurTime )
				SetEndState( pChar, SES_NONE );

			CheckLinearCombo( pChar );
		}
		break;
	case SS_GRAPPLING:
		{
			// Target 유효 체크
			ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
			if( pTarget )
			{
				if( m_dwMotionStartTime + g_App.GetGrapplingCheckDelay() < dwCurTime &&
					pTarget->GetState() != CS_GRAPPLING_WOUNDED_SKILL )
				{
					SetEndState( pChar, SES_NONE );
					return;
				}
			}
			else
			{
				SetEndState( pChar, SES_NONE );
				return;
			}

			//motion 시간 체크
			if( m_dwMotionEndTime < dwCurTime )
			{
				SetEndState( pChar, SES_NONE );
				return;
			}
			
			//땅에 착지 체크
			if ( m_bIsJumpPower || m_fGrapplingJumpPower )
			{
				if( CheckLanding( pChar ) )
				{
					SetEndState( pChar, SES_LAND_ATTACK );
					return;
				}
			}
		}
		break;
	}
}

void ioTargetMoveComboAttackSkill::SetNextSkillState( ioBaseChar *pWounder, ioWeapon *pWeapon )
{
	if ( !pWounder || !pWeapon )
		return;
	ioBaseChar *pAttacker = pWeapon->GetOwner();
	if ( !pAttacker )
		return;

	if( pWounder->IsNeedProcess() )
	{
		if( !pWounder->HasBuff( BT_POSITION2 ) )
		{
			ioItem *pItem = GetOwnerItem();
			if ( !pItem || pItem->GetOwnerName().IsEmpty())
				return;

			SetEndState( pAttacker, SES_ATTACK );
			if ( pWounder->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pItem->GetOwnerName();
				kPacket << GetName();
				kPacket << (int)SS_END;
				kPacket << (int)SES_ATTACK;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return;
		}

		ioItem *pItem = GetOwnerItem();
		if ( !pItem || pItem->GetOwnerName().IsEmpty())
			return;

		SetNextActionState( pAttacker );

		if ( pWounder->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pItem->GetOwnerName();
			kPacket << GetName();
			kPacket << (int)SS_NEXTACTION;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	
}

void ioTargetMoveComboAttackSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
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
	ioAdhesiveMissileWeapon3 *pAdhesive3 = ToAdhesiveMissileWeapon3( pWeapon );
	ioAdhesiveMissileWeapon4 *pAdhesive4 = ToAdhesiveMissileWeapon4( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{				
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( true, true );
		}		
	}
	/*
	else if( pAdhesive3 )
	{
		pAdhesive3->CheckOwnerAttach( true );
	}
	else if( pAdhesive4 )
	{
		pAdhesive4->SetState( ioAdhesiveMissileWeapon4::WS_ATTACH_MOVE );
	}
	*/
}

void ioTargetMoveComboAttackSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar || !pStage ) return;
	int iState;
	rkPacket >> iState;
	switch( iState )
	{
	case SS_NEXTACTION:
		SetNextActionState( pChar );
		break;
	case SS_LINEAR:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			rkPacket >> m_iCurLinearCombo;
			SetNextLinearCombo( pChar );
			pChar->SetTargetRotToRotate( qtRot, true, false );
		}
		break;
	case SS_GRAPPLING:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pChar->SetTargetRotToRotate( qtRot, true, false );

			ioHashString szTargetName;
			rkPacket >> m_GrapplingTargetName;

			ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
			SetGrapplingAttackState( pChar );
			if( pTarget && pTarget->IsNeedProcess() )
			{
				pTarget->SetTargetRotToRotate( qtRot, true, false );
				pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
			}
			
		}
		break;
	case SS_END:
		{
			int iendstate;
			rkPacket >> iendstate;
			SetEndState( pChar, (SkillEndState)iendstate );
		}
	}
}


bool ioTargetMoveComboAttackSkill::IsNoDropState() const
{
	if( m_SkillState == SS_LINEAR )
		return true;

	return false;
}
bool ioTargetMoveComboAttackSkill::SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName )
{
	if ( !pChar) return false;
	if ( m_SkillState != SS_LINEAR )	return false;
	m_GrapplingTargetName = szTargetName;

	ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
	if( !pTarget ) return false;
	SetGrapplingAttackState( pChar );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_GRAPPLING;
		kPacket << pChar->GetTargetRot();
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
	if( (!P2PNetwork::IsNetworkPlaying() || !pChar->GetCreator()->IsNetworkMode() || (bNPC || bAttackerNPC)) )
	{
		pTarget->SetTargetRotToRotate( pChar->GetTargetRot(), true, false );
		pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
	}

	return true;
}

D3DXVECTOR3 ioTargetMoveComboAttackSkill::GetGrapplingTargetPos( ioBaseChar *pChar )
{
	if( m_fTargetGapRange > 0.0f )
	{
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( m_fCharHeightRate );
		D3DXVECTOR3 vDir = pChar->GetMoveDir();

		if( m_fTargetGapRange > 0.0f )
			vPos += m_fTargetGapRange * vDir;

		return vPos;
	}

	return ioMath::VEC3_ZERO;
}


bool ioTargetMoveComboAttackSkill::IsProtected( int iDefenseBreakType ) const
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

	if ( m_dwSkillProtectTime > 0 && m_dwSkillProtectStartTime + m_dwSkillProtectTime > FRAMEGETTIME() )
		return true;

	return false;
}

AttackAttribute& ioTargetMoveComboAttackSkill::GetAttribute()
{
	static AttackAttribute kAttack;
	kAttack.Init();
	if ( m_SkillState == SS_END )
	{
		if ( m_SkillEndState == SES_NONE )
			return kAttack;
		else if ( m_SkillEndState == SES_ATTACK )
			return m_EndAttackAttribute;
		else if ( m_SkillEndState == SES_LAND_ATTACK )
			return m_LandAttackAttribute;
	}
	else if ( m_SkillState == SS_MOVEATTACK )
		return m_AttackAttribute;
	else if( m_SkillState == SS_LINEAR )
	{
		int iMaxSize = static_cast<int>( m_ComboList.size() ) + m_iLastAttack;
		if( COMPARE( m_iCurLinearCombo, 0,  iMaxSize ) )
		{
			if (COMPARE( m_iCurLinearCombo, 0,  static_cast<int>( m_ComboList.size() ) ) )
				return m_ComboList[m_iCurLinearCombo];
			else
				return m_GrappleAttribute;
		}
	}
	return kAttack;
}

const AttackAttribute& ioTargetMoveComboAttackSkill::GetAttributeConst() const
{
	static const AttackAttribute kConstAttack;
	if ( m_SkillState == SS_END )
	{
		if ( m_SkillEndState == SES_NONE )
			return kConstAttack;
		else if ( m_SkillEndState == SES_ATTACK )
			return m_EndAttackAttribute;
		else if ( m_SkillEndState == SES_LAND_ATTACK )
			return m_LandAttackAttribute;
	}
	else if ( m_SkillState == SS_MOVEATTACK )
		return m_AttackAttribute;
	else if( m_SkillState == SS_LINEAR )
	{
		int iMaxSize = static_cast<int>( m_ComboList.size() ) + m_iLastAttack;
		if( COMPARE( m_iCurLinearCombo, 0,  iMaxSize ) )
		{
			if (COMPARE( m_iCurLinearCombo, 0,  static_cast<int>( m_ComboList.size() ) ) )
				return m_ComboList[m_iCurLinearCombo];
			else
				return m_GrappleAttribute;
		}
	}
	return kConstAttack;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//추가한 함수
//move attack state
void ioTargetMoveComboAttackSkill::SetMoveAttackState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar || !pStage )
		return;
	
	m_dwMoveAttackWeaponIndex = m_dwWeaponBaseIndex;
	m_SkillState = SS_MOVEATTACK;
	ioAttackSkill::AttackSkillFire( pChar );
	m_dwSkillProtectStartTime = FRAMEGETTIME();
	
}

bool ioTargetMoveComboAttackSkill::CheckMove( ioBaseChar *pChar )
{
	if( !pChar ) return false;

	ioPlayStage *pStage = pChar->GetCreator();
	if( !pStage ) return false;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;

	D3DXVECTOR3 vDir;
	vDir = pChar->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt;

		if( m_fRunSpeed > 0.0f && vDir != ioMath::VEC3_ZERO )
		{
			float fFinalMoveAmt = m_fRunSpeed * fDivideTimePerSec;
			if( fFinalMoveAmt > 0.0f )
			{
				vMoveAmt = vDir * fFinalMoveAmt;
				vTotalMove += vMoveAmt;
				bCurMove = true;
			}
		}

		if( pChar->ProcessForceMove( fDivideTimePerSec, vMoveAmt ) )
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
			float fPreHeight = pChar->GetBottomHeight();

			pChar->Translate( vTotalMove );

			D3DXVECTOR3 vDiff = ioMath::VEC3_ZERO;
			vDiff = vTotalMove - vDiff;
			float fMoveDist = D3DXVec3Length( &vDiff );
			m_fCurMoveDist += fMoveDist;

			if( pChar->GetForceAmt() != 0.0f )	// 빠른 ForceMove로 경사면을 통과하는 경우에 대한 보강
			{
				float fMoveHeight = pChar->GetBottomHeight();

				if( fPreHeight > 0.0f && fMoveHeight <= FLOAT1 )
				{
					D3DXVECTOR3 vCurPos = pChar->GetWorldPosition();

					float fTopHeight = pStage->GetMapHeight( vCurPos.x,
						vCurPos.z,
						pChar,
						false,
						pChar->GetMidHeightByRate( FLOAT1 ) );

					if( fTopHeight > FLOAT1 )
					{
						vCurPos.y = max( fTopHeight, fPreHeight );
						pChar->SetWorldPosition( vCurPos );
					}
				}
			}
		}
		if ( bCol )
			return false;
	}

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	float fPreHeight = pChar->GetBottomHeight();

	if( vPos.y < fPreHeight )
		vPos.y = fPreHeight;

	if( pStage && pChar->IsAxisLimit() )
	{
		pStage->CheckAxisLimit( &vPos );
		pChar->SetWorldPosition( vPos );
	}	

	return true;
}

void ioTargetMoveComboAttackSkill::DestroyMoveAttackWeapon( ioBaseChar *pChar )
{
	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwMoveAttackWeaponIndex );
	if( pWeapon )
		pWeapon->SetWeaponDead();
}

//next action state
void ioTargetMoveComboAttackSkill::SetNextActionState( ioBaseChar *pChar )
{
	m_SkillState = SS_NEXTACTION;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;
	int iAniID = 0;
	iAniID = pGrp->GetAnimationIdx( m_NextActionAnimation );
	if( iAniID == -1 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	float fAniRate = m_fNextActionAniRate;
	if( fAniRate <= 0.0f )
		fAniRate = FLOAT1;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fAniRate;
	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fAniRate;

	SetExtraAniJump( pChar, iAniID, fAniRate );
	if ( m_fExtraAniJumpPower > 0.0f )
		m_bIsJumpPower = true;
	DestroyMoveAttackWeapon( pChar );
	
}

//linear combo state
void ioTargetMoveComboAttackSkill::SetNextLinearCombo( ioBaseChar *pChar )
{
	m_SkillState = SS_LINEAR;

	if( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if ( !pGrp )
		return;

	int iMaxSize = static_cast<int>( m_ComboList.size() ) + m_iLastAttack;
	if ( m_iCurLinearCombo == 4 )
		int k = 10;
	if( !COMPARE( m_iCurLinearCombo, 0,  iMaxSize ) )
		return;
	pChar->InitActionStopDelay( true );

	if( m_iCurLinearCombo == 0 )
		m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	ioAttackSkill::AttackSkillFire( pChar );
	if ( pChar->IsNeedProcess() )
	{
		if ( m_bUseLinearAutoTargetValue )
			SearchTargetTracking( pChar );
	}

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_LINEAR;
		kPacket << pChar->GetTargetRot();
		kPacket << m_iCurLinearCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_iCurLinearCombo++;
}


void ioTargetMoveComboAttackSkill::SearchTargetTracking( ioBaseChar *pChar )
{
	if ( !pChar )
		return;
	ioHashString szTarget;
	if( pChar->CheckAimedTarget( m_fLinearAutoTargetAngle, m_fLinearAutoTargetMinRange, m_fLinearAutoTargetMaxRange, szTarget, m_bLinearAutoTargetEnableDownAim ) )
	{
		if( !szTarget.IsEmpty() )
		{
			ioBaseChar *pTarget = pChar->GetBaseChar( szTarget );
			bool bTarget = true;
			if( pTarget && !pTarget->IsEnableAimTarget(false) )
				bTarget = false;
			else if( !pTarget)
				bTarget = false;

			if( bTarget )
			{
				D3DXVECTOR3 vAimedPos = pTarget->GetMidPositionByRate();
				pChar->SetTargetRotToTargetPos( vAimedPos, false, false);	
			}
		}
	}
}


void ioTargetMoveComboAttackSkill::CheckLinearCombo( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( !pChar->IsNeedProcess() )
		return;

	if( m_dwPreDelayStartTime != 0 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	
	if( m_dwEnableReserveTime > 0 && m_dwEnableReserveTime < dwCurTime )
	{
		if( pChar->IsAttackKey() )
			SetNextLinearCombo( pChar );
	}
}

//grappling state
void ioTargetMoveComboAttackSkill::SetGrapplingAttackState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	m_SkillState = SS_GRAPPLING;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;
	
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = m_fGrapplingAniRate;
	if( fAniRate <= 0.0f )
		fAniRate = FLOAT1;

	int iAniID = pGrp->GetAnimationIdx( m_GrapplingAnimation );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );
	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fAniRate;

	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetJumpPower( m_fGrapplingJumpPower );
}

bool ioTargetMoveComboAttackSkill::CheckLanding( ioBaseChar *pChar )
{
	if( !pChar ) return true;

	DWORD dwCurTime = FRAMEGETTIME();

	float fHeightGap = 0.0f;
	if( pChar->CheckGhostState() )
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetPreHeightForGhostState();
	else
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetBottomHeight();

	if( fHeightGap > 0.0f || !pChar->IsDownState() )
		return false;

	return true;
}

//end state
void ioTargetMoveComboAttackSkill::SetEndState( ioBaseChar *pChar, SkillEndState endstate  )
{
	if( !pChar )
		return;

	m_SkillState = SS_END;
	m_SkillEndState = endstate;
	ioAttackSkill::AttackSkillFire( pChar );
	if ( m_SkillEndState == SES_NONE )
	{
		m_dwAttackEndTime = m_dwMotionEndTime = FRAMEGETTIME();
		m_dwMotionEndTime += 100;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////