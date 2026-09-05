

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioGrowthUpInfo.h"

#include "ioConsecution3Skill.h"

ioConsecution3Skill::ioConsecution3Skill()
{
	ClearData();
}

ioConsecution3Skill::ioConsecution3Skill( const ioConsecution3Skill &rhs )
	: ioAttackSkill( rhs ),
	m_bDisableGrowth( rhs.m_bDisableGrowth ),
	m_fGuagePerTic( rhs.m_fGuagePerTic ),
	m_dwTicTime( rhs.m_dwTicTime ),
	m_szPreLoopMotion( rhs.m_szPreLoopMotion ),
	m_szRunLoopMotion( rhs.m_szRunLoopMotion ),
	m_SkillMotionFr( rhs.m_SkillMotionFr ),
	m_SkillMotionBk( rhs.m_SkillMotionBk ),
	m_SkillMotionLt( rhs.m_SkillMotionLt ),
	m_SkillMotionRt( rhs.m_SkillMotionRt ),
	m_fSkillMotionRate( rhs.m_fSkillMotionRate ),
	m_LeftWeapon( rhs.m_LeftWeapon ),
	m_RightWeapon( rhs.m_RightWeapon ),
	m_szLeftHandAction( rhs.m_szLeftHandAction ),
	m_szRightHandAction( rhs.m_szRightHandAction ),
	m_fLeftHandActionRate( rhs.m_fLeftHandActionRate ),
	m_fRightHandActionRate( rhs.m_fRightHandActionRate ),
	m_fMaxOverHeatGauge( rhs.m_fMaxOverHeatGauge ),
	m_fDisableOverHeatGauge( rhs.m_fDisableOverHeatGauge ),
	m_fIncreaseOverHeatGauge( rhs.m_fIncreaseOverHeatGauge ),
	m_fDecreaseOverHeatGauge( rhs.m_fDecreaseOverHeatGauge ),	
	m_dwFireMotionLoopCnt( rhs.m_dwFireMotionLoopCnt ),
	m_dwSkillProtectedTime( rhs.m_dwSkillProtectedTime ) 
{
	ClearData();
}

ioConsecution3Skill::~ioConsecution3Skill()
{
}

void ioConsecution3Skill::ClearData()
{
	m_State = SS_PRE;
	m_HandState = HS_LEFT;

	m_PreMotionState = MS_RUN;
	m_CurMotionState = MS_PRE;

	m_iWeaponFireCnt = 0;
	
	m_bRetrunLoop = true;

	m_bMotionSetted = false;
	m_bFirstMoveMotion = true;

	m_bSetHandMesh = false;
	m_dwFireMotionStartTime = 0;
	m_dwMotionEndTime = 0;

	m_dwCheckTime = 0;
	m_fTicGaugeRateByGrowth = FLOAT1;
	m_dwTicTimeByGrowth = 0;
		
	m_dwEnableReserveStartTime = 0;
	m_dwEnableReserveEndTime = 0;

	m_dwCurFireMotionLoopCnt = 0;

	m_fCurOverHeatGauge = m_fMaxOverHeatGauge;

	m_dwStartSkillProtectedTime = 0;
}

void ioConsecution3Skill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_bDisableGrowth = rkLoader.LoadBool_e( "disable_growth", false );
	m_fGuagePerTic = rkLoader.LoadFloat_e( "gauge_per_tick", 0.0f );
	m_dwTicTime = (DWORD)rkLoader.LoadInt_e( "tick_time", 0 );

	rkLoader.LoadString_e( "skill_motion_fr", "", szBuf, MAX_PATH );
	m_SkillMotionFr = szBuf;
	rkLoader.LoadString_e( "skill_motion_bk", "", szBuf, MAX_PATH );
	m_SkillMotionBk = szBuf;
	rkLoader.LoadString_e( "skill_motion_lt", "", szBuf, MAX_PATH );
	m_SkillMotionLt = szBuf;
	rkLoader.LoadString_e( "skill_motion_rt", "", szBuf, MAX_PATH );
	m_SkillMotionRt = szBuf;
	m_fSkillMotionRate = rkLoader.LoadFloat_e( "skill_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "delay_animation", "", szBuf, MAX_PATH );
	m_szPreLoopMotion = szBuf;
	rkLoader.LoadString_e( "move_animation", "", szBuf, MAX_PATH );
	m_szRunLoopMotion = szBuf;

	rkLoader.LoadString_e( "left_skill_motion", "", szBuf, MAX_PATH );
	m_szLeftHandAction = szBuf;
	rkLoader.LoadString_e( "right_skill_motion", "", szBuf, MAX_PATH );
	m_szRightHandAction = szBuf;

	m_dwFireMotionLoopCnt = rkLoader.LoadInt_e( "left_right_motion_cnt", 3 );

	m_fLeftHandActionRate = rkLoader.LoadFloat_e( "left_skill_motion_rate", FLOAT1 );
	m_fRightHandActionRate = rkLoader.LoadFloat_e( "right_skill_motion_rate", FLOAT1 );
	
	m_fMaxOverHeatGauge = rkLoader.LoadFloat_e( "max_overheat_gauge", 99.0f );	
	m_fDisableOverHeatGauge = rkLoader.LoadFloat_e( "disable_overheat_gauge", 80.0f );
	m_fIncreaseOverHeatGauge = rkLoader.LoadFloat_e( "increase_overheat_gauge", FLOAT10 );
	m_fDecreaseOverHeatGauge = rkLoader.LoadFloat_e( "decrease_overheat_gauge", FLOAT10 );
	
	m_dwSkillProtectedTime = rkLoader.LoadFloat_e( "skill_protect_time", 0 );

	LoadAttackAttribute( rkLoader );
}

void ioConsecution3Skill::LoadAttackAttribute( ioINILoader &rkLoader )
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

ioSkill* ioConsecution3Skill::Clone()
{
	return new ioConsecution3Skill( *this );
}

bool ioConsecution3Skill::OnSkillStart( ioBaseChar *pChar )
{	
	ClearData();

	m_bSetHandMesh = false;
	m_dwCheckTime = FRAMEGETTIME();

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

void ioConsecution3Skill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	float fCurTicGauge = m_fGuagePerTic / m_fTicGaugeRateByGrowth;
	int iTickGap = (int)( dwCurTime - m_dwCheckTime );
	int iCurTicTime = m_dwTicTime + m_dwTicTimeByGrowth;

	switch( m_State )
	{
	case SS_PRE :
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < FRAMEGETTIME() )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );
				SetLoopState( pChar, pStage );

				if( pChar->IsSettedDirection() )
				{
					m_CurMotionState = MS_RUN;
					m_PreMotionState = MS_PRE;
				}
				else
				{
					m_PreMotionState = MS_RUN;
					m_CurMotionState = MS_PRE;
				}
			}
		}
		break;
	case SS_LOOP:
		{			
			float fGauge = m_fDecreaseOverHeatGauge * fTimePerSec;

			m_fCurOverHeatGauge += fGauge;
			m_fCurOverHeatGauge = min( m_fCurOverHeatGauge, m_fMaxOverHeatGauge );

			if( pChar->IsSettedDirection() )
			{
				m_CurMotionState = MS_RUN;
				CheckMoveAndDelay( pChar, pStage );
			}
			else
			{
				m_CurMotionState = MS_PRE;
				CheckMoveAndDelay( pChar, pStage );
			}

			if( pChar->IsAttackKey() && pChar->IsNeedProcess() )
			{
				if( m_CurMotionState == MS_RUN )
				{
					m_bMotionSetted = true;
					m_bFirstMoveMotion = true;
				}
				else
				{
					m_bMotionSetted = false;
					m_bFirstMoveMotion = false;
				}

				m_dwCurFireMotionLoopCnt = 0;
				SetActionState( pChar, pStage );

				if( pChar->IsNeedProcess() && pChar->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
					kPacket << pChar->GetCharName();
					kPacket << GetName();
					kPacket << SSC_ACTION;
					kPacket << pChar->GetTargetRot();
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
		}
		break;
	case SS_ACTION:
		{
			CheckActionMove( pChar );
			float fGauge = m_fIncreaseOverHeatGauge * fTimePerSec;

			m_fCurOverHeatGauge -= fGauge;
			m_fCurOverHeatGauge = max( 0, m_fCurOverHeatGauge );

			if( 0 < m_dwMotionEndTime && m_dwMotionEndTime < dwCurTime )
			{
				//AttackSkillFire()가 이미 한번 수행되고 이지점으로 오기 떄문에 -1을 함
				if( m_dwCurFireMotionLoopCnt < m_dwFireMotionLoopCnt - 1 )
				{
					SetActionState( pChar, pStage );
					m_dwCurFireMotionLoopCnt++;
				}
				else
				{
					if( m_bRetrunLoop )
					{
						m_dwCurFireMotionLoopCnt = 0;
						m_bRetrunLoop = true;
						SetLoopState( pChar, pStage );
					}
					else
					{
						m_dwCurFireMotionLoopCnt = 0;
						m_bRetrunLoop = true;
						SetActionState( pChar, pStage );
					}
				}
			}
			else if(  0 < m_dwMotionEndTime && dwCurTime < m_dwMotionEndTime )
			{
				if( 0 < m_dwFireMotionStartTime && m_dwFireMotionStartTime < dwCurTime )
				{
					if( m_fDisableOverHeatGauge < m_fCurOverHeatGauge  )
					{						
						m_dwFireMotionStartTime = 0;
						AttackSkillFire( pChar );
					}
				}
			}

			if( m_dwEnableReserveStartTime < dwCurTime && dwCurTime < m_dwEnableReserveEndTime )
			{						
				if( pChar->IsAttackKey() && pChar->IsNeedProcess() )
				{
					if( m_bRetrunLoop )
					{						
						m_bRetrunLoop = false;
						if( pChar->IsNeedSendNetwork() )
						{
							SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
							kPacket << pChar->GetCharName();
							kPacket << GetName();
							kPacket << SSC_RESERVE_ACTION;
							P2PNetwork::SendToAllPlayingUser( kPacket );
						}
					}
				}
			}
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

void ioConsecution3Skill::OnSkillEnd( ioBaseChar *pChar )
{
	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_bSetHandMesh = false;
	m_bReduceGauge = false;
	pChar->SetSkillMoveEnable( false, false );
	pChar->RestoreSkillProtection();
}

void ioConsecution3Skill::SetLoopState( ioBaseChar* pChar, ioPlayStage* pStage )
{
	if( !pChar )
		return;

	if( !pStage )
		return;

	m_State = SS_LOOP;
	if( m_CurMotionState == MS_PRE )
		m_PreMotionState = MS_RUN;
	else
		m_PreMotionState = MS_PRE;
	
	if( m_dwStartSkillProtectedTime == 0 )
		m_dwStartSkillProtectedTime = FRAMEGETTIME();

	pChar->SetSkillMoveEnable( true, true );
	pChar->ChangeDirectionExplicitIfDirChanged( false );
}

void ioConsecution3Skill::SetActionState( ioBaseChar* pChar, ioPlayStage* pStage )
{
	if( !pChar )
		return;

	if( !pStage ) 
		return;
	
	m_State = SS_ACTION;
	
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = -1;
	float fTimeRate  = FLOAT1;

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

	if( iAniID == -1 )	
		return;
		
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwFullTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime +=  pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwEnableReserveStartTime = dwCurTime;
	m_dwEnableReserveEndTime = m_dwEnableReserveStartTime + (dwFullTime * m_dwFireMotionLoopCnt);

	m_dwFireMotionStartTime = dwCurTime;
	m_dwFireMotionStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	pChar->SetSkillMoveEnable( true, false );

	pChar->ChangeDirectionExplicitIfDirChanged( true );
	m_StartDirKey = pChar->GetCurDirKey();
}

void ioConsecution3Skill::AttackSkillFire( ioBaseChar *pChar )
{
	if( !pChar ) return;	

	D3DXVECTOR3 vCurPos = pChar->GetMidPositionByRate();
	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	if( m_HandState == HS_LEFT )
	{
		pChar->SkillFireExplicit( m_LeftWeapon, GetName(), m_dwWeaponBaseIndex, vCurPos );
		m_HandState = HS_RIGHT;
	}
	else
	{
		pChar->SkillFireExplicit( m_RightWeapon, GetName(), m_dwWeaponBaseIndex, vCurPos );
		m_HandState = HS_LEFT;
	}

	m_iWeaponFireCnt++;
}

void ioConsecution3Skill::CheckMoveAndDelay( ioBaseChar* pChar, ioPlayStage* pStage )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = -1;

	if( m_PreMotionState != m_CurMotionState && m_CurMotionState == MS_RUN )
	{
		iAniID = pGrp->GetAnimationIdx( m_szRunLoopMotion );
		if( iAniID == -1 )
			return;

		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->SetLoopAni( iAniID, FLOAT100 );
		m_PreMotionState = m_CurMotionState;
	}
	else if( m_PreMotionState != m_CurMotionState && m_CurMotionState == MS_PRE )
	{
		iAniID = pGrp->GetAnimationIdx( m_szPreLoopMotion );
		if( iAniID == -1 )	
			return;
		
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->SetLoopAni( iAniID, FLOAT100 );
		m_PreMotionState = m_CurMotionState;
	}
}

void ioConsecution3Skill::CheckActionMove( ioBaseChar *pChar )
{
	if( !pChar ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = 0;

	if( pChar->IsSettedDirection() )
	{
		if( pChar->IsChangeDirection() || m_bFirstMoveMotion )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );
			m_bMotionSetted = false;
			m_bFirstMoveMotion = false;

			ioUserKeyInput::DirKeyInput eXformDir;
			eXformDir = pChar->GetXformDir( m_StartDirKey );

			float fRate = m_fSkillMotionRate;

			switch( eXformDir )
			{
			case ioUserKeyInput::DKI_UP:
				pGrp->SetLoopAni( m_SkillMotionFr, FLOAT100, FLOAT1, FLOAT1/fRate );
				break;
			case ioUserKeyInput::DKI_RIGHTUP:
			case ioUserKeyInput::DKI_RIGHT:
			case ioUserKeyInput::DKI_RIGHTDOWN:
				pGrp->SetLoopAni( m_SkillMotionRt, FLOAT100, FLOAT1, FLOAT1/fRate );
				break;
			case ioUserKeyInput::DKI_DOWN:
				pGrp->SetLoopAni( m_SkillMotionBk, FLOAT100, FLOAT1, FLOAT1/fRate );
				break;
			case ioUserKeyInput::DKI_LEFTDOWN:
			case ioUserKeyInput::DKI_LEFT:
			case ioUserKeyInput::DKI_LEFTUP:
				pGrp->SetLoopAni( m_SkillMotionLt, FLOAT100, FLOAT1, FLOAT1/fRate );
				break;
			}
		}

		m_CurMotionState = MS_RUN;
	}
	else
	{
		if( !m_bMotionSetted )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );
			m_bMotionSetted = true;
		}

		m_CurMotionState = MS_PRE;
	}
}

void ioConsecution3Skill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	if( !pWeapon ) return;

	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioBaseChar *pOwner = pWeapon->GetOwner();
	if( pOwner )
	{
		D3DXVECTOR3 vAttDir =  pOwner->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
		pWeapon->SetMoveDir( vAttDir );
	}
}

void ioConsecution3Skill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;

	if( iState == SSC_ACTION )
	{
		D3DXQUATERNION qRot;
		rkPacket >> qRot;
		pChar->SetTargetRotToRotate( qRot, true, true );

		m_dwCurFireMotionLoopCnt = 0;
		m_bMotionSetted = true;
		m_bFirstMoveMotion = true;
		SetActionState( pChar, pStage );		
	}
	else if( iState == SSC_RESERVE_ACTION )
	{		
		m_bRetrunLoop = false;		
	}
}

bool ioConsecution3Skill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
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


bool ioConsecution3Skill::IsProtected( int iDefenseBreakType ) const
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

bool ioConsecution3Skill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}


