

#include "stdafx.h"

#include "ioSummonWoodSkill.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioGrowthUpInfo.h"

ioSummonWoodSkill::ioSummonWoodSkill()
{
	m_dwCurMapEffect = -1;

	m_State = SS_NONE;

	m_bSetHandMesh = false;

	m_dwFireStartTime = 0;
	m_dwActionStartTime = 0;
	m_dwLoopStartTime = 0;
	m_dwFirstLoopStartTime = 0;

	m_iCurSummonCnt = 0;

	m_dwCheckTime			= 0;
	m_fTicGaugeRateByGrowth	= FLOAT1;
	m_dwTicTimeByGrowth		= 0;
}

ioSummonWoodSkill::ioSummonWoodSkill( const ioSummonWoodSkill &rhs )
: ioNormalSkill( rhs ),
m_RedCircle( rhs.m_RedCircle ),
m_BlueCircle( rhs.m_BlueCircle ),
m_bCheckDropZone( rhs.m_bCheckDropZone ),
m_fGaugePerTic( rhs.m_fGaugePerTic ),
m_dwTicTime( rhs.m_dwTicTime ),
m_AreaWeaponName( rhs.m_AreaWeaponName ),
m_fCircleRange( rhs.m_fCircleRange ),
m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
m_fCircleOffSet( rhs.m_fCircleOffSet ),
m_fCircleToCircleOffset( rhs.m_fCircleToCircleOffset ),
m_OnCircle( rhs.m_OnCircle ),
m_OffCircle( rhs.m_OffCircle ),
m_SkillEndMotion( rhs.m_SkillEndMotion ),
m_fEndMotionRate( rhs.m_fEndMotionRate ),
m_StartEffect( rhs.m_StartEffect ),
m_EndEffect( rhs.m_EndEffect ),
m_RedGuidCircle( rhs.m_RedGuidCircle ),
m_BlueGuidCircle( rhs.m_BlueGuidCircle ),
m_LoopAnimation( rhs.m_LoopAnimation ),
m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
m_dwLoopProtectTime( rhs.m_dwLoopProtectTime ),
m_bDisableActionProtect( rhs.m_bDisableActionProtect ),
m_iSummonCnt( rhs.m_iSummonCnt ),
m_bPreventRecoveryGauge( rhs.m_bPreventRecoveryGauge ),
m_DummyCharList( rhs.m_DummyCharList ),
m_vDestroyDummyCharList( rhs.m_vDestroyDummyCharList ),
m_DummyCreateType( rhs.m_DummyCreateType ),
m_bUseDestroyDummy( rhs.m_bUseDestroyDummy ),
m_dwLoopDuration( rhs.m_dwLoopDuration ),
m_fReduceSkillGauge( rhs.m_fReduceSkillGauge )
{
	ClearData();
}

ioSummonWoodSkill::~ioSummonWoodSkill()
{
	m_vDestroyDummyCharList.clear();
	m_DummyCharList.clear();
}

void ioSummonWoodSkill::ClearData()
{
	m_dwCurMapEffect = -1;

	m_State = SS_NONE;

	m_bSetHandMesh = false;

	m_dwFireStartTime = 0;
	m_dwActionStartTime = 0;
	m_dwLoopStartTime = 0;
	m_dwFirstLoopStartTime = 0;

	m_iCurSummonCnt = 0;

	m_dwCheckTime			= 0;
	m_fTicGaugeRateByGrowth	= FLOAT1;
	m_dwTicTimeByGrowth		= 0;

	m_CurCircleEffect = m_OnCircle;

	m_bOffCircle = false;
	m_bEnableSummon = true;
}

void ioSummonWoodSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_bCheckDropZone = rkLoader.LoadBool_e( "check_drop_zone", false );

	rkLoader.LoadString_e( "red_team_circle", "", szBuf, MAX_PATH );
	m_RedCircle = szBuf;
	rkLoader.LoadString_e( "blue_team_circle", "", szBuf, MAX_PATH );
	m_BlueCircle = szBuf;

	m_fGaugePerTic	= rkLoader.LoadFloat_e( "gauge_per_tick", 0.0f );
	m_dwTicTime		= (DWORD)rkLoader.LoadInt_e( "tick_time", 0 );

	rkLoader.LoadString_e( "area_weapon_name", "", szBuf, MAX_PATH );
	m_AreaWeaponName = szBuf;

	m_fCircleRange = rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	m_fCircleToCircleOffset =  rkLoader.LoadFloat_e( "circle_to_circle_offset", 0.0f );

	rkLoader.LoadString_e( "loop_animation", "", szBuf, MAX_PATH );
	m_LoopAnimation = szBuf;
	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );
	m_dwLoopProtectTime  = (DWORD)rkLoader.LoadInt_e( "loop_protect_time", 0 );

	m_bDisableActionProtect = rkLoader.LoadInt_e( "disable_action_protect", false );

	rkLoader.LoadString_e( "move_key_on_magic_circle", "", szBuf, MAX_PATH );
	m_OnCircle = szBuf;
	rkLoader.LoadString_e( "move_key_off_magic_circle", "", szBuf, MAX_PATH );
	m_OffCircle = szBuf;

	rkLoader.LoadString_e( "red_guid_circle", "", szBuf, MAX_PATH );
	m_RedGuidCircle = szBuf;
	rkLoader.LoadString_e( "blud_guid_circle", "", szBuf, MAX_PATH );
	m_BlueGuidCircle = szBuf;

	rkLoader.LoadString_e( "skill_end_motion", "", szBuf, MAX_PATH );
	m_SkillEndMotion = szBuf;
	m_fEndMotionRate = rkLoader.LoadFloat_e( "end_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "start_effect", "", szBuf, MAX_PATH );
	m_StartEffect = szBuf;
	rkLoader.LoadString_e( "end_effect", "", szBuf, MAX_PATH );
	m_EndEffect = szBuf;

	m_iSummonCnt = rkLoader.LoadInt_e( "summon_cnt", 0 );

	m_bPreventRecoveryGauge = rkLoader.LoadBool_e( "prevent_recovery_gauge_on_skill", false );


	//
	m_DummyCharList.clear();
	m_vDestroyDummyCharList.clear();

	Help::LoadDummyCharCreateDestroyInfo( rkLoader, m_DummyCharList, m_vDestroyDummyCharList );

	m_DummyCreateType = (DummyCreateType)rkLoader.LoadInt_e( "dummy_create_type", DCT_DIRECT );
	m_bUseDestroyDummy = rkLoader.LoadBool_e( "use_destroy_dummy", false );

	m_fReduceSkillGauge = rkLoader.LoadFloat_e( "reduce_skill_gauge", 0.0f );

	m_dwLoopDuration = (DWORD)rkLoader.LoadInt_e( "loop_duration", 0 );
}

ioSkill* ioSummonWoodSkill::Clone()
{
	return new ioSummonWoodSkill( *this );
}

bool ioSummonWoodSkill::OnSkillStart( ioBaseChar *pChar )
{	
	if( !HasSkillMotion() )
		return false;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	pChar->AttachEffect( m_SkillEffectName );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	ClearData();

	pChar->SetSkillProtection();

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		SetLoopState( pChar );
	}

	SetCameraBuff( pChar );

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}
	
	m_dwCheckTime			= FRAMEGETTIME();
	m_fTicGaugeRateByGrowth	= FLOAT1;
	m_dwTicTimeByGrowth		= 0;

	// 성장관련 요소 추가.
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

	SetSkillNameChatBubble( pChar );

	if ( m_bUseDestroyDummy )
		g_DummyCharMgr.DestroyDummyCharByNameList( pChar, m_vDestroyDummyCharList );

	pChar->ReduceNeedGaugeBySkill( this, m_fReduceSkillGauge );

	return true;
}

void ioSummonWoodSkill::OnSkillEnd( ioBaseChar *pChar )
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

	pChar->EndMapEffect( m_dwCurMapEffect );
	pChar->DestroyMagicCircle();

	ClearData();
}

void ioSummonWoodSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	float fValue = 0.0f;
	DWORD dwCurTime = FRAMEGETTIME();
	float fCurRange = m_fCircleRange;

	float fCurTicGauge = m_fGaugePerTic / m_fTicGaugeRateByGrowth;
	int iTickGap = (int)( dwCurTime - m_dwCheckTime );
	int iCurTicTime = m_dwTicTime + m_dwTicTimeByGrowth;

	if( iTickGap >= iCurTicTime )
	{
		m_dwCheckTime = dwCurTime - ( iTickGap - iCurTicTime );

		if( fCurTicGauge != 0.0f && m_bReduceGauge )
		{
			if( pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), fCurTicGauge ) <= 0.0f )
			{
				SetEndState( pChar );
				return;
			}
		}
	}

	switch( m_State )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				SetLoopState( pChar );
			}
		}
		break;
	case SS_LOOP:
		{
			if ( m_dwLoopDuration > 0 && m_dwLoopStartTime + m_dwLoopDuration < dwCurTime )
			{
				SetEndState( pChar );
				return;
			}
			ChangeCircleDropZoneAndPositionGap( pChar, pStage );

			// 뱡향 변화 체크
			pChar->CheckChangeDirectionForMagicCircle();

			// MagicCircle 위치 갱신
			pChar->UpdateMagicCirclePosAll( m_fCircleMoveSpeed, fCurRange, m_fCircleHeightGap, false );

			// MagicCircle 위치방향으로 캐릭터 회전
			D3DXVECTOR3 vTargetPos = pChar->GetMagicCirclePos();
			pChar->SetTargetRotToTargetPos( vTargetPos, true, false );
			
			if( pChar->IsNeedProcess() )
			{
				if( pChar->IsAttackKey() && m_bEnableSummon )
				{
					SetActionState( pChar, pStage );					
				}
			}
		}
		break;
	case SS_ACTION:
		if( m_dwMotionEndTime < dwCurTime )
		{
			if( m_iCurSummonCnt < m_iSummonCnt )
				SetLoopState( pChar );
			else
			{
				if( m_fGaugePerTic > 0.0f )
				{
					pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), pChar->GetCurItemSkillGauge( GetOwnerItem() ) );
				}
				SetEndState( pChar );
			}
		}
		else
		{
			if( 0 < m_dwFireStartTime && m_dwFireStartTime < dwCurTime )
			{					
				CheckDummyChar( pChar, pStage );
				CheckAreaWeapon( pChar, pStage );
				m_dwFireStartTime = 0;
			}
		}
		break;
	case SS_END:
		break;
	}
}

bool ioSummonWoodSkill::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}

void ioSummonWoodSkill::SetMagicCircle( ioBaseChar *pChar )
{
	if( !pChar->CheckMagicCircle() )
	{
		// 팀 체크
		if( pChar->GetTeam() == TEAM_RED )
		{
			pChar->CreateMagicCircle( m_OnCircle, m_OnCircle, m_RedGuidCircle, m_fCircleOffSet );
		}
		else if( pChar->GetTeam() == TEAM_BLUE )
		{
			pChar->CreateMagicCircle( m_OnCircle, m_OnCircle, m_BlueGuidCircle, m_fCircleOffSet );
		}
		else
		{
			if( pChar->GetSingleConvertTeam() == TEAM_BLUE )
			{
				pChar->CreateMagicCircle( m_OnCircle, m_OnCircle, m_BlueGuidCircle, m_fCircleOffSet );
			}
			else
			{
				pChar->CreateMagicCircle( m_OnCircle, m_OnCircle, m_RedGuidCircle, m_fCircleOffSet );
			}
		}
	}
}

void ioSummonWoodSkill::ChangeCircleDropZoneAndPositionGap( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pStage ) return;
	if( !pChar ) return;

	D3DXVECTOR3 vPos = pChar->GetMagicCirclePos();

	if( pStage->GetMapHeightNoneEntity( vPos.x, vPos.z, vPos.y + 3.0f ) > 0.0f )
	{
		bool bOn = true;
		SummonList pList = pStage->GetSummonDummyInfoList();
		SummonList::iterator iter = pList.begin();
		for( ; iter != pList.end(); ++iter )
		{
			D3DXVECTOR3 vPos1 = (*iter).m_vPos;
			D3DXVECTOR3 vPos2 = pChar->GetMagicCirclePos();				
			D3DXVECTOR3 vRange = vPos1 - vPos2;
			float fRange = fabs(D3DXVec3Length( &vRange ));

			if( fRange < m_fCircleToCircleOffset )
				bOn = false;				
		}

		if( !bOn )
		{
			if( !m_bOffCircle && m_CurCircleEffect == m_OnCircle )
			{
				ioHashString szDummyCircle;
				pChar->CreateMagicCircle( m_OffCircle, m_OffCircle, szDummyCircle, 0 );
				pChar->SetMagicCirclePos( false, vPos );
				m_bOffCircle = true;
				m_CurCircleEffect = m_OffCircle;
			}
			
			m_bEnableSummon = false;
		}
		else
		{
			if( m_bOffCircle && m_CurCircleEffect == m_OffCircle )
			{	
				ioHashString szDummyCircle;
				pChar->CreateMagicCircle( m_OnCircle, m_OnCircle, szDummyCircle, 0 );
				pChar->SetMagicCirclePos( false, vPos );
				m_bOffCircle = false;
				m_CurCircleEffect = m_OnCircle;		
			}

			m_bEnableSummon = true;
		}		
	}
	else
	{
		if( !m_bOffCircle && m_CurCircleEffect == m_OnCircle )
		{
			ioHashString szDummyCircle;
			pChar->CreateMagicCircle( m_OffCircle, m_OffCircle, szDummyCircle, 0 );
			pChar->SetMagicCirclePos( false, vPos );
			m_bOffCircle = true;
			m_CurCircleEffect = m_OffCircle;
		}

		m_bEnableSummon = false;
	}
}

void ioSummonWoodSkill::SetNormalSkill( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	return;

	DWORD dwCurTime = FRAMEGETTIME();

	float fAniRate = GetSkillMotionRate();
	float fTimeRate = fAniRate * m_fExtraAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwMotionStartTime = dwCurTime;
	DWORD dwDuration = pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;
	m_dwMotionEndTime = m_dwMotionStartTime + dwDuration;
	m_dwMotionEndTime += m_dwAnimationEndDelay;

	m_dwFireStartTime = m_dwFireMotionEndTime = m_dwActionStartTime = dwCurTime;
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "fire_s" ) * fTimeRate;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "firemotion_e" ) * fTimeRate;

	if( m_dwFireStartTime >= m_dwFireMotionEndTime )
		m_dwFireMotionEndTime = m_dwMotionEndTime;

	pChar->AttachEffect( m_StartEffect );

	if( m_fGaugePerTic == 0.0f && !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
}

void ioSummonWoodSkill::SetActionState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_bCheckDropZone && !m_bEnableSummon )
		return;
	
	m_State = SS_ACTION;

	float fCurRange = m_fCircleRange;

	m_dwPreDelayStartTime = 0;
	if( pChar->CheckMagicCircle() )
	{
		pChar->UpdateMagicCirclePos( m_fCircleMoveSpeed, fCurRange, m_fCircleHeightGap, true, true );
	}

	m_vCirclePos = pChar->GetMagicCirclePos( m_fCircleOffSet, m_bCircleNoChangeY );
	
	// team circle
	ioMapEffect *pMapEffect = NULL;
	if( pChar->GetTeam() == TEAM_RED )
	{
		pMapEffect = pChar->CreateMapEffectBySkill( m_RedCircle, m_vCirclePos );
	}
	else if( pChar->GetTeam() == TEAM_BLUE )
	{
		pMapEffect = pChar->CreateMapEffectBySkill( m_BlueCircle, m_vCirclePos );
	}
	else
	{
		if( pChar->GetSingleConvertTeam() == TEAM_BLUE )
			pMapEffect = pChar->CreateMapEffectBySkill( m_BlueCircle, m_vCirclePos );
		else
			pMapEffect = pChar->CreateMapEffectBySkill( m_RedCircle, m_vCirclePos );
	}

	if( pMapEffect )
	{
		m_dwCurMapEffect = pMapEffect->GetUniqueID();
	}

	SetNormalSkill( pChar );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSS_ACTION;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSummonWoodSkill::SetLoopState( ioBaseChar *pChar )
{
	if( m_LoopAnimation.IsEmpty() || !pChar )
	{
		m_dwLoopStartTime = 0;
		m_dwFirstLoopStartTime = 0;
		return;
	}

	SetMagicCircle( pChar );

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( m_LoopAnimation, FLOAT100 );

	m_State = SS_LOOP;
	m_dwLoopStartTime = FRAMEGETTIME();
	if( m_dwFirstLoopStartTime == 0 )
		m_dwFirstLoopStartTime = m_dwLoopStartTime;

	EndPreDelayEffect( pChar );

	if( m_fGaugePerTic > 0.0f && ! m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
}


void ioSummonWoodSkill::SetEndState( ioBaseChar *pChar )
{
	if( m_State == SS_END )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_State = SS_END;
	pChar->DestroyMagicCircle();

	if( m_SkillEndMotion.IsEmpty() )
	{
		m_dwMotionStartTime = dwCurTime;
		m_dwMotionEndTime = m_dwMotionStartTime;
		return;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iCurSkillAniID = pGrp->GetAnimationIdx( m_SkillEndMotion );
	if( iCurSkillAniID == -1 )	return;

	float fCurEndRate = m_fEndMotionRate;
	if( fCurEndRate <= 0.0f )
		fCurEndRate = FLOAT1;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1/fCurEndRate );

	m_dwMotionStartTime = dwCurTime;
	m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID ) * fCurEndRate;
	m_dwMotionEndTime += m_dwAnimationEndDelay;

	pChar->AttachEffect( m_EndEffect );
}


void ioSummonWoodSkill::CheckDummyChar( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !pChar->IsNeedProcess() ) return;
	if( !pStage ) return;

	if( m_DummyCharList.empty() )
		return;


	if ( m_DummyCreateType == DCT_DIRECT )
	{
		m_dwFireStartTime = 0;

		g_DummyCharMgr.DestroyDummyCharByNameList( pChar, m_vDestroyDummyCharList );

		//
		vDummyCharCreateInfoList vCreateList;
		vCreateList.clear();

		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );

		D3DXVECTOR3 vSideDir;
		D3DXVec3Cross( &vSideDir, &ioMath::UNIT_Y, &vDir );
		D3DXVec3Normalize( &vSideDir, &vSideDir );

		int iSize = m_DummyCharList.size();
		for( int i=0; i < iSize; ++i )
		{
			float fOffset = m_DummyCharList[i].m_fDummyCharOffset;
			ioHashString szDummyCharName = m_DummyCharList[i].m_DummyCharName;
			float fSideOffset = m_DummyCharList[i].m_fDummyCharSideOffset;
			float fStartAngle = m_DummyCharList[i].m_fDummyCharStartAngle;
			int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();

			D3DXVECTOR3 vTargetPos =  pChar->GetMagicCirclePos() + (fOffset * vDir);
			vTargetPos += fSideOffset * vSideDir;

			ioDummyChar *pDummy = pStage->CreateDummyChar( szDummyCharName, iCurIndex,
				pChar->GetCharName(),
				vTargetPos,
				fStartAngle,
				0, true );

			DummyCharCreateInfo kInfo;
			kInfo.m_DummyCharName = szDummyCharName;
			kInfo.m_iDummyCharIdx = iCurIndex;
			kInfo.m_fDummyCharStartAngle = fStartAngle;
			kInfo.m_vTargetPos = vTargetPos;

			vCreateList.push_back( kInfo );

			pStage->SetSummonDummyInfo( pChar->GetCharName(), iCurIndex, pChar->GetMagicCirclePos() );
		}

		int iCreateCnt = vCreateList.size();
		if( pChar->IsNeedSendNetwork() && iCreateCnt > 0 )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SSS_CREATE_DUMMY;
			kPacket << pChar->GetTargetRot();

			kPacket << iCreateCnt;

			for( int i=0; i < iCreateCnt; ++i )
			{
				kPacket << vCreateList[i].m_DummyCharName;
				kPacket << vCreateList[i].m_iDummyCharIdx;
				kPacket << vCreateList[i].m_fDummyCharStartAngle;
				kPacket << vCreateList[i].m_vTargetPos;
			}
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		m_iCurSummonCnt++;
	}
	else if ( m_DummyCreateType == DCT_COUNT )
	{
		if ( COMPARE( m_iCurSummonCnt, 0, m_DummyCharList.size() ) )
		{
			m_dwFireStartTime = 0;

			//
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );

			D3DXVECTOR3 vSideDir;
			D3DXVec3Cross( &vSideDir, &ioMath::UNIT_Y, &vDir );
			D3DXVec3Normalize( &vSideDir, &vSideDir );

			float fOffset = m_DummyCharList[m_iCurSummonCnt].m_fDummyCharOffset;
			ioHashString szDummyCharName = m_DummyCharList[m_iCurSummonCnt].m_DummyCharName;
			float fSideOffset = m_DummyCharList[m_iCurSummonCnt].m_fDummyCharSideOffset;
			float fStartAngle = m_DummyCharList[m_iCurSummonCnt].m_fDummyCharStartAngle;
			int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();

			D3DXVECTOR3 vTargetPos =  pChar->GetMagicCirclePos() + (fOffset * vDir);
			vTargetPos += fSideOffset * vSideDir;

			ioDummyChar *pDummy = pStage->CreateDummyChar( szDummyCharName, iCurIndex,
				pChar->GetCharName(),
				vTargetPos,
				fStartAngle,
				0, true );

			pStage->SetSummonDummyInfo( pChar->GetCharName(), iCurIndex, pChar->GetMagicCirclePos() );

			if( pChar->IsNeedSendNetwork() && pDummy )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pChar->GetCharName();
				kPacket << GetName();
				kPacket << SSS_CREATE_COUNT_DUMMY;
				kPacket << pChar->GetTargetRot();

				kPacket << szDummyCharName;
				kPacket << iCurIndex;
				kPacket << fStartAngle;
				kPacket << vTargetPos;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}

			m_iCurSummonCnt++;
		}
	}
	
}

void ioSummonWoodSkill::CheckAreaWeapon( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;

	if( m_dwFireStartTime > 0 && m_dwFireStartTime < FRAMEGETTIME() )
	{
		// Create AreaWeapon
		if( !m_AreaWeaponName.IsEmpty() )
		{
			D3DXVECTOR3 vTargetPos =  pChar->GetMagicCirclePos();

			ioAreaWeapon *pArea = g_AreaWeaponMgr.CreateAreaWeapon( pChar->GetCharName(),
																	"",
																	GetName(),
																	m_AreaWeaponName,
																	vTargetPos,
																	pChar->GetTargetRot(),
																	ioAreaWeapon::CT_NORMAL );

			if( pChar->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pChar->GetCharName();
				kPacket << GetName();
				kPacket << SSS_CREATE_AREA_WEAPON;
				kPacket << m_AreaWeaponName;
				kPacket << vTargetPos;
				kPacket << (int)ioAreaWeapon::CT_NORMAL;
				kPacket << pChar->GetTargetRot();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
		m_dwFireStartTime = 0;
	}
}

void ioSummonWoodSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar )	return;
	if( !pStage )	return;

	int iSyncState;
	rkPacket >> iSyncState;

	switch( iSyncState )
	{
	case SSS_ACTION:
		SetActionState( pChar, pStage );
		break;
	case SSS_CREATE_DUMMY:
		{
			g_DummyCharMgr.DestroyDummyCharByNameList( pChar, m_vDestroyDummyCharList );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pChar->SetTargetRotToRotate( qtRot, true, false );

			int iCreateCnt;
			rkPacket >> iCreateCnt;

			for( int i=0; i < iCreateCnt; ++i )
			{
				int iCurIndex;
				ioHashString szDummyCharName;
				float fStartAngle;
				D3DXVECTOR3 vTargetPos;

				rkPacket >> szDummyCharName >> iCurIndex;
				rkPacket >> fStartAngle;
				rkPacket >> vTargetPos;

				m_dwFireStartTime = 0;

				g_DummyCharMgr.SetCurLocalIndex( iCurIndex );

				ioDummyChar *pDummy = pStage->CreateDummyChar( szDummyCharName, iCurIndex,
																pChar->GetCharName(),
																vTargetPos,
																fStartAngle,
																0, false );

				if( pDummy )
					pDummy->SetControlSkillName( GetName() );
			}
		}
		break;
	case SSS_CREATE_AREA_WEAPON:
		{
			ioHashString szAreaWeaponName;
			rkPacket >> szAreaWeaponName;

			D3DXVECTOR3 vCreatePos;
			rkPacket >> vCreatePos;

			int iCreateType;
			rkPacket >> iCreateType;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			ioAreaWeapon *pArea = g_AreaWeaponMgr.CreateAreaWeapon( pChar->GetCharName(),
																	"",
																	GetName(),
																	szAreaWeaponName,
																	vCreatePos,
																	qtRot,
																	iCreateType );
		}
		break;
	case SSS_CREATE_COUNT_DUMMY:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pChar->SetTargetRotToRotate( qtRot, true, false );

			int iCurIndex;
			ioHashString szDummyCharName;
			float fStartAngle;
			D3DXVECTOR3 vTargetPos;

			rkPacket >> szDummyCharName >> iCurIndex;
			rkPacket >> fStartAngle;
			rkPacket >> vTargetPos;

			m_dwFireStartTime = 0;

			g_DummyCharMgr.SetCurLocalIndex( iCurIndex );

			ioDummyChar *pDummy = pStage->CreateDummyChar( szDummyCharName, iCurIndex,
				pChar->GetCharName(),
				vTargetPos,
				fStartAngle,
				0, false );

			if( pDummy )
				pDummy->SetControlSkillName( GetName() );
		}
		break;
	}
}

bool ioSummonWoodSkill::IsEndState() const
{
	if( m_State == SS_END )
		return true;

	return false;
}

bool ioSummonWoodSkill::IsProtected( int iDefenseBreakType ) const
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

	if( m_dwSkillProtectTime > 0 && m_dwSkillStartTime + m_dwSkillProtectTime  > FRAMEGETTIME() )
	{
		return true;
	}

	switch( m_State )
	{
	case SS_NONE:
		return false;
	case SS_LOOP:
	case SS_ACTION:
		{
			if( m_dwFirstLoopStartTime > 0 &&
				m_dwLoopProtectTime > 0 &&
				m_dwFirstLoopStartTime + m_dwLoopProtectTime > FRAMEGETTIME() )
				return true;

			if( m_bDisableActionProtect )
				return false;

			if( m_dwFireMotionEndTime > FRAMEGETTIME() )
				return true;
		}
		break;
	case SS_END:
		return false;
	}

	return false;
}

bool ioSummonWoodSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_State == SS_END && m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
		return true;

	return false;
}

bool ioSummonWoodSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}