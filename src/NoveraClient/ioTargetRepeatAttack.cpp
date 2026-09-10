
#include "stdafx.h"

#include "ioTargetRepeatAttack.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioTargetRepeatAttack::ioTargetRepeatAttack()
{
	Init();
}

ioTargetRepeatAttack::ioTargetRepeatAttack( const ioTargetRepeatAttack &rhs )
	: ioAttackSkill( rhs ),
	  m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	  m_TargetAttack( rhs.m_TargetAttack ),
	  m_nMaxRepeatCnt( rhs.m_nMaxRepeatCnt ),
	  m_vRepeatAttackList( rhs.m_vRepeatAttackList ),
	  m_szDelayLoopAni( rhs.m_szDelayLoopAni ),
	  m_fDelayLoopAniRate( rhs.m_fDelayLoopAniRate ),
	  m_szMoveLoopAni( rhs.m_szMoveLoopAni ),
	  m_fMoveLoopAniRate( rhs.m_fMoveLoopAniRate ),
	  m_fTargetFindScaleSpeedRate( rhs.m_fTargetFindScaleSpeedRate ),
	  m_fTargetFindScaleMaxRate( rhs.m_fTargetFindScaleMaxRate ),
	  m_TargetFindEffect( rhs.m_TargetFindEffect ),
	  m_fTargetFindEffectHeightOffset( rhs.m_fTargetFindEffectHeightOffset ),
	  m_fTargetAngle( rhs.m_fTargetAngle ),
	  m_fTargetMinRange( rhs.m_fTargetMinRange ),
	  m_fTargetMaxRange( rhs.m_fTargetMaxRange ),
	  m_fTargetRangeRate( rhs.m_fTargetRangeRate ),
	  m_bEnableTargetDown( rhs.m_bEnableTargetDown ),
	  m_szTargetBuff( rhs.m_szTargetBuff ),
	  m_dwFindLoopTime( rhs.m_dwFindLoopTime ),
	  m_fFindMoveSpeedRate( rhs.m_fFindMoveSpeedRate )
{
	Init();
}

ioTargetRepeatAttack::~ioTargetRepeatAttack()
{
	m_vRepeatAttackList.clear();
}

void ioTargetRepeatAttack::LoadProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];

	ioAttackSkill::LoadProperty( rkLoader );
		
	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	rkLoader.LoadString_e( "delay_loop_ani", "", szBuf, MAX_PATH );
	m_szDelayLoopAni = szBuf;
	m_fDelayLoopAniRate = rkLoader.LoadFloat_e( "delay_loop_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "move_loop_ani", "", szBuf, MAX_PATH );
	m_szMoveLoopAni = szBuf;
	m_fMoveLoopAniRate = rkLoader.LoadFloat_e( "move_loop_ani_rate", FLOAT1 );

	LoadAttackAttribute_e( "target_attack", m_TargetAttack, rkLoader );

	m_vRepeatAttackList.clear();
	m_nMaxRepeatCnt = rkLoader.LoadInt_e( "repeat_attack_cnt", 0 );
	for ( int i=0; i<m_nMaxRepeatCnt; i++ )
	{
		wsprintf( szBuf, "repeat_attack%d", i+1 );

		AttackAttribute sAttackInfo;
		LoadAttackAttribute( szBuf, sAttackInfo, rkLoader );
		m_vRepeatAttackList.push_back( sAttackInfo );
	}

	m_fTargetFindScaleSpeedRate	= rkLoader.LoadFloat_e( "target_find_effect_scale_rate", FLOAT1 );
	m_fTargetFindScaleMaxRate = rkLoader.LoadFloat_e( "target_find_effect_scale_max_rate", 4.0f );

	rkLoader.LoadString_e( "target_find_effect", "", szBuf, MAX_PATH );
	m_TargetFindEffect = szBuf;

	m_fTargetFindEffectHeightOffset	= rkLoader.LoadFloat_e( "target_find_effect_height_offset", FLOAT100 );
	
	m_fTargetMinRange = rkLoader.LoadFloat_e( "target_find_min_range", 0.0f );
	m_fTargetMaxRange = rkLoader.LoadFloat_e( "target_find_max_range", 0.0f );
	m_fTargetRangeRate = rkLoader.LoadFloat_e( "target_find_range_rate", 0.0f );
	m_fTargetAngle = rkLoader.LoadFloat_e( "target_find_angle", 0.0f );
	m_bEnableTargetDown = rkLoader.LoadBool_e( "enable_down_target", false );

	rkLoader.LoadString_e( "target_buff", "", szBuf, MAX_PATH );
	m_szTargetBuff = szBuf;

	m_dwFindLoopTime = rkLoader.LoadInt_e( "find_loop_time", 0 );

	m_fFindMoveSpeedRate = rkLoader.LoadFloat_e( "find_move_speed_rate", 0.0f );
}

ioSkill* ioTargetRepeatAttack::Clone()
{
	return new ioTargetRepeatAttack( *this );
}

bool ioTargetRepeatAttack::OnSkillStart( ioBaseChar *pChar )
{	
	Init();
	
	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = FRAMEGETTIME() + m_dwSkillProtectTime;
		
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	SetCameraBuff( pChar );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( false );

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}	
	
	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	return true;
}

void ioTargetRepeatAttack::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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
				SetFindTargetState( pChar, pStage );
			}
		}
		break;
	case SS_FIND_TARGET:
		{			
			if ( m_dwCheckFindLoopTime < dwCurTime )
			{
				SetEndState( pChar );
				return;
			}

			UpdateTargetEffect( pChar, pStage );
			FindTarget( pChar, pStage );

			CheckDelayMoveAni( pChar );
			if ( CheckKeyInput( pChar ) )
			{
				SetTargetAttackState( pChar, pStage );
				return;
			}
		}
		break;
	case SS_TARGET_ATTACK:
		{
			if ( m_dwMotionEndTime < dwCurTime )
				SetEndState( pChar );
			else
			{
				CreateWeapon( pChar );

				if ( CheckKeyInput( pChar ) )
					SetRepeatAttackState( pChar );
			}
		}
		break;
	case SS_REPEAT_ATTACK:
		{			
			if ( m_dwMotionEndTime < dwCurTime )
				SetEndState( pChar );
			else
			{
				if ( CheckKeyInput( pChar ) )
					SetRepeatAttackState( pChar );
			}
		}
		break;
	}
}

void ioTargetRepeatAttack::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	EndCameraBuff( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_bSetHandMesh = false;

	if ( m_dwTargetFindEffectID != -1 )
		pChar->EndMapEffect( m_dwTargetFindEffectID, false );

	RemoveAllTargetBuff( pChar, pChar->GetCreator() );

	pChar->SetSkillMoveEnable( false, false );
}

bool ioTargetRepeatAttack::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

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

	if( m_dwPreDelayStartTime == 0 )
	{		
		if( m_dwCurSkillProtectTime > dwCurTime )
			return true;
	}

	return false;
}

bool ioTargetRepeatAttack::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{	
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_NONE:
	case SS_FIND_TARGET:
	case SS_TARGET_ATTACK:
	case SS_REPEAT_ATTACK:
		return false;
	case SS_END:
		if( m_fSkillEndNextJumpAmt == 0.0f )
			return true;
		else
			return false;
	}

	if( m_dwMotionEndTime + dwActionStop >= FRAMEGETTIME() )
		return false;

	return true;
}

bool ioTargetRepeatAttack::IsAttackEndState() const
{
	if( m_SkillState == SS_END )
		return true;

	return false;
}

float ioTargetRepeatAttack::GetSkillMoveRate() const
{
	switch( m_SkillState )
	{
	case SS_FIND_TARGET:
		return m_fFindMoveSpeedRate;
	}

	return 0.0f;
}

void ioTargetRepeatAttack::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
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
}

void ioTargetRepeatAttack::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar || !pStage ) 
		return;

	int nState;
	rkPacket >> nState;

	switch( nState )
	{
	case SS_TARGET_ATTACK:
		SetTargetAttackState( pChar, pStage );
		break;
	case SS_REPEAT_ATTACK:
		{
			rkPacket >> m_nCurRepeatCnt;
			SetRepeatAttackState( pChar );
		}
		break;
	case SS_END:
		m_SkillState = SS_END;
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioTargetRepeatAttack::Init()
{
	m_SkillState = SS_NONE;

	m_bSetHandMesh = false;
	m_dwCurSkillProtectTime = 0;
	m_dwFireStartTime = 0;
	m_dwAttackReserveTime = 0;
	m_dwCheckFindLoopTime = 0;
	m_nCurRepeatCnt = 0;
	m_bSetDelay = false;
	m_bSetMove = false;
	m_dwTargetFindEffectID = -1;
	m_fCurScaleRate = FLOAT1;
	m_fTargetCurRange = 0.0f;
}

bool ioTargetRepeatAttack::CheckKeyInput( ioBaseChar *pChar )
{
	if( !pChar || !pChar->IsNeedProcess() || m_nCurRepeatCnt >= m_nMaxRepeatCnt ) 
		return false;

	if ( m_dwAttackReserveTime < FRAMEGETTIME() && pChar->IsAttackKey() )
		return true;

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioTargetRepeatAttack::UpdateTargetEffect( ioBaseChar *pChar, ioPlayStage* pStage )
{
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	m_fCurScaleRate = m_fCurScaleRate + m_fTargetFindScaleSpeedRate * fTimePerSec;
	m_fCurScaleRate = min( m_fCurScaleRate, m_fTargetFindScaleMaxRate );

	ioMapEffect* pMapEffect = pStage->FindMapEffect( m_dwTargetFindEffectID );
	if( pMapEffect )
	{
		D3DXVECTOR3 vPos( pChar->GetWorldPosition().x, pChar->GetMidHeightByRate() + m_fTargetFindEffectHeightOffset, pChar->GetWorldPosition().z );
		pMapEffect->SetWorldPosition( vPos );

		ioEffect* pEffect = pMapEffect->GetEffect();
		if( pEffect )
			pEffect->GetParentNode()->SetScale( m_fCurScaleRate, m_fCurScaleRate, m_fCurScaleRate );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioTargetRepeatAttack::FindTarget( ioBaseChar *pChar, ioPlayStage* pStage )
{
	if( !pChar ) 
		return false;
	
	BaseCharList vTargetList;
	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	m_fTargetCurRange = m_fTargetCurRange + m_fTargetRangeRate * fTimePerSec;
	m_fTargetCurRange = min( m_fTargetCurRange, m_fTargetMaxRange );
	m_fTargetCurRange = max( m_fTargetCurRange, m_fTargetMinRange );

	pChar->GetTargetCharListBySphere( vPos, m_fTargetCurRange, m_fTargetAngle, vTargetList );
	
	RemoveTargetBuff( pChar, pStage, vTargetList );
	SortTargetList( pChar, vTargetList );
	
	if ( m_szTargetList.empty() )
	{
		RemoveAllTargetBuff( pChar, pStage );
		return false;
	}

	AddTargetBuff( pChar, pStage );

	return true;
}

void ioTargetRepeatAttack::SortTargetList( ioBaseChar *pChar, const BaseCharList &TargetList )
{
	if( !pChar )	
		return;

	m_szTargetList.clear();

	D3DXVECTOR3 vDiff;
	D3DXVECTOR3 vOwnerPos = pChar->GetWorldPosition();

	BaseCharList::const_iterator iter;
	for( iter=TargetList.begin(); iter!=TargetList.end(); ++iter )
	{
		ioBaseChar *pCurChar = *iter;
		if ( !pCurChar )
			continue;

		if( pCurChar != pChar )
		{
			if ( pChar->GetTeam() == pCurChar->GetTeam() )
				continue;

			if ( !IsEnableTarget( pCurChar ) )
				continue;

			m_szTargetList.push_back( pCurChar->GetCharName() );
		}
	}
}

bool ioTargetRepeatAttack::IsEnableTarget( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;
	
	if( pTarget->IsSystemState() )
		return false;
			
	if( !pTarget->IsEnableTarget() )		
		return false;

	CharState eTargetState = pTarget->GetState();
	switch( eTargetState )
	{
	case CS_BLOW_WOUNDED:
		{
			if ( m_bEnableTargetDown && !pTarget->IsFloatingState() )
				return true;
			return false;
		}
	case CS_BOUND_BLOW_WOUNDED:
		{
			if ( m_bEnableTargetDown && !pTarget->IsFloatingState() )
				return true;
			return false;
		}
	case CS_ETC_STATE:
		{
			if ( m_bEnableTargetDown && pTarget->IsApplyDownState(false) )
				return true;
			return false;
		}
	case CS_USING_SKILL:
		{
			if ( m_bEnableTargetDown && pTarget->IsApplyDownState(false) )
				return true;
			return false;
		}
	case CS_FROZEN:
		{
			if( m_bEnableTargetDown && pTarget->GetBlowFrozenState() )
				return true;
			return false;
		}
	case CS_STOP_MOTION:
		{
			if( m_bEnableTargetDown && pTarget->GetBlowStopMotionState() )
				return true;
			return false;
		}
	}

	return true;
}

void ioTargetRepeatAttack::CheckDelayMoveAni( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	if( pChar->IsSettedDirection() )
	{
		if( !m_bSetMove )
		{	
			ioEntityGroup *pGrp = pChar->GetGroup();

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->SetLoopAni( m_szMoveLoopAni, FLOAT100, FLOAT1, FLOAT1/m_fMoveLoopAniRate );

			m_bSetDelay = false;
			m_bSetMove = true;
		}
	}
	else
	{		
		if( !m_bSetDelay )
		{
			ioEntityGroup *pGrp = pChar->GetGroup();

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->SetLoopAni( m_szDelayLoopAni, FLOAT100, FLOAT1, FLOAT1/m_fDelayLoopAniRate );

			m_bSetDelay = true;
			m_bSetMove = false;
		}
	}
}

void ioTargetRepeatAttack::CreateWeapon( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	if ( m_TargetAttack.m_vWeaponInfoList.empty() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwFireStartTime == 0 || m_dwFireStartTime > dwCurTime )
		return;
		
	m_dwFireStartTime = 0;

	int nTargetCnt = (int)m_szTargetList.size();
	for ( int i=0; i<nTargetCnt; i++ )
	{
		ioBaseChar *pTarget = pChar->GetBaseChar( m_szTargetList[i] );
		if ( pTarget )
		{
			WeaponInfo sWeaponInfo = m_TargetAttack.m_vWeaponInfoList[0];

			FireTime kFireTime;
			kFireTime.dwStart = FRAMEGETTIME();

			kFireTime.iAttributeIdx = sWeaponInfo.m_iWeaponIdx;
			kFireTime.iResistanceIdx = sWeaponInfo.m_iResistanceIdx;
			kFireTime.szWoundedAni = sWeaponInfo.m_WoundedAnimation;
			kFireTime.dwWoundedAniDuration = sWeaponInfo.m_dwWoundedDuration;
			kFireTime.bLoopWoundedAni = sWeaponInfo.m_bWoundedLoopAni;

			kFireTime.dwWeaponIdx = m_dwWeaponBaseIndex++;
			kFireTime.eFireTimeType = FTT_SKILL_ATTACK;

			D3DXVECTOR3 vPos = pTarget->GetMidPositionByRate();
			D3DXVECTOR3 vDir = pTarget->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );

			ioWeapon *pWeapon = pChar->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
			if ( pWeapon )
			{
				pWeapon->SetPosition( vPos );
				pWeapon->SetMoveDir( vDir );
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioTargetRepeatAttack::SetFindTargetState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage )
		return;
	
	CheckDelayMoveAni( pChar );
	pChar->SetSkillMoveEnable( true, true );
	m_SkillState = SS_FIND_TARGET;
	m_dwCheckFindLoopTime = FRAMEGETTIME() + m_dwFindLoopTime;
		
	D3DXVECTOR3 vPos( pChar->GetWorldPosition().x, pChar->GetMidHeightByRate() + m_fTargetFindEffectHeightOffset, pChar->GetWorldPosition().z );
	ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_TargetFindEffect, vPos, D3DXVECTOR3( m_fCurScaleRate, m_fCurScaleRate, m_fCurScaleRate ) );
	if( pMapEffect )
		m_dwTargetFindEffectID = pMapEffect->GetUniqueID();
}

void ioTargetRepeatAttack::SetTargetAttackState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int nAniID = pGrp->GetAnimationIdx( m_TargetAttack.m_AttackAnimation );
	if( nAniID == -1 )	
		return;

	float fTimeRate  = m_TargetAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_TargetAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
		
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay;

	m_dwAttackReserveTime = dwCurTime + pGrp->GetAnimationEventTime_e( nAniID, "key_reserve" );

	m_dwFireStartTime = dwCurTime + pGrp->GetAnimationEventTime_e( nAniID, "fire_s" ) * fTimeRate;

	pChar->SetSkillMoveEnable( false, false );

	if ( m_dwTargetFindEffectID != -1 )
		pChar->EndMapEffect( m_dwTargetFindEffectID, false );

	m_bSetDelay = false;
	m_bSetMove = false;

	m_SkillState = SS_TARGET_ATTACK;

	RemoveAllTargetBuff( pChar, pStage );

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_TARGET_ATTACK;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetRepeatAttack::SetRepeatAttackState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	if ( m_nCurRepeatCnt < 0 || (int)m_vRepeatAttackList.size() <= m_nCurRepeatCnt )
		return;

	const AttackAttribute &sAttackInfo = m_vRepeatAttackList[m_nCurRepeatCnt];

	ioEntityGroup *pGrp = pChar->GetGroup();
	int nAniID = pGrp->GetAnimationIdx( sAttackInfo.m_AttackAnimation );
	if( nAniID == -1 )	
		return;

	float fTimeRate  = sAttackInfo.m_fAttackAniRate;
	DWORD dwPreDelay = sAttackInfo.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
		
	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pChar->ClearReservedSliding();

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex, nAniID, sAttackInfo.m_vWeaponInfoList, FTT_SKILL_ATTACK, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay;

	m_dwAttackReserveTime = dwCurTime + pGrp->GetAnimationEventTime_e( nAniID, "key_reserve" );

	m_SkillState = SS_REPEAT_ATTACK;

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_REPEAT_ATTACK;
		kPacket << m_nCurRepeatCnt;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_nCurRepeatCnt++;
}

void ioTargetRepeatAttack::SetEndState( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return;

	m_SkillState = SS_END;

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioTargetRepeatAttack::AddTargetBuff( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage )
		return;
	
	int nTargetCnt = (int)m_szTargetList.size();
	for( int i=0; i<nTargetCnt; i++ )
	{
		ioBaseChar *pTarget = pChar->GetBaseChar( m_szTargetList[i] );
		if( !pTarget )
			continue;

		if( pTarget->GetCharName() == pChar->GetCharName() )
			continue;
		
		if( pChar->GetTeam() == pTarget->GetTeam() )
			continue;

		if ( pTarget->HasBuff( pChar->GetCharName(), m_szTargetBuff ) )
			continue;

		pTarget->AddNewBuff( m_szTargetBuff, pChar->GetCharName(), "", NULL );
	}
}

void ioTargetRepeatAttack::RemoveTargetBuff( ioBaseChar *pChar, ioPlayStage *pStage, const BaseCharList &TargetList )
{
	if ( !pChar || !pStage )
		return;

	for ( int i=0; i<(int)m_szTargetList.size(); i++ )
	{
		bool bRemove = true;
		BaseCharList::const_iterator iter;
		for( iter=TargetList.begin(); iter!=TargetList.end(); ++iter )
		{
			ioBaseChar *pTarget = *iter;
			if ( !pTarget )
				continue;

			if( pChar->GetTeam() == pTarget->GetTeam() )
				continue;

			if( pChar->GetCharName() == pTarget->GetCharName() )
				continue;

			if ( m_szTargetList[i] == pTarget->GetCharName() )
				bRemove = false;
		}	

		if ( bRemove )
		{
			ioBaseChar *pTarget = pChar->GetBaseChar( m_szTargetList[i] );
			if ( pTarget )
			{
				ioBuff *pBuff = pTarget->GetBuff( m_szTargetBuff, pChar->GetCharName() );
				if ( pBuff )
					pBuff->SetReserveEndBuff();
			}			
		}
	}
}

void ioTargetRepeatAttack::RemoveAllTargetBuff( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage )
		return;

	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);
		if( !pTarget )
			continue;

		if( pTarget->GetCharName() == pChar->GetCharName() )
			continue;

		if( pChar->GetTeam() == pTarget->GetTeam() )
			continue;

		if ( !pTarget->HasBuff( pChar->GetCharName(), m_szTargetBuff ) )
			continue;

		ioBuff *pBuff = pTarget->GetBuff( m_szTargetBuff, pChar->GetCharName() );
		if ( pBuff )
			pBuff->SetReserveEndBuff();
	}
}