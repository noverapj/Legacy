
#include "stdafx.h"

#include "ioGrapplingBlowLoopSkill2.h"

#include "ioNpcChar.h"
#include "ioPlayStage.h"

#include "WeaponDefine.h"

ioGrapplingBlowLoopSkill2::ioGrapplingBlowLoopSkill2()
{
	m_SkillState = SS_None;
	m_dwCurSkillProtectTime = 0;
	m_dwCurGrapLoopTime = 0;
	m_bReserveGrapLoop = false;
	m_nCurGrapCnt = 0;
	m_bSetLoopMove = false;
}

ioGrapplingBlowLoopSkill2::ioGrapplingBlowLoopSkill2( const ioGrapplingBlowLoopSkill2 &rhs )
	: ioAttackSkill( rhs ),
	  m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	  m_GrapAttack( rhs.m_GrapAttack ),
	  m_szGrapLoopAni( rhs.m_szGrapLoopAni ),
	  m_fGrapLoopAniRate( rhs.m_fGrapLoopAniRate ),
	  m_dwGrapLoopTime( rhs.m_dwGrapLoopTime ),
	  m_fGrapLoopMoveSpeed( rhs.m_fGrapLoopMoveSpeed ),
	  m_GrapEnd( rhs.m_GrapEnd ),
	  m_LastAttack( rhs.m_LastAttack ),
	  m_fCharDefaultHeightRate( rhs.m_fCharDefaultHeightRate ),
	  m_fTargetDefaultGapRange( rhs.m_fTargetDefaultGapRange ),
	  m_fTargetDefaultGapHeight( rhs.m_fTargetDefaultGapHeight ),
	  m_nMaxGrapCnt( rhs.m_nMaxGrapCnt ),
	  m_szLastAttackWoundAni( rhs.m_szLastAttackWoundAni ),
	  m_fLastAttackWoundAniRate( rhs.m_fLastAttackWoundAniRate ),
	  m_szBipedName( rhs.m_szBipedName ),
	  m_vLastGrapOffset( rhs.m_vLastGrapOffset )
{
	m_SkillState = SS_None;
	m_dwCurSkillProtectTime = 0;
	m_dwCurGrapLoopTime = 0;
	m_dwEnableReserveKeyTime = 0;
	m_nCurGrapCnt = 0;
	m_bSetLoopMove = false;
}

ioGrapplingBlowLoopSkill2::~ioGrapplingBlowLoopSkill2()
{
}

void ioGrapplingBlowLoopSkill2::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	LoadAttackAttribute_e( "grap_attack", m_GrapAttack, rkLoader );

	rkLoader.LoadString_e( "grap_loop_ani", "", szBuf, MAX_PATH );
	m_szGrapLoopAni = szBuf;
	m_fGrapLoopAniRate = rkLoader.LoadFloat_e( "grap_loop_ani_rate", FLOAT1 );

	m_dwGrapLoopTime = rkLoader.LoadInt_e( "grap_loop_time", 0 );
	m_fGrapLoopMoveSpeed = rkLoader.LoadFloat_e( "grap_loop_move_speed", 0.0f );

	LoadAttackAttribute_e( "grap_end", m_GrapEnd, rkLoader );
	LoadAttackAttribute_e( "last_attack", m_LastAttack, rkLoader );

	m_fCharDefaultHeightRate = rkLoader.LoadFloat_e( "char_default_height_rate", FLOAT05 );
	m_fTargetDefaultGapRange = rkLoader.LoadFloat_e( "skill_target_default_loop_gap_range", 0.0f );
	m_fTargetDefaultGapHeight = rkLoader.LoadFloat_e( "skill_target_default_loop_gap_height", 0.0f );
	
	m_nMaxGrapCnt = rkLoader.LoadInt_e( "max_extra_grap_cnt", 0 );

	rkLoader.LoadString_e( "last_attack_wound_ani", "", szBuf, MAX_PATH );
	m_szLastAttackWoundAni = szBuf;
	m_fLastAttackWoundAniRate = rkLoader.LoadFloat_e( "last_attack_wound_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "biped_name", "Bip01 L Hand", szBuf, MAX_PATH );
	m_szBipedName = szBuf;

	m_vLastGrapOffset.x = rkLoader.LoadFloat_e( "last_grap_target_offset_x", 0.0f );
	m_vLastGrapOffset.y = rkLoader.LoadFloat_e( "last_grap_target_offset_y", 0.0f );
	m_vLastGrapOffset.z = rkLoader.LoadFloat_e( "last_grap_target_offset_z", 0.0f );

	int nBuffCnt = rkLoader.LoadInt_e( "blow_cancel_buff_cnt", 0 );
	for( int i=0; i < nBuffCnt; ++i )
	{
		wsprintf_e( szKey, "blow_cancel_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_SkillGrapplingInfo.m_SwingCancelBuff.push_back( szBuf );
	}	
}

ioSkill* ioGrapplingBlowLoopSkill2::Clone()
{
	return new ioGrapplingBlowLoopSkill2( *this );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioGrapplingBlowLoopSkill2::OnSkillStart( ioBaseChar *pChar )
{
	m_bSetHandMesh = false;
	m_SkillState = SS_None;

	m_dwCurSkillProtectTime = 0;
	m_dwCurGrapLoopTime = 0;
	m_dwEnableReserveKeyTime = 0;
	m_szGrapTargetName.Clear();
	m_bReserveGrapLoop = false;
	m_nCurGrapCnt = 0;
	m_dwGrapplingStartTime	= 0;
	m_bSetLoopMove = false;

	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = FRAMEGETTIME() + m_dwSkillProtectTime;

	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;
	
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( false );
	
	return true;
}

void ioGrapplingBlowLoopSkill2::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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
				SetGrapAttackState( pChar );
			}
		}
		break;
	case SS_Grap_Attack:
		{
			if ( m_dwMotionEndTime < dwCurTime )
			{
				m_SkillState = SS_End;
				return;
			}
		}
		break;
	case SS_Grap_Loop:
		{
			if ( !CheckTargetState( pChar ) )
			{
				m_SkillState = SS_End;
				return;
			}

			if ( m_dwCurGrapLoopTime < dwCurTime )
			{
				if ( pChar->IsNeedProcess() )
					SetGrapEndState( pChar );

				return;
			}

			if ( m_bSetLoopMove )
				ProcessGrapLoopMove( pChar, pStage );
		}
		break;
	case SS_Grap_End:
		{
			if ( !CheckTargetState( pChar ) )
			{
				m_SkillState = SS_End;
				return;
			}

			if ( m_dwMotionEndTime < dwCurTime )
			{
				if ( m_bReserveGrapLoop && pChar->IsNeedProcess() )
				{
					m_bSetLoopMove = false;
					SetGrapLoopState( pChar );
				}
				else
					SetLastAttackState( pChar );
				return;
			}

			CheckAniRotate( pChar );
			CheckReserveKey( pChar );
		}
		break;
	case SS_Last_Attack:
		{
			if ( !CheckTargetState( pChar ) )
			{
				m_SkillState = SS_End;
				return;
			}

			if ( m_dwMotionEndTime < dwCurTime )
				m_SkillState = SS_End;
		}
		break;
	case SS_End:
		break;
	}
}

void ioGrapplingBlowLoopSkill2::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	m_bSetHandMesh = false;
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioGrapplingBlowLoopSkill2::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar || !pStage ) 
		return;

	int nState;
	rkPacket >> nState;
	
	switch( nState )
	{
	case SS_Grap_Loop:
		{
			rkPacket >> m_bSetLoopMove;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pChar->SetTargetRotAndMoveDirChange( qtRot );

			ioHashString szTargetName;
			rkPacket >> m_szGrapTargetName;

			SetGrapLoopState( pChar );

			ioBaseChar *pTarget = pStage->GetBaseChar( m_szGrapTargetName );
			if( pTarget && pTarget->IsNeedProcess() )
			{
				pTarget->SetTargetRotToRotate( qtRot, true, false );
				pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
			}
		}
		break;
	case SS_Grap_End:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pChar->SetTargetRotAndMoveDirChange( qtRot );

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pChar->SetWorldPosition( vPos );

			SetGrapEndState( pChar );
		}
		break;
	case SS_Grap_Reserve:
		{
			m_bReserveGrapLoop = true;
		}
		break;
	}
}

void ioGrapplingBlowLoopSkill2::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	if( pWeapon && pWeapon->CheckGrapplingState() == GPT_WRESTLING && !m_szGrapTargetName.IsEmpty() )
		pWeapon->SetGrapplingTargetName( m_szGrapTargetName );
	else if( pWeapon && pWeapon->CheckGrapplingState() == GPT_WRESTLING_ALL && !m_szGrapTargetName.IsEmpty() )
		pWeapon->SetGrapplingTargetName( m_szGrapTargetName );
	else if( pWeapon && pWeapon->CheckGrapplingState() == GPT_ATTACK && !m_szGrapTargetName.IsEmpty() )
		pWeapon->SetGrapplingTargetName( m_szGrapTargetName );
	else if( pWeapon && pWeapon->CheckGrapplingState() == GPT_NO_CHANGE3 && !m_szGrapTargetName.IsEmpty() )
		pWeapon->SetGrapplingTargetName( m_szGrapTargetName );
	else if( pWeapon->IsUseGrapplingTarget() && !m_szGrapTargetName.IsEmpty() )
		pWeapon->SetGrapplingTargetName( m_szGrapTargetName );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( false, true );
		}
	}
}

bool ioGrapplingBlowLoopSkill2::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_None:
	case SS_Grap_Attack:
	case SS_Grap_Loop:
	case SS_Grap_End:
	case SS_Last_Attack:
		return false;
	case SS_End:
		if( m_fSkillEndNextJumpAmt == 0.0f )
			return true;
		else
			return false;
	}

	if( m_dwMotionEndTime + dwActionStop >= FRAMEGETTIME() )
		return false;

	return true;
}

bool ioGrapplingBlowLoopSkill2::IsAttackEndState() const
{
	if( m_SkillState == SS_End )
		return true;

	return false;
}

bool ioGrapplingBlowLoopSkill2::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{
		if( iDefenseBreakType == m_ProtectCancelType )   // ½ºÅ³ ¹«Àû ÆÄ±«
			return false;
	}

	DWORD dwCurTime = FRAMEGETTIME();

	if( IsPreDelayProtect() )
	{
		if( m_dwPreDelayStartTime != 0 )
			return true;
	}
	
	if( m_dwPreDelayStartTime == 0 )
	{		
		if( m_dwCurSkillProtectTime > dwCurTime )
			return true;
	}

	return false;
}

bool ioGrapplingBlowLoopSkill2::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioGrapplingBlowLoopSkill2::SetGrapAttackState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();

	int nAniID = pGrp->GetAnimationIdx( m_GrapAttack.m_AttackAnimation );
	if( nAniID == -1 )	
		return;

	float fTimeRate  = m_GrapAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_GrapAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	
	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
	
	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex, nAniID, m_GrapAttack.m_vWeaponInfoList, FTT_SKILL_ATTACK, fTimeRate, dwPreDelay );
	
	pChar->ClearReservedSliding();
	pChar->SetReservedSliding( m_GrapAttack.m_vForceInfoList, nAniID, fTimeRate, dwPreDelay );

	pChar->CheckCharColSkipSkillTime( nAniID, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay;

	DWORD dwTrackingTime = dwCurTime + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( nAniID, "tracking_e" ) * fTimeRate;

	if( IsUseAutoTargetValue() )
	{
		pChar->SetAutoTarget( ATT_SKILL );
		pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	m_SkillState = SS_Grap_Attack;
}

void ioGrapplingBlowLoopSkill2::SetGrapLoopState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();

	int nAniID = pGrp->GetAnimationIdx( m_szGrapLoopAni );
	if( nAniID == -1 )	
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( nAniID, FLOAT100, FLOAT1, FLOAT1/m_fGrapLoopAniRate );

	ioBaseChar* pTarget = pChar->GetBaseChar( m_szGrapTargetName );
	if( pTarget )
	{
		ioEntityGroup *pGrpTarget = pTarget->GetGroup();
		if( pGrpTarget )
		{
			pGrpTarget->ClearAllActionAni( FLOAT100, true );
			pGrpTarget->ClearAllLoopAni( FLOAT100, true );

			int iAniIDTarget = pGrpTarget->GetAnimationIdx( m_SkillGrapplingInfo.m_GrapplingLoopMotion );
			pGrpTarget->SetLoopAni( iAniIDTarget, FLOAT100, FLOAT1, FLOAT1/m_SkillGrapplingInfo.m_fGrapplingLoopMotionRate );
		}
	}

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwCurGrapLoopTime = dwCurTime + m_dwGrapLoopTime;
	m_dwGrapplingStartTime = dwCurTime;
	m_SkillState = SS_Grap_Loop;
	m_bReserveGrapLoop = false;
	m_nCurGrapCnt++;

	if ( pChar->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = ioUserKeyInput::DKI_NONE;
		if ( pChar->IsSettedDirection() )
		{
			eNewDirKey = pChar->CheckCurDirKey();
			pChar->SetTargetRotToDirKey( eNewDirKey );
			pChar->SetWorldOrientation( pChar->GetTargetRot() );
			m_bSetLoopMove = true;
		}

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SS_Grap_Loop;
			kPacket << m_bSetLoopMove;
			kPacket << pChar->GetWorldOrientation();
			kPacket << m_szGrapTargetName;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}	
	}
}

void ioGrapplingBlowLoopSkill2::SetGrapEndState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();

	int nAniID = pGrp->GetAnimationIdx( m_GrapEnd.m_AttackAnimation );
	if( nAniID == -1 )	
		return;

	float fTimeRate  = m_GrapEnd.m_fAttackAniRate;
	DWORD dwPreDelay = m_GrapEnd.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex, nAniID, m_GrapEnd.m_vWeaponInfoList, FTT_SKILL_ATTACK, fTimeRate, dwPreDelay );

	pChar->ClearReservedSliding();
	pChar->SetForcePowerAmt( 0.0f );
	pChar->SetReservedSliding( m_GrapEnd.m_vForceInfoList, nAniID, fTimeRate, dwPreDelay );
	
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay;
	m_dwEnableReserveKeyTime = dwCurTime + pGrp->GetAnimationEventTime_e( nAniID, "key_reserve" ) * fTimeRate;

	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;
	m_vAniRotateInfoList.clear();

	AniEventConstPtrList vAniRotateList;
	pGrp->GetAnimationEventConstPtrList( nAniID, "ani_rotate", vAniRotateList );

	AniEventConstPtrList::iterator iter_r = vAniRotateList.begin();
	while( iter_r != vAniRotateList.end() )
	{
		AniRotateInfo kInfo;

		kInfo.m_dwTime = (*iter_r)->fEventTime * fTimeRate;

		if( kInfo.m_dwTime > 0 )
			kInfo.m_dwTime += dwCurTime;
		else
			kInfo.m_dwTime = 0;

		if( !(*iter_r)->szSubInfo.IsEmpty() )
		{
			const StringVector &vParam = ioStringConverter::Split( (*iter_r)->szSubInfo.c_str(), " " );
			if( vParam.size() == 1 )
			{
				kInfo.m_fAngle = ioStringConverter::ParseFloat( vParam[0].c_str() );
			}
		}

		m_vAniRotateInfoList.push_back(kInfo);

		++iter_r;
	}

	if( !m_vAniRotateInfoList.empty() )
	{
		m_iCurAniRotate = 0;
		m_dwAniRotateTime = m_vAniRotateInfoList[m_iCurAniRotate].m_dwTime;
		m_fAniRotateAngle = m_vAniRotateInfoList[m_iCurAniRotate].m_fAngle;
	}

	m_SkillState = SS_Grap_End;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_Grap_End;
		kPacket << pChar->GetWorldOrientation();
		kPacket << pChar->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGrapplingBlowLoopSkill2::SetLastAttackState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();

	int nAniID = pGrp->GetAnimationIdx( m_LastAttack.m_AttackAnimation );
	if( nAniID == -1 )	
		return;

	float fTimeRate  = m_LastAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_LastAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex, nAniID, m_LastAttack.m_vWeaponInfoList, FTT_SKILL_ATTACK, fTimeRate, dwPreDelay );

	pChar->ClearReservedSliding();
	pChar->SetReservedSliding( m_LastAttack.m_vForceInfoList, nAniID, fTimeRate, dwPreDelay );
	
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay;
	
	m_SkillState = SS_Last_Attack;

	ioBaseChar* pTarget = pChar->GetBaseChar( m_szGrapTargetName );
	if( pTarget )
	{
		ioEntityGroup *pGrpTarget = pTarget->GetGroup();
		if( pGrpTarget )
		{
			pGrpTarget->ClearAllActionAni( FLOAT100, true );
			pGrpTarget->ClearAllLoopAni( FLOAT100, true );

			int iAniIDTarget = pGrpTarget->GetAnimationIdx( m_szLastAttackWoundAni );
			pGrpTarget->SetActionAni( iAniIDTarget, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fLastAttackWoundAniRate, 0.0f, 0 );
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioGrapplingBlowLoopSkill2::SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName )
{
	if( !pChar ) 
		return false;

	if( m_SkillState != SS_Grap_Attack )
		return false;

	m_szGrapTargetName = szTargetName;

	SetGrapLoopState( pChar );
	
	ioBaseChar *pTarget = pChar->GetBaseChar( m_szGrapTargetName );
	if( !pTarget ) 
		return false;

	// for NPC
	bool bNPC = false;
	bool bAttackerNPC = false;
	if( ToNpcChar( pTarget ) )
		bNPC = true;

	if( ToNpcChar( pChar ) )
		bAttackerNPC = true;

	// local or ÈÆ·Ã¼Ò or NPC
	if( !P2PNetwork::IsNetworkPlaying() || !pChar->GetCreator()->IsNetworkMode() || (bNPC || bAttackerNPC) )
	{
		D3DXQUATERNION qtCharRot = pChar->GetTargetRot();
		pTarget->SetTargetRotToRotate( qtCharRot, true, false );
		pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
	}

	return true;
}

D3DXVECTOR3 ioGrapplingBlowLoopSkill2::GetGrapplingTargetPos( ioBaseChar *pChar )
{
	if ( !pChar )
		return ioMath::VEC3_ZERO;

	if ( m_SkillState == SS_Last_Attack )
	{		
		ioEntityGroup* pGroup = pChar->GetGroup();
		if( !pGroup )
			return ioMath::VEC3_ZERO;

		D3DXVECTOR3 vPos = pChar->GetWorldPosition();
		D3DXQUATERNION qtRot = pChar->GetWorldOrientation();

		ioBiped *pHand = pGroup->GetBiped( m_szBipedName );
		if( pHand )
		{
			D3DXVECTOR3 vHandPos = pHand->GetDerivedPosition() * pChar->GetWorldScale();
			vPos += ( qtRot * vHandPos );
		}
		
		vPos += ( qtRot * m_vLastGrapOffset );

		return vPos;
	}
	else
	{
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( m_fCharDefaultHeightRate );
		D3DXVECTOR3 vDir = pChar->GetMoveDir();
		
		vPos += m_fTargetDefaultGapRange * vDir;
		vPos.y += m_fTargetDefaultGapHeight;

		return vPos;
	}

	return ioMath::VEC3_ZERO;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioGrapplingBlowLoopSkill2::ProcessGrapLoopMove( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar || !pStage ) 
		return;
		
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	D3DXVECTOR3 vMoveDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt = vMoveDir * m_fGrapLoopMoveSpeed * fDivideTimePerSec;
		if( vMoveAmt != ioMath::VEC3_ZERO )
		{
			vTotalMove += vMoveAmt;
			bCurMove = true;
		}
	}

	if( bCurMove )
	{
		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( pChar, &vTotalMove, bCol ) )
			pChar->Translate( vTotalMove );

		if ( bCol )
		{
			SetGrapEndState( pChar );
			return;
		}
	}

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	if( pChar->CheckAxisLimit( &vPos ) )
		pChar->SetWorldPosition( vPos );
}

void ioGrapplingBlowLoopSkill2::CheckReserveKey( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() || m_bReserveGrapLoop )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwEnableReserveKeyTime > 0 && m_dwEnableReserveKeyTime < dwCurTime )
	{
		if ( m_nCurGrapCnt <= m_nMaxGrapCnt && pChar->IsAttackKey() )
		{
			m_bReserveGrapLoop = true;

			if ( pChar->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pChar->GetCharName();
				kPacket << GetName();
				kPacket << SS_Grap_Reserve;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

bool ioGrapplingBlowLoopSkill2::CheckTargetState( ioBaseChar *pChar )
{
	if ( !pChar || m_szGrapTargetName.IsEmpty() )
		return false;

	ioBaseChar* pTarget = pChar->GetBaseChar( m_szGrapTargetName );
	if( !pTarget )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwGrapplingStartTime + g_App.GetGrapplingCheckDelay() < dwCurTime && pTarget->GetState() != CS_GRAPPLING_WOUNDED_SKILL )
		return false;
	
	return true;
}