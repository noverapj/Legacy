#include "stdafx.h"

#include "ioCountTeleportAttackSkill.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioCountTeleportAttackSkill::ioCountTeleportAttackSkill()
{
	m_iCurUseCount = 0;
	m_dwMotionEndTime = 0;
	m_SkillState = SS_None;
	m_bSetHandMesh = false;
	m_dwWeaponIdx = -1;
	m_dwTargetEffectID = -1;
}

ioCountTeleportAttackSkill::ioCountTeleportAttackSkill( const ioCountTeleportAttackSkill &rhs )
	: ioCountSkill( rhs ),
	  m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	  m_FirstNormalAttack( rhs.m_FirstNormalAttack ),
	  m_FirstJumpAttack( rhs.m_FirstJumpAttack ),
	  m_SecondNormalAttack( rhs.m_SecondNormalAttack ),
	  m_SecondJumpAttack( rhs.m_SecondJumpAttack ),
	  m_fTargetAngle( rhs.m_fTargetAngle ),
	  m_fTargetRange( rhs.m_fTargetRange ),
	  m_fJumpHeightGap( rhs.m_fJumpHeightGap ),
	  m_szDummyChar( rhs.m_szDummyChar ),
	  m_fNormalYawAngle( rhs.m_fNormalYawAngle ),
	  m_fNormalPitchAngle( rhs.m_fNormalPitchAngle ),
	  m_fJumpYawAngle( rhs.m_fJumpYawAngle ),
	  m_fJumpPitchAngle( rhs.m_fJumpPitchAngle ),
	  m_szTargetEffect( rhs.m_szTargetEffect ),
	  m_SkillAttackType( rhs.m_SkillAttackType ),
	  m_vTeleportOffset( rhs.m_vTeleportOffset ),
	  m_vTeleportNonTargetOffset( rhs.m_vTeleportNonTargetOffset )
{
	m_iCurUseCount = 0;
	m_dwMotionEndTime = 0;
	m_SkillState = SS_None;
	m_bSetHandMesh = false;
	m_dwWeaponIdx = -1;
	m_dwTargetEffectID = -1;

	m_szTargetName.Clear();
	m_bSetEndJump = false;
	m_fCurYawAngle = 0.0f;
	m_fCurPitchAngle = 0.0f;
}

ioCountTeleportAttackSkill::~ioCountTeleportAttackSkill()
{
}

void ioCountTeleportAttackSkill::LoadProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];

	ioCountSkill::LoadProperty( rkLoader );
			
	m_dwSkillProtectTime = rkLoader.LoadInt_e( "skill_protect_time", 0 );

	LoadAttackAttribute_e( "first_normal_attack", m_FirstNormalAttack, rkLoader );
	LoadAttackAttribute_e( "first_jump_attack", m_FirstJumpAttack, rkLoader );
	LoadAttackAttribute_e( "second_normal_attack", m_SecondNormalAttack, rkLoader );
	LoadAttackAttribute_e( "second_jump_attack", m_SecondJumpAttack, rkLoader );

	m_fTargetAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fTargetRange = rkLoader.LoadFloat_e( "target_range", 0.0f );

	m_fJumpHeightGap = rkLoader.LoadFloat_e( "jump_height_gap", 0.0f );

	rkLoader.LoadString_e( "dummy_name", "", szBuf, MAX_PATH );
	m_szDummyChar = szBuf;

	rkLoader.LoadString_e( "target_effect", "", szBuf, MAX_PATH );
	m_szTargetEffect = szBuf;

	m_fNormalYawAngle = rkLoader.LoadFloat_e( "first_normal_yaw", 0.0f );
	m_fNormalPitchAngle = rkLoader.LoadFloat_e( "first_normal_pitch", 0.0f );
	m_fJumpYawAngle = rkLoader.LoadFloat_e( "first_jump_yaw", 0.0f );
	m_fJumpPitchAngle = rkLoader.LoadFloat_e( "first_jump_pitch", 0.0f );

	m_SkillAttackType = (SkillAttackType)rkLoader.LoadInt_e( "skill_attack_type", SAT_NONE );

	m_vTeleportOffset.x = rkLoader.LoadFloat_e( "teleport_offset_x", 0.0f );
	m_vTeleportOffset.y = rkLoader.LoadFloat_e( "teleport_offset_y", 0.0f );
	m_vTeleportOffset.z = rkLoader.LoadFloat_e( "teleport_offset_z", 0.0f );

	m_vTeleportNonTargetOffset.x = rkLoader.LoadFloat_e( "teleport_none_target_offset_x", 0.0f );
	m_vTeleportNonTargetOffset.y = rkLoader.LoadFloat_e( "teleport_none_target_offset_y", 0.0f );
	m_vTeleportNonTargetOffset.z = rkLoader.LoadFloat_e( "teleport_none_target_offset_z", 0.0f );
}

ioSkill* ioCountTeleportAttackSkill::Clone()
{
	return new ioCountTeleportAttackSkill( *this );
}

//////////////////////////////////////////////////////////////////////////////////////

bool ioCountTeleportAttackSkill::IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg )
{
	ioAttackableItem *pAttackable = ToAttackableItem( GetOwnerItem() );
	if( pAttackable )
	{
		if( !pAttackable->IsCanFire( pChar ) )
			return false;
	}

	if( m_iCurUseCount < m_iNeedCount )
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////

bool ioCountTeleportAttackSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioCountSkill::OnSkillStart(pChar) )
		return false;

	m_SkillState = SS_None;
	m_dwCurSkillProtectTime = 0;
	m_dwMotionEndTime = 0;
	m_bSetHandMesh = false;
	m_szTargetName.Clear();
	m_bSetEndJump = false;
	m_fCurYawAngle = 0.0f;
	m_fCurPitchAngle = 0.0f;
	m_dwTargetEffectID = -1;

	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( false );

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
	
	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = FRAMEGETTIME() + m_dwSkillProtectTime;
	
	return true;
}

void ioCountTeleportAttackSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_None:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );

				CheckSkillState( pChar );
			}
		}
		break;
	case SS_Action:
		{
			if ( m_dwMotionEndTime < dwCurTime )
				m_SkillState = SS_End;
		}
		break;
	}
}

void ioCountTeleportAttackSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioCountSkill::OnSkillEnd( pChar );
	
	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );
	
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	if ( !m_szTargetName.IsEmpty() && m_dwTargetEffectID != -1 )
	{
		ioBaseChar *pTarget = pChar->GetBaseChar( m_szTargetName );
		if ( pTarget )
			pTarget->EndEffect( m_dwTargetEffectID, false );
	}
}

//////////////////////////////////////////////////////////////////////////////////////

void ioCountTeleportAttackSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( true, true );
		}
	}

	ioMissileWeapon *pMissileWeapon = ToMissileWeapon( pWeapon );
	if ( pMissileWeapon )
	{
		m_dwWeaponIdx = pMissileWeapon->GetWeaponIndex();

		ioBaseChar *pChar = pMissileWeapon->GetOwner();
		if ( !pChar )
			return;

		if ( !m_szTargetName.IsEmpty() )
		{
			ioBaseChar *pTarget = pChar->GetBaseChar( m_szTargetName );
			if ( pTarget )
			{
				D3DXVECTOR3 vMoveDir = pTarget->GetMidPositionByRate() - pChar->GetMidPositionByRate();
				D3DXVec3Normalize( &vMoveDir, &vMoveDir );
				pMissileWeapon->SetMoveDir( vMoveDir );
				return;
			}
		}

		float fCurYawAngle = m_fCurYawAngle;
		float fCurPitchAngle = m_fCurPitchAngle;
		
		fCurYawAngle	= ioMath::ArrangeHead( fCurYawAngle );
		fCurPitchAngle	= ioMath::ArrangeHead( fCurPitchAngle );

		D3DXQUATERNION qtRotate = pWeapon->GetOwner()->GetTargetRot();
		D3DXVECTOR3 vMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

		D3DXQUATERNION qtRot;
		D3DXVECTOR3 vAxis;
		D3DXVec3Cross( &vAxis, &vMoveDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

		// vertical
		D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fCurPitchAngle) );
		vMoveDir = qtRot * vMoveDir;

		// horizon
		D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fCurYawAngle) );
		vMoveDir = qtRot * vMoveDir;	
		pMissileWeapon->SetMoveDir( vMoveDir );
	}
}

void ioCountTeleportAttackSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar || !pStage )
		return;

	int nState;
	rkPacket >> nState;

	switch ( (SkillState)nState )
	{
	case SS_First_Attack:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pChar->SetTargetRotAndMoveDirChange( qtRot );

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pChar->SetWorldPosition( vPos );

			rkPacket >> m_szTargetName;

			ioBaseChar *pTarget = pChar->GetBaseChar( m_szTargetName );
			if ( pTarget )
			{
				ioEffect *pEffect = pTarget->AttachEffect( m_szTargetEffect );
				if ( pEffect )
					m_dwTargetEffectID = pEffect->GetUniqueID();
			}

			if ( m_SkillUseType == SUT_NORMAL )
			{
				m_CurAttack = m_FirstNormalAttack;
				m_bSetEndJump = false;
				m_fCurYawAngle = m_fNormalYawAngle;
				m_fCurPitchAngle = m_fNormalPitchAngle;
			}
			else if ( m_SkillUseType == SUT_JUMP )
			{
				m_CurAttack = m_FirstJumpAttack;
				m_bSetEndJump = true;
				m_fCurYawAngle = m_fJumpYawAngle;
				m_fCurPitchAngle = m_fJumpPitchAngle;
			}

			SetActionState( pChar );
		}
		break;
	case SS_Second_Attack_Weapon:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwWeaponIdx );
			if ( pWeapon && pWeapon->IsLive() )
				pWeapon->SetWeaponDead();

			SetTeleportState( pChar, vPos, qtRot );
			SetActionState( pChar );		
		}
		break;
	case SS_Second_Attack_Dummy:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			ioDummyChar *pDummy = g_DummyCharMgr.FindDummyCharToName( pChar, m_szDummyChar );
			if ( pDummy )
				pDummy->SetDieState( true );

			SetTeleportState( pChar, vPos, qtRot );
			SetActionState( pChar );	
		}
		break;
	case SS_Second_Attack_Type01:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			SetTeleportState( pChar, vPos, qtRot );
			SetActionState( pChar );
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////

bool ioCountTeleportAttackSkill::IsProtected( int iDefenseBreakType ) const
{
	if( IsPreDelayProtect() )
	{
		if( m_dwPreDelayStartTime != 0 )
			return true;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwCurSkillProtectTime > dwCurTime )
		return true;

	return false;
}

bool ioCountTeleportAttackSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if ( m_SkillState == SS_End )
	{
		if( !m_bSetEndJump )
			return true;
	}

	return false;
}

bool ioCountTeleportAttackSkill::IsAttackEndState() const
{
	if( m_SkillState == SS_End )
		return true;

	return false;
}

bool ioCountTeleportAttackSkill::IsNoDropState() const
{	
	if( m_SkillState == SS_Action )
		return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////

int ioCountTeleportAttackSkill::GetCurUseCount() 
{ 
	return m_iCurUseCount; 
}

int ioCountTeleportAttackSkill::GetMaxUseCount()
{ 
	return m_iMaxUseCount;
}

int ioCountTeleportAttackSkill::GetNeedCount()
{
	return m_iNeedCount;
}

//////////////////////////////////////////////////////////////////////////////////////

void ioCountTeleportAttackSkill::CheckSkillState( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return;

	if ( SecondAttackStateByDummy( pChar ) )
		return;

	if ( SecondAttackStateByWeapon( pChar ) )
		return;

	if ( SecondAttackStateByType( pChar ) )
		return;
	
	FirstAttackState( pChar );
}

void ioCountTeleportAttackSkill::FirstAttackState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	if ( m_SkillUseType == SUT_NORMAL )
	{
		m_CurAttack = m_FirstNormalAttack;
		m_bSetEndJump = false;
		m_fCurYawAngle = m_fNormalYawAngle;
		m_fCurPitchAngle = m_fNormalPitchAngle;
	}
	else if ( m_SkillUseType == SUT_JUMP )
	{
		m_CurAttack = m_FirstJumpAttack;
		m_bSetEndJump = true;
		m_fCurYawAngle = m_fJumpYawAngle;
		m_fCurPitchAngle = m_fJumpPitchAngle;
	}

	FindTarget( pChar );
	SetActionState( pChar );
		
	if ( !m_szTargetName.IsEmpty() )
	{
		ioBaseChar *pTarget = pChar->GetBaseChar( m_szTargetName );
		if ( pTarget )
		{
			D3DXVECTOR3 vTargetDir = pTarget->GetMidPositionByRate() - pChar->GetMidPositionByRate();
			D3DXVec3Normalize( &vTargetDir, &vTargetDir );
			pChar->SetTargetRotToDir( vTargetDir, true );

			ioEffect *pEffect = pTarget->AttachEffect( m_szTargetEffect );
			if ( pEffect )
				m_dwTargetEffectID = pEffect->GetUniqueID();
		}
	}

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_First_Attack;
		kPacket << pChar->GetWorldOrientation();
		kPacket << pChar->GetWorldPosition();
		kPacket << m_szTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioCountTeleportAttackSkill::SecondAttackStateByWeapon( ioBaseChar *pChar )
{
	if ( !pChar )
		return false;

	if ( m_dwWeaponIdx == -1 )
		return false;

	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwWeaponIdx );
	if ( pWeapon && pWeapon->IsLive() )
	{
		pWeapon->SetWeaponDead();

		SetTeleportState( pChar, pWeapon->GetPosition(), pWeapon->GetOrientation() );
		SetActionState( pChar );		

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SS_Second_Attack_Weapon;
			kPacket << pWeapon->GetOrientation();
			kPacket << pWeapon->GetPosition();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		return true;
	}

	m_dwWeaponIdx = -1;
	return false;
}

bool ioCountTeleportAttackSkill::SecondAttackStateByDummy( ioBaseChar *pChar )
{
	if ( !pChar )
		return false;

	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyCharToName( pChar, m_szDummyChar );
	if ( pDummy )
	{
		pDummy->SetDieState( true );

		SetTeleportState( pChar, pDummy->GetWorldPosition(), pDummy->GetWorldOrientation() );
		SetActionState( pChar );		

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SS_Second_Attack_Dummy;
			kPacket << pDummy->GetWorldOrientation();
			kPacket << pDummy->GetWorldPosition();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////

void ioCountTeleportAttackSkill::SetActionState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( m_CurAttack.m_AttackAnimation );
	if ( iAniID == -1)
		return;

	float fTimeRate  = m_CurAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_CurAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0, true );	

	pChar->ClearReservedSliding();
	pChar->SetReservedSliding( m_CurAttack.m_vForceInfoList, iAniID, fTimeRate, 0 );

	pChar->RefreshFireTimeList( iAniID, m_CurAttack.m_vWeaponInfoList, FTT_SKILL_ATTACK, fTimeRate, 0 );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	
	m_SkillState = SS_Action;	
}

void ioCountTeleportAttackSkill::SetTeleportState( ioBaseChar *pChar, D3DXVECTOR3 vPos, D3DXQUATERNION qtRot )
{
	if ( !pChar )
		return;

	ioPlayStage *pStage = pChar->GetCreator();
	if ( !pStage )
		return;

	D3DXVECTOR3 vMoveDir = qtRot * -ioMath::UNIT_Z;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );
	pChar->SetTargetRotToDir( vMoveDir, true );

	D3DXVECTOR3 vNewPos = vPos;
	pStage->CheckFlyMapLimit( &vNewPos );
	pStage->CheckAxisLimit( &vNewPos );

	float fMapHeight = pStage->GetMapHeightNoneEntity( vNewPos.x, vNewPos.z );
	if ( fMapHeight > vNewPos.y )
		vNewPos.y = fMapHeight;

	if ( fabs( fMapHeight - vNewPos.y ) >= m_fJumpHeightGap )
	{
		m_CurAttack = m_SecondJumpAttack;
		m_bSetEndJump = true;
	}
	else
	{
		m_CurAttack = m_SecondNormalAttack;
		m_bSetEndJump = false;
		vNewPos.y = fMapHeight;
	}

	pChar->SetWorldPosition( vNewPos );

	if( pChar->IsNeedProcess() && pChar->IsOwnerChar() )
		pStage->SetCameraToOwnerExplicit( pChar );
}

//////////////////////////////////////////////////////////////////////////////////////

bool ioCountTeleportAttackSkill::FindTarget( ioBaseChar *pChar )
{
	if( !pChar ) 
		return false;

	ioPlayStage *pStage = pChar->GetCreator();
	if( !pStage ) 
		return false;

	BaseCharList vTargetList;
	D3DXVECTOR3 vPos = pChar->GetWorldPosition();

	pChar->GetTargetCharListBySphere( vPos, m_fTargetRange, m_fTargetAngle, vTargetList );

	m_szTargetName.Clear();
	if ( vTargetList.empty() )
		return false;

	SortTargetList( pChar, vTargetList, m_szTargetName );

	if ( m_szTargetName.IsEmpty() )
		return false;

	return true;
}

void ioCountTeleportAttackSkill::SortTargetList( ioBaseChar *pChar, const BaseCharList &TargetList, ioHashString &szTargetName )
{
	if( !pChar )	
		return;

	D3DXQUATERNION qtRot = pChar->GetWorldOrientation();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	float fMinRangeByTargetDist = m_fTargetRange * m_fTargetRange;

	BaseCharList::const_iterator iter;
	for( iter=TargetList.begin(); iter!=TargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( pTarget != pChar )
		{
			//팀이 같거나 타겟이 될수 없으면 리스트에 저장 안함
			if ( pChar->GetTeam() == pTarget->GetTeam() )
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

			D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate() - pChar->GetMidPositionByRate();
			float fLengthSq = D3DXVec3LengthSq( &vDiff );
			float fMaxRangeSq = m_fTargetRange * m_fTargetRange;

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

bool ioCountTeleportAttackSkill::SecondAttackStateByType( ioBaseChar *pChar )
{
	if ( !pChar )
		return false;

	if ( m_SkillAttackType == SAT_FIND_TARGET )
	{
		D3DXQUATERNION qtRot;
		D3DXVECTOR3 vPos;
		if ( FindAttackTarget( pChar ) )
		{
			ioBaseChar *pTarget = pChar->GetBaseChar( m_szTargetName );
			if ( !pTarget )
				return false;

			vPos = pTarget->GetWorldPosition() + pTarget->GetWorldOrientation() * m_vTeleportOffset;
			D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - vPos;
			vDiff.y = 0.0f;
			D3DXVec3Normalize( &vDiff, &vDiff );
			qtRot = Help::ConvertDirToQuaternion( vDiff );
		}
		else
		{
			vPos = pChar->GetWorldPosition() + pChar->GetWorldOrientation() * m_vTeleportNonTargetOffset;
			qtRot = pChar->GetWorldOrientation();
		}

		SetTeleportState( pChar, vPos, qtRot );
		SetActionState( pChar );

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SS_Second_Attack_Type01;
			kPacket << qtRot;
			kPacket << vPos;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		return true;
	}

	return false;
}

bool ioCountTeleportAttackSkill::FindAttackTarget( ioBaseChar *pChar )
{
	if( !pChar ) 
		return false;

	ioPlayStage *pStage = pChar->GetCreator();
	if( !pStage ) 
		return false;

	BaseCharList vTargetList;
	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	bool bLand = false;
	//지상이라면
	if ( vPos.y == pChar->GetBottomHeight() )
		bLand = true;


	pChar->GetTargetCharListBySphere( vPos, m_fTargetRange, m_fTargetAngle, vTargetList );

	m_szTargetName.Clear();
	if ( vTargetList.empty() )
		return false;

	D3DXQUATERNION qtRot = pChar->GetWorldOrientation();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	
	float fMaxRangeSq = m_fTargetRange * m_fTargetRange;
	float fLandMinRange = fMaxRangeSq;
	float fAirMinRange = fMaxRangeSq;

	ioHashString szLandTarget;
	ioHashString szAirTarget;

	BaseCharList::const_iterator iter;
	for( iter=vTargetList.begin(); iter!=vTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( pTarget != pChar )
		{
			//팀이 같거나 타겟이 될수 없으면 리스트에 저장 안함
			if ( pChar->GetTeam() == pTarget->GetTeam() )
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

			D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate() - pChar->GetMidPositionByRate();
			float fLengthSq = D3DXVec3LengthSq( &vDiff );
			bool bTargetLand = false;
			if ( pTarget->GetWorldPosition().y == pTarget->GetBottomHeight() )
				bTargetLand = true;

			//Range Check
			bool bRangeAvailable = false;
			if( COMPARE( fLengthSq, 0.0f, fMaxRangeSq ) )
				bRangeAvailable = true;

			if( bRangeAvailable )
			{	
				if ( bTargetLand )
				{
					if ( fLandMinRange > fLengthSq )
					{
						fLandMinRange = fLengthSq;
						szLandTarget = pTarget->GetCharName();
					}
				}
				else
				{
					if ( fAirMinRange > fLengthSq )
					{
						fAirMinRange = fLengthSq;
						szAirTarget = pTarget->GetCharName();
					}
				}
			}
		}
	}	

	if ( bLand )
	{
		if ( !szLandTarget.IsEmpty() )
			m_szTargetName = szLandTarget;
		else
			m_szTargetName = szAirTarget;
	}
	else
	{
		if ( !szAirTarget.IsEmpty() )
			m_szTargetName = szAirTarget;
		else
			m_szTargetName = szLandTarget;

	}

	if ( m_szTargetName.IsEmpty() )
		return false;

	return true;
}
