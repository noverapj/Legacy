#include "stdafx.h"
#include "ioGenisysarmyItem.h"
#include "ioBaseChar.h"
#include "WeaponDefine.h"
#include "ItemDefine.h"
#include "ioDummyGradeTurret.h"

ioGenisysarmyItem::ioGenisysarmyItem()
{
	m_FireState					= FS_NONE;
	m_dwAttackStartTime			= 0;
	m_iCurCombo					= 0;
	m_dwMotionEndTime			= 0;
	m_dwCancelDashAttackTime	= 0;
	m_SpecialState				= SS_NONE;

	// D~
	m_bAimedState				= false;
	m_bAimedBall				= false;
	m_fCurAngle					= 0.0f;
	m_vAimedDir					= ioMath::VEC3_ZERO;

	// 특별 공격 D~
	m_AimStartDirKey			= ioUserKeyInput::DKI_NONE;
	m_bAimMotionSetted			= false;
	m_bFirstAimMoveMotion		= true;

	// 특별 공격 A~ ( 더미 생성 )
	m_dwDummyMakeEndTime		= 0;
}

ioGenisysarmyItem::ioGenisysarmyItem( const ioGenisysarmyItem &rhs )
	: ioWeaponItem( rhs ),	
	m_iAttackMaxComboCnt( rhs.m_iAttackMaxComboCnt ),				
	m_AttackReadyAni( rhs.m_AttackReadyAni ),						
	m_NormalCancelDashAttribute( rhs.m_NormalCancelDashAttribute ),
	m_AttackExtendAttribute( rhs.m_AttackExtendAttribute ),
	m_iAttackExtendAttributeCnt( rhs.m_iAttackExtendAttributeCnt ),

	m_AimEffect( rhs.m_AimEffect ),
	m_fAimAngle( rhs.m_fAimAngle ),
	m_fAimRange( rhs.m_fAimRange ),
	m_fMinRange( rhs.m_fMinRange ),
	m_fMarkerRange( rhs.m_fMarkerRange ),
	m_bEnableDownAim( rhs.m_bEnableDownAim ),	// 다운타격 가능 여부 2020-07-16
	m_FireUp( rhs.m_FireUp ),
	m_FireCenter( rhs.m_FireCenter ),
	m_FireDown( rhs.m_FireDown ),
	m_AimMoveFront( rhs.m_AimMoveFront ),
	m_AimMoveBack( rhs.m_AimMoveBack ),
	m_AimMoveLeft( rhs.m_AimMoveLeft ),
	m_AimMoveRight( rhs.m_AimMoveRight ),
	m_JumpChargeAttribute( rhs.m_JumpChargeAttribute ),
	m_FireEffect( rhs.m_FireEffect ),
	m_fAimMoveAniRate( rhs.m_fAimMoveAniRate ),

	// Jump Attack
	m_JumpFireUp( rhs.m_JumpFireUp ),
	m_JumpFireCenter( rhs.m_JumpFireCenter ),
	m_JumpFireDown( rhs.m_JumpFireDown ),

	// 특별 공격 키 A~
	m_DummyInfoList( rhs.m_DummyInfoList ),
	m_DummyChargeAttribute( rhs.m_DummyChargeAttribute ),

	// 특별 공격 A~ ( 더미 생성 )
	m_fDummySetupOffset( rhs.m_fDummySetupOffset ),
	m_iDummyMaxCnt( rhs.m_iDummyMaxCnt ),
	m_DummyMakeAnimation( rhs.m_DummyMakeAnimation ),
	m_fDummyMakeAnimationRate( rhs.m_fDummyMakeAnimationRate )
{
	m_FireState					= FS_NONE;
	m_dwAttackStartTime			= 0;
	m_iCurCombo					= 0;
	m_dwMotionEndTime			= 0;
	m_dwCancelDashAttackTime	= 0;
	m_SpecialState				= SS_NONE;

	// D~
	m_bAimedState				= false;
	m_bAimedBall				= false;
	m_fCurAngle					= 0.0f;
	m_vAimedDir					= ioMath::VEC3_ZERO;

	// 특별 공격 D~
	m_AimStartDirKey			= ioUserKeyInput::DKI_NONE;
	m_bAimMotionSetted			= false;
	m_bFirstAimMoveMotion		= true;

	// 특별 공격 A~ ( 더미 생성 )
	m_dwDummyMakeEndTime		= 0;
}

ioGenisysarmyItem::~ioGenisysarmyItem()
{
}

void ioGenisysarmyItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	// 기본 Charge 준비 단계..
	m_iAttackMaxComboCnt = GetMaxCombo();
	rkLoader.LoadString_e( "attack_ready_animation", "", szBuf, MAX_PATH );
	m_AttackReadyAni = szBuf;

	// 특별 기본공격 중 캔슬 대시 
	LoadAttackAttribute_e( "normal_cancel_dash", m_NormalCancelDashAttribute, rkLoader );

	// 특별 공격 D~
	m_iAttackExtendAttributeCnt = rkLoader.LoadInt_e( "extend_attack_cnt", 0 );
	LoadAttackAttribute_e( "extend_attack", m_AttackExtendAttribute, rkLoader );

	// Charge D~
	rkLoader.LoadString_e( "aim_effect", "", szBuf, MAX_PATH );
	m_AimEffect = szBuf;
	m_fMarkerRange		= rkLoader.LoadFloat_e( "marker_range", 0.0f );
	
	// Charge D~ Target
	m_fAimAngle			= rkLoader.LoadFloat_e( "aim_angle", 45.0f );
	m_fAimRange			= rkLoader.LoadFloat_e( "aim_range", FLOAT1000 );
	m_fMinRange			= rkLoader.LoadFloat_e( "aim_min_range", 0.0f );
	m_bEnableDownAim	= rkLoader.LoadBool_e( "enable_down_aim", false );	// 다운타격 가능 여부 2020-07-16

	// Jump Attack
	rkLoader.LoadString_e( "attack_fire_up_animation", "", szBuf, MAX_PATH );
	m_JumpFireUp = szBuf;
	rkLoader.LoadString_e( "attack_fire_center_animation", "", szBuf, MAX_PATH );
	m_JumpFireCenter = szBuf;
	rkLoader.LoadString_e( "attack_fire_down_animation", "", szBuf, MAX_PATH );
	m_JumpFireDown = szBuf;

	// Aim
	rkLoader.LoadString_e( "fire_up_animation", "", szBuf, MAX_PATH );
	m_FireUp = szBuf;
	rkLoader.LoadString_e( "fire_center_animation", "", szBuf, MAX_PATH );
	m_FireCenter = szBuf;
	rkLoader.LoadString_e( "fire_down_animation", "", szBuf, MAX_PATH );
	m_FireDown = szBuf;

	rkLoader.LoadString_e( "aim_move_front", "", szBuf, MAX_PATH );
	m_AimMoveFront = szBuf;
	rkLoader.LoadString_e( "aim_move_back", "", szBuf, MAX_PATH );
	m_AimMoveBack = szBuf;
	rkLoader.LoadString_e( "aim_move_left", "", szBuf, MAX_PATH );
	m_AimMoveLeft = szBuf;
	rkLoader.LoadString_e( "aim_move_right", "", szBuf, MAX_PATH );
	m_AimMoveRight = szBuf;

	rkLoader.LoadString_e( "fire_effect", "", szBuf, MAX_PATH );
	m_FireEffect = szBuf;

	m_fAimMoveAniRate = rkLoader.LoadFloat_e( "aim_move_ani_rate", FLOAT1 );

	// Jump Attack
	m_JumpChargeAttribute.Init();
	LoadAttackAttribute( "jump_charge_attack", m_JumpChargeAttribute, rkLoader );

	// 특별 공격 키 A~ Charge
	m_DummyChargeAttribute.Init();
	LoadAttackAttribute( "dummy_charge_ani", m_DummyChargeAttribute, rkLoader );

	// 특별 공격 키 A~
	m_iDummyMaxCnt = rkLoader.LoadInt_e( "dummy_cnt", 0 );
	for( int i = 0; i < m_iDummyMaxCnt; ++i )
	{
		GroupDummyInfo Info;

		wsprintf_e( szKey, "dummy%d_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		Info.szDummyName = szBuf;

		wsprintf_e( szKey, "dummy%d_need_gauge", i+1 );
		Info.fNeedGauge = rkLoader.LoadFloat( szKey, 0.0f );
		m_DummyInfoList.push_back( Info );
	}

	// 특별 공격 A~ ( 더미 생성 )
	m_fDummySetupOffset	= rkLoader.LoadFloat_e( "dummy_setup_offset", 0.0f );
	rkLoader.LoadString_e( "dummy_make_animation", "", szBuf, MAX_PATH );
	m_DummyMakeAnimation = szBuf;
	m_fDummyMakeAnimationRate = rkLoader.LoadFloat_e( "dummy_make_animation_rate", FLOAT1 );
}

ioItem* ioGenisysarmyItem::Clone()
{
	return new ioGenisysarmyItem( *this );
}

ioWeaponItem::WeaponSubType ioGenisysarmyItem::GetSubType() const
{  
	return WST_GENISYSARMY_ITEM;
}

// 2020-07-08
void ioGenisysarmyItem::SetNoneState( ioBaseChar* pOwner, int eNewState /*= 0 */ )
{
	m_AimedTarget.Clear();
	m_bAimedBall	= false;
	m_bAimedState	= false;
	m_FireState		= FS_NONE;
}

void ioGenisysarmyItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );

	// ClearKeyReserve 초기화
	ClearCancelInfo();

	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );		
		m_SpecialState	= SS_NONE;	// 2020-07-22
		m_FireState		= FS_NONE;
		m_iCurCombo		= iCurCombo;
	}
	else
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAni );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_SpecialState			= SS_NONE;	// 2020-07-22
		m_FireState				= FS_CHARGING;
		m_dwAttackStartTime		= FRAMEGETTIME();
		m_iCurCombo				= iCurCombo;
		DWORD dwTrackingTime	= FRAMEGETTIME();

		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

		pOwner->SetState( CS_ATTACK );
	}
}

void ioGenisysarmyItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_FireState )
	{
		case FS_CHARGING:
		{
			OnCharging( pOwner );
			break;
		}

		case FS_NORMAL_ATTACK:
		{
			if( pOwner->IsNeedProcess() )
			{
				// 캔슬 대시
				CheckCancelReserveByConditionalSpecialDash( pOwner ); // m_bEnableDashState 체크 
				if( ProcessCancelBySpeicalDash( pOwner ) )
				{
					return;
				}

				CheckCancelReserve( pOwner );
				if( ProcessCancel(pOwner) )
					return;
			}

			ioWeaponItem::CheckNormalAttack( pOwner );
			break;
		}

		case FS_CHARGE_ATTACK:
		{
			ProcessChargeAttack( pOwner );
			break;
		}
	}

	UpdateAimDir( pOwner );
	CheckAimMoveAniState( pOwner );
}

void ioGenisysarmyItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToChargeAttackState( pOwner );
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioGenisysarmyItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearCancelInfo();

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni = pAttribute->m_AttackAnimation;
		m_fCurAniRate = pAttribute->m_fAttackAniRate;

		// 캔슬 체크 시간 
		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_FireState = FS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( ST_NORMAL_ATTACK );
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGenisysarmyItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	BYTE btState;
	rkPacket >> btState;

	switch( btState )
	{
		case ST_NORMAL_ATTACK:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );
			rkPacket >> m_iCurCombo;
			ChangeToNormalAttack( pOwner );
			break;
		}

		case ST_JUMP_ATTACK:
		{
			rkPacket >> m_iCurCombo;
			rkPacket >> m_AimedTarget;
			rkPacket >> m_bAimedBall;

			pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
			if( !m_AimedTarget.IsEmpty() || m_bAimedBall )
			{
				pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
			}
			else
			{
				pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
			}
			DWORD dwInput_Dash_s;
			SetChangeJumpFireMotion( pOwner, dwInput_Dash_s );

			break;
		}

		case ST_CHARGE_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotAndMoveDirChange( qtRot );
			int iDir;
			rkPacket >> iDir;
			m_AimStartDirKey = (ioUserKeyInput::DirKeyInput)iDir;
			rkPacket >> m_AimedTarget;
			rkPacket >> m_bAimedBall;
			ChangeToChargeAttackState( pOwner );
			break;
		}

		case ST_CHARGE_ATTACK_COMBO:
		{
			rkPacket >> m_AimedTarget;
			rkPacket >> m_bAimedBall;
			rkPacket >> m_iCurCombo;

			pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
			if( !m_AimedTarget.IsEmpty() || m_bAimedBall )
			{
				pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
			}
			else
			{
				pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
			}
			SetChangeFireMotion( pOwner );
			break;
		}
		case SS_DUMMY_MAKE_CREATE:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			D3DXVECTOR3 vPos = ioMath::VEC3_ZERO;
			rkPacket >> vPos;

			ioHashString szNewDummy;
			int iNewDummyIndex = 0;			
			int iDieDummyIndex = 0;

			rkPacket >> szNewDummy;
			rkPacket >> iNewDummyIndex;
			rkPacket >> iDieDummyIndex;
			ioDummyChar* pNewDummy = CreateDummyChar( szNewDummy, pOwner->GetCreator(), pOwner, iNewDummyIndex );
			if( !pNewDummy )
			{
				LOG.PrintTimeAndLog(0, "%s Create Dummy Failed : %s", __FUNCTION__, szNewDummy.c_str() );
				return;
			}

			ioDummyChar *pDieDummy = g_DummyCharMgr.FindDummyChar( pOwner, iDieDummyIndex );
			if( pDieDummy )
			{
				pDieDummy->SetDieState( true );
			}			

			ChangeToDummyMake( pOwner->GetCreator(), pOwner, 0 );
			break;
		}
	}
}

void ioGenisysarmyItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
	{
		return;
	}

	int nState;
	rkPacket >> nState;

	switch( nState )
	{
		case SS_SPECIAL_DASH:
		{
			D3DXVECTOR3 vWorldPos;
			rkPacket >> vWorldPos;
			pOwner->SetWorldPosition( vWorldPos);

			int	iCurDir, iCallerType;
			rkPacket >> iCurDir;
			rkPacket >> iCallerType;

			SetChangeConditionalSpecialDash( pOwner, (ioUserKeyInput::DirKeyInput)iCurDir, iCallerType );
			break;
		}
		case SS_DUMMY_READY:
		{
			SetDummyChargeState( pOwner );
			break;
		}

	}

}

// 기본 공격중 캔슬 대시 기능 함수들....
bool ioGenisysarmyItem::ProcessCancelBySpeicalDash( ioBaseChar *pOwner )
{
	if( m_szCurAni.IsEmpty() )
		return false;

	if( m_dwInputCancelCheckTime == 0 )
		return false;

	if( !pOwner )
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	int iAniID = pGrp->GetAnimationIdx( m_szCurAni );			
	DWORD dwInputSpecialDashTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" );
	if( 0 < dwInputSpecialDashTime )
	{
		dwInputSpecialDashTime *= m_fCurAniRate;
		dwInputSpecialDashTime += m_dwInputCancelCheckTime;		
	}

	if( 0 < dwInputSpecialDashTime && dwCurTime > dwInputSpecialDashTime )
	{
		if( m_bEnableDashState && IsConditionalSpecialDashReserve( pOwner ) )
		{
			SetChangeConditionalSpecialDash( pOwner, m_ConditionalSpecialDashKey, ioAttackableItem::CCSD_NORMAL );
			return true;
		}
	}

	return false;
}

// ioBaseChar::CheckEnableDashCheck() 함수를 체크 하기 위해서는 m_CurDashType == DT_GENISYSARMY 추가해줘야 함 
// 또한 ioAttackableItem 기본은 무조건 false 리턴시킨다.
bool ioGenisysarmyItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if ( !pOwner )
		return	false;

	// 기본공격중 일때 m_bEnableDashCheck 상태 값을 변경하여 ioBaseChar::ProcessKeyInput() 함수에서 체크 중..
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_FireState )
		{
			case FS_NORMAL_ATTACK:
			{
				if( !bCheck )	{	return	false;	}
				return	true;
			}
		}
	}

	return	false;
}

bool ioGenisysarmyItem::IsConditionalSpecialDashReserve( ioBaseChar *pOwner )
{
	if( m_SpecialState == SS_SPECIAL_DASH )
	{
		return	false;
	}

	// 기본 대시일때 2020-07-16
	if( pOwner->GetState() == CS_EXTEND_DASH )
	{
		return false;
	}

	return	true;
}

void ioGenisysarmyItem::SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType )
{
	if( !pOwner || eCurrDir == ioUserKeyInput::DKI_NONE )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_NormalCancelDashAttribute.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	DWORD dwPreDelay = m_NormalCancelDashAttribute.m_dwPreDelay;
	DWORD dwEndDelay = m_NormalCancelDashAttribute.m_dwEndDelay;
	DWORD dwStartTime = FRAMEGETTIME() + dwPreDelay;

	ClearCancelInfo();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetChargingState( false );

	float fYawD = 180.0f + 45.0f * ( eCurrDir - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );


	pOwner->SetTargetRotToRotate( qtRotate, true, false );

	float fTimeRate = m_NormalCancelDashAttribute.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( m_NormalCancelDashAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_NormalCancelDashAttribute.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	m_dwMotionEndTime = dwStartTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwEndDelay;

	m_dwKeyReserveTime	= 0;
	DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( dwKeyReserve > 0 )
		m_dwKeyReserveTime += dwStartTime;

	DWORD dwInputSpecialDashTime = pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" ) * fTimeRate;
	if( dwInputSpecialDashTime > 0 )
		m_dwCancelDashAttackTime = dwStartTime + dwInputSpecialDashTime;

	m_SpecialState	= SS_SPECIAL_DASH;
	pOwner->SetState( CS_GENISYSARMY_SPECIAL );
	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );		
		kPacket << m_SpecialState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << static_cast<int>( eCurrDir );
		kPacket << eCallerType;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGenisysarmyItem::ChangeReserveKeyInputState( ioBaseChar *pOwner, int iCurCombo /* = 0 */ )
{
	// 필살기 예약되었으면 필살기 발동
	if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
	{
		m_iSkillInput = -1;
		return;
	}

	// 방어
	if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
	{
		pOwner->SetDefenseState();
		return;
	}

	// 점프
	if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
	{
		pOwner->SetJumpState();
		return;
	}

	// 공격
	if( m_bEnableAttackState )
	{
		ioObjectItem *pItem = pOwner->GetObject();

		if( ToDrinkItem( pItem ) )
		{
			pOwner->SetDrinkState();
		}
		else
		{
			if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				if ( !COMPARE( iCurCombo, 0, GetMaxCombo() ) )
					iCurCombo = 0;
				pOwner->SetNormalAttack( iCurCombo );
			}
		}
		return;
	}

	// 대기
	pOwner->SetState( CS_DELAY );
}

void ioGenisysarmyItem::ProcessSpecialState( ioBaseChar *pOwner )
{
	if( !pOwner )
	{
		return;
	}

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
	{
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_SpecialState )
	{
		case SS_SPECIAL_DASH:
		{
			ProcessSpecialDash( pOwner );
			break;
		}

		case SS_DUMMY_READY:
		{
			SetDummyChargeState( pOwner );
			break;
		}
		case SS_DUMMY_MAKE:
		{
			int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
			ChangeToDummyMake( pOwner->GetCreator(), pOwner, iCurIndex );
			break;
		}
		case SS_DUMMY_MAKE_CREATE:
		{
			OnDummyMake( pOwner );
			break;
		}		
	}
}

void ioGenisysarmyItem::ClearAttackState( ioBaseChar *pOwner )
{

}

void ioGenisysarmyItem::ProcessSpecialDash( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( m_dwMotionEndTime < dwCurTime )
	{
		m_FireState		= FS_NONE;
		m_SpecialState	= SS_NONE;
		ChangeReserveKeyInputState( pOwner );

		return;
	}

	if ( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
	{
		CheckReserve( pOwner );
	}
}

// 기본, 점프 D~
bool ioGenisysarmyItem::CompareAimedTarget( ioBaseChar *pOwner )
{
	if( !pOwner )	{	return false;	}

	ioBall *pBall = pOwner->FindBall();
	if( !pBall )	{	return false;	}

	ioBaseChar *pChar = pOwner->GetBaseChar( m_AimedTarget );

	D3DXVECTOR3 vDiff;
	D3DXVECTOR3 vOwnerPos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vCharDir  = pOwner->GetMoveDir();

	float fHalfCosine = cosf( DEGtoRAD( m_fAimAngle ) * FLOAT05 );

	float fLimitDistSq = m_fAimRange * m_fAimRange;
	float fMinDistSq = m_fMinRange * m_fMinRange;

	float fBallDist = std::numeric_limits<float>::infinity();
	float fCharDist = std::numeric_limits<float>::infinity();

	// Ball
	if( pBall )
	{
		vDiff = pBall->GetWorldPosition() - vOwnerPos;
		float fCurDistSq = D3DXVec3LengthSq( &vDiff );

		if( (fLimitDistSq >= fCurDistSq) && (fMinDistSq <= fCurDistSq) )
		{
			vDiff.y = 0.0f;
			D3DXVec3Normalize( &vDiff, &vDiff );

			if( D3DXVec3Dot( &vCharDir, &vDiff ) >= fHalfCosine )
				return true;
		}
	}

	return false;
}

void ioGenisysarmyItem::SetComboChargingJumpAttack( ioBaseChar *pOwner, OUT DWORD& dwInput_Dash_S_StartTime, OUT ioHashString& szTarget, OUT bool& bAimedBall )
{
	if( !pOwner )	{	return;	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )	{	return;	}

	pOwner->StopAutoTargetTracking();

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ChangeDirectionExplicitIfDirChanged( true );
	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	m_vStartDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	if ( pOwner->IsNeedProcess() )
	{
		m_AimedTarget.Clear();
		m_bAimedBall = false;

		// 타겟 찾기
		ioHashString szTarget;		
		if( pOwner->CheckAimedTargetByBow( m_fAimAngle, m_fMinRange, m_fAimRange, szTarget, m_bEnableDownAim ) )
		{
			m_AimedTarget = szTarget;
		}

		// 타겟 비교
		if( CompareAimedTarget( pOwner ) )
		{
			m_bAimedBall = true;
			m_AimedTarget.Clear();
		}
	}

	m_bAimedState	= false;
	m_iCurCombo		= 0;
	m_fCurAngle		= 0.0f;
	if( !m_bAimEffect )
	{
		pOwner->AttachEffect( m_AimEffect );
		m_bAimEffect = true;
	}

	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->ClearAllLoopAni( FLOAT10, true );

	pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );

	if( !m_AimedTarget.IsEmpty() || m_bAimedBall )
	{
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
	}
	else
	{
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
	}

	SetChangeJumpFireMotion( pOwner, dwInput_Dash_S_StartTime );
	szTarget = m_AimedTarget;
	bAimedBall = m_bAimedBall;
}

bool ioGenisysarmyItem::SetChangeJumpFireMotion( ioBaseChar *pOwner, DWORD& dwInput_Dash_S_StartTime )
{
	if( !pOwner )	{	return false;	}

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
	{
		return false;
	}

	// FireDir 처리
	// 발사 모션 처리
	int	  iAniID = -1;
	float fTimeRate = FLOAT1;
	float fWeight = 0.0f;

	pGrp->ClearAllActionAni( 0.0f, true );
	iAniID = pGrp->GetAnimationIdx(m_JumpChargeAttribute.m_AttackAnimation );
	if ( iAniID == -1 )
	{
		return false;
	}

	fTimeRate = m_JumpChargeAttribute.m_fAttackAniRate;

	pOwner->RefreshFireTimeList( iAniID, m_JumpChargeAttribute.m_vWeaponInfoList, FTT_JUMP_ATTACK, fTimeRate, 0 );
	pOwner->IncreaseWeaponIndexBase();

	// AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

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

		m_bAimedState = true;
	}
	else if( m_bAimedBall && pBall )
	{
		D3DXVECTOR3 vAimedDir = pBall->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
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

		m_bAimedState = true;
	}
	else
	{
		pGrp->SetActionAni( m_JumpFireCenter, 0.0f, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0, 0, true );
		m_bAimedState = false;
	}


	// 시간값들 갱신
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwRotateStartTime = dwCurTime;

	m_dwRotateEndTime = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if ( m_dwKeyReserveTime > 0 )
	{
		m_dwKeyReserveTime += dwCurTime;
	}

	DWORD dwInputDashS = pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" ) * fTimeRate;
	if( 0 < dwInputDashS )
	{
		dwInput_Dash_S_StartTime = dwCurTime + dwInputDashS;
	}

	return true;
}

float ioGenisysarmyItem::GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle )
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

void ioGenisysarmyItem::AimedFireToUp( ioEntityGroup *pGrp, float fWeight, float fTimeRate, DWORD dwPreDelay,bool bJump, bool bHold )
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
		szFireUp		= m_JumpFireUp;
		szFireCenter	= m_JumpFireCenter;
	}
	else
	{
		szFireUp		= m_FireUp;
		szFireCenter	= m_FireCenter;
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

void ioGenisysarmyItem::AimedFireToDown( ioEntityGroup *pGrp, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bJump, bool bHold )
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
		szFireDown		= m_JumpFireDown;
		szFireCenter	= m_JumpFireCenter;
	}
	else
	{
		szFireDown		= m_FireDown;
		szFireCenter	= m_FireCenter;
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

bool ioGenisysarmyItem::CheckCurrentTargetValid( ioBaseChar *pOwner, bool bAngleAlsoCheck )
{
	ioBall *pBall = pOwner->FindBall();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( !pTarget && !m_bAimedBall )
	{
		return false;
	}
	else if( !pTarget && !pBall )
	{
		m_bAimedBall = false;
		return false;
	}

	D3DXVECTOR3 vTargetPos;
	if( pTarget )
	{
		if( !pTarget->IsEnableAimTarget(m_bEnableDownAim) )
		{
			return false;
		}

		if( pTarget->HasHideBuff() )
		{
			return false;
		}

		if( pTarget->IsChatModeState( false ) )
		{
			return false;
		}

		vTargetPos = pTarget->GetWorldPosition();
	}
	else if( m_bAimedBall && pBall )
	{
		vTargetPos = pBall->GetWorldPosition();
	}


	D3DXVECTOR3 vDiff = vTargetPos - pOwner->GetWorldPosition();
	if( D3DXVec3LengthSq( &vDiff ) > m_fAimRange * m_fAimRange )
	{
		return false;
	}

	if( D3DXVec3LengthSq( &vDiff ) < m_fMinRange * m_fMinRange )
	{
		return false;
	}

	if( bAngleAlsoCheck )
	{
		vDiff.y = 0.0f;
		D3DXVec3Normalize( &vDiff, &vDiff );

		float fHalfCosine = cosf( DEGtoRAD( m_fAimAngle * FLOAT05 ) );
		if( D3DXVec3Dot( &m_vStartDir, &vDiff ) < fHalfCosine )
		{
			return false;
		}
	}

	return true;
}

void ioGenisysarmyItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioArrowWeapon *pArrow = ToArrowWeapon( pWeapon );
	ioMissileWeapon *pMissile = ToMissileWeapon( pWeapon );
	ioBombMissileWeapon *pBombMissile = ToBombMissileWeapon( pWeapon );
	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );

	D3DXVECTOR3 vAimedDir = m_vAimedDir;
	if( bJumpAttack )
	{
		if( m_bAimedState )
		{
			vAimedDir = ReCalcMoveDir( pWeapon->GetOwner(), pWeapon );
			pWeapon->SetMoveDir( vAimedDir );
		}

		if( pArrow || pMissile || pBombMissile )
		{
			SetFireEffect( pWeapon->GetOwner(), vAimedDir );
		}
	}
	else
	{
		if( m_bAimedState && m_FireState == FS_CHARGE_ATTACK )
		{
			vAimedDir = ReCalcMoveDir( pWeapon->GetOwner(), pWeapon );
			pWeapon->SetMoveDir( vAimedDir );
		}

		if( pArrow || pMissile || pBombMissile )
		{
			SetFireEffect( pWeapon->GetOwner(), vAimedDir );
		}
	}

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

void ioGenisysarmyItem::SetFireEffect( ioBaseChar *pOwner, const D3DXVECTOR3 &vDir )
{
	if( !pOwner ) return;

	if( m_bAimedState )
	{
		pOwner->CreateMapEffectWithDir( m_FireEffect, vDir );
	}
	else
	{
		D3DXVECTOR3 vNormalDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		pOwner->CreateMapEffectWithDir( m_FireEffect, vNormalDir );
	}
}

D3DXVECTOR3 ioGenisysarmyItem::ReCalcMoveDir( ioBaseChar *pOwner, ioWeapon *pWeapon )
{
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( !pTarget && !m_bAimedBall )
		return m_vAimedDir;

	ioBall *pBall = pOwner->FindBall();
	if( !pTarget && !pBall ) return m_vAimedDir;

	D3DXVECTOR3 vTargetPos;
	D3DXVECTOR3 vWeaponPos = pWeapon->GetPosition();

	if( pTarget )
		vTargetPos = pTarget->GetMidPositionByRate( FLOAT08);
	else if( m_bAimedBall && pBall )
		vTargetPos = pBall->GetMidPositionByRate();

	D3DXVECTOR3 vAimedDir =  vTargetPos - vWeaponPos;
	D3DXVec3Normalize( &vAimedDir, &vAimedDir );

	vAimedDir.x = m_vAimedDir.x;
	vAimedDir.z = m_vAimedDir.z;

	D3DXVec3Normalize( &vAimedDir, &vAimedDir );

	return vAimedDir;
}

void ioGenisysarmyItem::ApplyJComboChargingJumpAttack( ioBaseChar *pOwner, OUT DWORD& dwInput_Dash_S_StartTime, IN const ioHashString& szTarget, IN const bool& bAimedBall )
{
	if( !pOwner )	{	return;	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )	{	return;	}

	pOwner->StopAutoTargetTracking();

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ChangeDirectionExplicitIfDirChanged( true );
	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	m_vStartDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	m_AimedTarget = szTarget;
	m_bAimedBall = bAimedBall;

	m_bAimedState = false;
	m_iCurCombo = 0;
	m_fCurAngle = 0.0f;
	if( !m_bAimEffect )
	{
		pOwner->AttachEffect( m_AimEffect );
		m_bAimEffect = true;
	}

	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->ClearAllLoopAni( FLOAT10, true );

	pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );

	if( !m_AimedTarget.IsEmpty() || m_bAimedBall )
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
	else
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );

	SetChangeJumpFireMotion( pOwner, dwInput_Dash_S_StartTime );
}

void ioGenisysarmyItem::ProcessComboChargingJumpAttack( ioBaseChar *pOwner, DWORD& dwInput_Dash_S_StartTime )
{
	if ( !pOwner )	{	return;	}

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwMotionEndTime < dwCurTime )
	{
		if ( !pOwner->IsNeedProcess() )
			return;

		// 버튼 뗌 : 
		if( !pOwner->IsAttackKeyDown() )
		{
			SetNoneState( pOwner );
			return;
		}

		// 다음 발사
		if( IsCanFire( pOwner, 1 ) )
		{
			if( !CheckCurrentTargetValid( pOwner, true ) )
			{
				// 타겟 찾기
				ioHashString szTarget;

				m_AimedTarget.Clear();
				m_bAimedBall = false;

				if( pOwner->CheckAimedTargetByBow( m_fAimAngle, m_fMinRange, m_fAimRange, szTarget, m_bEnableDownAim ) )	// m_bEnableDownAim = false
				{
					m_AimedTarget = szTarget;
				}

				// 타겟 비교
				if( CompareAimedTarget( pOwner ) )
				{
					m_AimedTarget.Clear();
					m_bAimedBall = true;
				}

				pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );

				if( !m_AimedTarget.IsEmpty() && m_bAimedBall )
					pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
				else
					pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
			}

			m_iCurCombo++;
			ioGenisysarmyJump *pGenisysarmyJump = ToGenisysarmyJump( GetCurExtendJump() );
			if( !pGenisysarmyJump )	{	return;	}
			int iJumpMaxComboCnt = pGenisysarmyJump->GetMaxComboCnt();			
			if ( m_iCurCombo >= iJumpMaxComboCnt )
			{
				m_iCurCombo = 0;
			}

			SetChangeJumpFireMotion( pOwner, dwInput_Dash_S_StartTime );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << static_cast<BYTE>(ST_JUMP_ATTACK);
				kPacket << m_iCurCombo;
				kPacket << m_AimedTarget;
				kPacket << m_bAimedBall;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return;
		}
		else
		{
			SetNoneState( pOwner );
			return;
		}
	}
	else
	{
		int iMaxCnt = m_vAirJumpTimeList.size();

		if( m_iCurAirJump < iMaxCnt )
		{
			DWORD dwCurAirJumpTime = 0;
			std::list< float >::iterator iter = m_vAirJumpTimeList.begin();

			for( int i=0; iter != m_vAirJumpTimeList.end(); i++ )
			{
				if( m_iCurAirJump == i )
				{
					dwCurAirJumpTime = (DWORD)(*iter);
					break;
				}

				iter++;
			}

			if( dwCurAirJumpTime > 0 && dwCurAirJumpTime < FRAMEGETTIME() )
			{
				if( m_fAirJumpPower >= 0.0f )
				{
					D3DXQUATERNION qtTargetRot = pOwner->GetTargetRot();
					pOwner->SetAirJumpPower( m_fAirJumpPower*m_fAirJumpPowerRate );
					D3DXVECTOR3 vAttDir =  qtTargetRot * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
					if( m_fAirForcePower < 0.0f )
						pOwner->SetForcePower( -vAttDir, fabs(m_fAirForcePower), FLOAT1, true );
					else
						pOwner->SetForcePower( vAttDir, fabs(m_fAirForcePower), FLOAT1, true );
				}
				m_iCurAirJump++;
			}
		}
	}

	UpdateAimDir( pOwner );
}

void ioGenisysarmyItem::UpdateAimDir( ioBaseChar *pOwner )
{
	if ( !pOwner )	{	return;	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )	{	return;	}

	//ioBiped *pBiped = pGrp->GetBiped("Bip01 Spine1");
	//if(!pBiped)	{	return;	}

	DWORD dwCurTime = FRAMEGETTIME();
	bool bStateCheck = false;
	if ( ( pOwner->GetState() == CS_ATTACK && m_FireState == FS_CHARGE_ATTACK ) || pOwner->GetState() == CS_JUMP )
	{
		bStateCheck = true;
	}

	if ( !bStateCheck )
	{
		return;
	}

	ioBall *pBall = pOwner->FindBall();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( !bStateCheck || (!pTarget && !m_bAimedBall) || !CheckCurrentTargetValid( pOwner, true ) )
	{
		m_fCurAngle = 0.0f;
		//pBiped->ReleaseExtraRotation();
		return;
	}

	D3DXVECTOR3 vTargetPos;
	if( pTarget )
	{
		vTargetPos = pTarget->GetWorldPosition();
	}
	else if( m_bAimedBall && pBall )
	{
		vTargetPos = pBall->GetWorldPosition();
	}

	float fRotAngle = 0.0f;

	D3DXVECTOR3 vSightDir = vTargetPos - pOwner->GetWorldPosition();
	D3DXVec3Normalize( &vSightDir, &vSightDir );

	D3DXVECTOR3 vXZDir( vSightDir.x, 0.0f, vSightDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vXZDir );

	pOwner->SetTargetRotAndMoveDirChange( qtTargetRot, true );
	pOwner->SetWorldOrientation( qtTargetRot );
}

void ioGenisysarmyItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );

	pOwner->SetTargetMarker( ioTargetMarker::MT_BOW_ITEM,
		m_szReadyMarker, m_szAimingMarker, m_szAimedMarker, m_szZeroAimedMarker, "" );

	pOwner->SetTargetMarkerRange( m_fMarkerRange );
}

void ioGenisysarmyItem::OnReleased( ioBaseChar *pOwner )
{
	pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "", "" );
	ioWeaponItem::OnReleased( pOwner );
}

// 특별 공격 D~
void ioGenisysarmyItem::ChangeToChargeAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_dwAttackStartTime	= 0;
	m_dwMotionEndTime	= 0;
	m_bAimedState		= false;
	m_iCurCombo			= 0;
	
	m_FireState = FS_CHARGE_ATTACK;
	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->StopAutoTargetTracking();
	pOwner->SetAttackMoveEnable( true );
	pOwner->ChangeDirectionExplicitIfDirChanged( true );
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	
	ClearCancelInfo();
	m_bFirstAimMoveMotion = true;
	m_bAimMotionSetted = true;

	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	m_vStartDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	m_bAimedState = false;
	if( !m_bAimEffect )
	{
		pOwner->AttachEffect( m_AimEffect );
		m_bAimEffect = true;
	}

	pGrp->ClearAllLoopAni( FLOAT10, true );
	if ( pOwner->IsNeedProcess() )
	{
		m_AimStartDirKey = pOwner->GetCurDirKey();
		m_AimedTarget.Clear();
		m_bAimedBall = false;
		// 타겟 찾기
		ioHashString szTarget;
		if( pOwner->CheckAimedTargetByBow( m_fAimAngle, m_fMinRange, m_fAimRange, szTarget, m_bEnableDownAim ) )	// m_bEnableDownAim = false
		{
			m_AimedTarget = szTarget;
		}
		// 타겟 비교
		if( CompareAimedTarget( pOwner ) )
		{
			m_AimedTarget.Clear();
			m_bAimedBall = true;
		}
	}
	
	pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );

	if( !m_AimedTarget.IsEmpty() || m_bAimedBall )
	{
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
	}
	else
	{
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
	}

	SetChangeFireMotion( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( ST_CHARGE_ATTACK );
		kPacket << pOwner->GetTargetRot();
		kPacket << (int)m_AimStartDirKey;
		kPacket << m_AimedTarget;
		kPacket << m_bAimedBall;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioGenisysarmyItem::SetChangeFireMotion( ioBaseChar *pOwner )
{
	if( !COMPARE( m_iCurCombo, 0, m_iAttackExtendAttributeCnt) )
	{
		return false;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	// FireDir 처리
	// 발사 모션 처리
	int	  iAniID = -1;
	float fTimeRate = FLOAT1;
	float fWeight = 0.0f;

	pGrp->ClearAllActionAni( 0.0f, true );

	iAniID = pGrp->GetAnimationIdx( m_AttackExtendAttribute.m_AttackAnimation );
	if( iAniID == -1 )
	{
		return false;
	}
	fTimeRate = m_AttackExtendAttribute.m_fAttackAniRate;
	pOwner->RefreshFireTimeList( iAniID, m_AttackExtendAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
	pOwner->IncreaseWeaponIndexBase();


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
			AimedFireToUp( pGrp, fWeight, fTimeRate, 0, false, true );
		}
		else
		{
			AimedFireToDown( pGrp, fWeight, fTimeRate, 0, false, true );
		}

		m_bAimedState = true;
	}
	else if( m_bAimedBall && pBall )
	{
		D3DXVECTOR3 vAimedDir = pBall->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		D3DXVec3Normalize( &vAimedDir, &vAimedDir );

		fWeight = GetAimedUpDownWeight( vAimedDir, 85.0f );
		m_vAimedDir = vAimedDir;

		if( m_vAimedDir.y >= 0.0f )
		{
			AimedFireToUp( pGrp, fWeight, fTimeRate, 0, false, true );
		}
		else
		{
			AimedFireToDown( pGrp, fWeight, fTimeRate, 0, false, true );
		}

		m_bAimedState = true;
	}
	else
	{
		pGrp->SetActionAni( m_FireCenter, 0.0f, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0, 0, true );
		m_bAimedState = false;
	}


	// 시간값들 갱신
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwRotateStartTime = dwCurTime;

	m_dwRotateEndTime = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if ( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwCurTime;

	return true;
}

void ioGenisysarmyItem::ProcessChargeAttack( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
	{
		return;
	}

	// 시간체크...
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwMotionEndTime < dwCurTime )
	{
		// 버튼 뗌 : 종료동작으로...
		if( !pOwner->IsAttackKeyDown() )
		{
			pOwner->NormalAttackOver();
			SetNoneState( pOwner );
			return;
		}

		// 다음 발사
		if( IsCanFire( pOwner, 1 ) )
		{
			// 타겟 체크
			if( !CheckCurrentTargetValid( pOwner, true ) )
			{
				// 타겟 찾기
				ioHashString szTarget;

				m_AimedTarget.Clear();
				m_bAimedBall = false;

				if( pOwner->CheckAimedTargetByBow( m_fAimAngle, m_fMinRange, m_fAimRange, szTarget, m_bEnableDownAim ) )	// m_bEnableDownAim = false
				{
					m_AimedTarget = szTarget;
				}

				// 타겟 비교
				if( CompareAimedTarget( pOwner ) )
				{
					m_AimedTarget.Clear();
					m_bAimedBall = true;
				}

				pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );

				if( !m_AimedTarget.IsEmpty() || m_bAimedBall )
					pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
				else
					pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
			}

			m_iCurCombo++;
			if ( m_iCurCombo >= m_iAttackExtendAttributeCnt )
			{
				m_iCurCombo = 0;
			}
			SetChangeFireMotion( pOwner );

			
			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << static_cast<BYTE>( ST_CHARGE_ATTACK_COMBO );
				kPacket << m_AimedTarget;
				kPacket << m_bAimedBall;
				kPacket << m_iCurCombo;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			
			return;
		}
		else
		{
			pOwner->NormalAttackOver();
			return;
		}
	}
}

void ioGenisysarmyItem::CheckAimMoveAniState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsCanAttackMove() )
	{
		return;
	}

	if( !COMPARE( m_iCurCombo, 0, m_iAttackExtendAttributeCnt ) )
	{
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	if( pOwner->IsSettedDirection() )
	{
		if( pOwner->IsChangeDirection() || m_bFirstAimMoveMotion )
		{
			pGrp->ClearAllLoopAni( FLOAT10 );

			m_bAimMotionSetted = false;
			m_bFirstAimMoveMotion = false;

			ioUserKeyInput::DirKeyInput eXformDir;
			eXformDir = pOwner->GetXformDir( m_AimStartDirKey );

			switch( eXformDir )
			{
			case ioUserKeyInput::DKI_UP:
				pGrp->SetLoopAni( m_AimMoveFront, FLOAT100, FLOAT1, m_fAimMoveAniRate );
				break;
			case ioUserKeyInput::DKI_RIGHTUP:
			case ioUserKeyInput::DKI_RIGHT:
			case ioUserKeyInput::DKI_RIGHTDOWN:
				pGrp->SetLoopAni( m_AimMoveRight, FLOAT100, FLOAT1, m_fAimMoveAniRate );
				break;
			case ioUserKeyInput::DKI_DOWN:
				pGrp->SetLoopAni( m_AimMoveBack, FLOAT100, FLOAT1, m_fAimMoveAniRate );
				break;
			case ioUserKeyInput::DKI_LEFTDOWN:
			case ioUserKeyInput::DKI_LEFT:
			case ioUserKeyInput::DKI_LEFTUP:
				pGrp->SetLoopAni( m_AimMoveLeft, FLOAT100, FLOAT1, m_fAimMoveAniRate );
				break;
			}
			g_TutorialMgr.ActionAimingMove();
		}
	}
	else
	{
		if( !m_bAimMotionSetted )
		{
			pGrp->ClearAllLoopAni( FLOAT10 );

			m_bAimMotionSetted = true;
		}
	}
}

// 특별 공격 A~ (기본이 점프 길게 누르면 A~)
void ioGenisysarmyItem::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	// true 일 경우 A~
	if( bFullTime )
	{
		SetDummyReadyState( pOwner );
		return;
	}

	ioAttackableItem::SetJumpping( pOwner, bFullTime );
}

void ioGenisysarmyItem::SetDummyReadyState( ioBaseChar* pOwner )
{
	if( !pOwner )	{	return;	}

	if( !pOwner->IsNeedProcess() )
		return;

	pOwner->SetState( CS_GENISYSARMY_SPECIAL );
	m_SpecialState	= SS_DUMMY_READY;

	if( pOwner->IsNeedProcess() && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)SS_DUMMY_READY;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGenisysarmyItem::SetDummyChargeState( ioBaseChar* pOwner )
{
	if( !pOwner )	{	return;	}

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )	{	return;	}

	pOwner->SetState( CS_GENISYSARMY_SPECIAL );
	m_SpecialState		= SS_DUMMY_MAKE;

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_DummyChargeAttribute.m_AttackAnimation );
	float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

}

void ioGenisysarmyItem::ChangeToDummyMake( ioPlayStage* pStage, ioBaseChar *pOwner, int iCurIndex )
{
	ioHashString	szName;
	ioDummyChar*	pCurDummy = NULL;

	ioDummyGroup* pDummyGroup = g_DummyCharMgr.GetDummyGroup( pOwner->GetCharName(), GetName() );
	if( !pDummyGroup )
	{
		pDummyGroup = g_DummyCharMgr.AddDummyGroup( pOwner->GetCharName(), GetName() );
	}

	if( !pDummyGroup )	{	return;	}
	
	if( pOwner->IsNeedProcess() )
	{
		szName.Clear();
		if( 0 < (int)m_DummyInfoList.size() )
		{

			szName = m_DummyInfoList[0].szDummyName;
		}

		if( szName.IsEmpty() )	{	return;	}

		//더미 생성
		ioDummyChar* pNewDummy = CreateDummyChar( szName, pStage, pOwner, iCurIndex );
		if( !pNewDummy )	{	return;	}

		ioDummyGradeTurret* pNewTurret = ToDummyGradeTurret( pNewDummy );
		if( pNewTurret )
		{
			pNewTurret->SetAttackDir( pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 ) );
			pOwner->SetTargetRotToTargetPos( pNewDummy->GetWorldPosition(), true, true );
			//새로 생성한 더미를 더미그룹에 추가
			pDummyGroup->AddDummy( pNewDummy->GetDummyCharIdx() );
		}

		// 게이지 체크, 맥스값보다 크다면 생성한지 가장 오래된 더미 제거
		ioDummyChar* pDieDummy = NULL;
		if( GetTotalDummyGauge( pOwner ) > 5.0f )
		{
			pDieDummy = pDummyGroup->FirstDummyDie( pNewDummy );
		}

		// 설치 개수 지정
		int iCount = pDummyGroup->GetDummyCount( szName );
		if( iCount > m_iDummyMaxCnt )
		{
			pDieDummy = pDummyGroup->FirstDummyDie( pNewDummy );
		}

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << static_cast<BYTE>( SS_DUMMY_MAKE_CREATE );
			kPacket << pOwner->GetTargetRot();
			kPacket << pOwner->GetWorldPosition();

			if( pNewDummy )
			{
				kPacket << szName;
				kPacket << pNewDummy->GetDummyCharIdx();
			}
			else
			{
				kPacket << 0;
				kPacket << "";
			}

			if( pDieDummy )
			{
				kPacket << pDieDummy->GetDummyCharIdx();
			}
			else
			{
				kPacket << 0;
			}

			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	pOwner->SetState( CS_GENISYSARMY_SPECIAL );
	m_SpecialState		= SS_DUMMY_MAKE_CREATE;
	DWORD dwKeyReserve	= 0;
	DWORD dwPreDelay	= 0;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllActionAni( FLOAT100 );
		pGrp->ClearAllLoopAni( FLOAT100 );

		int iAniID = pGrp->GetAnimationIdx( m_DummyMakeAnimation );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / m_fDummyMakeAnimationRate, 0.0f, dwPreDelay, true );
		m_dwDummyMakeEndTime = FRAMEGETTIME() + pGrp->GetAnimationFullTime(iAniID) * m_fDummyMakeAnimationRate;
		DWORD dwReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * m_fDummyMakeAnimationRate + dwPreDelay;
		if( 0 < dwReserve )
		{
			dwKeyReserve = FRAMEGETTIME() + dwReserve;
		}
	}
}

// 특별 공격 A~ ( 더미 생성 )
ioDummyChar* ioGenisysarmyItem::CreateDummyChar( const ioHashString& szName, ioPlayStage *pStage, ioBaseChar *pOwner, int iCurIndex )
{	
	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + vDir * m_fDummySetupOffset;
	float fHeight = pStage->GetMapHeight( vPos.x, vPos.z );
	if( 0.0f < fHeight )
	{
		vPos.y = fHeight;
	}

	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, iCurIndex );
	if( pDummy )
	{
		pDummy->SetDieState( false );
	}

	return pStage->CreateDummyChar( szName, iCurIndex, pOwner->GetCharName(), vPos, 0.0f, 0, true );
}

float ioGenisysarmyItem::GetTotalDummyGauge( ioBaseChar* pOwner )
{
	ioDummyGroup* pDummyGroup = g_DummyCharMgr.GetDummyGroup( pOwner->GetCharName(), GetName() );
	if( !pDummyGroup )
	{
		return 0.0f;
	}

	DummyCharList List;
	pDummyGroup->GetDummyAll( List );

	float fTotalGauge = 0.0f;
	for( DummyCharList::iterator iter = List.begin(); iter != List.end(); ++iter )
	{
		ioDummyChar* pDummy = *iter;
		if( pDummy )
		{
			fTotalGauge += GetDummyNeedGauge( pDummy->GetDummyCharName() );
		}
	}

	return fTotalGauge;
}

const GroupDummyInfo* const ioGenisysarmyItem::GetGroupDummyInfo( const ioHashString& szDummyName )
{
	for( int i = 0; i < (int)m_DummyInfoList.size(); ++i )
	{
		if( m_DummyInfoList[i].szDummyName == szDummyName )
		{	
			return &m_DummyInfoList[i];
		}
	}

	return NULL;
}

float ioGenisysarmyItem::GetDummyNeedGauge( const ioHashString& szDummyName )
{
	const GroupDummyInfo* const pInfo = GetGroupDummyInfo( szDummyName );
	if( pInfo )
	{
		return pInfo->fNeedGauge;
	}

	return 0.0f;
}

void ioGenisysarmyItem::OnDummyMake( ioBaseChar *pOwner )
{
	if( m_dwDummyMakeEndTime < FRAMEGETTIME() )
	{
		m_SpecialState = SS_NONE;
		pOwner->SetState( CS_DELAY );
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioGenisysarmySpecialState::ioGenisysarmySpecialState()
{
}

ioGenisysarmySpecialState::~ioGenisysarmySpecialState()
{
}

void ioGenisysarmySpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

// ioBaseChar::SetState 함수에서 상태 체크, 이전 특별 상태 삭제할때 호출
void ioGenisysarmySpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioGenisysarmyItem *pGenisysarmyItem = ToGenisysarmyItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pGenisysarmyItem )
	{
		pGenisysarmyItem->ClearAttackState( pOwner );
	}
}

// ioBaseChar::ProcessState() 함수에서 디폴트로 함수 계속 호출한다.
void ioGenisysarmySpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioGenisysarmyItem *pGenisysarmyItem = ToGenisysarmyItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pGenisysarmyItem )
	{
		pGenisysarmyItem->ProcessSpecialState( pOwner );
	}
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

// 특별 상태가 지속되는지 검사해서 아니면 false로 리턴하여 ioBaseChar::ProcessMove()를 실행
bool ioGenisysarmySpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ioGenisysarmyItem *pGenisysarmyItem = ToGenisysarmyItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pGenisysarmyItem )
	{
		return false;
	}

	// 일반 공격 중 캔슬 대시
	if( !pGenisysarmyItem->IsConditionalSpecialDashReserve( pOwner ) )
	{
		return false;
	}

	return true;
}