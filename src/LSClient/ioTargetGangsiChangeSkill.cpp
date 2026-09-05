

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"


#include "ioTargetGangsiChangeSkill.h"

#include "WeaponDefine.h"

ioTargetGangsiChangeSkill::ioTargetGangsiChangeSkill()
{
	InitSkillData();
}

ioTargetGangsiChangeSkill::ioTargetGangsiChangeSkill( const ioTargetGangsiChangeSkill &rhs )
: ioAttackSkill( rhs ),
  m_fTargetRange( rhs.m_fTargetRange ),
  m_fTargetAngle( rhs.m_fTargetAngle ),
  m_fTargetUpHeight( rhs.m_fTargetUpHeight ),
  m_fTargetDownHeight( rhs.m_fTargetDownHeight ),
  m_TargetWoundType( rhs.m_TargetWoundType ),
  m_iWeaponCount( rhs.m_iWeaponCount ),
  m_JumpAttackType( rhs.m_JumpAttackType )
{
	InitSkillData();
}

ioTargetGangsiChangeSkill::~ioTargetGangsiChangeSkill()
{
}

void ioTargetGangsiChangeSkill::InitSkillData()
{
	m_State = SS_NONE;
	m_bSetHandMesh = false;

	m_dwFireStartTime = 0;
	m_iCurWeaponCount = 0;

	m_TargetList.clear();
}

void ioTargetGangsiChangeSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	m_fTargetAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fTargetRange = rkLoader.LoadFloat_e( "target_range", 0.0f );
	m_fTargetUpHeight = rkLoader.LoadFloat_e( "target_up_height", 0.0f );
	m_fTargetDownHeight = rkLoader.LoadFloat_e( "target_down_height", 0.0f );
	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );

	m_iWeaponCount = rkLoader.LoadInt_e( "attack_count", 0 );
	m_JumpAttackType = (JumpAttackType)rkLoader.LoadInt_e( "jump_attack_type", JAT_NORMAL );
}

ioSkill* ioTargetGangsiChangeSkill::Clone()
{
	return new ioTargetGangsiChangeSkill( *this );
}

bool ioTargetGangsiChangeSkill::OnSkillStart( ioBaseChar *pChar )
{	
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

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

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

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

	return true;
}

void ioTargetGangsiChangeSkill::OnSkillEnd( ioBaseChar *pChar )
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
}

void ioTargetGangsiChangeSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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

	case SS_ACTION:
		{	
			if ( m_SkillUseType == SUT_NORMAL )
				CreateTargetSearchWeapon( pChar );
			else if ( m_SkillUseType == SUT_JUMP )
			{
				if ( m_JumpAttackType == JAT_NORMAL )
					CreateTargetSearchWeapon( pChar );
				else if ( m_JumpAttackType == JAT_ROUND )
					CreateWeapon( pChar );
			}
		}
		break;
	}
}

void ioTargetGangsiChangeSkill::AttackSkillFire( ioBaseChar *pChar )
{	
	if( !pChar ) return;

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge( pChar->GetCharName() ) );
		m_bReduceGauge = true;
	}

	m_State = SS_ACTION;
	ioEntityGroup *pGrp = pChar->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )	return;

	float fAniRate = GetAttribute().m_fAttackAniRate;
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
}

void ioTargetGangsiChangeSkill::FindTarget( ioBaseChar *pChar )
{
	if( !pChar )	return;
	ioPlayStage *pStage = pChar->GetCreator();
	if( !pStage )	return;

	D3DXVECTOR3 vCurPos = pChar->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_fTargetRange * m_fTargetRange;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) continue;

		if( pTarget->GetTeam() == pChar->GetTeam() ) continue;
		if( pTarget->GetCharName() == pChar->GetCharName() ) continue;
		if( pTarget->GetExperienceMode() != EMS_NONE ) continue;

		if( !IsEnableTargetState( pTarget ) )
			continue;

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( fCurAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = pChar->GetTargetRot();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );
			D3DXVECTOR3 vDiff2 = vDiff;
			vDiff2.y = 0.0f;
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				continue;
		}

		// 높이 체크
		float fHeightGap = vTargetPos.y - vCurPos.y;
		if( fHeightGap > 0.0f && fHeightGap > m_fTargetUpHeight )
			continue;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_fTargetDownHeight )
			continue;

		// 거리 체크
		if( fDiffRangeSq < fTargetRangeSq )
			m_TargetList.push_back( pTarget );
	}
}

void ioTargetGangsiChangeSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{	
	if( !pWeapon )
		return;

	if( !pStage )
		return;

	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioTraceWeaponType2 *pTrace = ToTraceWeaponType2( pWeapon );
	if( pTrace )
	{	
		if( !m_TargetList.empty() )
		{
			ioBaseChar* pTarget = m_TargetList[m_iCurWeaponCount];
			if( pTarget )
			{	
				ioHashString szCharName = pTarget->GetCharName();

				if( !szCharName.IsEmpty() )
				{
					pTrace->SetMoveDir( pTrace->GetOwner()->GetMoveDir() );
					pTrace->SetTraceTarget( szCharName );
				}
			}
		}	
	}
}

bool ioTargetGangsiChangeSkill::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;
			
	if( !pTarget->IsEnableTarget() )
		return false;

	if( !pTarget->CheckTargetWoundType(m_TargetWoundType) )
		return false;

	return true;
}

bool ioTargetGangsiChangeSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{	
	if( m_dwPreDelayStartTime > 0 )
		return false;
	
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_NONE:
		return false;
		break;

	case SS_ACTION:
		if( m_dwMotionEndTime == 0 )		
			return true;		

		if( 0 < m_dwMotionEndTime &&  m_dwMotionEndTime < dwCurTime )
		{
			if ( m_fSkillEndNextJumpAmt <= 0.0f )
				return true;
		}
		
		break;
	}

	return false;
}

bool ioTargetGangsiChangeSkill::IsProtected( int iDefenseBreakType ) const
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
	case SS_ACTION:
		if( 0 < m_dwFireMotionEndTime && dwCurTime <= m_dwFireMotionEndTime )
			return true;
	}

	return false;
}

bool ioTargetGangsiChangeSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioTargetGangsiChangeSkill::CreateTargetSearchWeapon( ioBaseChar *pChar )
{
	if ( !pChar && GetAttribute().m_vWeaponInfoList.empty() )
		return;

	if( 0 < m_dwFireStartTime && m_dwFireStartTime < FRAMEGETTIME() )
	{	
		FindTarget( pChar );

		int iCnt = min( (int)m_TargetList.size(), m_iWeaponCount );

		for( int i=0; i< iCnt; ++i )
		{						
			pChar->SkillFireExplicit( GetAttribute().m_vWeaponInfoList[0],
				GetName(),
				m_dwWeaponBaseIndex+i,
				pChar->GetMidPositionByRate() );
			m_iCurWeaponCount++;
		}

		if( iCnt == 0 )
		{
			pChar->SkillFireExplicit( GetAttribute().m_vWeaponInfoList[0],
				GetName(),
				m_dwWeaponBaseIndex,
				pChar->GetMidPositionByRate() );
		}

		m_dwFireStartTime = 0;
	}	
}

void ioTargetGangsiChangeSkill::CreateWeapon( ioBaseChar *pChar )
{
	if( 0 < m_dwFireStartTime && m_dwFireStartTime < FRAMEGETTIME() )
	{	
		int iSize = GetAttribute().m_vWeaponInfoList.size();
		for( int i=0; i< iSize; ++i )
		{						
			pChar->SkillFireExplicit( GetAttribute().m_vWeaponInfoList[i],
				GetName(),
				m_dwWeaponBaseIndex+i,
				pChar->GetMidPositionByRate() );
			m_iCurWeaponCount++;
		}
		m_dwFireStartTime = 0;
	}	
}

bool ioTargetGangsiChangeSkill::IsNoDropState() const
{
	if( m_SkillUseType == SUT_JUMP )
		return true;
	return false;
}

bool ioTargetGangsiChangeSkill::IsEndState() const
{
	if( m_State != SS_NONE && 0 < m_dwMotionEndTime &&  m_dwMotionEndTime < FRAMEGETTIME() )
	{
		if ( m_fSkillEndNextJumpAmt >= 0.0f )
			return true;
	}
	return false;
}

bool ioTargetGangsiChangeSkill::IsAttackEndState() const
{
	return IsEndState();
}
