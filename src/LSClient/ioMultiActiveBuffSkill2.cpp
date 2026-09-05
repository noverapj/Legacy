
#include "stdafx.h"

#include "ioMultiActiveBuffSkill2.h"

#include "ioPlayStage.h"
#include "ioNpcChar.h"

#include "WeaponDefine.h"

ioMultiActiveBuffSkill2::ioMultiActiveBuffSkill2()
{
	m_bSetHandMesh = false;
}

ioMultiActiveBuffSkill2::ioMultiActiveBuffSkill2( const ioMultiActiveBuffSkill2 &rhs )
	: ioBuffSkill( rhs ),
	m_szStateCheckBuff( rhs.m_szStateCheckBuff ),
	m_szTargetCheckBuff( rhs.m_szTargetCheckBuff ),
	m_MarkAttack( rhs.m_MarkAttack ),
	m_GrapAttack( rhs.m_GrapAttack ),
	m_SwingAttack( rhs.m_SwingAttack ),
	m_szMoveAni( rhs.m_szMoveAni ),
	m_szGrapAni( rhs.m_szGrapAni ),
	m_fMoveSpeed( rhs.m_fMoveSpeed ),
	m_fArriveDist( rhs.m_fArriveDist ),
	m_fCharHeightRate( rhs.m_fCharHeightRate ),
	m_fGrapTargetRange( rhs.m_fGrapTargetRange ),
	m_dwGrapCheckTime( rhs.m_dwGrapCheckTime ),
	m_fCheckDist( rhs.m_fCheckDist ),
	m_fNeedGrapGauge( rhs.m_fNeedGrapGauge ),
	m_fNeedSwingGauge( rhs.m_fNeedSwingGauge )
{
	m_ActiveState = ASS_NONE;
}

ioMultiActiveBuffSkill2::~ioMultiActiveBuffSkill2()
{
}

void ioMultiActiveBuffSkill2::LoadProperty( ioINILoader &rkLoader )
{
	ioBuffSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	rkLoader.LoadString_e( "state_check_buff", "", szBuf, MAX_PATH );
	m_szStateCheckBuff = szBuf;

	rkLoader.LoadString_e( "target_check_buff", "", szBuf, MAX_PATH );
	m_szTargetCheckBuff = szBuf;

	LoadAttackAttribute_e( "mark_attack", m_MarkAttack, rkLoader );
	LoadAttackAttribute_e( "grap_attack", m_GrapAttack, rkLoader );
	LoadAttackAttribute_e( "swing_attack", m_SwingAttack, rkLoader );

	rkLoader.LoadString_e( "move_ani", "", szBuf, MAX_PATH );
	m_szMoveAni = szBuf;

	rkLoader.LoadString_e( "grap_ani", "", szBuf, MAX_PATH );
	m_szGrapAni = szBuf;
	
	m_fMoveSpeed = rkLoader.LoadFloat_e( "move_speed", 0.0f );
	m_fArriveDist = rkLoader.LoadFloat_e( "arrive_dist", 10000.0f );
	m_fCharHeightRate = rkLoader.LoadFloat_e( "char_height_rate", FLOAT05 );
	m_fGrapTargetRange = rkLoader.LoadFloat_e( "grap_range", 0.0f );
	m_dwGrapCheckTime = rkLoader.LoadInt_e( "grap_check_time", 0 );
	m_fCheckDist = rkLoader.LoadFloat_e( "check_dist", 0.0f );
	m_fNeedGrapGauge = rkLoader.LoadFloat_e( "need_grap_gauge", 0.0f );
	m_fNeedSwingGauge = rkLoader.LoadFloat_e( "need_swing_gauge", 0.0f );

	int nBuffCnt = rkLoader.LoadInt_e( "swing_buff_cnt", 0 );
	for( int i=0; i<nBuffCnt; ++i )
	{
		wsprintf_e( szKey, "swing_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_SkillGrapplingInfo.m_SwingBuff.push_back( szBuf );
	}

	nBuffCnt = rkLoader.LoadInt_e( "swing_wound_buff_cnt", 0 );
	for( int i=0; i<nBuffCnt; ++i )
	{
		wsprintf_e( szKey, "swing_wound_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_SkillGrapplingInfo.m_SwingWoundBuff.push_back( szBuf );
	}

	nBuffCnt = rkLoader.LoadInt_e( "swing_cancel_buff_cnt", 0 );
	for( int i=0; i<nBuffCnt; ++i )
	{
		wsprintf_e( szKey, "swing_cancel_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_SkillGrapplingInfo.m_SwingCancelBuff.push_back( szBuf );
	}
}

ioSkill* ioMultiActiveBuffSkill2::Clone()
{
	return new ioMultiActiveBuffSkill2( *this );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioMultiActiveBuffSkill2::OnSkillStart( ioBaseChar *pChar )
{
	if( !HasSkillMotion() )
		return false;

	if( !ioSkill::OnSkillStart( pChar ) )
		return false;

	ioPlayStage *pStage = pChar->GetCreator();
	if ( !pStage )
		return false;

	m_dwAttackEndTime = 0;
	m_dwPreDelayStartTime = 0;
	m_dwFireStartTime = 0;
	m_dwFireMotionEndTime = 0;
	m_bSetHandMesh = false;
	m_bRemoveEquipSkillBuff = false;

	m_SkillState = SS_NONE;

	m_szTargetName.Clear();
	m_szGrapTargetName.Clear();

	m_dwGrapStartTime = 0;

	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );

		// 사용 아이템 처리
		if( !m_HandMesh.IsEmpty() )
		{
			pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
			m_bSetHandMesh = true;
		}
	}	

	if( !StartPreDelayAnimation( pChar ) )
		BuffSkillSet( pChar );
	else
	{
		DWORD dwCurTime = FRAMEGETTIME();
		DWORD dwTrackingTime = 0;
		ioEntityGroup *pGrp = pChar->GetGroup();
		if( pGrp )
		{
			ioHashString szPreDelay = GetPreDelayAniName();

			int iAniID = pGrp->GetAnimationIdx( szPreDelay );
			if( iAniID != -1 )
				dwTrackingTime = pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" );
		}

		DWORD dwPreDelay = GetPreDelayTime();

		if( dwTrackingTime > 0 )
			dwTrackingTime += dwCurTime;
		else
			dwTrackingTime = dwCurTime + dwPreDelay;

		if( IsUseAutoTargetValue() )
		{
			pChar->SetAutoTarget( ATT_SKILL );
			pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
	}

	if( m_bSkillMoveEnableDelayTime )
		pChar->SetSkillMoveEnable( true, true );

	if( m_SkillActivateEffectID != -1 )
	{
		pChar->EndEffect( m_SkillActivateEffectID, false );
		m_SkillActivateEffectID = -1;
	}

	//////////////////////////////////////////////////////////////////////////

	if ( IsMarkBuff( pChar ) )
	{
		ioBaseChar *pTargetChar = pChar->GetBaseChar( m_szTargetName );
		if ( pTargetChar )
		{
			D3DXVECTOR3 vTargetPos = pTargetChar->GetWorldPosition();
			float fMapHeight = pStage->GetMapHeightOnlyTerrain( vTargetPos.x, vTargetPos.z );
			if ( fMapHeight <= 0.0f || vTargetPos.y < fMapHeight )
				m_ActiveState = ASS_Target_Drop;
			else
				m_ActiveState = ASS_Check_Mark;
		}
		else 
			return false;
	}
	else 
	{
		if( !pChar->HasBuff( m_szStateCheckBuff ) && !m_bReduceGauge )
		{
			pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
			m_bReduceGauge = true;
		}
		m_ActiveState = ASS_No_Mark;
	}

	m_SkillState = SS_NONE;
	return true;
}

void ioMultiActiveBuffSkill2::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ActiveState )
	{
	case ASS_Target_Drop:
		{
			DWORD dwPreDelayTime = GetPreDelayTime();
			if( m_dwPreDelayStartTime > 0 && m_dwPreDelayStartTime + dwPreDelayTime < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );
				SetEndState( pChar );
			}
		}
		break;
	case ASS_No_Mark:
		{
			DWORD dwPreDelayTime = GetPreDelayTime();
			if( m_dwPreDelayStartTime > 0 && m_dwPreDelayStartTime + dwPreDelayTime < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );

				AddOwnerBuff( pChar );
				
				SetMarkAttackState( pChar );
			}
		}
		break;
	case ASS_Mark_Attack:
		{
			if( m_dwMotionEndTime < dwCurTime )
				SetEndState( pChar );
		}
		break;
	case ASS_Check_Mark:
		{
			DWORD dwPreDelayTime = GetPreDelayTime();
			if( m_dwPreDelayStartTime > 0 && m_dwPreDelayStartTime + dwPreDelayTime < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );
				
				SetMoveState( pChar );
			}
		}
		break;
	case ASS_Move:
		{
			if ( IsArrived( pChar, pStage ) )
			{	
				ioBaseChar *pTarget = pChar->GetBaseChar( m_szTargetName );
				if( !pTarget )
				{
					SetEndState( pChar );
					return;
				}

				D3DXVECTOR3 vDir = pTarget->GetWorldPosition() - pChar->GetWorldPosition();
				pChar->SetTargetRotToDir( vDir, true );
				
				SetGrapAttackState( pChar );
				return;
			}

			CheckMove( pChar, pStage );			
		}
		break;
	case ASS_Grap_Attack:
		{
			if( m_dwMotionEndTime < dwCurTime )
				SetEndState( pChar );
		}
		break;
	case ASS_Grap_Loop:
		{
			ioBaseChar *pTarget = pStage->GetBaseChar( m_szGrapTargetName );
			if( pTarget )
			{
				if( m_dwGrapStartTime + g_App.GetGrapplingCheckDelay() < dwCurTime &&
					pTarget->GetState() != CS_GRAPPLING_WOUNDED_SKILL )
				{
					SetEndState( pChar );
					return;
				}
			}
			else
			{
				SetEndState( pChar );
				return;
			}

			if ( pChar->IsNeedProcess() && m_dwGrapStartTime + m_dwGrapCheckTime < dwCurTime )
			{
				SetSwingState( pChar, pStage );
				return;
			}
			
			CheckInputKey( pChar, pStage );
		}
		break;
	case ASS_Swing:
		{
			if( m_dwMotionEndTime < dwCurTime )
				SetEndState( pChar );
		}
		break;
	case ASS_End:
		break;
	}
}

void ioMultiActiveBuffSkill2::OnSkillEnd( ioBaseChar *pChar )
{
	if ( m_ActiveState == ASS_Grap_Loop && !m_szGrapTargetName.IsEmpty() )
	{
		ioBaseChar *pTarget = pChar->GetBaseChar( m_szGrapTargetName );
		if( pTarget && pTarget->GetState() == CS_GRAPPLING_WOUNDED_SKILL )
			pTarget->SetState( CS_DELAY );
	}

	// 기본무기 장착
	if( m_bUseItemMesh && IsCanShowWeaponItemMesh( pChar ) )
		pChar->ShowWeaponItemMesh( true );

	if( m_bSetHandMesh && !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_bSetHandMesh = false;

	pChar->EndEffect( m_SkillEffectName );

	ioSkill::OnSkillEnd( pChar );
}

void ioMultiActiveBuffSkill2::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	if ( pWeapon && !m_szGrapTargetName.IsEmpty() )
	{
		GrapplingType eGT = pWeapon->CheckGrapplingState();
		switch( eGT )
		{
		case GPT_WRESTLING:
		case GPT_WRESTLING_ALL:
		case GPT_NO_CHANGE:
		case GPT_NO_CHANGE2:
		case GPT_EXCEPT:
			pWeapon->SetGrapplingTargetName( m_szGrapTargetName );
			break;
		}
	}

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

void ioMultiActiveBuffSkill2::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar || !pStage )
		return;

	int nState;
	rkPacket >> nState;
	
	switch( nState )
	{
	case ASS_Grap_Loop:
		{
			rkPacket >> m_szGrapTargetName;

			D3DXVECTOR3 vNewPos;
			rkPacket >> vNewPos;
			D3DXQUATERNION qtNewRot;
			rkPacket >> qtNewRot;

			pChar->SetWorldPosition( vNewPos );
			pChar->SetTargetRotToRotate( qtNewRot, true );

			SetGrapLoopState( pChar );

			ioBaseChar *pTarget = pStage->GetBaseChar( m_szGrapTargetName );
			if( pTarget && pTarget->IsNeedProcess() )
			{
				pTarget->SetTargetRotToRotate( pChar->GetTargetRot(), true );
				pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
			}
		}
		break;
	case ASS_Swing:
		{
			ioBaseChar *pTarget = pStage->GetBaseChar( m_szGrapTargetName );
			if ( !pTarget )
			{
				SetEndState( pChar );
				return;
			}
			
			SetSwingState( pChar, pStage );

			if( pTarget->IsNeedProcess() && pTarget->GetState() == CS_GRAPPLING_WOUNDED_SKILL )
			{
				pTarget->SetTargetRotToRotate( pChar->GetTargetRot(), true );
				pTarget->SetGrapplingWoundSwing( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), "", FLOAT1 );
			}
		}
		break;
	case ASS_Rotate:
		{
			int nDir;
			rkPacket >> nDir;

			pChar->SetTargetRotToDirKey( (ioUserKeyInput::DirKeyInput)nDir );
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioMultiActiveBuffSkill2::IsProtected( int iDefenseBreakType ) const
{
	if ( m_ActiveState == ASS_Move )
		return true;

	if( m_bDisableSkillProtect )
		return false;

	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{
		if( iDefenseBreakType == m_ProtectCancelType )   // 스킬 무적 파괴
			return false;
	}

	if( IsPreDelayProtect() )
	{
		if( m_dwPreDelayStartTime != 0 )
			return true;
	}

	if( m_dwPreDelayStartTime == 0 )
	{
		if( m_dwFireMotionEndTime > FRAMEGETTIME() )
			return true;
	}

	return false;
}

bool ioMultiActiveBuffSkill2::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	return false;
}

bool ioMultiActiveBuffSkill2::IsEndState() const
{
	if( m_ActiveState == ASS_End )
		return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioMultiActiveBuffSkill2::IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg )
{
	// 온/오프 가능이고, 사용중인지 체크
	if( m_bOnOffEnable )
	{
		if( m_dwOnOffEnableTime <= 0 )
		{
			// 버프/영역 체크
			if( CheckUseBuff( pChar ) )
			{
				return true;
			}
		}
		else if( m_dwOnOffEnableTime > 0 && m_dwSkillStartTime + m_dwOnOffEnableTime <= FRAMEGETTIME() )
		{
			// 버프/영역 체크
			if( CheckUseBuff( pChar ) )
			{
				return true;
			}
		}
		else
		{
			return false;
		}
	}
	else if( CheckUseBuff( pChar ) && !IsEnableMultiActive(pChar) )
	{
		return false;
	}

	// 사용가능인지 체크
	ioAttackableItem *pAttackable = ToAttackableItem( GetOwnerItem() );
	if( pAttackable && !pAttackable->IsCanFire( pChar ) )
		return false;

	if( !IsEnableMultiActive(pChar) && !CheckNeedGauge( pChar, bHelpMsg ) )
		return false;

	return true;
}

bool ioMultiActiveBuffSkill2::IsEnableMultiActive( ioBaseChar *pChar )
{
	if( !pChar )
		return false;

	ioItem *pItem = GetOwnerItem();
	if( !pItem )
		return false;

	if( pChar->HasBuff( m_szStateCheckBuff ) && pItem->GetCurSkillGuage() )
		return true;

	return false;
}

bool ioMultiActiveBuffSkill2::IsSkipState() const
{
	if( m_ActiveState == ASS_Move )
		return true;
	
	return false;
}

bool ioMultiActiveBuffSkill2::SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName )
{
	if( !pChar ) 
		return false;

	ioPlayStage *pStage = pChar->GetCreator();
	if ( !pStage )
		return false;

	if( m_ActiveState != ASS_Grap_Attack )		
		return false;

	// 충돌처리 후 체크된 타겟을 m_GrapplingTargetName로 지정한다.
	m_szGrapTargetName = szTargetName;
		
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

	// local or 훈련소 or NPC
	if( !P2PNetwork::IsNetworkPlaying() || !pStage->IsNetworkMode() || (bNPC || bAttackerNPC) )
	{
		pTarget->SetTargetRotToRotate( pChar->GetTargetRot(), true );
		pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
	}

	SetGrapLoopState( pChar );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << ASS_Grap_Loop;
		kPacket << m_szGrapTargetName;
		kPacket << pChar->GetWorldPosition();
		kPacket << pChar->GetWorldOrientation();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return true;
}

D3DXVECTOR3 ioMultiActiveBuffSkill2::GetGrapplingTargetPos( ioBaseChar *pChar )
{
	if( m_fGrapTargetRange > 0.0f )
	{
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( m_fCharHeightRate );
		D3DXVECTOR3 vDir = pChar->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );

		if( m_fGrapTargetRange > 0.0f )
			vPos += m_fGrapTargetRange * vDir;

		return vPos;
	}

	return ioMath::VEC3_ZERO;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioMultiActiveBuffSkill2::SetMarkAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_MarkAttack.m_AttackAnimation );
	if( iAniID == -1 )
		return;

	float fTimeRate = m_MarkAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_MarkAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pOwner->ClearReservedSliding();

	m_dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();

	pOwner->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex, iAniID, m_MarkAttack.m_vWeaponInfoList, FTT_SKILL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetReservedSliding( m_MarkAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	
	ioItem *pItem = GetOwnerItem();
	if ( pItem )
		pItem->ReduceSkillGauge( m_fFireDecreaseGauge );

	m_ActiveState = ASS_Mark_Attack;
}

void ioMultiActiveBuffSkill2::SetMoveState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_szMoveAni );
	if( iAniID == -1 )
		return;

	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->SetLoopAni( iAniID, FLOAT100 );
	
	m_ActiveState = ASS_Move;	
}

void ioMultiActiveBuffSkill2::SetGrapAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_GrapAttack.m_AttackAnimation );
	if( iAniID == -1 )
		return;

	float fTimeRate = m_GrapAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_GrapAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pOwner->ClearReservedSliding();

	m_dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex, iAniID, m_GrapAttack.m_vWeaponInfoList, FTT_SKILL_ATTACK, fTimeRate, dwPreDelay );

	pOwner->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetReservedSliding( m_GrapAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	
	m_ActiveState = ASS_Grap_Attack;
}

void ioMultiActiveBuffSkill2::SetGrapLoopState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) 
		return;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	
	int iAniID = pGrp->GetAnimationIdx( m_szGrapAni );
	if ( iAniID == -1 )
		return;

	pGrp->SetLoopAni( iAniID, FLOAT100 );

	ioItem *pItem = GetOwnerItem();
	if ( pItem )
		pItem->ReduceSkillGauge( m_fNeedGrapGauge );

	m_dwGrapStartTime = FRAMEGETTIME();

	m_ActiveState = ASS_Grap_Loop;
}

void ioMultiActiveBuffSkill2::SetSwingState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if( !pOwner || !pStage ) 
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_SwingAttack.m_AttackAnimation );
	if( iAniID == -1 )
		return;

	float fTimeRate = m_SwingAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_SwingAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pOwner->ClearReservedSliding();

	m_dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex, iAniID, m_SwingAttack.m_vWeaponInfoList, FTT_SKILL_ATTACK, fTimeRate, dwPreDelay );

	pOwner->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetReservedSliding( m_SwingAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
			
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_szGrapTargetName );
	if( !pTarget ) 
		return;
	
	// for NPC
	bool bNPC = false;
	bool bAttackerNPC = false;
	if( ToNpcChar( pTarget ) )
		bNPC = true;

	if( ToNpcChar( pOwner ) )
		bAttackerNPC = true;
	
	// local or 훈련소 or NPC
	if( !P2PNetwork::IsNetworkPlaying() || !pStage->IsNetworkMode() || (bNPC || bAttackerNPC) )
	{
		if( pTarget->IsNeedProcess() && pTarget->GetState() == CS_GRAPPLING_WOUNDED_SKILL )
		{
			pTarget->SetTargetRotToRotate( pOwner->GetTargetRot(), true );
			pTarget->SetGrapplingWoundSwing( pOwner->GetCharName(), GetSkillWoundGrapplingInfo(), "", FLOAT1 );
		}
	}

	ioItem *pItem = GetOwnerItem();
	if ( pItem )
		pItem->ReduceSkillGauge( m_fNeedSwingGauge );

	m_ActiveState = ASS_Swing;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << ASS_Swing;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMultiActiveBuffSkill2::SetEndState( ioBaseChar *pOwner )
{
	m_SkillState = SS_END;
	m_ActiveState = ASS_End;
	
	AddOwnerBuff( pOwner );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioMultiActiveBuffSkill2::IsMarkBuff( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return false;

	const BaseCharList &pBaseCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter = pBaseCharList.begin(); iter != pBaseCharList.end(); ++iter )
	{
		ioBaseChar *pChar = *iter;
		if( !pChar ) 
			continue;
		
		if( pChar->IsSystemState() )
			continue;
			
		if( !pChar->IsEnableTarget() )
			continue;

		if ( pChar->HasBuff( pOwner->GetCharName(), m_szTargetCheckBuff ) )
		{
			D3DXVECTOR3 vDist = pChar->GetWorldPosition() - pOwner->GetWorldPosition();
			float fDist = D3DXVec3LengthSq( &vDist );
			if ( fDist >= m_fCheckDist * m_fCheckDist )
				return false;
			else
			{
				m_szTargetName = pChar->GetCharName();
				return true;
			}
		}
	}

	return false;
}

void ioMultiActiveBuffSkill2::CheckMove( ioBaseChar *pOwner, ioPlayStage *pStage )
{	
	if( !pOwner || !pStage ) 
		return;
	
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
		
	D3DXVECTOR3 vDir;
	if ( !m_szTargetName.IsEmpty() )
	{
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTargetName );
		if( !pTarget )	
			return;
		
		vDir = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();

		pOwner->SetTargetRotToDir( vDir, true );
	}
	else
		vDir = pOwner->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	D3DXVec3Normalize( &vDir, &vDir );

	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt;

		if( m_fMoveSpeed > 0.0f && vDir != ioMath::VEC3_ZERO )
		{
			float fFinalMoveAmt = m_fMoveSpeed * fDivideTimePerSec;
			if( fFinalMoveAmt > 0.0f )
			{
				vMoveAmt = vDir * fFinalMoveAmt;
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
		float fPreHeight = pOwner->GetBottomHeight();

		pOwner->Translate( vTotalMove );
			
		if( pOwner->GetForceAmt() != 0.0f )
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

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	float fPreHeight = pOwner->GetBottomHeight();

	if( vPos.y < fPreHeight )
		vPos.y = fPreHeight;

	if( pStage && pOwner->IsAxisLimit() )
	{
		pStage->CheckAxisLimit( &vPos );
		pOwner->SetWorldPosition( vPos );
	}	
}

bool ioMultiActiveBuffSkill2::IsArrived( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if( !pOwner || !pStage ) 
		return true;

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTargetName );
	if( !pTarget )	
		return true;

	D3DXVECTOR3 vDist = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();

	float fDist = D3DXVec3LengthSq( &vDist );
	if ( fDist <= m_fArriveDist * m_fArriveDist )
		return true;
	
	return false;	
}

void ioMultiActiveBuffSkill2::CheckInputKey( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if ( !pOwner || !pOwner->IsNeedProcess() || !pStage )
		return;

	if ( pOwner->IsSettedDirection() )
	{
		if ( pOwner->IsChangeDirection() )
		{
			ioUserKeyInput::DirKeyInput eDirKey = pOwner->CheckCurDirKey();
			pOwner->SetTargetRotToDirKey( eDirKey );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pOwner->GetCharName();
				kPacket << GetName();
				kPacket << ASS_Rotate;
				kPacket << (int)eDirKey;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}

	if( pOwner->IsAttackKey() )
	{
		SetSwingState( pOwner, pStage );
		return;
	}
}