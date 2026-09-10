
#include "StdAfx.h"

#include "ioTargetMoveAttackBuff.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "WeaponDefine.h"
#include "FindPredicateImpl.h"

ioTargetMoveAttackBuff::ioTargetMoveAttackBuff(void)
{
	Init();
}

ioTargetMoveAttackBuff::ioTargetMoveAttackBuff( const ioTargetMoveAttackBuff &rhs )
	: ioBuff( rhs ),
	m_RemoveBuffList( rhs.m_RemoveBuffList ),
	  m_EnableInputKeyState( rhs.m_EnableInputKeyState ),
	  m_BuffEndState( rhs.m_BuffEndState ),
	  m_fEndJumpPower( rhs.m_fEndJumpPower ),
	  m_fMaxMoveDist( rhs.m_fMaxMoveDist ),
	  m_nAttachWeapon( rhs.m_nAttachWeapon ),
	  m_NormalMove( rhs.m_NormalMove ),
	  m_fNormalTargetYAngle( rhs.m_fNormalTargetYAngle ),
	  m_fNormalTargetRange( rhs.m_fNormalTargetRange ),
	  m_fNormalMoveSpeed( rhs.m_fNormalMoveSpeed ),
	  m_fJumpTargetYAngle( rhs.m_fJumpTargetYAngle ),
	  m_fJumpTargetRange( rhs.m_fJumpTargetRange ),
	  m_fJumpMoveSpeed( rhs.m_fJumpMoveSpeed ),
	  m_JumpMove( rhs.m_JumpMove ),
	  m_NormalAttack( rhs.m_NormalAttack ),
	  m_JumpAttack( rhs.m_JumpAttack ),
	  m_bEnableWeaponSkill( rhs.m_bEnableWeaponSkill ),
	  m_bEnableArmorSkill( rhs.m_bEnableArmorSkill ),
	  m_bEnableHelmetSkill( rhs.m_bEnableHelmetSkill ),
	  m_bEnableCloakSkill( rhs.m_bEnableCloakSkill ),
	  m_fUseMoveGauge( rhs.m_fUseMoveGauge ),
	  m_fFloatingGap( rhs.m_fFloatingGap ),
	  m_dwAttackTime( rhs.m_dwAttackTime ),
	  m_fGapHeight( rhs.m_fGapHeight ),
	  m_bTeleportMove( rhs.m_bTeleportMove ),
	  m_vTargetTeleportOffset( rhs.m_vTargetTeleportOffset ),
	  m_vNoTargetTeleportOffset( rhs.m_vNoTargetTeleportOffset )
{
	Init();
}

ioTargetMoveAttackBuff::~ioTargetMoveAttackBuff(void)
{
	m_EnableInputKeyState.clear();
	m_BuffEndState.clear();
}

ioBuff* ioTargetMoveAttackBuff::Clone()
{
	return new ioTargetMoveAttackBuff( *this );
}

void ioTargetMoveAttackBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );
	
	char szBuf[MAX_PATH], szKey[MAX_PATH];
		
	m_EnableInputKeyState.clear();
	int nMaxState = rkLoader.LoadInt_e( "max_enable_input_state", 0 );
	for ( int i=0; i<nMaxState; ++i )
	{
		wsprintf_e( szKey, "enable_input_state%d", i+1 );
		int nState = rkLoader.LoadInt( szKey, -1 );
		m_EnableInputKeyState.push_back( nState );
	}

	m_BuffEndState.clear();
	nMaxState = rkLoader.LoadInt_e( "max_buff_end_state", 0 );
	for ( int i=0; i<nMaxState; ++i )
	{
		wsprintf_e( szBuf, "buff_end_state%d", i+1 );
		int nState = rkLoader.LoadInt( szBuf, -1 );
		m_BuffEndState.push_back( nState );
	}

	m_fNormalTargetYAngle = rkLoader.LoadFloat_e( "normal_target_y_angle", 0.0f );
	m_fNormalTargetRange = rkLoader.LoadFloat_e( "normal_target_range", 0.0f );
	m_fNormalMoveSpeed = rkLoader.LoadFloat_e( "normal_move_speed", 0.0f );

	m_fJumpTargetYAngle = rkLoader.LoadFloat_e( "jump_target_y_angle", 0.0f );
	m_fJumpTargetRange = rkLoader.LoadFloat_e( "jump_target_range", 0.0f );
	m_fJumpMoveSpeed = rkLoader.LoadFloat_e( "jump_move_speed", 0.0f );

	m_fEndJumpPower = rkLoader.LoadFloat_e( "end_jump_power", 0.0f );

	m_fMaxMoveDist = rkLoader.LoadFloat_e( "max_move_dist", 0.0f );
	m_nAttachWeapon = rkLoader.LoadInt_e( "move_attach_weapon", NO_WEAPON_ATTRIBUTE );

	LoadAttackAttribute_e( "normal_move", m_NormalMove, rkLoader );
	LoadAttackAttribute_e( "jump_move", m_JumpMove, rkLoader );

	LoadAttackAttribute_e( "normal_attack", m_NormalAttack, rkLoader );
	LoadAttackAttribute_e( "jump_attack", m_JumpAttack, rkLoader );

	int nCnt = rkLoader.LoadInt_e( "remove_buff_cnt", 0 );
	char szRemoveBuf[MAX_PATH] = "";
	for (int i=0; i<nCnt; ++i)
	{
		wsprintf_e( szRemoveBuf, "remove_buff%d_name",i+1);
		rkLoader.LoadString( szRemoveBuf, "", szBuf, MAX_PATH );
		m_RemoveBuffList.push_back( szBuf );
	}

	m_bEnableWeaponSkill = rkLoader.LoadBool_e( "enable_weapon_skill", false );
	m_bEnableArmorSkill = rkLoader.LoadBool_e( "enable_armor_skill", false );
	m_bEnableHelmetSkill = rkLoader.LoadBool_e( "enable_helmet_skill", false );
	m_bEnableCloakSkill = rkLoader.LoadBool_e( "enable_cloak_skill", false );

	m_fUseMoveGauge = rkLoader.LoadFloat_e( "use_move_gauge", 0.0f );
	m_fFloatingGap = rkLoader.LoadFloat_e( "float_gap", 0.0f );
	m_dwAttackTime = rkLoader.LoadInt_e( "attack_time", 0 );
	m_fGapHeight = rkLoader.LoadFloat_e( "gap_height", 0.0f );

	m_bTeleportMove = rkLoader.LoadBool_e( "teleport_move", false );

	m_vTargetTeleportOffset.x = rkLoader.LoadFloat_e( "target_teleport_offset_x", 0.0f );
	m_vTargetTeleportOffset.y = rkLoader.LoadFloat_e( "target_teleport_offset_y", 0.0f );
	m_vTargetTeleportOffset.z = rkLoader.LoadFloat_e( "target_teleport_offset_z", 0.0f );

	m_vNoTargetTeleportOffset.x = rkLoader.LoadFloat_e( "no_target_teleport_offset_x", 0.0f );
	m_vNoTargetTeleportOffset.y = rkLoader.LoadFloat_e( "no_target_teleport_offset_y", 0.0f );
	m_vNoTargetTeleportOffset.z = rkLoader.LoadFloat_e( "no_target_teleport_offset_z", 0.0f );
}

void ioTargetMoveAttackBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	Init();
}

bool ioTargetMoveAttackBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	Init();

	return true;
}

void ioTargetMoveAttackBuff::ProcessBuff( float fTimePerSec )
{
	if( !m_pOwner || CheckWoundState() || CheckBuffEndState( m_pOwner ) )
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

	switch( m_BuffState )
	{
	case BS_Wait:
		{
			DWORD dwCurTime = FRAMEGETTIME();
			if ( m_pOwner->IsNeedProcess() && CheckEnableKey( m_pOwner ) && 
				m_dwEnableAttackTime + m_dwAttackTime < dwCurTime && m_pOwner->IsAttackKey() )
			{
				m_dwEnableAttackTime = dwCurTime;

				if ( m_bTeleportMove )
					SetTeleportMoveState( m_pOwner );
				else
					SetMoveState( m_pOwner );
			}
		}
		break;
	case BS_Move:
		{
			D3DXVECTOR3 vTargetDir = m_pOwner->GetWorldOrientation() * -ioMath::UNIT_Z;
			if ( !m_szTargetName.IsEmpty() )
			{
				ioBaseChar* pTarget = m_pOwner->GetBaseChar( m_szTargetName );
				if ( pTarget )
				{
					vTargetDir = pTarget->GetMidPositionByRate() - m_pOwner->GetMidPositionByRate();					
					m_pOwner->SetTargetRotToDir( vTargetDir, true );
				}
			}

			D3DXVec3Normalize( &vTargetDir, &vTargetDir );
			if ( m_fCurMoveDist >= m_fMaxMoveDist || ProcessMove( m_pOwner, m_fCurMoveSpeed, vTargetDir ) )
			{
				if ( m_pOwner->IsNeedProcess() )
					SetEndState( m_pOwner );
			}
		}
		break;
	case BS_Action:
		{
			if ( m_dwMotionEndTime > 0 && m_dwMotionEndTime < FRAMEGETTIME() )
			{
				if ( m_bTeleportMove && m_iCurActiveCount <= 0 )
					SetReserveEndBuff();
				else
					SetEndState( m_pOwner );
			}
		}
		break;
	}
}

void ioTargetMoveAttackBuff::EndBuff()
{	
	ioBuff::EndBuff();

	if ( !m_pOwner )	
		return;

	if ( !m_bWound )
	{
		m_pOwner->SetCurMoveSpeed(0.0f);
		m_pOwner->SetJumpPower(0.0f);
		m_pOwner->SetGravityAmt( 0.0f );

		if ( m_pOwner->IsFloating() )
		{
			if ( m_fEndJumpPower > 0.0f )
				m_pOwner->SetSKillEndJumpState( m_fEndJumpPower, false, false );
		}
		else
			m_pOwner->SetState( CS_DELAY );
	}

	for ( int i=0; i<(int)m_RemoveBuffList.size(); ++i )
		m_pOwner->RemoveBuff( m_RemoveBuffList[i], true );
	m_RemoveBuffList.clear();	
}

void ioTargetMoveAttackBuff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	int nType;
	rkPacket >> nType;

	switch ( nType )
	{
	case BS_Move:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pOwner->SetWorldPosition( vPos );

			rkPacket >> m_szTargetName;

			SetMoveState( pOwner );
		}
		break;
	case BS_Telport_Move:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pOwner->SetWorldPosition( vPos );
			
			SetTeleportMoveState( pOwner );
		}
		break;
	case BS_Action:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pOwner->SetWorldPosition( vPos );

			SetActionState( pOwner, false );
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////

void ioTargetMoveAttackBuff::Init()
{
	m_bWound = false;
	m_BuffState = BS_Wait;
	m_dwEnableAttackTime = 0;
	m_iCurActiveCount = m_iMaxActiveCount;
}

bool ioTargetMoveAttackBuff::CheckWoundState()
{
	if( !m_pOwner ) 
		return false;

	CharState eState = m_pOwner->GetState();

	if( m_bEnableWoundCancel )
	{
		switch( eState )
		{
		case CS_BLOW_WOUNDED:
		case CS_BOUND_BLOW_WOUNDED:
		case CS_WOUNDED:
		case CS_DROP_ZONE_DOWN:
			m_bWound = true;
			break;
		}

		if( m_bWound )
			return true;
	}

	return false;
}

bool ioTargetMoveAttackBuff::CheckEnableKey( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	int nOwnerState = (int)pOwner->GetState();
	for ( int i=0; i<(int)m_EnableInputKeyState.size(); ++i )
	{
		if ( nOwnerState == m_EnableInputKeyState[i] )
			return true;
	}

	return false;
}

bool ioTargetMoveAttackBuff::CheckBuffEndState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	int nOwnerState = (int)pOwner->GetState();
	for ( int i=0; i<(int)m_BuffEndState.size(); ++i )
	{
		if ( nOwnerState == m_BuffEndState[i] )
		{
			m_bWound = true;
			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////

void ioTargetMoveAttackBuff::SetMoveState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	AttackAttribute sCurAttack;

	bool bFloat = false;
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeightNoneEntity( vPos.x, vPos.z );
	if ( vPos.y >= fMapHeight + m_fGapHeight )
	{
		sCurAttack = m_JumpMove;
		m_fCurTargetRange = m_fJumpTargetRange;
		m_fCurTargetYAngle = m_fJumpTargetYAngle;
		m_fCurMoveSpeed = m_fJumpMoveSpeed;
		bFloat = true;
	}
	else
	{
		sCurAttack = m_NormalMove;
		m_fCurTargetRange = m_fNormalTargetRange;
		m_fCurTargetYAngle = m_fNormalTargetYAngle;
		m_fCurMoveSpeed = m_fNormalMoveSpeed;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( sCurAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	float fTimeRate  = sCurAttack.m_fAttackAniRate;
	DWORD dwPreDelay = sCurAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );	

	pOwner->ClearReservedSliding();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	
	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = ioUserKeyInput::DKI_NONE;
		if ( pOwner->IsSettedDirection() )
		{
			eNewDirKey = pOwner->CheckCurDirKey();
			pOwner->SetTargetRotToDirKey( eNewDirKey );
			pOwner->SetWorldOrientation( pOwner->GetTargetRot() );
		}

		FindTarget( pOwner, bFloat );
	}
	
	CreateAttachWeapon( pOwner );

	pOwner->SetState( CS_BUFF_ACTION );
	pOwner->ReduceCurSkillGaugeBySkill( m_ItemName, m_fUseMoveGauge );

	m_BuffState = BS_Move;
	m_fCurMoveDist = 0.0f;

	if ( m_pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << m_pOwner->GetCharName();
		kPacket << GetName();
		kPacket << BS_Move;
		kPacket << pOwner->GetWorldOrientation();
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_szTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetMoveAttackBuff::SetTeleportMoveState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;
		
	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = ioUserKeyInput::DKI_NONE;
		if ( pOwner->IsSettedDirection() )
		{
			eNewDirKey = pOwner->CheckCurDirKey();
			pOwner->SetTargetRotToDirKey( eNewDirKey );
			pOwner->SetWorldOrientation( pOwner->GetTargetRot() );
		}

		bool bFloat = false;
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
		float fMapHeight = pStage->GetMapHeightNoneEntity( vPos.x, vPos.z );
		if ( vPos.y >= fMapHeight + m_fGapHeight )
		{
			m_fCurTargetRange = m_fJumpTargetRange;
			m_fCurTargetYAngle = m_fJumpTargetYAngle;
			bFloat = true;
		}
		else
		{
			m_fCurTargetRange = m_fNormalTargetRange;
			m_fCurTargetYAngle = m_fNormalTargetYAngle;
		}

		FindTarget( pOwner, bFloat );

		D3DXVECTOR3 vTargetDir = pOwner->GetWorldOrientation() * -ioMath::UNIT_Z;
		ioBaseChar* pTarget = pOwner->GetBaseChar( m_szTargetName );
		if ( pTarget )
		{
			D3DXVECTOR3 vPos = pTarget->GetWorldPosition() + pTarget->GetWorldOrientation() * m_vTargetTeleportOffset;
			fMapHeight = pStage->GetMapHeightNoneEntity( vPos.x, vPos.z );
			if ( vPos.y <= fMapHeight )
				vPos.y = fMapHeight;

			pOwner->SetWorldPosition( vPos );

			vTargetDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
			D3DXVec3Normalize( &vTargetDir, &vTargetDir );

			pOwner->SetTargetRotToDir( vTargetDir, true );
		}
		else
		{
			D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + pOwner->GetWorldOrientation() * m_vNoTargetTeleportOffset;
			fMapHeight = pStage->GetMapHeightNoneEntity( vPos.x, vPos.z );
			if ( vPos.y <= fMapHeight )
				vPos.y = fMapHeight;

			pOwner->SetWorldPosition( vPos );
		}
	}

	pOwner->SetState( CS_BUFF_ACTION );

	SetActionState( pOwner, false );

	if ( m_pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << m_pOwner->GetCharName();
		kPacket << GetName();
		kPacket << BS_Telport_Move;
		kPacket << pOwner->GetWorldOrientation();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetMoveAttackBuff::SetActionState( ioBaseChar *pOwner, bool bSendNetwork )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	AttackAttribute sCurAttack;
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeightNoneEntity( vPos.x, vPos.z );
	if ( vPos.y >= fMapHeight + m_fGapHeight )
		sCurAttack = m_JumpAttack;
	else
		sCurAttack = m_NormalAttack;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int nAniID = pGrp->GetAnimationIdx( sCurAttack.m_AttackAnimation );
	if( nAniID == -1 )	
		return;

	float fTimeRate  = sCurAttack.m_fAttackAniRate;
	DWORD dwPreDelay = sCurAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->RefreshFireTimeList( nAniID, sCurAttack.m_vWeaponInfoList, FTT_SKILL_ATTACK, fTimeRate, dwPreDelay );

	pOwner->ClearReservedSliding();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay;

	m_BuffState = BS_Action;

	if ( m_bTeleportMove )
	{
		m_iCurActiveCount--;
		m_iCurActiveCount = max( m_iCurActiveCount, 0 );
	}

	if( bSendNetwork && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << BS_Action;
		kPacket << pOwner->GetWorldOrientation();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetMoveAttackBuff::SetEndState( ioBaseChar *pOwner )
{
	m_pOwner->SetCurMoveSpeed(0.0f);
	m_pOwner->SetJumpPower(0.0f);
	m_pOwner->SetGravityAmt( 0.0f );

	if ( m_pOwner->IsFloating() )
	{
		if ( m_fEndJumpPower > 0.0f )
			m_pOwner->SetSKillEndJumpState( m_fEndJumpPower, false, false );
	}
	else
		m_pOwner->SetState( CS_DELAY );

	m_BuffState = BS_Wait;
}

//////////////////////////////////////////////////////////////////////////////////////////////

bool ioTargetMoveAttackBuff::FindTarget( ioBaseChar *pOwner, bool bFloating )
{
	if( !pOwner ) 
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return false;

	BaseCharList vTargetList;
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();

	pOwner->GetTargetCharListBySphere( vPos, m_fCurTargetRange, m_fCurTargetYAngle, vTargetList );

	m_szTargetName.Clear();
	if ( vTargetList.empty() )
		return false;

	SortTargetList( pOwner, vTargetList, m_szTargetName, bFloating );

	if ( m_szTargetName.IsEmpty() )
		return false;

	return true;
}

void ioTargetMoveAttackBuff::SortTargetList( ioBaseChar *pOwner, const BaseCharList &TargetList, ioHashString &szTargetName, bool bFloating )
{
	if( !pOwner )	
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	D3DXQUATERNION qtRot = pOwner->GetWorldOrientation();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	float fMinRangeByTargetDist = m_fCurTargetRange * m_fCurTargetRange;

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

			D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
			float fMapHeight = pStage->GetMapHeightNoneEntity( vTargetPos.x, vTargetPos.z );
			if ( !m_bTeleportMove && bFloating && vTargetPos.y < fMapHeight + m_fFloatingGap )
				continue;
			else if ( !m_bTeleportMove && !bFloating && vTargetPos.y > fMapHeight + m_fFloatingGap )
				continue;

			D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
			float fLengthSq = D3DXVec3LengthSq( &vDiff );
			float fMaxRangeSq = m_fCurTargetRange * m_fCurTargetRange;

			//Range Test
			bool bRangeAvailable = false;
			if( COMPARE( fLengthSq, 0.0f, fMaxRangeSq ) )
				bRangeAvailable = true;
			
			if( bRangeAvailable )
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

void ioTargetMoveAttackBuff::CreateAttachWeapon( ioBaseChar *pOwner )
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

//////////////////////////////////////////////////////////////////////////////////////////////

bool ioTargetMoveAttackBuff::ProcessMove( ioBaseChar *pOwner, float fMoveSpeed, D3DXVECTOR3 vMoveDir )
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
	float fMapHeight = pStage->GetMapHeightNoneEntity( vPos.x, vPos.z );
	if ( vPos.y <= fMapHeight )
		vPos.y = fMapHeight;

	pStage->CheckAxisLimit( &vPos );
	pOwner->SetWorldPosition( vPos );

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////

bool ioTargetMoveAttackBuff::IsCharCollisionSkipState()
{
	switch( m_BuffState )
	{
	case BS_Move:
		return true;
	}

	return false;
}

void ioTargetMoveAttackBuff::ApplyActionState( ioBaseChar *pWoundChar )
{
	if ( !pWoundChar || !m_pOwner )
		return;

	D3DXVECTOR3 vTargetDir = pWoundChar->GetMidPositionByRate() - m_pOwner->GetMidPositionByRate();					
	m_pOwner->SetTargetRotToDir( vTargetDir, true );

	SetActionState( m_pOwner, false );

	if ( pWoundChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << m_pOwner->GetCharName();
		kPacket << GetName();
		kPacket << BS_Action;
		kPacket << m_pOwner->GetWorldOrientation();
		kPacket << m_pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioTargetMoveAttackBuff::CheckEnableUseSkill( int iSlot )
{
	switch( iSlot )
	{
	case ES_WEAPON:
		if( m_bEnableWeaponSkill )
			return true;
		break;
	case ES_ARMOR:
		if( m_bEnableArmorSkill )
			return true;
		break;
	case ES_HELMET:
		if( m_bEnableHelmetSkill )
			return true;
		break;
	case ES_CLOAK:
		if( m_bEnableCloakSkill )
			return true;
		break;
	}

	return false;
}