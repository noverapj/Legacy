

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"


#include "ioDoubleIceMissileSkill.h"

#include "WeaponDefine.h"

ioDoubleIceMissileSkill::ioDoubleIceMissileSkill()
{
	InitSkillData();
}

ioDoubleIceMissileSkill::ioDoubleIceMissileSkill( const ioDoubleIceMissileSkill &rhs )
: ioAttackSkill( rhs ),
m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
m_dwOrgLoopDuration( rhs.m_dwOrgLoopDuration ),
m_fSkillLoopMotionRate( rhs.m_fSkillLoopMotionRate ),
m_SkillEndMotion( rhs.m_SkillEndMotion ),
m_fSkillEndMotionRate( rhs. m_fSkillEndMotionRate )

{
	InitSkillData();
}

ioDoubleIceMissileSkill::~ioDoubleIceMissileSkill()
{
}

void ioDoubleIceMissileSkill::InitSkillData()
{
	m_State = SS_NONE;
	m_bSetHandMesh = false;

	m_dwLoopDurationStart = 0;
	m_dwLoopDurationEnd = 0;

	m_dwFireStartTime = 0;
	m_dwEndStateEndTime = 0;

	m_dwLoopProtectStartTime = 0;

	m_vCreatePos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
}

void ioDoubleIceMissileSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_SkillLoopMotion = szBuf;
	m_fSkillLoopMotionRate = rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );

	m_dwOrgLoopDuration = (DWORD)rkLoader.LoadInt_e( "skill_loop_duration", 0 );
	m_dwLoopProtectTime	= (DWORD)rkLoader.LoadInt_e( "skill_loop_protect_time", 0 );

	rkLoader.LoadString_e( "skill_end_motion", "", szBuf, MAX_PATH );
	m_SkillEndMotion = szBuf;
	m_fSkillEndMotionRate = rkLoader.LoadFloat_e( "skill_motion_rate", FLOAT1 );
}

ioSkill* ioDoubleIceMissileSkill::Clone()
{
	return new ioDoubleIceMissileSkill( *this );
}

bool ioDoubleIceMissileSkill::OnSkillStart( ioBaseChar *pChar )
{
	InitSkillData();

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

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	pChar->SetSkillProtection();

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{			
		AttackSkillFire(pChar);
	}

	SetSkillNameChatBubble( pChar );

	return true;
}

void ioDoubleIceMissileSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	InitSkillData();

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	// 스킬 중간 동작에 사용된 메쉬들 제거	
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	g_WeaponMgr.CheckSkillDependencyWeaponDead( pChar->GetCharName(), GetName() );
}

void ioDoubleIceMissileSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_NONE :
		{				
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				AttackSkillFire(pChar);
			}
		}
		break;

	case SS_FIRE :
		{	
			CheckFireState( pChar, pStage );

			if( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
			{				
				SetLoopState( pChar, pStage );
			}
		}
		break;
		
	case SS_LOOP :
		{			
			if( COMPARE( dwCurTime, m_dwLoopDurationStart, m_dwLoopDurationEnd ) )
			{	
				if( !CheckControlWeapon( pChar ) )
				{
					SetEndState( pChar );
				}
			}
			else
			{
				SetEndState( pChar );
			}
		}
		break;

	case SS_END:
		{
			
		}
		break;
	}
}

void ioDoubleIceMissileSkill::AttackSkillFire( ioBaseChar *pChar )
{	
	if( !pChar ) return;

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge( pChar->GetCharName() ) );
		m_bReduceGauge = true;
	}

	m_State = SS_FIRE;
	ioEntityGroup *pGrp = pChar->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( m_AttackAttribute.m_AttackAnimation );
	if( iAniID == -1 )	return;

	float fAniRate = m_AttackAttribute.m_fAttackAniRate;
	if( fAniRate <= 0.0f )
		fAniRate = FLOAT1;

	DWORD dwPreDelay = GetAttribute().m_dwPreDelay * fAniRate;
	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fAniRate;
	m_dwMotionEndTime += dwPreDelay;

	m_dwFireMotionEndTime = dwCurTime;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fAniRate;
	
	if( m_dwFireStartTime == 0 )
	{
		m_dwFireStartTime = dwCurTime;
		m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fAniRate;
	}

	if( m_dwLoopDurationStart == 0 )
	{		
		m_dwLoopDurationStart = dwCurTime;
		m_dwLoopDurationEnd = m_dwLoopDurationStart + m_dwOrgLoopDuration;		
	}
}

void ioDoubleIceMissileSkill::CheckFireState( ioBaseChar *pChar, ioPlayStage *pStage )
{	
	DWORD dwCurTime = FRAMEGETTIME();
	
	if( m_dwFireStartTime <dwCurTime && m_dwFireStartTime != 0 )
	{	
		m_vCreatePos = pChar->GetMidPositionByRate();
	
		pChar->SkillFireExplicit( m_AttackAttribute.m_vWeaponInfoList.front(),
								  GetName(),
								  m_dwWeaponBaseIndex,
								  m_vCreatePos );

		m_dwFireStartTime = 0;
	}
}

void ioDoubleIceMissileSkill::SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	m_State = SS_LOOP;

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( m_SkillLoopMotion );
	if( iAniID == -1 )	return;

	float fAniRate = FLOAT1;
	if( m_fSkillLoopMotionRate > 0.0f )
		fAniRate = m_fSkillLoopMotionRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_dwLoopProtectStartTime = dwCurTime;
	m_dwLoopProtectStartTime += m_dwLoopProtectTime;
}

void ioDoubleIceMissileSkill::SetEndState( ioBaseChar *pChar )
{	
	m_State = SS_END;
	g_WeaponMgr.CheckSkillDependencyWeaponDead( pChar->GetCharName(), GetName() );

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	int iAniID = pGrp->GetAnimationIdx( m_SkillEndMotion );
	if( iAniID == -1 )	return;

	float fAniRate = m_fSkillEndMotionRate;
	if( fAniRate <= 0.0f )
		fAniRate = FLOAT1;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_dwEndStateEndTime = dwCurTime;
	m_dwEndStateEndTime += pGrp->GetAnimationFullTime( iAniID ) * fAniRate;	
}

bool ioDoubleIceMissileSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_NONE:
	case SS_FIRE:
	case SS_LOOP:
		return false;
	case SS_END:
		
		if( m_dwEndStateEndTime == 0 )
			return true;

		if( 0 < m_dwEndStateEndTime &&  m_dwEndStateEndTime < dwCurTime )
			return true;
	}

	return true;
}

bool ioDoubleIceMissileSkill::IsProtected( int iDefenseBreakType ) const
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
	case SS_FIRE:
		if( 0 < m_dwFireMotionEndTime && dwCurTime <= m_dwFireMotionEndTime )
			return true;

	case SS_LOOP:		
		if( 0 < m_dwLoopProtectStartTime  && dwCurTime <= m_dwLoopProtectStartTime )		
			return true;			
		
	case SS_END:
		return false;
	}

	return false;
}

const D3DXVECTOR3& ioDoubleIceMissileSkill::GetWeaponPos( ioBaseChar *pChar )
{
	if( !pChar )
		return m_vCreatePos;

	if( m_dwWeaponBaseIndex < 0 )
		return pChar->GetWorldPosition();

	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwWeaponBaseIndex );
	ioDoubleIceMissileWaepon *pIce = ToDoubleIceMissileWaepon( pWeapon );

	if( pIce )
	{
		return pIce->GetPosition();
	}

	return m_vCreatePos;
}

bool ioDoubleIceMissileSkill::CheckControlWeapon( ioBaseChar *pChar )
{
	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwWeaponBaseIndex );
	if( ToDoubleIceMissileWaepon( pWeapon ) )
		return true;

	return false;
}

bool ioDoubleIceMissileSkill::IsWeaponControl( ioBaseChar *pChar )
{
	switch( m_State )
	{
	case SS_LOOP:
		if( CheckControlWeapon( pChar ) )
			return true;
		break;
	}

	return false;
}

bool ioDoubleIceMissileSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}
