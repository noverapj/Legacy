#include "StdAfx.h"
#include "ioKeyWaitAttackSKill.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioFireWorksStep1Weapon.h"

ioKeyWaitAttackSKill::ioKeyWaitAttackSKill(void)
{
	ClearData();
}

ioKeyWaitAttackSKill::ioKeyWaitAttackSKill( const ioKeyWaitAttackSKill &rhs ) :
ioAttackSkill( rhs ),
//weapon offset
m_vWeaponOffsetList( rhs.m_vWeaponOffsetList ),
//loop
m_szLoopMotion( rhs.m_szLoopMotion ),
m_fLoopMotionRate( rhs.m_fLoopMotionRate ),
m_dwLoopDuration( rhs.m_dwLoopDuration ),
m_dwKeyInputEnableTime( rhs.m_dwKeyInputEnableTime ),
//key action
m_KeyActionAttribute( rhs.m_KeyActionAttribute ),
//end
m_szEndMotion( rhs.m_szEndMotion ),
m_fEndMotionRate( rhs.m_fEndMotionRate ),
//protect
m_dwProtectTime( rhs.m_dwProtectTime ),
//jump
m_fJumpAngle( rhs.m_fJumpAngle ),
m_fJumpMaxRange( rhs.m_fJumpMaxRange ),
m_dwJumpDuration( rhs.m_dwJumpDuration ),
m_bDisableJumpWhenJumpSkill( rhs.m_bDisableJumpWhenJumpSkill )
{
	ClearData();
}

ioKeyWaitAttackSKill::~ioKeyWaitAttackSKill(void)
{
}

ioSkill* ioKeyWaitAttackSKill::Clone()
{
	return new ioKeyWaitAttackSKill( *this );
}

void ioKeyWaitAttackSKill::ClearData()
{
	m_dwFireStartTime = 0;
	m_dwProtectStartTime = 0;
	m_vWeaponIndexList.clear();
	m_SkillState = SS_NONE;
	m_fJumpCurRange = 0;
}

void ioKeyWaitAttackSKill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );
	char szBuf[MAX_PATH];
	int iWeaponOffsetCount= rkLoader.LoadInt_e( "weapon_offset_count", 0 );
	if ( iWeaponOffsetCount > 0 )
		m_vWeaponOffsetList.reserve( iWeaponOffsetCount );
	for ( int i=0 ; i<iWeaponOffsetCount ; ++i )
	{
		D3DXVECTOR3 vOffset;
		wsprintf_e( szBuf, "weapon_offset%d_x", i+1 );
		vOffset.x = rkLoader.LoadFloat( szBuf, 0.0f );
		wsprintf_e( szBuf, "weapon_offset%d_y", i+1 );
		vOffset.y = rkLoader.LoadFloat( szBuf, 0.0f );
		wsprintf_e( szBuf, "weapon_offset%d_z", i+1 );
		vOffset.z = rkLoader.LoadFloat( szBuf, 0.0f );

		m_vWeaponOffsetList.push_back( vOffset );
	}

	m_dwProtectTime = (DWORD)rkLoader.LoadInt( "skill_protect_time", 0 );

	//loop
	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_szLoopMotion = szBuf;
	m_fLoopMotionRate = rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );
	m_dwKeyInputEnableTime = (DWORD)rkLoader.LoadInt_e( "key_input_enable_time", 0 );
	m_dwLoopDuration = (DWORD)rkLoader.LoadInt_e( "loop_duration", 0 );

	//key action
	LoadAttackAttribute( "key_action_motion", m_KeyActionAttribute, rkLoader );

	//End
	rkLoader.LoadString_e( "skill_end_motion", "", szBuf, MAX_PATH );
	m_szEndMotion = szBuf;
	m_fEndMotionRate = rkLoader.LoadFloat_e( "skill_end_motion_rate", FLOAT1 );

	//Jump
	m_fJumpAngle = rkLoader.LoadFloat_e( "jump_angle", 0.0f );
	m_fJumpMaxRange = rkLoader.LoadFloat_e( "jump_max_range", 0.0f );
	m_dwJumpDuration = (DWORD)rkLoader.LoadInt_e( "jump_duration", 0 );
	m_bDisableJumpWhenJumpSkill = rkLoader.LoadBool( "disable_jump_when_jump_skill", false );
}

bool ioKeyWaitAttackSKill::OnSkillStart( ioBaseChar *pChar )
{
	if ( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{			
		pChar->ShowWeaponItemMesh( false );
	}

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	ClearData();

	return true;
}

void ioKeyWaitAttackSKill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	// 스킬 중간 동작에 사용된 메쉬들 제거	
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );	

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge( pChar->GetCharName() ) );
		m_bReduceGauge = true;
	}
	ioPlayStage *pStage = pChar->GetCreator();
	if ( !pStage )
		return;
	DestroyWeapon( pChar, pStage );

	ClearData();
}

void ioKeyWaitAttackSKill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_NONE :
		{
			pChar->SetCurMoveSpeed( 0 );
			if( !ProcessFlyStart( pChar ) )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );
				AttackSkillFire(pChar);
			}
		}
		break;

	case SS_FIRE :
		{	
			CheckFireState( pChar, pStage );

			if( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
				SetLoopState( pChar );
		}
		break;

	case SS_LOOP:
		{
			if( m_dwLoopDuration > 0 && m_dwLoopStartTime + m_dwLoopDuration < dwCurTime )
			{
				SetEndState( pChar, pStage );
				break;
			}

			if ( pChar->IsNeedProcess() )
			{
				if ( m_dwLoopStartTime + m_dwKeyInputEnableTime < dwCurTime )
				{
					if ( pChar->IsAttackKey() )
					{
						SetKeyActionState( pChar, pStage );
						break;
					}
				}
			}
		}
		break;

	case SS_KEY_ACTION:
		{
			if( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
				SetEndState( pChar, pStage );
		}
		break;
	}
}

bool ioKeyWaitAttackSKill::ProcessFlyStart( ioBaseChar *pChar )
{
	if( !pChar )
		return false;

	if ( m_bDisableJumpWhenJumpSkill && m_SkillUseType == SUT_JUMP )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	D3DXVECTOR3 vTargetDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vTargetDir, &ioMath::UNIT_Y );
	D3DXVec3Normalize( &vAxis, &vAxis );

	D3DXQUATERNION qtNewRot;
	float fCurAngle = ioMath::ArrangeHead( m_fJumpAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	D3DXVECTOR3 vMoveDir = qtNewRot * vTargetDir;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	if( vMoveDir == ioMath::VEC3_ZERO )
		return false;

	if( m_dwJumpDuration == 0 )
		return false;

	float fCurSpeed = m_fJumpMaxRange / (float)m_dwJumpDuration * FLOAT1000;
	fCurSpeed = max( 0.0f, fCurSpeed );

	float fDistance = fCurSpeed * fTimePerSec;

	bool bRangeLimit = false;
	D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );
	if( m_fJumpCurRange+fDistance < m_fJumpMaxRange )
	{
		bRangeLimit = true;
		vMoveAmt = vMoveDir * fDistance;

		m_fJumpCurRange += fDistance;
	}
	else
	{
		bRangeLimit = false;

		float fGapDistance = m_fJumpMaxRange - m_fJumpCurRange;
		vMoveAmt = vMoveDir * fGapDistance;
	}

	bool bCol = false;
	ioPlayStage *pStage = pChar->GetCreator();
	if( pStage && pStage->ReCalculateMoveVectorByTerrain( pChar, &vMoveAmt, bCol ) )
	{
		pChar->Translate( vMoveAmt );
	}

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	if( pChar->CheckAxisLimit( &vPos ) )
		pChar->SetWorldPosition( vPos );

	return bRangeLimit;
}

bool ioKeyWaitAttackSKill::IsProtected( int iDefenseBreakType ) const
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

	if( m_dwProtectTime > 0 && m_dwProtectStartTime + m_dwProtectTime > dwCurTime )
		return true;

	return false;
}

bool ioKeyWaitAttackSKill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if ( m_SkillState == SS_END && m_dwMotionEndTime < FRAMEGETTIME() && m_fSkillEndNextJumpAmt <= 0.0f)
		return true;
	return false;
}

void ioKeyWaitAttackSKill::AttackSkillFire( ioBaseChar *pChar )
{
	if( !pChar ) return;
	m_SkillState = SS_FIRE;

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )	return;

	float fAniRate = GetAttribute().m_fAttackAniRate;
	if( fAniRate <= 0.0f )
		fAniRate = FLOAT1;

	DWORD dwPreDelay = GetAttribute().m_dwPreDelay * fAniRate;
	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fAniRate;
	m_dwMotionEndTime += dwPreDelay;

	m_dwFireMotionEndTime = dwCurTime;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fAniRate;

	if( m_dwFireStartTime == 0 )
	{
		m_dwFireStartTime = dwCurTime;
		m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fAniRate;
	}

	m_dwProtectStartTime = dwCurTime;
}

void ioKeyWaitAttackSKill::CheckFireState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if ( !pChar || GetAttribute().m_vWeaponInfoList.empty() )
		return;

	if( m_dwFireStartTime <dwCurTime && m_dwFireStartTime != 0 )
	{	
		m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
		for ( int i=0 ; i<(int)GetAttribute().m_vWeaponInfoList.size() ; ++i )
		{
			D3DXVECTOR3 vWeaponPos = pChar->GetMidPositionByRate();
			if ( i < (int)m_vWeaponOffsetList.size() )
			{
				D3DXVECTOR3 vCharDirX = pChar->GetTargetRot() * ioMath::UNIT_X;
				D3DXVECTOR3 vCharDirY = pChar->GetTargetRot() * ioMath::UNIT_Y;
				D3DXVECTOR3 vCharDirZ = pChar->GetTargetRot() * -(ioMath::UNIT_Z);
				D3DXVec3Normalize( &vCharDirX, &vCharDirX );
				D3DXVec3Normalize( &vCharDirY, &vCharDirY );
				D3DXVec3Normalize( &vCharDirZ, &vCharDirZ );
				vWeaponPos += vCharDirX * m_vWeaponOffsetList[i].x ;
				vWeaponPos += vCharDirY * m_vWeaponOffsetList[i].y ;
				vWeaponPos += vCharDirZ * m_vWeaponOffsetList[i].z ;
			}
			DWORD dwWeaponIndex = m_dwWeaponBaseIndex + (DWORD)i;
			pChar->SkillFireExplicit( GetAttribute().m_vWeaponInfoList[i],
				GetName(),
				dwWeaponIndex,
				vWeaponPos );
			m_vWeaponIndexList.push_back( dwWeaponIndex );
		}

		m_dwFireStartTime = 0;
	}
}

void ioKeyWaitAttackSKill::SetLoopState( ioBaseChar *pChar )
{
	DWORD dwCurTime = FRAMEGETTIME();
	m_SkillState = SS_LOOP;

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iAniID = 0;
	iAniID = pGrp->GetAnimationIdx( m_szLoopMotion );
	if( iAniID == -1 )
		return;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = m_fLoopMotionRate;
	if( fAniRate <= 0.0f )
		fAniRate = FLOAT1;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );
	m_dwLoopStartTime = dwCurTime;
}

void ioKeyWaitAttackSKill::SetKeyActionState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage)
		return;
	m_SkillState = SS_KEY_ACTION;

	DestroyWeapon( pChar, pStage );

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	int iAniID = pGrp->GetAnimationIdx( m_KeyActionAttribute.m_AttackAnimation );
	if( iAniID == -1 )	return;

	float fAniRate = m_KeyActionAttribute.m_fAttackAniRate;
	if( fAniRate <= 0.0f )
		fAniRate = FLOAT1;
	DWORD dwPreDelay = m_KeyActionAttribute.m_dwPreDelay;
	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fAniRate;
	m_dwMotionEndTime += m_KeyActionAttribute.m_dwPreDelay;

	if ( !m_KeyActionAttribute.m_vWeaponInfoList.empty())
	{
		m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
			iAniID,
			GetAttribute().m_vWeaponInfoList,
			FTT_SKILL_ATTACK,
			fAniRate,
			dwPreDelay );
	}

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_KEY_ACTION;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioKeyWaitAttackSKill::SetEndState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage)
		return;
	m_SkillState = SS_END;

	DestroyWeapon( pChar, pStage );

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	int iAniID = pGrp->GetAnimationIdx( m_szEndMotion );
	if( iAniID == -1 )	return;

	float fAniRate = m_fEndMotionRate;
	if( fAniRate <= 0.0f )
		fAniRate = FLOAT1;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fAniRate;
}

void ioKeyWaitAttackSKill::DestroyWeapon( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage || m_vWeaponIndexList.empty())
		return;

	for ( int i=0 ; i<(int)m_vWeaponIndexList.size() ; ++i)
	{
		ioWeapon* pWeapon = g_WeaponMgr.FindWeapon( pChar, m_vWeaponIndexList[i] );
		if ( pWeapon )
		{
			ioFireWorksStep1Weapon* pFireWorksStep1Weapon = ToFireWorksStep1Waepon( pWeapon );
			if ( pFireWorksStep1Weapon )
			{
				if ( pFireWorksStep1Weapon->IsLive() && !pFireWorksStep1Weapon->IsExplosion() )
					pFireWorksStep1Weapon->Explosion( pStage );
			}
		}
	}

	m_vWeaponIndexList.clear();
}

void ioKeyWaitAttackSKill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar || !pStage ) return;
	int iState;
	rkPacket >> iState;
	if ( iState == SS_KEY_ACTION )
		SetKeyActionState( pChar, pStage );
}

bool ioKeyWaitAttackSKill::IsNoDropState() const
{
	if ( m_SkillState != SS_END )
		return true;
	return false;
}

bool ioKeyWaitAttackSKill::IsAttackEndState() const
{
	return IsEndState();
}

bool ioKeyWaitAttackSKill::IsEndState() const
{
	if ( m_SkillState == SS_END && m_dwMotionEndTime < FRAMEGETTIME() && m_fSkillEndNextJumpAmt > 0.0f)
		return true;

	return false;
}
