#include "stdafx.h"
#include "ItemDefine.h"
#include "ioSizeComboJump.h"

ioSizeComboJump::ioSizeComboJump()
{
	m_SizeComboJumpState		= SCJS_NONE;
	m_iCurComboCnt				= 0;
	m_dwEnableComboTime_S		= 0;
	m_dwEnableComboTime_E		= 0;
	m_dwChargingStartTime		= 0;
	m_bReserveJumpAttack		= false;

	// D~
	m_bScale					= false;
	m_vBeforeScale				= D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
	m_bExtendChargingAttack		= false;
	m_bJumpAttackReserve		= false;

	// D~ 크기가 작아지며 전방 위치로 이동하면서 공격
	m_vMoveDir					= ioMath::VEC3_ZERO;
	m_fMoveAmt					= 0.0f;
	m_fSlideSpeed				= 0.0f;
	m_fTargetRange				= 0.0f;
	m_bAimedBall				= false;
	m_AimedTarget.Clear();

	// D~ 이어서 +--> 대시
	m_bUsedDashJumpAttack		= false;
	m_dwJumpDashStartTime		= 0.0f;
	m_dwJumpDashEndTime			= 0.0f;


	// D~ 이어서 +D~ 이나 +D
	m_dwExtendChargingStartTime	= 0.0f;

	m_dwActionEndTime			= 0;

	// 이펙트
	m_dwScaleEffectID			= -1;
	m_dwExtendScaleEffectID		= -1;
}

ioSizeComboJump::ioSizeComboJump( const ioSizeComboJump &rhs )
	: ioExtendJump( rhs ),
	m_dwChargingTime( rhs.m_dwChargingTime ),
	m_ChargingAni( rhs.m_ChargingAni ),
	m_AttackAttributeList( rhs.m_AttackAttributeList ),
	m_iMaxComboCnt( rhs.m_iMaxComboCnt),
	m_fScaleValue( rhs.m_fScaleValue),							// D~
	m_fExtendScaleValue( rhs.m_fExtendScaleValue),
	m_ExtendAttackList( rhs.m_ExtendAttackList),
	m_ExtendChargeAttackList( rhs.m_ExtendChargeAttackList),	
	m_fChargeSlideSpeed( rhs.m_fChargeSlideSpeed),				// D~ 크기가 작아지며 전방 위치로 이동하면서 공격
	m_TargetWoundType( rhs.m_TargetWoundType),
	m_fJumpSlideMoveRange( rhs.m_fJumpSlideMoveRange),
	m_fJumpSlideTaregtCheckRange( rhs.m_fJumpSlideTaregtCheckRange),
	m_fJumpSlideAngle( rhs.m_fJumpSlideAngle),
	m_fSlideEndJumpAmt( rhs.m_fSlideEndJumpAmt),				// D~, +-->, +D 중 공중일 경우 상태 종료 시 점핑
	m_fSlideEndJumpEnableHeight( rhs.m_fSlideEndJumpEnableHeight),
	m_dwExtendChargingTime( rhs.m_dwExtendChargingTime ),		// D~ 이어서 +D~ 이나 +D
	m_szScaleEffectName( rhs.m_szScaleEffectName ),				// 이펙트
	m_szExtendScaleEffectName( rhs.m_szExtendScaleEffectName ),
	m_szScaleEffectCreateSound( rhs.m_szScaleEffectCreateSound ),
	m_fComboDashGravityAmt(rhs.m_fComboDashGravityAmt),
	m_SpeedDownBuff(rhs.m_SpeedDownBuff)	// 이동 속도 제한 2020-10-29
{
	m_SizeComboJumpState		= SCJS_NONE;
	m_iCurComboCnt				= 0;
	m_dwEnableComboTime_S		= 0;
	m_dwEnableComboTime_E		= 0;
	m_dwChargingStartTime		= 0;
	m_bReserveJumpAttack		= false;

	// D~
	m_bScale					= false;
	m_vBeforeScale				= D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
	m_bExtendChargingAttack		= false;
	m_bJumpAttackReserve		= false;

	// D~ 크기가 작아지며 전방 위치로 이동하면서 공격
	m_vMoveDir					= ioMath::VEC3_ZERO;
	m_fMoveAmt					= 0.0f;
	m_fSlideSpeed				= 0.0f;
	m_fTargetRange				= 0.0f;
	m_bAimedBall				= false;
	m_AimedTarget.Clear();

	// D~ 이어서 +--> 대시
	m_bUsedDashJumpAttack		= false;
	m_dwJumpDashStartTime		= 0.0f;
	m_dwJumpDashEndTime			= 0.0f;

	// D~ 이어서 +D~ 이나 +D
	m_dwExtendChargingStartTime	= 0.0f;

	m_dwActionEndTime			= 0;

	// 이펙트
	m_dwScaleEffectID			= -1;
	m_dwExtendScaleEffectID		= -1;
}

ioExtendJump* ioSizeComboJump::Clone()
{
	return new ioSizeComboJump( *this );
}

ioSizeComboJump::~ioSizeComboJump()
{

}

JumpType ioSizeComboJump::GetType() const
{
	return JT_SIZECOMBO_JUMP;
}

void ioSizeComboJump::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendJump::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	// 기본 점프 차징
	m_dwChargingTime = (DWORD)rkLoader.LoadInt_e( "charging_time", 0 );

	// 기본 점프 차징 준비 애니
	m_ChargingAni.Init();
	LoadAttackAttribute( "charging_jump_ani", m_ChargingAni, rkLoader );

	// 기본 점프 공격
	m_iMaxComboCnt = rkLoader.LoadInt_e( "jump_max_combo", 0 );
	for( int i = 0; i < m_iMaxComboCnt; ++i )
	{
		AttackAttribute kAttribute;

		wsprintf_e( szBuf, "jump_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_AttackAttributeList.push_back( kAttribute );
	}

	// D~
	m_fScaleValue = rkLoader.LoadFloat_e( "extend_jump_scale_value", 0.0f );
	m_fExtendScaleValue = rkLoader.LoadFloat_e( "jump_extend_jump_scale_value", 0.0f );
	

	// Extend combo 
	// D~ : 크기가 작아지며 전방 위치로 이동하며서 주먹을 휘두름 
	// --> : 주먹을 휘두르면서 전방으로 이동
	// +D : 주먹을 올려쳐서 상대를 띄움 
	int iMax = rkLoader.LoadInt_e( "extend_jump_max_cnt", 0 );
	for( int i=0; i < iMax; ++i )
	{
		AttackAttribute eAttribute;

		wsprintf_e( szKey, "extend_jump_attack%d", i+1);
		LoadAttackAttribute( szKey, eAttribute, rkLoader );

		m_ExtendAttackList.push_back( eAttribute );
	}

	// D~ 크기가 작아지며 전방 위치로 이동하면서 공격
	m_fChargeSlideSpeed = rkLoader.LoadFloat_e( "jump_extend_charging_slide_speed", 0 );
	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );
	m_fJumpSlideMoveRange = rkLoader.LoadFloat_e( "jump_extend_jump_slide_move_range", 0.0f );
	m_fJumpSlideTaregtCheckRange = rkLoader.LoadFloat_e( "jump_extend_jump_slide_traget_check_range", 0.0f );
	m_fJumpSlideAngle = rkLoader.LoadFloat_e( "jump_extend_jump_slide_angle", 0.0f );

	// D~, +-->, +D 중 공중일 경우 상태 종료 시 점핑
	m_fSlideEndJumpEnableHeight = rkLoader.LoadFloat_e( "jump_extend_slide_end_jump_enable_height", 0.0f );
	m_fSlideEndJumpAmt = rkLoader.LoadFloat_e( "jump_extend_slide_end_jump_amt", 0.0f );

	// Extend Charge Combo D~ 이어서 +D~ 이나 +D
	m_dwExtendChargingTime = rkLoader.LoadInt_e( "jump_extend_charge_time", 0 );

	// +D~ : 크기가 커지면서 상대를 내려침
	// +--> : 하강하면서 양팔로 내려침
	// +D : 공중에서 발로 참
	iMax = rkLoader.LoadInt_e( "jump_extend_charge_max_cnt", 0 );
	for( int i=0; i < iMax; ++i )
	{
		AttackAttribute eAttribute;
		wsprintf_e( szKey, "jump_extend_charge_attack%d", i+1);
		LoadAttackAttribute( szKey, eAttribute, rkLoader );
		m_ExtendChargeAttackList.push_back( eAttribute );
	}

	// 이펙트
	rkLoader.LoadString_e( "scale_effect_name", "", szBuf, MAX_PATH );	// 커질때
	m_szScaleEffectName = szBuf;
	rkLoader.LoadString_e( "extend_scale_effect_name", "", szBuf, MAX_PATH );	// 작아질때
	m_szExtendScaleEffectName = szBuf;

	rkLoader.LoadString_e( "scale_effect_create_sound", "", szBuf, MAX_PATH );
	m_szScaleEffectCreateSound = szBuf;

	// D~ 에서 +D~ 이어서 +--> 할때 중력값 적용
	m_fComboDashGravityAmt	= rkLoader.LoadFloat_e( "jump_dash_gravity_amt", 0.0f );

	// 이동 속도 제한 2020-10-29
	rkLoader.LoadString( "speed_down_buff", "", szBuf, MAX_PATH );
	m_SpeedDownBuff = szBuf;
}

void ioSizeComboJump::InitJump()
{

}

void ioSizeComboJump::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	pOwner->SetUsedFlashJump( false );

	m_SizeComboJumpState		= SCJS_NONE;
	m_iCurComboCnt				= 0;
	m_bExtendChargingAttack		= false;

	m_dwEnableComboTime_S		= 0;
	m_dwEnableComboTime_E		= 0;
	m_dwChargingStartTime		= 0;
	m_bReserveJumpAttack		= false;
	m_bUsedDashJumpAttack		= false;
	m_dwExtendChargingStartTime	= 0.0f;

	m_bJumpAttackReserve		= false;

	// D~ 이어서 +--> 대시
	m_bUsedDashJumpAttack		= false;
	m_dwJumpDashStartTime		= 0.0f;
	m_dwJumpDashEndTime			= 0.0f;

	// D~ 크기가 작아지며 전방 위치로 이동하면서 공격
	m_vMoveDir					= ioMath::VEC3_ZERO;
	m_fMoveAmt					= 0.0f;
	m_fSlideSpeed				= 0.0f;
	m_fTargetRange				= 0.0f;
	m_bAimedBall				= false;
	m_AimedTarget.Clear();

	// 크기를 원래 상태로 2020-10-19
	DWORD dwCurTime = FRAMEGETTIME();
	if( !pOwner->HasBuff( BT_KING_SCALE ) || m_dwActionEndTime > 0 && m_dwActionEndTime < dwCurTime )
	{
		m_bScale					= false;
		m_dwActionEndTime			= 0;
		pOwner->SetWorldScale( m_vBeforeScale );

		// 이동 속도 제한 2020-10-29
		if( !m_SpeedDownBuff.IsEmpty() )
		{
			pOwner->RemoveBuff( m_SpeedDownBuff );
		}
	}

	pOwner->EndEffect( m_dwScaleEffectID );
	pOwner->EndEffect( m_dwExtendScaleEffectID );
	m_dwScaleEffectID			= -1;
	m_dwExtendScaleEffectID		= -1;
}

void ioSizeComboJump::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge )
{
	if( !pOwner )
	{
		return;
	}
	
	DWORD dwCurTime = FRAMEGETTIME();
	
	// 기본 공격 
	if( m_bReserveJumpAttack )
	{		
		SetChargingState( pOwner );
		return;
	}
}

void ioSizeComboJump::ProcessLanding( ioBaseChar *pOwner )
{	
	m_bUseExtendJump		= false;
	m_bReserveJumpAttack	= false;
	m_SizeComboJumpState	= SCJS_NONE;
	
	// 원래 크기대로 변경 2020-10-19
	DWORD dwCurTime = FRAMEGETTIME();
	if( !pOwner->HasBuff( BT_KING_SCALE ) || m_dwActionEndTime > 0 && m_dwActionEndTime < dwCurTime )
	{
		m_bScale					= false;
		m_dwActionEndTime			= 0;
		pOwner->SetWorldScale( m_vBeforeScale );

		// 이동 속도 제한 2020-10-29
		if( !m_SpeedDownBuff.IsEmpty() )
		{
			pOwner->RemoveBuff( m_SpeedDownBuff );
		}
	}
	pOwner->EndEffect( m_dwScaleEffectID );
	pOwner->EndEffect( m_dwExtendScaleEffectID );
	m_dwScaleEffectID			= -1;
	m_dwExtendScaleEffectID		= -1;
}

void ioSizeComboJump::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner ) return;

	switch( m_SizeComboJumpState )
	{
		case SCJS_NONE:
		{			
			break;
		}
		case SCJS_CHARGE:
		{
			ProcessCharging( pOwner );
			break;
		}
		case SCJS_NORMAL_ATTACK:	// 기본공격 
		{	
			ProcessComboJumpAttack( pOwner );
			break;
		}
		case SCJS_CHARGING_ATTACK:
		{
			ProcessComboChargingJumpAttack( pOwner );
			break;
		}
		case SCJS_DASH:
		{
			ProcessJumpDash( pOwner );
			break;
		}

		case SCJS_EXTEND_CHARGE:
		{
			ProcessExtendCharging( pOwner );
			break;
		}
	}

	// 이펙트
	ProcessScaleEffect( pOwner );
}

void ioSizeComboJump::ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )	{	return;	}
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )	{	return;	}

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
		case SCJS_CHARGE:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetChargingState( pOwner );
			break;
		}
		case SCJS_EXTEND_CHARGE:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			ProcessExtendCharging( pOwner );
			break;
		}

		
		case SCJS_NORMAL_ATTACK:
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
		case SCJS_CHARGING_ATTACK:
		{
			rkPacket >> m_iCurComboCnt;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			D3DXVECTOR3 vMoveDir, vInputDir;
			rkPacket >> vMoveDir >> vInputDir;

			pOwner->SetJumpAttckTargetRot( qtRot, vMoveDir, vInputDir );

			rkPacket >> m_bExtendChargingAttack;
			rkPacket >> m_fSlideSpeed;
			rkPacket >> m_bScale;
			SetExtendComboChargingJumpAttack( pOwner, m_bExtendChargingAttack );
			break;
		}

		case SCJS_CHARGING_END:	// D~, +-->, +D 중 공중일 경우 상태 종료 시 점핑
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );
			SetEndState( pOwner );
			break;
		}

		case SCJS_CHARGING_TARGET:
		{
			rkPacket >> m_AimedTarget;
			break;
		}
		case SCJS_DASH:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			D3DXVECTOR3 vMoveDir, vInputDir;
			rkPacket >> vMoveDir >> vInputDir;
			rkPacket >> m_bExtendChargingAttack;

			pOwner->SetJumpAttckTargetRot( qtRot, vMoveDir, vInputDir );
			SetJumpDash( pOwner, m_bExtendChargingAttack );
			break;
		}
	}
}

void ioSizeComboJump::OnEndExtendJump( ioBaseChar *pOwner )
{
}

bool ioSizeComboJump::IsEnableJumpDash( ioBaseChar *pOwner )
{
	switch( m_SizeComboJumpState )
	{
		case SCJS_CHARGING_ATTACK:
		{
			return true;
		}

		default:
		{
			return false;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 기본 함수 정의
void ioSizeComboJump::SetChargingState( ioBaseChar *pOwner )
{
	if( !pOwner ) {	return;	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) {	return;	}

	pOwner->ClearAttackFireTimeAndSkill();

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_ChargingAni.m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioSizeComboJump::SetChargingState - %s Animation is Not Exist",
								m_ChargingAni.m_AttackAnimation.c_str() );
		return;
	}

	float fTimeRate = (float)m_dwChargingTime / pGrp->GetAnimationFullTime( iAniID );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_bUseExtendJump		= true;
	m_SizeComboJumpState	= SCJS_CHARGE;
	m_dwChargingStartTime	= dwCurTime;

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );

	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	pOwner->SetJumpAttackState();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_SizeComboJumpState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSizeComboJump::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
	{
		return;
	}	

	DWORD dwCurTime = FRAMEGETTIME();
	if ( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown()  )
	{
		if( m_dwChargingStartTime > 0 && m_dwChargingStartTime + m_dwChargingTime < dwCurTime )
		{	
			// 게이지 검사
			if( CheckEnableExtendComboState( pOwner ) )
			{
				// D~ 크기가 작아지며 전방으로 주먹을 휘두름
				m_AimedTarget.Clear();
				m_iCurComboCnt	= 0;
				m_fMoveAmt		= 0.0f;
				m_fSlideSpeed	= m_fChargeSlideSpeed;
				SetExtendComboChargingJumpAttack( pOwner );				
			}
			else
			{				
				// 기본 공격
				SetComboJumpAttack( pOwner );
			}
			return;
		}
	}
	else
	{
		DWORD dwLooseTime = pOwner->GetActionStopTotalDelay();
		if( m_bReserveJumpAttack )
		{
			if( dwCurTime >= m_dwEnableComboTime_S+dwLooseTime )
			{
				SetComboJumpAttack( pOwner );
			}
		}
	}
}

void ioSizeComboJump::ProcessComboJumpAttack( ioBaseChar *pOwner )
{
	if( !pOwner )	{	return;	}

	CheckAirJump( pOwner );

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLooseTime = 0;

	if( pOwner )
		dwLooseTime = pOwner->GetActionStopTotalDelay();

	if( dwCurTime >= m_dwEnableComboTime_S+dwLooseTime )
	{
		if ( m_bReserveJumpAttack )
		{	
			SetChargingState( pOwner );
			return;
		}
	}
}

void ioSizeComboJump::CheckComboJumpAttack( ioBaseChar *pOwner )
{	
	int iMaxCnt = m_AttackAttributeList.size();
	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
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

void ioSizeComboJump::CheckExtendComboAttack( ioBaseChar *pOwner )
{
	if( !pOwner )	{	return;	}
	
	int iMaxCnt = 0;
	if( !m_bExtendChargingAttack )
	{
		// extend combo D~, +-->, +D
		iMaxCnt = m_ExtendAttackList.size();
	}
	else
	{
		// extend Charge combo +D~, +-->, +D
		iMaxCnt = m_ExtendChargeAttackList.size();
	}
	
	if( !COMPARE( m_iCurComboCnt-1, 0, iMaxCnt ) )
	{
		return;
	}

	if( m_bReserveJumpAttack )
	{
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLooseTime = 0;

	if( pOwner )
		dwLooseTime = pOwner->GetActionStopTotalDelay();

	if( m_iCurComboCnt > 0 && !COMPARE( dwCurTime, m_dwEnableComboTime_S+dwLooseTime, m_dwEnableComboTime_E+dwLooseTime ) )
	{		
		return;
	}

	if( m_iCurComboCnt > 0 && pOwner->IsHasCrown() )
	{
		return;
	}

	m_bReserveJumpAttack = true;
}

void ioSizeComboJump::SetComboJumpAttack( ioBaseChar *pOwner )
{
	if( !pOwner )	{	return;	}

	if( !COMPARE( m_iCurComboCnt, 0, m_iMaxComboCnt ) )
	{
		return;
	}

	DWORD dwCurTime			= FRAMEGETTIME();
	m_SizeComboJumpState	= SCJS_NORMAL_ATTACK;

	pOwner->ClearAttackFireTimeAndSkill();

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
		LOG.PrintTimeAndLog( 0, "ioSizeComboJump::SetComboJumpAttack - %s Animation is Not Exist",
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
		kPacket << m_SizeComboJumpState;
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


// 점프 D~
void ioSizeComboJump::SetExtendComboChargingJumpAttack( ioBaseChar *pOwner, bool bExtendChargeCombo )
{
	if( !pOwner )	{	return;	}

	DWORD dwCurTime = FRAMEGETTIME();
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

	AttackAttribute rkAttribute;
	int iMaxCnt = 0;
	if( !bExtendChargeCombo )
	{
		// extend combo D~, +-->, +D
		rkAttribute = m_ExtendAttackList[m_iCurComboCnt];
		iMaxCnt = m_ExtendAttackList.size();
		if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		{
			return;
		}

		// 이동 속도 제한 2020-10-29
		if( !m_SpeedDownBuff.IsEmpty() )
		{
			pOwner->AddNewBuff( m_SpeedDownBuff, pOwner->GetCharName(), "", NULL );			
		}

		// 크기가 작아지며 전방 위치로 이동하면서 주먹을 휘두름
		if( !m_bScale )
		{
			// 이펙트
			CreateScaleEffectChargeEffect( pOwner, bExtendChargeCombo );
			m_bScale = true;			
			D3DXVECTOR3 vScale = m_vBeforeScale - (m_vBeforeScale * m_fScaleValue);
			pOwner->SetWorldScale( vScale );	
			m_bExtendChargingAttack	= false;
		}
	}
	else
	{

		// 이동 속도 제한 커질때는 삭제 2020-10-29
		if( !m_SpeedDownBuff.IsEmpty() )
		{
			pOwner->RemoveBuff( m_SpeedDownBuff );
		}

		// extend combo +D~, +-->, +D
		rkAttribute = m_ExtendChargeAttackList[m_iCurComboCnt];
		iMaxCnt = m_ExtendChargeAttackList.size();
		if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		{
			return;
		}

		// 크기가 커지면서 상대를 내려침
		if( !m_bScale )
		{
			// 이펙트
			CreateScaleEffectChargeEffect( pOwner, bExtendChargeCombo );
			D3DXVECTOR3 vScale = m_vBeforeScale + (m_vBeforeScale * m_fExtendScaleValue);
			pOwner->SetWorldScale( vScale );
			m_bScale = true;
		}
	}

	// D~ 크기가 작아지며 전방 위치로 이동하면서 공격
	if( m_iCurComboCnt == 0 && !m_bExtendChargingAttack )
	{
		CheckTarget( pOwner, m_fJumpSlideAngle, m_fJumpSlideTaregtCheckRange, m_fJumpSlideMoveRange, false );
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
		if( pTarget )
		{
			pOwner->SetTargetRotToTargetPos( pTarget, true );

			m_vMoveDir = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
			D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );
		}
		else
		{
			m_vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );
		}
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( pOwner && !pOwner->IsActionStop() )	{	pOwner->InitActionStopDelay( true );	}
	else	{	pOwner->InitActionStopDelay( false );	}

	int iAniID = pGrp->GetAnimationIdx( rkAttribute.m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioSizeComboJump::SetExtendComboChargingJumpAttack - %s Animation is Not Exist",
								rkAttribute.m_AttackAnimation.c_str() );
		return;
	}

	float fTimeRate = rkAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = rkAttribute.m_dwPreDelay;

	DWORD dwStartTime = dwCurTime + dwPreDelay;
	m_dwEnableComboTime_E = dwStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		
	pOwner->RefreshFireTimeList( iAniID, rkAttribute.m_vWeaponInfoList, FTT_JUMP_ATTACK, fTimeRate, dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	pOwner->ClearReservedSliding();
	pOwner->SetReservedSliding( rkAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwTrackingTime = dwStartTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_dwEnableComboTime_S = dwStartTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	DWORD dwDuration = dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + rkAttribute.m_dwEndDelay;
	m_dwActionEndTime = dwCurTime + dwDuration;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );
	
	m_SizeComboJumpState = SCJS_CHARGING_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_SizeComboJumpState;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetMoveDir();
		kPacket << pOwner->GetInputDir();
		kPacket << bExtendChargeCombo;
		kPacket << m_fSlideSpeed;	
		kPacket << !m_bScale;
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_bJumpAttackReserve = false;
	m_bReserveJumpAttack = false;
	m_bUseExtendJump = true;
	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );

	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	m_iCurComboCnt++;
	pOwner->SetJumpAttackState();
}


void ioSizeComboJump::ProcessComboChargingJumpAttack( ioBaseChar *pOwner )
{
	if( !pOwner )	{	return;	}

	CheckAirJump( pOwner );

	CheckExtendComboAttack( pOwner );	

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLooseTime = 0;
	if( pOwner )
	{
		dwLooseTime = pOwner->GetActionStopTotalDelay();
	}

	if( m_bReserveJumpAttack && pOwner->IsNeedProcess() )
	{
		if( dwCurTime >= m_dwEnableComboTime_S+dwLooseTime )
		{
			if( pOwner->IsAttackKey() && !pOwner->IsHasCrown() )
			{
				m_dwExtendChargingStartTime	= FRAMEGETTIME();
				m_SizeComboJumpState		= SCJS_EXTEND_CHARGE;

				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_EXTEND_JUMP );
					kPacket << pOwner->GetCharName();
					kPacket << (int)GetType();
					kPacket << pOwner->GetHP().m_fCurValue;
					kPacket << m_SizeComboJumpState;
					kPacket << pOwner->GetTargetRot();
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
				return;				
			}
		}
	}

	// D~ 이어서 --> 대시 아니면 +D~ 이후에 +--> 대시
	if( IsCanJumpDash( pOwner ) && m_iCurComboCnt == 1 && pOwner->IsNeedProcess() )
	{	
		// 이동 속도가 누적되어 추가 2020-10-29
		pOwner->SetCurJumpPowerAmt( 0.0f );
		pOwner->SetForcePowerAmt( 0.0f );
		pOwner->SetGravityAmt( 0.0f );

		SetJumpDash( pOwner, m_bExtendChargingAttack );
		return;
	}

	// D~ 크기가 작아지며 전방 위치로 이동하면서 공격
	if( !m_bExtendChargingAttack )
	{
		if( m_iCurComboCnt == 1 )
		{
			float fMoveSpeed = 0.0f;
			fMoveSpeed = m_fSlideSpeed * g_FrameTimer.GetSecPerFrame();
			m_fMoveAmt += fMoveSpeed;
			D3DXVECTOR3 vTotalMove = m_vMoveDir * fMoveSpeed;		
			// m_fJumpSlideMoveRange 영역까지 이동
			if( m_fMoveAmt < m_fJumpSlideMoveRange )
			{
				ProcessSlideMove( pOwner, vTotalMove );
			}
		}

		// D~, +-->, +D 중 공중일 경우 상태 종료 시 점핑 2020-10-21
		DWORD dwEndTime = m_dwActionEndTime + dwLooseTime;
		if( dwEndTime < FRAMEGETTIME() && !m_bExtendChargingAttack && pOwner->IsNeedProcess() )
		{
			SetEndState( pOwner );
			return;
		}
	}
}

void ioSizeComboJump::ProcessSlideMove( ioBaseChar *pOwner, D3DXVECTOR3 vTotalMove )
{
	bool bCol = false;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage && pStage->ReCalculateMoveVectorByTerrain( pOwner, &vTotalMove, bCol ) )
	{
		float fPreHeight = pOwner->GetBottomHeight();
		pOwner->Translate( vTotalMove );

		float fMoveHeight = pOwner->GetBottomHeight();

		if( fPreHeight > 0.0f && fMoveHeight <= FLOAT1 )
		{
			D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();

			float fTopHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate( FLOAT1 ) );

			if( fTopHeight > FLOAT1 )
			{
				vCurPos.y = max( fTopHeight, fPreHeight );
				pOwner->SetWorldPosition( vCurPos );
			}
		}
	}

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	float fPreHeight = pOwner->GetBottomHeight();

	if( vPos.y < fPreHeight )
		vPos.y = fPreHeight;

	if( pOwner->CheckAxisLimit( &vPos ) )
		pOwner->SetWorldPosition( vPos );
}

void ioSizeComboJump::CheckTarget( ioBaseChar *pOwner, float fAngle, float fTargetCheckRange, float fRange, bool bSend )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	bool bChange = false;
	ioHashString szTarget;
	m_fTargetRange = fRange;

	ioBaseChar *pTarget = FindTarget( pOwner, fAngle, fTargetCheckRange );

	if( pTarget )
		szTarget = pTarget->GetCharName();

	if ( m_AimedTarget != szTarget )
	{
		bChange = true;
		m_bAimedBall = false;
		m_AimedTarget = szTarget;
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SCJS_CHARGING_TARGET;
		kPacket << m_AimedTarget;
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


ioBaseChar *ioSizeComboJump::FindTarget( ioBaseChar *pOwner, float fAngle, float fRange )
{
	if( !pOwner )
		return NULL;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return NULL;

	ioBaseChar *pTargetChar = NULL;
	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	float fPreDistSq = fRange * fRange;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	BaseCharDistanceSqList kList;
	kList.clear();
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget )	continue;

		if( pTarget == pOwner )
			continue;

		if( pTarget->GetTeam() == pOwner->GetTeam() )
			continue;

		if ( pTarget->IsSystemState() )
			continue;

		if( !pTarget->IsEnableTarget() )
			continue;

		if( !pTarget->CheckTargetWoundType( m_TargetWoundType ) )
			continue;

		D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		float fDistSq = D3DXVec3LengthSq( &vDiff );

		if( fDistSq <= fPreDistSq )
		{
			fPreDistSq = fDistSq;

			D3DXVec3Normalize( &vDiff, &vDiff );
			float fValue = D3DXVec3Dot( &vDir, &vDiff );
			float fAngleGap = RADtoDEG( acosf(fValue) );
			if( fAngleGap < fAngle * FLOAT05 )
			{
				pTargetChar = pTarget;
			}
		}
	}

	return pTargetChar;
}

void ioSizeComboJump::SetEndState( ioBaseChar *pOwner )
{
	if( !pOwner )	{	return;	}	

	float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
	if( fHeightGap < m_fSlideEndJumpEnableHeight )
	{
		pOwner->SetExtendAttackEndJump( 0, FLOAT1 );
	}
	else
	{
		pOwner->SetSKillEndJumpState( m_fSlideEndJumpAmt, false, false, true, true );
	}

	if( pOwner->IsNeedSendNetwork() && pOwner->IsNeedProcess())
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SCJS_CHARGING_END;
		kPacket << pOwner->GetWorldPosition();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


// Jump Dash
bool ioSizeComboJump::IsCanJumpDash( ioBaseChar *pOwner )
{
	if( !pOwner )							{	return false;	}
	if( !pOwner->IsNeedProcess() )			{	return false;	}	

	if( pOwner->IsHasCrown()								||
		pOwner->IsCatchMode()								||
		pOwner->IsPrisonerMode()							||
		(pOwner->GetUsedBuffJump() != BJUT_NONE )			||
		(pOwner->GetUsedBuffFlyJump() != BJUT_NONE )		||
		(pOwner->GetUsedBuffMultiFlyJump() != BJUT_NONE ) )
	{
		return false;
	}

	if( pOwner->IsBuffLimitExtendJump() )	{	return false;	}
	if( pOwner->HasBuff(BT_AIR_ATTACK) )	{	return false;	}
	if( m_bUsedDashJumpAttack  )			{	return false;	}

	
	DWORD dwMoveStartTime = pOwner->GetJumpMoveStartTime();
	if( dwMoveStartTime == 0 )				{	return false;	}

	if( !pOwner->IsDirKeyDoubleClick() )
	{
		return false;
	}

	return true;
}

void ioSizeComboJump::SetJumpDash( ioBaseChar *pOwner, bool bExtendChargingAttack )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
	if( eDir != ioUserKeyInput::DKI_NONE )
	{
		float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );

		float fYawR = DEGtoRAD( fYawD );
		D3DXQUATERNION qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

		pOwner->SetTargetRotToRotate( qtRotate, false );
	}

	ioEntityGroup *pGrp		= pOwner->GetGroup();
	m_SizeComboJumpState	= SCJS_DASH;
	m_bUseExtendJump		= true;
	m_dwChargingStartTime	= FRAMEGETTIME();
	
	AttackAttribute rkAttribute;
	if( !bExtendChargingAttack )
	{
		// extend combo D~, +-->
		rkAttribute = m_ExtendAttackList[m_iCurComboCnt];
		int iMaxCnt = m_ExtendAttackList.size();
		if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		{
			return;
		}		
	}
	else
	{
		// extend combo +D~, ++-->
		rkAttribute = m_ExtendChargeAttackList[m_iCurComboCnt];
		int iMaxCnt = m_ExtendChargeAttackList.size();
		if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		{
			return;
		}
	}

	int iAniID = pGrp->GetAnimationIdx( rkAttribute.m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioSizeComboJump::SetJumpDash - %s Animation is Not Exist",
								rkAttribute.m_AttackAnimation.c_str() );
		return;
	}

	float fTimeRate = rkAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = rkAttribute.m_dwPreDelay;
	DWORD dwEndDelay = rkAttribute.m_dwEndDelay;

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pOwner->SetAutoTarget( ATT_JUMP );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->RefreshFireTimeList( iAniID,
								 rkAttribute.m_vWeaponInfoList,
								 FTT_JUMP_ATTACK,
								 fTimeRate,
								 dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	m_dwJumpDashStartTime = dwCurTime;
	m_dwJumpDashEndTime = dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	// 2020-10-19
	m_dwEnableComboTime_S = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( rkAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );


	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SCJS_DASH;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetMoveDir();
		kPacket << pOwner->GetInputDir();
		kPacket << bExtendChargingAttack;
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_bUsedDashJumpAttack = true;

	m_iCurComboCnt++;
}

void ioSizeComboJump::ProcessJumpDash( ioBaseChar *pOwner )
{
	if( !pOwner )	{	return;	}

	DWORD dwCurTime = FRAMEGETTIME();	// 2020-10-21
	if( m_dwJumpDashEndTime > 0 && m_dwJumpDashEndTime < dwCurTime && pOwner->IsNeedProcess()  )
	{
		if( !m_bExtendChargingAttack )
		{
			// D~, +-->, +D 중 공중일 경우 상태 종료 시 점핑
			SetEndState( pOwner );
		}
	}
	else
	{
		if( pOwner->IsAttackKey() && !pOwner->IsHasCrown()  && pOwner->IsNeedProcess() )			
		{
			// 2020-10-19			
			if( m_dwChargingStartTime > 0 && m_dwChargingStartTime + m_dwChargingTime < dwCurTime )
			{	
				// 게이지 검사
				if( CheckEnableExtendComboState( pOwner ) )
				{
					// D~ 크기가 작아지며 전방으로 주먹을 휘두름
					m_AimedTarget.Clear();
					m_iCurComboCnt	= 0;
					m_fMoveAmt		= 0.0f;
					m_fSlideSpeed	= m_fChargeSlideSpeed;

					// 대시 상태에서 다시 D~ 을 했을 때는 false로 해준다.
					m_bUsedDashJumpAttack = false;

					SetExtendComboChargingJumpAttack( pOwner );				
					return;
				}

			}
		}
	}
}

void ioSizeComboJump::ProcessExtendCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwExtendChargingStartTime > 0 && m_dwExtendChargingStartTime+m_dwExtendChargingTime < dwCurTime )
		{			
			// 게이지 검사
			if( CheckEnableExtendComboState( pOwner ) )
			{
				if( m_iCurComboCnt == 1 || m_iCurComboCnt == 3 )
				{
					// +D or +--> 공격 이 없는 경우에는 다음 +D~ 으로 공격 한다. 
					// 크기가 커지면서 상대를 내려침 
					m_iCurComboCnt			= 0;
					m_bScale				= false;

					if( !m_bExtendChargingAttack )
					{
						m_bExtendChargingAttack	= true;						
					}
					else
					{
						m_bExtendChargingAttack	= false;						
					}
					SetExtendComboChargingJumpAttack( pOwner, m_bExtendChargingAttack );

				}
				else
				{
					// +--> 공격 후 +D~ 했을 경우에는 다시 기본 D~으로 되돌아간다. 
					m_iCurComboCnt = 0;
					m_bExtendChargingAttack = false;
					SetExtendComboChargingJumpAttack( pOwner, m_bExtendChargingAttack );
				}
			}
			else
			{
				// 기본 공격
				SetComboJumpAttack( pOwner );
			}

		}
	}
	else
	{
		int iMaxCnt = m_ExtendAttackList.size();
		if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )	{	return;	}
		
		iMaxCnt = m_ExtendChargeAttackList.size();
		if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )	{	return;	}

		// +D 2번 애니 공격 
		m_iCurComboCnt = 2;
		SetExtendComboChargingJumpAttack( pOwner, m_bExtendChargingAttack );
		
	}
}

bool ioSizeComboJump::IsExtendComboChargingJumpAttack()
{
	if( m_bExtendChargingAttack ) return false;
	if( m_iCurComboCnt == 3 ) return false;

	switch( m_SizeComboJumpState )
	{
		case SCJS_CHARGING_ATTACK:
		case SCJS_DASH:
		{
			return true;
		}
	}

	return false;
}


float ioSizeComboJump::GetCurGravityAmt()
{
	switch( m_SizeComboJumpState )
	{
		case SCJS_DASH:
		{
			// D~ 이어서 +D~ 이어서 대시 했을 경우에는 SetEndState() 함수를 호출 안함 중력으로 내려감
			if( m_bExtendChargingAttack )
			{
				return m_fComboDashGravityAmt;
			}
			return -FLOAT1;
		}
		case SCJS_CHARGING_ATTACK:
		{
			if( m_iCurComboCnt == 1 && !m_bExtendChargingAttack )
			{
				return -FLOAT1;
			}			
			return 0.0f;
		}
	}
	return 0.0f;
}

bool ioSizeComboJump::CheckEnableExtendComboState( ioBaseChar *pOwner )
{
	if( NULL == pOwner )					{	return false;	}
	if( m_ExtendAttackList.empty() )		{	return false;	}
	if( m_ExtendChargeAttackList.empty() )	{	return false;	}

	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioSizeItem* pSizeItem = ToSizeItem( pItem );
	if( pSizeItem )
	{
		// 처음 콤보 시작
		int iCurBullet = pSizeItem->GetCurBullet();
		int iNeedBullet = pSizeItem->GetNeedBullet();

		if( iCurBullet >= iNeedBullet )
		{
			pSizeItem->DecreaseCoolTime();
			return true;
		}
	}

	return false;
}


void ioSizeComboJump::CreateScaleEffectChargeEffect( ioBaseChar *pOwner, bool bExtendChargeCombo )
{
	if ( !pOwner )
	{
		return;
	}

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
	{
		return;
	}

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vScale = pOwner->GetWorldScale();

	if( !bExtendChargeCombo )
	{
		// 크기가 작아지면서 상대의 위치로 이동하며 공격한다.
		if ( m_szExtendScaleEffectName.IsEmpty() )
		{
			return;
		}

		ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_szExtendScaleEffectName, vPos, m_vBeforeScale );
		if ( pMapEffect )
		{
			m_dwExtendScaleEffectID =  pMapEffect->GetUniqueID();
		}
	}
	else
	{
		// 크기가 커지면서 상대를 내려친다.
		if ( m_szScaleEffectName.IsEmpty() )
		{
			return;
		}

		
		ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_szScaleEffectName, vPos, m_vBeforeScale );
		if ( pMapEffect )
		{
			m_dwScaleEffectID =  pMapEffect->GetUniqueID();
		}
	}

	if( !m_szScaleEffectCreateSound.IsEmpty() )
	{
		g_SoundMgr.PlaySound( m_szScaleEffectCreateSound );
	}
}

void ioSizeComboJump::ProcessScaleEffect( ioBaseChar* pOwner )
{
	if ( !pOwner )
	{
		return;
	}


	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
	{
		return;
	}
	
	if( !m_bExtendChargingAttack )
	{
		// 크기가 커지면서 상대를 내려친다.
		if ( m_dwExtendScaleEffectID == -1)
		{
			return;
		}

		ioMapEffect* pMapEffect = pStage->FindMapEffect( m_dwExtendScaleEffectID );
		if( pMapEffect )
		{
			D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
			pMapEffect->SetWorldPosition( vPos );
		}
	}
	else
	{
		// 크기가 작아지면서 상대의 위치로 이동하며 공격한다.
		if ( m_dwScaleEffectID == -1 )
		{
			return;
		}

		ioMapEffect* pMapEffect = pStage->FindMapEffect( m_dwScaleEffectID );
		if( pMapEffect )
		{
			D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
			pMapEffect->SetWorldPosition( vPos );
		}
	}
}