#include "StdAfx.h"
#include "ioRepeatLoopAttack2.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioZoneEffectWeapon.h"

ioRepeatLoopAttack2::ioRepeatLoopAttack2(void)
{
	ClearData();
}

ioRepeatLoopAttack2::ioRepeatLoopAttack2( const ioRepeatLoopAttack2 &rhs ):
ioAttackSkill( rhs ),
m_szLoopAni( rhs.m_szLoopAni ),
m_fLoopAniRate( rhs.m_fLoopAniRate ),
m_LoopAttackInfo( rhs.m_LoopAttackInfo ),
m_dwLoopTime( rhs.m_dwLoopTime ),
m_dwKeyInputEnableTime( rhs.m_dwKeyInputEnableTime ),
m_fMoveSpeed( rhs.m_fMoveSpeed ),
m_EndAttackAttribute( rhs.m_EndAttackAttribute ),
m_iMaxMoveAttackCount( rhs.m_iMaxMoveAttackCount ),
m_OwnerBuffList( rhs.m_OwnerBuffList ),
m_LoopAttackKeyType( rhs.m_LoopAttackKeyType )
{
	ClearData();
}

ioRepeatLoopAttack2::~ioRepeatLoopAttack2(void)
{
	ClearData();
}

void ioRepeatLoopAttack2::ClearData()
{
	m_SkillState = SS_NONE;
	SetMotionTime( 0, 0 );
	m_bAttackKeyInput = false;
	m_iCurMoveAttackCount = 0;
}

ioSkill* ioRepeatLoopAttack2::Clone()
{
	return new ioRepeatLoopAttack2( *this );
}

void ioRepeatLoopAttack2::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );
	char szBuf[MAX_PATH] = "";

	rkLoader.LoadString_e( "loop_ani", "", szBuf, MAX_PATH );
	m_szLoopAni = szBuf;
	m_fLoopAniRate = rkLoader.LoadFloat_e( "loop_ani_rate", 0.0f );

	m_fMoveSpeed = rkLoader.LoadFloat_e( "loop_move_speed", 0.0f );
	m_LoopAttackInfo.m_iWeaponIdx = rkLoader.LoadInt_e( "loop_attack_type", NO_WEAPON_ATTRIBUTE );
	m_LoopAttackInfo.m_iResistanceIdx = rkLoader.LoadInt_e( "loop_attack_type_resist", NO_WEAPON_ATTRIBUTE );
	rkLoader.LoadString( "loop_attack_type_wounded", "", szBuf, MAX_PATH );
	m_LoopAttackInfo.m_WoundedAnimation = szBuf;
	m_LoopAttackInfo.m_dwWoundedDuration = rkLoader.LoadInt_e( "loop_attack_type_wounded_time", 0 );
	m_LoopAttackInfo.m_bWoundedLoopAni = rkLoader.LoadBool_e( "loop_attack_type_wounded_loop_ani", false );

	m_dwLoopTime = (DWORD)rkLoader.LoadInt_e( "loop_duration", 0 );
	m_dwKeyInputEnableTime = (DWORD)rkLoader.LoadInt_e( "key_input_enable_time", 0 );

	LoadAttackAttribute_e( "end_attack", m_EndAttackAttribute, rkLoader );

	m_iMaxMoveAttackCount = rkLoader.LoadInt_e( "max_move_attack_count", 0 );

	LoadOwnerBuffList( rkLoader );

	m_LoopAttackKeyType = (LoopAttackKeyType)rkLoader.LoadInt_e( "loop_attack_key_type", LAKT_RESERVE_ATTACK );
}

void ioRepeatLoopAttack2::LoadOwnerBuffList( ioINILoader &rkLoader )
{
	int iCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	m_OwnerBuffList.clear();

	if( iCnt <= 0 ) 
		return;
	m_OwnerBuffList.reserve( iCnt );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "owner_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		ioBuffInfo tBuffInfo;
		tBuffInfo.m_BuffName = szBuf;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				m_OwnerBuffList.push_back( tBuffInfo );
			}
		}
	}
}

bool ioRepeatLoopAttack2::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	SetMotionTime( 0, 0 );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	SetCameraBuff( pChar );

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	ClearData();

	AddOwnerBuff( pChar );

	return true;
}

void ioRepeatLoopAttack2::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );
				if( pChar->IsNeedProcess() )
					SetLoopAttackState( pChar );
			}
		}
		break;
	case SS_LOOP_ATTACK:
		{
			if ( m_dwFireStartTime > 0 && m_dwFireStartTime < dwCurTime )
			{
				m_dwFireStartTime = 0;
				m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
				D3DXVECTOR3 vCharPos = pChar->GetMidPositionByRate();
				if ( m_LoopAttackInfo.m_iWeaponIdx != NO_WEAPON_ATTRIBUTE )
					pChar->SkillFireExplicit( m_LoopAttackInfo, GetName(), m_dwWeaponBaseIndex, vCharPos );
			}

			if ( pChar->IsNeedProcess() )
			{
				//Check Loop Time
				if ( m_dwLoopTime > 0 && m_dwMotionStartTime + m_dwLoopTime < dwCurTime )
				{
					if ( m_bAttackKeyInput && m_iCurMoveAttackCount < m_iMaxMoveAttackCount )
					{
						SetLoopAttackState( pChar );
						return;
					}
					else
					{
						SetEndAttackState( pChar );
						return;
					}
				}

				//Check Key Input
				if ( m_dwKeyInputEnableTime > 0 && m_dwMotionStartTime + m_dwKeyInputEnableTime < dwCurTime )
				{
					if ( m_LoopAttackKeyType == LAKT_RESERVE_ATTACK )
					{
						if ( !m_bAttackKeyInput && pChar->IsAttackKey() )
							m_bAttackKeyInput = true;
					}
					else if ( m_LoopAttackKeyType == LAKT_DIRECT_ATTACK )
					{
						if ( pChar->IsAttackKey() )
						{
							if ( m_iCurMoveAttackCount < m_iMaxMoveAttackCount )
							{
								SetLoopAttackState( pChar );
								return;
							}
							else
							{
								SetEndAttackState( pChar );
								return;
							}
						}
					}
					
				}
			}

			if ( m_fMoveSpeed != 0.0f )
				pChar->SetCurMoveSpeed( m_fMoveSpeed );
		}
		break;
	case SS_END_ATTACK:
		break;
	}
}

void ioRepeatLoopAttack2::SetLoopAttackState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if ( !pGrp )
		return;

	m_SkillState = SS_LOOP_ATTACK;	

	int iAniID = pGrp->GetAnimationIdx( m_szLoopAni );
	if( iAniID == -1 )	
		return;

	SetMotionTime( 0, 0 );
	m_bAttackKeyInput = false;

	float fAniRate = FLOAT1;
	if ( fAniRate > 0.0f )
		fAniRate = m_fLoopAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100,1.0f, FLOAT1/ fAniRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwFireStartTime = m_dwMotionStartTime = m_dwFireMotionEndTime = dwCurTime;
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fAniRate;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fAniRate;
	if( m_dwFireStartTime >= m_dwFireMotionEndTime )
		m_dwFireMotionEndTime = m_dwFireStartTime;

	if ( pChar->IsNeedProcess() && pChar->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();
		pChar->SetTargetRotToDirKey( eNewDirKey );
	}

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fAniRate;

	if( IsUseAutoTargetValue() )
	{
		pChar->SetAutoTarget( ATT_SKILL );
		pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}

	m_iCurMoveAttackCount++;

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << static_cast<BYTE>(m_SkillState);
		kPacket << pChar->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioRepeatLoopAttack2::SetEndAttackState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	pChar->SetCurMoveSpeed( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );

	ioEntityGroup *pGrp = pChar->GetGroup();
	if ( !pGrp )
		return;

	//단계의 애니메이션 값 읽기
	int iAniID = pGrp->GetAnimationIdx( m_EndAttackAttribute.m_AttackAnimation );
	if( iAniID == -1 )	
		return;

	SetMotionTime( 0, 0 );

	//빠르기 설정, 딜레이 설정
	float fTimeRate  = m_EndAttackAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = m_EndAttackAttribute.m_dwPreDelay;
	DWORD dwEndDelay = m_EndAttackAttribute.m_dwEndDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	//모션 시간 설정
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = m_dwFireMotionEndTime = dwCurTime;

	//종료 시간 설정
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + dwEndDelay;
	m_dwFireMotionEndTime += dwPreDelay;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pChar->ClearReservedSliding();

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	pChar->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex, iAniID, m_EndAttackAttribute.m_vWeaponInfoList, FTT_SKILL_ATTACK, fTimeRate, dwPreDelay );
	pChar->SetReservedSliding( m_EndAttackAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	DWORD dwTrackingTime = dwCurTime + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	if( IsUseAutoTargetValue() )
	{
		pChar->SetAutoTarget( ATT_SKILL );
		pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}

	m_SkillState = SS_END_ATTACK;

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << static_cast<BYTE>(m_SkillState);
		kPacket << pChar->GetTargetRot();
		if( IsUseAutoTargetValue() )
			pChar->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioRepeatLoopAttack2::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if ( m_SkillState == SS_END_ATTACK && m_dwMotionEndTime < FRAMEGETTIME() && m_fSkillEndNextJumpAmt == 0.0f )
		return true;

	return false;
}

bool ioRepeatLoopAttack2::IsAttackEndState() const
{
	if( m_SkillState == SS_END_ATTACK && m_dwMotionEndTime < FRAMEGETTIME() )
		return true;

	return false;
}

bool ioRepeatLoopAttack2::IsEndState() const
{
	return IsAttackEndState();
}

void ioRepeatLoopAttack2::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_bReduceGauge = false;
	pChar->SetSkillMoveEnable( false, false );

	EndCameraBuff( pChar );

	RemoveOwnerBuff( pChar );

	ClearData();
}

void ioRepeatLoopAttack2::AddOwnerBuff( ioBaseChar *pChar )
{
	if( !pChar )	
		return;
	if( m_OwnerBuffList.empty() ) 
		return;

	int iCnt = m_OwnerBuffList.size();
	for( int i = 0; i < iCnt; ++i )
		pChar->AddNewBuff( m_OwnerBuffList[i].m_BuffName, pChar->GetCharName(), GetOwnerItem()->GetName(), this );
}

void ioRepeatLoopAttack2::RemoveOwnerBuff( ioBaseChar *pChar )
{
	if( !pChar )	
		return;
	if( m_OwnerBuffList.empty() ) 
		return;
	int iCnt = m_OwnerBuffList.size();

	for( int i=0; i<iCnt; ++i)
	{
		ioBuff* pBuff = pChar->GetBuff(m_OwnerBuffList[i].m_BuffName );
		if( pBuff )
			pBuff->SetReserveEndBuff();
	}
}

void ioRepeatLoopAttack2::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	ioAttackSkill::ApplyExtraSkillInfo( pChar, pStage, rkPacket );

	if( !pChar || !pStage ) 
		return;

	BYTE btState;
	rkPacket >> btState;
	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pChar->SetTargetRotToRotate( qtRot, true );
	if ( btState == SS_LOOP_ATTACK )
	{
		SetLoopAttackState( pChar );
		if( IsUseAutoTargetValue() )
			pChar->ApplyAutoTargetInfo( rkPacket );
	}
	else if ( btState == SS_END_ATTACK )
	{
		SetEndAttackState( pChar );
		if( IsUseAutoTargetValue() )
			pChar->ApplyAutoTargetInfo( rkPacket );
	}
}

void ioRepeatLoopAttack2::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioAttackSkill::ApplyAttributeToWeapon( pWeapon, pStage );

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
}
