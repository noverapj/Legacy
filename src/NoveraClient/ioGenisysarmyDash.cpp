#include "stdafx.h"
#include "ioGenisysarmyDash.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ItemDefine.h"
#include "WeaponDefine.h"

ioGenisysarmyDash::ioGenisysarmyDash()
{
	m_dwReserveEnableTime	= 0;
	m_bSetGenisysarmy		= false;
	m_iCurExtendCnt			= 0;
	m_dwActionEndTime		= 0;

	m_dwAniRotateTime		= 0;
	m_fAniRotateAngle		= 0.0f;
	m_iCurAniRotate			= 0;
	m_vAniRotateInfoList.clear();
}

ioGenisysarmyDash::ioGenisysarmyDash( const ioGenisysarmyDash &rhs )
: ioExtendDash( rhs ),
 m_AttackAttribute( rhs.m_AttackAttribute ),
 m_ExtraAttackAttribute( rhs.m_ExtraAttackAttribute ),
 m_iExtraAttackMaxCnt( rhs.m_iExtraAttackMaxCnt )
{
	m_dwReserveEnableTime	= 0;
	m_bSetGenisysarmy		= false;
	m_iCurExtendCnt			= 0;
	m_dwActionEndTime		= 0;
	m_dwAniRotateTime		= 0;
	m_fAniRotateAngle		= 0.0f;
	m_iCurAniRotate			= 0;
	m_vAniRotateInfoList.clear();
}

ioExtendDash* ioGenisysarmyDash::Clone()
{
	return new ioGenisysarmyDash( *this );
}

ioGenisysarmyDash::~ioGenisysarmyDash()
{
	m_vAniRotateInfoList.clear();
}

DashType ioGenisysarmyDash::GetType() const
{
	return DT_GENISYSARMY;
}

void ioGenisysarmyDash::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendDash::LoadProperty( rkLoader );

	// 대시 구르기
	LoadAttackAttribute_e( "dash_action_attack", m_AttackAttribute, rkLoader );

	// 대시 +D
	m_iExtraAttackMaxCnt = rkLoader.LoadInt_e( "extra_dash_max_cnt", 0 );
	LoadAttackAttribute_e( "extra_dash_attack", m_ExtraAttackAttribute, rkLoader );	
}

// ioBaseChar::ProcessKeyInput() 함수 내부 SetDashState() 함수를 호출 
// 자기 자신 부터 StartDash 함수를 호출 해주고, SendDashState(false) 함수를 호출합니다.
bool ioGenisysarmyDash::StartDash( ioBaseChar *pOwner )
{
	ioExtendDash::StartDash( pOwner );

	m_bSetGenisysarmy	= false;
	m_iCurExtendCnt		= 0;
	m_dwActionEndTime	= 0;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_AttackAttribute.m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioGenisysarmyDash::StartDash - %s Animation is Not Exist",
								m_AttackAttribute.m_AttackAnimation.c_str() );
		return false;
	}

	float fTimeRate = m_AttackAttribute.m_fAttackAniRate;

	if( pOwner->IsOwnerChar() && pOwner->GetCurItemSkillGauge() < m_fDashExtraGauge )
		return false;



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
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, m_AttackAttribute.m_dwPreDelay ); // 캐릭터 관통
	pOwner->SetInputCancelInfo( m_AttackAttribute.m_AttackAnimation, m_AttackAttribute.m_fAttackAniRate );

	DWORD dwTrackingTime = FRAMEGETTIME() + m_AttackAttribute.m_dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	pOwner->ReduceCurSkillGaugeBySkill( NULL, m_fDashExtraGauge );

	pOwner->ClearDirDoubleClick();	// 2020-07-15

	return true;
}

void ioGenisysarmyDash::ProcessDash( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwEndTime = m_dwActionEndTime + pOwner->GetActionStopTotalDelay();

	if( dwEndTime > dwCurTime )
	{
		if( m_bSetGenisysarmy )
			CheckAniRotate( pOwner );

		if( m_dwReserveEnableTime > 0 && m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			// 예약체크
			if( m_bSetGenisysarmy && m_iCurExtendCnt >= m_iExtraAttackMaxCnt )
			{
				CheckExtendReserve( pOwner );
			}
			else
			{
				CheckKeyReserve( pOwner );
			}
			return;
		}
	}
	else
	{
		ProcessKeyReserve( pOwner );
		//pOwner->ClearDirDoubleClick();	// 2020-07-15
	}
}

void ioGenisysarmyDash::ApplyDash( ioBaseChar *pOwner, SP2Packet &rkPacket )
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
		case DS_ATTACK:
		{
			SetGenisysarmyDash( pOwner );
			break;
		}
	}
}

void ioGenisysarmyDash::SetExtraInfoToWeapon( ioBaseChar *pOwner, ioPlayStage *pStage, ioWeapon *pWeapon, const ioHashString &szName )
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

void ioGenisysarmyDash::CheckKeyReserve( ioBaseChar *pOwner )
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
		// +D 입력 시 방향키 입력 방향으로 권총 한발 발사
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

		SetGenisysarmyDash( pOwner );
		return;
	}
}

void ioGenisysarmyDash::CheckExtendReserve( ioBaseChar *pOwner )
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
}

void ioGenisysarmyDash::SetGenisysarmyDash( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( !COMPARE( m_iCurExtendCnt, 0, m_iExtraAttackMaxCnt ) )
	{
		return;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_ExtraAttackAttribute.m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioGenisysarmyDash::SetGenisysarmyDash - %s Animation is Not Exist",
								m_ExtraAttackAttribute.m_AttackAnimation.c_str() );
		return;
	}

	float fTimeRate = m_ExtraAttackAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = m_ExtraAttackAttribute.m_dwPreDelay;

	if( pOwner->IsOwnerChar() && pOwner->GetCurItemSkillGauge() < m_fDashExtraGauge )
		return;

	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->RefreshFireTimeList( iAniID,
								 m_ExtraAttackAttribute.m_vWeaponInfoList,
								 FTT_EXTEND_DASH_ATTACK,
								 fTimeRate,
								 dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetNoWoundState( false );

	pOwner->SetState( CS_EXTEND_DASH );
	pOwner->SetAutoTarget( ATT_DASH );
	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, (float)dwPreDelay, true );

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwDuration = dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + m_ExtraAttackAttribute.m_dwEndDelay;

	m_dwActionEndTime = dwCurTime + dwDuration;

	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetReservedSliding( m_ExtraAttackAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

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

	m_bSetGenisysarmy = true;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_iCurExtendCnt;
		kPacket << pOwner->GetWorldOrientation();
		kPacket << DS_ATTACK;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	// send 이후
	m_iCurExtendCnt++;
}

void ioGenisysarmyDash::CheckAniRotate( ioBaseChar *pChar )
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