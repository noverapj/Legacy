#include "StdAfx.h"
#include "ioRandomGenerate2Skill.h"

#include "ioBaseChar.h"
#include "ioGrowthUpInfo.h"

ioRandomGenerate2Skill::ioRandomGenerate2Skill(void)
{
	m_dwGenerateStartTime = 0;
	m_dwNextGenerateTime = 0;
	m_dwCircleEnd = 0;
		
	m_dwCurMapEffect = -1;

	m_bSetHandMesh = false;
	m_vWeaponMoveDir = D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f );

	m_State = SS_NONE;

	m_ReadyCircle.Clear();
	m_dwLoopStartTime = 0;
}

ioRandomGenerate2Skill::ioRandomGenerate2Skill( const ioRandomGenerate2Skill &rhs ) :
ioAttackSkill(rhs),
 m_fMagicCircleOffSet( rhs.m_fMagicCircleOffSet ),
 m_fCircleRadius( rhs.m_fCircleRadius ),
 m_RedCircle( rhs.m_RedCircle ),
 m_BlueCircle( rhs.m_BlueCircle ),
 m_dwMinGenerateTime( rhs.m_dwMinGenerateTime ),
 m_dwMaxGenerateTime( rhs.m_dwMaxGenerateTime ),
 m_fGenerateStartHeight( rhs.m_fGenerateStartHeight ),
 m_dwCircleDuration( rhs.m_dwCircleDuration ),
 m_vWeaponMoveDir( rhs.m_vWeaponMoveDir ),
 m_State( rhs.m_State ),
 m_ReadyCircle( rhs.m_ReadyCircle ),
 m_fMagicCircleMoveSpeed( rhs. m_fMagicCircleMoveSpeed ),
 m_fMagicCircleHeightGap( rhs. m_fMagicCircleHeightGap ),
 m_fMagicCircleRange( rhs. m_fMagicCircleRange ),
 m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
 m_fSkillLoopMotionRate( rhs.m_fSkillLoopMotionRate ),
 m_dwLoopDuration( rhs.m_dwLoopDuration ),
 m_WeaponStartPositionState(rhs.m_WeaponStartPositionState),
 m_dwSkillProtectTime( rhs.m_dwSkillProtectTime )
{
	m_dwGenerateStartTime = 0;
	m_dwNextGenerateTime = 0;
	m_dwCircleEnd = 0;

	m_dwCurMapEffect = -1;
	m_bSetHandMesh = false;
}


ioRandomGenerate2Skill::~ioRandomGenerate2Skill(void)
{
	m_vGenerateList.clear();
}

ioSkill* ioRandomGenerate2Skill::Clone()
{
	return new ioRandomGenerate2Skill( *this );
}

void ioRandomGenerate2Skill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "red_team_circle", "", szBuf, MAX_PATH );
	m_RedCircle = szBuf;
	rkLoader.LoadString_e( "blue_team_circle", "", szBuf, MAX_PATH );
	m_BlueCircle = szBuf;

	m_fCircleRadius = rkLoader.LoadFloat_e( "circle_radius", 0.0f );
	m_fMagicCircleOffSet = rkLoader.LoadFloat_e( "circle_offset", 0.0f );
	m_fMagicCircleMoveSpeed = rkLoader.LoadFloat_e( "circle_move_speed", 0.0f );
	m_fMagicCircleHeightGap = rkLoader.LoadFloat_e( "circle_height_gap", 0.0f );
	m_fMagicCircleRange = rkLoader.LoadFloat_e( "circle_range", 0.0f );

	m_dwCircleDuration = rkLoader.LoadInt_e( "circle_duration", 0 );
	m_fGenerateStartHeight = rkLoader.LoadFloat_e( "generate_start_height", FLOAT100 );
	m_dwMinGenerateTime = rkLoader.LoadInt_e( "min_generate_time", 0.0f );
	m_dwMaxGenerateTime = rkLoader.LoadInt_e( "max_generate_time", 0.0f );
	if (m_dwMinGenerateTime > m_dwMaxGenerateTime)
		LOG.PrintTimeAndLog( 0, "ioRandomGenerateSkill::LoadProperty - ini error! max_generate_time < min_generate_time " );
	
	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;

	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_SkillLoopMotion = szBuf;
	m_fSkillLoopMotionRate = rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );
	m_dwLoopDuration = rkLoader.LoadInt_e( "skill_loop_duration", 0 );
	m_WeaponStartPositionState = (WeaponStartPositionState)rkLoader.LoadInt_e( "weapon_startposition_state", WSPS_AREA );

	m_dwSkillProtectTime = rkLoader.LoadInt_e( "skill_protect_time", 0 );
}

bool ioRandomGenerate2Skill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	m_bSetHandMesh = false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_State = SS_NONE;

	SetCameraBuff( pChar );


	m_dwGenerateStartTime = 0;

	m_dwLoopStartTime = 0;
	
	m_dwFireProtectEndTime = 0;
	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	m_dwNextGenerateTime = 0;
	m_GeneratePos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_vCirclePos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_vGenerateList.clear();

	return true;
}

void ioRandomGenerate2Skill::SetWaitState( ioBaseChar *pChar )
{
	if( !pChar ) 
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if (!pGrp)
		return;

	m_State = SS_WAIT;
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

	//마법진 생성
	CreateMagicCircle( pChar );


}

void ioRandomGenerate2Skill::SetActionState( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return;

	GetMagicCirclePos( pChar );

	CreateEffect( pChar );
	
	AttackSkillFire( pChar );

	SetMoveDir();

	pChar->SetCurItemSkillGauge( GetOwnerItem()->GetName() , 0.0f );

	if( pChar->IsNeedSendNetwork() )
	{
		int iGenerateCnt = m_vGenerateList.size();
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_ACTION;
		kPacket << m_vCirclePos;
		kPacket << m_vWeaponMoveDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioRandomGenerate2Skill::SetMoveDir()
{
	//m_vWeaponMoveDir 
	D3DXVECTOR3 vTemp = m_GeneratePos;
	vTemp.y += m_fGenerateStartHeight;
	m_vWeaponMoveDir = m_vCirclePos - vTemp;
	D3DXVec3Normalize( &m_vWeaponMoveDir, &m_vWeaponMoveDir);
}


void ioRandomGenerate2Skill::SetDeadState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;
	//DestroyMapEffect
	pChar->EndMapEffect( m_dwCurMapEffect );
	pChar->DestroyMagicCircle();
	m_dwCircleEnd = 0;
	m_State = SS_END;
}

void ioRandomGenerate2Skill::CreateMagicCircle( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	if( !pChar->CheckMagicCircle() )
	{
		// 팀 체크
		if( pChar->GetTeam() == TEAM_RED )
			pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, "", m_fMagicCircleOffSet );
		else if( pChar->GetTeam() == TEAM_BLUE )
			pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, "", m_fMagicCircleOffSet );
		else
		{
			if( pChar->GetSingleConvertTeam() == TEAM_BLUE )			
				pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, "", m_fMagicCircleOffSet );
			else
				pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, "", m_fMagicCircleOffSet );
		}
	}
}

void ioRandomGenerate2Skill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	//DestroyMapEffect
	pChar->EndMapEffect( m_dwCurMapEffect );
	pChar->DestroyMagicCircle();

	m_vGenerateList.erase( m_vGenerateList.begin(), m_vGenerateList.end() );

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

	
}

void ioRandomGenerate2Skill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
		case SS_NONE:
			{
				DWORD dwPreDelay = GetPreDelayTime();
				if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
				{
					m_dwPreDelayStartTime = 0;

					SetWaitState( pChar );
				}
			}
			break;
		case SS_WAIT:
			{
				UpdateMagicCircle( pChar );

				CheckRotate( pChar );

				if(pChar->IsNeedProcess())
					CheckInputKey( pChar );

				if( m_dwLoopDuration > 0 && m_dwLoopStartTime + m_dwLoopDuration < dwCurTime )
				{
					SetActionState( pChar );
				}
			}
			break;
		case SS_ACTION:
			{
				DWORD dwCurTime = FRAMEGETTIME();
				if( m_dwCircleEnd > 0 && m_dwCircleEnd < dwCurTime )
				{
					SetDeadState( pChar );
					break;
				}

				if( pChar->IsNeedProcess() )
					FireWeapon( pChar, pStage );
			}
			break;
	}	
}

void ioRandomGenerate2Skill::FireWeapon( ioBaseChar *pChar, ioPlayStage *pStage )
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
			vPos = iter->vPos ;
			pChar->SkillFireExplicit( GetAttribute().m_vWeaponInfoList[iIndex],
				GetName(),
				m_dwWeaponBaseIndex++,
				vPos );

			iter = m_vGenerateList.erase( iter );

			bFire = true; 

			if( iter != m_vGenerateList.end() )
			{
				m_dwNextGenerateTime += iter->dwGapTime;
			}
		}
		else
			break;
	}

	if ( bFire )
	{
		if( pChar->IsNeedSendNetwork() )
		{
			int iGenerateCnt = m_vGenerateList.size();
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SS_FIRE;
			kPacket << vPos;
			kPacket << iIndex;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}


void ioRandomGenerate2Skill::AttackSkillFire( ioBaseChar *pChar )
{
	if ( !pChar )
		return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	m_State = SS_ACTION;
	
	ioHashString szSkillMotion = GetSkillMotionName();
	DWORD dwCurTime = FRAMEGETTIME();

	int iAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iAniID == -1 )	return;

	pChar->AttachEffect( m_SkillEffectName );

	float fTimeRate  = GetAttackTimeRate();
	DWORD dwPreDelay = GetAttribute().m_dwPreDelay;
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	m_dwFireProtectEndTime = dwCurTime + dwPreDelay;
	

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

	if( pChar->IsNeedProcess() )
	{
		m_dwGenerateStartTime = FRAMEGETTIME();

		float fFireTime = pChar->GetFirstFireTime( szSkillMotion );
		m_dwGenerateStartTime += dwPreDelay + ( DWORD )( fFireTime * GetAttribute().m_fAttackAniRate );
		
		m_dwNextGenerateTime = m_dwGenerateStartTime;
		
		GenerateWeapon( pChar );
	}
}

void ioRandomGenerate2Skill::GenerateWeapon( ioBaseChar *pChar )
{
	DWORD dwNextTime = m_dwGenerateStartTime;
	m_vGenerateList.clear();
	if (!pChar)
		return;

	IORandom random;
	random.SetRandomSeed( pChar->GetRandomSeed() );
	
	switch( m_WeaponStartPositionState )
	{
	case WSPS_AREA:
		m_GeneratePos = m_vCirclePos;
		break;
	case WSPS_PLYER:
		m_GeneratePos = pChar->GetWorldPosition();
		break;
	}

	if ( GetAttribute().m_vWeaponInfoList.empty() )
	{
		LOG.PrintTimeAndLog( 0, "ioRandomGenerate2Skill::GenerateWeapon - WeaponAttribute is not exist");
		return;
	}
	int iWeaponInfoCnt =  GetAttribute().m_vWeaponInfoList.size();
	int iIndex = 0;
	while ( dwNextTime <= m_dwCircleEnd )
	{
		//Generate Pos
		GenerateInfo eGenerateInfo;
		int iAngle = random.Random( 360 );
		int iRange = (int)m_fCircleRadius;
		if (iRange < 0)
			iRange = 0;
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

		eGenerateInfo.vPos = vPos;
		eGenerateInfo.dwGapTime = dwGapTime;
		eGenerateInfo.iAttributeIndex = iIndex;
		iIndex++;

		if ( iIndex == iWeaponInfoCnt )
			iIndex = 0;

		m_vGenerateList.push_back( eGenerateInfo );
	}
}

void ioRandomGenerate2Skill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	pWeapon->SetMoveDir(m_vWeaponMoveDir);
}

void ioRandomGenerate2Skill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iType;
	rkPacket >> iType;
	if ( iType == SS_ACTION )
	{
		rkPacket >> m_vCirclePos;
		rkPacket >> m_vWeaponMoveDir;
		CreateEffect( pChar );

		AttackSkillFire( pChar );
	}
	if ( iType == SS_FIRE )
	{
		OnGenerateList( pChar, rkPacket );
	}
	
}

void ioRandomGenerate2Skill::OnGenerateList( ioBaseChar *pChar, SP2Packet &rkPacket )
{
	if ( !pChar )
		return;

	D3DXVECTOR3 vPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	int iIndex = 0;

	rkPacket >> vPos;
	rkPacket >> iIndex;

	pChar->SkillFireExplicit( GetAttribute().m_vWeaponInfoList[iIndex],
		GetName(),
		m_dwWeaponBaseIndex++,
		vPos );
}

bool ioRandomGenerate2Skill::IsProtected( int iDefenseBreakType ) const
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
	if ( m_dwFireProtectEndTime > 0 && m_dwFireProtectEndTime > dwCurTime )
		return true;

	if( 0 < m_dwSkillProtectTime && dwCurTime < m_dwSkillStartTime + m_dwSkillProtectTime )
		return true;

	return false;
}

bool ioRandomGenerate2Skill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if ( m_State == SS_END)
		return true;
	return false;
}

void ioRandomGenerate2Skill::UpdateMagicCircle( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	if( pChar->CheckMagicCircle() )
	{
		//뱡향 체크
		pChar->CheckChangeDirectionForMagicCircle();
		//위치 갱신
		pChar->UpdateMagicCirclePosAll( m_fMagicCircleMoveSpeed, m_fMagicCircleRange, m_fMagicCircleHeightGap, false );
	}
}

void ioRandomGenerate2Skill::CheckRotate( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	//마법진 방향으로 캐릭터 회전
	D3DXVECTOR3 vTargetPos = pChar->GetMagicCirclePos();
	pChar->SetTargetRotToTargetPos( vTargetPos, true, false );
}

void ioRandomGenerate2Skill::CheckInputKey( ioBaseChar *pChar )
{
	if ( !pChar )
		return;	

	if( pChar->IsAttackKey() ||
		pChar->IsDefenseKey() ||
		pChar->IsJumpKey() )
	{
		SetActionState( pChar );
	}
}

void ioRandomGenerate2Skill::GetMagicCirclePos( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	if( pChar->CheckMagicCircle() )
	{
		pChar->CheckMagicCircleVisible( false );
		pChar->UpdateMagicCirclePos( m_fMagicCircleMoveSpeed, m_fMagicCircleRange, m_fMagicCircleHeightGap, true, true );
	}

	m_vCirclePos = pChar->GetMagicCirclePos( m_fMagicCircleOffSet, m_bCircleNoChangeY );
}

void ioRandomGenerate2Skill::CreateEffect( ioBaseChar *pChar )
{
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
}
