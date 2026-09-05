#include "StdAfx.h"
#include "ioMoveAttackSkill4.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"

ioMoveAttackSkill4::ioMoveAttackSkill4(void)
{
	ClearData();
}

ioMoveAttackSkill4::ioMoveAttackSkill4( const ioMoveAttackSkill4 &rhs ):
ioAttackSkill(rhs),
m_bDisableGrowth( rhs.m_bDisableGrowth ),
m_fGuagePerTic( rhs.m_fGuagePerTic ),
m_dwTicTime( rhs.m_dwTicTime ),
m_szDelayAni( rhs.m_szDelayAni ),
m_fDelayAniRate( rhs.m_fDelayAniRate ),
m_szRunAni( rhs.m_szRunAni ),
m_fRunAniRate( rhs.m_fRunAniRate ),
m_fJumpPower( rhs.m_fJumpPower ),
m_fJumpNormalGravityRate( rhs.m_fJumpNormalGravityRate ),
m_fJumpFreeGravityRate( rhs.m_fJumpFreeGravityRate ),
m_szJumpAni( rhs.m_szJumpAni ),
m_fJumpAniRate( rhs.m_fJumpAniRate ),
m_fSkillJumpMoveSpeedRate( rhs.m_fSkillJumpMoveSpeedRate ),
m_szLandingAni( rhs.m_szLandingAni ),
m_fLandingAniRate( rhs.m_fLandingAniRate ),
m_vWeaponOffset( rhs.m_vWeaponOffset ),
m_szCircleEffect( rhs.m_szCircleEffect ),
m_vCircleOffset( rhs.m_vCircleOffset )
{
	ClearData();
}

ioMoveAttackSkill4::~ioMoveAttackSkill4(void)
{
}

ioSkill* ioMoveAttackSkill4::Clone()
{
	return new ioMoveAttackSkill4(*this);
}

void ioMoveAttackSkill4::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );
	char szBuf[MAX_PATH];
	
	//Delay Run
	rkLoader.LoadString_e( "delay_animation", "", szBuf, MAX_PATH );
	m_szDelayAni = szBuf;
	m_fDelayAniRate = rkLoader.LoadFloat_e( "delay_animation_rate", FLOAT1 );

	rkLoader.LoadString_e( "run_animation", "", szBuf, MAX_PATH );
	m_szRunAni = szBuf;
	m_fRunAniRate = rkLoader.LoadFloat_e( "run_animation_rate", FLOAT1 );

	//Gauge
	m_bDisableGrowth = rkLoader.LoadBool_e( "disable_growth", false );
	m_fGuagePerTic = rkLoader.LoadFloat_e( "gauge_per_tick", 0.0f );
	m_dwTicTime = (DWORD)rkLoader.LoadInt_e( "tick_time", 0 );

	//Jump
	m_fJumpPower = rkLoader.LoadFloat_e( "jump_power", 0.0f );
	rkLoader.LoadString_e( "jump_animation", "", szBuf, MAX_PATH );
	m_szJumpAni = szBuf;
	m_fJumpAniRate = rkLoader.LoadFloat_e( "jump_animation_rate", FLOAT1 );
	m_fJumpNormalGravityRate = rkLoader.LoadFloat_e( "jump_normal_gravity_rate", 1.0f );
	m_fJumpFreeGravityRate = rkLoader.LoadFloat_e( "jump_free_gravity_rate", 1.0f );
	m_fSkillJumpMoveSpeedRate = rkLoader.LoadFloat_e( "jump_move_speed_rate", 0.0f );

	//Landing
	rkLoader.LoadString_e( "landing_animation", "", szBuf, MAX_PATH );
	m_szLandingAni = szBuf;
	m_fLandingAniRate = rkLoader.LoadFloat_e( "landing_animation_rate", FLOAT1 );
	
	//Circle
	rkLoader.LoadString_e( "circle_effect", "", szBuf, MAX_PATH );
	m_szCircleEffect = szBuf;

	m_vCircleOffset.x = rkLoader.LoadFloat_e( "circle_offset_x", 0.f );
	m_vCircleOffset.y = rkLoader.LoadFloat_e( "circle_offset_y", 0.f );
	m_vCircleOffset.z = rkLoader.LoadFloat_e( "circle_offset_z", 0.f );

	m_vWeaponOffset.x = rkLoader.LoadFloat_e( "weapon_offset_x", 0.f );
	m_vWeaponOffset.y = rkLoader.LoadFloat_e( "weapon_offset_y", 0.f );
	m_vWeaponOffset.z = rkLoader.LoadFloat_e( "weapon_offset_z", 0.f );
}

void ioMoveAttackSkill4::ClearData()
{
	m_SkillState = SS_NONE;
	m_bDelayMove = false;
	m_dwCheckTime = 0;
	m_fTicGaugeRateByGrowth = FLOAT1;
	m_dwTicTimeByGrowth = 0;
	m_bChangeJumpGravity = false;
	m_dwCircleEffectID = -1;
	m_vCirclePos = ioMath::VEC3_ZERO;
	m_fCircleMapHeight = 0.0f;
	m_vWeaponMoveDir = ioMath::VEC3_ZERO;
}

bool ioMoveAttackSkill4::OnSkillStart( ioBaseChar *pOwner )
{
	if ( !ioAttackSkill::OnSkillStart( pOwner ) )
		return false;

	m_bSetHandMesh = false;
	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
		pOwner->ShowWeaponItemMesh( false );
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pOwner->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	ClearData();

	m_dwCheckTime = FRAMEGETTIME();
	// 성장관련 요소 추가.
	if( !m_bDisableGrowth )
	{
		ioPlayStage *pStage = pOwner->GetCreator();
		if( pStage )
		{
			ioGrowthNormalUpInfo *pUpInfo = NULL;

			pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_SKILL_COOL_TIME, pOwner->GetCharName()) );
			if( pUpInfo )
				m_fTicGaugeRateByGrowth = pUpInfo->GetValue(pStage);

			m_fTicGaugeRateByGrowth = min( m_fTicGaugeRateByGrowth, FLOAT100 );
			m_fTicGaugeRateByGrowth = FLOAT1 + ( m_fTicGaugeRateByGrowth/FLOAT100 );


			pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_BUFF_TIC_TIME, pOwner->GetCharName()) );
			if( pUpInfo )
				m_dwTicTimeByGrowth = pUpInfo->GetValue(pStage);			
		}
	}

	SetCameraBuff( pOwner );

	if( !m_bReduceGauge )
	{
		pOwner->ReduceNeedGaugeBySkill( this, GetNeedGauge(pOwner->GetCharName()) );
		m_bReduceGauge = true;
	}

	return true;
}

void ioMoveAttackSkill4::OnProcessState( ioBaseChar *pOwner, ioPlayStage *pStage )
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
				EndPreDelayEffect( pOwner );
				SetDelayState( pOwner, pStage, false );
			}
		}
		break;
	case SS_DELAY:
		{
			if ( pOwner->IsJumpKey() )
			{
				SetJumpState( pOwner );
				return;
			}
			else if ( pOwner->IsAttackKey() )
			{
				SetNormalAttackState( pOwner );
				return;
			}

			CheckMove( pOwner, false );
		}
		break;
	case SS_JUMP:
		{
			if ( pOwner->IsNeedProcess() )
			{
				if( CheckLanding( pOwner ) )
				{
					SetLandingState( pOwner );
					return;
				}

				if ( !m_bChangeJumpGravity && pOwner->IsJumpKey() )
				{
					m_bChangeJumpGravity = true;
					if ( pOwner->IsNeedSendNetwork() )
					{
						if ( pOwner->IsNeedSendNetwork() )
						{
							SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
							kPacket << pOwner->GetCharName();
							kPacket << GetName();
							kPacket << SST_JUMP_CHANGE;
							kPacket << pOwner->GetWorldPosition();
							kPacket << m_bChangeJumpGravity;
							P2PNetwork::SendToAllPlayingUser( kPacket );
						}
					}
				}

				if ( !m_bChangeJumpGravity && pOwner->IsAttackKey() )
				{
					SetJumpAttackState( pOwner );
					return;
				}
			}
			
		}
		break;
	case SS_LANDING:
		{
			if ( pOwner->IsNeedProcess() )
			{
				if ( m_dwMotionEndTime < dwCurTime )
				{
					SetDelayState( pOwner, pStage, true );
					return;
				}
			}
		}
		break;
	case SS_NORMAL_ATTACK:
		{
			if ( pOwner->IsNeedProcess() )
			{
				if ( m_dwMotionEndTime < dwCurTime )
				{
					SetDelayState( pOwner, pStage, true );
					return;
				}

				if ( m_dwAttackFireStartTime && m_dwAttackFireStartTime < dwCurTime )
				{
					m_dwAttackFireStartTime = 0;
					FireWeapon( pOwner );
				}
			}
		}
		break;
	case SS_JUMP_ATTACK:
		{
			if ( pOwner->IsNeedProcess() )
			{
				if( CheckLanding( pOwner ) )
				{
					SetLandingState( pOwner );
					return;
				}

				if ( m_dwMotionEndTime < dwCurTime )
				{
					m_SkillState = SS_JUMP;
					pOwner->SetSkillMoveEnable( true, false );

					if ( pOwner->IsNeedSendNetwork() )
					{
						SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
						kPacket << pOwner->GetCharName();
						kPacket << GetName();
						kPacket << SST_JUMP_ATTACK_END;
						kPacket << pOwner->GetWorldPosition();
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}
					return;
				}

				if ( m_dwAttackFireStartTime && m_dwAttackFireStartTime < dwCurTime )
				{
					m_dwAttackFireStartTime = 0;
					FireWeapon( pOwner );
				}
			}
		}
		break;
	}

	CheckCircle( pOwner, pStage );
	CheckGauge( pOwner );
}

void ioMoveAttackSkill4::SetDelayState( ioBaseChar *pOwner, ioPlayStage *pStage, bool bSend )
{
	if( !pOwner )
		return;

	m_SkillState = SS_DELAY;	

	pOwner->SetSkillMoveEnable( true, true );
	CheckMove( pOwner, true );

	if ( pOwner->IsNeedProcess() )
	{
		float fMapHeight = pOwner->GetBottomHeight();
		if( !m_szCircleEffect.IsEmpty() && m_dwCircleEffectID == -1 )
		{
			D3DXVECTOR3 vScale = D3DXVECTOR3(1.0f, FLOAT1, FLOAT1 );
			D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
			vPos += pOwner->GetWorldOrientation() * m_vCircleOffset;
			m_fCircleMapHeight = pStage->GetMapHeight( vPos.x, vPos.z );
			if ( m_fCircleMapHeight == 0.0f )
				vPos.y = m_fCircleMapHeight = fMapHeight;

			ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_szCircleEffect, vPos, vScale );
			if( pMapEffect )
			{
				pMapEffect->SetWorldOrientation( pOwner->GetWorldOrientation() );
				m_dwCircleEffectID = pMapEffect->GetUniqueID();
			}
			m_vCirclePos = vPos;
		}
	}

	if ( bSend && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << SST_DELAY;
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMoveAttackSkill4::CheckMove( ioBaseChar *pOwner, bool bFirst )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	if ( bFirst || ( !pOwner->IsSettedDirection() && m_bDelayMove ) )
	{
		m_bDelayMove = false;
		int iAniID = pGrp->GetAnimationIdx( m_szDelayAni );
		if( iAniID == -1 )		
			return;

		float fTimeRate = m_fDelayAniRate;
		if ( fTimeRate <= 0.0f )
			fTimeRate = 1.0f;
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );		
		pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	}
	else if( !m_bDelayMove && pOwner->IsSettedDirection() )
	{
		m_bDelayMove = true;
		int iAniID = pGrp->GetAnimationIdx( m_szRunAni );
		if( iAniID == -1 )		
			return;

		float fTimeRate = m_fRunAniRate;
		if ( fTimeRate <= 0.0f )
			fTimeRate = 1.0f;
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );		
		pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	}
}

void ioMoveAttackSkill4::CheckCircle( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( m_dwCircleEffectID != -1 )
	{
		ioMapEffect* pMapEffect = pStage->FindMapEffect( m_dwCircleEffectID );
		if ( pMapEffect )
		{
			float fMapHeight = m_fCircleMapHeight;
			D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
			vPos += pOwner->GetWorldOrientation() * m_vCircleOffset;
			vPos.y = pStage->GetMapHeight( vPos.x, vPos.z );
			if ( vPos.y == 0.0f )
				vPos.y = m_fCircleMapHeight;
			else
				m_fCircleMapHeight = vPos.y;

			pMapEffect->SetWorldPosition( vPos );
			m_vCirclePos = vPos;
		}
	}
}

void ioMoveAttackSkill4::SetJumpState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;
	m_bChangeJumpGravity = false;
	m_SkillState = SS_JUMP;
	pOwner->SetSkillMoveEnable( true, false );
	pOwner->SetJumpPower( m_fJumpPower );

	int iAniID = pGrp->GetAnimationIdx( m_szJumpAni );
	if( iAniID == -1 )		
		return;

	float fTimeRate = m_fJumpAniRate;
	if ( fTimeRate <= 0.0f )
		fTimeRate = 1.0f;
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );		
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT1, 1.0f, FLOAT1/fTimeRate );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << SST_JUMP;
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioMoveAttackSkill4::CheckLanding( ioBaseChar *pOwner )
{
	if( !pOwner )
		return true;

	float fHeightGap = 0.0f;
	if( pOwner->CheckGhostState() )
		fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetPreHeightForGhostState();
	else
		fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();

	if( fHeightGap > 0.0f || !pOwner->IsDownState() )
		return false;

	return true;
}

void ioMoveAttackSkill4::SetLandingState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;
	m_SkillState = SS_LANDING;
	pOwner->SetSkillMoveEnable( false, false );

	int iAniID = pGrp->GetAnimationIdx( m_szLandingAni );
	if( iAniID == -1 )		
		return;

	float fTimeRate = m_fLandingAniRate;
	if ( fTimeRate <= 0.0f )
		fTimeRate = 1.0f;
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );		
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT1, 1.0f, FLOAT1/fTimeRate );

	m_dwMotionEndTime = FRAMEGETTIME() + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << SST_LANDING;
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMoveAttackSkill4::SetNormalAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	m_bRefreshFireTimeList = false;
	m_SkillState = SS_NORMAL_ATTACK;
	AttackSkillFire( pOwner );
	pOwner->SetSkillMoveEnable( false, false );

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetCurMoveSpeed( 0.0f );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << SST_NORMAL_ATTACK;
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMoveAttackSkill4::SetJumpAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	m_bRefreshFireTimeList = false;
	m_SkillState = SS_JUMP_ATTACK;
	AttackSkillFire( pOwner );
	pOwner->SetSkillMoveEnable( false, false );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << SST_JUMP_ATTACK;
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMoveAttackSkill4::FireWeapon( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + pOwner->GetWorldOrientation() * m_vWeaponOffset;
	int iSize = GetAttribute().m_vWeaponInfoList.size();
	m_dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();
	m_vWeaponMoveDir = m_vCirclePos - vPos;
	D3DXVec3Normalize( &m_vWeaponMoveDir, &m_vWeaponMoveDir);
	for ( int i=0 ; i<iSize ; ++i )
	{
		pOwner->SkillFireExplicit( GetAttribute().m_vWeaponInfoList[i],
			GetName(),
			m_dwWeaponBaseIndex++,
			vPos );
	}

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << SST_FIRE;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetWorldOrientation();
		kPacket << m_vCirclePos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMoveAttackSkill4::CheckGauge( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_SkillState == SS_END )
		return;
	DWORD dwCurTime = FRAMEGETTIME();
	float fCurTicGauge = m_fGuagePerTic / m_fTicGaugeRateByGrowth;
	int iTickGap = (int)( dwCurTime - m_dwCheckTime );
	int iCurTicTime = m_dwTicTime + m_dwTicTimeByGrowth;
	if( m_SkillState != SS_END )
	{
		if( iTickGap >= iCurTicTime )
		{
			m_dwCheckTime = dwCurTime - ( iTickGap - iCurTicTime );

			if( fCurTicGauge != 0.0f )
			{
				if( pOwner->ReduceCurSkillGaugeBySkill( GetOwnerItem(), fCurTicGauge ) <= 0.0f )
				{
					SetEndState( pOwner );
					return;
				}
			}
		}
	}
}

void ioMoveAttackSkill4::SetEndState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	m_SkillState = SS_END;
	pOwner->SetSkillMoveEnable( false, false );
	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << SST_END;
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioMoveAttackSkill4::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	if ( m_SkillState == SS_END && m_fSkillEndNextJumpAmt == 0.0f )
		return true;

	return false;
}

bool ioMoveAttackSkill4::IsAttackEndState() const
{
	if( m_SkillState == SS_END )
		return true;

	return false;
}

float ioMoveAttackSkill4::GetSkillGravityRate()
{
	switch( m_SkillState )
	{
	case SS_DELAY:
		return m_fSkillGravityRate;
	case SS_JUMP:
	case SS_JUMP_ATTACK:
		if ( !m_bChangeJumpGravity )
			return m_fJumpFreeGravityRate;
		else
			return m_fJumpNormalGravityRate;
	}

	return m_fSkillGravityRate;
}

float ioMoveAttackSkill4::GetSkillMoveRate() const
{
	switch( m_SkillState )
	{
	case SS_DELAY:
		return m_fSkillMoveRate;
	case SS_JUMP:
		return m_fSkillJumpMoveSpeedRate;
	}

	return m_fSkillMoveRate;
}

void ioMoveAttackSkill4::OnSkillEnd( ioBaseChar *pOwner )
{
	ioAttackSkill::OnSkillEnd( pOwner );

	pOwner->SetSkillMoveEnable( false, false );

	if( m_bUseItemMesh )
		pOwner->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pOwner->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	if ( m_dwCircleEffectID != -1 )
	{
		ioPlayStage *pStage = pOwner->GetCreator();
		if ( pStage )
		{
			ioMapEffect* pMapEffect = pStage->FindMapEffect( m_dwCircleEffectID );
			if ( pMapEffect )
				pMapEffect->EndEffectForce();
		}
		
	}

	pOwner->SetGravityAmt( 0.0f );

	EndCameraBuff( pOwner );
	ClearData();
}

void ioMoveAttackSkill4::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	if ( pWeapon )
		pWeapon->SetMoveDir(m_vWeaponMoveDir);
}

void ioMoveAttackSkill4::ApplyExtraSkillInfo( ioBaseChar *pOwner, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pOwner || !pStage ) 
		return;
	int iState;
	rkPacket >> iState;
	D3DXVECTOR3 vPos;
	rkPacket >> vPos;
	pOwner->SetWorldPosition( vPos );
	switch( iState )
	{
	case SST_DELAY:
		SetDelayState( pOwner, pStage, false );
		break;
	case SST_JUMP:
		SetJumpState( pOwner );
		break;
	case SST_LANDING:
		SetLandingState( pOwner );
		break;
	case SST_NORMAL_ATTACK:
		SetNormalAttackState( pOwner );
		break;
	case SST_JUMP_ATTACK:
		SetJumpAttackState( pOwner );
		break;
	case SST_FIRE:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotAndMoveDirChange( qtRot );
			rkPacket >> m_vCirclePos;
			FireWeapon( pOwner );
		}
		break;
	case SST_END:
		SetEndState( pOwner );
		break;
	case SST_JUMP_CHANGE:
		rkPacket >> m_bChangeJumpGravity;
		break;
	case SST_JUMP_ATTACK_END:
		{
			m_SkillState = SS_JUMP;
			pOwner->SetSkillMoveEnable( true, false );
		}
		break;
	}
}
