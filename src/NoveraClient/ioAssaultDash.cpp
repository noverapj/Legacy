

#include "stdafx.h"

#include "ioAssaultDash.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"


#include "ItemDefine.h"
#include "WeaponDefine.h"

ioAssaultDash::ioAssaultDash()
{
	m_dwActionEndTime = 0;
}

ioAssaultDash::ioAssaultDash( const ioAssaultDash &rhs )
: ioExtendDash( rhs ),
 m_AttackAttribute( rhs.m_AttackAttribute ),
 m_fOffSet( rhs.m_fOffSet ),
 m_CircleEffectRed( rhs.m_CircleEffectRed ),
 m_CircleEffectBlue( rhs.m_CircleEffectBlue )
{
	m_dwActionEndTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
}

ioExtendDash* ioAssaultDash::Clone()
{
	return new ioAssaultDash( *this );
}

ioAssaultDash::~ioAssaultDash()
{
}

DashType ioAssaultDash::GetType() const
{
	return DT_ASSAULT;
}

void ioAssaultDash::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendDash::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "circle_effect_red", "", szBuf, MAX_PATH );
	m_CircleEffectRed = szBuf;
	rkLoader.LoadString_e( "circle_effect_blue", "", szBuf, MAX_PATH );
	m_CircleEffectBlue = szBuf;

	m_fOffSet = rkLoader.LoadFloat_e( "circle_offset", 0.0f );

	rkLoader.LoadString_e( "dash_action_animation", "", szBuf, MAX_PATH );
	m_AttackAttribute.m_AttackAnimation = szBuf;
	m_AttackAttribute.m_fAttackAniRate  = rkLoader.LoadFloat_e( "animation_time_rate", FLOAT1 );
	
	m_AttackAttribute.m_dwPreDelay	    = rkLoader.LoadInt_e( "animation_pre_delay", 0 );
	m_AttackAttribute.m_dwEndDelay	    = rkLoader.LoadInt_e( "animation_end_delay", 0 );
	

	m_AttackAttribute.m_vForceInfoList.clear();
	int iForceCnt = rkLoader.LoadInt_e( "dash_force_cnt", 0 );
	if( iForceCnt > 0 )
		m_AttackAttribute.m_vForceInfoList.reserve( iForceCnt );

	for( int i=0; i < iForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szBuf, "dash_force_move%d", i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "dash_force_friction%d", i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szBuf, FLOAT1 );

		m_AttackAttribute.m_vForceInfoList.push_back( kInfo );
	}

	int iAttackTypeCnt = rkLoader.LoadInt_e( "attack_type_count", 1 );
	m_AttackAttribute.m_vWeaponInfoList.clear();
	m_AttackAttribute.m_vWeaponInfoList.reserve( iAttackTypeCnt );

	char szKey[MAX_PATH];
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
}

bool ioAssaultDash::StartDash( ioBaseChar *pOwner )
{
	ioExtendDash::StartDash( pOwner );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_AttackAttribute.m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioAssaultDash::StartDash - %s Animation is Not Exist",
								m_AttackAttribute.m_AttackAnimation.c_str() );
		return false;
	}

	float fTimeRate = m_AttackAttribute.m_fAttackAniRate;

	if( pOwner->IsOwnerChar() && pOwner->GetCurItemSkillGauge() < m_fDashExtraGauge )
		return false;

	m_dwActionEndTime = 0;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->RefreshFireTimeList( iAniID,
							     m_AttackAttribute.m_vWeaponInfoList,
								 FTT_EXTEND_DASH_ATTACK,
								 fTimeRate,
								 m_AttackAttribute.m_dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	pOwner->SetNoWoundState( false );

	pOwner->InitExtendAttackTagInfo();
	pOwner->SetState( CS_EXTEND_DASH );
	pOwner->SetAutoTarget( ATT_DASH );
	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, (float)m_AttackAttribute.m_dwPreDelay, true );

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwDuration = m_AttackAttribute.m_dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + m_AttackAttribute.m_dwEndDelay;

	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	m_dwActionEndTime = dwCurTime;
	m_dwActionEndTime += dwDuration;

	pOwner->SetReservedSliding( m_AttackAttribute.m_vForceInfoList, iAniID, fTimeRate, m_AttackAttribute.m_dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, m_AttackAttribute.m_dwPreDelay );

	DWORD dwTrackingTime = FRAMEGETTIME() + m_AttackAttribute.m_dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	
	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, 0 );

	pOwner->ReduceCurSkillGaugeBySkill( NULL, m_fDashExtraGauge );

	g_TutorialMgr.ActionAssaultDash();

	return true;
}

void ioAssaultDash::SetExtraInfoToWeapon( ioBaseChar *pOwner, ioPlayStage *pStage, ioWeapon *pWeapon, const ioHashString &szName )
{
	// CheckPos : 이펙트 출력위치
	D3DXVECTOR3 vOffSetDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	vOffSetDir = m_fOffSet * vOffSetDir;

	D3DXVECTOR3 vCreatePos = pOwner->GetMidPositionByRate();

	if( m_fOffSet > 0.0f )
		vCreatePos += vOffSetDir;
	vCreatePos.y = pStage->GetMapHeightNoneEntity( vCreatePos.x, vCreatePos.z );

	// Effect
	ioHashString szEffectName;
	TeamType eTeam = pOwner->GetTeam();
	
	if( eTeam == TEAM_RED )
		szEffectName = m_CircleEffectRed;
	else if( eTeam == TEAM_BLUE )
		szEffectName = m_CircleEffectBlue;
	else
	{
		if( pOwner->GetSingleConvertTeam() == TEAM_BLUE )
			szEffectName = m_CircleEffectBlue;
		else
			szEffectName = m_CircleEffectRed;
	}

	if( !szEffectName.IsEmpty() )
		pStage->CreateMapEffect( szEffectName, vCreatePos, D3DXVECTOR3(1.0f, FLOAT1, FLOAT1) );

	// CheckPos : 실제 Weapon 생성 위치
	float fGap = pOwner->GetMidHeightByRate() - pOwner->GetBottomHeight();
	vCreatePos.y += fGap;

	pWeapon->SetCreateItem( szName );

	if( pWeapon->GetType() != ioWeapon::WT_CONTACT && pWeapon->GetType() != ioWeapon::WT_CONTACT2 )
	{
		pWeapon->SetStartPosition( vCreatePos, pStage );

		ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
		if( pZone )
		{
			ioBaseChar *pChar = pZone->GetOwner();
			if( pChar )
			{
				D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
				pZone->SetCollisionDir( vDir );
				pZone->SetZoneValue( false, false );
			}
		}
	}

}

void ioAssaultDash::ProcessDash( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwEndTime = m_dwActionEndTime + pOwner->GetActionStopTotalDelay();

	if( dwEndTime > dwCurTime )
	{
		if( m_dwReserveEnableTime > 0 && m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			// 예약체크
			CheckKeyReserve( pOwner );

			// Check Reload
			if( !m_bReloadCheck )
			{
				m_bReloadCheck = true;
				CheckReload( pOwner );
			}
		}
	}
	else
	{
		ProcessKeyReserve( pOwner );
		return;
	}
}
