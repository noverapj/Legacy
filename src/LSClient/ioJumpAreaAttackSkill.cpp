#include "StdAfx.h"
#include "ioJumpAreaAttackSkill.h"
#include "ioBaseChar.h"
#include "WeaponDefine.h"

ioJumpAreaAttackSkill::ioJumpAreaAttackSkill(void)
{
	ClearData();
}

ioJumpAreaAttackSkill::ioJumpAreaAttackSkill( const ioJumpAreaAttackSkill &rhs ):
ioAttackSkill( rhs ),
m_fJumpAngle( rhs.m_fJumpAngle ),
m_fJumpPower( rhs.m_fJumpPower ),
m_szSkillLoopMotion( rhs.m_szSkillLoopMotion ),
m_fSkillLoopMotionRate( rhs.m_fSkillLoopMotionRate ),
m_dwLoopDuration( rhs.m_dwLoopDuration ),
m_szSkillLoopEffect( rhs.m_szSkillLoopEffect ),
m_szPreActionName( rhs.m_szPreActionName ),
m_szPreActionEffect( rhs.m_szPreActionEffect ),
m_fPreActionRate( rhs.m_fPreActionRate ),
m_vWeaponOffset( rhs.m_vWeaponOffset ),
m_bFirstWeaponCall( rhs.m_bFirstWeaponCall ),
m_dwSkillLoopProtectTime( rhs.m_dwSkillLoopProtectTime ),
m_szAttackMotionName( rhs.m_szAttackMotionName ),
m_fAttackMotionRate( rhs.m_fAttackMotionRate ),
m_dwPreAttackProtectTime( rhs.m_dwPreAttackProtectTime ),
m_dwKeyInputLimitTime( rhs.m_dwKeyInputLimitTime ),
m_dwCreateWeaponTicTime( rhs.m_dwCreateWeaponTicTime )
{
	ClearData();
}


ioJumpAreaAttackSkill::~ioJumpAreaAttackSkill(void)
{
}

void ioJumpAreaAttackSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_fJumpAngle = rkLoader.LoadFloat_e( "jump_angle", 0.0f );
	m_fJumpPower = rkLoader.LoadFloat_e( "jump_power", 0.0f );

	//pre action
	rkLoader.LoadString_e( "skill_preaction_motion", "", szBuf, MAX_PATH );
	m_szPreActionName = szBuf;
	rkLoader.LoadString_e( "skill_preaction_effect", "", szBuf, MAX_PATH );
	m_szPreActionEffect = szBuf;
	m_fPreActionRate = rkLoader.LoadFloat_e( "skill_preaction_rate", 0.0f );

	//pre attack action
	m_dwPreAttackProtectTime = rkLoader.LoadInt_e( "skill_preattack_protect_duration", 0 );

	//loop
	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_szSkillLoopMotion = szBuf;
	m_dwLoopDuration = rkLoader.LoadInt_e( "skill_loop_duration", 0 ); 
	m_fSkillLoopMotionRate = rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );
	rkLoader.LoadString_e( "skill_loop_effect", "", szBuf, MAX_PATH );
	m_szSkillLoopEffect = szBuf;
	m_dwSkillLoopProtectTime = rkLoader.LoadInt_e( "skill_loop_protect_duration", 0);

	//attack ani
	rkLoader.LoadString_e( "skill_attack_ani_motion", "", szBuf, MAX_PATH );
	m_szAttackMotionName = szBuf;
	m_fAttackMotionRate = rkLoader.LoadFloat_e( "skill_attack_ani_rate", 0.0f );
	//animation중에 weapon발사 시 발사 animation이 존재하느냐 마느냐를 체크.

	//weapon
	m_vWeaponOffset.x = rkLoader.LoadFloat_e( "weapon_offset_x", 0.0f);
	m_vWeaponOffset.y = rkLoader.LoadFloat_e( "weapon_offset_y", 0.0f);
	m_vWeaponOffset.z = rkLoader.LoadFloat_e( "weapon_offset_z", 0.0f);

	//d키 입력 없이 처음 웨폰을 발사 할지 안할지
	m_bFirstWeaponCall = rkLoader.LoadBool_e( "first_weapon_call", false );

	m_dwKeyInputLimitTime = rkLoader.LoadInt_e( "keyinput_limit_time", 0 ); 

	//attack ani가 존재 하지 않는 경우 d키를 누를 때마다 웨폰이 생성되는 사태를 막기 위해
	//m_dwCreateWeaponTicTime 시간 이후에 주도록 세팅
	m_dwCreateWeaponTicTime = rkLoader.LoadInt_e( "create_weapon_tic_time", 0 );
}

void ioJumpAreaAttackSkill::ClearData()
{
	m_SkillState = SS_NONE;

	m_dwPreActionEndTime = 0;
	m_dwPreAttackStartTime = 0;
	m_dwPreAttackEndTime = 0;
	m_dwLoopStartTime = 0;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_dwPreAttackKeyReserveTime = 0;
	m_bKeyInput = false;

	m_dwCreatedWeaponTime = 0;
}

ioSkill* ioJumpAreaAttackSkill::Clone()
{
	return new ioJumpAreaAttackSkill( *this );
}


bool ioJumpAreaAttackSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	m_bSetHandMesh = false;

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

	SetCameraBuff( pChar );

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	m_SkillState = SS_NONE;

	m_dwPreAttackStartTime = 0;
	m_dwPreAttackEndTime = 0;

	m_dwLoopStartTime = 0;

	m_dwPreActionEndTime = 0;
	if ( m_fJumpPower != 0 )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}
	pChar->SetForcePowerAmt( 0.0f );

	m_iCurActiveCount = m_iMaxActiveCount;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_dwPreAttackKeyReserveTime = 0;

	m_bKeyInput = false;

	m_dwCreatedWeaponTime = 0;

	return true;
}

void ioJumpAreaAttackSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	if( !m_HandMesh.IsEmpty() )
	{
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	}
	m_bSetHandMesh = false;

	m_SkillState = SS_NONE;

}

void ioJumpAreaAttackSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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
				SetPreActionState( pChar );
				SetCharJumping( pChar );
			}
		}
		break;
	case SS_PREACTION:
		{
			if ( m_dwPreActionEndTime > 0 && m_dwPreActionEndTime < dwCurTime )
			{
				SetPreAttackAction( pChar );
			}
		}
		break;
	case SS_PREATTACACTION:
		{
			if ( m_dwPreAttackEndTime > 0 && m_dwPreAttackEndTime < dwCurTime )
			{
				if ( m_bKeyInput )
					SetLoopState( pChar );
				else
					SetActionEndState( pChar );
			}
			if ( pChar->IsNeedProcess() )
			{
				if ( pChar->IsAttackKey() )
				{
					if ( m_dwPreAttackKeyReserveTime > dwCurTime )
					{
						m_bKeyInput = true;
					}
				}
			}
			
		}
		break;
	case SS_LOOP:
		{
			//loop가 끝나는지 체크 끝나면 다음 상태로 넘어간다.
			if ( CheckLoopEnd( pChar ) )
				break;

			if ( pChar->IsNeedProcess() )
			{
				CheckKeyInput( pChar, pStage );
			}
		}
		break;
	case SS_ACTION:
		{
			if ( m_dwFireMotionEndTime < dwCurTime)
			{
				SetLoopState( pChar );
			}
		}
		break;
	case SS_ENDACTION:
		{
			if ( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
			{
				SetEndState( pChar );
			}

		}
		break;
	}
}

void ioJumpAreaAttackSkill::SetCharJumping( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	if ( m_fJumpPower == 0.0f )
		return;

	D3DXVECTOR3 vTargetDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	float fJumpAngle = m_fJumpAngle;

	float fAngleV = ioMath::ArrangeHead( fJumpAngle );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vTargetDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

	// vertical
	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngleV) );
	vTargetDir = qtRot * vTargetDir;

	pChar->SetForcePower( vTargetDir, fabs(m_fJumpPower), FLOAT1, true );
}

void ioJumpAreaAttackSkill::SetPreActionState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	EndPreDelayEffect( pChar );

	m_SkillState = SS_PREACTION;
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_szPreActionName );
	if( iAniID == -1 )	return;

	float fTimeRate = FLOAT1;
	if ( m_fPreActionRate > 0.0f )
		fTimeRate = m_fPreActionRate;	

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, 5.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwPreActionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
}

void ioJumpAreaAttackSkill::SetFirstWeaponCall( ioBaseChar *pChar, int iAniID, float fTimeRate, DWORD dwPreDelay )
{
	if ( !pChar || iAniID == -1 || GetAttribute().m_vWeaponInfoList.empty() )
		return;

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
		iAniID,
		GetAttribute().m_vWeaponInfoList,
		FTT_SKILL_ATTACK,
		fTimeRate,
		dwPreDelay );

	m_iCurActiveCount--;
}

void ioJumpAreaAttackSkill::SetPreAttackAction( ioBaseChar *pChar )
{
	if( !pChar ) 
		return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if (!pGrp)
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_SkillState = SS_PREATTACACTION;
	pChar->EndEffect( m_szPreActionEffect);

	ioHashString szSkillMotion = GetSkillMotionName();
	
	int iAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iAniID == -1 )	return;

	pChar->AttachEffect( m_SkillEffectName );

	float fTimeRate  = GetAttackTimeRate();
	if ( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;
	DWORD dwPreDelay = GetAttribute().m_dwPreDelay;
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	m_dwPreAttackStartTime = m_dwPreAttackEndTime = m_dwPreAttackKeyReserveTime = dwCurTime;
	m_dwPreAttackEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
	m_dwPreAttackKeyReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	if ( m_bFirstWeaponCall )
		SetFirstWeaponCall( pChar, iAniID, fTimeRate, dwPreDelay );
}


void ioJumpAreaAttackSkill::SetLoopState( ioBaseChar *pChar )
{
	if( !pChar ) 
		return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if (!pGrp)
		return;
	m_SkillState = SS_LOOP;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pChar->AttachEffect( m_szSkillLoopEffect );
	int iAniID = pGrp->GetAnimationIdx( m_szSkillLoopMotion );
	float fAniRate = FLOAT1;

	if( m_fSkillLoopMotionRate > 0.0f )
		fAniRate = m_fSkillLoopMotionRate;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	//처음 들어왔을 때 한번만 해주기 위해서.
	if ( m_dwLoopStartTime == 0 )
		m_dwLoopStartTime = m_dwCreatedWeaponTime = dwCurTime;
	
}

void ioJumpAreaAttackSkill::WeaponFire( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar )
		return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;
	DWORD dwCurTime = FRAMEGETTIME();

	//attack motion이 있는 경우면 weapon 생성만, loop를 계속 돌게 한다.
	if ( !m_szAttackMotionName.IsEmpty() )
	{
		m_SkillState = SS_ACTION;
		int iAniID = pGrp->GetAnimationIdx( m_szAttackMotionName );
		if( iAniID == -1 )	return;

		pChar->AttachEffect( m_SkillEffectName );

		float fTimeRate  = FLOAT1;
		if ( m_fAttackMotionRate > 0.0f )
			fTimeRate = m_fAttackMotionRate;
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );
		m_dwFireMotionEndTime = dwCurTime;
		m_dwFireMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0, true );

		CreateWeaponByAni( pChar, iAniID, fTimeRate, 0 );
	}
	else
		CreateWeapon( pChar, pStage );
}

void ioJumpAreaAttackSkill::SetActionEndState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;
	if( GetAttribute().m_AttackEndAnimation.IsEmpty() )
	{
		SetEndState( pChar );
		return;
	}

	pChar->EndEffect( m_szSkillLoopEffect );
	DWORD dwCurTime = FRAMEGETTIME();
	ioHashString szSkillMotion = GetAttribute().m_AttackEndAnimation;

	int iAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iAniID == -1 )	return;

	float fTimeRate = FLOAT1;
	if ( GetAttribute().m_fAttackEndAniRate > 0.0f )
		fTimeRate = GetAttribute().m_fAttackEndAniRate;

	m_SkillState = SS_ENDACTION;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
}


void ioJumpAreaAttackSkill::SetEndState( ioBaseChar *pChar )
{
	pChar->EndEffect( m_szSkillLoopEffect );
	m_SkillState = SS_END;
	
}

bool ioJumpAreaAttackSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if ( m_SkillState == SS_END && m_fSkillEndNextJumpAmt == 0.0f )
	{
		return true;
	}

	return false;
}

void ioJumpAreaAttackSkill::CreateWeapon( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || GetAttribute().m_vWeaponInfoList.empty() )
		return;

	D3DXQUATERNION qtRot = pChar->GetWorldOrientation();
	D3DXVECTOR3 vPos = pChar->GetWorldPosition() + ( qtRot * m_vWeaponOffset );
		
	for ( UINT i=0 ; i< GetAttribute().m_vWeaponInfoList.size() ; ++i )
	{
		pChar->SkillFireExplicit( GetAttribute().m_vWeaponInfoList[i],
			GetName(),
			m_dwWeaponBaseIndex++,
			vPos );
	}

	m_dwCreatedWeaponTime = FRAMEGETTIME();
	m_iCurActiveCount--;
}

void ioJumpAreaAttackSkill::CreateWeaponByAni( ioBaseChar *pChar, int iAniID, float fTimeRate, DWORD dwPreDelay )
{
	if ( !pChar || iAniID == -1 || GetAttribute().m_vWeaponInfoList.empty() )
		return;

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
		iAniID,
		GetAttribute().m_vWeaponInfoList,
		FTT_SKILL_ATTACK,
		fTimeRate,
		dwPreDelay );

	m_dwCreatedWeaponTime = FRAMEGETTIME();
	m_iCurActiveCount--;
}

bool ioJumpAreaAttackSkill::CheckLoopEnd( ioBaseChar *pChar )
{
	DWORD dwCurTime = FRAMEGETTIME();
	//loop 시간에 의한 loop 종료 체크
	if( m_dwLoopStartTime + m_dwLoopDuration < dwCurTime )
	{
		SetActionEndState( pChar );
		return true;
	}
	//키 입력을 하지 않았을 경우 key limit time에 의한 loop 종료 체크 및 
	//loop 시간 동안 무기를 다 소환 했을 경우
	if ( m_dwCreatedWeaponTime + m_dwKeyInputLimitTime < dwCurTime || m_iCurActiveCount <= 0 )
	{
		SetActionEndState( pChar );
		return true;
	}
	return false;
}

void ioJumpAreaAttackSkill::CheckKeyInput( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	CheckDirKeyInput( pChar );
	
	if ( m_dwCreatedWeaponTime + m_dwCreateWeaponTicTime <= dwCurTime )
	{
		if ( pChar->IsAttackKey() )
		{
			WeaponFire( pChar, pStage );
			if( pChar->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pChar->GetCharName();
				kPacket << GetName();
				kPacket << static_cast<byte>( S_ATTACKKEY );
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	
}


void ioJumpAreaAttackSkill::CheckDirKeyInput( ioBaseChar *pChar )
{
	if( pChar->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();
		if( eNewDirKey != m_CurDirKey )
		{
			m_CurDirKey = eNewDirKey;
			pChar->SetTargetRotToDirKey( eNewDirKey );

			if( pChar->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pChar->GetCharName();
				kPacket << GetName();
				kPacket << static_cast<byte>( S_DIRKEY );
				kPacket << (int )m_CurDirKey;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}


bool ioJumpAreaAttackSkill::IsProtected( int iDefenseBreakType ) const
{
	DWORD dwCurTime = FRAMEGETTIME();
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

	if ( m_dwPreAttackProtectTime > 0 && m_dwPreAttackStartTime + m_dwPreAttackProtectTime > dwCurTime )
		return true;

	if ( m_dwSkillLoopProtectTime > 0 && m_dwLoopStartTime + m_dwSkillLoopProtectTime > dwCurTime )
		return true;

	return false;
}

bool ioJumpAreaAttackSkill::IsNoDropState() const
{
	if( m_SkillState != SS_END )
		return true;
	return false;
}

bool ioJumpAreaAttackSkill::IsEndState() const
{
	return IsAttackEndState();
}

bool ioJumpAreaAttackSkill::IsAttackEndState() const
{
	if( m_SkillState == SS_END )
		return true;

	return false;
}

void ioJumpAreaAttackSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	byte cState = 0;
	rkPacket >> cState;

	if ( cState == S_ATTACKKEY )
		WeaponFire( pChar, pStage ); 
	else if ( cState == S_DIRKEY )
	{
		int Key;
		rkPacket >> Key;
		m_CurDirKey = ( ioUserKeyInput::DirKeyInput )Key;
		pChar->SetTargetRotToDirKey( m_CurDirKey );
	}
}

void ioJumpAreaAttackSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	SetChargingRateForWeapon( pWeapon );

	ioZoneEffectWeapon *pZoneEffect = ToZoneEffectWeapon( pWeapon );
	if( pZoneEffect )
	{
		ioBaseChar *pChar = pZoneEffect->GetOwner();
		D3DXQUATERNION qtRot = pChar->GetWorldOrientation();
		D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVECTOR3 vPos = pChar->GetWorldPosition() + ( qtRot * m_vWeaponOffset );
		if( pChar )
		{
			pZoneEffect->SetStartPosition( vPos, pStage );
			pZoneEffect->SetCollisionDir( vDir );
			pZoneEffect->SetZoneValue( false, true );
		}
	}
}


