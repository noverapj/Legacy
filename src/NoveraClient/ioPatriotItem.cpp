#include "StdAfx.h"
#include "ioPatriotItem.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioTraceWeapon.h"
#include "ioZoneEffectWeapon.h"


ioPatriotItem::ioPatriotItem(void)
{
	ClearData();
	m_fCurBullet = 0.0f;
}

ioPatriotItem::ioPatriotItem( const ioPatriotItem &rhs ) :
ioWeaponItem( rhs ),
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
m_fGatherMoveSpeedRate( rhs.m_fGatherMoveSpeedRate ),
m_szGatherLoopMotion( rhs.m_szGatherLoopMotion ),
m_szGatherMoveLoopMotion( rhs.m_szGatherMoveLoopMotion ),
m_fGatherAniRate( rhs.m_fGatherAniRate ),
m_fGatheringScaleRate( rhs.m_fGatheringScaleRate ),
m_fGatheringScaleMaxRate( rhs.m_fGatheringScaleMaxRate ),
m_GatherAuraEffect( rhs.m_GatherAuraEffect ),
m_GatherOwnerAuraEffect( rhs.m_GatherOwnerAuraEffect ),
m_fHeightOffSet( rhs.m_fHeightOffSet ),
m_fTargetAngle( rhs.m_fTargetAngle ),
m_fTargetRange( rhs.m_fTargetRange),
m_fTargetRangeRate( rhs.m_fTargetRangeRate ),
m_fEnableTargetHeightGap( rhs.m_fEnableTargetHeightGap ),
m_AimEffect( rhs.m_AimEffect ),
m_ExtendAttack( rhs.m_ExtendAttack ),
m_szFailAni( rhs.m_szFailAni ),
m_fFailAniRate( rhs.m_fFailAniRate ),
m_fMaxBullet( rhs.m_fMaxBullet ),
m_fIncreseGaugeTic( rhs.m_fIncreseGaugeTic ),
m_fDecreaseTargetFindTic( rhs.m_fDecreaseTargetFindTic ),
m_fEnableExtendAttackGauge( rhs.m_fEnableExtendAttackGauge ),
m_fDecreaseExtendAttackGauge( rhs.m_fDecreaseExtendAttackGauge ),
m_dwFindTargetTic( rhs.m_dwFindTargetTic ),
m_szGatherMoveLoopFemaleMotion( rhs.m_szGatherMoveLoopFemaleMotion ),
m_szGatherLoopFemaleMotion( rhs.m_szGatherLoopFemaleMotion ),
m_szExtendFemaleAttack( rhs.m_szExtendFemaleAttack )
{
	ClearData();
	m_fCurBullet = 0.0f;
}

ioPatriotItem::~ioPatriotItem(void)
{
}

ioItem* ioPatriotItem::Clone()
{
	return new ioPatriotItem( *this );
}

ioWeaponItem::WeaponSubType ioPatriotItem::GetSubType() const
{
	return WST_PATRIOT_ITEM;
}

void ioPatriotItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	//Attack Ready
	int iMaxCombo = GetMaxCombo();
	m_AttackReadyAniList.clear();
	if ( iMaxCombo > 0 )
	{
		m_AttackReadyAniList.reserve( iMaxCombo );
		for( int i=0; i < iMaxCombo; ++i )
		{
			wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
			rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
			m_AttackReadyAniList.push_back( ioHashString(szBuf) );
		}
	}

	//////////////////////
	//D~
	//Speed
	m_fGatherMoveSpeedRate = rkLoader.LoadFloat_e( "gather_move_speed_rate", 0.0f );

	//Ani
	m_fGatherAniRate = rkLoader.LoadFloat_e( "gather_ani_rate", 1.0f );
	rkLoader.LoadString_e( "gather_loop_motion", "", szBuf, MAX_PATH );
	m_szGatherLoopMotion = szBuf;
	rkLoader.LoadString_e( "gather_move_loop_motion", "", szBuf, MAX_PATH );
	m_szGatherMoveLoopMotion = szBuf;

	//Effect
	rkLoader.LoadString_e( "gather_aura_effect", "", szBuf, MAX_PATH );
	m_GatherAuraEffect = szBuf;
	rkLoader.LoadString_e( "gather_owner_aura_effect", "", szBuf, MAX_PATH );
	m_GatherOwnerAuraEffect = szBuf;
	
	m_fGatheringScaleRate		= rkLoader.LoadFloat_e( "gather_scale_rate", FLOAT1 );
	m_fGatheringScaleMaxRate	= rkLoader.LoadFloat_e( "gather_scale_max_rate", 4.0f );
	m_fHeightOffSet		= rkLoader.LoadFloat_e( "gather_aura_height_offset", FLOAT100 );
	m_fTargetAngle		= rkLoader.LoadFloat_e( "gather_target_angle", 45.0f );
	m_fTargetRange		= rkLoader.LoadFloat_e( "gather_target_max_range", FLOAT1000 );
	m_fTargetRangeRate	= rkLoader.LoadFloat_e( "gather_target_range_rate", FLOAT1 );
	m_fEnableTargetHeightGap = rkLoader.LoadFloat_e( "enable_target_height_gap", 0.0f );
	m_dwFindTargetTic = rkLoader.LoadInt_e( "find_target_tic_time", 0 );
	rkLoader.LoadString_e( "aim_effect", "", szBuf, MAX_PATH );
	m_AimEffect = szBuf;

	//Attack
	LoadAttackAttribute( "extend_attack", m_ExtendAttack, rkLoader );

	rkLoader.LoadString_e( "fail_ani", "", szBuf, MAX_PATH );
	m_szFailAni = szBuf;
	m_fFailAniRate	= rkLoader.LoadFloat_e( "fail_ani_rate", 1.0f );
	//////////////////////

	//gauge
	m_fMaxBullet = rkLoader.LoadFloat_e( "max_gauge", 0 );
	m_fIncreseGaugeTic = rkLoader.LoadFloat_e( "increase_gauge_tic", 0.0f );
	m_fDecreaseTargetFindTic = rkLoader.LoadFloat_e( "decrease_target_find_tic", 0.0f );
	m_fEnableExtendAttackGauge = rkLoader.LoadFloat_e( "enable_extend_attack_gauge", 0.0f );
	m_fDecreaseExtendAttackGauge = rkLoader.LoadFloat_e( "decreas_extend_attack_gauge", 0.0f );
	

	//For Female
	rkLoader.LoadString_e( "gather_move_loop_hw_motion", "", szBuf, MAX_PATH );
	m_szGatherMoveLoopFemaleMotion = szBuf;
	rkLoader.LoadString_e( "gather_loop_hw_motion", "", szBuf, MAX_PATH );
	m_szGatherLoopFemaleMotion = szBuf;
	rkLoader.LoadString_e( "extend_hw_attack", "", szBuf, MAX_PATH );
	m_szExtendFemaleAttack = szBuf;
}

void ioPatriotItem::ClearData()
{
	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;
	m_dwFireStartTime = 0;
	m_bGatherMoveAni = false;
	m_dwGatherAuraEffectID = -1;
	m_fCurrScaleRate = 0.0f;
	m_fTargetCurrRange = 0.0f;
	m_dwFindTargetTime = 0;
	m_iCurCombo = 0;
	m_TargetList.clear();
}

void ioPatriotItem::ClearState( ioBaseChar *pOwner )
{
	m_ChargeState = CS_NONE;
	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;
	pOwner->SetAttackMoveEnable( false );

	AllRemoveTargetList( pOwner );
	RemoveAuraEffect( pOwner );
}

int ioPatriotItem::GetNeedBullet()
{
	return (int)m_fEnableExtendAttackGauge;
}

void ioPatriotItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner )
		return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	ClearCancelInfo();
	ClearData();

	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = CS_NONE;
		m_iCurCombo = iCurCombo;

		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );
	}
	else
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAniList[iCurCombo] );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_ChargeState = CS_CHARGING;
		m_dwMotionStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( true );
		pOwner->SetChargingState( true );

		if( m_bChargeAutoTarget )
		{
			DWORD dwTrackingTime = FRAMEGETTIME();
			dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
			pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
		else
			pOwner->StopAutoTargetTracking();
	}
}

void ioPatriotItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		ProcessCharging( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		ProcessNormalAttack( pOwner );
		break;
	case CS_FIND_TARGET:
		ProcessTargetFind( pOwner );
		UpdateTargetMaker( pOwner );
		UpdateGatheringEffect( pOwner );
		break;
	case CS_TARGET_ATTACK:
		ProcessExtendAttack( pOwner );
		break;
	case CS_FAIL:
		ProcessFailState( pOwner );
		break;
	}

	CheckGatherMoveState( pOwner );
}

void ioPatriotItem::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() && m_fCurBullet > GetNeedBullet() )
	{
		if( m_dwMotionStartTime + m_dwChargeTime <= FRAMEGETTIME() )
			ChangeToTargetFind( pOwner );
	}
	else
		ChangeToNormalAttack( pOwner );
}

void ioPatriotItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );
	ClearCancelInfo();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		if( !m_bChargeAutoTarget )
			pOwner->SetAutoTarget( ATT_NORMAL );

		pOwner->SetNormalAttackByAttribute( *pAttribute );

		SetCurAttackAniForInputJump( pAttribute->m_AttackAnimation, pAttribute->m_fAttackAniRate );
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPatriotItem::ProcessNormalAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( pOwner->IsNeedProcess() )
	{
		CheckCancelReserve( pOwner );

		if( ProcessCancel(pOwner) )
			return;
	}
	ioWeaponItem::CheckNormalAttack( pOwner );
}

void ioPatriotItem::CheckGatherMoveState( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsCanAttackMove() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	if( pOwner->IsSettedDirection() )
	{
		if( pOwner->IsChangeDirection()  )
		{
			pGrp->ClearAllLoopAni( FLOAT10 );

			m_bGatherMoveAni = true;

			float fCurRate = FLOAT1;
			if( m_fGatherAniRate > 0.0f )
				fCurRate = m_fGatherAniRate;

			if ( pOwner->IsMale() )
				pGrp->SetLoopAni( m_szGatherMoveLoopMotion, FLOAT100, FLOAT1, FLOAT1/fCurRate );
			else
				pGrp->SetLoopAni( m_szGatherMoveLoopFemaleMotion, FLOAT100, FLOAT1, FLOAT1/fCurRate );
		}
	}
	else
	{
		if( m_bGatherMoveAni )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );

			m_bGatherMoveAni = false;

			float fCurRate = FLOAT1;
			if( m_fGatherAniRate > 0.0f )
				fCurRate = m_fGatherAniRate;

			if ( pOwner->IsMale() )
				pGrp->SetLoopAni( m_szGatherLoopMotion, FLOAT100, FLOAT1, FLOAT1/fCurRate );
			else
				pGrp->SetLoopAni( m_szGatherLoopFemaleMotion, FLOAT100, FLOAT1, FLOAT1/fCurRate );
		}
	}
}

float ioPatriotItem::GetAttackSpeedRate() const
{
	if( m_ChargeState == CS_CHARGING || m_ChargeState == CS_FIND_TARGET )
		return m_fGatherMoveSpeedRate;

	return FLOAT1;
}

void ioPatriotItem::ChangeToTargetFind( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage* pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_ChargeState = CS_FIND_TARGET;
	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->SetChargingState( true );
	pOwner->SetAttackMoveEnable( true );
	pOwner->ChangeDirectionExplicitIfDirChanged( true );

	//내가 owner일 경우 생기는 이펙트
	if ( pOwner->IsNeedProcess() )
	{
		D3DXVECTOR3 vPos( pOwner->GetWorldPosition().x, pOwner->GetMidHeightByRate() + m_fHeightOffSet, pOwner->GetWorldPosition().z );
		ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_GatherOwnerAuraEffect, vPos, D3DXVECTOR3( m_fCurrScaleRate, m_fCurrScaleRate, m_fCurrScaleRate ) );
		if( pMapEffect )
			m_dwGatherAuraEffectID = pMapEffect->GetUniqueID();
	}
	//owner가 적일 경우 생기는 이펙트
	else
	{
		ioBaseChar *pOwnerChar = pStage->GetOwnerChar();
		if ( pOwnerChar->GetTeam() != pOwner->GetTeam() )
		{
			D3DXVECTOR3 vPos( pOwner->GetWorldPosition().x, pOwner->GetMidHeightByRate() + m_fHeightOffSet, pOwner->GetWorldPosition().z );
			ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_GatherAuraEffect, vPos, D3DXVECTOR3( m_fCurrScaleRate, m_fCurrScaleRate, m_fCurrScaleRate ) );
			if( pMapEffect )
				m_dwGatherAuraEffectID = pMapEffect->GetUniqueID();
		}
	}
	
	float fCurRate = FLOAT1;
	if( m_fGatherAniRate > 0.0f )
		fCurRate = m_fGatherAniRate;
	m_bGatherMoveAni = false;
	if ( !pOwner->IsSettedDirection() )
	{
		if ( pOwner->IsMale() )
			pGrp->SetLoopAni( m_szGatherLoopMotion, FLOAT100, FLOAT1, FLOAT1/fCurRate );
		else
			pGrp->SetLoopAni( m_szGatherLoopFemaleMotion, FLOAT100, FLOAT1, FLOAT1/fCurRate );
	}
	else
	{
		m_bGatherMoveAni = true;
		if ( pOwner->IsMale() )
			pGrp->SetLoopAni( m_szGatherMoveLoopMotion, FLOAT100, FLOAT1, FLOAT1/fCurRate );
		else
			pGrp->SetLoopAni( m_szGatherMoveLoopFemaleMotion, FLOAT100, FLOAT1, FLOAT1/fCurRate );
		
	}

	m_dwFindTargetTime = dwCurTime;
	m_fTargetCurrRange = 0.0f;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPatriotItem::ProcessTargetFind( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	ioPlayStage* pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	bool bNoKeyDown		= true;
	if( m_bChargeCharJump && pOwner->IsJumpKeyDown() && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
	{
		ClearState( pOwner );
		pOwner->SetJumpState();
		return;
	}
	else if ( !pOwner->IsAttackKeyDown() )
	{
		if ( m_TargetList.empty() )
		{
			ChangeToExtendFail( pOwner );
			return;
		}
		else
		{
			ChangeToExtendAttack( pOwner );
			return;
		}
	}
	else if ( m_fCurBullet <= 0.0f )
	{
		ChangeToExtendFail( pOwner );
		return;
	}

	ProcessTargeting( pOwner, pStage );
}

void ioPatriotItem::ProcessTargeting( ioBaseChar *pOwner, ioPlayStage* pStage )
{
	if ( !pOwner || !pStage || !pOwner->IsNeedProcess() )
		return;

	if ( m_dwFindTargetTime && m_dwFindTargetTime > FRAMEGETTIME() )
		return;

	ioPlayMode *pMode = pStage->GetPlayMode();
	if( pMode && pMode->GetModeType() == MT_FOOTBALL )
		return;
	
	ioHashStringVec vAddTargetList;
	ioHashStringVec vRemoveTargetList;
	m_dwFindTargetTime = FRAMEGETTIME() + m_dwFindTargetTic;
	//Targeting
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	m_fTargetCurrRange = m_fTargetCurrRange + m_fTargetRangeRate * fTimePerSec;
	m_fTargetCurrRange = min( m_fTargetCurrRange, m_fTargetRange );

	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	const BaseCharList &rkCharList = pStage->GetBaseCharList();

	BaseCharList::const_iterator iter;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		ioHashString szTargetName = pTarget->GetCharName();
		if( pTarget == pOwner )
			continue;

		if( pTarget->GetTeam() == pOwner->GetTeam() )
			continue;

		if( pTarget->IsSystemState() )
			continue;

		if( !pTarget->IsEnableTarget() )
			continue;

		float fHeightGap = fabs( pTarget->GetWorldPosition().y - pTarget->GetBottomHeight() );
		bool bLandChar = false;
		if ( m_fEnableTargetHeightGap >= fHeightGap )
			bLandChar = true;

		D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		float fLengthSq = D3DXVec3LengthSq( &vDiff );

		float fMaxRangeSq = m_fTargetCurrRange * m_fTargetCurrRange;
		bool bRangeAvailable = false;
		if( fLengthSq < fMaxRangeSq )
			bRangeAvailable = true;

		//Angle Test
		bool bAngleAvailable = false;
		D3DXVec3Normalize( &vDiff, &vDiff );
		float fValue = D3DXVec3Dot( &vDir, &vDiff );
		float fAngleGap = RADtoDEG( acosf(fValue) );
		if( fAngleGap <= m_fTargetAngle )
			bAngleAvailable = true;

		//가능한 타겟
		if( !bLandChar && bRangeAvailable && bAngleAvailable )
		{
			//이미 존재하는 타겟인지 체크
			TargetList::iterator TargetIter = m_TargetList.find( szTargetName );
			if( TargetIter != m_TargetList.end() )
				continue;

			//존재하지 않는 다면 새로운 타겟 추가
			vAddTargetList.push_back( szTargetName );
			AddTargetList( pOwner, pStage, pTarget );
		}
		//불가능한 타겟
		else
		{
			//현재 타겟에 존재 여부 찾기
			TargetList::iterator TargetIter = m_TargetList.find( szTargetName );
			if( TargetIter == m_TargetList.end() )
				continue;

			//현재 타겟에 있으면 타겟을 제거
			vRemoveTargetList.push_back( szTargetName );
			RemoveTarget( pOwner, pStage, szTargetName );
		}
	}

	if ( !vAddTargetList.empty() || !vRemoveTargetList.empty() )
	{
		int iAddSize = (int)vAddTargetList.size();
		int iRemoveSize = (int)vRemoveTargetList.size();
		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << CS_UPDATE_TARGET;
			kPacket << iAddSize;
			for ( int i=0 ; i < iAddSize ; ++i )
				kPacket << vAddTargetList[i];
			kPacket << iRemoveSize;
			for ( int i=0 ; i < iRemoveSize ; ++i )
				kPacket << vRemoveTargetList[i];
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

DWORD ioPatriotItem::AddTargetList( ioBaseChar *pOwner, ioPlayStage* pStage, ioBaseChar* pTarget )
{
	if ( !pOwner || !pStage || !pTarget )
		return (DWORD)-1;

	if ( m_AimEffect.IsEmpty() )
		return (DWORD)-1;

	ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_AimEffect, pTarget->GetMidPositionByRate(), D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
	if( !pMapEffect )
		return (DWORD)-1;
	
	m_TargetList.insert( TargetList::value_type(pTarget->GetCharName(), pMapEffect->GetUniqueID() ) );
	return pMapEffect->GetUniqueID();
}

void ioPatriotItem::RemoveTarget( ioBaseChar *pOwner, ioPlayStage* pStage, ioHashString szTargetName )
{
	if ( !pOwner || !pStage )
		return;

	TargetList::iterator iter = m_TargetList.find( szTargetName );
	if( ( m_TargetList.end() == iter ) )
		return;

	pStage->DestroyMapEffect( iter->second );
	m_TargetList.erase( szTargetName );
}

void ioPatriotItem::AllRemoveTargetList( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage* pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	TargetList::iterator iter;	
	for( iter = m_TargetList.begin(); iter != m_TargetList.end(); ++iter )
		pStage->DestroyMapEffect( iter->second );

	m_TargetList.clear();
}

void ioPatriotItem::UpdateTargetMaker( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage* pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	TargetList::iterator iter;	
	for( iter = m_TargetList.begin(); iter != m_TargetList.end(); ++iter )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( iter->second );
		ioBaseChar* pTarget = pStage->GetBaseChar( iter->first );
		if ( pTarget && pMapEffect )
			pMapEffect->SetWorldPosition( pTarget->GetMidPositionByRate() );
	}
}

void ioPatriotItem::UpdateGatheringEffect( ioBaseChar *pOwner )
{
	if ( m_dwGatherAuraEffectID == -1 )
		return;

	if ( !pOwner )
		return;

	ioPlayStage* pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	m_fCurrScaleRate = m_fCurrScaleRate + m_fGatheringScaleRate * fTimePerSec;
	m_fCurrScaleRate = min( m_fCurrScaleRate, m_fGatheringScaleMaxRate );

	ioMapEffect* pMapEffect = pStage->FindMapEffect( m_dwGatherAuraEffectID );
	if( pMapEffect )
	{
		D3DXVECTOR3 vPos( pOwner->GetWorldPosition().x, pOwner->GetMidHeightByRate() + m_fHeightOffSet, pOwner->GetWorldPosition().z );
		pMapEffect->SetWorldPosition( vPos );

		ioEffect* pEffect = pMapEffect->GetEffect();
		if( pEffect )
			pEffect->GetParentNode()->SetScale( m_fCurrScaleRate, m_fCurrScaleRate, m_fCurrScaleRate );
	}
}

void ioPatriotItem::ChangeToExtendAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	int iAniID = -1;
	if ( pOwner->IsMale() )
		iAniID = pGrp->GetAnimationIdx( m_ExtendAttack.m_AttackAnimation );
	else
		iAniID = pGrp->GetAnimationIdx( m_szExtendFemaleAttack );
	 
	if ( iAniID == -1 )
		return;

	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;
	ClearKeyReserve();

	m_ChargeState = CS_TARGET_ATTACK;

	pOwner->SetAttackMoveEnable( false );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pOwner->ClearReservedSliding();

	DWORD dwPreDelay = m_ExtendAttack.m_dwPreDelay;
	float fTimeRate = 1.0f;
	if ( m_ExtendAttack.m_fAttackAniRate > 0.0f )
		fTimeRate = m_ExtendAttack.m_fAttackAniRate;

	DWORD dwStartTime = FRAMEGETTIME() + dwPreDelay;
	m_dwMotionEndTime = dwStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + m_ExtendAttack.m_dwEndDelay;
	m_dwFireStartTime = dwStartTime + pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	// Col Skip
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	//Force
	pOwner->SetReservedSliding( m_ExtendAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwStartTime;

	RemoveAuraEffect( pOwner );
	DecreaseGague( m_fDecreaseExtendAttackGauge );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPatriotItem::ProcessExtendAttack( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwMotionEndTime && m_dwMotionEndTime < dwCurTime )
	{
		ProcessReserveKeyInput( pOwner );
		return;
	}
	
	if ( m_dwFireStartTime && m_dwFireStartTime < dwCurTime )
	{
		m_dwFireStartTime = 0;
		ExtendAttackFire( pOwner );
	}
	
	if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		CheckReserve( pOwner );
}

void ioPatriotItem::ExtendAttackFire( ioBaseChar *pOwner )
{
	if ( !pOwner || m_TargetList.empty() )
		return;

	int iAttackSize = m_ExtendAttack.m_vWeaponInfoList.size();
	if ( iAttackSize <= 0 )
		return;

	DWORD dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();
	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDir = pOwner->GetWorldOrientation() * D3DXVECTOR3(0.0f,0.0f,-FLOAT1);
	D3DXVec3Normalize( &vDir, &vDir );
	TargetList::iterator iter;
	for( iter = m_TargetList.begin(); iter != m_TargetList.end(); ++iter )
	{
		ioHashString szTargetName = iter->first;
		for ( int i = 0; i<iAttackSize ; ++i )
		{
			WeaponInfo sWeaponInfo = m_ExtendAttack.m_vWeaponInfoList[i];

			FireTime kFireTime;
			kFireTime.dwStart = FRAMEGETTIME();

			kFireTime.iAttributeIdx = sWeaponInfo.m_iWeaponIdx;
			kFireTime.iResistanceIdx = sWeaponInfo.m_iResistanceIdx;
			kFireTime.szWoundedAni = sWeaponInfo.m_WoundedAnimation;
			kFireTime.dwWoundedAniDuration = sWeaponInfo.m_dwWoundedDuration;
			kFireTime.bLoopWoundedAni = sWeaponInfo.m_bWoundedLoopAni;

			kFireTime.dwWeaponIdx = dwWeaponBaseIndex++;
			kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;
			
			ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
			if ( !szTargetName.IsEmpty() )
			{
				ioTraceWeapon *pTraceWeapon = ToTraceWeapon( pWeapon );
				if ( pTraceWeapon )
					pTraceWeapon->SetTraceTarget( szTargetName );
			}
		}
	}

	AllRemoveTargetList( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << CS_EXTEND_ATTACK_FIRE;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPatriotItem::ChangeToExtendFail( ioBaseChar* pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_szFailAni );
	if ( iAniID == -1 )
		return;

	m_ChargeState = CS_FAIL;

	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;
	ClearKeyReserve();

	pOwner->SetAttackMoveEnable( false );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pOwner->ClearReservedSliding();

	float fTimeRate = 1.0f;
	if ( m_fFailAniRate > 0.0f )
		fTimeRate = m_fFailAniRate;

	// Col Skip
	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, 0 );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwCurTime;

	AllRemoveTargetList( pOwner );
	RemoveAuraEffect( pOwner );
	DecreaseGague( m_fDecreaseExtendAttackGauge );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPatriotItem::ProcessFailState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwMotionEndTime && m_dwMotionEndTime < dwCurTime )
	{
		ProcessReserveKeyInput( pOwner );
		return;
	}
	
	if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		CheckReserve( pOwner );
}

void ioPatriotItem::RemoveAuraEffect( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if ( m_dwGatherAuraEffectID == -1 )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	pStage->DestroyMapEffect( m_dwGatherAuraEffectID );
	m_dwGatherAuraEffectID = -1;
}

int ioPatriotItem::GetMaxBullet()
{
	return m_fMaxBullet;
}

int ioPatriotItem::GetCurBullet()
{
	return m_fCurBullet;
}

void ioPatriotItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxBullet ) );

	m_fCurBullet = fCurBullet;
}

void ioPatriotItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem/*=false */ )
{
	ClearState( pOwner );
}

void ioPatriotItem::DecreaseGague( float fDecraseGauge )
{
	m_fCurBullet -= fDecraseGauge;
	m_fCurBullet = max( 0, m_fCurBullet );
}

void ioPatriotItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	//Increase
	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_DASH:
	case CS_RUN:
		{
			if( m_fCurBullet < m_fMaxBullet )
			{
				float fGauge = m_fIncreseGaugeTic * fTimePerSec;

				m_fCurBullet += fGauge;
				m_fCurBullet = min( m_fCurBullet, m_fMaxBullet );
			}
			return;
		}
		break;
	case CS_ATTACK:
		{
			if ( m_ChargeState == CS_FIND_TARGET )
			{
				float fGauge = m_fDecreaseTargetFindTic * fTimePerSec;
				m_fCurBullet -= fGauge;
				m_fCurBullet = max( m_fCurBullet, 0.0f );
				return;
			}
		}
	}
}

void ioPatriotItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( true, false );
		}
	}
}

void ioPatriotItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iState;
	rkPacket >> iState;
	if ( iState == CS_NORMAL_ATTACK )
	{
		rkPacket >> m_iCurCombo;

		if( !m_bChargeAutoTarget )
			pOwner->ApplyAutoTargetInfo( rkPacket );

		ChangeToNormalAttack( pOwner );
	}
	else if ( iState == CS_FIND_TARGET )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );
		ChangeToTargetFind( pOwner );
	}
	else if ( iState == CS_UPDATE_TARGET )
	{
		ioPlayStage *pStage = pOwner->GetCreator();
		if ( !pStage )
			return;

		int iAddSize,iDeleteSize;
		rkPacket >> iAddSize;
		for ( int i=0 ; i < iAddSize ; ++i )
		{
			ioHashString szTargetName;
			rkPacket >> szTargetName;
			ioBaseChar *pTarget = pStage->GetBaseChar( szTargetName );
			if ( pTarget )
				AddTargetList( pOwner, pStage, pTarget );
		}

		rkPacket >> iDeleteSize;
		for ( int i=0 ; i < iDeleteSize ; ++i )
		{
			ioHashString szTargetName;
			rkPacket >> szTargetName;
			RemoveTarget( pOwner, pStage, szTargetName );
		}
	}
	else if ( iState == CS_TARGET_ATTACK )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );

		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );

		ChangeToExtendAttack( pOwner );
	}
	else if ( iState == CS_FAIL )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );

		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );

		ChangeToExtendFail( pOwner );
	}
	else if ( iState == CS_EXTEND_ATTACK_FIRE )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );

		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );

		ExtendAttackFire( pOwner );
	}
}