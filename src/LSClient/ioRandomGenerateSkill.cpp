

#include "stdafx.h"

#include "ioRandomGenerateSkill.h"

#include "ioBaseChar.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioRandomGenerateSkill::ioRandomGenerateSkill()
{
	m_dwGenerateStartTime = 0;
	m_dwNextGenerateTime = 0;
	m_dwCircleEnd = 0;
	m_bSendGenerateList = false;
	m_dwSeed = 0;

	m_dwCurMapEffect = -1;

	m_bSetHandMesh = false;
	m_iCurGenerateCnt = 0;
}

ioRandomGenerateSkill::ioRandomGenerateSkill( const ioRandomGenerateSkill &rhs )
: ioAttackSkill( rhs ),
 m_fCircleOffSet( rhs.m_fCircleOffSet ),
 m_fCircleRadius( rhs.m_fCircleRadius ),
 m_RedCircle( rhs.m_RedCircle ),
 m_BlueCircle( rhs.m_BlueCircle ),
 m_dwMinGenerateTime( rhs.m_dwMinGenerateTime ),
 m_dwMaxGenerateTime( rhs.m_dwMaxGenerateTime ),
 m_fGenerateStartHeight( rhs.m_fGenerateStartHeight ),
 m_dwCircleDuration( rhs.m_dwCircleDuration )
{
	m_dwGenerateStartTime = 0;
	m_dwNextGenerateTime = 0;
	m_dwCircleEnd = 0;
	m_dwSeed = 0;

	m_bSendGenerateList = false;

	m_dwCurMapEffect = -1;
	m_bSetHandMesh = false;
	m_iCurGenerateCnt = 0;
}

ioRandomGenerateSkill::~ioRandomGenerateSkill()
{
	m_vGenerateList.clear();
}

void ioRandomGenerateSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "red_team_circle", "", szBuf, MAX_PATH );
	m_RedCircle = szBuf;
	rkLoader.LoadString_e( "blue_team_circle", "", szBuf, MAX_PATH );
	m_BlueCircle = szBuf;

	m_fCircleRadius = rkLoader.LoadFloat_e( "circle_radius", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "circle_offset", 0.0f );

	m_dwCircleDuration = rkLoader.LoadInt_e( "circle_duration", 0 );
	m_fGenerateStartHeight = rkLoader.LoadFloat_e( "generate_start_height", FLOAT100 );
	m_dwMinGenerateTime = rkLoader.LoadInt_e( "min_generate_time", 0.0f );
	m_dwMaxGenerateTime = rkLoader.LoadInt_e( "max_generate_time", 0.0f );
}

ioSkill* ioRandomGenerateSkill::Clone()
{
	return new ioRandomGenerateSkill( *this );
}

bool ioRandomGenerateSkill::IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg )
{
	ioAttackableItem *pAttackable = ToAttackableItem( GetOwnerItem() );
	if( pAttackable )
	{
		if( !pAttackable->IsCanFire( pChar ) )
			return false;
	}

	if( !CheckNeedGauge( pChar, bHelpMsg ) )
		return false;

	return true;
}

bool ioRandomGenerateSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	m_bSetHandMesh = false;
	m_iCurGenerateCnt = 0;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	ioMagicStaffItem *pStaff = ToMagicStaffItem( GetOwnerItem() );
	if( pStaff )
	{
		m_fCircleRadius = pStaff->GetCircleRadius();
	}

	ioFireMageExpansionItem *pFireMageExpansionItem = ToFireMageExpansionItem( GetOwnerItem() );
	if( pFireMageExpansionItem )
	{
		m_fCircleRadius = pFireMageExpansionItem->GetCircleRadius();
	}

	// 2020-02-14
	ioNuClearSoldierItem* pNuClearSoldierItem = ToNuClearSoldierItem( GetOwnerItem() );
	if( pNuClearSoldierItem )
	{
		m_fCircleRadius = pNuClearSoldierItem->GetCircleRadius();
	}

	if( pChar->CheckMagicCircle() )
	{
		pChar->CheckMagicCircleVisible( false );
		pChar->UpdateMagicCirclePos( 0.0f, 0.0f, 0.0f, true, true );
	}

	float fCurOffSet = GetOwnerItem()->GetMagicCircleOffSet();
	if( fCurOffSet == 0.0f )
		fCurOffSet = m_fCircleOffSet;

	m_vCirclePos = pChar->GetMagicCirclePos( fCurOffSet, m_bCircleNoChangeY );
	
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
		DWORD dwPreDelayTime = GetPreDelayTime();

		m_dwCurMapEffect = pMapEffect->GetUniqueID();
		m_dwCircleEnd = FRAMEGETTIME() + dwPreDelayTime + m_dwCircleDuration;
	}

	m_dwGenerateStartTime = 0;

	return true;
}

void ioRandomGenerateSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	ioAttackSkill::OnProcessState( pChar, pStage );

	if( m_dwCircleEnd > 0 && m_dwCircleEnd < FRAMEGETTIME() )
	{
		//DestroyMapEffect
		pChar->EndMapEffect( m_dwCurMapEffect );
		pChar->DestroyMagicCircle();
		m_dwCircleEnd = 0;
	}

	if( m_dwGenerateStartTime > 0 && m_dwGenerateStartTime <= FRAMEGETTIME() )
	{
		if( !m_bSendGenerateList && pChar->IsNeedProcess() )
		{
			m_bSendGenerateList = true;

			int iGenerateCnt = m_vGenerateList.size();

			//send GenerateList
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << g_GlobalTimer.GetGlobalTime();
			kPacket << iGenerateCnt;
			kPacket << m_dwSeed;
			kPacket << m_GeneratePos;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	else
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	GenerateList::iterator iter = m_vGenerateList.begin();
	while ( iter != m_vGenerateList.end() )
	{
		if( m_dwNextGenerateTime > 0 && m_dwNextGenerateTime <= dwCurTime )
		{
			//G Weapon
			int iIndex = (*iter).iAttributeIndex;

			pChar->SkillFireExplicit( GetAttribute().m_vWeaponInfoList[iIndex],
									  GetName(),
									  m_dwWeaponBaseIndex+m_iCurGenerateCnt,
									  iter->vPos );

			iter = m_vGenerateList.erase( iter );

			if( iter != m_vGenerateList.end() )
			{
				m_dwNextGenerateTime += iter->dwGapTime;
				m_iCurGenerateCnt++;
			}
		}
		else
			break;
	}
}

void ioRandomGenerateSkill::AttackSkillFire( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iAniID == -1 )	return;

	pChar->AttachEffect( m_SkillEffectName );

	float fTimeRate  = GetAttackTimeRate();
	DWORD dwPreDelay = GetAttribute().m_dwPreDelay;
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	
	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + GetAttribute().m_dwEndDelay;

	DWORD dwFireStartTime;
	dwFireStartTime = m_dwFireMotionEndTime = FRAMEGETTIME();
	dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate + dwPreDelay;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	m_dwEnableReserveTime = FRAMEGETTIME();
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	if( dwFireStartTime >= m_dwFireMotionEndTime )
	{
		m_dwFireMotionEndTime = m_dwMotionEndTime;
	}

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

		int iEndAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackEndAnimation );
		m_dwMotionEndTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndTimeRate;
	}	

	if( pChar->IsNeedProcess() )
	{
		m_dwGenerateStartTime = FRAMEGETTIME() + dwPreDelay;

		float fFireTime = pChar->GetFirstFireTime( szSkillMotion );
		m_dwGenerateStartTime += fFireTime * GetAttribute().m_fAttackAniRate;
		m_dwNextGenerateTime = m_dwGenerateStartTime;
		GenerateBlizzard( pChar );
	}

	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;
}

void ioRandomGenerateSkill::OnSkillEnd( ioBaseChar *pChar )
{
	//DestroyMapEffect
	pChar->EndMapEffect( m_dwCurMapEffect );
	pChar->DestroyMagicCircle();

	m_vGenerateList.erase( m_vGenerateList.begin(), m_vGenerateList.end() );
	m_bSendGenerateList = false;

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}


	if( !m_HandMesh.IsEmpty() )
	{
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	}
	m_bSetHandMesh = false;

	ioAttackSkill::OnSkillEnd( pChar );
}

void ioRandomGenerateSkill::GenerateBlizzard( ioBaseChar *pChar )
{
	DWORD dwNextTime = m_dwGenerateStartTime;
	m_vGenerateList.clear();

	/*
	if( m_dwCurMapEffect == -1 )
		return;
	*/

	m_dwSeed = timeGetTime();
	IORandom random;
	random.SetRandomSeed( m_dwSeed );
	
	m_GeneratePos = m_vCirclePos;

	int iWeaponInfoCnt =  GetAttribute().m_vWeaponInfoList.size();
	while ( dwNextTime <= m_dwMotionEndTime )
	{
		//Generate Pos
		GenerateInfo eGenerateInfo;
		int iAngle = random.Random( 361 );
		int iRange = (int)(m_fCircleRadius-20.0f);
		iRange = random.Random(iRange);

		int iXPos = cos((double) iAngle ) * iRange;
		int iZPos = sin((double) iAngle ) * iRange;

		D3DXVECTOR3 vPos = m_GeneratePos;
		vPos.x += iXPos;
		vPos.y += m_fGenerateStartHeight;
		vPos.z += iZPos;

		//Generate NextTime
		DWORD dwGapTime = 0;
		if( !m_vGenerateList.empty() )
		{
			dwGapTime = m_dwMinGenerateTime;
			dwGapTime += random.Random(m_dwMaxGenerateTime - m_dwMinGenerateTime);
		}

		dwNextTime += dwGapTime;

		// Index
		int iIndex = random.Random( iWeaponInfoCnt );

		eGenerateInfo.vPos = vPos;
		eGenerateInfo.dwGapTime = dwGapTime;
		eGenerateInfo.iAttributeIndex = iIndex;

		m_vGenerateList.push_back( eGenerateInfo );
	}
}

void ioRandomGenerateSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	OnGenerateList( pChar, rkPacket );
}

void ioRandomGenerateSkill::OnGenerateList( ioBaseChar *pChar, SP2Packet &rkPacket )
{
	DWORD dwStartTime;
	rkPacket >> dwStartTime;

	DWORD dwGapTime = g_GlobalTimer.CalcGlobalGapTime( dwStartTime );
	m_dwGenerateStartTime = FRAMEGETTIME();
	m_dwGenerateStartTime -= dwGapTime;
	m_dwNextGenerateTime = m_dwGenerateStartTime;

	int iGenerateCnt;
	rkPacket >> iGenerateCnt;
	rkPacket >> m_dwSeed;
	rkPacket >> m_GeneratePos;

	m_vGenerateList.clear();
	if( iGenerateCnt > 0 )
	{
		m_vGenerateList.reserve( iGenerateCnt );
	}

	IORandom random;
	random.SetRandomSeed( m_dwSeed );

	DWORD dwNextTime = m_dwGenerateStartTime;

	int iWeaponInfoCnt = GetAttribute().m_vWeaponInfoList.size();
	for( int i=0; i < iGenerateCnt; i++ )
	{
		GenerateInfo eGenerate;

		int iAngle = random.Random( 361 );
		int iRange = (int)( m_fCircleRadius - 20.0f );
		iRange = random.Random(iRange);

		int iXPos = cos((double) iAngle ) * iRange;
		int iZPos = sin((double) iAngle ) * iRange;

		D3DXVECTOR3 vPos = m_GeneratePos;
		vPos.x += iXPos;
		vPos.y += m_fGenerateStartHeight;
		vPos.z += iZPos;

		//Generate NextTime
		DWORD dwGapTime = 0;
		if( !m_vGenerateList.empty() )
		{
			dwGapTime = m_dwMinGenerateTime;
			dwGapTime += random.Random(m_dwMaxGenerateTime - m_dwMinGenerateTime);
		}

		dwNextTime += dwGapTime;

		// Index
		int iIndex = random.Random( iWeaponInfoCnt );

		eGenerate.vPos = vPos;
		eGenerate.dwGapTime = dwGapTime;
		eGenerate.iAttributeIndex = iIndex;

		m_vGenerateList.push_back( eGenerate );
	}
}

