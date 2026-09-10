
#include "stdafx.h"

#include "ioFlyJumpExtraAttackBuff.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "WeaponDefine.h"

ioFlyJumpExtraAttackBuff::ioFlyJumpExtraAttackBuff()
{
}

ioFlyJumpExtraAttackBuff::ioFlyJumpExtraAttackBuff( const ioFlyJumpExtraAttackBuff &rhs )
	: ioBuff( rhs ),
	  m_fTargetXAngle( rhs.m_fTargetXAngle ),
	  m_fTargetYAngle( rhs.m_fTargetYAngle ),
	  m_fTargetRange( rhs.m_fTargetRange ),
	  m_fMoveTargetSpeed( rhs.m_fMoveTargetSpeed ),
	  m_fEndJumpPower( rhs.m_fEndJumpPower ),
	  m_MoveTarget( rhs.m_MoveTarget ),
	  m_szGrapJumpLoopAni( rhs.m_szGrapJumpLoopAni ),
	  m_fGrapJumpLoopAniRate( rhs.m_fGrapJumpLoopAniRate ),
	  m_szGrapNormalLoopAni( rhs.m_szGrapNormalLoopAni ),
	  m_fGrapNormalLoopAniRate( rhs.m_fGrapNormalLoopAniRate ),
	  m_WoundedJumpGrapplingInfo( rhs.m_WoundedJumpGrapplingInfo ),
	  m_WoundedNormalGrapplingInfo( rhs.m_WoundedNormalGrapplingInfo ),
	  m_fCharJumpHeightRate( rhs.m_fCharJumpHeightRate ),
	  m_fTargetJumpGapRange( rhs.m_fTargetJumpGapRange ),
	  m_fTargetJumpGapHeight( rhs.m_fTargetJumpGapHeight ),
	  m_fCharNormalHeightRate( rhs.m_fCharNormalHeightRate ),
	  m_fTargetNormalGapRange( rhs.m_fTargetNormalGapRange ),
	  m_fTargetNormalGapHeight( rhs.m_fTargetNormalGapHeight ),
	  m_fCharLandAttackHeightRate( rhs.m_fCharLandAttackHeightRate ),
	  m_fTargetLandAttackGapRange( rhs.m_fTargetLandAttackGapRange ),
	  m_fTargetLandAttackGapHeight( rhs.m_fTargetLandAttackGapHeight ),
	  m_fJumpHeightGap( rhs.m_fJumpHeightGap ),
	  m_fGrapJumpPower( rhs.m_fGrapJumpPower ),
	  m_fGrapJumpForce( rhs.m_fGrapJumpForce ),
	  m_fGrapNormalMoveSpeed( rhs.m_fGrapNormalMoveSpeed ),
	  m_JumpGrapLandAttack( rhs.m_JumpGrapLandAttack ),
	  m_NormalGrapLandAttack( rhs.m_NormalGrapLandAttack ),
	  m_dwGrapNormalLoopTime( rhs.m_dwGrapNormalLoopTime ),
	  m_fExtraGaugeInc( rhs.m_fExtraGaugeInc ),
	  m_fMaxExtraGauge( rhs.m_fMaxExtraGauge ),
	  m_fNeedGauge( rhs.m_fNeedGauge ),
	  m_fMaxMoveDist( rhs.m_fMaxMoveDist ),
	  m_nAttachWeapon( rhs.m_nAttachWeapon )
{
}

ioFlyJumpExtraAttackBuff::~ioFlyJumpExtraAttackBuff()
{
}

void ioFlyJumpExtraAttackBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );	

	char szBuf[MAX_PATH];

	m_fTargetXAngle = rkLoader.LoadFloat_e( "target_x_angle", 0.0f );
	m_fTargetYAngle = rkLoader.LoadFloat_e( "target_y_angle", 0.0f );
	m_fTargetRange = rkLoader.LoadFloat_e( "target_range", 0.0f );
	m_fMoveTargetSpeed = rkLoader.LoadFloat_e( "move_target_speed", 0.0f );
	m_fEndJumpPower = rkLoader.LoadFloat_e( "end_jump_power", 0.0f );

	LoadAttackAttribute_e( "move_target", m_MoveTarget, rkLoader );

	rkLoader.LoadString_e( "jump_grap_loop_ani", "", szBuf, MAX_PATH );
	m_szGrapJumpLoopAni = szBuf;
	m_fGrapJumpLoopAniRate = rkLoader.LoadFloat_e( "jump_grap_loop_ani_rate", FLOAT1 );

	m_WoundedJumpGrapplingInfo.Init();
	m_WoundedJumpGrapplingInfo.m_bReverseTargetRot = rkLoader.LoadBool_e( "jump_target_reverse_rotate", false );
	m_WoundedJumpGrapplingInfo.m_GrapplingColType = (GrapplingColType)rkLoader.LoadInt_e( "jump_wound_grappling_enable_col", GCT_ALL );

	rkLoader.LoadString_e( "jump_grap_wound_ani", "", szBuf, MAX_PATH );
	m_WoundedJumpGrapplingInfo.m_GrapplingLoopMotion = szBuf;
	m_WoundedJumpGrapplingInfo.m_GrapplingLoopMotion2 = szBuf;
	m_WoundedJumpGrapplingInfo.m_fGrapplingLoopMotionRate = rkLoader.LoadFloat_e( "jump_grap_wound_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "normal_grap_loop_ani", "", szBuf, MAX_PATH );
	m_szGrapNormalLoopAni = szBuf;
	m_fGrapNormalLoopAniRate = rkLoader.LoadFloat_e( "normal_grap_loop_ani_rate", FLOAT1 );

	m_WoundedNormalGrapplingInfo.Init();
	m_WoundedNormalGrapplingInfo.m_bReverseTargetRot = rkLoader.LoadBool_e( "normal_target_reverse_rotate", false );
	m_WoundedNormalGrapplingInfo.m_GrapplingColType = (GrapplingColType)rkLoader.LoadInt_e( "normal_wound_grappling_enable_col", GCT_ALL );

	rkLoader.LoadString_e( "normal_grap_wound_ani", "", szBuf, MAX_PATH );
	m_WoundedNormalGrapplingInfo.m_GrapplingLoopMotion = szBuf;
	m_WoundedNormalGrapplingInfo.m_GrapplingLoopMotion2 = szBuf;
	m_WoundedNormalGrapplingInfo.m_fGrapplingLoopMotionRate = rkLoader.LoadFloat_e( "normal_grap_wound_ani_rate", FLOAT1 );

	m_fCharJumpHeightRate = rkLoader.LoadFloat_e( "jump_char_height_rate", FLOAT05 );
	m_fTargetJumpGapRange = rkLoader.LoadFloat_e( "jump_target_loop_gap_range", 0.0f );
	m_fTargetJumpGapHeight = rkLoader.LoadFloat_e( "jump_target_loop_gap_height", 0.0f );

	m_fCharNormalHeightRate = rkLoader.LoadFloat_e( "normal_char_height_rate", FLOAT05 );
	m_fTargetNormalGapRange = rkLoader.LoadFloat_e( "normal_target_loop_gap_range", 0.0f );
	m_fTargetNormalGapHeight = rkLoader.LoadFloat_e( "normal_target_loop_gap_height", 0.0f );

	m_fCharLandAttackHeightRate = rkLoader.LoadFloat_e( "land_attack_char_height_rate", FLOAT05 );
	m_fTargetLandAttackGapRange = rkLoader.LoadFloat_e( "land_attack_target_loop_gap_range", 0.0f );
	m_fTargetLandAttackGapHeight = rkLoader.LoadFloat_e( "land_attack_target_loop_gap_height", 0.0f );

	m_fJumpHeightGap = rkLoader.LoadFloat_e( "jump_height_gap", 0.0f );
	m_fGrapJumpPower = rkLoader.LoadFloat_e( "grap_jump_power", 0.0f );
	m_fGrapJumpForce = rkLoader.LoadFloat_e( "grap_jump_force", 0.0f );

	LoadAttackAttribute_e( "jump_land_attack", m_JumpGrapLandAttack, rkLoader );
	LoadAttackAttribute_e( "normal_land_attack", m_NormalGrapLandAttack, rkLoader );
	
	m_fGrapNormalMoveSpeed = rkLoader.LoadFloat_e( "grap_normal_move_speed", 0.0f );
	m_dwGrapNormalLoopTime = rkLoader.LoadInt_e( "grap_normal_loop_time", 0 );

	m_fExtraGaugeInc = rkLoader.LoadFloat_e( "extra_gauge_inc", 0.0f );
	m_fMaxExtraGauge = rkLoader.LoadFloat_e( "max_extra_gauge", 0.0f );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", 0.0f );
	m_fMaxMoveDist = rkLoader.LoadFloat_e( "max_move_dist", 0.0f );
	m_nAttachWeapon = rkLoader.LoadInt_e( "move_target_attach_weapon", NO_WEAPON_ATTRIBUTE );
}

ioBuff* ioFlyJumpExtraAttackBuff::Clone()
{
	return new ioFlyJumpExtraAttackBuff( *this );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioFlyJumpExtraAttackBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
	
	m_SpecialState = BS_None;

	m_fCurExtraGauge = 0.0f;
	m_fCurMoveDist = 0.0f;
}

bool ioFlyJumpExtraAttackBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	m_SpecialState = BS_None;
	m_fCurMoveDist = 0.0f;

	return true;
}

void ioFlyJumpExtraAttackBuff::ProcessBuff( float fTimePerSec )
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

	if ( GetOwner() && GetOwner()->IsNeedProcess() && CheckSpecialState( GetOwner() ) )
		SetSpecialState( GetOwner() );

	UpdateExtraCnt( GetOwner() );
}

void ioFlyJumpExtraAttackBuff::EndBuff()
{
	ioBuff::EndBuff();
}

void ioFlyJumpExtraAttackBuff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	int nBuffState;
	rkPacket >> nBuffState;

	switch( (BuffState)nBuffState )
	{
	case BS_Special_State_Init:
		{			
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pOwner->SetWorldPosition( vPos );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			rkPacket >> m_szTargetName;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetSpecialState( pOwner );
		}
		break;
	case BS_Special_State_Process:
		{
			RecvSpecialState( pOwner, rkPacket );
		}
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioFlyJumpExtraAttackBuff::CheckSpecialState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	if ( pOwner->GetState() == CS_JUMP && pOwner->GetUsedBuffFlyJump() == BJUT_CUR && pOwner->IsAttackKeyDown() && m_fCurExtraGauge >= m_fNeedGauge )
		return true;

	return false;
}

void ioFlyJumpExtraAttackBuff::SetSpecialState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	pOwner->SetState( CS_FLY_BUFF_EXTRA_ATTACK );

	m_dwMotionEndTime = 0;
	m_GrapplingTargetName.Clear();
	m_bSetGrapJump = false;
	m_dwCurGrapLoopTime = 0;
	m_dwGrapplingStartTime = 0;

	m_fCurExtraGauge -= m_fNeedGauge;
	m_fCurExtraGauge = max( m_fCurExtraGauge, 0.0f );

	CreateAttachWeapon( pOwner );
	SetMoveTargetState( pOwner );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << BS_Special_State_Init;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetWorldOrientation();
		kPacket << m_szTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioFlyJumpExtraAttackBuff::IsCharCollisionSkipState()
{
	switch( m_SpecialState )
	{
	case BS_Move_Target:
		return true;
	}

	return false;
}

void ioFlyJumpExtraAttackBuff::ProcessSpecialState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	switch ( m_SpecialState )
	{
	case BS_Move_Target:
		{
			D3DXVECTOR3 vTargetDir = pOwner->GetWorldOrientation() * -ioMath::UNIT_Z;
			if ( !m_szTargetName.IsEmpty() )
			{
				ioBaseChar* pTarget = pOwner->GetBaseChar( m_szTargetName );
				if ( pTarget )
				{
					vTargetDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();					
					pOwner->SetTargetRotToDir( vTargetDir, true );
				}
			}

			D3DXVec3Normalize( &vTargetDir, &vTargetDir );
			if ( m_fCurMoveDist >= m_fMaxMoveDist || ProcessMove( pOwner, m_fMoveTargetSpeed, vTargetDir ) )
				SetSpecialEndState( pOwner, false );
		}
		break;
	case BS_Grap_Jump_Loop:
		{
			if ( !CheckTargetState( pOwner ) )//|| IsDropZone( pOwner ) )
			{
				SetSpecialEndState( pOwner, false );
				return;
			}

			if ( pOwner->IsNeedProcess() && !IsFloatState( pOwner ) )
				SetGrapLandAttackState( pOwner, m_JumpGrapLandAttack, true );
		}
		break;
	case BS_Grap_Normal_Loop:
		{
			if ( !CheckTargetState( pOwner ) )//|| IsDropZone( pOwner ) )
			{
				SetSpecialEndState( pOwner, false );
				return;
			}
			
			if ( m_dwCurGrapLoopTime < dwCurTime || ProcessMove( pOwner, m_fGrapNormalMoveSpeed, pOwner->GetMoveDir() ) )
			{
				if ( pOwner->IsNeedProcess() )
					SetGrapLandAttackState( pOwner, m_NormalGrapLandAttack, false );
				return;
			}
		}
		break;
	case BS_Grap_Land_Attack:
		{
			if ( m_dwMotionEndTime < dwCurTime )
				SetSpecialEndState( pOwner, false );
		}
		break;
	}
}

void ioFlyJumpExtraAttackBuff::RecvSpecialState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	int nSpecialState;
	rkPacket >> nSpecialState;

	switch( nSpecialState )
	{
	case BS_Grappling:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pOwner->SetWorldPosition( vPos );

			rkPacket >> m_GrapplingTargetName;

			rkPacket >> m_bSetGrapJump;

			ioBaseChar *pTarget = pOwner->GetBaseChar( m_GrapplingTargetName );
			if( !pTarget ) 
				return;

			if ( m_bSetGrapJump )
			{
				SetGrapJumpLoopState( pOwner );
				pTarget->SetBuffGrapplingState( pOwner->GetCharName(), m_WoundedJumpGrapplingInfo );
			}
			else
			{
				SetGrapNormalLoopState( pOwner );
				pTarget->SetBuffGrapplingState( pOwner->GetCharName(), m_WoundedNormalGrapplingInfo );
			}
		}
		break;
	case BS_Grap_Land_Attack:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pOwner->SetWorldPosition( vPos );

			rkPacket >> m_bSetGrapJump;

			if ( m_bSetGrapJump )		
				SetGrapLandAttackState( pOwner, m_JumpGrapLandAttack, m_bSetGrapJump );
			else
				SetGrapLandAttackState( pOwner, m_NormalGrapLandAttack, m_bSetGrapJump );
		}
		break;
	case BS_End:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pOwner->SetWorldPosition( vPos );

			SetSpecialEndState( pOwner, false );
		}
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioFlyJumpExtraAttackBuff::FindTarget( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return false;

	BaseCharList vTargetList;
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();

	pOwner->GetTargetCharListBySphere( vPos, m_fTargetRange, m_fTargetYAngle, vTargetList );

	m_szTargetName.Clear();
	if ( vTargetList.empty() )
		return false;
	
	SortTargetList( pOwner, vTargetList, m_szTargetName );

	if ( m_szTargetName.IsEmpty() )
		return false;
	
	return true;
}

void ioFlyJumpExtraAttackBuff::SortTargetList( ioBaseChar *pOwner, const BaseCharList &TargetList, ioHashString &szTargetName )
{
	if( !pOwner )	
		return;
	
	D3DXQUATERNION qtRot = pOwner->GetWorldOrientation();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	float fMinRangeByTargetDist = m_fTargetRange * m_fTargetRange;

	BaseCharList::const_iterator iter;
	for( iter=TargetList.begin(); iter!=TargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( pTarget != pOwner )
		{
			//팀이 같거나 타겟이 될수 없으면 리스트에 저장 안함
			if ( pOwner->GetTeam() == pTarget->GetTeam() )
				continue;


			if( pTarget->IsSystemState() )
				continue;
		
			if( !pTarget->IsEnableTarget() ) 	
				continue;


			CharState eState = pTarget->GetState();
			switch( eState )
			{			
				continue;
			case CS_BLOW_WOUNDED:
				{
					if ( !pTarget->IsFloatingState() )			
						continue;
				}
			case CS_BOUND_BLOW_WOUNDED:
				{
					if ( !pTarget->IsFloatingState() )			
						continue;
				}
			case CS_ETC_STATE:
				{
					if ( pTarget->IsApplyDownState(false) )			
						continue;
				}
			case CS_USING_SKILL:
				{
					if ( pTarget->IsApplyDownState(false) )			
						continue;
				}
			}

			D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
			float fLengthSq = D3DXVec3LengthSq( &vDiff );
			float fMaxRangeSq = m_fTargetRange * m_fTargetRange;

			//Range Test
			bool bRangeAvailable = false;
			if( COMPARE( fLengthSq, 0.0f, fMaxRangeSq ) )
				bRangeAvailable = true;

			//XAngle Test
			D3DXVec3Normalize( &vDiff, &vDiff );

			bool bXAngle = false;
			D3DXVECTOR3 vMoveDir = vDir;
			D3DXVECTOR3 vTargetDir = vDiff;
			vMoveDir.x = 0;
			vTargetDir.x = 0;
			float fValue = D3DXVec3Dot( &vDir, &vDiff );
			float fAngleGap = RADtoDEG( acosf(fValue) );
			if( fAngleGap <= m_fTargetXAngle )
				bXAngle = true;

			if( bRangeAvailable && bXAngle )
			{	
				if ( fMinRangeByTargetDist > fLengthSq )
				{
					fMinRangeByTargetDist = fLengthSq;
					szTargetName = pTarget->GetCharName();
				}
			}
		}
	}	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioFlyJumpExtraAttackBuff::SetMoveTargetState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_MoveTarget.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	float fTimeRate  = m_MoveTarget.m_fAttackAniRate;
	DWORD dwPreDelay = m_MoveTarget.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );	

	pOwner->ClearReservedSliding();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	m_SpecialState = BS_Move_Target;

	m_fCurMoveDist = 0.0f;

	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = ioUserKeyInput::DKI_NONE;
		if ( pOwner->IsSettedDirection() )
		{
			eNewDirKey = pOwner->CheckCurDirKey();
			pOwner->SetTargetRotToDirKey( eNewDirKey );
			pOwner->SetWorldOrientation( pOwner->GetTargetRot() );
		}

		FindTarget( pOwner );
	}
}

void ioFlyJumpExtraAttackBuff::SetGrapJumpLoopState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	
	ioEntityGroup *pGrp = pOwner->GetGroup();

	int nAniID = pGrp->GetAnimationIdx( m_szGrapJumpLoopAni );
	if( nAniID == -1 )	
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( nAniID, FLOAT100, FLOAT1, FLOAT1/m_fGrapJumpLoopAniRate );

	pOwner->ClearReservedSliding();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetJumpPower( m_fGrapJumpPower );

	D3DXVECTOR3 vJumpDir = pOwner->GetMoveDir();
	if( m_fGrapJumpForce > 0.0f )
		pOwner->SetForcePower( vJumpDir, fabs(m_fGrapJumpForce), FLOAT1, true );
	else
		pOwner->SetForcePower( -vJumpDir, fabs(m_fGrapJumpForce), FLOAT1, true );

	m_SpecialState = BS_Grap_Jump_Loop;
}

void ioFlyJumpExtraAttackBuff::SetGrapNormalLoopState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();

	int nAniID = pGrp->GetAnimationIdx( m_szGrapNormalLoopAni );
	if( nAniID == -1 )	
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( nAniID, FLOAT100, FLOAT1, FLOAT1/m_fGrapNormalLoopAniRate );

	pOwner->ClearReservedSliding();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	m_SpecialState = BS_Grap_Normal_Loop;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwCurGrapLoopTime = dwCurTime + m_dwGrapNormalLoopTime;
}

void ioFlyJumpExtraAttackBuff::SetGrapLandAttackState( ioBaseChar *pOwner, const AttackAttribute &LandAttack, bool bJump )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();

	int nAniID = pGrp->GetAnimationIdx( LandAttack.m_AttackAnimation );
	if( nAniID == -1 )	
		return;

	float fTimeRate  = LandAttack.m_fAttackAniRate;
	DWORD dwPreDelay = LandAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	DWORD dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwWeaponBaseIndex, nAniID, LandAttack.m_vWeaponInfoList, FTT_SKILL_ATTACK, fTimeRate, dwPreDelay );

	pOwner->ClearReservedSliding();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetReservedSliding( LandAttack.m_vForceInfoList, nAniID, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay;
	
	m_bSetGrapJump = bJump;
	m_SpecialState = BS_Grap_Land_Attack;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << BS_Special_State_Process;
		kPacket << BS_Grap_Land_Attack;
		kPacket << pOwner->GetWorldOrientation();
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_bSetGrapJump;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioFlyJumpExtraAttackBuff::SetSpecialEndState( ioBaseChar *pOwner, bool bSendNetwork )
{
	if ( !pOwner )
		return;
	
	m_fCurMoveDist = 0.0f;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	if ( pOwner->IsFloating() )
		pOwner->SetSKillEndJumpState( m_fEndJumpPower, true, false, true, true );
	else
		pOwner->SetState( CS_DELAY, false );

	if( bSendNetwork && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << BS_Special_State_Process;
		kPacket << BS_End;
		kPacket << pOwner->GetWorldOrientation();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioFlyJumpExtraAttackBuff::ProcessMove( ioBaseChar *pOwner, float fMoveSpeed, D3DXVECTOR3 vMoveDir )
{
	if( !pOwner ) 
		return true;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return true;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt;

		if( fMoveSpeed > 0.0f && vMoveDir != ioMath::VEC3_ZERO )
		{
			float fFinalMoveAmt = fMoveSpeed * fDivideTimePerSec;
			if( fFinalMoveAmt > 0.0f )
			{
				vMoveAmt = vMoveDir * fFinalMoveAmt;
				vTotalMove += vMoveAmt;
				bCurMove = true;
			}
		}

		if( pOwner->ProcessForceMove( fDivideTimePerSec, vMoveAmt ) )
		{
			vTotalMove += vMoveAmt;
			bCurMove = true;
		}
	}

	if( bCurMove )
	{
		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( pOwner, &vTotalMove, bCol ) )
		{
			float fPreHeight = pOwner->GetBottomHeight();

			pOwner->Translate( vTotalMove );

			D3DXVECTOR3 vDiff = ioMath::VEC3_ZERO;
			vDiff = vTotalMove - vDiff;
			float fMoveDist = D3DXVec3Length( &vDiff );
			m_fCurMoveDist += fMoveDist;

			if( pOwner->GetForceAmt() != 0.0f )	// 빠른 ForceMove로 경사면을 통과하는 경우에 대한 보강
			{
				float fMoveHeight = pOwner->GetBottomHeight();

				if( fPreHeight > 0.0f && fMoveHeight <= FLOAT1 )
				{
					D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();

					float fTopHeight = pStage->GetMapHeight( vCurPos.x,
						vCurPos.z,
						pOwner,
						false,
						pOwner->GetMidHeightByRate( FLOAT1 ) );

					if( fTopHeight > FLOAT1 )
					{
						vCurPos.y = max( fTopHeight, fPreHeight );
						pOwner->SetWorldPosition( vCurPos );
					}
				}
			}
		}

		if ( bCol )
			return true;
	}

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	if( pStage && pOwner->IsAxisLimit() )
	{
		pStage->CheckAxisLimit( &vPos );
		pOwner->SetWorldPosition( vPos );
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioFlyJumpExtraAttackBuff::SetBuffGrapplingState( ioBaseChar *pOwner, const ioHashString &szTargetName )
{
	if( !pOwner ) 
		return false;

	if( m_SpecialState != BS_Move_Target )
		return false;

	m_GrapplingTargetName = szTargetName;

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_GrapplingTargetName );
	if( !pTarget ) 
		return false;

	m_bSetGrapJump = false;
	if ( pTarget->IsFloating() )
	{
		SetGrapJumpLoopState( pOwner );
		pTarget->SetBuffGrapplingState( pOwner->GetCharName(), m_WoundedJumpGrapplingInfo );
		m_bSetGrapJump = true;
	}
	else
	{
		SetGrapNormalLoopState( pOwner );
		pTarget->SetBuffGrapplingState( pOwner->GetCharName(), m_WoundedNormalGrapplingInfo );
	}

	m_dwGrapplingStartTime = FRAMEGETTIME();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << BS_Special_State_Process;
		kPacket << BS_Grappling;
		kPacket << pOwner->GetWorldOrientation();
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_GrapplingTargetName;
		kPacket << m_bSetGrapJump;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return true;
}

D3DXVECTOR3 ioFlyJumpExtraAttackBuff::GetGrapplingTargetPos( ioBaseChar *pChar )
{
	if ( !pChar )
		return ioMath::VEC3_ZERO;

	if ( m_SpecialState == BS_Grap_Jump_Loop )
	{		
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( m_fCharJumpHeightRate );
		D3DXVECTOR3 vDir = pChar->GetMoveDir();

		vPos += m_fTargetJumpGapRange * vDir;
		vPos.y += m_fTargetJumpGapHeight;

		return vPos;
	}
	else if ( m_SpecialState == BS_Grap_Normal_Loop )
	{		
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( m_fCharNormalHeightRate );
		D3DXVECTOR3 vDir = pChar->GetMoveDir();

		vPos += m_fTargetNormalGapRange * vDir;
		vPos.y += m_fTargetNormalGapHeight;

		return vPos;
	}
	else if ( m_SpecialState == BS_Grap_Land_Attack )
	{
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( m_fCharLandAttackHeightRate );
		D3DXVECTOR3 vDir = pChar->GetMoveDir();

		vPos += m_fTargetLandAttackGapRange * vDir;
		vPos.y += m_fTargetLandAttackGapHeight;

		return vPos;
	}

	return ioMath::VEC3_ZERO;
}

bool ioFlyJumpExtraAttackBuff::IsFloatState( ioBaseChar *pOwner )
{	
	if ( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return false;

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeightOnlyTerrain( vPos.x, vPos.z );

	if ( fabs( fMapHeight - vPos.y ) >= m_fJumpHeightGap )
		return true;

	return false;
}

bool ioFlyJumpExtraAttackBuff::CheckTargetState( ioBaseChar *pOwner )
{
	if ( !pOwner || m_GrapplingTargetName.IsEmpty() )
		return false;

	ioBaseChar* pTarget = pOwner->GetBaseChar( m_GrapplingTargetName );
	if( !pTarget )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwGrapplingStartTime + g_App.GetGrapplingCheckDelay() < dwCurTime && pTarget->GetState() != CS_GRAPPLING_WOUNDED_BUFF )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return false;

	return true;
}

bool ioFlyJumpExtraAttackBuff::IsDropZone( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return false;

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeightOnlyTerrain( vPos.x, vPos.z );
	if ( fMapHeight <= 0.0f )
		return true;

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int ioFlyJumpExtraAttackBuff::GetCurCnt()
{
	float fCurCoolTime = m_fCurExtraGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxExtraGauge ) );

	return (int)fCurCoolTime;
}

int ioFlyJumpExtraAttackBuff::GetMaxCnt()
{
	return (int)m_fMaxExtraGauge;
}

void ioFlyJumpExtraAttackBuff::UpdateExtraCnt( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return;
	
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
		{
			float fGauge = m_fExtraGaugeInc * fTimePerSec;
			m_fCurExtraGauge += fGauge;
			m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
		}
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioFlyJumpExtraAttackBuff::CreateAttachWeapon( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();

	kFireTime.iAttributeIdx = m_nAttachWeapon;
	kFireTime.iResistanceIdx = 0;
	kFireTime.szWoundedAni  = "";
	kFireTime.dwWoundedAniDuration = 0;
	kFireTime.bLoopWoundedAni = false;
	DWORD dwWeaponBaseIdx = pOwner->IncreaseWeaponIndexBase();
	kFireTime.dwWeaponIdx = dwWeaponBaseIdx;
	kFireTime.eFireTimeType = FTT_SKILL_ATTACK;

	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );

	ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		pZone->SetCollisionDir( vDir );
		pZone->SetZoneValue( true, false );
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioFlyJumpExtraAttackBuffSpecialState::ioFlyJumpExtraAttackBuffSpecialState()
{
}

ioFlyJumpExtraAttackBuffSpecialState::~ioFlyJumpExtraAttackBuffSpecialState()
{
}

void ioFlyJumpExtraAttackBuffSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioFlyJumpExtraAttackBuffSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
}

void ioFlyJumpExtraAttackBuffSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	if ( !pOwner )
		return;

	ioBuff *pBuff = pOwner->GetBuff( BT_FLY_JUMP_EXTRA_ATTACK );
	ioFlyJumpExtraAttackBuff *pFlyJumpExtraAttackBuff = ToFlyJumpExtraAttackBuff( pBuff );
	if ( pFlyJumpExtraAttackBuff )
		pFlyJumpExtraAttackBuff->ProcessSpecialState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioFlyJumpExtraAttackBuffSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioFlyJumpExtraAttackBuffSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	if ( !pOwner )
		return false;

	ioBuff *pBuff = pOwner->GetBuff( BT_FLY_JUMP_EXTRA_ATTACK );
	ioFlyJumpExtraAttackBuff *pFlyJumpExtraAttackBuff = ToFlyJumpExtraAttackBuff( pBuff );
	if ( pFlyJumpExtraAttackBuff )
	{
		if ( pFlyJumpExtraAttackBuff->GetSpecialState() == ioFlyJumpExtraAttackBuff::BS_Grap_Jump_Loop ||
			 pFlyJumpExtraAttackBuff->GetSpecialState() == ioFlyJumpExtraAttackBuff::BS_Grap_Land_Attack )
		{
			return false;
		}
	}

	return true;
}