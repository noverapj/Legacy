

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioFantasticWhipAttack.h"

#include "FindPredicateImpl.h"

#include "ioUroborusWeapon4.h"
#include "ioOwnerAttackWeapon.h"
#include "ioZoneEffectWeapon.h"

ioFantasticWhipAttack::ioFantasticWhipAttack()
{
	Init();
}

ioFantasticWhipAttack::ioFantasticWhipAttack( const ioFantasticWhipAttack &rhs )
	: ioCountSkill( rhs ),
	m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	m_LoopAnimation( rhs.m_LoopAnimation ),
	m_fLoopAnimationRate( rhs.m_fLoopAnimationRate ),
	m_dwLoopDuration( rhs.m_dwLoopDuration ),
	m_dwKeyEnableTime( rhs.m_dwKeyEnableTime ),
	m_TargetBuffNameList( rhs.m_TargetBuffNameList ),
	m_MoveAnimation( rhs.m_MoveAnimation ),
	m_fMoveAnimationRate( rhs.m_fMoveAnimationRate ),
	m_fMoveSpeed( rhs.m_fMoveSpeed ),
	m_WhipSlidingAttack( rhs.m_WhipSlidingAttack ),
	m_WhipSlidingJumpAttack( rhs.m_WhipSlidingJumpAttack ),
	m_WhipPullAttack( rhs.m_WhipPullAttack ),
	m_WhipPullJumpAttack( rhs.m_WhipPullJumpAttack ),
	m_WhipEdgeAttack( rhs.m_WhipEdgeAttack ),
	m_WhipEdgeJumpAttack( rhs.m_WhipEdgeJumpAttack ),
	m_fZoneOffset( rhs.m_fZoneOffset )
{
	Init();
}

ioFantasticWhipAttack::~ioFantasticWhipAttack()
{
}

void ioFantasticWhipAttack::Init()
{
	m_TargetName.Clear();

	m_SkillState	= SS_PRE;
	m_EndJumpType	= SEJT_NOT_JUMP;

	m_fCurrMoveAmt	= 0.0f;
	m_fMaxMoveAmt	= 0.0f;

	m_vTargetDir	= ioMath::VEC3_ZERO;
	m_vTargetPos	= ioMath::VEC3_ZERO;
	m_vMoveDir		= ioMath::VEC3_ZERO;
}

void ioFantasticWhipAttack::LoadProperty( ioINILoader &rkLoader )
{
	ioCountSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	m_dwSkillProtectTime = rkLoader.LoadInt_e( "skill_protect_time", 0 );

	//Loop	
	rkLoader.LoadString_e( "loop_animation", "", szBuf, MAX_PATH );
	m_LoopAnimation			= szBuf;
	m_fLoopAnimationRate	= rkLoader.LoadFloat_e( "loop_animation_rate", FLOAT1 );
	m_dwLoopDuration		= rkLoader.LoadInt_e( "loop_duration", 0 );
	m_dwKeyEnableTime		= rkLoader.LoadInt_e( "key_enable_time", 0 );

	//Target
	int iMax = rkLoader.LoadInt_e( "target_buff_max", 0 );
	for( int i = 0; i < iMax; ++i )
	{
		sprintf_e( szKey, "target_buff%d", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_TargetBuffNameList.push_back( szBuf );
	}

	//Move
	rkLoader.LoadString_e( "move_animation", "", szBuf, MAX_PATH );
	m_MoveAnimation			= szBuf;
	m_fMoveAnimationRate	= rkLoader.LoadFloat_e( "move_animation_rate", FLOAT1 );
	m_fMoveSpeed			= rkLoader.LoadFloat_e( "move_speed", FLOAT1 );
	
	//Offset
	m_fZoneOffset			= rkLoader.LoadFloat_e( "zone_offset", FLOAT1 );

	//
	LoadAttackAttribute( "whip_sliding_attack", m_WhipSlidingAttack, rkLoader );
	LoadAttackAttribute( "whip_sliding_jump_attack", m_WhipSlidingJumpAttack, rkLoader );

	LoadAttackAttribute( "whip_pull_attack", m_WhipPullAttack, rkLoader );
	LoadAttackAttribute( "whip_pull_jump_attack", m_WhipPullJumpAttack, rkLoader );

	LoadAttackAttribute( "whip_edge_attack", m_WhipEdgeAttack, rkLoader );
	LoadAttackAttribute( "whip_edge_jump_attack", m_WhipEdgeJumpAttack, rkLoader );
}

ioSkill* ioFantasticWhipAttack::Clone()
{
	return new ioFantasticWhipAttack( *this );		
}

bool ioFantasticWhipAttack::IsCanUseSkill( ioBaseChar *pOwner, bool bHelpMsg )
{
	ioAttackableItem *pAttackable = ToAttackableItem( GetOwnerItem() );
	if( pAttackable )
	{
		ioHashString szSkillMotion = GetSkillMotionName();

		if( !pAttackable->IsCanFire( pOwner ) )
			return false;
	}

	if( m_iCurUseCount < m_iNeedCount )
		return false;

	return true;
}

bool ioFantasticWhipAttack::OnSkillStart( ioBaseChar *pOwner )
{
	if( !ioCountSkill::OnSkillStart(pOwner) )
		return false;

	RemoveWeapon( pOwner );
	RemoveBuff( pOwner );

	Init();

	if( m_bUseItemMesh )
		pOwner->ShowWeaponItemMesh( false );

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pOwner->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	m_vTargetPos	= pOwner->GetMidPositionByRate();
	m_vTargetDir	= pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );	
	D3DXVec3Normalize(&m_vTargetDir, &m_vTargetDir );
	m_vMoveDir		= m_vTargetDir;

	return true;
}

bool ioFantasticWhipAttack::HasBuff( ioBaseChar* pTarget )
{
	if( pTarget == NULL )
		return false;

	for( ioHashStringVec::iterator iter = m_TargetBuffNameList.begin(); iter != m_TargetBuffNameList.end(); ++iter )
	{
		const ioHashString& TargetBuff = *iter;
		if( pTarget->HasBuff( *iter ) )
		{
			return true;
		}
	}

	return false;
}

void ioFantasticWhipAttack::SetTarget( const ioHashString& TargetName, ioBaseChar* pOwner )
{	
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( !m_TargetName.IsEmpty() )
		return;

	//발사한 ioUroborusWeapon4 웨폰이 적군을 피격한 경우
	ioBaseChar* pTarget = pOwner->GetBaseChar( TargetName );
	if( pTarget && HasBuff( pTarget ) )
	{
		m_TargetName = TargetName;
		m_vTargetPos = pOwner->GetWorldPosition();

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pOwner->GetCharName();
			kPacket << GetName();
			kPacket << (byte)SSS_TARGET;
			kPacket << m_TargetName;
			kPacket << m_vTargetPos;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	if( m_SkillState == SS_NORMAL_ATTACK || m_SkillState == SS_JUMP_ATTACK )
	{
		SetSkillLoopState( pOwner, pOwner->GetCreator() );
	}	
}

ioBaseChar* ioFantasticWhipAttack::FindTarget( ioBaseChar* pOwner )
{
	FD_AutoTarget kFindPred( pOwner, m_fAutoTargetMinAngle, m_fAutoTargetMaxAngle, m_fAutoTargetRange );
	return pOwner->GetCreator()->FindUser( kFindPred );	
}

void ioFantasticWhipAttack::OnProcessState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	switch( m_SkillState )
	{
	case SS_PRE: 
		{
			CheckPreState( pOwner, pStage );
		}
		break;
	case SS_NORMAL_ATTACK:
	case SS_JUMP_ATTACK:
		{
			CheckAttackState( pOwner, pStage );
		}
		break;
	case SS_LOOP:
		{
			CheckLoopState( pOwner, pStage );
		}
		break;
	case SS_MOVE:
		{
			ProcessMove( pOwner, pStage, g_FrameTimer.GetSecPerFrame() );
			CheckMoveState( pOwner, pStage );
		}
		break;
	case SS_WHIP_SLIDING_ATTACK:
	case SS_WHIP_SLIDING_JUMP_ATTACK:
	case SS_WHIP_PULL_ATTACK:
	case SS_WHIP_PULL_JUMP_ATTACK:
	case SS_WHIP_EDGE_ATTACK:
	case SS_WHIP_EDGE_JUMP_ATTACK:
		{
			CheckWhipState( pOwner, pStage );
		}
		break;
	}
}

void ioFantasticWhipAttack::CheckPreState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	DWORD dwCurTime  = FRAMEGETTIME();
	DWORD dwPreDelay = GetPreDelayTime();

	if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
	{
		m_dwPreDelayStartTime = 0;
		EndPreDelayEffect( pOwner );

		if( pOwner->IsNeedProcess() )
		{
			if( GetSkillUseType() == SUT_JUMP )
			{
				SetJumpAttackState( pOwner, pStage );
			}
			else
			{
				SetNormalAttackState( pOwner, pStage );
			}
		}
	}
}

void ioFantasticWhipAttack::SetNormalAttackState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	m_SkillState = SS_NORMAL_ATTACK;
	ioCountSkill::AttackSkillFire( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << (byte)SSS_NORMAL_ATTACK;
		kPacket << pOwner->GetWorldOrientation();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioFantasticWhipAttack::CheckAttackState( ioBaseChar *pOwner, ioPlayStage *pStage )
{	
	if( 0 < m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		if( !CheckLiveWeapon( pOwner ) )
		{
			if( pOwner->IsNeedProcess() )
			{
				SetEndState( pOwner, pStage );
			}
		}
		else
		{
			if( pOwner->IsNeedProcess() )
			{
				SetSkillLoopState( pOwner, pStage );
			}
		}
	}
}

void ioFantasticWhipAttack::SetJumpAttackState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	m_SkillState = SS_JUMP_ATTACK;
	ioCountSkill::AttackSkillFire( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << (byte)SSS_JUMP_ATTACK;
		kPacket << pOwner->GetWorldOrientation();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioFantasticWhipAttack::SetSkillLoopState( ioBaseChar* pOwner, ioPlayStage* pStage )
{
	m_SkillState		= SS_LOOP;
	m_dwLoopStartTime	= FRAMEGETTIME();

	if( m_LoopAnimation.IsEmpty() || !pOwner )
	{
		return;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_LoopAnimation );
	if( iAniID == -1 )
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/m_fLoopAnimationRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << (byte)SSS_LOOP;
		kPacket << pOwner->GetWorldOrientation();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioFantasticWhipAttack::CheckLoopState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	for( DWORDVec::iterator iter = m_ActiveWeaponList.begin(); iter != m_ActiveWeaponList.end(); ++iter )
	{
		ioUroborusWeapon4 *pWeapon = ToUroborusWeapon4( g_WeaponMgr.FindWeapon( pOwner, *iter ) );		
		if( pWeapon )
		{
			m_vTargetPos = pWeapon->GetPosition();
		}
	}

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwLoopStartTime + m_dwLoopDuration < dwCurTime )
	{	
		if( pOwner->IsNeedProcess() )
		{
			SetEndState( pOwner, pStage );
		}
	}
	else
	{		
		if( pOwner->IsAttackKey() && m_dwLoopStartTime + m_dwKeyEnableTime < dwCurTime )
		{
			if( pOwner->IsNeedProcess() )
			{
				if( GetSkillUseType() == SUT_JUMP )
				{
					SetWhipEdgeJumpAttackState( pOwner, pStage );
				}
				else
				{
					SetWhipEdgeAttackState( pOwner, pStage );
				}
			}
		}
		else if( pOwner->IsDefenseKey() && m_dwLoopStartTime + m_dwKeyEnableTime < dwCurTime )
		{
			if( pOwner->IsNeedProcess() )
			{
				if( GetSkillUseType() == SUT_JUMP )
				{
					SetWhipPullJumpAttackState( pOwner, pStage );
				}
				else
				{
					SetWhipPullAttackState( pOwner, pStage );
				}
			}
		}
		else if( pOwner->IsJumpKey() && m_dwLoopStartTime + m_dwKeyEnableTime < dwCurTime )
		{
			if( pOwner->IsNeedProcess() )
			{
				SetMoveState( pOwner, pStage );
			}
		}
		else
		{
			if( !CheckLiveWeapon( pOwner ) )
			{
				if( pOwner->IsNeedProcess() )
				{
					SetEndState( pOwner, pStage );
				}
			}
		}
	}
}

void ioFantasticWhipAttack::SetMoveState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	m_SkillState = SS_MOVE;

	if( m_MoveAnimation.IsEmpty() || !pOwner )
	{
		return;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_MoveAnimation );
	if( iAniID == -1 )
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/m_fMoveAnimationRate );

	for( DWORDVec::iterator iter = m_ActiveWeaponList.begin(); iter != m_ActiveWeaponList.end(); ++iter )
	{
		ioUroborusWeapon4 *pWeapon = ToUroborusWeapon4( g_WeaponMgr.FindWeapon( pOwner, *iter ) );		
		if( pWeapon )
		{
			D3DXVECTOR3 vLength = pWeapon->GetPosition() - pOwner->GetWorldPosition();
			m_fMaxMoveAmt = D3DXVec3Length( &vLength );
		}
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << (byte)SSS_MOVE;
		kPacket << pOwner->GetWorldOrientation();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioFantasticWhipAttack::ProcessMove( ioBaseChar* pOwner, ioPlayStage* pStage, float fTimePerSec )
{
	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt;
		float fFinalMoveAmt = m_fMoveSpeed * fDivideTimePerSec;

		// 감속이 심해서 음수가 나올 경우는 제외시킴
		if( fFinalMoveAmt > 0.0f )
		{
			vMoveAmt	= m_vMoveDir * fFinalMoveAmt;
			vTotalMove	+= vMoveAmt;
			bCurMove	= true;
		}
	}

	if( bCurMove )
	{
		float fCurLength = D3DXVec3Length( &vTotalMove );
		m_fCurrMoveAmt += fCurLength;

		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( pOwner, &vTotalMove, bCol ) )
		{
			float fPreHeight = pOwner->GetBottomHeight();
			pOwner->Translate( vTotalMove );

				// 빠른 ForceMove로 경사면을 통과하는 경우에 대한 보강
			if( pOwner->GetForceAmt() != 0.0f )
			{
				float fMoveHeight = pOwner->GetBottomHeight();
				if( fPreHeight > 0.0f && fMoveHeight <= FLOAT1 )
				{
					D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
					float fTopHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate( FLOAT1 ) );
					if( fTopHeight > FLOAT1 )
					{
						vCurPos.y = max( fTopHeight, fPreHeight );
						pOwner->SetWorldPosition( vCurPos );
					}
				}
			}
		}

		if( bCol )
		{
			if( GetSkillUseType() == SUT_JUMP )
			{
				SetWhipSlidingJumpAttackState( pOwner, pStage );
			}
			else
			{
				SetWhipSlidingAttackState( pOwner, pStage );
			}
		}
	}

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	if( pStage && pOwner->IsAxisLimit() )
	{
		pStage->CheckAxisLimit( &vPos );
		pOwner->SetWorldPosition( vPos );
	}
}

void ioFantasticWhipAttack::CheckMoveState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if( m_fMaxMoveAmt <= m_fCurrMoveAmt )
	{
		if( GetSkillUseType() == SUT_JUMP )
		{
			SetWhipSlidingJumpAttackState( pOwner, pStage );
		}
		else
		{
			SetWhipSlidingAttackState( pOwner, pStage );
		}
	}
}

void ioFantasticWhipAttack::SetWhipSlidingAttackState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	m_SkillState = SS_WHIP_SLIDING_ATTACK;
	ioCountSkill::AttackSkillFire( pOwner );
	RemoveWeapon( pOwner );

	ioBaseChar* pTarget = pOwner->GetBaseChar( m_TargetName );
	if( pTarget )
	{
		float fBottomHeight = pTarget->GetBottomHeight();
		if( 5.0f < pTarget->GetWorldPosition().y - fBottomHeight )
		{
			m_EndJumpType = SEJT_JUMP;
		}
		else
		{
			m_EndJumpType = SEJT_NOT_JUMP;
		}
	}
	else
	{
		if( GetSkillUseType() == SUT_JUMP || pOwner->IsCharDropZonePos() )
		{
			m_EndJumpType = SEJT_JUMP;
		}
		else
		{
			m_EndJumpType = SEJT_NOT_JUMP;
		}
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << (byte)SSS_WHIP_SLIDING_ATTACK;
		kPacket << pOwner->GetWorldOrientation();
		kPacket << pOwner->GetWorldPosition();
		kPacket << (byte)m_EndJumpType;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioFantasticWhipAttack::SetWhipSlidingJumpAttackState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	m_SkillState = SS_WHIP_SLIDING_JUMP_ATTACK;	
	ioCountSkill::AttackSkillFire( pOwner );
	RemoveWeapon( pOwner );

	ioBaseChar* pTarget = pOwner->GetBaseChar( m_TargetName );
	if( pTarget )
	{
		float fBottomHeight = pTarget->GetBottomHeight();
		if( FLOAT10 < pTarget->GetWorldPosition().y - fBottomHeight )
		{
			m_EndJumpType = SEJT_JUMP;
		}
		else
		{
			m_EndJumpType = SEJT_NOT_JUMP;
		}
	}
	else
	{
		if( GetSkillUseType() == SUT_JUMP || pOwner->IsCharDropZonePos() )
		{
			m_EndJumpType = SEJT_JUMP;
		}
		else
		{
			m_EndJumpType = SEJT_NOT_JUMP;
		}
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << (byte)SSS_WHIP_SLIDING_JUMP_ATTACK;
		kPacket << pOwner->GetWorldOrientation();
		kPacket << pOwner->GetWorldPosition();
		kPacket << (byte)m_EndJumpType;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioFantasticWhipAttack::SetWhipPullAttackState( ioBaseChar *pOwner, ioPlayStage *pStage )
{	
	m_SkillState	= SS_WHIP_PULL_ATTACK;	
	m_EndJumpType	= SEJT_NOT_JUMP;

	ioCountSkill::AttackSkillFire( pOwner );
	RemoveWeapon( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << (byte)SSS_WHIP_PULL_ATTACK;
		kPacket << pOwner->GetWorldOrientation();
		kPacket << pOwner->GetWorldPosition();		
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioFantasticWhipAttack::SetWhipPullJumpAttackState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	m_SkillState	= SS_WHIP_PULL_JUMP_ATTACK;	
	m_EndJumpType	= SEJT_JUMP;

	ioCountSkill::AttackSkillFire( pOwner );
	RemoveWeapon( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << (byte)SSS_WHIP_PULL_JUMP_ATTACK;
		kPacket << pOwner->GetWorldOrientation();
		kPacket << pOwner->GetWorldPosition();		
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioFantasticWhipAttack::SetWhipEdgeAttackState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	m_SkillState	= SS_WHIP_EDGE_ATTACK;	
	m_EndJumpType	= SEJT_NOT_JUMP;

	ioCountSkill::AttackSkillFire( pOwner );
	RemoveWeapon( pOwner );

	

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << (byte)SSS_WHIP_EDGE_ATTACK;
		kPacket << pOwner->GetWorldOrientation();
		kPacket << pOwner->GetWorldPosition();		
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioFantasticWhipAttack::SetWhipEdgeJumpAttackState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	m_SkillState	= SS_WHIP_EDGE_JUMP_ATTACK;	
	m_EndJumpType	= SEJT_JUMP;

	ioCountSkill::AttackSkillFire( pOwner );
	RemoveWeapon( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << (byte)SSS_WHIP_EDGE_JUMP_ATTACK;
		kPacket << pOwner->GetWorldOrientation();
		kPacket << pOwner->GetWorldPosition();		
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioFantasticWhipAttack::CheckWhipState( ioBaseChar *pOwner, ioPlayStage *pStage )
{	
	if( 0 < m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		if( pOwner->IsNeedProcess() )
		{
			SetEndState( pOwner, pStage, false );
		}
	}
}

void ioFantasticWhipAttack::SetEndState( ioBaseChar *pOwner, ioPlayStage *pStage, bool bCheckJump /* = true */ )
{
	m_SkillState = SS_END;

	pOwner->SetCurMoveSpeed( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetExtraMoveSpeed( 0.0f );

	if( bCheckJump )
	{
		if( GetSkillUseType() == SUT_JUMP )
		{
			m_EndJumpType = SEJT_JUMP;
		}
		else
		{
			m_EndJumpType = SEJT_NOT_JUMP;
		}
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << (byte)SSS_END;
		kPacket << pOwner->GetWorldOrientation();
		kPacket << pOwner->GetWorldPosition();		
		kPacket << bCheckJump;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioFantasticWhipAttack::OnSkillEnd( ioBaseChar *pOwner )
{
	ioCountSkill::OnSkillEnd( pOwner );

	pOwner->SetSkillMoveEnable( false, false );

	if( m_bUseItemMesh )
		pOwner->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pOwner->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );


	RemoveWeapon( pOwner );
	RemoveBuff( pOwner );

	Init();
}

bool ioFantasticWhipAttack::CheckLiveWeapon( ioBaseChar *pOwner )
{
	for( DWORDVec::iterator iter = m_ActiveWeaponList.begin(); iter != m_ActiveWeaponList.end(); ++iter )
	{
		ioUroborusWeapon4 *pWeapon = ToUroborusWeapon4( g_WeaponMgr.FindWeapon( pOwner, *iter ) );		
		if( pWeapon )
			return true;
	}

	return false;
}

void ioFantasticWhipAttack::RemoveWeapon( ioBaseChar *pOwner )
{
	for( DWORDVec::iterator iter = m_ActiveWeaponList.begin(); iter != m_ActiveWeaponList.end(); ++iter )
	{
		ioUroborusWeapon4 *pWeapon = ToUroborusWeapon4( g_WeaponMgr.FindWeapon( pOwner, *iter ) );		
		if( pWeapon )
			pWeapon->SetEndState( false, false, false, true );
	}
	m_ActiveWeaponList.clear();
}

void ioFantasticWhipAttack::RemoveBuff( ioBaseChar *pOwner )
{
	if( pOwner )
	{
		ioBaseChar* pTarget = pOwner->GetBaseChar( m_TargetName );
		if( pTarget )
		{
			for( ioHashStringVec::iterator iter = m_TargetBuffNameList.begin(); iter != m_TargetBuffNameList.end(); ++iter )
			{
				const ioHashString& TargetBuff = *iter;
				if( pTarget->HasBuff( *iter ) )
				{
					pTarget->SetReserveEndBuff( TargetBuff );
				}
			}
		}
	}
}

AttackAttribute& ioFantasticWhipAttack::GetAttribute()
{
	switch( m_SkillState )
	{
	case SS_WHIP_SLIDING_ATTACK:
		return m_WhipSlidingAttack;
	case SS_WHIP_SLIDING_JUMP_ATTACK:
		return m_WhipSlidingJumpAttack;
	case SS_WHIP_PULL_ATTACK:
		return m_WhipPullAttack;
	case SS_WHIP_PULL_JUMP_ATTACK:
		return m_WhipPullJumpAttack;
	case SS_WHIP_EDGE_ATTACK:
		return m_WhipEdgeAttack;
	case SS_WHIP_EDGE_JUMP_ATTACK:
		return m_WhipEdgeJumpAttack;
	}

	return ioCountSkill::GetAttribute();
}

const AttackAttribute& ioFantasticWhipAttack::GetAttributeConst() const
{
	switch( m_SkillState )
	{
	case SS_WHIP_SLIDING_ATTACK:
		return m_WhipSlidingAttack;
	case SS_WHIP_SLIDING_JUMP_ATTACK:
		return m_WhipSlidingJumpAttack;
	case SS_WHIP_PULL_ATTACK:
		return m_WhipPullAttack;
	case SS_WHIP_PULL_JUMP_ATTACK:
		return m_WhipPullJumpAttack;
	case SS_WHIP_EDGE_ATTACK:
		return m_WhipEdgeAttack;
	case SS_WHIP_EDGE_JUMP_ATTACK:
		return m_WhipEdgeJumpAttack;
	}

	return ioCountSkill::GetAttributeConst();
}

bool ioFantasticWhipAttack::IsEndState() const
{
	if( 0.0f < m_fSkillEndNextJumpAmt && m_EndJumpType == SEJT_JUMP )
		return false;

	if( m_SkillState == SS_END )
		return true;

	return false;
}

bool ioFantasticWhipAttack::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	if( 0.0f < m_fSkillEndNextJumpAmt && m_EndJumpType == SEJT_JUMP )
		return false;

	if( m_SkillState == SS_END )
		return true;

	return false;
}

bool ioFantasticWhipAttack::IsAttackEndState() const
{
	if( m_fSkillEndNextJumpAmt == 0.0f )
		return false;

	if( m_EndJumpType == SEJT_NOT_JUMP )
		return false;

	if( m_SkillState == SS_END )
		return true;

	return false;
}

SkillCharColType ioFantasticWhipAttack::IsCharSkipState() const
{
	if( m_SkillState == SS_MOVE )
		return SCCT_TEAM;
	
	return ioCountSkill::IsCharSkipState();
}

bool ioFantasticWhipAttack::IsNoDropState() const
{
	switch( m_SkillState )
	{
	case SS_PRE:
	case SS_JUMP_ATTACK:
	case SS_LOOP:
	case SS_MOVE:
	case SS_WHIP_PULL_ATTACK:
	case SS_WHIP_PULL_JUMP_ATTACK:
	case SS_WHIP_EDGE_ATTACK:
	case SS_WHIP_EDGE_JUMP_ATTACK:
	case SS_WHIP_SLIDING_ATTACK:
	case SS_WHIP_SLIDING_JUMP_ATTACK:
	case SS_END:
		return true;
	}

	return false;
}

bool ioFantasticWhipAttack::IsEnableReserve() const
{
	return false;
}

SkillEndJumpType ioFantasticWhipAttack::GetSkillEndNextJumpMustEnable()
{
	return m_EndJumpType;
}

bool ioFantasticWhipAttack::IsCanSendControl() const
{
	if( m_SkillState == SS_MOVE )
		return false;

	return true;
}

#define POS_ROT_PACKET( pChar ) if( pChar != NULL ){ rkPacket >> qtRot; rkPacket >> vPos; pChar->SetTargetRot( qtRot ); pChar->SetWorldPosition( vPos ); }

void ioFantasticWhipAttack::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar )
		return;

	D3DXQUATERNION qtRot;
	D3DXVECTOR3 vPos;

	byte eState;
	rkPacket >> eState;

	switch( eState )
	{
	case SSS_NORMAL_ATTACK:
		{
			POS_ROT_PACKET( pChar );
			SetNormalAttackState( pChar, pChar->GetCreator() );
		}
		break;
	case SSS_JUMP_ATTACK:
		{
			POS_ROT_PACKET( pChar );
			SetJumpAttackState( pChar, pChar->GetCreator() );
		}
		break;
	case SSS_TARGET:
		{
			rkPacket >> m_TargetName;
			rkPacket >> m_vTargetPos;
		}
		break;
	case SSS_LOOP:
		{
			POS_ROT_PACKET( pChar );
			SetSkillLoopState( pChar, pStage );
		}
		break;
	case SSS_MOVE:
		{
			POS_ROT_PACKET( pChar );
			SetMoveState( pChar, pStage );
		}
		break;
	case SSS_WHIP_EDGE_ATTACK:
		{
			POS_ROT_PACKET( pChar );
			SetWhipEdgeAttackState( pChar, pStage );
		}
		break;
	case SSS_WHIP_EDGE_JUMP_ATTACK:
		{
			POS_ROT_PACKET( pChar );
			SetWhipEdgeJumpAttackState( pChar, pStage );
		}
		break;
	case SSS_WHIP_PULL_ATTACK:
		{
			POS_ROT_PACKET( pChar );
			SetWhipPullAttackState( pChar, pStage );
		}
		break;
	case SSS_WHIP_PULL_JUMP_ATTACK:
		{
			POS_ROT_PACKET( pChar );
			SetWhipPullJumpAttackState( pChar, pStage );
		}
		break;
	case SSS_WHIP_SLIDING_ATTACK:
		{
			POS_ROT_PACKET( pChar );
			byte eJumpType;
			rkPacket >> eJumpType;
			m_EndJumpType = (SkillEndJumpType)eJumpType;

			SetWhipSlidingAttackState( pChar, pStage );
		}
		break;
	case SSS_WHIP_SLIDING_JUMP_ATTACK:
		{
			POS_ROT_PACKET( pChar );
			byte eJumpType;
			rkPacket >> eJumpType;
			m_EndJumpType = (SkillEndJumpType)eJumpType;

			SetWhipSlidingJumpAttackState( pChar, pStage );
		}
		break;
	case SSS_END:
		{
			POS_ROT_PACKET( pChar );
			bool bCheckJump;
			rkPacket >> bCheckJump;
			SetEndState( pChar, pStage, bCheckJump );
		}
		break;
	}
}

void ioFantasticWhipAttack::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	SetChargingRateForWeapon( pWeapon );

	ioUroborusWeapon4 *pUroborus = ToUroborusWeapon4( pWeapon );
	if( pUroborus )
	{
		ioBaseChar* pTarget = FindTarget( pWeapon->GetOwner() );
		if( pTarget )
		{
			D3DXVECTOR3 vPos	= pTarget->GetWorldPosition();
			vPos.y				= pTarget->GetMidHeightByRate();
			m_vTargetDir		= vPos - pUroborus->GetPosition();
			D3DXVec3Normalize( &m_vTargetDir, &m_vTargetDir );

			m_vMoveDir		= pTarget->GetWorldPosition() - pWeapon->GetOwner()->GetWorldPosition();
			D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );			
			pUroborus->SetMoveDir( m_vTargetDir );
		}		
		m_ActiveWeaponList.push_back( pWeapon->GetWeaponIndex() );
	}

	ioOwnerAttackWeapon *pOwnerWeapon = ToOwnerAttackWeapon( pWeapon );
	if( pOwnerWeapon )
	{
		pOwnerWeapon->SetTargetName( m_TargetName, pStage );
	}

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone && pZone->GetOwner() )
	{
		D3DXVECTOR3 vOffset = m_vMoveDir * m_fZoneOffset;
		pZone->SetStartPosition( m_vTargetPos + vOffset, pStage );
		pZone->SetCollisionDir( m_vMoveDir );
	}
}

bool ioFantasticWhipAttack::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	// 스킬 무적 파괴
	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{
		if( iDefenseBreakType == m_ProtectCancelType )
			return false;
	}

	if( IsPreDelayProtect() )
	{
		if( m_dwPreDelayStartTime != 0 )
			return true;
	}

	if( 0 < m_dwSkillProtectTime && FRAMEGETTIME() < m_dwSkillStartTime + m_dwSkillProtectTime )
		return true;

	return false;
}