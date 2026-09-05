

#include "stdafx.h"

#include "ioProject_K_Item.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "WeaponDefine.h"

ioProject_K_Item::ioProject_K_Item()
{
	m_ChargeState = WS_NONE;
	m_dwAttackStartTime = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bUsedExtraAttack = false;

	m_fCurGauge = 0.0f;

	m_dwFireEventTime = 0;
}

ioProject_K_Item::ioProject_K_Item( const ioProject_K_Item &rhs )
: ioWeaponItem( rhs ),
  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
  m_ExtraAttackAttribute( rhs.m_ExtraAttackAttribute ),

  m_fTargetAngle(rhs.m_fTargetAngle ),
  m_fTargetRange( rhs.m_fTargetRange ),
  m_fTargetUpHeight( rhs.m_fTargetUpHeight ),
  m_fTargetDownHeight( rhs.m_fTargetDownHeight ),

  m_fMaxGauge( rhs.m_fMaxGauge ),
  m_fNeedGauge( rhs.m_fNeedGauge ),
  m_fIncreaseGauge( rhs.m_fIncreaseGauge ),
  m_fDecreaseGauge( rhs.m_fDecreaseGauge ),

  m_nTargetMaxCount( rhs.m_nTargetMaxCount ),

  m_bSwitchDownTarget( rhs.m_bSwitchDownTarget )
{
	m_ChargeState = WS_NONE;
	m_dwAttackStartTime = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bUsedExtraAttack = false;

	m_fCurGauge = 0.0f;
}

ioProject_K_Item::~ioProject_K_Item()
{
}

void ioProject_K_Item::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	// ready ani
	int iMaxCombo = GetMaxCombo();
	m_AttackReadyAniList.clear();
	for( int i=0; i < iMaxCombo; ++i )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	//////////////////////////////////////////////////////////////////////////
	m_fTargetAngle			= rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fTargetRange			= rkLoader.LoadFloat_e( "target_range", 0.0f );
	m_fTargetUpHeight		= rkLoader.LoadFloat_e( "target_up_height", 0.0f );
	m_fTargetDownHeight		= rkLoader.LoadFloat_e( "target_down_height", 0.0f );

	m_ExtraAttackAttribute.Init();
	wsprintf_e( szBuf, "charge_extra_attack" );
	LoadAttackAttribute( szBuf, m_ExtraAttackAttribute, rkLoader );

	m_nTargetMaxCount = rkLoader.LoadInt_e( "max_special_weapon_cnt", 1 );

	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", FLOAT100 );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );

	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_gauge", FLOAT1 );
	m_fDecreaseGauge = rkLoader.LoadFloat_e( "decrease_gauge", 2.0f );

	//////////////////////////////////////////////////////////////////////////
	m_bSwitchDownTarget = rkLoader.LoadBool_e( "search_target_mode", 0 );
}

ioItem* ioProject_K_Item::Clone()
{
	return new ioProject_K_Item( *this );
}

ioWeaponItem::WeaponSubType ioProject_K_Item::GetSubType() const
{
	return WST_PROJECT_K;
}

void ioProject_K_Item::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	Clear();

	ClearCancelInfo();

	m_dwAttackStartTime = 0;
	m_bUsedExtraAttack = false;
	m_iChargeCombo = 0;

	DWORD dwCurTime = FRAMEGETTIME();
	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = WS_NONE;
		m_iNormalCombo = 0;

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

		m_ChargeState = WS_CHARGING;
		m_dwAttackStartTime = dwCurTime;
		m_iNormalCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );

		DWORD dwTrackingTime = dwCurTime;
		dwTrackingTime += pGrp->GetAnimationEventTime( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void ioProject_K_Item::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ChargeState )
	{
	case WS_CHARGING:
		OnCharging( pOwner );
		break;
	case WS_FIRE_MOTION:
		//fire함수가 호출되면 m_dwFireEventTime = 0;으로 셋팅이 됨
		if( m_dwFireEventTime > 0 && m_dwFireEventTime <= dwCurTime )
			Fire( pOwner );

		if( /*pOwner->IsNeedProcess() &&*/ m_dwMotionEndTime < dwCurTime )
			ChangeEnd( pOwner );

		break;
	case WS_ATTACK_FIRE:
		{
			if( pOwner->IsNeedProcess() )
			{
				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return;
			}

			ioWeaponItem::CheckNormalAttack( pOwner );
		}
		break;
	}
}

void ioProject_K_Item::OnCharging( ioBaseChar *pOwner )
{
	m_dwMotionEndTime = 0;
	m_bKeyReserved = false;
	m_bAttackKeyReserved = false;
	m_bDefenseKeyReserved = false;
	m_bJumpKeyReserved = false;

	if( !pOwner->IsNeedProcess() )	return;

	//게이지 확인
	if( pOwner->IsAttackKeyDown() && GetCurExtraGauge() >= m_fNeedGauge )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			FindTargetList( pOwner );
			ChangeToFireMotionState( pOwner );
		}
	}
	else
	{
		ChangeToAttackFire( pOwner );
	}
}



void ioProject_K_Item::OnFullGather( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTinme = FRAMEGETTIME();
}


void ioProject_K_Item::ChangeToFireMotionState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->SetAttackMoveEnable( false );

	m_ChargeState = WS_FIRE_MOTION;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_ExtraAttackAttribute.m_AttackAnimation );
	float fTimeRate  = m_ExtraAttackAttribute.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_ExtraAttackAttribute.m_dwPreDelay;
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	/*pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_AttackAttribute.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );*/

	m_dwFireStartTime = FRAMEGETTIME();
	m_dwFireEventTime = m_dwFireStartTime + pGrp->GetAnimationEventTime( iAniID, "fire_s" ) * m_ExtraAttackAttribute.m_fAttackAniRate;
	m_dwFireEndTime = m_dwFireStartTime + pGrp->GetAnimationFullTime( iAniID ) * m_ExtraAttackAttribute.m_fAttackAniRate;

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	DecreaseExtraNeedGauge(pOwner);
	//Send Packet
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioProject_K_Item::ChangeToAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );

	ClearCancelInfo();

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iNormalCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni = pAttribute->m_AttackAnimation;
		m_fCurAniRate = pAttribute->m_fAttackAniRate;

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;
	}

	pOwner->ApplyNormalAttackResult( m_iNormalCombo );
	m_ChargeState = WS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iNormalCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}



void ioProject_K_Item::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case WS_FIND_TARGET:
		{
			rkPacket >> m_iNormalCombo;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true );

			int iSize;
			rkPacket >> iSize;

			m_vTargetList.clear();
			for( int i=0; i < iSize; ++i )
			{
				ioHashString szTargetName;
				rkPacket >> szTargetName;

				m_vTargetList.push_back( szTargetName );
			}
		}
		break;
	case WS_ATTACK_FIRE:
		{
			rkPacket >> m_iNormalCombo;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true );
			ChangeToAttackFire( pOwner );
		}
		break;
	case WS_FIRE_MOTION:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true, false );

			ChangeToFireMotionState(pOwner);
		}
		break;
	case WE_END:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true, false );

			ChangeEnd(pOwner);
		}
		break;
	}
}

void ioProject_K_Item::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	rkPacket >> m_iChargeCombo;

	bool bUsedExtra;
	rkPacket >> bUsedExtra;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	pOwner->SetTargetRotToRotate( qtRot, true );

	pOwner->ApplyAutoTargetInfo( rkPacket );
}

void ioProject_K_Item::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	//
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

	ioIceMissileWeapon *pIceMissile = ToIceMissileWeapon( pWeapon );
	if( pIceMissile )
	{
		ioBaseChar *pChar = pWeapon->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
			D3DXVec3Normalize( &vDir, &vDir );

			if( !m_CurTargetName.IsEmpty() )
			{
				ioBaseChar *pTargetChar = pChar->GetBaseChar( m_CurTargetName );
				if( pTargetChar )
				{
					vDir = pTargetChar->GetWorldPosition() - pChar->GetWorldPosition();
					//vDir.y = 0;
					D3DXVec3Normalize( &vDir, &vDir );
				}
			}

			pIceMissile->SetMoveDir( vDir );
		}
	}
}

bool ioProject_K_Item::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == WS_CHARGING )
			return true;
	}

	return false;
}


bool ioProject_K_Item::ProcessCancel( ioBaseChar *pOwner )
{
	if( m_szCurAni.IsEmpty() )
		return false;

	if( !pOwner ) return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	int iAniID = pGrp->GetAnimationIdx( m_szCurAni );

	std::list<float> vTimeList;
	pGrp->GetAnimationEventTimeList_e( iAniID, "input_jump_s", vTimeList );

	if( vTimeList.empty() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	std::list< float >::iterator iter = vTimeList.begin();
	while( iter != vTimeList.end() )
	{
		DWORD dwTime = (DWORD)*iter;
		dwTime *= m_fCurAniRate;

		dwTime += m_dwInputCancelCheckTime;

		if( m_dwCancelCheckTime > 0 && COMPARE( dwTime, m_dwCancelCheckTime, dwCurTime ) )
		{
			m_dwCancelCheckTime = dwTime+1;
			
			if( m_bEnableJumpState && !pOwner->IsBuffLimitJump() )
			{
				pOwner->SetJumpState();
				return true;
			}

			return false;
		}

		if( dwTime > dwCurTime )
			return false;

		iter++;
	}

	return false;
}


void ioProject_K_Item::ChangeEnd( ioBaseChar *pOwner )
{
	m_ChargeState = WE_END;
	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetTargetRot();;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	pOwner->SetState( CS_DELAY );
}


bool ioProject_K_Item::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget() )
		return false;

	if( !m_bSwitchDownTarget )
	{
		if( pTarget->IsApplyDownState(false) )
			return false;
	}

	return true;
}

int ioProject_K_Item::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioProject_K_Item::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

int ioProject_K_Item::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

void ioProject_K_Item::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}


void ioProject_K_Item::DecreaseExtraNeedGauge( ioBaseChar *pOwner )
{
	m_fCurGauge -= m_fNeedGauge;
	m_fCurGauge = (float)max( 0.f, min( m_fCurGauge, m_fMaxGauge ) );
}


void ioProject_K_Item::Clear()
{
	m_vTargetList.clear();

	m_dwFireStartTime = 0;
	m_dwFireEventTime = 0;
	m_dwFireEndTime = 0;
}


void ioProject_K_Item::FindTargetList( ioBaseChar *pOwner )
{
	if( !pOwner )	return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )	return;

	m_vTargetList.clear();

	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_fTargetRange * m_fTargetRange;

	float fCurGap = -FLOAT1;
	ioHashString szTarget;

	//m_ChargeState = WS_FIND_TARGET;

	int nTargetCnt = 0;
	BaseCharList::const_iterator iter;
	const BaseCharList &rkCharList = pStage->GetBaseCharList();

	BaseCharDistanceSq vTargetInfo;
	BaseCharDistanceSqList	m_vTargetListTemp;

	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		/*if( nTargetCnt >= m_nTargetMaxCount )
			break;*/

		ioBaseChar *pTarget = *iter;
		if( !pTarget ) continue;

		if( pTarget->GetTeam() == pOwner->GetTeam() ) continue;
		if( pTarget->GetCharName() == GetOwnerName() ) continue;
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

		// 높이 체크
		float fHeightGap = vTargetPos.y - vCurPos.y;
		if( fHeightGap > 0.0f && fHeightGap > m_fTargetUpHeight )
			continue;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_fTargetDownHeight )
			continue;

		// 거리 체크
		if( fDiffRangeSq < fTargetRangeSq )
		{
			/*if( fCurGap == -FLOAT1 )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pTarget->GetCharName();
			}
			else if( fCurGap >= 0.0f && fDiffRangeSq < fCurGap )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pTarget->GetCharName();
			}
			else
				continue;*/

			vTargetInfo.m_Name = pTarget->GetCharName();
			vTargetInfo.m_fDistanceSq = fDiffRangeSq;
			m_vTargetListTemp.push_back( vTargetInfo );
			/*szTarget = pTarget->GetCharName();
			m_vTargetList.push_back(szTarget);
			nTargetCnt++;*/
		}
	}

	sort( m_vTargetListTemp.begin(), m_vTargetListTemp.end(), BaseCharDistanceSqSort() );
	if( m_vTargetListTemp.size() > 0 )
	{
		for( int i = 0; i < (int)m_vTargetListTemp.size() && i < m_nTargetMaxCount; i++ )
		{
			int nIndex = m_vTargetListTemp.size() - i - 1;
			ioHashString stTargetName = m_vTargetListTemp[nIndex].m_Name;
			m_vTargetList.push_back( stTargetName );
		}
	}

	/*if( pOwner->IsNeedSendNetwork() )
	{
		if( !m_vTargetList.empty() )
		{
			int iSize = m_vTargetList.size();

			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << m_ChargeState;
			kPacket << m_iNormalCombo;
			kPacket << pOwner->GetTargetRot();
			kPacket << iSize;
			for( int i=0; i < iSize; ++i )
				kPacket << m_vTargetList[i];
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}*/
}


void ioProject_K_Item::Fire( ioBaseChar *pOwner )
{
	DWORD dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();

	if( m_dwFireStartTime > 0 && m_dwFireStartTime < FRAMEGETTIME() )
	{
		// 타겟의 개수만큼 충돌체 생성
		int iTargetCnt = m_vTargetList.size();
		int iCurMaxCnt = iTargetCnt;
		if( iCurMaxCnt == 0 )
			iCurMaxCnt = 1;

		int iCurFireCnt = 0;
		for(int i=0; i<iTargetCnt; ++i)
		{
			m_CurTargetName = m_vTargetList[i];

			D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();

			pOwner->NormalAttackFireExplicit( m_ExtraAttackAttribute.m_vWeaponInfoList.front() );
			/*pOwner->SkillFireExplicit(m_FullGatherAttack.m_vWeaponInfoList.front(),
				GetName(),
				dwWeaponBaseIndex+i,
				vPos);*/

			iCurFireCnt++;
		}

		if( iCurMaxCnt > 0 && iCurFireCnt < iCurMaxCnt )
		{
			m_CurTargetName.Clear();

			D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
			pOwner->NormalAttackFireExplicit( m_ExtraAttackAttribute.m_vWeaponInfoList.front() );
		}

		m_dwFireEventTime = 0;
	}
}


bool ioProject_K_Item::IsEnableGauge()
{
	if( m_fCurGauge >= m_fNeedGauge )
		return true;

	return false;
}


void ioProject_K_Item::InitExtraGauge()
{
	m_fCurGauge = 0.0f;
}


void ioProject_K_Item::MaxExtraGauge()
{
	m_fCurGauge = m_fMaxGauge;
}


float ioProject_K_Item::GetMaxExtraGauge()
{
	return m_fMaxGauge;
}


float ioProject_K_Item::GetCurExtraGauge()
{
	return m_fCurGauge;
}


void ioProject_K_Item::SetCurExtraGauge( float fGauge )
{
	m_fCurGauge = fGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}


void ioProject_K_Item::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	// check owner state
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	if( pOwner->IsSystemState() )
		return;

	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_DASH:
	case CS_RUN:
		if( m_fCurGauge < m_fMaxGauge )
		{
			float fGauge = m_fIncreaseGauge * fTimePerSec;

			m_fCurGauge += fGauge;
			m_fCurGauge = min( m_fCurGauge, m_fMaxGauge );
		}
		return;
	}
}