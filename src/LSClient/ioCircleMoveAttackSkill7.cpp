

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioMagicCircle.h"

#include "ioCircleMoveAttackSkill7.h"

#include "WeaponDefine.h"

ioCircleMoveAttackSkill7::ioCircleMoveAttackSkill7()
{
	ClearData();
}

ioCircleMoveAttackSkill7::ioCircleMoveAttackSkill7( const ioCircleMoveAttackSkill7 &rhs )
: ioNormalSkill( rhs ),
  m_LoopAnimation( rhs.m_LoopAnimation ),
  m_fLoopAniRate( rhs.m_fLoopAniRate ),
  m_dwLoopDuration( rhs.m_dwLoopDuration ),
  m_fCircleRange( rhs.m_fCircleRange ),
  m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
  m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
  m_fCircleOffSet( rhs.m_fCircleOffSet ),
  m_ReadyCircle( rhs.m_ReadyCircle ),
  m_RedEffect( rhs.m_RedEffect ),
  m_BlueEffect( rhs.m_BlueEffect ),
  m_fRangeSpeed( rhs.m_fRangeSpeed ),
  m_fMaxRange( rhs.m_fMaxRange ),
  m_fMinRange( rhs.m_fMinRange ),
  m_vRangeInfo( rhs.m_vRangeInfo ),
  m_SkillEffect1( rhs.m_SkillEffect1 ),
  m_SkillEffect2( rhs.m_SkillEffect2 ),
  m_TestEffect( rhs.m_TestEffect ),
  m_dwSkillProtectTime( rhs.m_dwSkillProtectTime )
{
	ClearData();
}

ioCircleMoveAttackSkill7::~ioCircleMoveAttackSkill7()
{
}

void ioCircleMoveAttackSkill7::ClearData()
{
	m_State = SS_NONE;

	m_dwCurMapEffect = -1;

	m_bSetHandMesh = false;
	m_dwFireStartTime = 0;

	m_dwMotionEndTime     = 0;
	m_dwMotionStartTime   = 0;
	m_dwFireMotionEndTime = 0;

	m_dwLoopStartTime = 0;

	m_dwTestEffectID[0] = -1;
	m_dwTestEffectID[1] = -1;
	m_dwTestEffectID[2] = -1;
	m_dwTestEffectID[3] = -1;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_dwSkillEffect1ID = -1;
	m_dwSkillEffect2ID = -1;
}

void ioCircleMoveAttackSkill7::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szKey[MAX_PATH], szBuf[MAX_PATH];

	rkLoader.LoadString_e( "skill_loop_animation", "", szBuf, MAX_PATH );
	m_LoopAnimation  = szBuf;
	m_fLoopAniRate   = rkLoader.LoadFloat_e( "skill_loop_ani_rate", 0.0f );
	m_dwLoopDuration = (DWORD)rkLoader.LoadInt_e( "skill_loop_duration", 0 );

	m_fCircleRange     = rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet    = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;

	rkLoader.LoadString_e( "red_team_effect", "", szBuf, MAX_PATH );
	m_RedEffect = szBuf;
	rkLoader.LoadString_e( "blue_team_effect", "", szBuf, MAX_PATH );
	m_BlueEffect = szBuf;

	int iCnt = rkLoader.LoadInt_e( "range_count", 0 );
	for( int i=0; i<iCnt; ++i )
	{
		RangeInfo kInfo;

		wsprintf_e( szKey, "range%d_max_dist", i+1 );
		kInfo.m_fMaxDist = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "range%d_min_dist", i+1 );
		kInfo.m_fMinDist = rkLoader.LoadFloat( szKey, 0.0f );

		// Action Attack
		kInfo.m_AttackAttribute.Init();

		wsprintf_e( szKey, "range%d_animation", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_AttackAttribute.m_AttackAnimation = szBuf;

		wsprintf_e( szKey, "range%d_ani_rate", i+1 );
		kInfo.m_AttackAttribute.m_fAttackAniRate = rkLoader.LoadFloat( szKey, FLOAT1 );
		wsprintf_e( szKey, "range%d_animation_pre_delay", i+1 );
		kInfo.m_AttackAttribute.m_dwPreDelay	 = rkLoader.LoadInt_e( "animation_pre_delay", 0 );
		wsprintf_e( szKey, "range%d_animation_end_delay", i+1 );
		kInfo.m_AttackAttribute.m_dwEndDelay	 = rkLoader.LoadInt_e( "animation_end_delay", 0 );
		
		wsprintf_e( szKey, "range%d_attack_type_count", i+1 );
		int iAttackTypeCnt = rkLoader.LoadInt( szKey, 0 );
		kInfo.m_AttackAttribute.m_vWeaponInfoList.clear();
		kInfo.m_AttackAttribute.m_vWeaponInfoList.reserve( iAttackTypeCnt );

		for( int j=0 ; j<iAttackTypeCnt ; ++j )
		{
			WeaponInfo kWeaponInfo;

			wsprintf_e( szKey, "range%d_attack_type%d", i+1, j+1 );
			kWeaponInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

			wsprintf_e( szKey, "range%d_attack_type%d_resist", i+1, j+1 );
			kWeaponInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

			wsprintf_e( szKey, "range%d_attack_type%d_wounded", i+1, j+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			kWeaponInfo.m_WoundedAnimation = szBuf;

			wsprintf_e( szKey, "range%d_attack_type%d_wounded_time", i+1, j+1 );
			kWeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

			wsprintf_e( szKey, "range%d_attack_type%d_wounded_loop_ani", i+1, j+1 );
			kWeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

			kInfo.m_AttackAttribute.m_vWeaponInfoList.push_back( kWeaponInfo );
		}

		m_vRangeInfo.push_back( kInfo );
	}

	m_fRangeSpeed = rkLoader.LoadFloat_e( "range_speed", FLOAT1 );
	m_fMaxRange   = rkLoader.LoadFloat_e( "max_range", 0.0f );
	m_fMinRange   = rkLoader.LoadFloat_e( "min_range", 0.0f );

	rkLoader.LoadString_e( "skill_range_effect_1", "", szBuf, MAX_PATH );
	m_SkillEffect1 = szBuf;

	rkLoader.LoadString_e( "skill_range_effect_2", "", szBuf, MAX_PATH );
	m_SkillEffect2 = szBuf;

	rkLoader.LoadString_e( "test_effect", "", szBuf, MAX_PATH );
	m_TestEffect = szBuf;

	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );
}

ioSkill* ioCircleMoveAttackSkill7::Clone()
{
	return new ioCircleMoveAttackSkill7( *this );
}

bool ioCircleMoveAttackSkill7::OnSkillStart( ioBaseChar *pChar )
{	
	if( !ioNormalSkill::OnSkillStart( pChar ) )
		return false;

	ClearData();
	
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	return true;
}

void ioCircleMoveAttackSkill7::OnSkillEnd( ioBaseChar *pChar )
{
	ioNormalSkill::OnSkillEnd( pChar );

	for( int i=0; i<4; ++i )
	{
		pChar->DestroyMapEffect( m_dwTestEffectID[i] );
		m_dwTestEffectID[i] = -1;
	}

	pChar->DestroyMapEffect( m_dwSkillEffect1ID );
	m_dwSkillEffect1ID = -1;
	pChar->DestroyMapEffect( m_dwSkillEffect2ID );
	m_dwSkillEffect2ID = -1;

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	// 스킬 중간 동작에 사용된 메쉬들 제거
	m_bSetHandMesh = false;
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	EndCameraBuff( pChar );

	pChar->SetMagicCircleTarget( "" );
	pChar->DestroyMagicCircle();

	//DestroyMapEffect
	if( m_dwCurMapEffect != -1 )
		pChar->EndMapEffect( m_dwCurMapEffect, false );

	ClearData();
}

void ioCircleMoveAttackSkill7::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_NONE:
		{
			if( m_dwPreDelayStartTime + GetPreDelayTime() < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );

				SetNormalSkill( pChar );
			}
		}
		break;
	case SS_PRE_LOOP:
		{
			ProcessPreLoopState( pChar );
		}
		break;
	case SS_LOOP:
		{
			ProcessLoopState( pChar );
		}
		break;
	case SS_END:
		break;
	}
}

void ioCircleMoveAttackSkill7::SetNormalSkill( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	SetPreLoopState( pChar );

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
}

void ioCircleMoveAttackSkill7::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar )
		return;

	int iState;
	rkPacket >> iState;
	switch( iState )
	{
	case SSC_POSITION:
		{
			rkPacket >> m_vCirclePos;
			SetLoopState( pChar );
		}
		break;
	case SSC_ATTACK:
		{
			int iDirKey;
			rkPacket >> iDirKey;
			rkPacket >> m_vCirclePos;
			rkPacket >> m_fCurRange;
			m_CurDirKey = static_cast<ioUserKeyInput::DirKeyInput>(iDirKey);

			SetAttackState( pChar );
		}
		break;
	}
}

bool ioCircleMoveAttackSkill7::IsProtected( int iDefenseBreakType ) const
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

	if( m_dwPreDelayStartTime == 0 )
	{
		if( m_dwFireMotionEndTime > FRAMEGETTIME() )
			return true;

		if( m_dwSkillProtectTime > 0 && m_dwSkillStartTime + m_dwSkillProtectTime > FRAMEGETTIME() )
			return true;
	}

	return false;
}

bool ioCircleMoveAttackSkill7::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_State == SS_END && m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
		return true;
	
	return false;
}

bool ioCircleMoveAttackSkill7::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioCircleMoveAttackSkill7::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
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
			pZone->SetZoneValue( m_vCirclePos, true );
			pZone->SetZoneRange( m_fCurRange );
		}
	}
}

void ioCircleMoveAttackSkill7::SetPreLoopState( ioBaseChar *pChar )
{
	if( m_State == SS_PRE_LOOP )
		return;

	m_State = SS_PRE_LOOP;

	if( !pChar->CheckMagicCircle() && pChar->IsNeedProcess() )
	{
		ioHashString szDummyCircle;
		pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, szDummyCircle, m_fCircleOffSet );
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( pGrp )
	{
		int iAniID = pGrp->GetAnimationIdx( m_LoopAnimation );
		if( iAniID == -1 )	return;

		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/m_fLoopAniRate );

		m_dwLoopStartTime = FRAMEGETTIME() + m_dwLoopDuration;
	}
}

void ioCircleMoveAttackSkill7::SetLoopState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_State == SS_LOOP )
		return;

	m_State = SS_LOOP;
	
	m_fCurRange = m_fMaxRange;

	if( pChar->CheckMagicCircle() )
	{
		m_vCirclePos = pChar->GetMagicCirclePos();
		ioMagicCircle *pCircle = pChar->GetMagicCircle();
		if( pCircle )
		{
			DWORD dwCircleID = pCircle->GetReadyCircelID();
			pChar->DestroyMapEffect( dwCircleID );
		}
	}

	ioMapEffect *pEffect = pChar->CreateMapEffectBySkill( m_SkillEffect1, m_vCirclePos, ioMath::UNIT_ALL );
	if( pEffect )
		m_dwSkillEffect1ID = pEffect->GetUniqueID();

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_POSITION;
		kPacket << m_vCirclePos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	{
		// 테스트
		D3DXVECTOR3 vDir[4];
		vDir[0] = D3DXVECTOR3( -FLOAT1, 0.0f, 0.0f ) * m_fCurRange;
		vDir[1] = D3DXVECTOR3( FLOAT1, 0.0f, 0.0f ) * m_fCurRange;
		vDir[2] = D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 ) * m_fCurRange;
		vDir[3] = D3DXVECTOR3( 0.0f, 0.0f, FLOAT1 ) * m_fCurRange;

		for( int i=0; i<4; ++i )
		{
			D3DXVECTOR3 vPos = m_vCirclePos + vDir[i];
			ioMapEffect *pEff = pChar->CreateMapEffectBySkill( m_TestEffect, vPos, D3DXVECTOR3( FLOAT05, FLOAT05, FLOAT05 ) );
			if( pEff )
				m_dwTestEffectID[i] = pEff->GetUniqueID();
		}
	}
}

void ioCircleMoveAttackSkill7::SetAttackState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_State == SS_END )
		return;

	m_State = SS_END;

	pChar->DestroyMapEffect( m_dwSkillEffect1ID );
	m_dwSkillEffect1ID = -1;

	AttackAttribute kAttribute;
	kAttribute.Init();
	int iCnt = m_vRangeInfo.size();
	for( int i=0; i<iCnt; ++i )
	{
		if( m_vRangeInfo[i].CheckRange( m_fCurRange ) )
			kAttribute = m_vRangeInfo[i].m_AttackAttribute;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( kAttribute.m_AttackAnimation );
	if( iAniID == -1 )	return;

	float fTimeRate  = kAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = kAttribute.m_dwPreDelay;
	DWORD dwEndDelay = kAttribute.m_dwEndDelay;
	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
										iAniID,
										kAttribute.m_vWeaponInfoList,
										FTT_SKILL_ATTACK,
										fTimeRate,
										dwPreDelay );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwMotionStartTime = m_dwMotionEndTime = m_dwFireMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + dwEndDelay;

	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	if( pChar->IsNeedProcess() )
	{
		m_CurDirKey = pChar->CheckCurDirKey();

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SSC_ATTACK;
			kPacket << static_cast<int>(m_CurDirKey);
			kPacket << m_vCirclePos;
			kPacket << m_fCurRange;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		if( GetOwnerItem() )
			pChar->ReduceNeedGaugeBySkill( this, GetOwnerItem()->GetMaxSkillGauge( pChar->GetCharName() ) );
	}
}

void ioCircleMoveAttackSkill7::ProcessPreLoopState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	pChar->CheckChangeDirectionForMagicCircle();
	pChar->UpdateMagicCirclePosAll( m_fCircleMoveSpeed, m_fCircleRange, m_fCircleHeightGap, false );

	D3DXVECTOR3 vTargetPos = pChar->GetMagicCirclePos();
	pChar->SetTargetRotToTargetPos( vTargetPos, true, false );

	if( pChar->IsNeedProcess() && ( pChar->IsAttackKey() || m_dwLoopStartTime < FRAMEGETTIME() ) )
	{
		SetLoopState( pChar );
	}
}

void ioCircleMoveAttackSkill7::ProcessLoopState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( !pChar->IsNeedProcess() )
		return;
	
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	float fRate = (fTimePerSec*1000) / m_fRangeSpeed;
	float fRange = m_fMaxRange * fRate;
	
	m_fCurRange -= fRange;
	m_fCurRange = max( m_fCurRange, m_fMinRange );

	ioPlayStage *pStage = pChar->GetCreator();
	if( pStage )
	{
		if( m_fCurRange <= m_fMinRange )
		{
			pStage->DestroyMapEffect( m_dwSkillEffect1ID );
			m_dwSkillEffect1ID = -1;

			if( m_dwSkillEffect2ID == -1 )
			{
				ioMapEffect *pEffect = pChar->CreateMapEffectBySkill( m_SkillEffect2, m_vCirclePos, ioMath::UNIT_ALL );
				if( pEffect )
					m_dwSkillEffect2ID = pEffect->GetUniqueID();
			}
		}

		D3DXVECTOR3 vDir[4];
		vDir[0] = D3DXVECTOR3( -FLOAT1, 0.0f, 0.0f ) * m_fCurRange;
		vDir[1] = D3DXVECTOR3( FLOAT1, 0.0f, 0.0f ) * m_fCurRange;
		vDir[2] = D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 ) * m_fCurRange;
		vDir[3] = D3DXVECTOR3( 0.0f, 0.0f, FLOAT1 ) * m_fCurRange;

		for( int i=0; i<4; ++i )
		{
			ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwTestEffectID[i] );
			if( pMapEffect )
			{
				// 테스트
				D3DXVECTOR3 vPos = m_vCirclePos + vDir[i];
				pMapEffect->SetWorldPosition( vPos );
			}			
		}
	}

	if( pChar->IsAttackKey() )
	{
		SetAttackState( pChar );
	}
}

ioUserKeyInput::DirKeyInput ioCircleMoveAttackSkill7::GetCurKey()
{
	return m_CurDirKey;
}