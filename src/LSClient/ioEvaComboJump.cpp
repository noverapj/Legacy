#include "stdafx.h"
#include "ItemDefine.h"
#include "ioEvaComboJump.h"

ioEvaComboJump::ioEvaComboJump()
{
	m_EvaComboJumpState			= ECJS_NONE;
	m_iCurComboCnt				= 0;
	m_dwEnableComboTime_S		= 0;
	m_dwEnableComboTime_E		= 0;
	m_dwChargingStartTime		= 0;
	m_bReserveJumpAttack		= false;
	m_bFindJumpTarget			= false;
	m_bCheckFirstTarget			= false;
	m_stFireUpAni.Clear();
	m_stFireCenterAni.Clear();
	m_stFireDownAni.Clear();
}

ioEvaComboJump::ioEvaComboJump( const ioEvaComboJump &rhs )
	: ioExtendJump( rhs ),
	m_dwChargingTime( rhs.m_dwChargingTime ),
	m_AttackAttributeList( rhs.m_AttackAttributeList ),
	m_iMaxComboCnt( rhs.m_iMaxComboCnt),
	m_JumpAimUp( rhs.m_JumpAimUp ),		// Charge Combo
	m_JumpAimCenter( rhs.m_JumpAimCenter ),
	m_JumpAimDown( rhs.m_JumpAimDown ),
	m_fMarkerRange( rhs.m_fMarkerRange ),
	m_vChargeComboAttList( rhs.m_vChargeComboAttList ),
	m_JumpMotionInfoList( rhs.m_JumpMotionInfoList ),
	m_dwJumpChargeTime( rhs.m_dwJumpChargeTime )	// 특별 D~ 누른 상태에서 차징 시간
{
	m_EvaComboJumpState			= ECJS_NONE;
	m_iCurComboCnt				= 0;
	m_dwEnableComboTime_S		= 0;
	m_dwEnableComboTime_E		= 0;
	m_dwChargingStartTime		= 0;
	m_bReserveJumpAttack		= false;
	m_bFindJumpTarget			= false;		// D~
	m_bCheckFirstTarget			= false;
	m_stFireUpAni.Clear();
	m_stFireCenterAni.Clear();
	m_stFireDownAni.Clear();
}

ioExtendJump* ioEvaComboJump::Clone()
{
	return new ioEvaComboJump( *this );
}

ioEvaComboJump::~ioEvaComboJump()
{

}

JumpType ioEvaComboJump::GetType() const
{
	return JT_EVACOMBO_JUMP;
}

void ioEvaComboJump::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendJump::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	// 기본 점프 차징
	m_dwChargingTime = (DWORD)rkLoader.LoadInt_e( "jump_charge_time", 0 );

	// 기본 점프 공격
	m_iMaxComboCnt = rkLoader.LoadInt_e( "jump_max_combo", 0 );
	for( int i = 0; i < m_iMaxComboCnt; ++i )
	{
		AttackAttribute kAttribute;

		wsprintf_e( szBuf, "jump_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_AttackAttributeList.push_back( kAttribute );
	}


	// 
	// D~ 바라보는 방향의 적 조준
	rkLoader.LoadString_e( "jump_aim_up_animation", "", szBuf, MAX_PATH );
	m_JumpAimUp = szBuf;
	rkLoader.LoadString_e( "jump_aim_center_animation", "", szBuf, MAX_PATH );
	m_JumpAimCenter = szBuf;
	rkLoader.LoadString_e( "jump_aim_down_animation", "", szBuf, MAX_PATH );
	m_JumpAimDown = szBuf;

	m_fMarkerRange = rkLoader.LoadFloat_e( "marker_range", 0.0f );

	m_vChargeComboAttList.clear();
	int iMaxChargeCombo = rkLoader.LoadInt_e( "jump_max_charge_combo", 0 );
	for( i = 0; i < iMaxChargeCombo; i++ )
	{
		AttackAttribute kAttribute;

		wsprintf_e( szBuf, "jump_charge_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vChargeComboAttList.push_back( kAttribute );
	}

	//////////////////////////////////////////////////////////////////////////
	m_JumpMotionInfoList.clear();
	m_JumpMotionInfoList.reserve( iMaxChargeCombo );
	for( int i=0; i < iMaxChargeCombo; ++i )
	{
		JumpMotionInfo kInfo;

		wsprintf_e( szKey, "jump_fire_up_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_FireUp = szBuf;

		wsprintf_e( szKey, "jump_fire_center_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_FireCenter = szBuf;

		wsprintf_e( szKey, "jump_fire_down_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_FireDown = szBuf;

		m_JumpMotionInfoList.push_back( kInfo );
	}

	// 특별 D~ 누른 상태에서 차징 시간
	m_dwJumpChargeTime = rkLoader.LoadInt_e( "jump_charge_time", 0 );
}

void ioEvaComboJump::InitJump()
{

}

void ioEvaComboJump::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	m_EvaComboJumpState			= ECJS_NONE;
	m_iCurComboCnt				= 0;
	m_dwEnableComboTime_S		= 0;
	m_dwEnableComboTime_E		= 0;
	m_dwChargingStartTime		= 0;
	m_bReserveJumpAttack		= false;
}

DWORD ioEvaComboJump::GetChargingTime( ioBaseChar *pChar )
{
	return m_dwJumpChargeTime;
}

void ioEvaComboJump::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge )
{
	if( !pOwner )	{	return;	}
	
	DWORD dwCurTime = FRAMEGETTIME();

	// 특별 D~ 누른 상태에서 점프 했을 경우에 2020-11-25
	if( ( m_bReserveJumpAttack || pOwner->IsAttackKeyDown() ) && !pOwner->IsBuffLimitAttack() ) 
	{
		pOwner->SetNoWoundState( false );
		ChangeToJumpAttack( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_EXTEND_JUMP );
			kPacket << pOwner->GetCharName();
			kPacket << (int)GetType();
			kPacket << pOwner->GetHP().m_fCurValue;
			kPacket << ECJS_CHANGE_JUMP_ATTACK;
			kPacket << pOwner->GetTargetRot();
			pOwner->FillAutoTargetInfo( kPacket );
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioEvaComboJump::ProcessLanding( ioBaseChar *pOwner )
{
}

void ioEvaComboJump::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner ) return;

	switch( m_EvaComboJumpState )
	{
		case ECJS_NONE:
		{
			break;
		}
		case ECJS_CHARGE:
		{	
			break;
		}
		case ECJS_NORMAL_ATTACK:
		{
			ProcessComboJumpAttack( pOwner );
			break;
		}
		case ECJS_AIM:
		{
			ProcessAim( pOwner );
			break;
		}
	}
}

void ioEvaComboJump::ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )	{	return;	}
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )	{	return;	}

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
		case ECJS_CHANGE_JUMP_ATTACK:	// 특별 D~ 누른 상태에서 점프 했을 경우에
		{
			ChangeToJumpAttack( pOwner );
			break;
		}

		case ECJS_NORMAL_ATTACK:
		{
			rkPacket >> m_iCurComboCnt;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			D3DXVECTOR3 vMoveDir, vInputDir;
			rkPacket >> vMoveDir >> vInputDir;

			pOwner->SetJumpAttckTargetRot( qtRot, vMoveDir, vInputDir );

			SetComboJumpAttack( pOwner );

			pOwner->ApplyAutoTargetInfo( rkPacket );
			break;
		}

		case ECJS_CHANGE_AIM:
		{			
			ChangeToAimState(pOwner);
			break;
		}

		case ECJS_FIRE:
		{
			rkPacket >> m_iChargeCombo;
			rkPacket >> m_AimedTarget;

			if( m_AimedTarget.IsEmpty() )
			{
				ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
				ioEvaItem *pEvaItem = ToEvaItem( pItem );
				if( pEvaItem )
				{
					pEvaItem->ClearTarget();
				}
			}
			SetChangeFireMotion(pOwner);
			break;
		}
	}
}

void ioEvaComboJump::OnEndExtendJump( ioBaseChar *pOwner )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 기본 함수 정의
void ioEvaComboJump::ChangeToJumpAttack( ioBaseChar *pOwner )
{
	m_EvaComboJumpState = ECJS_NORMAL_ATTACK;
	pOwner->SetJumpAttackState();
	m_dwChargingStartTime = FRAMEGETTIME();	
}

void ioEvaComboJump::ProcessComboJumpAttack( ioBaseChar *pOwner )
{
	if( !pOwner )	{	return;	}

	DWORD dwCurTime = FRAMEGETTIME();

	ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
	ioEvaItem *pEvaItem = ToEvaItem( pItem );

	if( pEvaItem && pOwner->IsNeedProcess() && pOwner->IsAttackKeyDown() && COMPARE( m_iCurComboCnt, 0, m_iMaxComboCnt ) )
	{
		// 차징 
		if( m_dwChargingStartTime + m_dwChargingTime < dwCurTime )
		{
			// 게이지 체크
			int nCurBulletCnt = pEvaItem->GetCurBullet();
			int nNeedBullet = pEvaItem->GetNeedBullet();
			if( nCurBulletCnt >= nNeedBullet )
			{
				ChangeToAimState( pOwner );

				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_EXTEND_JUMP );
					kPacket << pOwner->GetCharName();
					kPacket << (int)GetType();
					kPacket << pOwner->GetHP().m_fCurValue;
					kPacket << ECJS_CHANGE_AIM;
					kPacket << pOwner->GetTargetRot();
					pOwner->FillAutoTargetInfo( kPacket );
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
			else
			{
				// 게이지 부족시 일반 공격
				m_dwChargingStartTime = FRAMEGETTIME();
				CheckAirJump( pOwner );

				if( m_bReserveJumpAttack )
				{
					DWORD dwLooseTime = 0;

					if( pOwner )
						dwLooseTime = pOwner->GetActionStopTotalDelay();

					if( dwCurTime >= m_dwEnableComboTime_S+dwLooseTime )
					{
						SetComboJumpAttack( pOwner );
					}
				}
			}
		}
	}
	else
	{
		m_dwChargingStartTime = FRAMEGETTIME();
		CheckAirJump( pOwner );

		if( m_bReserveJumpAttack )
		{
			DWORD dwLooseTime = 0;

			if( pOwner )
				dwLooseTime = pOwner->GetActionStopTotalDelay();

			if( dwCurTime >= m_dwEnableComboTime_S+dwLooseTime )
			{
				SetComboJumpAttack( pOwner );
			}
		}
	}
}

void ioEvaComboJump::ChangeToAimState( ioBaseChar *pOwner )
{
	ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
	ioEvaItem *pEvaItem = ToEvaItem( pItem );
	if( !pEvaItem )	{	return;	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )	{	return;	}

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetLoopAni( m_JumpAimUp, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_JumpAimDown, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_JumpAimCenter, FLOAT1 );

	ioHashString stString;
	stString.Clear();
	m_AimedTarget = pEvaItem->FindAimTarget(pOwner, stString);
	m_FirstAimedTarget = m_AimedTarget;

	pOwner->SetTargetMarkerRange( m_fMarkerRange );

	if( !m_AimedTarget.IsEmpty() )
	{
		pOwner->SetTargetMarkerTarget( m_AimedTarget, false );
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );

		pEvaItem->TrackingTarget(pOwner, m_AimedTarget );
		m_bFindJumpTarget = true;
	}
	else
	{
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
		m_bFindJumpTarget = false;
	}

	m_EvaComboJumpState	= ECJS_AIM;
	m_bFirstAimBullet	= true;
	m_iChargeCombo		= 0;

	m_dwFireStartTime	= 0;
	m_dwFireEndTime		= 0;
	m_dwKeyReserveTime	= 0;

	m_stFireUpAni.Clear();
	m_stFireCenterAni.Clear();
	m_stFireDownAni.Clear();

	m_bCheckFirstTarget = true;

	ClearAirJump();
}

void ioEvaComboJump::ProcessAim( ioBaseChar *pOwner )
{
	DWORD dwCurTime		= FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();

	ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
	ioEvaItem *pEvaItem = ToEvaItem( pItem );
	if( !pEvaItem )
		return;

	if( m_iChargeCombo >= (int)m_vChargeComboAttList.size() )
		return;

	ioHashString stAimedTarget;
	stAimedTarget.Clear();

	//타겟이 중간에 타겟팅이 불가능한 위치로 이동하였다면
	if( m_bCheckFirstTarget && !m_FirstAimedTarget.IsEmpty() )
	{
		if( !pEvaItem->CheckCurrentTargetValid( pOwner, true ) )
		{
			m_bCheckFirstTarget = false;
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
			pEvaItem->SetNullAimedTarget();
			m_AimedTarget.Clear();
		}
	}

	if( m_FirstAimedTarget.IsEmpty() && m_bFirstAimBullet )
		stAimedTarget = pEvaItem->FindAimTarget(pOwner, m_FirstAimedTarget);
	
	if( m_FirstAimedTarget.IsEmpty() && !stAimedTarget.IsEmpty() )
	{
		m_FirstAimedTarget = stAimedTarget;
		m_AimedTarget = stAimedTarget;
		pOwner->SetTargetMarkerTarget( m_AimedTarget, false );
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
	}
	
	pEvaItem->TrackingTarget( pOwner, m_stFireUpAni, m_stFireCenterAni, m_stFireDownAni );
	pEvaItem->UpdateAimDir( pOwner, true );

	CheckAirJump( pOwner );

	if( pOwner->IsNeedProcess() )
	{
		// 게이지 체크
		int nCurBulletCnt = pEvaItem->GetCurBullet();
		int nNeedBullet = pEvaItem->GetNeedBullet();
		if( nCurBulletCnt < nNeedBullet )	{	return;	}

		if( m_bFirstAimBullet )
		{
			if( !pOwner->IsAttackKeyDown() )
			{
				SetChangeFireMotion(pOwner);	//	빔 발사				
				m_bFirstAimBullet = false;

				m_iChargeCombo++;

				if( m_AimedTarget.IsEmpty() )
					pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
			}
		}
		else
		{
			if( pOwner->IsAttackKeyDown() && m_dwKeyReserveTime < dwCurTime )
			{
				DWORD dwLooseTime = 0;

				if( pOwner )
					dwLooseTime = pOwner->GetActionStopTotalDelay();

				SetChangeFireMotion(pOwner);				
				m_iChargeCombo++;

				m_bReserveJumpAttack = false;
			}
		}
	}
}

void ioEvaComboJump::CheckComboJumpAttack( ioBaseChar *pOwner )
{
	if( !COMPARE( m_iCurComboCnt, 0, m_iMaxComboCnt ) )
	{
		return;
	}

	if( m_bReserveJumpAttack )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLooseTime = 0;

	if( pOwner )
		dwLooseTime = pOwner->GetActionStopTotalDelay();

	if( m_iCurComboCnt > 0 && !COMPARE( dwCurTime, m_dwEnableComboTime_S+dwLooseTime, m_dwEnableComboTime_E+dwLooseTime ) )
		return;

	if( m_iCurComboCnt > 0 && pOwner->IsHasCrown() )
		return;

	m_bReserveJumpAttack = true;
}

void ioEvaComboJump::SetComboJumpAttack( ioBaseChar *pOwner )
{
	if( !pOwner )	{	return;	}

	if( !COMPARE( m_iCurComboCnt, 0, m_iMaxComboCnt ) )
	{
		return;
	}

	DWORD dwCurTime			= FRAMEGETTIME();

	if( pOwner->IsNeedProcess() && pOwner->IsCanRotateJumpAttack() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, false, false );
			pOwner->SetInputDirection( pOwner->GetMoveDir() );
		}
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( pOwner && !pOwner->IsActionStop() )
	{
		pOwner->InitActionStopDelay( true );
	}
	else
	{
		pOwner->InitActionStopDelay( false );
	}

	int iAniID			= pGrp->GetAnimationIdx( m_AttackAttributeList[m_iCurComboCnt].m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioEvaComboJump::SetComboJumpAttack - %s Animation is Not Exist",
								m_AttackAttributeList[m_iCurComboCnt].m_AttackAnimation.c_str() );
		return;
	}

	float fTimeRate		= m_AttackAttributeList[m_iCurComboCnt].m_fAttackAniRate;
	DWORD dwPreDelay	= m_AttackAttributeList[m_iCurComboCnt].m_dwPreDelay;

	DWORD dwStartTime = dwCurTime + dwPreDelay;
	m_dwEnableComboTime_E = dwStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;


	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		
	pOwner->RefreshFireTimeList( iAniID, m_AttackAttributeList[m_iCurComboCnt].m_vWeaponInfoList,
								 FTT_JUMP_ATTACK,
								 fTimeRate,
								 dwPreDelay );
	 
	pOwner->IncreaseWeaponIndexBase();

	DWORD dwTrackingTime = dwStartTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	m_dwEnableComboTime_S = dwStartTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_EvaComboJumpState;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetMoveDir();
		kPacket << pOwner->GetInputDir();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_bReserveJumpAttack = false;
	m_bUseExtendJump = true;
	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );

	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	m_iCurComboCnt++;
	pOwner->SetJumpAttackState();
}

void ioEvaComboJump::SetChangeFireMotion( ioBaseChar *pOwner)
{
	if( !COMPARE( m_iChargeCombo, 0, (int)m_vChargeComboAttList.size() ) )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) 
		return;

	// FireDir 처리
	// 발사 모션 처리
	int	  iAniID = -1;
	float fTimeRate = FLOAT1;
	float fWeight = 0.0f;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );

	const AttackAttribute *pAttr = &m_vChargeComboAttList[m_iChargeCombo];

	if( pAttr )
	{
		iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
		fTimeRate = pAttr->m_fAttackAniRate;

		pOwner->RefreshFireTimeList( iAniID, pAttr->m_vWeaponInfoList, FTT_JUMP_ATTACK, fTimeRate, 0 );
		pOwner->IncreaseWeaponIndexBase();

		// For AirJumpPower
		SetAirJump( pOwner, iAniID, fTimeRate );
	}

	ioBall *pBall = pOwner->FindBall();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( pTarget )
	{
		D3DXVECTOR3 vAimedDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		D3DXVec3Normalize( &vAimedDir, &vAimedDir );

		fWeight = GetAimedUpDownWeight( vAimedDir, 85.0f );
		m_vAimedDir = vAimedDir;

		if( m_vAimedDir.y >= 0.0f )
		{
			AimedFireToUp( pGrp, fWeight, fTimeRate, 0, true, true );
		}
		else
		{
			AimedFireToDown( pGrp, fWeight, fTimeRate, 0, true, true );
		}
	}
	else
	{
		ioHashString szFireUp, szFireCenter;
		if( COMPARE( m_iChargeCombo, 0, (int)m_JumpMotionInfoList.size() ) )
		{
			szFireCenter = m_JumpMotionInfoList[m_iChargeCombo].m_FireCenter;
			pGrp->SetActionAni( szFireCenter, 0.0f, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0, 0, true );
		}
	}

	m_stFireUpAni = m_JumpMotionInfoList[m_iChargeCombo].m_FireUp;
	m_stFireCenterAni = m_JumpMotionInfoList[m_iChargeCombo].m_FireCenter;
	m_stFireDownAni = m_JumpMotionInfoList[m_iChargeCombo].m_FireDown;


	// 시간값들 갱신
	m_dwFireStartTime = dwCurTime;
	m_dwFireEndTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwKeyReserveTime = m_dwFireStartTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		// 패킷 수정완료
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << ECJS_FIRE;
		kPacket << m_iChargeCombo;
		kPacket << m_AimedTarget;
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	// 특수 기능2 : 생성한 서브웨폰 발사
	ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
	ioEvaItem *pEvaItem = ToEvaItem( pItem );
	if( pEvaItem && !m_AimedTarget.IsEmpty() )
	{
		pEvaItem->SetSubWeaponAttackState( pOwner, 0 );
	}
}

float ioEvaComboJump::GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle )
{
	D3DXVECTOR3 vXZDir( vAimDir.x, 0.0f, vAimDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	float fAngle = D3DXVec3Dot( &vXZDir, &vAimDir );
	fAngle = max( -FLOAT1, min( fAngle, FLOAT1 ) );
	fAngle = acosf( fAngle );
	fAngle = RADtoDEG( fAngle );
	fAngle = max( 0.0f, min( fAngle, fMaxAngle ) );

	float fWeight = fAngle / fMaxAngle;

	return fWeight;
}


void ioEvaComboJump::AimedFireToUp( ioEntityGroup *pGrp, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bJump, bool bHold )
{
	float fFadeOut = 0.0f;

	bool bAniFx1 = false;
	bool bAniFx2 = true;

	if( fWeight > FLOAT05 )
	{
		bAniFx1 = true;
		bAniFx2 = false;
	}

	ioHashString szFireUp, szFireCenter;
	if( bJump )
	{
		if( COMPARE( m_iChargeCombo, 0, (int)m_JumpMotionInfoList.size() ) )
		{
			szFireUp = m_JumpMotionInfoList[m_iChargeCombo].m_FireUp;
			szFireCenter = m_JumpMotionInfoList[m_iChargeCombo].m_FireCenter;
		}
	}

	if( fWeight > 0.0f )
	{
		pGrp->SetActionAni( szFireUp,
			0.0f, fFadeOut,
			fWeight, FLOAT1/fTimeRate,
			0.0f, dwPreDelay,
			bHold, bAniFx1 );
	}

	pGrp->SetActionAni( szFireCenter,
		0.0f, fFadeOut,
		1.0f - fWeight, FLOAT1/fTimeRate,
		0.0f, dwPreDelay,
		bHold, bAniFx2 );
}



void ioEvaComboJump::AimedFireToDown( ioEntityGroup *pGrp, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bJump, bool bHold )
{
	float fFadeOut = 0.0f;

	bool bAniFx1 = true;
	bool bAniFx2 = false;

	if( fWeight > FLOAT05 )
	{
		bAniFx1 = false;
		bAniFx2 = true;
	}

	ioHashString szFireDown, szFireCenter;

	if( bJump )
	{
		if( COMPARE( m_iChargeCombo, 0, (int)m_JumpMotionInfoList.size() ) )
		{
			szFireDown = m_JumpMotionInfoList[m_iChargeCombo].m_FireDown;
			szFireCenter = m_JumpMotionInfoList[m_iChargeCombo].m_FireCenter;
		}
	}

	if( fWeight > 0.0f )
	{
		pGrp->SetActionAni( szFireCenter,
			0.0f, fFadeOut,
			1.0f - fWeight, FLOAT1/fTimeRate,
			0.0f, dwPreDelay,
			bHold, bAniFx1 );
	}

	pGrp->SetActionAni( szFireDown,
		0.0f, fFadeOut,
		fWeight, FLOAT1/fTimeRate,
		0.0f, dwPreDelay,
		bHold, bAniFx2 );
}

// 점프 하는 순간 CheckComboJumpAttack() 함수를 호출하여 사용함
bool ioEvaComboJump::IsJumpAttackReserve()
{
	return false;
}