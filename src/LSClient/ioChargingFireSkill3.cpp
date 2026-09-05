

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"


#include "ioChargingFireSkill3.h"

#include "WeaponDefine.h"

ioChargingFireSkill3::ioChargingFireSkill3()
{
	m_dwChargingEffect = -1;

	ClearData();
}

ioChargingFireSkill3::ioChargingFireSkill3( const ioChargingFireSkill3 &rhs )
: ioNormalSkill( rhs ),
 m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
 m_fSkillLoopMotionRate( rhs.m_fSkillLoopMotionRate ),
 m_SkillChargingEffect( rhs.m_SkillChargingEffect ),
 m_dwCharRotateSpeed( rhs.m_dwCharRotateSpeed ),
 m_dwLoopDuration( rhs.m_dwLoopDuration ),
 m_WeaponInfo( rhs.m_WeaponInfo ),
 m_FireCntInfoList( rhs.m_FireCntInfoList ),
 m_AttackAngleList( rhs.m_AttackAngleList ),
 m_SkillEndAni( rhs.m_SkillEndAni ),
 m_fSkillEndAniRate( rhs.m_fSkillEndAniRate )
{
	m_dwChargingEffect = -1;

	ClearData();
}

ioChargingFireSkill3::~ioChargingFireSkill3()
{
}

void ioChargingFireSkill3::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	rkLoader.LoadString_e( "skill_charging_effect", "", szBuf, MAX_PATH );
	m_SkillChargingEffect = szBuf;

	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_SkillLoopMotion = szBuf;
	m_fSkillLoopMotionRate = rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );
	m_dwLoopDuration = rkLoader.LoadInt_e( "skill_loop_duration", 0 );

	m_dwCharRotateSpeed = rkLoader.LoadInt_e( "char_rotate_speed", 0 );

	// Action Attack
	m_WeaponInfo.m_iWeaponIdx = rkLoader.LoadInt_e( "weapon_attribte_index", 0 );
	m_WeaponInfo.m_iResistanceIdx = rkLoader.LoadInt_e( "weapon_attribute_resist_index", 0 );

	rkLoader.LoadString_e( "weapon_wounded_animation", "", szBuf, MAX_PATH );
	m_WeaponInfo.m_WoundedAnimation = szBuf;
	m_WeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt_e( "weapon_wounded_duration", 0 );
	m_WeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool_e( "weapon_wounded_loop_ani", false );

	// End
	rkLoader.LoadString_e( "skill_end_ani", "", szBuf, MAX_PATH );
	m_SkillEndAni = szBuf;
	m_fSkillEndAniRate = rkLoader.LoadFloat_e( "skill_end_ani_rate", FLOAT1 );

	// cnt info
	m_FireCntInfoList.clear();

	int iFireCntMax = rkLoader.LoadInt_e( "fire_cnt_info_max", 0 );
	for( int i=0; i < iFireCntMax; ++i )
	{
		FireCntInfo kInfo;

		wsprintf_e( szKey, "fire_cnt_info%d_time", i+1 );
		kInfo.m_dwCheckTime = rkLoader.LoadInt( szKey, 0 );
		
		wsprintf_e( szKey, "fire_cnt_info%d_cnt", i+1 );
		kInfo.m_iCreateCnt = rkLoader.LoadInt( szKey, 0 );

		m_FireCntInfoList.push_back( kInfo );
	}

	// angle info
	m_AttackAngleList.clear();

	int iMaxAngle = rkLoader.LoadInt_e( "fire_max_angle", 0 );
	for( int j=0; j < iMaxAngle; ++j )
	{
		wsprintf_e( szKey, "fire_angle%d", j+1 );
		float fAngle = rkLoader.LoadFloat( szKey, 0.0f );

		m_AttackAngleList.push_back( fAngle );
	}
}

ioSkill* ioChargingFireSkill3::Clone()
{
	return new ioChargingFireSkill3( *this );
}

void ioChargingFireSkill3::ClearData()
{
	m_State = SS_NONE;

	m_bSetHandMesh = false;

	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;

	m_dwLoopStartTime = 0;
	m_dwFireStartTime = 0;

	m_iCurMaxCnt = 0;
	m_iCurWayCnt = 0;

	m_vCharMoveDir = ioMath::VEC3_ZERO;

	m_bTargetRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
}

bool ioChargingFireSkill3::OnSkillStart( ioBaseChar *pChar )
{	
	if( !HasSkillMotion() )
		return false;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	if( m_dwChargingEffect != -1 )
	{
		pChar->EndEffect( m_dwChargingEffect );
		m_dwChargingEffect = -1;
	}

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	pChar->SetSkillProtection();

	SetCameraBuff( pChar );

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		SetLoopState( pChar );
	}

	SetSkillNameChatBubble( pChar );

	return true;
}

void ioChargingFireSkill3::OnSkillEnd( ioBaseChar *pChar )
{
	ioNormalSkill::OnSkillEnd( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	// 스킬 중간 동작에 사용된 메쉬들 제거
	m_bSetHandMesh = false;
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	EndCameraBuff( pChar );

	//
	if( m_dwChargingEffect != -1 )
	{
		pChar->EndEffect( m_dwChargingEffect );
		m_dwChargingEffect = -1;
	}

	ClearData();
}

void ioChargingFireSkill3::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	int iSlot = GetOwnerItem()->GetType() - 1;
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;

				SetLoopState( pChar );
				return;
			}
		}
		break;
	case SS_LOOP:
		{
			CheckCharRotateKey( pChar );
			ProcessCharRotateDir( pChar );

			if( pChar->IsNeedProcess() && m_dwLoopStartTime+m_dwLoopDuration < dwCurTime )
			{
				// End
				SetEndState( pChar );
				return;
			}

			if( pChar->IsNeedProcess() )
			{
				if( pChar->IsAttackKey() ||
					pChar->IsDefenseKey() ||
					pChar->IsJumpKey() ||
					pChar->GetSkillKeyInput() == iSlot )
				{
					SetActionState( pChar, pStage, true );
					return;
				}
			}
		}
		break;
	case SS_ACTION:
		if( m_dwFireStartTime > 0 && m_dwFireStartTime < dwCurTime )
		{
			m_dwFireStartTime = 0;
			D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();

			for( int i=0; i < m_iCurMaxCnt; ++i )
			{
				pChar->SkillFireExplicit( m_WeaponInfo, GetName(), m_dwWeaponBaseIndex+i, vPos );
			}
		}
		break;
	case SS_END:
		break;
	}
}

bool ioChargingFireSkill3::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}

void ioChargingFireSkill3::SetNormalSkill( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	return;

	DWORD dwCurTime = FRAMEGETTIME();
	pChar->AttachEffect( m_SkillEffectName );

	float fAniRate = GetSkillMotionRate();
	float fTimeRate = fAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;

	m_dwFireStartTime = m_dwFireMotionEndTime = dwCurTime;
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "fire_s" ) * fTimeRate;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "firemotion_e" ) * fTimeRate;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "key_reserve" ) * fTimeRate;

	if( m_dwFireStartTime >= m_dwFireMotionEndTime )
		m_dwFireMotionEndTime = m_dwMotionEndTime;

	//
	SetExtraAniJump( pChar, iCurSkillAniID, fTimeRate );

	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;
}

void ioChargingFireSkill3::SetLoopState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();

	EndPreDelayEffect( pChar );

	DWORD dwCurTime = FRAMEGETTIME();
	m_State = SS_LOOP;
	m_dwLoopStartTime = dwCurTime;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_SkillLoopMotion );
	float fAniRate = FLOAT1;
	if( m_fSkillLoopMotionRate > 0.0f )
		fAniRate = m_fSkillLoopMotionRate;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	if( m_dwChargingEffect != -1 )
	{
		pChar->EndEffect( m_dwChargingEffect, false );
		m_dwChargingEffect = -1;
	}

	ioEffect *pEffect = pChar->AttachEffect( m_SkillChargingEffect );
	if( pEffect )
	{
		m_dwChargingEffect = pEffect->GetUniqueID();
	}
}

void ioChargingFireSkill3::SetActionState( ioBaseChar *pChar, ioPlayStage *pStage, bool bSend )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();

	if( m_State == SS_ACTION )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_State = SS_ACTION;

	if( m_dwChargingEffect != -1 )
	{
		pChar->EndEffect( m_dwChargingEffect, false );
		m_dwChargingEffect = -1;
	}

	m_iCurMaxCnt = GetCurFireMaxCnt();

	SetNormalSkill( pChar );

	if( bSend && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_ACTION;
		kPacket << pChar->GetWorldPosition();
		kPacket << pChar->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargingFireSkill3::SetEndState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_State = SS_END;

	if( m_dwChargingEffect != -1 )
	{
		pChar->EndEffect( m_dwChargingEffect, false );
		m_dwChargingEffect = -1;
	}

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_SkillEndAni );
	float fTimeRate = m_fSkillEndAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	if( iAniID != -1 )
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_END;
		kPacket << pChar->GetWorldPosition();
		kPacket << pChar->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargingFireSkill3::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar ) return;

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case SSC_LOOP:
		break;
	case SSC_ACTION:
		{
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRot;

			rkPacket >> vPos >> qtRot;
			pChar->SetWorldPosition( vPos );
			pChar->SetTargetRotToRotate( qtRot, true, false );

			SetActionState( pChar, pStage, false );
		}
		break;
	case SSC_END:
		{
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRot;

			rkPacket >> vPos >> qtRot;
			pChar->SetWorldPosition( vPos );
			pChar->SetTargetRotToRotate( qtRot, true, false );

			SetEndState( pChar );
		}
		break;
	case SSC_DIR:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pChar->SetTargetRotToRotate( qtRot, true, false );

			int iCurKey;
			rkPacket >> iCurKey;

			m_bTargetRot = false;
			m_CurDirKey = (ioUserKeyInput::DirKeyInput)iCurKey;
		}
		break;
	}
}

bool ioChargingFireSkill3::IsProtected( int iDefenseBreakType ) const
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

	switch( m_State )
	{
	case SS_LOOP:
		return false;
	case SS_ACTION:
		if( m_dwFireMotionEndTime > FRAMEGETTIME() )
			return true;
		break;
	}

	return false;
}

bool ioChargingFireSkill3::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwMotionEndTime > 0 && m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
		return true;
	
	return false;
}

bool ioChargingFireSkill3::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioChargingFireSkill3::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	if( pWeapon && m_iCurWayCnt < (int)m_AttackAngleList.size() )
	{
		float fCurAngle = 0.0f;

		fCurAngle = m_AttackAngleList[m_iCurWayCnt];
		fCurAngle = ioMath::ArrangeHead( fCurAngle );

		D3DXVECTOR3 vMoveDir;
		D3DXQUATERNION qtAngle;
		D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fCurAngle), 0.0f, 0.0f );

		D3DXQUATERNION qtRotate = pWeapon->GetOwner()->GetTargetRot();
		vMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		vMoveDir = qtAngle * vMoveDir;
		D3DXVec3Normalize( &vMoveDir, &vMoveDir );

		pWeapon->SetMoveDir( vMoveDir );

		m_iCurWayCnt++;
	}

	ioBaseChar *pChar = pWeapon->GetOwner();
	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( false, true );
		}
	}


}

void ioChargingFireSkill3::CheckCharRotateKey( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( !pChar->IsNeedProcess() )
		return;

	ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		m_bTargetRot = false;
		m_CurDirKey = eNewDirKey;

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SSC_DIR;
			kPacket << pChar->GetTargetRot();
			kPacket << (int)m_CurDirKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioChargingFireSkill3::ProcessCharRotateDir( ioBaseChar *pChar )
{
	if( m_dwCharRotateSpeed == 0 )
		return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	if( m_bTargetRot )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwCharRotateSpeed;
	float fRotateAngle = 360.0f * fRate;

	D3DXQUATERNION qtCurRot = pChar->GetTargetRot();
	D3DXVECTOR3 vCurMoveDir = qtCurRot * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vCurMoveDir, &vCurMoveDir );

	// 목표에 도달체크 및 처리
	float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXVECTOR3 vTargetDir = qtRotate * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	float fDotValue = D3DXVec3Dot( &vCurMoveDir, &vTargetDir );
	float fGapAngle = RADtoDEG( acosf(fDotValue) );

	if( fDotValue >= FLOAT1 )
	{
		m_bTargetRot = true;
		pChar->SetTargetRotToRotate( qtRotate, true );
		return;
	}

	D3DXVECTOR3 vAxis;
	if( fDotValue <= -FLOAT1 )
	{
		vAxis = ioMath::UNIT_Y;
	}
	else
	{
		D3DXVec3Cross( &vAxis, &vTargetDir, &vCurMoveDir );
		D3DXVec3Normalize( &vAxis, &-vAxis );
	}

	D3DXQUATERNION qtNewRot;
	if( fGapAngle < fRotateAngle )
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fGapAngle) );
	else
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fRotateAngle) );

	D3DXVECTOR3 vNewDir = qtNewRot * vCurMoveDir;
	D3DXVec3Normalize( &vNewDir, &vNewDir );

	ioMath::CalcDirectionQuaternion( qtNewRot, -vNewDir );
	pChar->SetTargetRotToRotate( qtNewRot, true );
}

int ioChargingFireSkill3::GetCurFireMaxCnt()
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGapTime = dwCurTime - m_dwLoopStartTime;

	int iCurMaxCnt = 0;

	int iInfoCnt = m_FireCntInfoList.size();
	for( int i=0; i < iInfoCnt; ++i )
	{
		DWORD dwCurCheckTime = m_FireCntInfoList[i].m_dwCheckTime;
		DWORD dwNextCheckTime = 0;
		if( i+1 < iInfoCnt )
			dwNextCheckTime = m_FireCntInfoList[i+1].m_dwCheckTime;

		if( dwCurCheckTime <= dwGapTime && dwNextCheckTime == 0 )
		{
			iCurMaxCnt = m_FireCntInfoList[i].m_iCreateCnt;
			break;
		}
		else if( COMPARE( dwGapTime, dwCurCheckTime, dwNextCheckTime ) )
		{
			iCurMaxCnt = m_FireCntInfoList[i].m_iCreateCnt;
			break;
		}
	}

	return iCurMaxCnt;
}




