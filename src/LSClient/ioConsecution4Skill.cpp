#include "StdAfx.h"

#include "../io3DEngine/ioINILoader.h"
#include "../io3DEngine/ioEntityGroup.h"
#include "../io3DEngine/ioStringConverter.h"
#include "../io3DEngine/HelpFunc.h"

#include "ioWeapon.h"
#include "ioBaseChar.h"
#include "ioGrowthUpInfo.h"
#include "ioPlayStage.h"

#include "SkillTypeID.h"
#include "ioSkillMaker.h"
#include "ioConsecution4Skill.h"

#include "NetworkWrappingFunc.h"
#include "PlayEntityHelpStruct.h"

ioConsecution4Skill::ioConsecution4Skill(void)
{
	ClearData();
}

ioConsecution4Skill::ioConsecution4Skill( const ioConsecution4Skill &rhs )
	: ioAttackSkill( rhs ),
	m_bDisableGrowth( rhs.m_bDisableGrowth ),
	m_fGuagePerTic( rhs.m_fGuagePerTic ),
	m_dwTicTime( rhs.m_dwTicTime ),
	m_szPreLoopMotion( rhs.m_szPreLoopMotion ),
	m_szRunLoopMotion( rhs.m_szRunLoopMotion ),
	m_szRunLeftHandAction( rhs.m_szRunLeftHandAction ),
	m_szRunRightHandAction( rhs.m_szRunRightHandAction ),
	m_fRunLeftHandActionRate( rhs.m_fRunLeftHandActionRate ),
	m_fRunRightHandActionRate( rhs.m_fRunRightHandActionRate ),
	m_szLeftHandAction( rhs.m_szLeftHandAction ),
	m_szRightHandAction( rhs.m_szRightHandAction ),
	m_fLeftHandActionRate( rhs.m_fLeftHandActionRate ),
	m_fRightHandActionRate( rhs.m_fRightHandActionRate ),
	m_LeftWeapon( rhs.m_LeftWeapon ),
	m_RightWeapon( rhs.m_RightWeapon ),
	m_TargetWoundType( rhs.m_TargetWoundType ),
	m_fLeftTargetMinAngle( rhs.m_fLeftTargetMinAngle ),
	m_fLeftTargetMaxAngle( rhs.m_fLeftTargetMaxAngle ),
	m_fRightTargetMinAngle( rhs.m_fRightTargetMinAngle ),
	m_fRightTargetMaxAngle( rhs.m_fRightTargetMaxAngle ),
	m_fTargetRange( rhs.m_fTargetRange ),
	m_fTargetMinRange( rhs.m_fTargetMinRange ),
	m_szAimMarker( rhs.m_szAimMarker ),
	m_dwSkillProtectedTime( rhs.m_dwSkillProtectedTime ),
	m_OwnerBuffList( rhs.m_OwnerBuffList ),
	m_dwFireMotionLoopCnt( rhs.m_dwFireMotionLoopCnt ),
	m_dwLifeTime( rhs. m_dwLifeTime )
{
	ClearData();
}

ioConsecution4Skill::~ioConsecution4Skill(void)
{
}

void ioConsecution4Skill::ClearData()
{
	m_State = SS_PRE;
	m_HandState = HS_LEFT;

	m_dwStartSkillProtectedTime = 0;

	m_bSetHandMesh = false;
	m_bSetMoveAni = false;
	m_dwCurFireMotionLoopCnt = 0;
	m_dwCheckTime = 0;
	m_fTicGaugeRateByGrowth = FLOAT1;
	m_dwTicTimeByGrowth = 0;

	m_dwFireMotionStartTime = 0;
	m_bFire = true;

	m_vAttackDir = ioMath::VEC3_ZERO;
	m_dwLeftAimEffectID = -1;
	m_dwRightAimEffectID = -1;
	m_szLeftTargetName.Clear();
	m_szRightTargetName.Clear();
}

void ioConsecution4Skill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );
	char szBuf[MAX_PATH];

	m_bDisableGrowth = rkLoader.LoadBool_e( "disable_growth", false );
	m_fGuagePerTic = rkLoader.LoadFloat_e( "gauge_per_tick", 0.0f );
	m_dwTicTime = (DWORD)rkLoader.LoadInt_e( "tick_time", 0 );

	m_dwLifeTime = (DWORD)rkLoader.LoadInt_e( "life_time", 0 );

	rkLoader.LoadString_e( "run_left_skill_motion", "", szBuf, MAX_PATH );
	m_szRunLeftHandAction = szBuf;
	rkLoader.LoadString_e( "run_right_skill_motion", "", szBuf, MAX_PATH );
	m_szRunRightHandAction = szBuf;
	m_fRunLeftHandActionRate = rkLoader.LoadFloat_e( "run_left_skill_motion_rate", FLOAT1 );
	m_fRunRightHandActionRate = rkLoader.LoadFloat_e( "run_right_skill_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "delay_animation", "", szBuf, MAX_PATH );
	m_szPreLoopMotion = szBuf;
	rkLoader.LoadString_e( "move_animation", "", szBuf, MAX_PATH );
	m_szRunLoopMotion = szBuf;

	rkLoader.LoadString_e( "left_skill_motion", "", szBuf, MAX_PATH );
	m_szLeftHandAction = szBuf;
	rkLoader.LoadString_e( "right_skill_motion", "", szBuf, MAX_PATH );
	m_szRightHandAction = szBuf;
	m_fLeftHandActionRate = rkLoader.LoadFloat_e( "left_skill_motion_rate", FLOAT1 );
	m_fRightHandActionRate = rkLoader.LoadFloat_e( "right_skill_motion_rate", FLOAT1 );
	
	// 타겟 검색 범위
	m_TargetWoundType	= (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );
	m_fLeftTargetMinAngle		= rkLoader.LoadFloat_e( "left_target_min_angle", 0.0f );
	m_fLeftTargetMaxAngle		= rkLoader.LoadFloat_e( "left_target_max_angle", 0.0f );
	m_fRightTargetMinAngle		= rkLoader.LoadFloat_e( "right_target_min_angle", 0.0f );
	m_fRightTargetMaxAngle		= rkLoader.LoadFloat_e( "right_target_max_angle", 0.0f );

	m_fTargetRange		= rkLoader.LoadFloat_e( "target_max_range", 0.0f );
	m_fTargetMinRange	= rkLoader.LoadFloat_e( "target_min_range", 0.0f );

	rkLoader.LoadString_e( "fire_Aim_marker", "", szBuf, MAX_PATH );
	m_szAimMarker = szBuf;	

	m_dwSkillProtectedTime = rkLoader.LoadFloat_e( "skill_protect_time", 0 );

	m_dwFireMotionLoopCnt = rkLoader.LoadInt_e( "left_right_motion_cnt", 3 );

	LoadAttackAttribute( rkLoader );
	LoadOwnerBuffList( rkLoader );
}

void ioConsecution4Skill::LoadAttackAttribute( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];

	m_LeftWeapon.Init();
	m_RightWeapon.Init();

	//left
	m_LeftWeapon.m_iWeaponIdx = rkLoader.LoadInt_e( "left_weapon", NO_WEAPON_ATTRIBUTE );
	m_LeftWeapon.m_iResistanceIdx = rkLoader.LoadInt_e( "left_weapon_resist", NO_WEAPON_ATTRIBUTE );

	rkLoader.LoadString_e( "left_weapon_wounded", "", szBuf, MAX_PATH );
	m_LeftWeapon.m_WoundedAnimation = szBuf;

	m_LeftWeapon.m_dwWoundedDuration = rkLoader.LoadInt_e( "left_weapon_wounded_time", 0 );
	m_LeftWeapon.m_bWoundedLoopAni = rkLoader.LoadBool_e( "left_weapon_wounded_loop_ani", false );

	//right
	m_RightWeapon.m_iWeaponIdx = rkLoader.LoadInt_e( "right_weapon", NO_WEAPON_ATTRIBUTE );
	m_RightWeapon.m_iResistanceIdx = rkLoader.LoadInt_e( "right_weapon_resist", NO_WEAPON_ATTRIBUTE );

	rkLoader.LoadString_e( "right_weapon_wounded", "", szBuf, MAX_PATH );
	m_RightWeapon.m_WoundedAnimation = szBuf;

	m_RightWeapon.m_dwWoundedDuration = rkLoader.LoadInt_e( "right_weapon_wounded_time", 0 );
	m_RightWeapon.m_bWoundedLoopAni = rkLoader.LoadBool_e( "right_weapon_wounded_loop_ani", false );
}

void ioConsecution4Skill::LoadOwnerBuffList( ioINILoader &rkLoader )
{
	int iCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	m_OwnerBuffList.clear();

	if( iCnt <= 0 ) return;

	m_OwnerBuffList.reserve( iCnt );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "owner_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		ioBuffInfo tBuffInfo;
		tBuffInfo.m_BuffName = szBuf;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				m_OwnerBuffList.push_back( tBuffInfo );
			}
		}
	}
}

ioSkill* ioConsecution4Skill::Clone()
{
	return new ioConsecution4Skill( *this );
}

bool ioConsecution4Skill::OnSkillStart( ioBaseChar *pChar )
{
	ClearData();

	m_bSetHandMesh = false;
	m_dwCheckTime = m_dwSkillStartTime = FRAMEGETTIME();
	SetOwnerBuff( pChar );
	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	ioPlayStage *pStage = pChar->GetCreator();

	if( !m_bDisableGrowth )
	{		
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

	if( !m_bReduceGauge )
	{
		float fCurGauge = pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	// Delay Animation이 없다
	if( !StartPreDelayAnimation( pChar ) )
	{		
		SetLoopState( pChar, pStage );
	}

	SetSkillNameChatBubble( pChar );

	return true;
}

void ioConsecution4Skill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	float fCurTicGauge = m_fGuagePerTic / m_fTicGaugeRateByGrowth;
	int iTickGap = (int)( dwCurTime - m_dwCheckTime );
	int iCurTicTime = m_dwTicTime + m_dwTicTimeByGrowth;
	
	switch( m_State )
	{
	case SS_PRE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < FRAMEGETTIME() )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );
				SetLoopState( pChar, pStage );
			}
		}
		break;
	case SS_LOOP:
		{
			if ( m_dwSkillStartTime > 0 && m_dwLifeTime > 0 && m_dwSkillStartTime + m_dwLifeTime < dwCurTime )
			{
				m_State = SS_END;
			}
			CheckNormalMoveAni( pChar );
			FindLeftTarget( pChar, pStage );
			FindRightTarget( pChar, pStage );
			CreateMarker( pChar, pStage );
			UpdateMarker( pStage );
			if ( pChar->IsAttackKey() && pChar->IsNeedProcess() )
			{
				SetActionState( pChar, pStage );
			}
		}
		break;
	case SS_ACTION:
		{
			//FindTarget( pChar, pStage );
			if ( m_HandState == HS_LEFT )
			{
				FindLeftTarget( pChar, pStage );
			}
			else
			{
				FindRightTarget( pChar, pStage );
			}
			CreateMarker( pChar, pStage );
			UpdateMarker( pStage );

			if ( m_dwMotionStartTime > 0 && m_dwMotionEndTime < dwCurTime )
			{
				//AttackSkillFire()가 이미 한번 수행되고 이지점으로 오기 떄문에 -1을 함
				if( m_dwCurFireMotionLoopCnt < m_dwFireMotionLoopCnt - 1 )
				{
					SetActionState( pChar, pStage );
					m_dwCurFireMotionLoopCnt++;
				}
				else
				{
					m_dwCurFireMotionLoopCnt = 0;
					SetLoopState( pChar, pStage );
				}				
				DestroyMarker( pChar );
			}

			if ( m_dwFireMotionStartTime > 0 && m_dwFireMotionStartTime < dwCurTime && m_bFire )
			{				
				AttackSkillFire( pChar );
				m_bFire = false;
			}
		}
		break;
	case SS_END:
		{
			DestroyMarker( pChar );
		}
		break;
	}

	if( iTickGap >= iCurTicTime )
	{
		m_dwCheckTime = dwCurTime - ( iTickGap - iCurTicTime );

		if( fCurTicGauge != 0.0f )
		{
			if( pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), fCurTicGauge ) <= 0.0f )
			{
				m_State = SS_END;
			}
		}
	}
}

void ioConsecution4Skill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_bReduceGauge = false;
	pChar->SetSkillMoveEnable( false, false );
	pChar->RestoreSkillProtection();

	RemoveOwnerBuff( pChar );
	DestroyMarker( pChar );
	
	ClearData();
}

void ioConsecution4Skill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	if( !pWeapon ) return;

	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );
}

void ioConsecution4Skill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case SST_ACTION:
		SetActionState( pChar, pStage );
		break;
	}
}

bool ioConsecution4Skill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_State )
	{
	case SS_PRE:
	case SS_LOOP:
	case SS_ACTION:
		return false;
	case SS_END:
		return true;
	}
	return true;
}

bool ioConsecution4Skill::IsProtected( int iDefenseBreakType ) const
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
	
	switch( m_State )
	{
	case SS_LOOP:
	case SS_ACTION:
		{
			if( 0 < m_dwStartSkillProtectedTime && 
				dwCurTime < m_dwStartSkillProtectedTime + m_dwSkillProtectedTime )
				return true;
		}
	}

	return false;
}

bool ioConsecution4Skill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioConsecution4Skill::SetLoopState( ioBaseChar* pChar, ioPlayStage* pStage )
{
	if( !pChar )
		return;

	if( !pStage )
		return;

	m_State = SS_LOOP;

	if( m_dwStartSkillProtectedTime == 0 )
		m_dwStartSkillProtectedTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iAniID = 0;
	iAniID = pGrp->GetAnimationIdx( m_szPreLoopMotion );
	if( iAniID == -1 )
		return;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetLoopAni( iAniID, FLOAT100 );
	m_dwMotionEndTime = 0;

	pChar->SetSkillMoveEnable( true, true );
	pChar->ChangeDirectionExplicitIfDirChanged( false );
	m_bSetMoveAni = false;
}

void ioConsecution4Skill::SetActionState( ioBaseChar* pChar, ioPlayStage* pStage )
{
	if( !pChar )
		return;

	if( !pStage ) 
		return;

	m_State = SS_ACTION;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = -1;
	float fTimeRate  = FLOAT1;
	if ( m_bSetMoveAni )
	{
		if( m_HandState == HS_LEFT )
		{		
			iAniID = pGrp->GetAnimationIdx( m_szRunLeftHandAction );
			fTimeRate = m_fRunLeftHandActionRate;		
		}
		else
		{
			iAniID = pGrp->GetAnimationIdx( m_szRunRightHandAction );
			fTimeRate = m_fRunRightHandActionRate;		
		}
	}
	else
	{
		if( m_HandState == HS_LEFT )
		{		
			iAniID = pGrp->GetAnimationIdx( m_szLeftHandAction );
			fTimeRate = m_fLeftHandActionRate;		
		}
		else
		{
			iAniID = pGrp->GetAnimationIdx( m_szRightHandAction );
			fTimeRate = m_fRightHandActionRate;		
		}
	}
	

	if( iAniID == -1 )	
		return;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime +=  pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwFireMotionStartTime = dwCurTime;
	m_dwFireMotionStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	pChar->SetSkillMoveEnable( true, false );
	m_bFire = true;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SST_ACTION;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioConsecution4Skill::CheckNormalMoveAni( ioBaseChar* pChar )
{
	if( !pChar ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = 0;

	if( pChar->IsSettedDirection() )
	{	
		if( !m_bSetMoveAni )
		{	
			iAniID = pGrp->GetAnimationIdx( m_szRunLoopMotion );
			if( iAniID == -1 )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			pGrp->SetLoopAni( iAniID, FLOAT100 );	
			m_bSetMoveAni = true;
		}
	}
	else
	{
		if( m_bSetMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx( m_szPreLoopMotion );
			if( iAniID == -1 )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			pGrp->SetLoopAni( iAniID, FLOAT100 );
			m_bSetMoveAni = false;
		}
	}
}

void ioConsecution4Skill::AttackSkillFire( ioBaseChar *pChar )
{
	if( !pChar ) return;	

	D3DXVECTOR3 vCurPos = pChar->GetMidPositionByRate();
	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	if( m_HandState == HS_LEFT )
	{
		pChar->SkillFireExplicit( m_LeftWeapon, GetName(), m_dwWeaponBaseIndex, vCurPos, m_vAttackDir );
		m_HandState = HS_RIGHT;
	}
	else
	{
		pChar->SkillFireExplicit( m_RightWeapon, GetName(), m_dwWeaponBaseIndex, vCurPos, m_vAttackDir );
		m_HandState = HS_LEFT;
	}
}

void ioConsecution4Skill::FindTarget( ioBaseChar* pChar, ioPlayStage* pStage )
{
	if( !pChar ) return;
	if( !pStage ) return;
	
	D3DXQUATERNION qtRot = pChar->GetTargetRot();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );
	
	m_vAttackDir = vDir;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();

	for ( auto iter = rkCharList.begin(); iter != rkCharList.end(); ++iter )
	{
		ioBaseChar* pTarget = *iter;
		
		if( !pTarget )	continue;

		if ( pTarget == pChar )
			continue;

		if ( pTarget->GetTeam() == pChar->GetTeam() )
			continue;

		if ( pTarget->IsSkipState() )
			continue;

		if( !IsEnableTargetState( pTarget, true ) )
			continue;

		D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate() - pChar->GetMidPositionByRate();
		float fLengthSq = D3DXVec3LengthSq( &vDiff );

		float fMaxRangeSq = m_fTargetRange * m_fTargetRange;
		float fMinRangeSq = m_fTargetMinRange * m_fTargetMinRange;
		
		if( !COMPARE( fLengthSq, fMinRangeSq, fMaxRangeSq ) )
			continue;

		D3DXVec3Normalize( &vDiff, &vDiff );
		
		float fValue = D3DXVec3Dot( &vDir, &vDiff );
		float fAngleGap = RADtoDEG( acosf(fValue) );

		D3DXVECTOR3 vAxis;
		D3DXVec3Cross( &vAxis, &vDir, &vDiff );
		D3DXVec3Normalize( &vAxis, &vAxis );
		
		if ( m_HandState == HS_LEFT )
		{	
			if ( vAxis.y > 0.0f )
				continue;

			if ( !COMPARE( fAngleGap, m_fLeftTargetMinAngle, m_fLeftTargetMaxAngle ) )
				continue;
		}
		else if ( m_HandState == HS_RIGHT )
		{
			if ( vAxis.y < 0.0f )
				continue;

			if ( !COMPARE( fAngleGap, m_fRightTargetMinAngle, m_fRightTargetMaxAngle ) )
				continue;
		}

		m_vAttackDir = vDiff;
	}
}


void ioConsecution4Skill::FindLeftTarget( ioBaseChar* pChar, ioPlayStage* pStage )
{
	if( !pChar ) return;
	if( !pStage ) return;

	m_szLeftTargetNameList.clear();
	m_szLeftTargetName.Clear();
	D3DXQUATERNION qtRot = pChar->GetTargetRot();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	m_vAttackDir = vDir;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();

	for ( auto iter = rkCharList.begin(); iter != rkCharList.end(); ++iter )
	{
		ioBaseChar* pTarget = *iter;

		if( !pTarget )	continue;

		if ( pTarget == pChar )
			continue;

		if ( pTarget->GetTeam() == pChar->GetTeam() )
			continue;

		if ( pTarget->IsSkipState() )
			continue;

		if( !IsEnableTargetState( pTarget, true ) )
			continue;

		D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate() - pChar->GetMidPositionByRate();
		float fLengthSq = D3DXVec3LengthSq( &vDiff );

		float fMaxRangeSq = m_fTargetRange * m_fTargetRange;
		float fMinRangeSq = m_fTargetMinRange * m_fTargetMinRange;

		if( !COMPARE( fLengthSq, fMinRangeSq, fMaxRangeSq ) )
			continue;

		D3DXVec3Normalize( &vDiff, &vDiff );

		float fValue = D3DXVec3Dot( &vDir, &vDiff );
		float fAngleGap = RADtoDEG( acosf(fValue) );

		D3DXVECTOR3 vAxis;
		D3DXVec3Cross( &vAxis, &vDir, &vDiff );
		D3DXVec3Normalize( &vAxis, &vAxis );

		if ( vAxis.y > 0.0f )
			continue;

		if ( !COMPARE( fAngleGap, m_fLeftTargetMinAngle, m_fLeftTargetMaxAngle ) )
			continue;

		m_vAttackDir = vDiff;
		float fDistanceSq = D3DXVec3LengthSq( &vDiff );
		
		BaseCharDistanceSq kTarget;
		kTarget.m_Name = pTarget->GetCharName();
		kTarget.m_fDistanceSq = fLengthSq;
		kTarget.m_vPos = pTarget->GetMidPositionByRate();

		m_szLeftTargetNameList.push_back( kTarget );
	}

	if ( !m_szLeftTargetNameList.empty() )
	{
		std::sort( m_szLeftTargetNameList.begin(), m_szLeftTargetNameList.end(), BaseCharDistanceSqSort2() );
		m_szLeftTargetName = m_szLeftTargetNameList.begin()->m_Name;
	}
	
	if ( m_szLeftTargetName.IsEmpty() )
	{
		DestroyMarker( pChar );
	}
}


void ioConsecution4Skill::FindRightTarget( ioBaseChar* pChar, ioPlayStage* pStage )
{
	if( !pChar ) return;
	if( !pStage ) return;

	m_szRightTargetNameList.clear();
	m_szRightTargetName.Clear();
	D3DXQUATERNION qtRot = pChar->GetTargetRot();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	m_vAttackDir = vDir;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();

	for ( auto iter = rkCharList.begin(); iter != rkCharList.end(); ++iter )
	{
		ioBaseChar* pTarget = *iter;

		if( !pTarget )	continue;

		if ( pTarget == pChar )
			continue;

		if ( pTarget->GetTeam() == pChar->GetTeam() )
			continue;

		if ( pTarget->IsSkipState() )
			continue;

		if( !IsEnableTargetState( pTarget, true ) )
			continue;

		D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate() - pChar->GetMidPositionByRate();
		float fLengthSq = D3DXVec3LengthSq( &vDiff );
		
		float fMaxRangeSq = m_fTargetRange * m_fTargetRange;
		float fMinRangeSq = m_fTargetMinRange * m_fTargetMinRange;

		if( !COMPARE( fLengthSq, fMinRangeSq, fMaxRangeSq ) )
			continue;

		D3DXVec3Normalize( &vDiff, &vDiff );

		float fValue = D3DXVec3Dot( &vDir, &vDiff );
		float fAngleGap = RADtoDEG( acosf(fValue) );

		D3DXVECTOR3 vAxis;
		D3DXVec3Cross( &vAxis, &vDir, &vDiff );
		D3DXVec3Normalize( &vAxis, &vAxis );

		if ( vAxis.y < 0.0f )
			continue;

		if ( !COMPARE( fAngleGap, m_fRightTargetMinAngle, m_fRightTargetMaxAngle ) )
			continue;

		m_vAttackDir = vDiff;
		

		BaseCharDistanceSq kTarget;
		kTarget.m_Name = pTarget->GetCharName();
		kTarget.m_fDistanceSq = fLengthSq;
		kTarget.m_vPos = pTarget->GetMidPositionByRate();

		m_szRightTargetNameList.push_back( kTarget );
	}

	if ( !m_szRightTargetNameList.empty() )
	{
		std::sort( m_szRightTargetNameList.begin(), m_szRightTargetNameList.end(), BaseCharDistanceSqSort2() );
		m_szRightTargetName = m_szRightTargetNameList.begin()->m_Name;
	}
	
	if ( m_szRightTargetName.IsEmpty() )
	{
		DestroyMarker( pChar );
	}
}

bool ioConsecution4Skill::IsEnableTargetState( ioBaseChar *pTarget, bool bCheckTargetWound )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;
		
	if( !pTarget->IsEnableTarget() )
		return false;

	if( bCheckTargetWound )
	{
		if( !pTarget->CheckTargetWoundType(m_TargetWoundType) )
			return false;
	}

	return true;
}

void ioConsecution4Skill::CreateMarker( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !pStage ) return;

	ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwLeftAimEffectID );
	if( !pMapEffect )
	{
		auto pTarget = pStage->GetBaseChar( m_szLeftTargetName );
		if ( pTarget )
		{
			D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
			D3DXVECTOR3 vPos = pTarget->GetMidPositionByRate();
			D3DXVECTOR3 vDir = pTarget->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pMapEffect = pStage->CreateMapEffect( m_szAimMarker, vPos, vScale );
			if( pMapEffect )
			{
				m_dwLeftAimEffectID = pMapEffect->GetUniqueID();

				D3DXQUATERNION qtCharRot;
				ioMath::CalcDirectionQuaternion( qtCharRot, -vDir );
				pMapEffect->SetWorldOrientation( qtCharRot );
			}
		}
	}
	
	pMapEffect = pStage->FindMapEffect( m_dwRightAimEffectID );
	if( !pMapEffect )
	{
		auto pTarget = pStage->GetBaseChar( m_szRightTargetName );
		if ( pTarget )
		{
			D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
			D3DXVECTOR3 vPos = pTarget->GetMidPositionByRate();
			D3DXVECTOR3 vDir = pTarget->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pMapEffect = pStage->CreateMapEffect( m_szAimMarker, vPos, vScale );
			if( pMapEffect )
			{
				m_dwRightAimEffectID = pMapEffect->GetUniqueID();

				D3DXQUATERNION qtCharRot;
				ioMath::CalcDirectionQuaternion( qtCharRot, -vDir );
				pMapEffect->SetWorldOrientation( qtCharRot );
			}
		}
	}
}

void ioConsecution4Skill::UpdateMarker( ioPlayStage *pStage )
{
	ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwLeftAimEffectID );
	if( pMapEffect )
	{
		auto pTarget = pStage->GetBaseChar( m_szLeftTargetName );
		if ( pTarget )
		{
			D3DXVECTOR3 vPos = pTarget->GetMidPositionByRate();
			pMapEffect->SetWorldPosition( vPos );
		}		
	}

	pMapEffect = pStage->FindMapEffect( m_dwRightAimEffectID );
	if( pMapEffect )
	{
		auto pTarget = pStage->GetBaseChar( m_szRightTargetName );
		if ( pTarget )
		{
			D3DXVECTOR3 vPos = pTarget->GetMidPositionByRate();
			pMapEffect->SetWorldPosition( vPos );
		}		
	}
}

void ioConsecution4Skill::DestroyMarker( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_dwLeftAimEffectID != -1 )
	{
		pChar->EndMapEffect( m_dwLeftAimEffectID, false );
		m_dwLeftAimEffectID = -1;
	}

	if( m_dwRightAimEffectID != -1 )
	{
		pChar->EndMapEffect( m_dwRightAimEffectID, false );
		m_dwRightAimEffectID = -1;
	}
}

void ioConsecution4Skill::SetOwnerBuff( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_OwnerBuffList.empty() ) return;

	int iCnt = m_OwnerBuffList.size();
	for( int i = 0; i < iCnt; ++i )
	{
		pChar->AddNewBuff( m_OwnerBuffList[i].m_BuffName, pChar->GetCharName(), "", this );
	}
}

void ioConsecution4Skill::RemoveOwnerBuff( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_OwnerBuffList.empty() ) return;
	int iCnt = m_OwnerBuffList.size();
	for( int i=0; i<iCnt; ++i)
	{
		pChar->RemoveBuff( m_OwnerBuffList[i].m_BuffName );
	}
}
