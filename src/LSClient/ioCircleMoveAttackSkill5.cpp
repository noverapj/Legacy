

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioGrowthUpInfo.h"

#include "ioCircleMoveAttackSkill5.h"

#include "WeaponDefine.h"

ioCircleMoveAttackSkill5::ioCircleMoveAttackSkill5()
{
	ClearData();
}

ioCircleMoveAttackSkill5::ioCircleMoveAttackSkill5( const ioCircleMoveAttackSkill5 &rhs )
: ioNormalSkill( rhs ),
  m_AttackAttribute( rhs.m_AttackAttribute ),
  m_vCircleRange( rhs.m_vCircleRange ),
  m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
  m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
  m_fCircleOffSet( rhs.m_fCircleOffSet ),
  m_ReadyCircle( rhs.m_ReadyCircle ),
  m_RedEffect( rhs.m_RedEffect ),
  m_BlueEffect( rhs.m_BlueEffect ),
  m_SkillEndAnimation( rhs.m_SkillEndAnimation ),
  m_fSkillEndAniRate( rhs.m_fSkillEndAniRate ),
  m_bDisableGrowth( rhs.m_bDisableGrowth ),
  m_fGuagePerTic( rhs.m_fGuagePerTic ),
  m_dwTicTime( rhs.m_dwTicTime ),
  m_fHeightOffset( rhs.m_fHeightOffset ),
  m_iMaxAttackCount( rhs.m_iMaxAttackCount ),
  m_fFindTargetRange( rhs.m_fFindTargetRange )
{
	ClearData();
}

ioCircleMoveAttackSkill5::~ioCircleMoveAttackSkill5()
{
}

void ioCircleMoveAttackSkill5::ClearData()
{
	m_State = SS_NONE;

	m_dwCurMapEffect = -1;

	m_bSetHandMesh = false;
	m_dwFireStartTime = 0;

	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;

	m_dwCheckTime = 0;

	m_iCurAttackCount = 0;
}

void ioCircleMoveAttackSkill5::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szKey[MAX_PATH], szBuf[MAX_PATH];

	m_iMaxAttackCount = rkLoader.LoadInt_e( "attack_count", 1 );

	m_vCircleRange.clear();
	m_vCircleRange.reserve( m_iMaxAttackCount );
	// Magic circle
	for( int i=0; i<m_iMaxAttackCount; ++i )
	{
		wsprintf_e( szKey, "magic_circle_range%d", i+1 );
		float fRange = rkLoader.LoadFloat( szKey, 0.0f );
		m_vCircleRange.push_back( fRange );
	}
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet    = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;

	rkLoader.LoadString_e( "red_team_effect", "", szBuf, MAX_PATH );
	m_RedEffect = szBuf;
	rkLoader.LoadString_e( "blue_team_effect", "", szBuf, MAX_PATH );
	m_BlueEffect = szBuf;

	// End animation
	rkLoader.LoadString_e( "skill_end_animation", "", szBuf, MAX_PATH );
	m_SkillEndAnimation = szBuf;
	m_fSkillEndAniRate = rkLoader.LoadFloat_e( "skill_end_ani_rate", FLOAT1 );

	// Action Attack
	m_AttackAttribute.Init();
	m_AttackAttribute.m_AttackAnimation = m_SkillMotionName;
	m_AttackAttribute.m_fAttackAniRate  = m_fAnimationTimeRate;
	m_AttackAttribute.m_dwPreDelay	    = rkLoader.LoadInt_e( "animation_pre_delay", 0 );
	m_AttackAttribute.m_dwEndDelay	    = rkLoader.LoadInt_e( "animation_end_delay", 0 );

	m_AttackAttribute.m_vForceInfoList.clear();
	int iForceCnt = rkLoader.LoadInt_e( "attack_force_cnt", 0 );
	if( iForceCnt > 0 )
		m_AttackAttribute.m_vForceInfoList.reserve( iForceCnt );

	for( int i=0; i < iForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szKey, "attack_force_move%d", i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "attack_force_friction%d", i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "attack_force_angle%d", i+1 );
		kInfo.m_fForceAngle = rkLoader.LoadFloat( szKey, 0.0f );

		m_AttackAttribute.m_vForceInfoList.push_back( kInfo );
	}

	int iAttackTypeCnt = rkLoader.LoadInt_e( "attack_type_count", 0 );
	m_AttackAttribute.m_vWeaponInfoList.clear();
	m_AttackAttribute.m_vWeaponInfoList.reserve( iAttackTypeCnt );

	for( int i=0 ; i<iAttackTypeCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "attack_type%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "attack_type%d_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "attack_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "attack_type%d_wounded_time", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "attack_type%d_wounded_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_AttackAttribute.m_vWeaponInfoList.push_back( kInfo );
	}

	m_bDisableGrowth = rkLoader.LoadBool_e( "disable_growth", false );
	m_fGuagePerTic   = rkLoader.LoadFloat_e( "gauge_per_tick", 0.0f );
	m_dwTicTime      = (DWORD)rkLoader.LoadInt_e( "tick_time", 0 );

	m_fHeightOffset = rkLoader.LoadFloat_e( "height_offset", 0.0f );
	m_fFindTargetRange = rkLoader.LoadFloat_e( "find_target_range", 0.0f );
}

ioSkill* ioCircleMoveAttackSkill5::Clone()
{
	return new ioCircleMoveAttackSkill5( *this );
}

bool ioCircleMoveAttackSkill5::OnSkillStart( ioBaseChar *pChar )
{	
	if( !ioNormalSkill::OnSkillStart( pChar ) )
		return false;

	ClearData();
	
	m_bSetHandMesh = true;
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_dwCheckTime			= FRAMEGETTIME();
	m_fTicGaugeRateByGrowth	= FLOAT1;
	m_dwTicTimeByGrowth		= 0;

	// 성장관련 요소 추가.
	if( !m_bDisableGrowth )
	{
		ioPlayStage *pStage = pChar->GetCreator();
		if( pStage )
		{
			ioGrowthNormalUpInfo *pUpInfo = NULL;

			pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_SKILL_COOL_TIME, pChar->GetCharName()) );
			if( pUpInfo )
				m_fTicGaugeRateByGrowth = pUpInfo->GetValue(pStage);

			m_fTicGaugeRateByGrowth = min( m_fTicGaugeRateByGrowth, FLOAT100 );
			m_fTicGaugeRateByGrowth = FLOAT1 + ( m_fTicGaugeRateByGrowth/FLOAT100 );


			pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_BUFF_TIC_TIME, pChar->GetCharName()) );
			if( pUpInfo )
				m_dwTicTimeByGrowth = pUpInfo->GetValue(pStage);			
		}
	}

	return true;
}

void ioCircleMoveAttackSkill5::OnSkillEnd( ioBaseChar *pChar )
{
	ioNormalSkill::OnSkillEnd( pChar );

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

	pChar->SetAlphaRateDirect( MAX_ALPHA_RATE );
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( pGrp )
		pGrp->SetVisible( true );

	ioHelmetItem *pHelmet = ToHelmetItem( pChar->GetEquipedItem( ES_HELMET ) );
	if( pHelmet )
	{
		ioHelmetItem::InvisibleFaceType eType = pHelmet->GetInvisibleFaceType();

		switch( eType )
		{
		case ioHelmetItem::IFT_ALL:
			pChar->ShowFaceMesh( false );
			break;
		case ioHelmetItem::IFT_MALE:
			if( pChar->IsMale() )
				pChar->ShowFaceMesh( false );
			break;
		case ioHelmetItem::IFT_FEMALE:
			if( !pChar->IsMale() )
				pChar->ShowFaceMesh( false );
			break;
		}
	}

	ClearData();
}

void ioCircleMoveAttackSkill5::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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
	case SS_LOOP:
		{
			ProcessSkillLoopState( pChar );
		}
		break;
	case SS_ATTACK:
		{
			ProcessSkillAttackState( pChar );
		}
		break;
	case SS_END:
		break;
	}

	CheckSkillGauge( pChar );
}

void ioCircleMoveAttackSkill5::SetNormalSkill( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	SetSkillLoopState( pChar );

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
}

bool ioCircleMoveAttackSkill5::HideHPGauge()
{
	if( m_State == SS_LOOP )
		return true;

	return false;
}

void ioCircleMoveAttackSkill5::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pChar->SetTargetRotToRotate( qtRot, true, false );
	SetSkillAttackState( pChar );
}

bool ioCircleMoveAttackSkill5::IsProtected( int iDefenseBreakType ) const
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
		if( m_State != SS_END )
			return true;

		if( m_dwFireMotionEndTime > FRAMEGETTIME() )
			return true;
	}

	return false;
}

bool ioCircleMoveAttackSkill5::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_State == SS_END && m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
		return true;
	
	return false;
}

bool ioCircleMoveAttackSkill5::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioCircleMoveAttackSkill5::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
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

SkillCharColType ioCircleMoveAttackSkill5::IsCharSkipState() const
{
	if( m_EnableCharCollisionSkip == SCCT_ALL )
	{
		switch( m_State )
		{
		case SS_END:
			return SCCT_ALL;
		default:
			return SCCT_NONE;
		}
	}

	return m_EnableCharCollisionSkip;
}

bool ioCircleMoveAttackSkill5::IsSkipState() const
{
	if( m_State == SS_LOOP )
		return true;

	return false;
}

void ioCircleMoveAttackSkill5::SetSkillLoopState( ioBaseChar *pChar )
{
	if( m_State == SS_LOOP )
		return;

	m_State = SS_LOOP;

	if( !pChar->CheckMagicCircle() )
	{
		ioHashString szDummyCircle;
		pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, szDummyCircle, m_fCircleOffSet );
	}

	pChar->SetAlphaRateDirect( 0 );
	pChar->CreateMapEffectBySkill( m_SkillEffectName, pChar->GetWorldPosition() );

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->SetVisible( false );
	}
}

void ioCircleMoveAttackSkill5::SetSkillAttackState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_State == SS_ATTACK )
		return;

	m_State = SS_ATTACK;

	D3DXVECTOR3 vTargetPos = pChar->GetMagicCirclePos();
	D3DXVECTOR3 vPos = vTargetPos;
	vPos.y += m_fHeightOffset;
	pChar->SetWorldPosition( vPos );

	RotateToNearChar( pChar );

	pChar->SetMagicCircleTarget( "" );
	pChar->DestroyMagicCircle();

	pChar->CreateMapEffectBySkill( m_SkillEffectName, pChar->GetWorldPosition() );

	ioHelmetItem *pHelmet = ToHelmetItem( pChar->GetEquipedItem( ES_HELMET ) );
	if( pHelmet )
	{
		ioHelmetItem::InvisibleFaceType eType = pHelmet->GetInvisibleFaceType();

		switch( eType )
		{
		case ioHelmetItem::IFT_ALL:
			pChar->ShowFaceMesh( false );
			break;
		case ioHelmetItem::IFT_MALE:
			if( pChar->IsMale() )
				pChar->ShowFaceMesh( false );
			break;
		case ioHelmetItem::IFT_FEMALE:
			if( !pChar->IsMale() )
				pChar->ShowFaceMesh( false );
			break;
		}
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_AttackAttribute.m_AttackAnimation );
	if( iAniID == -1 )	return;

	float fTimeRate  = m_AttackAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = m_AttackAttribute.m_dwPreDelay;
	DWORD dwEndDelay = m_AttackAttribute.m_dwEndDelay;
	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
										iAniID,
										m_AttackAttribute.m_vWeaponInfoList,
										FTT_SKILL_ATTACK,
										fTimeRate,
										dwPreDelay );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + dwEndDelay;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pChar->SetReservedSliding( m_AttackAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_iCurAttackCount++;

	if( !CheckAttackCount() )
	{
		ioItem *pItem = GetOwnerItem();
		if( pItem )
		{
			pChar->ReduceCurSkillGaugeBySkill( pItem, pItem->GetMaxSkillGauge( pChar->GetCharName() ) );
		}
	}

	pChar->SetAlphaRateDirect( MAX_ALPHA_RATE );
	pGrp->SetVisible( true );

	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	if( pStage )
	{
		D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
		if( pChar->GetTeam() == TEAM_RED )
		{
			pStage->CreateMapEffect( m_RedEffect, vTargetPos, vScale );
		}
		else if( pChar->GetTeam() == TEAM_BLUE )
		{
			pStage->CreateMapEffect( m_BlueEffect, vTargetPos, vScale );
		}
		else
		{
			if( pChar->GetSingleConvertTeam() == TEAM_BLUE )
			{
				pStage->CreateMapEffect( m_BlueEffect, vTargetPos, vScale );
			}
			else
			{
				pStage->CreateMapEffect( m_RedEffect, vTargetPos, vScale );
			}
		}
	}

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << pChar->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCircleMoveAttackSkill5::SetSkillEndState( ioBaseChar *pChar, bool bEndMotion )
{
	if( m_State == SS_END )
		return;

	m_State = SS_END;

	if( bEndMotion )
	{
		ioEntityGroup *pGrp = pChar->GetGroup();

		int iAniID = pGrp->GetAnimationIdx( m_SkillEndAnimation );
		if( iAniID == -1 )	return;

		DWORD dwCurTime = FRAMEGETTIME();

		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fSkillEndAniRate );

		m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
		m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * m_fSkillEndAniRate;

		pChar->SetAlphaRateDirect( MAX_ALPHA_RATE );
		pGrp->SetVisible( true );
		pChar->CreateMapEffectBySkill( m_SkillEffectName, pChar->GetWorldPosition() );
	}

	ioHelmetItem *pHelmet = ToHelmetItem( pChar->GetEquipedItem( ES_HELMET ) );
	if( pHelmet )
	{
		ioHelmetItem::InvisibleFaceType eType = pHelmet->GetInvisibleFaceType();

		switch( eType )
		{
		case ioHelmetItem::IFT_ALL:
			pChar->ShowFaceMesh( false );
			break;
		case ioHelmetItem::IFT_MALE:
			if( pChar->IsMale() )
				pChar->ShowFaceMesh( false );
			break;
		case ioHelmetItem::IFT_FEMALE:
			if( !pChar->IsMale() )
				pChar->ShowFaceMesh( false );
			break;
		}
	}
}

void ioCircleMoveAttackSkill5::ProcessSkillLoopState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( !pChar->IsNeedProcess() )
		return;

	pChar->CheckChangeDirectionForMagicCircle();
	pChar->UpdateMagicCirclePosAll( m_fCircleMoveSpeed, m_vCircleRange[m_iCurAttackCount], m_fCircleHeightGap, false );

	D3DXVECTOR3 vTargetPos = pChar->GetMagicCirclePos();
	pChar->SetTargetRotToTargetPos( vTargetPos, true, false );

	if( pChar->IsAttackKey() )
	{
		SetSkillAttackState( pChar );
	}
}

void ioCircleMoveAttackSkill5::ProcessSkillAttackState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_dwMotionEndTime < FRAMEGETTIME() )
	{
		if( CheckAttackCount() )
			SetSkillLoopState( pChar );
		else
		{
			SetSkillEndState( pChar, false );
		}
	}
}

void ioCircleMoveAttackSkill5::CheckSkillGauge( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	float fCurTicGauge = m_fGuagePerTic / m_fTicGaugeRateByGrowth;
	int iTickGap = (int)( dwCurTime - m_dwCheckTime );
	int iCurTicTime = m_dwTicTime + m_dwTicTimeByGrowth;

	if( iTickGap >= iCurTicTime )
	{
		m_dwCheckTime = dwCurTime - ( iTickGap - iCurTicTime );

		if( fCurTicGauge != 0.0f && m_bReduceGauge )
		{
			if( pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), fCurTicGauge ) <= 0.0f && m_State == SS_LOOP )
			{
				D3DXVECTOR3 vTargetPos = pChar->GetMagicCirclePos();
				pChar->SetWorldPosition( vTargetPos );

				SetSkillEndState( pChar, true );
				return;
			}
		}
	}
}

void ioCircleMoveAttackSkill5::RotateToNearChar( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	if( !pStage )
		return;

	D3DXVECTOR3 vTargetPos = ioMath::VEC3_ZERO;
	float fEnableDistSq = m_fFindTargetRange * m_fFindTargetRange;
	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);
		if( pChar->GetCharName() == pTarget->GetCharName() )
			continue;

		if( pChar->GetTeam() == pTarget->GetTeam() )
			continue;

		if( pTarget->IsSystemState() )
			continue;
		
		if( !pTarget->IsEnableTarget() ) 
			continue;

		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - pChar->GetWorldPosition();
		float fDistSq = D3DXVec3LengthSq( &vDiff );
		if( fDistSq < fEnableDistSq )	// Enemy
		{
			fEnableDistSq = fDistSq;
			vTargetPos = pTarget->GetWorldPosition();
		}
	}

	if( vTargetPos != ioMath::VEC3_ZERO )
		pChar->SetTargetRotToTargetPos( vTargetPos, true );
}

bool ioCircleMoveAttackSkill5::CheckAttackCount()
{
	if( m_iCurAttackCount < m_iMaxAttackCount )
		return true;

	return false;
}