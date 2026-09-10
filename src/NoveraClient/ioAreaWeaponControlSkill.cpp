
#include "stdafx.h"

#include "ioAreaWeaponControlSkill.h"


#include "ioBaseChar.h"
#include "ioPlayStage.h"

ioAreaWeaponControlSkill::ioAreaWeaponControlSkill()
{
	m_ControlState = RS_NONE;
	m_AreaWeaponState = AS_DELAY;

	m_dwLoopStartTime = 0;
	m_dwEndAniStartTime = 0;
	m_dwEndAniEndTime = 0;
	m_dwAreaWeaponIndex = 0;
	m_bSetHandMesh = false;
}

ioAreaWeaponControlSkill::ioAreaWeaponControlSkill( const ioAreaWeaponControlSkill &rhs )
: ioNormalSkill( rhs ),
 m_fOffSet( rhs.m_fOffSet ),
 m_LoopAnimation( rhs.m_LoopAnimation ),
 m_dwLoopTime( rhs.m_dwLoopTime ),
 m_EndAnimation( rhs.m_EndAnimation ),
 m_fEndAniRate( rhs.m_fEndAniRate ),
 m_StartHandMesh( rhs.m_StartHandMesh ),
 m_LoopHandMesh( rhs.m_LoopHandMesh ),
 m_EndHandMesh( rhs.m_EndHandMesh ),
 m_dwLoopProtectDuration( rhs.m_dwLoopProtectDuration ),
 m_AreaWeaponName( rhs.m_AreaWeaponName ),
 m_RedCircle( rhs.m_RedCircle ),
 m_BlueCircle( rhs.m_BlueCircle ),
 m_dwCircleDuration( rhs.m_dwCircleDuration ),
 m_fMoveSpeed( rhs.m_fMoveSpeed ),
 m_fMoveRange( rhs.m_fMoveRange )
{
	m_ControlState = RS_NONE;
	m_AreaWeaponState = AS_DELAY;

	m_dwLoopStartTime = 0;
	m_dwEndAniStartTime = 0;
	m_dwEndAniEndTime = 0;
	m_dwAreaWeaponIndex = 0;
	m_bSetHandMesh = false;
}

ioAreaWeaponControlSkill::~ioAreaWeaponControlSkill()
{
	DestroyAreaWeapon();
}

void ioAreaWeaponControlSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	// Animation
	rkLoader.LoadString_e( "loop_animation", "", szBuf, MAX_PATH );
	m_LoopAnimation = szBuf;
	m_dwLoopTime = rkLoader.LoadInt_e( "loop_duration", 0 );
	m_dwLoopProtectDuration = rkLoader.LoadInt_e( "loop_protect_duration", 0 );

	rkLoader.LoadString_e( "end_animation", "", szBuf, MAX_PATH );
	m_EndAnimation = szBuf;
	m_fEndAniRate = rkLoader.LoadFloat_e( "end_ani_rate", FLOAT1 );

	// MagicCircle
	m_fOffSet = rkLoader.LoadFloat_e( "circle_offset", 0.0f );
	m_dwCircleDuration = rkLoader.LoadInt_e( "circle_duration", 0 );

	rkLoader.LoadString_e( "red_team_circle", "", szBuf, MAX_PATH );
	m_RedCircle = szBuf;
	rkLoader.LoadString_e( "blue_team_circle", "", szBuf, MAX_PATH );
	m_BlueCircle = szBuf;

	// HandMesh
	rkLoader.LoadString_e( "start_hand_mesh", "", szBuf, MAX_PATH );
	m_StartHandMesh = szBuf;
	rkLoader.LoadString_e( "loop_hand_mesh", "", szBuf, MAX_PATH );
	m_LoopHandMesh = szBuf;
	rkLoader.LoadString_e( "end_hand_mesh", "", szBuf, MAX_PATH );
	m_EndHandMesh = szBuf;

	// AreaWeapon
	rkLoader.LoadString_e( "area_weapon_name", "", szBuf, MAX_PATH );
	m_AreaWeaponName = szBuf;

	m_fMoveSpeed = rkLoader.LoadFloat_e( "area_weapon_move_speed", 0.0f );
	m_fMoveRange = rkLoader.LoadFloat_e( "area_weapon_move_range", 0.0f );
}

ioSkill* ioAreaWeaponControlSkill::Clone()
{
	return new ioAreaWeaponControlSkill( *this );
}

bool ioAreaWeaponControlSkill::IsProtected( int iDefenseBreakType ) const
{
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

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_ControlState == RS_ACTION )
		return true;

	if( m_ControlState == RS_LOOP )
	{
		if( m_dwLoopProtectDuration == 0 )
			return true;

		if( m_dwProtectTime > dwCurTime )
			return true;
	}
	
	if( m_ControlState == RS_END )
		return false;

	return false;
}

bool ioAreaWeaponControlSkill::IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg )
{
	ioAttackableItem *pAttackable = ToAttackableItem( GetOwnerItem() );
	if( pAttackable )
	{
		if( !pAttackable->IsCanFire( pChar ) )
			return false;
	}

	if( !CheckNeedGauge( pChar, bHelpMsg ) )
		return false;

	return true;
}

bool ioAreaWeaponControlSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_ControlState != RS_END )
		return false;

	if( m_dwEndAniStartTime > 0 && m_dwEndAniEndTime < FRAMEGETTIME() )
		return true;

	return false;
}

bool ioAreaWeaponControlSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !HasSkillMotion() )
		return false;

	pChar->SetSkillProtection();

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	m_ControlState = RS_NONE;
	m_AreaWeaponState = AS_DELAY;

	m_dwMotionStartTime = 0;
	m_dwLoopStartTime = 0;
	m_dwEndAniStartTime = 0;

	m_dwProtectTime = 0;
	m_bSetHandMesh = false;

	m_dwAreaWeaponIndex = 0;
	m_vMoveDir = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	// MagicCircle
	float fCurOffSet = GetOwnerItem()->GetMagicCircleOffSet();
	if( fCurOffSet == 0.0f )
		fCurOffSet = m_fOffSet;

	m_vCreatePos = pChar->GetMagicCirclePos( fCurOffSet, m_bCircleNoChangeY );
	
	ioMapEffect *pMapEffect = NULL;
	if( m_dwCircleDuration > 0 )
	{
		if( pChar->GetTeam() == TEAM_RED )
		{
			pMapEffect = pChar->CreateMapEffectBySkill( m_RedCircle, m_vCreatePos );
		}
		else if( pChar->GetTeam() == TEAM_BLUE )
		{
			pMapEffect = pChar->CreateMapEffectBySkill( m_BlueCircle, m_vCreatePos );
		}
		else
		{
			if( pChar->GetSingleConvertTeam() == TEAM_BLUE )
				pMapEffect = pChar->CreateMapEffectBySkill( m_BlueCircle, m_vCreatePos );
			else
				pMapEffect = pChar->CreateMapEffectBySkill( m_RedCircle, m_vCreatePos );
		}
	}

	DWORD dwPreDelayTime = GetPreDelayTime();

	if( pMapEffect )
	{
		m_dwCurMapEffect = pMapEffect->GetUniqueID();
		m_dwCircleEnd = FRAMEGETTIME() + dwPreDelayTime + m_dwCircleDuration;
	}


	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		m_dwPreDelayStartTime = 0;
		EndPreDelayEffect( pChar );
		SetActionState( pChar );
	}
	else
	{
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
			dwTrackingTime += FRAMEGETTIME();
		else
			dwTrackingTime = FRAMEGETTIME() + dwPreDelay;

		if( IsUseAutoTargetValue() )
		{
			pChar->SetAutoTarget( ATT_SKILL );
			pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
	}

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	SetSkillNameChatBubble( pChar );

	return true;
}

void ioAreaWeaponControlSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwPreDelayTime = GetPreDelayTime();

	// MagicCircle
	if( m_dwCircleEnd > 0 && m_dwCircleEnd < dwCurTime )
	{
		//DestroyMapEffect
		pChar->EndMapEffect( m_dwCurMapEffect );
		pChar->DestroyMagicCircle();
		m_dwCircleEnd = 0;
	}

	switch( m_ControlState )
	{
	case RS_NONE:
		if( m_dwPreDelayStartTime > 0 && m_dwPreDelayStartTime + dwPreDelayTime < dwCurTime )
		{
			m_dwPreDelayStartTime = 0;
			EndPreDelayEffect( pChar );
			SetActionState( pChar );
		}
		break;
	case RS_ACTION:
		if( m_dwMotionStartTime > 0 && m_dwMotionEndTime < dwCurTime )
		{
			if( m_dwCreateAreaWeaponTime > 0 && m_dwCreateAreaWeaponTime < dwCurTime )
			{
				CreateAreaWeapon( pChar, pStage );
				pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
				m_bReduceGauge = true;
			}

			m_dwMotionStartTime = 0;
			SetLoopState( pChar, pStage );
		}
		break;
	case RS_LOOP:
		// AreaWeapon
		if( m_dwLoopStartTime > 0 )
		{
			CheckControl( pChar );
			ProcessControl( pChar, pStage );
		}

		if( m_dwLoopStartTime > 0 && m_dwLoopStartTime+m_dwLoopTime < dwCurTime )
		{
			m_dwLoopStartTime = 0;
			SetEndState( pChar );
		}
		break;
	case RS_END:
		break;
	}
}

void ioAreaWeaponControlSkill::OnSkillEnd( ioBaseChar *pOwner )
{
	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pOwner->ShowWeaponItemMesh( true );
	}

	// 스킬 중간 동작에 사용된 메쉬들 제거
	m_bSetHandMesh = false;

	if( !m_StartHandMesh.IsEmpty() )
		pOwner->RestoreSkillWeaponMesh( this, m_StartHandMesh, m_bVisibleEquipMesh );

	if( !m_LoopHandMesh.IsEmpty() )
		pOwner->RestoreSkillWeaponMesh( this, m_LoopHandMesh, m_bVisibleEquipMesh );

	if( !m_EndHandMesh.IsEmpty() )
		pOwner->RestoreSkillWeaponMesh( this, m_EndHandMesh, m_bVisibleEquipMesh );

	// Ani
	m_dwMotionStartTime = 0;
	m_dwLoopStartTime = 0;
	m_dwEndAniStartTime = 0;

	//DestroyMapEffect
	pOwner->EndMapEffect( m_dwCurMapEffect );
	pOwner->DestroyMagicCircle();

	//AreaDestroy
	DestroyAreaWeapon();

	ioNormalSkill::OnSkillEnd( pOwner );
}

void ioAreaWeaponControlSkill::StartPreDelayEffect( ioBaseChar *pChar )
{
	ioSkill::StartPreDelayEffect( pChar );
}

void ioAreaWeaponControlSkill::EndPreDelayEffect( ioBaseChar *pChar )
{
	ioSkill::EndPreDelayEffect( pChar );
}

void ioAreaWeaponControlSkill::SetActionState( ioBaseChar *pChar )
{
	m_dwMotionStartTime = FRAMEGETTIME();
	m_dwMotionEndTime = m_dwMotionStartTime;

	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	return;

	float fAniRate = GetSkillMotionRate();

	float fTimeRate = fAniRate * m_fExtraAniRate;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	

	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iCurSkillAniID )*fTimeRate;

	DWORD dwCreateTime = pGrp->GetAnimationEventTime_e( iCurSkillAniID, "fire_s" ) * fTimeRate;
	m_dwCreateAreaWeaponTime = m_dwMotionStartTime + dwCreateTime;

	m_ControlState = RS_ACTION;

	pChar->AttachEffect( m_SkillEffectName );

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_StartHandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_StartHandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}

void ioAreaWeaponControlSkill::CreateAreaWeapon( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_AreaWeaponName.IsEmpty() ) return;

	if( pChar->IsNeedProcess() )
	{
		if( m_dwCircleDuration == 0 )
		{
			float fCurOffSet = GetOwnerItem()->GetMagicCircleOffSet();
			if( fCurOffSet == 0.0f )
				fCurOffSet = m_fOffSet;

			m_vCreatePos = pChar->GetMagicCirclePos( fCurOffSet, m_bCircleNoChangeY );
		}

		D3DXQUATERNION qtRot = pChar->GetTargetRot();
		ioAreaWeapon *pArea = g_AreaWeaponMgr.CreateAreaWeapon( pChar->GetCharName(),
																"",
																GetName(),
																m_AreaWeaponName,
																m_vCreatePos,
																qtRot,
																ioAreaWeapon::CT_SKILL_DEPENDENCY );
		if( pArea )
		{
			m_dwAreaWeaponIndex = pArea->GetAreaWeaponIndex();

			//UPD로 생성 상황 동기화 맞춰줄것
			if( pChar->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pChar->GetCharName();
				kPacket << GetName();

				kPacket << SS_CREATE;
				kPacket << m_AreaWeaponName;
				kPacket << m_vCreatePos;
				kPacket << qtRot;
				kPacket << (int)ioAreaWeapon::CT_SKILL_DEPENDENCY;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

void ioAreaWeaponControlSkill::DestroyAreaWeapon()
{
	if( m_dwAreaWeaponIndex == 0 )
		return;

	g_AreaWeaponMgr.DestroyAreaWeapon( m_dwAreaWeaponIndex );
	m_dwAreaWeaponIndex = 0;
}

void ioAreaWeaponControlSkill::SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_LoopAnimation.IsEmpty() )
	{
		m_dwLoopStartTime = 0;
		return;
	}

	if( m_bSetHandMesh )
	{
		if( !m_StartHandMesh.IsEmpty() )
		{
			pChar->RestoreSkillWeaponMesh( this, m_StartHandMesh, m_bVisibleEquipMesh );
		}
		m_bSetHandMesh = false;
	}

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_LoopHandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_LoopHandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( m_LoopAnimation, FLOAT100 );

	m_dwLoopStartTime = FRAMEGETTIME();
	m_dwProtectTime = m_dwLoopStartTime + m_dwLoopProtectDuration;

	m_ControlState = RS_LOOP;
}

void ioAreaWeaponControlSkill::SetEndState( ioBaseChar *pChar )
{
	m_ControlState = RS_END;
	m_dwEndAniStartTime = FRAMEGETTIME();
	m_dwEndAniEndTime = m_dwEndAniStartTime;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iCurSkillAniID = pGrp->GetAnimationIdx( m_EndAnimation );
	if( iCurSkillAniID == -1 )	return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fEndAniRate );
	
	m_dwEndAniEndTime += pGrp->GetAnimationFullTime( iCurSkillAniID ) * m_fEndAniRate;

	if( m_bSetHandMesh )
	{
		if( !m_LoopHandMesh.IsEmpty() )
		{
			pChar->RestoreSkillWeaponMesh( this, m_LoopHandMesh, m_bVisibleEquipMesh );
		}
		m_bSetHandMesh = false;
	}

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_EndHandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_EndHandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	DestroyAreaWeapon();
}

void ioAreaWeaponControlSkill::CheckControl( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( !pChar->IsNeedProcess() ) return;

	if( m_dwAreaWeaponIndex == 0 )
		return;

	ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponByIndex( m_dwAreaWeaponIndex );
	if( !pAreaWeapon )
	{
		m_dwAreaWeaponIndex = 0;
		return;
	}

	bool bUpdate = false;
	if( m_AreaWeaponState == AS_DELAY )
	{
		if( pChar->IsSettedDirection() )
		{
			ioUserKeyInput::DirKeyInput eDir = pChar->CheckCurDirKey();
			if( eDir != ioUserKeyInput::DKI_NONE )
			{
				float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
				fYawD = ioMath::ArrangeHead( fYawD );

				float fYawR = DEGtoRAD( fYawD );
				D3DXQUATERNION qtRotate;
				D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

				m_vMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			}

			m_AreaWeaponState = AS_MOVE;
			bUpdate = true;
		}
	}
	else if( m_AreaWeaponState == AS_MOVE )
	{
		if( !pChar->IsSettedDirection() )
		{
			m_AreaWeaponState = AS_DELAY;
			bUpdate = true;
		}
		else
		{
			if( pChar->IsChangeDirection() )
			{
				ioUserKeyInput::DirKeyInput eDir = pChar->CheckCurDirKey();
				if( eDir != ioUserKeyInput::DKI_NONE )
				{
					float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
					fYawD = ioMath::ArrangeHead( fYawD );

					float fYawR = DEGtoRAD( fYawD );
					D3DXQUATERNION qtRotate;
					D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

					m_vMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
				}

				bUpdate = true;
			}
		}
	}

	if( bUpdate && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();

		kPacket << SS_UPDATE;
		kPacket << m_AreaWeaponState;
		kPacket << m_vMoveDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioAreaWeaponControlSkill::ProcessControl( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_dwAreaWeaponIndex == 0 )
		return;

	ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponByIndex( m_dwAreaWeaponIndex );
	if( !pAreaWeapon )
	{
		m_dwAreaWeaponIndex = 0;
		return;
	}

	if( m_AreaWeaponState == AS_MOVE && m_fMoveSpeed > 0.0f )
	{
		D3DXVECTOR3 vPrePos, vCurPos, vPos;
		vPos = m_vMoveDir;
		vPos *= m_fMoveSpeed * g_FrameTimer.GetSecPerFrame();

		vPrePos = vCurPos = pAreaWeapon->GetStartPos();
		vCurPos += vPos;

		float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pChar, false );
		if( fMapHeight > 0.0f )
			vCurPos.y = fMapHeight;

		D3DXVECTOR3 vCreatePos = m_vCreatePos;
		D3DXVECTOR3 vCurXZ = vCurPos;

		vCreatePos.y = 0.0f;
		vCurXZ.y = 0.0f;

		D3DXVECTOR3 vDiff = vCurXZ - vCreatePos;

		if( D3DXVec3Length( &vDiff ) > m_fMoveRange )
		{
			D3DXVec3Normalize( &vDiff, &vDiff );
			vDiff *= m_fMoveRange;

			float fCurHeight = vCurPos.y;
			vCurPos = vCreatePos + vDiff;
			vCurPos.y = fCurHeight;

			fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pChar, false );
			if( fMapHeight > 0.0f )
				vCurPos.y = fMapHeight;
		}

		float fGapHeight = fabs( vPrePos.y - vCurPos.y );
		if( fGapHeight > pChar->GetMidHeightByRate() )
			vCurPos = vPrePos;

		pAreaWeapon->SetAreaPosition( vCurPos, pStage );
	}
}

void ioAreaWeaponControlSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iSyncState;
	rkPacket >> iSyncState;

	if( iSyncState == SS_CREATE )
	{
		ioHashString szAreaWeaponName;
		rkPacket >> szAreaWeaponName;

		int iCreateType;
		D3DXVECTOR3 vPos;
		D3DXQUATERNION qtRot;
		rkPacket >> vPos >> qtRot >> iCreateType;

		ioAreaWeapon *pArea = g_AreaWeaponMgr.CreateAreaWeapon( pChar->GetCharName(),
																"",
																"",
																szAreaWeaponName,
																vPos,
																qtRot,
																iCreateType );
		if( pArea )
		{
			m_dwAreaWeaponIndex = pArea->GetAreaWeaponIndex();
		}
	}
	else if( iSyncState == SS_UPDATE && CheckAreaWeapon() )
	{
		int iAreaWeaponState;
		rkPacket >> iAreaWeaponState;
		
		m_AreaWeaponState = (AreaWeaponState)iAreaWeaponState;
		rkPacket >> m_vMoveDir;
	}
}

bool ioAreaWeaponControlSkill::CheckAreaWeapon()
{
	if( m_dwAreaWeaponIndex == 0 )
		return false;

	ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponByIndex( m_dwAreaWeaponIndex );
	if( pAreaWeapon )
		return true;

	return false;
}

const D3DXVECTOR3& ioAreaWeaponControlSkill::GetAreaWeaponPos()
{
	if( m_dwAreaWeaponIndex == 0 )
		return m_vCreatePos;

	ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponByIndex( m_dwAreaWeaponIndex );
	if( pAreaWeapon )
		return pAreaWeapon->GetStartPos();

	return m_vCreatePos;
}

const D3DXVECTOR3& ioAreaWeaponControlSkill::GetAreaWeaponPrePos()
{
	if( m_dwAreaWeaponIndex == 0 )
		return m_vCreatePos;

	ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponByIndex( m_dwAreaWeaponIndex );
	if( pAreaWeapon )
		return pAreaWeapon->GetPreStartPos();

	m_dwAreaWeaponIndex = 0;

	return m_vCreatePos;
}

bool ioAreaWeaponControlSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

