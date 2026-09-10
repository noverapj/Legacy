#include "StdAfx.h"
#include "ioJumpingFire2Skill.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ioJumpingFire2Skill::ioJumpingFire2Skill(void)
{
	ClearData();
}

ioJumpingFire2Skill::ioJumpingFire2Skill( const ioJumpingFire2Skill &rhs ) :
ioAttackSkill( rhs ),
//Ani
m_LoopDelayAnimation( rhs.m_LoopDelayAnimation ),
m_fLoopDelayAniRate( rhs.m_fLoopDelayAniRate ),
m_LoopMoveAnimation( rhs.m_LoopMoveAnimation ),
m_fLoopMoveAniRate( rhs.m_fLoopMoveAniRate ),
m_EndAnimation( rhs.m_EndAnimation ),
m_fEndAniRate( rhs.m_fEndAniRate ),
//Time
m_dwFlyUpDuration( rhs.m_dwFlyUpDuration ),
m_dwLoopDuration( rhs.m_dwLoopDuration ),
m_dwLoopProtectTime( rhs.m_dwLoopProtectTime ),
m_dwLoopFireWeaponTicTime( rhs.m_dwLoopFireWeaponTicTime ),
//Value
m_fFlayUpMaxRange( rhs.m_fFlayUpMaxRange ),
m_fFlyUpAngle( rhs. m_fFlyUpAngle ),
m_fFlyMoveSpeed( rhs.m_fFlyMoveSpeed ),
m_fFlyUpDownSpeed( rhs.m_fFlyUpDownSpeed ),
m_vLoopWeaponInfoList( rhs.m_vLoopWeaponInfoList )
{
	ClearData();
}

void ioJumpingFire2Skill::ClearData()
{
	m_SkillState = SS_NONE;

	m_bSetMoveAni = false;

	m_dwLoopStartTime = 0;
	m_dwNextFireWeaponTime = 0;

	m_fFlyCurRange = 0;

	m_bSetUpKey = false;
	m_bSetDownKey = false;
	m_bExtraAttack = false;
}

ioJumpingFire2Skill::~ioJumpingFire2Skill(void)
{
}

ioSkill* ioJumpingFire2Skill::Clone()
{
	return new ioJumpingFire2Skill( *this );
}

void ioJumpingFire2Skill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	//Ani
	char szBuf[MAX_PATH];
	m_LoopDelayAnimation.Clear();
	rkLoader.LoadString_e( "loop_delay_animation", "", szBuf, MAX_PATH );
	m_LoopDelayAnimation = szBuf;
	m_fLoopDelayAniRate = rkLoader.LoadFloat_e("loop_delay_ani_rate", FLOAT1 );
	m_LoopMoveAnimation.Clear();
	rkLoader.LoadString_e( "loop_move_animation", "", szBuf, MAX_PATH );
	m_LoopMoveAnimation	= szBuf;
	m_fLoopMoveAniRate = rkLoader.LoadFloat_e("loop_move_ani_rate", FLOAT1 );
	m_EndAnimation.Clear();
	rkLoader.LoadString_e( "end_animation", "", szBuf, MAX_PATH );
	m_EndAnimation	= szBuf;
	m_fEndAniRate = rkLoader.LoadFloat_e("end_ani_rate", FLOAT1 );

	//Time
	m_dwFlyUpDuration = (DWORD)rkLoader.LoadInt_e( "fly_up_duration", 0 );
	m_dwLoopDuration = (DWORD)rkLoader.LoadInt_e( "loop_duration", 0 );
	m_dwLoopProtectTime = (DWORD)rkLoader.LoadInt_e( "loop_protecte_time", 0 );
	m_dwLoopFireWeaponTicTime = (DWORD)rkLoader.LoadInt_e( "loop_weapon_tic_time", 0 );
	//Value
	m_fFlayUpMaxRange = rkLoader.LoadFloat_e( "fly_up_max_range", 0.0f );
	m_fFlyUpAngle = rkLoader.LoadFloat_e( "fly_up_angle", 0.0f );
	m_fFlyMoveSpeed = rkLoader.LoadFloat_e( "fly_move_speed", 0.0f );
	m_fFlyUpDownSpeed = rkLoader.LoadFloat_e( "fly_updown_speed", 0.0f );


	m_vLoopWeaponInfoList.clear();
	int iLoopAttackCount = rkLoader.LoadInt_e( "loop_attack_type_count", 0 );
	if ( iLoopAttackCount > 0 )
	{
		m_vLoopWeaponInfoList.reserve( iLoopAttackCount );
		for ( int i=0 ; i < iLoopAttackCount ; ++i )
		{
			WeaponInfo kInfo;
			wsprintf_e( szBuf, "loop_attack_type%d", i+1 );
			kInfo.m_iWeaponIdx = rkLoader.LoadInt( szBuf, NO_WEAPON_ATTRIBUTE );

			wsprintf_e( szBuf, "loop_attack_type%d_resist", i+1 );
			kInfo.m_iResistanceIdx = rkLoader.LoadInt( szBuf, NO_WEAPON_ATTRIBUTE );

			wsprintf_e( szBuf, "loop_attack_type%d_wounded", i+1 );
			rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
			kInfo.m_WoundedAnimation = szBuf;

			wsprintf_e( szBuf, "loop_attack_type%d_wounded_time", i+1 );
			kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szBuf, 0 );

			wsprintf_e( szBuf, "loop_attack_type%d_wounded_loop_ani", i+1 );
			kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szBuf, false );

			m_vLoopWeaponInfoList.push_back( kInfo );
		}
	}
}

bool ioJumpingFire2Skill::OnSkillStart( ioBaseChar *pChar )
{
	if( !pChar )
		return false;

	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	SetCameraBuff( pChar );

	pChar->SetSkillMoveEnable( false, false );

	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	if( !m_bReduceGauge )
	{
		float fCurGauge = pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	ClearData();

	return true;
}

void ioJumpingFire2Skill::OnSkillEnd( ioBaseChar *pChar )
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

	pChar->SetSkillMoveEnable( false, false );

	EndCameraBuff( pChar );

	ClearData();
}

void ioJumpingFire2Skill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_SkillState )
	{
	case SS_NONE:
		{
			pChar->SetCurMoveSpeed( 0 );
			if( !ProcessFlyUp( pChar ) )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );
				SetLoopState( pChar, pStage );
			}
		}
		break;
	case SS_LOOP:
		{
			if ( m_dwLoopStartTime + m_dwLoopDuration < dwCurTime )
			{
				SetEndState( pChar );
				break;
			}
			CheckExtraKeyInput( pChar );
			if ( m_bExtraAttack )
			{
				SetAttackState( pChar );
				break;
			}
			CheckNormalMoveAni( pChar );
			ProcessFlyMove( pChar );
			ProcessLoopFire( pChar );
		}
		break;
	case SS_ATTACK:
		{
			if ( m_dwMotionEndTime <= dwCurTime )
			{
				SetEndState( pChar );
				break;
			}
		}
		break;
	}
}

bool ioJumpingFire2Skill::ProcessFlyUp( ioBaseChar *pChar )
{
	if( !pChar )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	D3DXVECTOR3 vTargetDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vTargetDir, &ioMath::UNIT_Y );
	D3DXVec3Normalize( &vAxis, &vAxis );

	D3DXQUATERNION qtNewRot;
	float fCurAngle = ioMath::ArrangeHead( m_fFlyUpAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	D3DXVECTOR3 vMoveDir = qtNewRot * vTargetDir;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	if( vMoveDir == ioMath::VEC3_ZERO )
		return false;

	if( m_dwFlyUpDuration == 0 )
		return false;

	float fCurSpeed = m_fFlayUpMaxRange / (float)m_dwFlyUpDuration * FLOAT1000;
	fCurSpeed = max( 0.0f, fCurSpeed );

	float fDistance = fCurSpeed * fTimePerSec;

	bool bRangeLimit = false;
	D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );
	if( m_fFlyCurRange+fDistance < m_fFlayUpMaxRange )
	{
		bRangeLimit = true;
		vMoveAmt = vMoveDir * fDistance;

		m_fFlyCurRange += fDistance;
	}
	else
	{
		bRangeLimit = false;

		float fGapDistance = m_fFlayUpMaxRange - m_fFlyCurRange;
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

void ioJumpingFire2Skill::SetLoopState( ioBaseChar* pChar, ioPlayStage *pStage )
{
	if( !pChar )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_SkillState = SS_LOOP;

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iAniID = 0;
	iAniID = pGrp->GetAnimationIdx( m_LoopDelayAnimation );
	if( iAniID == -1 )
		return;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/m_fLoopDelayAniRate );
	m_dwMotionEndTime = 0;

	pChar->SetSkillMoveEnable( true, true );

	m_bSetMoveAni = false;

	m_dwLoopStartTime = m_dwNextFireWeaponTime = dwCurTime;
	m_dwNextFireWeaponTime += m_dwLoopFireWeaponTicTime;
}

void ioJumpingFire2Skill::CheckNormalMoveAni( ioBaseChar* pChar )
{
	if( !pChar ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = 0;

	if( pChar->IsSettedDirection() )
	{	
		if( !m_bSetMoveAni )
		{	
			iAniID = pGrp->GetAnimationIdx( m_LoopMoveAnimation );
			if( iAniID == -1 )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1/m_fLoopMoveAniRate );	
			m_bSetMoveAni = true;
		}
	}
	else
	{
		if( m_bSetMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx( m_LoopDelayAnimation );
			if( iAniID == -1 )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1/m_fLoopDelayAniRate );
			m_bSetMoveAni = false;
		}
	}
}

void ioJumpingFire2Skill::ProcessFlyMove( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	ioPlayStage *pStage = pChar->GetCreator();
	if( !pStage )
		return;

	if( pChar->IsSettedDirection() )
		pChar->SetCurMoveSpeed( m_fFlyMoveSpeed );
	else
		pChar->SetCurMoveSpeed( 0.0f );

	//지상과의 높이 체크
	D3DXVECTOR3 vCurPos = pChar->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pChar, false, pChar->GetMidHeightByRate(1.0f) );

	// UpDown Move
	if ( m_fFlyUpDownSpeed > 0.0f )
	{
		if( m_bSetUpKey || m_bSetDownKey )
		{
			float fTimePerSec = g_FrameTimer.GetSecPerFrame();
			float fCurHeightAmt = 0.0f;
			float fCurUpDownSpeed = m_fFlyUpDownSpeed * fTimePerSec;

			if( m_bSetUpKey )
				fCurHeightAmt += fCurUpDownSpeed;
			else if( m_bSetDownKey )
				fCurHeightAmt -= fCurUpDownSpeed;

			vCurPos.y += fCurHeightAmt;
		}
	}
	vCurPos.y = max( vCurPos.y, fMapHeight + 11.0f );

	pStage->CheckFlyMapLimit( &vCurPos );
	pStage->CheckAxisLimit( &vCurPos );
	pChar->SetWorldPosition( vCurPos );
}

void ioJumpingFire2Skill::ProcessLoopFire( ioBaseChar *pChar )
{
	if ( !pChar || m_vLoopWeaponInfoList.empty() || !pChar->IsNeedProcess() )
		return;
	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwNextFireWeaponTime < dwCurTime )
	{
		m_dwNextFireWeaponTime = dwCurTime + m_dwLoopFireWeaponTicTime;
		CreateLoopWeapon( pChar );
		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SSC_LOOP_WEAPON;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioJumpingFire2Skill::CreateLoopWeapon( ioBaseChar *pChar )
{
	if ( !pChar || m_vLoopWeaponInfoList.empty() )
		return;
	
	for ( int i = 0 ; i < (int)m_vLoopWeaponInfoList.size() ; ++i )
	{
		pChar->SkillFireExplicit( m_vLoopWeaponInfoList[i],
			GetName(),
			m_dwWeaponBaseIndex++,
			pChar->GetWorldPosition() );
	}
}

void ioJumpingFire2Skill::CheckExtraKeyInput( ioBaseChar *pChar )
{
	if( !pChar || !pChar->IsNeedProcess() || m_fFlyUpDownSpeed <= 0.0f )
		return;

	bool bChangeUpDown = false;
	bool bAttackKey = false;

	if ( m_fFlyUpDownSpeed > 0.0f )
	{
		// Check UpDown
		if( m_bSetUpKey )
		{
			if( !pChar->IsJumpKeyDown() )
			{
				bChangeUpDown = true;
				m_bSetUpKey = false;

				if( pChar->IsDefenseKeyDown() )
					m_bSetDownKey = true;
			}
		}
		else if( m_bSetDownKey )
		{
			if( !pChar->IsDefenseKeyDown() )
			{
				bChangeUpDown = true;
				m_bSetDownKey = false;

				if( pChar->IsJumpKeyDown() )
					m_bSetUpKey = true;
			}
		}
		else
		{
			if( pChar->IsDefenseKeyDown() )
			{
				bChangeUpDown = true;
				m_bSetUpKey = false;
				m_bSetDownKey = true;
			}

			if( pChar->IsJumpKeyDown() )
			{
				bChangeUpDown = true;
				m_bSetUpKey = true;
				m_bSetDownKey = false;
			}
		}
	}

	//Check Attack Key
	if( pChar->IsAttackKey() )
		m_bExtraAttack = true;

	if( bChangeUpDown )
	{
		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SSC_ROTATE;
			kPacket << m_bSetUpKey;
			kPacket << m_bSetDownKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	if ( m_bExtraAttack )
	{
		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SSC_EXTRA_ATTACK;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioJumpingFire2Skill::SetAttackState( ioBaseChar* pChar )
{
	m_SkillState = SS_ATTACK;
	pChar->SetSkillMoveEnable( false, false );
	ioAttackSkill::AttackSkillFire( pChar );
}

void ioJumpingFire2Skill::SetEndState( ioBaseChar* pChar )
{
	if( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if ( !pGrp )
		return;
	m_SkillState = SS_END;
	int iAniID = 0;
	iAniID = pGrp->GetAnimationIdx( m_EndAnimation );
	if( iAniID == -1 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * m_fEndAniRate;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/m_fEndAniRate, 0.0f );
	pChar->SetSkillMoveEnable( false, false );
}
bool ioJumpingFire2Skill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if ( m_SkillState == SS_END && m_dwMotionEndTime < FRAMEGETTIME() && m_fSkillEndNextJumpAmt == 0.0f )
		return true;
	return false;
}

bool ioJumpingFire2Skill::IsProtected( int iDefenseBreakType ) const
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

	if( m_dwLoopProtectTime > 0 && m_dwLoopStartTime + m_dwLoopProtectTime > FRAMEGETTIME() )
		return true;
	return false;
}

bool ioJumpingFire2Skill::IsEndState() const
{
	if( m_SkillState == SS_END && m_dwMotionEndTime < FRAMEGETTIME() )
		return true;

	return false;
}

bool ioJumpingFire2Skill::IsNoDropState() const
{
	return true;
}
void ioJumpingFire2Skill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case SSC_ROTATE:
		{
			rkPacket >> m_bSetUpKey;
			rkPacket >> m_bSetDownKey;  
		}
		break;
	case SSC_LOOP_WEAPON:
		{
			CreateLoopWeapon( pChar );
		}
		break;
	case SSC_EXTRA_ATTACK:
		{
			SetAttackState( pChar );
		}
		break;
	}
}