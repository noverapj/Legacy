

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioExtraCounter.h"
#include "ItemDefine.h"

ioExtraCounter::ioExtraCounter()
{
	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_bSetExtendCAT = false;
	m_iCurExtraCnt = 0;

	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;

	m_iCurAniRotate = 0;
	m_vAniRotateInfoList.clear();
}

ioExtraCounter::ioExtraCounter( const ioExtraCounter &rhs )
: ioCounterAttack( rhs ),
 m_ExtraAttackList( rhs.m_ExtraAttackList ),
 m_bUseExtendJumpKeyCA( rhs.m_bUseExtendJumpKeyCA )
{
	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_bSetExtendCAT = false;
	m_iCurExtraCnt = 0;

	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;

	m_iCurAniRotate = 0;
	m_vAniRotateInfoList.clear();
}

ioExtraCounter::~ioExtraCounter()
{
	m_vAniRotateInfoList.clear();
}

void ioExtraCounter::LoadProperty( ioINILoader &rkLoader )
{
	ioCounterAttack::LoadProperty( rkLoader );

	char szKey[MAX_PATH] = "";
	int iMax = rkLoader.LoadInt_e( "extra_counter_max_cnt", 0 );
	for( int i=0; i < iMax; ++i )
	{
		AttackAttribute eAttribute;
		
		wsprintf_e( szKey, "extra_counter_attack%d", i+1);
		LoadAttackAttribute( szKey, eAttribute, rkLoader );

		m_ExtraAttackList.push_back( eAttribute );
	}

	m_bUseExtendJumpKeyCA = rkLoader.LoadBool_e( "use_extend_jump_key_counter_attack", 0 );
}

ioCounterAttack* ioExtraCounter::Clone()
{
	return new ioExtraCounter( *this );
}

CounterAttackType ioExtraCounter::GetType() const
{
	return CAT_BREAK;
}

bool ioExtraCounter::StartCAT( ioBaseChar *pOwner, int iCounterAttackKeyState )
{
	if ( iCounterAttackKeyState == CAKS_ATTACK_KEY )
		return StartAttackKeyCAT( pOwner );
	else
		return StartJumpKeyCAT( pOwner );
}

bool ioExtraCounter::StartAttackKeyCAT( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return false;

	m_bSetExtendCAT = false;
	m_iCurExtraCnt = 0;
	
	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	
	pOwner->SetNoWoundState( false );
	pOwner->InitExtendAttackTagInfo();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearInputCancelInfo();

	DWORD dwCurTime = FRAMEGETTIME();

	ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_DEFENSE );

	int iAniID	     = pGrp->GetAnimationIdx( m_CounterAttack.m_AttackAnimation );
	float fTimeRate  = m_CounterAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_CounterAttack.m_dwPreDelay;

	pOwner->RefreshFireTimeList( iAniID,
						 m_CounterAttack.m_vWeaponInfoList,
						 FTT_COUNTER_ATTACK,
						 fTimeRate,
						 dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	DWORD dwDuration = dwPreDelay + pGrp->GetAnimationFullTime(iAniID)*fTimeRate + m_CounterAttack.m_dwEndDelay;
	m_dwActionEndTime = dwCurTime + dwDuration;
	
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	m_dwReserveEnableTime = 0;
	float fReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( fReserveTime > 0.0f )
	{
		m_dwReserveEnableTime = dwCurTime + dwPreDelay + fReserveTime;
	}

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( m_CounterAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pOwner->SetState( CS_COUNTER_ATTACK );

	return true;
}

bool ioExtraCounter::StartJumpKeyCAT( ioBaseChar *pOwner )
{
	if ( m_bUseExtendJumpKeyCA )
		return StartJumpKeyExtendCAT( pOwner );
	else
		return StartJumpKeyDefaultCAT( pOwner );
}

bool ioExtraCounter::StartJumpKeyDefaultCAT( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return false;

	m_bSetExtendCAT = false;
	m_iCurExtraCnt = 0;

	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	pOwner->SetNoWoundState( false );
	pOwner->InitExtendAttackTagInfo();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearInputCancelInfo();

	DWORD dwCurTime = FRAMEGETTIME();

	ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_DEFENSE );

	int iAniID	     = pGrp->GetAnimationIdx( m_JumpKeyCounterAttack.m_AttackAnimation );
	float fTimeRate  = m_JumpKeyCounterAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_JumpKeyCounterAttack.m_dwPreDelay;

	pOwner->RefreshFireTimeList( iAniID,
		m_JumpKeyCounterAttack.m_vWeaponInfoList,
		FTT_COUNTER_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	DWORD dwDuration = dwPreDelay + pGrp->GetAnimationFullTime(iAniID)*fTimeRate + m_JumpKeyCounterAttack.m_dwEndDelay;
	m_dwActionEndTime = dwCurTime + dwDuration;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	m_dwReserveEnableTime = 0;
	float fReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( fReserveTime > 0.0f )
	{
		m_dwReserveEnableTime = dwCurTime + dwPreDelay + fReserveTime;
	}

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( m_JumpKeyCounterAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pOwner->SetState( CS_COUNTER_ATTACK );

	return true;
}

bool ioExtraCounter::StartJumpKeyExtendCAT( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	ioWeaponItem *pItem = pOwner->GetWeapon();
	ioGalfordItem *pGalford = ToGalfordItem( pItem );
	if ( pGalford && pGalford->IsEnableJumpCounterAttack() )
	{
		pGalford->SetJumpCountAttackState( pOwner );
		return true;
	}

	return false;
}


void ioExtraCounter::ProcessCAT( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwStopDelay = pOwner->GetActionStopTotalDelay();
	DWORD dwEndTime = m_dwActionEndTime + dwStopDelay;

	ioParkilpyoItem *pParkilpyoItem = ToParkilpyoItem( pOwner->GetWeapon() );
	if( pParkilpyoItem && pParkilpyoItem->CheckCmdInputState( pOwner ) )
		return;

	if( dwEndTime > dwCurTime )
	{
		if( m_bSetExtendCAT )
			CheckAniRotate( pOwner );
		
		if( m_dwReserveEnableTime > 0 && m_dwReserveEnableTime + dwStopDelay < dwCurTime )
		{
			// 예약체크
			if( m_bSetExtendCAT && m_iCurExtraCnt >= (int)m_ExtraAttackList.size() )
				CheckExtendReserve( pOwner );
			else
				CheckReserve( pOwner );

			return;
		}
	}
	else
	{
		// 공격키가 눌러져 있으면 공격 상태 처리
		if( pOwner->IsAttackKeyDown() && 
			!pOwner->IsJumpKeyDown() &&
			!pOwner->IsDefenseKeyDown() &&
			!pOwner->IsChatModeState(true) )
		{
			ioObjectItem *pItem = pOwner->GetObject();

			if( ToDrinkItem( pItem ) )
			{
				pOwner->SetDrinkState();
			}
			else
			{
				if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
				{
					pOwner->SetNormalAttack( 0 );
				}
			}

			return;
		}
		
		// 필살기 예약되었으면 필살기 발동
		if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
		{
			m_iSkillInput = -1;
			return;
		}

		// 방어
		if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
		{
			pOwner->SetDefenseState();
			return;
		}

		// 점프
		if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
		{
			pOwner->SetJumpState();
			return;
		}

		// 공격
		if( m_bEnableAttackState )
		{
			ioObjectItem *pItem = pOwner->GetObject();

			if( ToDrinkItem( pItem ) )
			{
				pOwner->SetDrinkState();
			}
			else
			{
				if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
				{
					pOwner->SetNormalAttack( 0 );
				}
			}
			return;
		}

		// 대기
		pOwner->ClearAttackFireTimeAndSkill();
		pOwner->ClearReservedSliding();

		pOwner->SetState( CS_DELAY );
	}
}

void ioExtraCounter::SetExtendCAT( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	if( !COMPARE( m_iCurExtraCnt, 0, (int)m_ExtraAttackList.size() ) )
		return;
	
	const AttackAttribute &rkAttribute = m_ExtraAttackList[m_iCurExtraCnt];

	int iAniID = pGrp->GetAnimationIdx( rkAttribute.m_AttackAnimation );
	if( iAniID == -1 )
		return;

	float fTimeRate = rkAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = rkAttribute.m_dwPreDelay;

	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( !pOwner->IsActionStop() )
		pOwner->InitActionStopDelay( true );
	else
		pOwner->InitActionStopDelay( false );

	pOwner->SetAutoTarget( ATT_NORMAL );

	pOwner->RefreshFireTimeList( iAniID,
								 rkAttribute.m_vWeaponInfoList,
								 FTT_COUNTER_ATTACK,
								 fTimeRate,
								 dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, (float)dwPreDelay, true );

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwDuration = dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + rkAttribute.m_dwEndDelay;

	m_dwActionEndTime = dwCurTime + dwDuration;

	float fReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( fReserveTime > 0.0f )
	{
		m_dwReserveEnableTime = dwCurTime + fReserveTime;
	}

	pOwner->SetReservedSliding( rkAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	//
	DWORD dwTrackingTime = dwCurTime + dwPreDelay;
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
	m_bSetExtendCAT = true;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_COUNTER_ATTACK_SYNC );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_iCurExtraCnt;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	// send 이후
	m_iCurExtraCnt++;
}

void ioExtraCounter::ApplyCAT( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwDelayTime = 0;
	DWORD dwCurTime = FRAMEGETTIME();

	rkPacket >> m_iCurExtraCnt;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	pOwner->SetTargetRotToRotate( qtRot, true, false );
	SetExtendCAT( pOwner );

	pOwner->ApplyAutoTargetInfo( rkPacket );
}

void ioExtraCounter::CheckReserve( ioBaseChar *pOwner )
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

		SetExtendCAT( pOwner );
		return;
	}
}

void ioExtraCounter::CheckExtendReserve( ioBaseChar *pOwner )
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

void ioExtraCounter::CheckAniRotate( ioBaseChar *pChar )
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
