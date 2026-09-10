#include "StdAfx.h"
#include "ioAirWalkBuff.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"


ioAirWalkBuff::ioAirWalkBuff(void)
{
	ClearData();
	
}

ioAirWalkBuff::ioAirWalkBuff( const ioAirWalkBuff &rhs ):
ioBuff( rhs ),
m_vJumpAnimationList( rhs.m_vJumpAnimationList ),
m_bJumpAttack( rhs.m_bJumpAttack ),
m_fAniRate( rhs.m_fAniRate ),
m_fJumpPower( rhs.m_fJumpPower ),
m_fJumpForce( rhs.m_fJumpForce ),
m_fJumpSpeedRate( rhs.m_fJumpSpeedRate ),
m_fJumpFixedSpeed( rhs.m_fJumpFixedSpeed ),
m_fEnableHeight( rhs.m_fEnableHeight ),
m_dwJumpEnableTime( rhs.m_dwJumpEnableTime ),
m_iMaxJumpCnt( rhs.m_iMaxJumpCnt ),
m_bEnableRotate( rhs.m_bEnableRotate ),
m_fGravityRate( rhs.m_fGravityRate ),
m_bContinueJump( rhs.m_bContinueJump ),
m_bEnableMoveRotate( rhs.m_bEnableMoveRotate ),
m_dwMoveRotateSpeed( rhs.m_dwMoveRotateSpeed ),
m_bAniLoop( rhs.m_bAniLoop ),
m_fWalkEndJumpAmt( rhs.m_fWalkEndJumpAmt ),
m_fJumpEnableGauge( rhs.m_fJumpEnableGauge ),
m_fBuffAirWalkStartGauge( rhs.m_fBuffAirWalkStartGauge ),

//Attack
m_AirWalkAttack( rhs.m_AirWalkAttack ),
m_fAirWalkAttackEndJumpPower( rhs.m_fAirWalkAttackEndJumpPower ),
m_fTargetRange( rhs.m_fTargetRange ),
m_fTargetAngle( rhs.m_fTargetAngle ),
m_fAirWalkAttackUsingGauge( rhs.m_fAirWalkAttackUsingGauge )
{
	ClearData();
}

ioAirWalkBuff::~ioAirWalkBuff(void)
{
}

ioBuff* ioAirWalkBuff::Clone()
{
	return new ioAirWalkBuff(*this);
}

void ioAirWalkBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );
	
	char szBuf[MAX_PATH] = "";
	
	m_bEnableRotate = rkLoader.LoadBool_e( "enable_rotate", false );
	m_bContinueJump = rkLoader.LoadBool_e( "continue_fly", false );

	m_fAniRate = rkLoader.LoadFloat_e( "jump_ani_rate", FLOAT1 );
	m_fJumpPower = rkLoader.LoadFloat_e( "jump_power", 0.0f );
	m_fJumpForce = rkLoader.LoadFloat_e( "jump_force", 0.0f );
	m_fJumpSpeedRate = rkLoader.LoadFloat_e( "jump_speed_rate", FLOAT1 );
	m_fGravityRate = rkLoader.LoadFloat_e( "jump_gravity_rate", FLOAT1 );
	m_fJumpFixedSpeed = rkLoader.LoadFloat_e( "jump_fixed_speed", 0.0f );

	m_fEnableHeight = rkLoader.LoadFloat_e( "jump_enable_height", 0.0f );
	m_dwJumpEnableTime = rkLoader.LoadInt_e( "jump_enable_time", 0 );
	m_fJumpEnableGauge = rkLoader.LoadFloat_e( "jump_enable_gauge", 0.0f );

	m_iMaxJumpCnt = rkLoader.LoadInt_e( "jump_max_cnt", 2 );

	m_bJumpAttack = rkLoader.LoadBool_e( "jump_attack_enable", false );

	m_vJumpAnimationList.clear();
	m_vJumpAnimationList.reserve( 4 );
	rkLoader.LoadString_e( "jump_animation_front", "", szBuf, MAX_PATH );
	m_vJumpAnimationList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "jump_animation_right", "", szBuf, MAX_PATH );
	m_vJumpAnimationList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "jump_animation_back", "", szBuf, MAX_PATH );
	m_vJumpAnimationList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "jump_animation_left", "", szBuf, MAX_PATH );
	m_vJumpAnimationList.push_back( ioHashString(szBuf) );

	m_bEnableMoveRotate = rkLoader.LoadBool_e( "enable_move_rotate", false );
	m_dwMoveRotateSpeed = (DWORD)rkLoader.LoadInt_e( "move_rotate_speed", 0 );
	m_bAniLoop = rkLoader.LoadBool_e( "jump_ani_loop", false );
	m_fWalkEndJumpAmt = rkLoader.LoadFloat_e( "walk_end_jump_power_amt", 0.0f );
	m_fBuffAirWalkStartGauge = rkLoader.LoadFloat_e( "buff_walk_start_gauge", 0.0f );

	//Attack
	LoadAttackAttribute_e( "air_walk_attack", m_AirWalkAttack, rkLoader );
	m_fAirWalkAttackEndJumpPower = rkLoader.LoadFloat_e( "walk_attack_end_jump_power_amt", 0.0f );
	m_fTargetRange = rkLoader.LoadFloat_e( "target_range", 0.0f );
	m_fTargetAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fAirWalkAttackUsingGauge = rkLoader.LoadFloat_e( "walk_attack_using_gauge", 0.0f );
}

void ioAirWalkBuff::ClearData()
{
	m_AirWalkAttackState = AWAS_NONE;
	m_dwMotionEndTime = 0;
	m_dwFireStartTime = 0;
	m_dwWeaponBaseIndex = 0;
	m_szTargetName.Clear();
	m_bReduceGauge = false;
}

void ioAirWalkBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	CharBuffJumpInfo kInfo;
	kInfo.m_Name = GetName();
	kInfo.m_BuffJumpType = BJT_WALK;
	kInfo.m_bEnableRotate = m_bEnableRotate;
	kInfo.m_bContinue = m_bContinueJump;
	kInfo.m_fBuffJumpPower = m_fJumpPower;
	kInfo.m_fBuffJumpForce = m_fJumpForce;
	kInfo.m_fBuffJumpSpeedRate = m_fJumpSpeedRate;
	kInfo.m_fBuffJumpGravityRate = m_fGravityRate;

	kInfo.m_fBuffJumpHeight = m_fEnableHeight;
	kInfo.m_iBuffJumpMaxCnt = max( 2, m_iMaxJumpCnt);

	kInfo.m_dwBuffJumpEnableTime = m_dwJumpEnableTime;

	kInfo.m_fBuffJumpAniRate = m_fAniRate;
	kInfo.m_vBuffJumpAnimationList = m_vJumpAnimationList;

	kInfo.m_bEnableMoveRotate = m_bEnableMoveRotate;
	kInfo.m_dwMoveRotateSpeed = m_dwMoveRotateSpeed;
	kInfo.m_bAniLoop = m_bAniLoop;
	kInfo.m_fBuffJumpEndJumpPower = m_fWalkEndJumpAmt;
	kInfo.m_fBuffJumpEnableGauge = m_fJumpEnableGauge;
	kInfo.m_fBuffAirWalkStartGauge = m_fBuffAirWalkStartGauge;
	ClearData();

	pOwner->SetBuffFlyJumpValue( kInfo );
}

bool ioAirWalkBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	CharBuffJumpInfo kInfo;
	kInfo.m_Name = GetName();
	kInfo.m_BuffJumpType = BJT_WALK;
	kInfo.m_bEnableRotate = m_bEnableRotate;
	kInfo.m_bContinue = m_bContinueJump;
	kInfo.m_fBuffJumpPower = m_fJumpPower;
	kInfo.m_fBuffJumpForce = m_fJumpForce;
	kInfo.m_fBuffJumpSpeedRate = m_fJumpSpeedRate;
	kInfo.m_fBuffJumpGravityRate = m_fGravityRate;

	kInfo.m_fBuffJumpHeight = m_fEnableHeight;
	kInfo.m_iBuffJumpMaxCnt = max( 2, m_iMaxJumpCnt);

	kInfo.m_dwBuffJumpEnableTime = m_dwJumpEnableTime;

	kInfo.m_fBuffJumpAniRate = m_fAniRate;
	kInfo.m_vBuffJumpAnimationList = m_vJumpAnimationList;

	kInfo.m_bEnableMoveRotate = m_bEnableMoveRotate;
	kInfo.m_dwMoveRotateSpeed = m_dwMoveRotateSpeed;
	kInfo.m_bAniLoop = m_bAniLoop;
	kInfo.m_fBuffJumpEndJumpPower = m_fWalkEndJumpAmt;
	kInfo.m_fBuffJumpEnableGauge = m_fJumpEnableGauge;
	ClearData();

	pOwner->SetBuffFlyJumpValue( kInfo );

	return true;
}

void ioAirWalkBuff::ProcessBuff( float fTimePerSec )
{
	if ( !GetOwner() )
	{
		SetReserveEndBuff();
		return;
	}

	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( !CheckSwitchType( fTimePerSec ) )
		{
			SetReserveEndBuff();
			return;
		}
		CheckCallingProcess();
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		
		CheckCallingAreaWeapon();
		break;
	}

	ProcessPassive();

	if ( GetOwner() && GetOwner()->IsNeedProcess() && CheckSpecialState( GetOwner() ) )
		SetSpecialState( GetOwner() );
}

void ioAirWalkBuff::ProcessPassive()
{
	if( !m_pOwner )
		return;

	if( m_fGaugePerTick <= 0.0f || m_dwTickTime <= 0 )
		return;

	float fCurTicGauge = m_fGaugePerTick / m_fTicGaugeRateByGrowth;
	DWORD dwCurTime = FRAMEGETTIME();
	int iTickGap = (int)( dwCurTime - m_dwCheckTime );
	int iCurTicTime = m_dwTickTime + m_dwTicTimeByGrowth;

	// 활강중에만 스킬 게이지가 감소한다.
	if( iTickGap >= iCurTicTime )
	{
		m_dwCheckTime = dwCurTime - ( iTickGap - iCurTicTime );
		if( fCurTicGauge == 0.0f )
			return;

		ioItem *pItem = m_pOwner->GetEquipedItem( m_ItemName );
		if ( !pItem )
			return;

		if( (m_pOwner->GetState() == CS_JUMP && m_pOwner->GetJumpState() == JS_JUMPPING) &&
			m_pOwner->GetUsedBuffFlyJump() == BJUT_WALK &&
			m_pOwner->IsFloating() )
		{
			int iSlot = pItem->GetType() - 1;
			ioSkill *pSkill = m_pOwner->GetEquipedSkill(iSlot);
			ioPassiveSkill *pPassiveSkill = ToPassiveSkill(pSkill);
			if( !pPassiveSkill )
				return;

			if( GetSkillName().IsEmpty() ||  pPassiveSkill->GetName() != GetSkillName() )
				return;

			if( pPassiveSkill->UsePassiveGauge() )
			{
				m_bReduceGauge = true;
				if( pPassiveSkill->ReduceCurPassiveGauge( fCurTicGauge ) <= 0.0f )
				{
					m_pOwner->SetBuffAirWalkEnd();
					return;
				}
			}
		}
		else 
			m_bReduceGauge = false;
	}
}

bool ioAirWalkBuff::CheckSpecialState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	ioItem *pItem = m_pOwner->GetEquipedItem( m_ItemName );
	if ( !pItem )
		return false;

	int iSlot = pItem->GetType() - 1;
	ioSkill *pSkill = m_pOwner->GetEquipedSkill(iSlot);
	ioPassiveSkill *pPassiveSkill = ToPassiveSkill(pSkill);
	if( !pPassiveSkill )
		return false;

	if( GetSkillName().IsEmpty() ||  pPassiveSkill->GetName() != GetSkillName() )
		return false;

	if( pPassiveSkill->GetCurPassiveGauge() <= m_fAirWalkAttackUsingGauge )
		return false;

	if ( pOwner->GetState() == CS_JUMP && pOwner->GetUsedBuffFlyJump() == BJUT_WALK && pOwner->IsDefenseKeyDown() && pOwner->GetJumpState() == JS_JUMPPING )
		return true;

	return false;
}

void ioAirWalkBuff::SetSpecialState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ClearData();

	pOwner->SetState( CS_BUFF_AIR_WALK_ATTACK );

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetCurMoveSpeed( 0.0f );
	pOwner->SetJumpPower( 0.0f );
	pOwner->SetCurJumpPowerAmt( 0.0f);
	pOwner->SetGravityAmt( 0.0f );

	SetAirWalkAttackState( pOwner );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << AWAST_ATTACK;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetWorldOrientation();
		kPacket << m_szTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	if ( pOwner->IsNeedProcess() )
		ReduceAirWalkAttackGauge( pOwner );
	

}


void ioAirWalkBuff::ReduceAirWalkAttackGauge( ioBaseChar *pOwner )
{
	if ( m_fAirWalkAttackUsingGauge <= 0.0f )
		return;

	if ( !pOwner )
		return;

	ioItem *pItem = m_pOwner->GetEquipedItem( m_ItemName );
	if ( !pItem )
		return;

	int iSlot = pItem->GetType() - 1;
	ioSkill *pSkill = m_pOwner->GetEquipedSkill(iSlot);
	ioPassiveSkill *pPassiveSkill = ToPassiveSkill(pSkill);
	if( !pPassiveSkill )
		return;

	if( GetSkillName().IsEmpty() ||  pPassiveSkill->GetName() != GetSkillName() )
		return;

	if( pPassiveSkill->UsePassiveGauge() )
		pPassiveSkill->ReduceCurPassiveGauge( m_fAirWalkAttackUsingGauge );
}

void ioAirWalkBuff::ProcessSpecialState( ioBaseChar *pOwner )
{
	switch( m_AirWalkAttackState )
	{
	case AWAS_ATTACK:
		ProcessAirWalkAttackState( pOwner );
		break;
	}
}

void ioAirWalkBuff::SetAirWalkAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_AirWalkAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;


	DWORD dwCurTime = FRAMEGETTIME();
	m_AirWalkAttackState = AWAS_ATTACK;

	float fTimeRate  = m_AirWalkAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_AirWalkAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwMotionEndTime = dwCurTime + dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + m_AirWalkAttack.m_dwEndDelay;
	m_dwFireStartTime = dwCurTime + dwPreDelay + pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );


}

bool ioAirWalkBuff::CheckAirWalkAttackNoDropState()
{
	switch( m_AirWalkAttackState )
	{
	case AWAS_ATTACK:
		return true;
	}

	return false;
}

void ioAirWalkBuff::ProcessAirWalkAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess())
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwMotionEndTime && m_dwMotionEndTime < dwCurTime )
	{
		SetAirWalkAttackEndState( pOwner );
		return;
	}

	if ( m_dwFireStartTime && m_dwFireStartTime < dwCurTime )
	{
		m_dwFireStartTime = 0;
		FindTarget( pOwner );
		FireWeapon( pOwner );

		if ( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
			kPacket << pOwner->GetCharName();
			kPacket << GetName();
			kPacket << AWAST_FIRE;
			kPacket << m_szTargetName;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioAirWalkBuff::FindTarget( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	ioFootballMode *pFootBall = ToFootballMode( pStage->GetPlayMode() );
	if( pFootBall && pFootBall->IsDisableAttackMode() )
		return;

	if( pOwner->GetExperienceMode() != EMS_NONE )
		return;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_fTargetRange * m_fTargetRange;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	ioHashString szTarget;
	float fCurGap = -FLOAT1;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) continue;

		if( pTarget->GetTeam() == pOwner->GetTeam() ) continue;
		if( pTarget->GetCharName() == pOwner->GetCharName() ) continue;
		if( pTarget->GetExperienceMode() != EMS_NONE ) continue;

		if( !IsEnableTargetState( pTarget ) )
			continue;

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( fCurAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = pOwner->GetTargetRot();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );
			D3DXVECTOR3 vDiff2 = vDiff;
			vDiff2.y = 0.0f;
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				continue;
		}
		/*
		// 높이 체크
		float fHeightGap = vTargetPos.y - vCurPos.y;
		if( fHeightGap > 0.0f && fHeightGap > m_fCurTargetUpHeight )
			continue;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_fCurTargetDownHeight )
			continue;
		*/

		// 거리 체크
		if( fDiffRangeSq < fTargetRangeSq )
		{
			if( fCurGap == -FLOAT1 )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pTarget->GetCharName();
			}
			else if( fCurGap >= 0.0f && fDiffRangeSq < fCurGap )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pTarget->GetCharName();
			}
		}
	}

	if( !szTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( szTarget );
		if( pTarget )
			m_szTargetName = szTarget;
	}
}

bool ioAirWalkBuff::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget( false ) )
		return false;

	// 쓰러짐 관련 체크
	if( !pTarget->CheckTargetWoundType( m_CurTargetWoundType ) )
		return false;

	return true;
}

void ioAirWalkBuff::FireWeapon( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	m_dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();
	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	D3DXQUATERNION qtRot = pOwner->GetWorldOrientation();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	const WeaponInfoList& rkInfo = m_AirWalkAttack.m_vWeaponInfoList;

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTargetName );
	if ( pTarget )
	{
		vDir = pTarget->GetMidPositionByRate() - vPos;
		D3DXVec3Normalize( &vDir, &vDir );
	}

	for( int i=0; i<(int)rkInfo.size() ; ++i )
	{
		FireTime kFireTime;
		kFireTime.dwStart = FRAMEGETTIME() + rkInfo[i].m_dwCallTime;
		kFireTime.iAttributeIdx = rkInfo[i].m_iWeaponIdx;
		kFireTime.iResistanceIdx = rkInfo[i].m_iResistanceIdx;
		kFireTime.szWoundedAni  = rkInfo[i].m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration = rkInfo[i].m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni = rkInfo[i].m_bWoundedLoopAni;
		kFireTime.dwWeaponIdx = m_dwWeaponBaseIndex++;
		kFireTime.eFireTimeType = FTT_BUFF_ATTACK2;

		pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
	}
}

void ioAirWalkBuff::SetAirWalkAttackEndState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	pOwner->SetExtendAttackEndJump( m_fAirWalkAttackEndJumpPower, FLOAT1 );
	ClearData();
}

bool ioAirWalkBuff::OnReduceGauge() const
{
	return m_bReduceGauge;
}

void ioAirWalkBuff::EndBuff()
{
	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
		pOwner->RemoveBuffFlyJumpValue( GetName() );

	ioBuff::EndBuff();
}

void ioAirWalkBuff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	int iBuffState;
	rkPacket >> iBuffState;

	switch( iBuffState)
	{
	case AWAST_ATTACK:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetSpecialState( pOwner );
		}
		break;
	case AWAST_FIRE:
		{
			rkPacket >> m_szTargetName;
			FireWeapon( pOwner );
		}
		break;

	}
}

float ioAirWalkBuff::GetFixedSpeed()
{
	return m_fJumpFixedSpeed;
}

//---------------------------------------------------------------------------------------------------------------------

ioBuffAirWalkAttackState::ioBuffAirWalkAttackState()
{

}

ioBuffAirWalkAttackState::~ioBuffAirWalkAttackState()
{

}

void ioBuffAirWalkAttackState::CheckSpecialState( ioBaseChar* pOwner )
{
	if ( !pOwner  )
		return;
	
	ioBuff *pBuff = pOwner->GetBuff( BT_BUFF_AIR_WALK );
	ioAirWalkBuff *pAirWalkBuff = ToAirWalkBuff( pBuff );
	if ( pAirWalkBuff )
		pAirWalkBuff->ProcessSpecialState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

bool ioBuffAirWalkAttackState::IsNoDropState( ioBaseChar* pOwner )
{
	ioBuff *pBuff = pOwner->GetBuff( BT_BUFF_AIR_WALK );
	ioAirWalkBuff *pAirWalkBuff = ToAirWalkBuff( pBuff );
	if ( pAirWalkBuff )
		return pAirWalkBuff->CheckAirWalkAttackNoDropState();
	else
		return false;

}

void ioBuffAirWalkAttackState::GetGravity( ioBaseChar* pOwner, float &fCurGravity )
{
	fCurGravity = 0.f;
}

void ioBuffAirWalkAttackState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}


