

#include "stdafx.h"

#include "ioWhirlwndAttack.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioWhirlwndAttack::ioWhirlwndAttack()
{
	Init();
}

ioWhirlwndAttack::ioWhirlwndAttack( const ioWhirlwndAttack &rhs )
	: ioAttackSkill( rhs ),
	m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	m_fCircleRange( rhs.m_fCircleRange ),
	m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
	m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
	m_fCircleOffSet( rhs.m_fCircleOffSet ),
	m_szOnCircle( rhs.m_szOnCircle ),
	m_szOffCircle( rhs.m_szOffCircle ),
	m_szLoopAnimation( rhs.m_szLoopAnimation ),
	m_fLoopAniRate( rhs.m_fLoopAniRate ),
	m_dwLoopDuration( rhs.m_dwLoopDuration ),
	m_LoopEndAttack( rhs.m_LoopEndAttack ),
	m_szLoopEndAttackEffect( rhs.m_szLoopEndAttackEffect ),
	m_fLoopEndAttackEffectYOffset( rhs.m_fLoopEndAttackEffectYOffset ),
	m_fLoopEndAttackEffectMoveSpeed( rhs.m_fLoopEndAttackEffectMoveSpeed ),
	m_szPreFastMove( rhs.m_szPreFastMove ),
	m_fPreFastMoveRate( rhs.m_fPreFastMoveRate ),
	m_fFastMoveSpeed( rhs.m_fFastMoveSpeed ),
	m_szFastMoveEnd( rhs.m_szFastMoveEnd ),	
	m_szFastMoveMapEffect( rhs.m_szFastMoveMapEffect ),
	m_fFastMoveEndRate( rhs.m_fFastMoveEndRate ),
	m_szWhirlAttack( rhs.m_szWhirlAttack ),
	m_fWhirlAttackRate( rhs.m_fWhirlAttackRate ),
	m_fMoveSpeed( rhs.m_fMoveSpeed ),
	m_dwWhirlAttackDuration( rhs.m_dwWhirlAttackDuration ),
	m_WhirlAttackInfo( rhs.m_WhirlAttackInfo ),
	m_dwCharRotateTime( rhs.m_dwCharRotateTime ),
	m_WallEndAttack( rhs.m_WallEndAttack )
{
	Init();
}

ioWhirlwndAttack::~ioWhirlwndAttack()
{
}

void ioWhirlwndAttack::Init()
{
	m_SkillState				= SS_PRE;

	m_dwLoopStartTime			= 0;
	m_dwPreFastMoveEndTime		= 0;	
	m_dwFastMoveEndEndTime		= 0;
	m_dwWhirlAttackEndTime		= 0;

	m_LoopEndAttackEffectID		= 0;
	m_FastMoveMapEffectID		= 0;
			
	m_dwWhirlFastMoveIndex		= 0;
	m_dwWhirlAttackIndex		= 0;

	m_fCurFastMoveRange			= 0.0f;
	m_fMaxFastMoveRange			= 0.0f;

	m_bSetHandMesh				= false;
	m_bCircleOn					= false;
	m_vAttackDir				= ioMath::VEC3_ZERO;
	m_vAttackPos				= ioMath::VEC3_ZERO;
	m_vCircleStartPos			= ioMath::VEC3_ZERO;

	m_szCurCircle.Clear();
}

void ioWhirlwndAttack::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );
	
	m_dwSkillProtectTime = rkLoader.LoadInt_e( "skill_protect_time", 0 );
	
	m_fCircleRange		= rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleMoveSpeed	= rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap	= rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet		= rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "on_magic_circle", "", szBuf, MAX_PATH );
	m_szOnCircle		= szBuf;
	rkLoader.LoadString_e( "off_magic_circle", "", szBuf, MAX_PATH );
	m_szOffCircle		= szBuf;

	rkLoader.LoadString_e( "loop_animation", "", szBuf, MAX_PATH );
	m_szLoopAnimation	= szBuf;
	m_dwLoopDuration = rkLoader.LoadInt_e( "loop_duration", 0 );
	m_fLoopAniRate = rkLoader.LoadFloat_e( "loop_animation_rate", FLOAT1 );

	LoadAttackAttribute( "loop_end_attack", m_LoopEndAttack, rkLoader );
	rkLoader.LoadString_e( "loop_end_attack_effect", "", szBuf, MAX_PATH );
	m_szLoopEndAttackEffect			= szBuf;
	m_fLoopEndAttackEffectYOffset	= rkLoader.LoadFloat_e( "loop_end_attack_effect_y_offset", 0.0f );
	m_fLoopEndAttackEffectMoveSpeed	= rkLoader.LoadFloat_e( "loop_end_attack_effect_speed", 0.0f );

	rkLoader.LoadString_e( "pre_fast_move", "", szBuf, MAX_PATH );
	m_szPreFastMove	= szBuf;
	m_fPreFastMoveRate = rkLoader.LoadFloat_e( "pre_fast_move_rate", FLOAT1 );
		
	m_fFastMoveSpeed = rkLoader.LoadFloat_e( "fast_move_speed", 0.0f );		
	rkLoader.LoadString_e( "fast_move_map_effect", "", szBuf, MAX_PATH );
	m_szFastMoveMapEffect = szBuf;

	rkLoader.LoadString_e( "fast_move_end", "", szBuf, MAX_PATH );
	m_szFastMoveEnd = szBuf;
	m_fFastMoveEndRate		= rkLoader.LoadFloat_e( "fast_move_end_rate", FLOAT1 );

	rkLoader.LoadString_e( "whirl_attack", "", szBuf, MAX_PATH );
	m_szWhirlAttack = szBuf;
	m_fWhirlAttackRate		= rkLoader.LoadFloat_e( "whirl_attack_rate", FLOAT1 );
	m_dwWhirlAttackDuration = rkLoader.LoadInt_e( "whirl_attack_duration", 0 );	

	LoadWeaponInfo( "whirl_attack", m_WhirlAttackInfo, rkLoader );
	m_dwCharRotateTime = rkLoader.LoadInt_e( "char_rotate_speed", 0 );
	m_fMoveSpeed	= rkLoader.LoadFloat_e( "char_move_speed", FLOAT100 );

	LoadAttackAttribute( "wall_end_attack", m_WallEndAttack, rkLoader );
}

ioSkill* ioWhirlwndAttack::Clone()
{
	return new ioWhirlwndAttack( *this );
}

bool ioWhirlwndAttack::OnSkillStart( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillStart( pChar );

	Init();

	CheckWeaponItemMesh( pChar );

	m_dwSkillStartTime = FRAMEGETTIME();

	return true;
}

void ioWhirlwndAttack::CheckWeaponItemMesh( ioBaseChar* pChar )
{
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}

void ioWhirlwndAttack::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	switch( m_SkillState )
	{
	case SS_PRE: 
		{
			CheckPreState( pChar, pStage );
		}
		break;
	case SS_LOOP:
		{
			CheckLoopState( pChar, pStage );
		}
		break;
	case SS_LOOP_END_ATTACK:
		{
			CheckLoopEndState( pChar, pStage );
		}
		break;
	case SS_FAST_MOVE_PRE:
		{
			CheckMovePreState( pChar, pStage );
		}
		break;
	case SS_FAST_MOVE:
		{
			CheckFastMoveState( pChar, pStage );
		}
		break;
	case SS_FAST_MOVE_END:
		{
			CheckFastEndState( pChar, pStage );
		}
		break;
	case SS_WHIRLWND_ATTACK:
		{
			CheckWhirlAttackState( pChar, pStage );
			ProcessCharRotate( pChar, pStage );
		}
		break;
	case SS_CHOP:
		{
			CheckChopState( pChar, pStage );
		}
		break;
	case SS_WALL:
		{
			CheckWallState( pChar, pStage );
		}
		break;
	case SS_END:
		{
		}
		break;
	}
}

void ioWhirlwndAttack::CheckPreState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwPreDelay = GetPreDelayTime();

	if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
	{
		m_dwPreDelayStartTime = 0;
		EndPreDelayEffect( pChar );
		SetLoopState( pChar, pStage );
	}
}

void ioWhirlwndAttack::SetLoopState( ioBaseChar* pChar, ioPlayStage* pStage )
{
	m_SkillState		= SS_LOOP;
	m_dwLoopStartTime	= FRAMEGETTIME();

	if( m_szLoopAnimation.IsEmpty() || !pChar )
	{
		return;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_szLoopAnimation );
	if( iAniID == -1 )
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/m_fLoopAniRate );
		
	pChar->CreateMagicCircle( m_szOnCircle, m_szOnCircle, GetMagicGuide( pChar ), m_fCircleOffSet );
	m_vCircleStartPos = pChar->GetMagicCirclePos();
}

const ioHashString& ioWhirlwndAttack::GetMagicGuide( ioBaseChar *pChar )
{
	// 팀 체크
	if( pChar->GetTeam() == TEAM_RED )
	{
		return m_szRedGuideCircle;
	}
	else if( pChar->GetTeam() == TEAM_BLUE )
	{
		return m_szBlueGuideCircle;
	}
	else
	{
		if( pChar->GetSingleConvertTeam() == TEAM_BLUE )
		{
			return m_szBlueGuideCircle;
		}
	}
	
	return m_szRedGuideCircle;
}

void ioWhirlwndAttack::CheckLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	CheckCircle( pChar, pStage );

	// 뱡향 변화 체크
	pChar->CheckChangeDirectionForMagicCircle();
		
	pChar->UpdateMagicCirclePosAll( m_fCircleMoveSpeed, m_fCircleRange, m_fCircleOffSet, m_fCircleHeightGap, false );
		
	D3DXVECTOR3 vTargetPos = pChar->GetMagicCirclePos();
	pChar->SetTargetRotToTargetPos( vTargetPos, true, false );
	
	if( pChar->IsNeedProcess() )
	{
		if( 0 < m_dwLoopStartTime && m_dwLoopStartTime + m_dwLoopDuration < FRAMEGETTIME() )
		{
			if( m_bCircleOn )
			{
				SetLoopEndAttackState( pChar, pStage );
			}
			else
			{
				SetEnd( pChar, pStage );
			}
		}
		else if( pChar->IsAttackKey() && m_bCircleOn )
		{
			SetLoopEndAttackState( pChar, pStage );
		}
	}
}

void ioWhirlwndAttack::CheckCircle( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pStage )
		return;

	if( !pChar )
		return;

	D3DXVECTOR3 vPos = pChar->GetMagicCirclePos();

	if( pStage->GetMapHeightNoneEntity( vPos.x, vPos.z, vPos.y + 3.0f ) > 0.0f )
	{
		if( m_szCurCircle != m_szOnCircle )
		{
			pChar->CreateMagicCircle( m_szOnCircle, m_szOnCircle, GetMagicGuide( pChar ), m_fCircleOffSet );			
			pChar->SetMagicCirclePos( false, vPos );
			m_szCurCircle	= m_szOnCircle;
			m_bCircleOn		= true;
		}
	}
	else
	{
		if( m_szCurCircle != m_szOffCircle )
		{
			pChar->CreateMagicCircle( m_szOffCircle, m_szOffCircle, GetMagicGuide( pChar ), m_fCircleOffSet );			
			pChar->SetMagicCirclePos( false, vPos );
			m_szCurCircle	= m_szOffCircle;
			m_bCircleOn		= false;
		}
	}
}

void ioWhirlwndAttack::SetLoopEndAttackState( ioBaseChar* pChar, ioPlayStage* pStage, bool bSync )
{
	m_SkillState = SS_LOOP_END_ATTACK;

	//ioWhirlwndAttack::GetAttribute()에서 해당 스테이트에 맞는 공격을 던져줌
	ioAttackSkill::AttackSkillFire( pChar );
		
	if( pChar->IsNeedProcess() )
	{
		m_vAttackPos = pChar->GetMagicCirclePos();
		m_vAttackDir = m_vAttackPos - pChar->GetWorldPosition();
		D3DXVec3Normalize( &m_vAttackDir, &m_vAttackDir );
	}

	D3DXVECTOR3 vPos = m_vAttackPos;
	vPos.y += m_fLoopEndAttackEffectYOffset;

	ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_szLoopEndAttackEffect, vPos, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
	if( pMapEffect && pMapEffect->GetEffect() )
	{
		m_LoopEndAttackEffectID = pMapEffect->GetEffect()->GetUniqueID();
	}
	pChar->DestroyMagicCircle();

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << (int)SSS_LOOP_END_ATTACK;
		kPacket << pChar->GetWorldPosition();
		kPacket << pChar->GetTargetRot();
		kPacket << m_vAttackPos;
		kPacket << m_vAttackDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioWhirlwndAttack::CheckLoopEndState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( 0 < m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
	{		
		SetFastMovePre( pChar, pStage );
	}
	else
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_LoopEndAttackEffectID );
		if( pMapEffect )
		{
			float fMoveDist		= m_fLoopEndAttackEffectMoveSpeed * g_FrameTimer.GetSecPerFrame();
			D3DXVECTOR3 vMove	= D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) * fMoveDist;
			D3DXVECTOR3 vPos	= pMapEffect->GetWorldPosition() + vMove;

			float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z, NULL, true, vPos.y );
			if( fMapHeight > 0.0f )
			{
				pMapEffect->SetWorldPosition( vPos );
			}
		}
	}
}

void ioWhirlwndAttack::SetFastMovePre( ioBaseChar* pChar, ioPlayStage* pStage )
{
	m_SkillState			= SS_FAST_MOVE_PRE;
	m_dwPreFastMoveEndTime	= FRAMEGETTIME();

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;
		
	int iAniID = pGrp->GetAnimationIdx( m_szPreFastMove );
	if ( iAniID == -1 )
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fPreFastMoveRate );

	m_dwPreFastMoveEndTime += pGrp->GetAnimationFullTime( iAniID ) * m_fPreFastMoveRate;
}

void ioWhirlwndAttack::CheckMovePreState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( 0 < m_dwPreFastMoveEndTime && m_dwPreFastMoveEndTime < FRAMEGETTIME() )
	{
		SetFastMove( pChar, pStage );
	}
}

void ioWhirlwndAttack::SetFastMove( ioBaseChar* pChar, ioPlayStage* pStage )
{
	m_SkillState = SS_FAST_MOVE;	
	pChar->SetInvisibleState( true, true );
	pChar->SetSpecialHide( true );

	D3DXVECTOR3 vDistance = m_vAttackPos - pChar->GetWorldPosition(); 
	m_fMaxFastMoveRange = D3DXVec3Length( &vDistance );

	ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_szFastMoveMapEffect, pChar->GetWorldPosition(), D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
	if( pMapEffect && pMapEffect->GetEffect() )
	{
		m_FastMoveMapEffectID = pMapEffect->GetEffect()->GetUniqueID();

		D3DXQUATERNION qtRot;
		ioMath::CalcDirectionQuaternion( qtRot, -m_vAttackDir );
		pMapEffect->SetWorldOrientation( qtRot );
				
	}

}

void ioWhirlwndAttack::CheckFastMoveState( ioBaseChar* pChar, ioPlayStage* pStage )
{
	float fMoveDist		= m_fFastMoveSpeed * g_FrameTimer.GetSecPerFrame();
	m_fCurFastMoveRange	+= fMoveDist;

	if( m_fMaxFastMoveRange < m_fCurFastMoveRange )
	{
		SetFastEnd( pChar, pStage );
	}
	else
	{
		D3DXVECTOR3 vMove	= m_vAttackDir * fMoveDist;
		D3DXVECTOR3 vPos	= pChar->GetWorldPosition() + vMove;

		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_FastMoveMapEffectID );
		if( pMapEffect )
		{
			pMapEffect->SetWorldPosition( vPos );
			pChar->SetWorldPosition( vPos );
		}
	}
}


void ioWhirlwndAttack::SetFastEnd( ioBaseChar* pChar, ioPlayStage* pStage )
{
	m_SkillState			= SS_FAST_MOVE_END;
	m_dwFastMoveEndEndTime	= FRAMEGETTIME();
		
	pChar->EndMapEffect( m_FastMoveMapEffectID, true );
	pChar->SetWorldPosition( m_vAttackPos );

	pChar->SetInvisibleState( false, true );
	pChar->SetSpecialHide( false );
	pChar->EndMapEffect( m_LoopEndAttackEffectID, false );

	//위의 하이드 옵션에 의해서 웨폰메쉬가 다시 나타나기 때문에 다시 무기 웨폰 감추기를 시작
	m_bSetHandMesh = false;
	CheckWeaponItemMesh( pChar );

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_szFastMoveEnd );
	if ( iAniID == -1 )  
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fFastMoveEndRate );
	m_dwFastMoveEndEndTime += pGrp->GetAnimationFullTime( iAniID ) * m_fFastMoveEndRate;
}

void ioWhirlwndAttack::CheckFastEndState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( 0 < m_dwFastMoveEndEndTime && m_dwFastMoveEndEndTime < FRAMEGETTIME() )
	{
		SetWhirlAttack( pChar, pStage );
	}
}

void ioWhirlwndAttack::SetWhirlAttack( ioBaseChar* pChar, ioPlayStage* pStage )
{
	m_SkillState			= SS_WHIRLWND_ATTACK;		
	m_dwWhirlAttackEndTime	= FRAMEGETTIME() + m_dwWhirlAttackDuration;

	pChar->SetSkillMoveEnable( true, false );

	m_dwWhirlAttackIndex = pChar->IncreaseWeaponIndexBase();
	pChar->SkillFireExplicit( m_WhirlAttackInfo, GetName(), m_dwWhirlAttackIndex, pChar->GetMidPositionByRate(), m_vAttackDir );
	
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_szWhirlAttack );
	if( iAniID == -1 )
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/m_fWhirlAttackRate );
}

bool ioWhirlwndAttack::CheckWeaponLive( ioBaseChar* pChar, ioPlayStage* pStage, DWORD dwWeaponIdx )
{
	if( dwWeaponIdx == 0 )
	{
		return false;
	}
	else
	{
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, dwWeaponIdx );
		if( !pWeapon )
		{
			return false;
		}
		else if( pWeapon && !pWeapon->IsLive() )
		{
			return false;
		}
	}

	return true;
}

void ioWhirlwndAttack::CheckWhirlAttackState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( 0 < m_dwWhirlAttackEndTime && m_dwWhirlAttackEndTime < dwCurTime )
	{
		SetChopAttack( pChar, pStage );
	}
	else
	{
		if( CheckWeaponLive( pChar, pStage, m_dwWhirlAttackIndex ) )
		{
			if( pChar->IsNeedProcess() && pChar->IsAttackKey() )
			{
				SetChopAttack( pChar, pStage );
			}
		}
		else
		{
			SetWallAttack( pChar, pStage );
		}		
	}
}

void ioWhirlwndAttack::ProcessCharRotate( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_dwCharRotateTime == 0 )
		return;

	// 캐릭터 회전
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	float fRate = (fTimePerSec*1000) / m_dwCharRotateTime;
	float fRotateAngle = 360.0f * fRate;

	fRotateAngle = ioMath::ArrangeHead( fRotateAngle );

	D3DXQUATERNION	qtNewRot;
	D3DXVECTOR3 vAxis = ioMath::UNIT_Y;

	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fRotateAngle) );

	D3DXQUATERNION qtCharRot = pChar->GetTargetRot();
	D3DXVECTOR3 vDir = qtCharRot * D3DXVECTOR3( 0.0, 0.0, -FLOAT1 );
	vDir = qtNewRot * vDir;

	ioMath::CalcDirectionQuaternion( qtNewRot, -vDir );
	pChar->SetTargetRotToRotate( qtNewRot, true, true );
	pChar->SetCurMoveSpeed( m_fMoveSpeed );
}

void ioWhirlwndAttack::SetChopAttack( ioBaseChar* pChar, ioPlayStage* pStage )
{
	//디폴트 공격 사용
	m_SkillState = SS_CHOP;
	ioAttackSkill::AttackSkillFire( pChar );

	pChar->SetSkillMoveEnable( false, false );
	m_vAttackPos = pChar->GetWorldPosition();

	g_WeaponMgr.CheckAdhesiveWeapon4( pChar->GetCharName() );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << (int)SSS_CHOP;
		kPacket << pChar->GetWorldPosition();
		kPacket << pChar->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioWhirlwndAttack::CheckChopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( 0 < m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
	{		
		m_SkillState = SS_END;
	}
}

void ioWhirlwndAttack::SetWallAttack( ioBaseChar* pChar, ioPlayStage* pStage )
{
	pChar->SetSkillMoveEnable( false, false );
	m_vAttackPos = pChar->GetWorldPosition();

	m_SkillState = SS_WALL;

	//ioWhirlwndAttack::GetAttribute()에서 해당 스테이트에 맞는 공격을 던져줌
	ioAttackSkill::AttackSkillFire( pChar );

	g_WeaponMgr.CheckAdhesiveWeapon4( pChar->GetCharName() );
}

void ioWhirlwndAttack::CheckWallState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( 0 < m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
	{		
		m_SkillState = SS_END;
	}
}

void ioWhirlwndAttack::SetEnd( ioBaseChar* pChar, ioPlayStage* pStage )
{
	m_SkillState = SS_END;

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge( pChar->GetCharName() ) );
		m_bReduceGauge = true;
	}
}

AttackAttribute& ioWhirlwndAttack::GetAttribute()
{
	switch( m_SkillState )
	{
	case SS_LOOP_END_ATTACK:
		return m_LoopEndAttack;
	case SS_WALL:
		return m_WallEndAttack;
	}
	
	return ioAttackSkill::GetAttribute();
}

const AttackAttribute& ioWhirlwndAttack::GetAttributeConst() const
{
	switch( m_SkillState )
	{
	case SS_LOOP_END_ATTACK:
		return m_LoopEndAttack;
	case SS_WALL:
		return m_WallEndAttack;
	}

	return ioAttackSkill::GetAttributeConst();
}

void ioWhirlwndAttack::OnSkillEnd( ioBaseChar *pChar )
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
	pChar->SetSkillMoveEnable( false, false );
	pChar->SetInvisibleState( false, true );
	pChar->SetSpecialHide( false );
	pChar->ClearReserveKeyInput();
	pChar->DestroyMagicCircle();
	pChar->EndMapEffect( m_LoopEndAttackEffectID, false );

	g_WeaponMgr.DestroyReserveAdhesive4( pChar->GetCharName() );

	Init();
}

bool ioWhirlwndAttack::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_PRE:
	case SS_LOOP:
	case SS_LOOP_END_ATTACK:
	case SS_FAST_MOVE_PRE:
	case SS_FAST_MOVE:
	case SS_FAST_MOVE_END:
	case SS_WHIRLWND_ATTACK:
	case SS_CHOP:
	case SS_WALL:
		return false;
	}
	
	return true;
}

bool ioWhirlwndAttack::IsAttackEndState() const
{
	switch( m_SkillState )
	{
	case SS_PRE:
	case SS_LOOP:
	case SS_LOOP_END_ATTACK:
	case SS_FAST_MOVE_PRE:
	case SS_FAST_MOVE:
	case SS_FAST_MOVE_END:
	case SS_WHIRLWND_ATTACK:
	case SS_CHOP:
	case SS_WALL:
		return false;
	}

	return true;
}

bool ioWhirlwndAttack::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioWhirlwndAttack::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioAttackSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioTraceWeapon7* pTraceWeapon = ToTraceWeapon7( pWeapon );
	if( pTraceWeapon )
	{
		pTraceWeapon->SetMoveDir( m_vAttackDir );
	}

	ioAdhesiveMissileWeapon4 *pAdhesive4 = ToAdhesiveMissileWeapon4( pWeapon );
	if( pAdhesive4 )
	{
		pAdhesive4->SetState( ioAdhesiveMissileWeapon4::WS_ATTACH_MOVE );
		pAdhesive4->SebCollisionMapForSkill( false );
	}

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( false, false );
			pZone->SetStartPosition( m_vAttackPos, pStage );
		}
	}
}

void ioWhirlwndAttack::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int eState;
	rkPacket >> eState;

	D3DXVECTOR3 vPos;
	rkPacket >> vPos;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	pChar->SetTargetRotToRotate( qtRot, true, true );
	pChar->SetWorldPosition( vPos );

	if( eState == SSS_LOOP_END_ATTACK )
	{
		rkPacket >> m_vAttackPos;
		rkPacket >> m_vAttackDir;

		SetLoopEndAttackState( pChar, pStage, false );
	}
	else if( eState == SSS_CHOP )
	{
		SetChopAttack( pChar, pStage );
	}
}

bool ioWhirlwndAttack::IsSkipState() const
{
	if( m_SkillState == SS_FAST_MOVE )
		return true;

	return false;
}

bool ioWhirlwndAttack::IsNoDropState() const
{
	if( m_SkillState == SS_FAST_MOVE )
		return true;

	return false;
}

bool ioWhirlwndAttack::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	// 스킬 무적 파괴
	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{		
		if( iDefenseBreakType == m_ProtectCancelType )
			return false;
	}

	//풀무적
	return true;
}