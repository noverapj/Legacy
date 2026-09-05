

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"



#include "ioFireWorksSkill.h"

#include "WeaponDefine.h"

ioFireWorksSkill::ioFireWorksSkill()
{
	InitSkillData();
}

ioFireWorksSkill::ioFireWorksSkill( const ioFireWorksSkill &rhs )
	: ioAttackSkill( rhs ),
	m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
	m_dwOrgLoopDuration( rhs.m_dwOrgLoopDuration ),
	m_fSkillLoopMotionRate( rhs.m_fSkillLoopMotionRate ),
	m_SkillEndMotion( rhs.m_SkillEndMotion ),
	m_fSkillEndMotionRate( rhs. m_fSkillEndMotionRate ),
	m_fRotateAngleV( rhs.m_fRotateAngleV ),
	m_OwnerBuffList( rhs.m_OwnerBuffList )

{
	InitSkillData();
}

ioFireWorksSkill::~ioFireWorksSkill()
{
}

void ioFireWorksSkill::InitSkillData()
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

void ioFireWorksSkill::LoadProperty( ioINILoader &rkLoader )
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
	m_fRotateAngleV = rkLoader.LoadFloat_e( "rotate_v", 0.0f );

	LoadOwnerBuffList( rkLoader );
}

ioSkill* ioFireWorksSkill::Clone()
{
	return new ioFireWorksSkill( *this );
}

bool ioFireWorksSkill::OnSkillStart( ioBaseChar *pChar )
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

	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{			
		AttackSkillFire(pChar);
	}

	SetSkillNameChatBubble( pChar );

	return true;
}

void ioFireWorksSkill::OnSkillEnd( ioBaseChar *pChar )
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

void ioFireWorksSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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
				SetOwnerBuffList(pChar);
			}
		}
		break;

	case SS_FIRE :
		{	
			CheckFireState( pChar, pStage );

			if( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )		
				SetEndState( pChar );
		}
		break;

	case SS_END:
		{

		}
		break;
	}
}

void ioFireWorksSkill::AttackSkillFire( ioBaseChar *pChar )
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

void ioFireWorksSkill::CheckFireState( ioBaseChar *pChar, ioPlayStage *pStage )
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


void ioFireWorksSkill::SetEndState( ioBaseChar *pChar )
{	
	m_State = SS_END;

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

bool ioFireWorksSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_NONE:
	case SS_FIRE:
		return false;
	case SS_END:

		if( m_dwEndStateEndTime == 0 )
			return true;

		if( 0 < m_dwEndStateEndTime &&  m_dwEndStateEndTime < dwCurTime )
			return true;
	}

	return true;
}

bool ioFireWorksSkill::IsProtected( int iDefenseBreakType ) const
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

	case SS_END:
		return false;
	}

	return false;
}


bool ioFireWorksSkill::CheckControlWeapon( ioBaseChar *pChar )
{
	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwWeaponBaseIndex );
	if( ToDoubleIceMissileWaepon( pWeapon ) )
		return true;

	return false;
}


bool ioFireWorksSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}



void ioFireWorksSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioFireWorksStep1Weapon *pStep1Weapon = ToFireWorksStep1Waepon( pWeapon );
	if( pStep1Weapon )
	{
		ioBaseChar *pChar = pWeapon->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir =	pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f,  -FLOAT1 );
			D3DXVec3Normalize(&vDir, &vDir);
			//float fAngleH = ioMath::ArrangeHead( m_CallAttFireList[i].m_fChangeAngleH );
			float fAngleV = ioMath::ArrangeHead( m_fRotateAngleV );

			D3DXQUATERNION qtRot;
			D3DXVECTOR3 vAxis;
			D3DXVec3Cross( &vAxis, &vDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

			// vertical
			D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngleV) );
			vDir = qtRot * vDir;

			// horizon
			//D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngleH) );
			//vDir = qtRot * vDir;
			pStep1Weapon->SetMoveDir( vDir );
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioFireWorksSkill::LoadOwnerBuffList( ioINILoader &rkLoader )
{
	int iCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	m_OwnerBuffList.clear();

	if( iCnt <= 0 ) 
		return;

	m_OwnerBuffList.reserve( iCnt );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "owner_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		m_OwnerBuffList.push_back( szBuf );
	}
}

void ioFireWorksSkill::SetOwnerBuffList( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	if( m_OwnerBuffList.empty() ) 
		return;

	int nBuffCnt = m_OwnerBuffList.size();
	for( int i=0; i < nBuffCnt; ++i )
	{
		ioHashString szBuffName = m_OwnerBuffList[i];

		if( !szBuffName.IsEmpty() )
			pOwner->AddNewBuff( szBuffName, pOwner->GetCharName(), "", NULL );	
	}
}
