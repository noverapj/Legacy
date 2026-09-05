

#include "stdafx.h"

#include "ioTargetCircleMoveSkill.h"
#include "WeaponDefine.h"
#include "ioEtcBuff.h"

ioTargetCircleMoveSkill::ioTargetCircleMoveSkill()
{
	ClearData();
}

ioTargetCircleMoveSkill::ioTargetCircleMoveSkill( const ioTargetCircleMoveSkill &rhs )
: ioNormalSkill( rhs ),
 m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
 m_CircleMoveLoopMotion( rhs.m_CircleMoveLoopMotion ),
 m_dwLoopDuration( rhs.m_dwLoopDuration ),
 m_fMaxAngle( rhs.m_fMaxAngle ),
 m_fMinAngle( rhs.m_fMinAngle ),
 m_fAimRange( rhs.m_fAimRange ),
 m_fAimOffset( rhs.m_fAimOffset ),
 m_iMaxFireCount( rhs.m_iMaxFireCount ),
 m_szAimMark( rhs.m_szAimMark ),
 m_AimAttack( rhs.m_AimAttack ),
 m_szAimAttackBuff( rhs.m_szAimAttackBuff ),
 m_fCircleRange( rhs.m_fCircleRange ),
 m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
 m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
 m_fCircleOffSet( rhs.m_fCircleOffSet ),
 m_dwCircleMoveTime( rhs.m_dwCircleMoveTime ),
 m_dwCircleActionTime( rhs.m_dwCircleActionTime ),
 m_ReadyCircle( rhs.m_ReadyCircle ),
 m_szCircleActionBuff( rhs.m_szCircleActionBuff ),
 m_szCircleMoveBuff( rhs.m_szCircleMoveBuff ),
 m_CircleActionAttack( rhs.m_CircleActionAttack ),
 m_fCircleActionReturnCheckLength( rhs.m_fCircleActionReturnCheckLength )
{
	ClearData();
}

ioTargetCircleMoveSkill::~ioTargetCircleMoveSkill()
{
	ClearData();
}

void ioTargetCircleMoveSkill::ClearData()
{
	m_bSetHandMesh = false;

	m_SkillState = SS_NONE;
	m_dwLoopStartTime	= 0;

	m_dwFireStartTime = 0;

	m_szTargetName.Clear();
	m_dwAimMarkID	= -1;
}

void ioTargetCircleMoveSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_SkillLoopMotion = szBuf;
	rkLoader.LoadString_e( "circle_move_loop_motion", "", szBuf, MAX_PATH );
	m_CircleMoveLoopMotion = szBuf;

	m_dwLoopDuration = rkLoader.LoadInt_e( "skill_loop_duration", 0 );

	// Limit Angle
	m_fMaxAngle = rkLoader.LoadFloat_e( "angle_max", 45.0f );
	m_fMinAngle = rkLoader.LoadFloat_e( "angle_min", 45.0f );
	m_fAimRange = rkLoader.LoadFloat_e( "aim_target_range", 0.0f );
	m_fAimOffset = rkLoader.LoadFloat_e( "aim_target_offset", 10.0f );

	m_iMaxFireCount = rkLoader.LoadInt_e( "max_fire_count", 0 );
	rkLoader.LoadString_e( "aim_mark_effect", "", szBuf, MAX_PATH );
	m_szAimMark = szBuf;
	rkLoader.LoadString_e( "aim_attack_buff", "", szBuf, MAX_PATH );
	m_szAimAttackBuff = szBuf;
	LoadAttackAttribute_e( "aim_attack", m_AimAttack, rkLoader );

	m_fCircleRange = rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );
	m_dwCircleMoveTime = rkLoader.LoadInt_e( "magic_circle_move_time", 0 );
	m_dwCircleActionTime = rkLoader.LoadInt_e( "magic_circle_action_time", 0 );
	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;

	rkLoader.LoadString_e( "magic_circle_action_buff", "", szBuf, MAX_PATH );
	m_szCircleActionBuff = szBuf;
	rkLoader.LoadString_e( "magic_circle_move_buff", "", szBuf, MAX_PATH );
	m_szCircleMoveBuff = szBuf;
	m_fCircleActionReturnCheckLength = rkLoader.LoadFloat_e( "magic_circle_action_return_check_length", 0.0f );
	LoadAttackAttribute_e( "magic_circle_action_attack", m_CircleActionAttack, rkLoader );
}

ioSkill* ioTargetCircleMoveSkill::Clone()
{
	return new ioTargetCircleMoveSkill( *this );
}

bool ioTargetCircleMoveSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !HasSkillMotion() )
		return false;

	ClearData();

	pChar->SetSkillProtection();

	m_dwAttackEndTime = 0;

	InitExtraAniJump();

	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;

	m_iCurAniRotate = 0;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	SetCameraBuff( pChar );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		SetLoopState( pChar, g_SkillMaker.GetPlayStage() );
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

	pChar->AttachEffect( m_SkillEffectName );

	CheckSkillDesc( pChar );

	m_vAttackDir = pChar->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &m_vAttackDir, &m_vAttackDir );

	m_iCurFireCount = 0;
	SetSkillNameChatBubble( pChar );
	return true;
}

void ioTargetCircleMoveSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime != 0 && m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;

				SetLoopState( pChar, pStage );
			}
		}
		break;
	case SS_LOOP:
		{
			CheckAimTime();
			CheckKeyInput( pChar );
			CheckTarget( pChar, pStage );
			CheckVDir( pChar );
			UpdateAimMark( pChar, pStage );

			if( pChar->IsNeedProcess() )
			{
				if( m_dwLoopStartTime > 0 && m_dwLoopStartTime+m_dwLoopDuration < dwCurTime )
				{
					SetFireState( pChar );
					return;
				}
				else if( pChar->IsAttackKey() )
				{
					SetFireState( pChar );
					return;
				}
			}
		}
		break;
	case SS_FIRE:
		{
			if( m_iCurFireCount < m_iMaxFireCount )
			{
				if( m_dwMotionEndTime < FRAMEGETTIME() )
					SetLoopState( pChar, pStage );
			}

			if( m_dwFireStartTime > 0 && m_dwFireStartTime < dwCurTime )
			{
				m_dwFireStartTime = 0;
			}
		}
		break;
	case SS_CIRCLE_MOVE:
		{
			if( m_dwAttackEndTime <= dwCurTime || !CheckTargetBuff( pChar, m_szCircleMoveBuff ) )
			{
				SetLoopState( pChar, pStage );
				return;
			}
			UpdateMagicCircle( pChar );
			CheckRotate( pChar );

			if( pChar->IsNeedProcess() )
			{
				if( pChar->IsAttackKey() )
				{
					SetCircleActionState( pChar, m_vCirclePos );
					return;
				}
			}
		}
		break;
	case SS_CIRCLE_ACTION:
		{
			if( m_dwAttackEndTime <= dwCurTime || !CheckTargetBuff( pChar, m_szCircleActionBuff ) )
			{
				SetCircleActionEndState( pChar );
			}
			else if( pChar->IsNeedSendNetwork() || !pChar->GetCreator()->IsNetworkMode() || !(P2PNetwork::IsNetworkPlaying()) )
			{
				WaitTargetReturn( pChar );
			}
		}
		break;
	}
}

void ioTargetCircleMoveSkill::CheckVDir( ioBaseChar *pChar )
{
	if( m_szTargetName.IsEmpty() )
		return;

	ioBaseChar *pTarget = pChar->GetBaseChar( m_szTargetName );
	if( !pTarget )	return;

	D3DXVECTOR3 vPos1 = pChar->GetWorldPosition();
	D3DXVECTOR3 vPos2 = pTarget->GetWorldPosition();

	pChar->SetTargetRotToTargetPos( vPos2, true );

	m_vAttackDir = vPos2 - vPos1;
	D3DXVec3Normalize( &m_vAttackDir, &m_vAttackDir );
}

void ioTargetCircleMoveSkill::OnSkillEnd( ioBaseChar *pChar )
{
	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( m_bSetHandMesh && !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_bSetHandMesh = false;

	EndPreDelayEffect( pChar );
	pChar->EndEffect( m_SkillEffectName, false );
	pChar->SetMagicCircleTarget( "" );
	pChar->DestroyMagicCircle();

	EndCameraBuff( pChar );

	m_bReduceGauge = false;
	pChar->SetSkillMoveEnable( false, false );
	pChar->SetMoveDirByRotate( pChar->GetTargetRot() );
	pChar->RestoreSkillProtection();
	pChar->SetCurMoveSpeed( 0.0f );

	m_SkillState = SS_NONE;
	m_SkillUseType = SUT_NORMAL;

	if( !m_SkillEndEffectName.IsEmpty() )
		pChar->AttachEffect( m_SkillEndEffectName );

	DestroyAimMark( pChar );

	ioBaseChar *pTarget = pChar->GetBaseChar( m_szTargetName );
	if( !pTarget )
		return;

	pTarget->RemoveBuff( m_szCircleMoveBuff );
	pTarget->RemoveBuff( m_szCircleActionBuff );
	m_szTargetName.Clear();
}

bool ioTargetCircleMoveSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_LOOP:
	case SS_CIRCLE_MOVE:
	case SS_CIRCLE_ACTION:
		return false;
	case SS_FIRE:
		{
			if( m_dwMotionEndTime + dwActionStop > FRAMEGETTIME() )
				return false;

			if( m_iCurFireCount < m_iMaxFireCount )
				return false;
		}
		return true;
	case SS_END:
		return true;
	}

	return true;
}

bool ioTargetCircleMoveSkill::IsEnableReserve() const
{
	if( m_dwPreDelayStartTime != 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_LOOP:
	case SS_CIRCLE_MOVE:
	case SS_CIRCLE_ACTION:
		return false;
	case SS_FIRE:
		if( m_dwEnableReserveTime > 0 && m_dwEnableReserveTime < FRAMEGETTIME() )
			return true;

		break;
	}

	return false;
}

void ioTargetCircleMoveSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case SNT_CHANGE:
		rkPacket >> m_szTargetName;
		break;
	case SNT_ATTACK:
		{
			rkPacket >> m_vAttackDir;
			SetFireState( pChar );
		}
		break;
	case SNT_CIRCLE_MOVE:
		{
			rkPacket >> m_szTargetName;
			SetCircleMoveState( pChar );
		}
		break;
	case SNT_CIRCLE_ACTION:
		{
			rkPacket >> m_szTargetName;
			rkPacket >> m_vCirclePos;
			SetCircleActionState( pChar, m_vCirclePos );
		}
		break;
	case SNT_CIRCLE_END:
		{
			D3DXVECTOR3 vFirePos, vFireDir;
			rkPacket >> m_szTargetName;
			rkPacket >> vFirePos;
			rkPacket >> vFireDir;
			ApplyCircleActionEndState( pChar, vFirePos, vFireDir );
		}
		break;
	}
}

void ioTargetCircleMoveSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	if( !pWeapon ) return;

	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );
}

void ioTargetCircleMoveSkill::SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwLoopStartTime = dwCurTime;

	EndPreDelayEffect( pChar );

	m_SkillState = SS_LOOP;

	pChar->SetMagicCircleTarget( "" );
	pChar->DestroyMagicCircle();

	pGrp->ClearAllActionAni( FLOAT100 );
	pGrp->ClearAllLoopAni( FLOAT100 );
	pGrp->SetLoopAni( m_SkillLoopMotion, FLOAT100, FLOAT1 );
}

void ioTargetCircleMoveSkill::SetFireState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	DestroyAimMark( pChar );
	AttackSkillFire( pChar );

	m_SkillState = SS_FIRE;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SNT_ATTACK;
		kPacket << m_vAttackDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetCircleMoveSkill::SetCircleMoveState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	DestroyAimMark( pChar );

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_CircleMoveLoopMotion );
	if( iAniID == -1 )	
		return;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100 );	

	//마법진 생성
	CreateMagicCircle( pChar );

	m_SkillState = SS_CIRCLE_MOVE;
	m_dwAttackEndTime = FRAMEGETTIME() + m_dwCircleMoveTime;

	ioBaseChar *pTarget = pChar->GetBaseChar( m_szTargetName );
	if( !pTarget )
		return;

	pTarget->AddNewBuff( m_szCircleMoveBuff, pChar->GetCharName(), GetName(), NULL );
}

void ioTargetCircleMoveSkill::AttackSkillFire( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_AimAttack.m_AttackAnimation );
	if( iAniID == -1 )	return;

	if( m_iCurFireCount >= m_iMaxFireCount )
	{
		m_SkillState = SS_END;
		return;
	}

	WeaponInfoList vWeaponInfoList = m_AimAttack.m_vWeaponInfoList;
	float fTimeRate  = m_AimAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_AimAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	
	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	++m_iCurFireCount;

	D3DXVECTOR3 vFirePos;
	ioBaseChar *pTarget = pChar->GetBaseChar( m_szTargetName );
	if( pTarget )
	{
		vFirePos = pTarget->GetMidPositionByRate();
	}
	else
	{
		vFirePos = pChar->GetMidPositionByRate();
		vFirePos += pChar->GetMoveDir() * m_fAimOffset;
	}
	if( !vWeaponInfoList.empty() )
		pChar->WeaponByWeaponExplicitList( vWeaponInfoList, FTT_NORMAL_ATTACK, vFirePos, m_vAttackDir );
	if( pTarget )
		pTarget->AddNewBuff( m_szAimAttackBuff, pChar->GetCharName(), "", NULL );

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + m_AimAttack.m_dwEndDelay;

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwFireStartTime = 0;
	m_dwFireStartTime = m_dwFireMotionEndTime = dwCurTime;
	m_dwFireStartTime += dwPreDelay;
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	m_dwAttackEndTime = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( m_dwAttackEndTime > 0 )
		m_dwAttackEndTime += dwCurTime;
	else
		m_dwAttackEndTime = 0;

	SetExtraAniJump( pChar, iAniID, fTimeRate );

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

	// Col Skip
	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	m_dwFireMotionEndTime += dwPreDelay;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	m_dwEnableReserveTime = 0;
	DWORD dwReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( dwReserveTime > 0 )
		m_dwEnableReserveTime = dwCurTime + dwReserveTime;

	if( m_dwFireStartTime >= m_dwFireMotionEndTime )
	{
		m_dwFireMotionEndTime = m_dwMotionEndTime;
	}

	pGrp->SetActionAni( iAniID, 0.0f, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pChar->SetReservedSliding( m_AimAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;

}

bool ioTargetCircleMoveSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioTargetCircleMoveSkill::CheckAimTime()
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_iCurFireCount >= m_iMaxFireCount )
	{
		m_SkillState = SS_END;
		return;	
	}
}

void ioTargetCircleMoveSkill::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

	if( pOwner->IsJumpKey() )
	{
		m_bJumpKey = true;
		m_bDefenseKey = false;
	}
	else if( pOwner->IsDefenseKey() )
	{
		m_bJumpKey = false;
		m_bDefenseKey = true;
	}
	else
	{
		m_bJumpKey = false;
		m_bDefenseKey = false;
	}
}

void ioTargetCircleMoveSkill::CheckTarget( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pStage )	return;
	if( !pChar )	return;


	if( !pChar->IsNeedProcess() )
		return;

	float fPrevDistSq = m_fAimRange*m_fAimRange;
	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	
	m_vTargetInfoList.clear();
	int iSize = rkTargetList.size();
	m_vTargetInfoList.reserve(iSize);
	
	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);
		if( pTarget == pChar )
			continue;

		if( pChar->GetCharName() == pTarget->GetCharName() )
			continue;

		if( pChar->GetTeam() == pTarget->GetTeam() )
			continue;

		if( pTarget->IsSystemState() )
			continue;
		
		if( !pTarget->IsEnableTarget() ) 
			continue;
		
		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - pChar->GetWorldPosition();
		float fDistSq = D3DXVec3LengthSq( &vDiff );

		TargetInfo	kInfo;
		kInfo.m_TargetName = pTarget->GetCharName();
		kInfo.m_fLenth = fDistSq;
		
		float fEnableDistSq = m_fAimRange*m_fAimRange;
		if( fDistSq < fEnableDistSq )	// Enemy
		{
			m_vTargetInfoList.push_back(kInfo);
		}
	}

	if( m_szTargetName.IsEmpty() )
	{
		int iCnt = m_vTargetInfoList.size();
		for( int i=0; i<iCnt; ++i )
		{
			float fDistSq = m_vTargetInfoList[i].m_fLenth;
			if( fDistSq < fPrevDistSq )	// Enemy
			{
				fPrevDistSq = fDistSq;
				m_szTargetName = m_vTargetInfoList[i].m_TargetName;
			}
		}

		if( !m_szTargetName.IsEmpty() && pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SNT_CHANGE;
			kPacket << m_szTargetName;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	else if( !m_vTargetInfoList.empty() )
	{
		int iCurTarget = -1;
		int iCnt = m_vTargetInfoList.size();
		for( int i=0; i<iCnt; ++i )
		{
			if( m_szTargetName == m_vTargetInfoList[i].m_TargetName )
				iCurTarget = i;
		}

		if( iCurTarget != -1 )
		{
			bool bTargetChange = false;
			if( m_bJumpKey )
			{
				iCurTarget++;
				bTargetChange = true;
				if( iCurTarget >= iCnt )
					iCurTarget = 0;
			}
			else if( m_bDefenseKey )
			{
				bTargetChange = true;
				iCurTarget--;
				if( iCurTarget < 0 )
					iCurTarget = iCnt-1;
			}

			m_szTargetName = m_vTargetInfoList[iCurTarget].m_TargetName;
			if( bTargetChange && pChar->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pChar->GetCharName();
				kPacket << GetName();
				kPacket << SNT_CHANGE;
				kPacket << m_szTargetName;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
		else
			m_szTargetName.Clear();
	}
	else
		m_szTargetName.Clear();
}

void ioTargetCircleMoveSkill::UpdateAimMark( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( m_iCurFireCount >= m_iMaxFireCount )
	{
		m_SkillState = SS_END;
		return;
	}

	D3DXVECTOR3 vPos;
	ioBaseChar *pTarget = pStage->GetBaseChar( m_szTargetName );
	if( pTarget )
	{
		vPos = pTarget->GetMidPositionByRate();
	}
	else
	{
		vPos = pChar->GetMidPositionByRate();
		vPos += pChar->GetMoveDir() * m_fAimOffset;
	}

	if( m_dwAimMarkID == -1 )
	{
		D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
		ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_szAimMark, vPos, vScale );
		if( pMapEffect )
			m_dwAimMarkID = pMapEffect->GetUniqueID(); 
	}
	else if( m_dwAimMarkID != -1 )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwAimMarkID );
		if( pMapEffect )
			pMapEffect->SetWorldPosition( vPos );
	}
}

void ioTargetCircleMoveSkill::DestroyAimMark( ioBaseChar *pChar )
{
	if( !pChar )	return;

	if( m_iCurFireCount >= m_iMaxFireCount )
	{
		m_SkillState = SS_END;
		return;
	}

	if( m_dwAimMarkID != -1 )
	{
		pChar->DestroyMapEffect( m_dwAimMarkID );
		m_dwAimMarkID = -1;
	}
}

void ioTargetCircleMoveSkill::SetNextSkillState( ioBaseChar *pWounder, ioWeapon *pWeapon )
{
	if( !pWounder || !pWeapon )
		return;

	ioBaseChar *pAttacker = pWeapon->GetOwner();
	if ( !pAttacker )
		return;

	if( pWounder->IsNeedProcess() )
	{
		if( pWounder->GetCharName() != m_szTargetName )
			return;

		ioItem *pItem = GetOwnerItem();
		if ( !pItem || pItem->GetOwnerName().IsEmpty())
			return;

		SetCircleMoveState( pAttacker );

		if( pWounder->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pItem->GetOwnerName();
			kPacket << GetName();
			kPacket << (int)SNT_CIRCLE_MOVE;
			kPacket << pWounder->GetCharName();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}	
}

void ioTargetCircleMoveSkill::CreateMagicCircle( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( !pOwner->CheckMagicCircle() )
		pOwner->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, "", m_fCircleOffSet );
}

void ioTargetCircleMoveSkill::UpdateMagicCircle( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( pOwner->CheckMagicCircle() )
	{
		pOwner->CheckChangeDirectionForMagicCircle();
		pOwner->UpdateMagicCirclePosAll( m_fCircleMoveSpeed, m_fCircleRange, m_fCircleHeightGap, false );

		m_vCirclePos = pOwner->GetMagicCirclePos( m_fCircleOffSet, false );
	}
}

void ioTargetCircleMoveSkill::CheckRotate( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	D3DXVECTOR3 vTargetPos = pChar->GetMagicCirclePos();
	pChar->SetTargetRotToTargetPos( vTargetPos, true, false );
}

void ioTargetCircleMoveSkill::SetCircleActionState( ioBaseChar *pChar, D3DXVECTOR3 vCirclePos )
{
	if ( !pChar )
		return;

	ioBaseChar *pTarget = pChar->GetBaseChar( m_szTargetName );
	if( !pTarget )
		return;

	m_SkillState = SS_CIRCLE_ACTION;
	m_dwAttackEndTime = FRAMEGETTIME() + m_dwCircleActionTime;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CircleActionAttack.m_AttackAnimation );
	float fTimeRate  = m_CircleActionAttack.m_fAttackAniRate;
	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0, true );

	pTarget->RemoveBuff( m_szCircleMoveBuff );

	ioBuff *pBuff = pTarget->AddNewBuff( m_szCircleActionBuff, pChar->GetCharName(), GetName(), NULL );
	ioFloatBuff *pFloatBuff = ToFloatBuff( pBuff );
	if( pFloatBuff )
	{
		D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
		D3DXVECTOR3 vPushDir = vCirclePos - vTargetPos;
		D3DXVec3Normalize( &vPushDir, &vPushDir );

		pTarget->SetTargetRotToDir( vPushDir, true );
		pFloatBuff->SetMoveDir( vPushDir );
	}

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << (int)SNT_CIRCLE_ACTION;
		kPacket << m_szTargetName;
		kPacket << m_vCirclePos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetCircleMoveSkill::WaitTargetReturn( ioBaseChar *pOwner )
{
	if( !m_szTargetName.IsEmpty() )
	{
		ioPlayStage *pStage = pOwner->GetCreator();
		if( pStage )
		{
			ioBaseChar *pTarget = pStage->GetBaseChar( m_szTargetName );
			if( pTarget )
			{
				D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
				D3DXVECTOR3	vDiff = vTargetPos - m_vCirclePos;
				float fDiffRangeSq = D3DXVec3Length( &vDiff );
				if( fDiffRangeSq < m_fCircleActionReturnCheckLength )
				{
					SetCircleActionEndState( pOwner );
				}
			}
		}
	}
}

void ioTargetCircleMoveSkill::SetCircleActionEndState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	SetLoopState( pChar, g_SkillMaker.GetPlayStage() );

	ioBaseChar *pTarget = pChar->GetBaseChar( m_szTargetName );
	if( !pTarget )
		return;

	pTarget->RemoveBuff( m_szCircleActionBuff );

	D3DXVECTOR3 vFirePos = pTarget->GetMidPositionByRate();
	D3DXVECTOR3 vFireDir = -pTarget->GetMoveDir();
	if( !m_CircleActionAttack.m_vWeaponInfoList.empty() )
		pChar->WeaponByWeaponExplicitList( m_CircleActionAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, vFirePos, vFireDir );

	m_szTargetName.Clear();

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << (int)SNT_CIRCLE_END;
		kPacket << m_szTargetName;
		kPacket << vFirePos;
		kPacket << vFireDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetCircleMoveSkill::ApplyCircleActionEndState( ioBaseChar *pChar, D3DXVECTOR3 vPos, D3DXVECTOR3 vDir )
{
	if ( !pChar )
		return;

	SetLoopState( pChar, g_SkillMaker.GetPlayStage() );

	ioBaseChar *pTarget = pChar->GetBaseChar( m_szTargetName );
	if( !pTarget )
		return;

	pTarget->RemoveBuff( m_szCircleActionBuff );

	if( !m_CircleActionAttack.m_vWeaponInfoList.empty() )
		pChar->WeaponByWeaponExplicitList( m_CircleActionAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, vPos, vDir );

	m_szTargetName.Clear();
}

bool ioTargetCircleMoveSkill::CheckTargetBuff( ioBaseChar *pChar, ioHashString szBuff )
{
	ioBaseChar *pTarget = pChar->GetBaseChar( m_szTargetName );
	if( !pTarget )
		return false;

	if( pTarget->HasBuff( szBuff ) || szBuff.IsEmpty() )
		return true;

	return false;
}