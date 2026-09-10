
#include "stdafx.h"

#include "ioExtendAssaultDash.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ItemDefine.h"
#include "WeaponDefine.h"

ioExtendAssaultDash::ioExtendAssaultDash()
{
	m_dwActionEndTime = 0;
	
	m_bSetExtendAssault = false;

	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;

	m_iCurAniRotate = 0;
	m_vAniRotateInfoList.clear();

	m_iCurExtendCnt = 0;

	m_dwReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_bEnableDashState = false;
	m_bEnableAttackAfterDash = false;
	m_nCurRepeatDashCnt = 0;
}

ioExtendAssaultDash::ioExtendAssaultDash( const ioExtendAssaultDash &rhs )
: ioExtendDash( rhs ),
 m_AttackAttribute( rhs.m_AttackAttribute ),
 m_ExtraAttackList( rhs.m_ExtraAttackList ),
 m_nMaxRepeatDashCnt( rhs.m_nMaxRepeatDashCnt ),
 m_RepeatDashInfo( rhs.m_RepeatDashInfo ),
 m_bSetEnableDash( rhs.m_bSetEnableDash ),
 m_bEnableAttackAfterDash( rhs.m_bEnableAttackAfterDash )
{
	m_dwActionEndTime = 0;
	
	m_bSetExtendAssault = false;

	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;

	m_iCurAniRotate = 0;
	m_vAniRotateInfoList.clear();

	m_iCurExtendCnt = 0;

	m_dwReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_bEnableDashState = false;
	m_nCurRepeatDashCnt = 0;
}

ioExtendDash* ioExtendAssaultDash::Clone()
{
	return new ioExtendAssaultDash( *this );
}

ioExtendAssaultDash::~ioExtendAssaultDash()
{
	m_vAniRotateInfoList.clear();
}

DashType ioExtendAssaultDash::GetType() const
{
	return DT_EXTEND_ASSAULT;
}

void ioExtendAssaultDash::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendDash::LoadProperty( rkLoader );

	char szKey[MAX_PATH];

	// 기본
	LoadAttackAttribute_e( "dash_action_attack", m_AttackAttribute, rkLoader );

	// Extra
	int iMax = rkLoader.LoadInt_e( "extra_dash_max_cnt", 0 );
	for( int i=0; i < iMax; ++i )
	{
		AttackAttribute eAttribute;
		
		wsprintf_e( szKey, "extra_dash_attack%d", i+1);
		LoadAttackAttribute( szKey, eAttribute, rkLoader );

		m_ExtraAttackList.push_back( eAttribute );
	}

	m_nMaxRepeatDashCnt = rkLoader.LoadInt_e( "max_repeat_dash", 0 );

	LoadAttackAttribute_e( "dash_repeat_action", m_RepeatDashInfo, rkLoader );

	m_bSetEnableDash = rkLoader.LoadBool_e( "set_enable_dash", false );
	m_bEnableAttackAfterDash = rkLoader.LoadBool_e( "set_enable_attack_dash", false );
}

bool ioExtendAssaultDash::StartDash( ioBaseChar *pOwner )
{
	ioExtendDash::StartDash( pOwner );

	m_bSetExtendAssault = false;
	m_iCurExtendCnt = 0;
	m_nCurRepeatDashCnt = 0;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_AttackAttribute.m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioExtendAssaultDash::StartDash - %s Animation is Not Exist",
								m_AttackAttribute.m_AttackAnimation.c_str() );
		return false;
	}

	float fTimeRate = m_AttackAttribute.m_fAttackAniRate;

	if( pOwner->IsOwnerChar() && pOwner->GetCurItemSkillGauge() < m_fDashExtraGauge )
		return false;

	m_dwActionEndTime = 0;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->RefreshFireTimeList( iAniID,
							     m_AttackAttribute.m_vWeaponInfoList,
								 FTT_EXTEND_DASH_ATTACK,
								 fTimeRate,
								 m_AttackAttribute.m_dwPreDelay );

	pOwner->InitExtendAttackTagInfo();

	if( pOwner->IsNeedProcess() )
		pOwner->SetDashAttackEndJumpInfo( iAniID, fTimeRate, m_AttackAttribute.m_dwPreDelay );

	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, m_AttackAttribute.m_dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetNoWoundState( false );
	pOwner->SetState( CS_EXTEND_DASH );
	pOwner->SetAutoTarget( ATT_DASH );
	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, (float)m_AttackAttribute.m_dwPreDelay, true );

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwDuration = m_AttackAttribute.m_dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + m_AttackAttribute.m_dwEndDelay;

	m_dwActionEndTime = dwCurTime + dwDuration;

	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetReservedSliding( m_AttackAttribute.m_vForceInfoList, iAniID, fTimeRate, m_AttackAttribute.m_dwPreDelay );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, m_AttackAttribute.m_dwPreDelay );
	pOwner->SetInputCancelInfo( m_AttackAttribute.m_AttackAnimation, m_AttackAttribute.m_fAttackAniRate );

	DWORD dwTrackingTime = FRAMEGETTIME() + m_AttackAttribute.m_dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_bEnableDashState = false;
	pOwner->ClearDirDoubleClick();

	pOwner->ReduceCurSkillGaugeBySkill( NULL, m_fDashExtraGauge );

	iolupinItem *pLupin = ToLupinItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pLupin )
	{
		pLupin->SetMoveDummyPos( pOwner, true, false, true );
	}

	g_TutorialMgr.ActionAssaultDash();
	return true;
}

void ioExtendAssaultDash::SetExtraInfoToWeapon( ioBaseChar *pOwner, ioPlayStage *pStage, ioWeapon *pWeapon, const ioHashString &szName )
{
	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
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

void ioExtendAssaultDash::CheckKeyReserve( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	bool bReserveSkill = false;
	int iSkillNum = pOwner->GetSkillKeyInput();
	if( iSkillNum >= 0 )
	{
		m_iSkillInput = iSkillNum;
		bReserveSkill = true;
	}

	if( pOwner->IsDefenseKeyDown() && !m_bEnableDefenseState && !bReserveSkill )
	{
		m_bEnableDefenseState = true;
		m_bEnableAttackState = false;
		m_bEnableJumpState = false;
		m_iSkillInput = -1;
		m_bEnableDashState = false;
	}
	else if( pOwner->IsJumpKey() && !m_bEnableJumpState && !bReserveSkill )
	{
		m_bEnableJumpState = true;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;
		m_iSkillInput = -1;
		m_bEnableDashState = false;
	}
	else if( pOwner->IsAttackKey() && !m_bEnableAttackState && !bReserveSkill )
	{
		ioUserKeyInput::DirKeyInput eCurKey = pOwner->CheckCurDirKey();
		
		if( pOwner->IsSettedDirection() )
		{
			float fYawD = 180.0f + 45.0f * ( eCurKey - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, true, false );
		}

		SetExtendAssaultDash( pOwner );
		return;
	}
	else if ( m_nMaxRepeatDashCnt > 0 && m_nCurRepeatDashCnt < m_nMaxRepeatDashCnt &&
		      pOwner->IsDirKeyDoubleClick() && !m_bEnableDashState && !bReserveSkill )
	{
		pOwner->ClearDirDoubleClick();
		m_bEnableJumpState = false;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;
		m_iSkillInput = -1;
		m_bEnableDashState = true;
		m_nCurRepeatDashCnt++;

		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		pOwner->SetTargetRotToDirKey( eNewDirKey );
		pOwner->SetWorldOrientation( pOwner->GetTargetRot() );

		SetRepeatDash( pOwner );
	}
}

void ioExtendAssaultDash::CheckExtendReserve( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	bool bReserveSkill = false;
	int iSkillNum = pOwner->GetSkillKeyInput();
	if( iSkillNum >= 0 )
	{
		m_iSkillInput = iSkillNum;
		bReserveSkill = true;
	}

	if( pOwner->IsDefenseKeyDown() && !m_bEnableDefenseState && !bReserveSkill )
	{
		m_bEnableDefenseState = true;
		m_bEnableAttackState = false;
		m_bEnableJumpState = false;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsJumpKey() && !m_bEnableJumpState && !bReserveSkill )
	{
		m_bEnableJumpState = true;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;
		m_iSkillInput = -1;
	}
	else if( pOwner->IsAttackKey() && !m_bEnableAttackState && !bReserveSkill )
	{
		m_bEnableAttackState = true;
		m_bEnableDefenseState = false;
		m_bEnableJumpState = false;

		m_iSkillInput = -1;
	}
	else if ( m_bEnableAttackAfterDash && m_nMaxRepeatDashCnt > 0 && m_nCurRepeatDashCnt < m_nMaxRepeatDashCnt &&
		      pOwner->IsDirKeyDoubleClick() && !m_bEnableDashState && !bReserveSkill )
	{
		pOwner->ClearDirDoubleClick();
		m_bEnableJumpState = false;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;
		m_iSkillInput = -1;
		m_bEnableDashState = true;
		m_nCurRepeatDashCnt++;

		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		pOwner->SetTargetRotToDirKey( eNewDirKey );
		pOwner->SetWorldOrientation( pOwner->GetTargetRot() );

		SetRepeatDash( pOwner );
	}
}

void ioExtendAssaultDash::ProcessDash( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwEndTime = m_dwActionEndTime + pOwner->GetActionStopTotalDelay();

	if( dwEndTime > dwCurTime )
	{
		if( m_bSetExtendAssault )
			CheckAniRotate( pOwner );

		if( m_dwReserveEnableTime > 0 && m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			// 예약체크
			if( m_bSetExtendAssault && m_iCurExtendCnt >= (int)m_ExtraAttackList.size() )
				CheckExtendReserve( pOwner );
			else
				CheckKeyReserve( pOwner );
			return;
		}
	}
	else
	{
		iolupinItem *pLupin = ToLupinItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
		if( pLupin )
			m_dwActionEndTime = 0;
		/*if ( m_bEnableDashState )
			SetRepeatDash( pOwner );
		else*/
			ProcessKeyReserve( pOwner );
	}
}

void ioExtendAssaultDash::SetExtendAssaultDash( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( !COMPARE( m_iCurExtendCnt, 0, (int)m_ExtraAttackList.size() ) )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	const AttackAttribute &rkAttribute = m_ExtraAttackList[m_iCurExtendCnt];

	int iAniID = pGrp->GetAnimationIdx( rkAttribute.m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioExtendAssaultDash::SetExtendAssaultDash - %s Animation is Not Exist",
								rkAttribute.m_AttackAnimation.c_str() );
		return;
	}

	float fTimeRate = rkAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = rkAttribute.m_dwPreDelay;

	if( pOwner->IsOwnerChar() && pOwner->GetCurItemSkillGauge() < m_fDashExtraGauge )
		return;

	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_bEnableDashState = false;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->RefreshFireTimeList( iAniID,
								 rkAttribute.m_vWeaponInfoList,
								 FTT_EXTEND_DASH_ATTACK,
								 fTimeRate,
								 dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetNoWoundState( false );

	pOwner->SetState( CS_EXTEND_DASH );
	pOwner->SetAutoTarget( ATT_DASH );
	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, (float)dwPreDelay, true );

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwDuration = dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + rkAttribute.m_dwEndDelay;

	m_dwActionEndTime = dwCurTime + dwDuration;

	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetReservedSliding( rkAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	DWORD dwTrackingTime = FRAMEGETTIME() + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	//
	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;
	m_vAniRotateInfoList.clear();

	AniEventConstPtrList vAniRotateList;
	pGrp->GetAnimationEventConstPtrList( iAniID, "ani_rotate", vAniRotateList );

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
	//

	m_bSetExtendAssault = true;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_iCurExtendCnt;
		kPacket << pOwner->GetWorldOrientation();
		kPacket << ST_Attack;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	// send 이후
	m_iCurExtendCnt++;
}

void ioExtendAssaultDash::ApplyDash( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwDelayTime = 0;
	DWORD dwCurTime = FRAMEGETTIME();

	rkPacket >> m_iCurExtendCnt;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pOwner->SetTargetRotAndMoveDirChange( qtRot );

	int nState;
	rkPacket >> nState;

	switch( nState )
	{
	case ST_Attack:
		SetExtendAssaultDash( pOwner );
		break;
	case ST_Repeat:
		SetRepeatDash( pOwner );
		break;
	}
}

void ioExtendAssaultDash::CheckAniRotate( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( m_vAniRotateInfoList.empty() ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwAniRotateTime > 0 && m_dwAniRotateTime < dwCurTime )
	{
		if( m_fAniRotateAngle > 0.0f )
		{
			D3DXQUATERNION qtRot;
			D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(m_fAniRotateAngle) );
			D3DXVECTOR3 vMoveDir = qtRot * pChar->GetMoveDir();

			D3DXQUATERNION qtTargetRot;
			ioMath::CalcDirectionQuaternion( qtTargetRot, -vMoveDir );

			pChar->SetTargetRotAndMoveDirChange( qtTargetRot );
		}

		m_iCurAniRotate++;

		int iSize = m_vAniRotateInfoList.size();
		if( COMPARE(m_iCurAniRotate, 0, iSize) )
		{
			m_dwAniRotateTime = m_vAniRotateInfoList[m_iCurAniRotate].m_dwTime;
			m_fAniRotateAngle = m_vAniRotateInfoList[m_iCurAniRotate].m_fAngle;
		}
		else
		{
			m_dwAniRotateTime = 0;
			m_fAniRotateAngle = 0.0f;

		}
	}
}

void ioExtendAssaultDash::SetRepeatDash( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_RepeatDashInfo.m_AttackAnimation );
	if( iAniID == -1 )
		return;

	float fTimeRate = m_RepeatDashInfo.m_fAttackAniRate;
	DWORD dwPreDelay = m_RepeatDashInfo.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->RefreshFireTimeList( iAniID, m_RepeatDashInfo.m_vWeaponInfoList, FTT_EXTEND_DASH_ATTACK, fTimeRate, dwPreDelay );

	pOwner->InitExtendAttackTagInfo();

	if( pOwner->IsNeedProcess() )
		pOwner->SetDashAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetNoWoundState( false );

	pOwner->SetAutoTarget( ATT_DASH );

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwDuration = dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + m_RepeatDashInfo.m_dwEndDelay;

	m_dwActionEndTime = dwCurTime + dwDuration;
	m_dwReserveEnableTime = 0;

	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetReservedSliding( m_RepeatDashInfo.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	DWORD dwTrackingTime = FRAMEGETTIME() + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_bEnableDashState = false;
	pOwner->ClearDirDoubleClick();

	if ( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = ioUserKeyInput::DKI_NONE;
		if ( pOwner->IsSettedDirection() )
		{
			eNewDirKey = pOwner->CheckCurDirKey();
			pOwner->SetTargetRotToDirKey( eNewDirKey );
			pOwner->SetWorldOrientation( pOwner->GetTargetRot() );
		}
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_iCurExtendCnt;
		kPacket << pOwner->GetWorldOrientation();
		kPacket << ST_Repeat;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioExtendAssaultDash::IsDashEnable( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	if( m_bSetEnableDash )
	{
		DWORD dwCurTime = FRAMEGETTIME();
		if ( m_dwActionEndTime > dwCurTime )
		{
			if( m_dwReserveEnableTime > 0 && m_dwReserveEnableTime < dwCurTime )
				return true;
		}
	}

	return false;
}

AttackAttribute& ioExtendAssaultDash::GetDashAttackAttribute()
{
	return m_AttackAttribute;
}