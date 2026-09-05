#include "StdAfx.h"
#include "ioRandomGenerateMoveSkill.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioMagicStaffItem.h"
#include "ioGrowthUpInfo.h"

ioRandomGenerateMoveSkill::ioRandomGenerateMoveSkill(void)
{
	ClearData();
}

ioRandomGenerateMoveSkill::ioRandomGenerateMoveSkill( const ioRandomGenerateMoveSkill &rhs ):
ioAttackSkill( rhs ),
m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
m_fSkillLoopMotionRate( rhs.m_fSkillLoopMotionRate ),
m_dwLoopDuration( rhs.m_dwLoopDuration ),
//Magic Cirlce
m_ReadyCircle( rhs.m_ReadyCircle ),
m_fCircleRadius( rhs.m_fCircleRadius ),
m_fMagicCircleOffSet( rhs.m_fMagicCircleOffSet ),
m_fMagicCircleMoveSpeed( rhs. m_fMagicCircleMoveSpeed ),
m_fMagicCircleHeightGap( rhs. m_fMagicCircleHeightGap ),
m_fMagicCircleRange( rhs. m_fMagicCircleRange ),
m_RedCircleEffect( rhs.m_RedCircleEffect ),
m_BlueCircleEffect( rhs.m_BlueCircleEffect ),
//Action
m_dwMinGenerateTime( rhs.m_dwMinGenerateTime ),
m_dwMaxGenerateTime( rhs.m_dwMaxGenerateTime ),
m_fGenerateStartHeight( rhs.m_fGenerateStartHeight ),
m_bDisableGrowth( rhs.m_bDisableGrowth ),
m_fGuagePerTic( rhs.m_fGuagePerTic ),
m_dwTicTime( rhs.m_dwTicTime )
{
	ClearData();
}

ioRandomGenerateMoveSkill::~ioRandomGenerateMoveSkill(void)
{
}

ioSkill* ioRandomGenerateMoveSkill::Clone()
{
	return new ioRandomGenerateMoveSkill( *this );
}

void ioRandomGenerateMoveSkill::LoadProperty( ioINILoader &rkLoader )
{

	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	//Loop
	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_SkillLoopMotion = szBuf;
	m_fSkillLoopMotionRate = rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );
	m_dwLoopDuration = rkLoader.LoadInt_e( "skill_loop_duration", 0 );

	//Circle
	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;

	rkLoader.LoadString_e( "red_team_circle_effect", "", szBuf, MAX_PATH );
	m_RedCircleEffect = szBuf;
	rkLoader.LoadString_e( "blue_team_circle_effect", "", szBuf, MAX_PATH );
	m_BlueCircleEffect = szBuf;

	m_fCircleRadius = rkLoader.LoadFloat_e( "circle_radius", 0.0f );
	m_fMagicCircleOffSet = rkLoader.LoadFloat_e( "circle_offset", 0.0f );
	m_fMagicCircleMoveSpeed = rkLoader.LoadFloat_e( "circle_move_speed", 0.0f );
	m_fMagicCircleHeightGap = rkLoader.LoadFloat_e( "circle_height_gap", 0.0f );
	m_fMagicCircleRange = rkLoader.LoadFloat_e( "circle_range", 0.0f );

	//Action
	m_dwMinGenerateTime = rkLoader.LoadInt_e( "min_generate_time", 0.0f );
	m_dwMaxGenerateTime = rkLoader.LoadInt_e( "max_generate_time", 0.0f );
	if (m_dwMinGenerateTime > m_dwMaxGenerateTime)
		LOG.PrintTimeAndLog( 0, "ioRandomGenerateSkill::LoadProperty - ini error! max_generate_time < min_generate_time " );
	m_fGenerateStartHeight = rkLoader.LoadFloat_e( "generate_start_height", FLOAT100 );

	m_bDisableGrowth = rkLoader.LoadBool_e( "disable_growth", false );
	m_fGuagePerTic = rkLoader.LoadFloat_e( "gauge_per_tick", 0.0f );
	m_dwTicTime = (DWORD)rkLoader.LoadInt_e( "tick_time", 0 );
}

bool ioRandomGenerateMoveSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( false );
	m_bSetHandMesh = false;
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	SetCameraBuff( pChar );

	ClearData();

	if ( GetOwnerItem() )
	{
		ioMagicStaffItem *pStaff = ToMagicStaffItem( GetOwnerItem() );
		if( pStaff )
			m_fCurCircleRadius = pStaff->GetCircleRadius();

		if ( m_fCurCircleRadius <= 0.0f )
			m_fCurCircleRadius = m_fCircleRadius;

		m_fCurCircleOffset = GetOwnerItem()->GetMagicCircleOffSet();
		if( m_fCurCircleOffset == 0.0f )
			m_fCurCircleOffset = m_fMagicCircleOffSet;
	}

	m_vCirclePos = pChar->GetMagicCirclePos( m_fCurCircleOffset, m_bCircleNoChangeY );

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}


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

void ioRandomGenerateMoveSkill::ClearData()
{
	m_SkillState = SS_NONE;
	m_fCurCircleOffset = 0.0f;
	m_fCurCircleRadius = 0.0f;
	m_vCirclePos = ioMath::VEC3_ZERO;
	m_dwLoopStartTime = 0;
	m_dwCurMapEffect = -1;

	m_dwGenerateStartTime = 0;
	m_dwNextGenerateTime = 0;
	m_vGenerateList.clear();

	m_dwCheckTime = 0;
	m_fTicGaugeRateByGrowth = FLOAT1;
	m_dwTicTimeByGrowth = 0;
}

void ioRandomGenerateMoveSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) 
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

				CreateMagicCircle( pChar );

				if ( !m_SkillLoopMotion.IsEmpty() )
					SetWaitState( pChar );
				else
					SetActionState( pChar );
			}
		}
		break;
	case SS_WAIT:
		{
			if( UpdateMagicCircle( pChar ) )
				CheckRotate( pChar );

			if( pChar->IsNeedProcess() && pChar->IsAttackKey() )
				SetActionState( pChar );

			if( m_dwLoopDuration > 0 && m_dwLoopStartTime + m_dwLoopDuration < dwCurTime )
			{
				SetActionState( pChar );
				return;
			}
		}
		break;
	case SS_ACTION:
		{
			if( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
			{
				SetDeadState( pChar );
				return;
			}

			float fCurTicGauge = m_fGuagePerTic / m_fTicGaugeRateByGrowth;
			if ( fCurTicGauge > 0.0f )
			{
				float fTimePerSec = g_FrameTimer.GetSecPerFrame();

				int iTickGap = (int)( dwCurTime - m_dwCheckTime );
				int iCurTicTime = m_dwTicTime + m_dwTicTimeByGrowth;
				if( iTickGap >= iCurTicTime )
				{
					m_dwCheckTime = dwCurTime - ( iTickGap - iCurTicTime );

					if( pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), fCurTicGauge ) <= 0.0f )
					{
						SetDeadState( pChar );
						return;
					}
				}
			}

			if( UpdateMagicCircle( pChar ) )
			{
				CheckRotate( pChar );
				UpdateEffect( pChar, pStage );
			}

			if( pChar->IsNeedProcess() )
				FireWeapon( pChar, pStage );
		}
		break;
	}
}

void ioRandomGenerateMoveSkill::OnSkillEnd( ioBaseChar *pChar )
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

	if ( m_dwCurMapEffect != - 1 )
		pChar->EndMapEffect( m_dwCurMapEffect );
	pChar->DestroyMagicCircle();
	m_vGenerateList.clear();

	ClearData();
}

void ioRandomGenerateMoveSkill::SetWaitState( ioBaseChar *pChar )
{
	if( !pChar ) 
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if (!pGrp)
		return;

	m_SkillState = SS_WAIT;
	DWORD dwCurTime = FRAMEGETTIME();
	EndPreDelayEffect( pChar );
	m_dwLoopStartTime = dwCurTime;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_SkillLoopMotion );
	float fAniRate = FLOAT1;

	if( m_fSkillLoopMotionRate > 0.0f )
		fAniRate = m_fSkillLoopMotionRate;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );
}

void ioRandomGenerateMoveSkill::CreateMagicCircle( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	if( !pChar->CheckMagicCircle() )
	{
		// 팀 체크
		if( pChar->GetTeam() == TEAM_RED )
			pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, "", m_fCurCircleOffset );
		else if( pChar->GetTeam() == TEAM_BLUE )
			pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, "", m_fCurCircleOffset );
		else
		{
			if( pChar->GetSingleConvertTeam() == TEAM_BLUE )			
				pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, "", m_fCurCircleOffset );
			else
				pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, "", m_fCurCircleOffset );
		}
	}
	pChar->CheckMagicCircleVisible( false );
}

bool ioRandomGenerateMoveSkill::UpdateMagicCircle( ioBaseChar *pChar )
{
	if ( !pChar )
		return false;

	if( pChar->CheckMagicCircle() )
	{
		//뱡향 체크
		pChar->CheckChangeDirectionForMagicCircle();
		//위치 갱신
		pChar->UpdateMagicCirclePosAll( m_fMagicCircleMoveSpeed, m_fMagicCircleRange, m_fMagicCircleHeightGap, false );
	}

	D3DXVECTOR3 vCurPos = pChar->GetMagicCirclePos( m_fCurCircleOffset, m_bCircleNoChangeY );
	if ( m_vCirclePos != vCurPos )
	{
		m_vCirclePos = vCurPos;
		return true;
	}

	return false;
}

void ioRandomGenerateMoveSkill::CheckRotate( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	//마법진 방향으로 캐릭터 회전
	pChar->SetTargetRotToTargetPos( m_vCirclePos, true, false );
}

void ioRandomGenerateMoveSkill::SetActionState( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return;

	m_vCirclePos = pChar->GetMagicCirclePos( m_fCurCircleOffset, m_bCircleNoChangeY );

	CreateEffect( pChar );

	AttackSkillFire( pChar );

	if( pChar->IsNeedSendNetwork() )
	{
		int iGenerateCnt = m_vGenerateList.size();
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << static_cast<BYTE>( SS_ACTION );
		kPacket << m_vCirclePos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioRandomGenerateMoveSkill::CreateEffect( ioBaseChar *pChar )
{
	ioMapEffect *pMapEffect = NULL;
	if( pChar->GetTeam() == TEAM_RED )
	{
		pMapEffect = pChar->CreateMapEffectBySkill( m_RedCircleEffect, m_vCirclePos );
	}
	else if( pChar->GetTeam() == TEAM_BLUE )
	{
		pMapEffect = pChar->CreateMapEffectBySkill( m_BlueCircleEffect, m_vCirclePos );
	}
	else
	{
		if( pChar->GetSingleConvertTeam() == TEAM_BLUE )
			pMapEffect = pChar->CreateMapEffectBySkill( m_BlueCircleEffect, m_vCirclePos );
		else
			pMapEffect = pChar->CreateMapEffectBySkill( m_RedCircleEffect, m_vCirclePos );
	}

	if( pMapEffect )
		m_dwCurMapEffect = pMapEffect->GetUniqueID();
}

void ioRandomGenerateMoveSkill::AttackSkillFire( ioBaseChar *pChar )
{
	if ( !pChar )
		return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	m_SkillState = SS_ACTION;

	ioHashString szSkillMotion = GetSkillMotionName();
	DWORD dwCurTime = FRAMEGETTIME();

	int iAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iAniID == -1 )
		return;

	pChar->AttachEffect( m_SkillEffectName );

	float fTimeRate  = GetAttackTimeRate();
	DWORD dwPreDelay = GetAttribute().m_dwPreDelay;
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( GetAttribute().m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
		float fWaitTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
		float fEndTimeRate = GetAttribute().m_fAttackEndAniRate;
		pGrp->SetActionAni( GetAttribute().m_AttackEndAnimation,
			0.0f, FLOAT100,
			1.0f, FLOAT1/fEndTimeRate,
			fWaitTime, 0.0f, true );
	}

	DWORD dwFireStartTime;
	dwFireStartTime = m_dwFireMotionEndTime = FRAMEGETTIME();
	dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate + dwPreDelay;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;
	if( dwFireStartTime >= m_dwFireMotionEndTime )
		m_dwFireMotionEndTime = m_dwMotionEndTime;

	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + GetAttribute().m_dwEndDelay;

	m_dwCheckTime = dwCurTime;

	if( pChar->IsNeedProcess() )
	{
		m_dwGenerateStartTime = dwFireStartTime;
		m_dwNextGenerateTime = m_dwGenerateStartTime;
		GenerateWeapon( pChar );
	}
}

void ioRandomGenerateMoveSkill::GenerateWeapon( ioBaseChar *pChar )
{
	DWORD dwNextTime = m_dwGenerateStartTime;
	m_vGenerateList.clear();
	if (!pChar)
		return;

	IORandom random;
	random.SetRandomSeed( pChar->GetRandomSeed() );

	if ( GetAttribute().m_vWeaponInfoList.empty() )
	{
		LOG.PrintTimeAndLog( 0, "ioRandomGenerate2Skill::GenerateWeapon - WeaponAttribute is not exist");
		return;
	}
	int iWeaponInfoCnt =  GetAttribute().m_vWeaponInfoList.size();
	int iIndex = 0;

	while ( dwNextTime <= m_dwMotionEndTime )
	{
		//Generate Pos
		GenerateInfo eGenerateInfo;
		int iAngle = random.Random( 360 );
		int iRange = (int)m_fCurCircleRadius;
		if (iRange < 0)
			iRange = 0;
		iRange = random.Random(iRange);

		int iXPos = cos((double) iAngle ) * iRange;
		int iZPos = sin((double) iAngle ) * iRange;

		D3DXVECTOR3 vOffset = ioMath::VEC3_ZERO;
		vOffset.x += iXPos;
		vOffset.y += m_fGenerateStartHeight;
		vOffset.z += iZPos;

		//Generate NextTime
		DWORD dwGapTime = 0;
		if( !m_vGenerateList.empty() )
		{
			dwGapTime = m_dwMinGenerateTime;
			dwGapTime += random.Random(m_dwMaxGenerateTime - m_dwMinGenerateTime);
		}

		dwNextTime += dwGapTime;

		eGenerateInfo.vPos = vOffset;
		eGenerateInfo.dwGapTime = dwGapTime;
		eGenerateInfo.iAttributeIndex = iIndex;
		iIndex++;

		if ( iIndex == iWeaponInfoCnt )
			iIndex = 0;

		m_vGenerateList.push_back( eGenerateInfo );
	}
}

void ioRandomGenerateMoveSkill::UpdateEffect( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage )
		return;

	ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwCurMapEffect );
	if( !pMapEffect )
		return;

	pMapEffect->SetWorldPosition( m_vCirclePos );
}

void ioRandomGenerateMoveSkill::SetDeadState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;
	if ( m_dwCurMapEffect != -1 )
	{
		pChar->EndMapEffect( m_dwCurMapEffect );
		m_dwCurMapEffect = -1;
	}

	pChar->DestroyMagicCircle();
	SetMotionTime( 0, 0 );
	m_SkillState = SS_END;
}

void ioRandomGenerateMoveSkill::FireWeapon( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	D3DXVECTOR3 vPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	int iIndex = 0;
	bool bFire =false;
	GenerateList::iterator iter = m_vGenerateList.begin();
	while ( iter != m_vGenerateList.end() )
	{
		if( m_dwNextGenerateTime > 0 && m_dwNextGenerateTime <= dwCurTime )
		{
			//G Weapon
			iIndex = (*iter).iAttributeIndex;
			vPos = m_vCirclePos + iter->vPos ;
			pChar->SkillFireExplicit( GetAttribute().m_vWeaponInfoList[iIndex],
				GetName(),
				m_dwWeaponBaseIndex++,
				vPos );

			iter = m_vGenerateList.erase( iter );
			bFire = true; 
			if( iter != m_vGenerateList.end() )
				m_dwNextGenerateTime += iter->dwGapTime;
		}
		else
			break;
	}

	if ( bFire )
	{
		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << static_cast<BYTE>( SS_FIRE );
			kPacket << vPos;
			kPacket << iIndex;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

bool ioRandomGenerateMoveSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	if( m_SkillState == SS_END && m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
		return true;

	return false;
}

void ioRandomGenerateMoveSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if ( !pChar || !pStage )
		return;
	BYTE btState;
	rkPacket >> btState;
	switch( btState )
	{
	case SS_ACTION:
		{
			rkPacket >> m_vCirclePos;
			CreateEffect( pChar );

			AttackSkillFire( pChar );
		}
		break;
	case SS_FIRE:
		OnGenerateList( pChar, rkPacket );
		break;
	}
}

void ioRandomGenerateMoveSkill::OnGenerateList( ioBaseChar *pChar, SP2Packet &rkPacket )
{
	if ( !pChar )
		return;

	D3DXVECTOR3 vPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	int iIndex = 0;

	rkPacket >> vPos;
	rkPacket >> iIndex;

	const WeaponInfoList& Info = GetAttribute().m_vWeaponInfoList;
	if ( (int)Info.size() <= iIndex )
		iIndex = 0;
	pChar->SkillFireExplicit( Info[iIndex], GetName(), m_dwWeaponBaseIndex++, vPos );
}
