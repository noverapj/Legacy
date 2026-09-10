

#include "stdafx.h"

#include "ioEngineerMachineGunAttack.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"
#include "ioEngineerGatlingDummy.h"

ioEngineerMachineGunAttack::ioEngineerMachineGunAttack()
{
	Init();
}

ioEngineerMachineGunAttack::ioEngineerMachineGunAttack( const ioEngineerMachineGunAttack &rhs )
	: ioNormalSkill( rhs ),
	m_iMaxBullet( rhs.m_iMaxBullet ),
	m_iGatlingAttackNeedBullet( rhs.m_iGatlingAttackNeedBullet ),
	m_iBeamAttackNeedBullet( rhs.m_iBeamAttackNeedBullet ),
	m_iShockAttackNeedBullet( rhs.m_iShockAttackNeedBullet ),
	m_iAirTraceAttackNeedBullet( rhs.m_iAirTraceAttackNeedBullet ),

	m_szDummyName( rhs.m_szDummyName ),

	m_szLoopAnimation( rhs.m_szLoopAnimation ),
	m_dwLoopDuration( rhs.m_dwLoopDuration ),
	m_fLoopAnimationRate( rhs.m_fLoopAnimationRate ),

	m_dwChargingTime( rhs.m_dwChargingTime ),
	m_szChargingAnimation( rhs.m_szChargingAnimation ),
	m_fChargingAnimationRate( rhs.m_fChargingAnimationRate ),

	m_szGatlingAttackAnimation( rhs.m_szGatlingAttackAnimation ),
	m_fGatlingAttackAnimationRate( rhs.m_fGatlingAttackAnimationRate ),
	m_GatlingAttackList( rhs.m_GatlingAttackList ),

	m_szBeamAttackAnimation( rhs.m_szBeamAttackAnimation ),
	m_fBeamAttackAnimationRate( rhs.m_fBeamAttackAnimationRate ),
	m_dwBeamAattackBulletUseTime( rhs.m_dwBeamAattackBulletUseTime ),
	m_BeamAttackList( rhs.m_BeamAttackList ),

	m_szShockAttackAnimation( rhs.m_szShockAttackAnimation ),
	m_fShockAttackAnimationRate( rhs.m_fShockAttackAnimationRate ),
	m_ShockAttackList( rhs.m_ShockAttackList ),

	m_szAirTraceAttackAnimation( rhs.m_szAirTraceAttackAnimation ),
	m_fAirTraceAttackAnimationRate( rhs.m_fAirTraceAttackAnimationRate ),
	m_AirTraceAttackList( rhs.m_AirTraceAttackList ),

	m_fRotateSpeed( rhs.m_fRotateSpeed ),
	m_fBeamRotateSpeed( rhs.m_fBeamRotateSpeed ),
	m_dwSkillProtectTime( rhs.m_dwSkillProtectTime )
{
	Init();
}

ioEngineerMachineGunAttack::~ioEngineerMachineGunAttack()
{
}

void ioEngineerMachineGunAttack::Init()
{
	m_SkillState						= SS_PRE;
	m_CurDirKey							= ioUserKeyInput::DKI_NONE;

	m_dwLoopStartTime					= 0;
	m_dwChargingStartTime				= 0;	
	m_dwBeamAattackBulletUseStartTime	= 0;
	m_dShockAttackStartTime				= 0;
	m_dwAirTraceAttackStartTime			= 0;

	m_dwGatlingAttackEndTime			= 0;
	m_dShockAttackEndTime				= 0;
	m_dwAirTraceAttackEndTime			= 0;

	m_iCurrBullet						= 0;
	m_dwWeaponBaseIndex					= 0;

	m_CurrAttackWeaponInfoList.clear();
}

void ioEngineerMachineGunAttack::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	//bullet
	m_iMaxBullet				= rkLoader.LoadInt_e( "max_bullet", 0 );
	m_iGatlingAttackNeedBullet	= rkLoader.LoadInt_e( "gatling_need_bullet", 0 );
	m_iBeamAttackNeedBullet		= rkLoader.LoadInt_e( "beam_need_bullet", 0 );
	m_iShockAttackNeedBullet	= rkLoader.LoadInt_e( "shock_need_bullet", 0 );
	m_iAirTraceAttackNeedBullet	= rkLoader.LoadInt_e( "air_trace_need_bullet", 0 );

	//Dummy
	rkLoader.LoadString_e( "dummy_name", "", szBuf, MAX_PATH );
	m_szDummyName = szBuf;

	//Loop
	m_dwLoopDuration = rkLoader.LoadInt_e( "loop_duration", 0 );

	rkLoader.LoadString_e( "loop_animation", "", szBuf, MAX_PATH );
	m_szLoopAnimation = szBuf;
	m_fLoopAnimationRate = rkLoader.LoadFloat_e( "loop_animation_rate", FLOAT1 );

	//Charging	
	m_dwChargingTime = rkLoader.LoadInt_e( "charging_time", 250 );
	rkLoader.LoadString_e( "charging_animation", "", szBuf, MAX_PATH );
	m_szChargingAnimation = szBuf;
	m_fChargingAnimationRate = rkLoader.LoadFloat_e( "charging_animation_rate", FLOAT1 );

	//Gatling Attack		
	rkLoader.LoadString_e( "gatling_attack_animation", "", szBuf, MAX_PATH );
	m_szGatlingAttackAnimation = szBuf;
	m_fGatlingAttackAnimationRate = rkLoader.LoadFloat_e( "gatling_attack_animation_rate", FLOAT1 );

	LoadAttackWeaponInfo( rkLoader, "gatling_attack", m_GatlingAttackList );

	//Beam Attack
	m_dwBeamAattackBulletUseTime = rkLoader.LoadInt_e( "beam_bullet_use_time", 0 );
	rkLoader.LoadString_e( "beam_attack_animation", "", szBuf, MAX_PATH );
	m_szBeamAttackAnimation = szBuf;
	m_fBeamAttackAnimationRate = rkLoader.LoadFloat_e( "beam_attack_animation_rate", FLOAT1 );

	LoadAttackWeaponInfo( rkLoader, "beam_attack", m_BeamAttackList );

	//ShockAttack
	rkLoader.LoadString_e( "shock_attack_animation", "", szBuf, MAX_PATH );
	m_szShockAttackAnimation = szBuf;
	m_fShockAttackAnimationRate = rkLoader.LoadFloat_e( "shock_attack_animation_rate", FLOAT1 );

	LoadAttackWeaponInfo( rkLoader, "Shock_attack", m_ShockAttackList );
	
	//Air Trace Attack
	rkLoader.LoadString_e( "air_trace_attack_animation", "", szBuf, MAX_PATH );
	m_szAirTraceAttackAnimation = szBuf;
	m_fAirTraceAttackAnimationRate = rkLoader.LoadFloat_e( "air_trace_attack_animation_rate", FLOAT1 );

	LoadAttackWeaponInfo( rkLoader, "air_trace_attack", m_AirTraceAttackList );

	//Rotate
	m_fRotateSpeed			= rkLoader.LoadFloat_e( "rotate_speed", FLOAT1000 );
	m_fBeamRotateSpeed		= rkLoader.LoadFloat_e( "beam_rotate_speed", 3000.0f );
	m_dwSkillProtectTime	= rkLoader.LoadInt_e( "skill_protect_time", 0 );
}

void ioEngineerMachineGunAttack::LoadAttackWeaponInfo( ioINILoader &rkLoader, const char *szTtitle, AttackWeaponInfoList& List )
{
	char szBuf[MAX_PATH];
	wsprintf_e( szBuf, "%s_max_count", szTtitle );
	int iMaxCount = rkLoader.LoadInt( szBuf, 0 );
	for( int i = 0; i < iMaxCount; ++i )
	{
		AttackWeaponInfo Info;
		wsprintf_e( szBuf, "%s%d", szTtitle, i+1 );
		LoadWeaponInfo( szBuf, Info.m_WeaponInfo, rkLoader );

		wsprintf_e( szBuf, "%s%d_fire_time", szTtitle, i+1 );
		Info.m_dwFireTime = rkLoader.LoadInt( szBuf, 0 );

		wsprintf_e( szBuf, "%s%d_offset_x", szTtitle, i+1 );
		Info.m_vOffset.x = rkLoader.LoadInt( szBuf, 0 );

		wsprintf_e( szBuf, "%s%d_offset_y", szTtitle, i+1 );
		Info.m_vOffset.y = rkLoader.LoadInt( szBuf, 0 );

		wsprintf_e( szBuf, "%s%d_offset_z", szTtitle, i+1 );
		Info.m_vOffset.z = rkLoader.LoadInt( szBuf, 0 );

		List.push_back( Info );
	}
}

ioSkill* ioEngineerMachineGunAttack::Clone()
{
	return new ioEngineerMachineGunAttack( *this );
}

bool ioEngineerMachineGunAttack::HasSkillMotion() const
{
	return !m_szLoopAnimation.IsEmpty();
}

bool ioEngineerMachineGunAttack::OnSkillStart( ioBaseChar *pChar )
{
	ioNormalSkill::OnSkillStart( pChar );

	Init();
	CheckWeaponItemMesh( pChar );

	m_dwSkillStartTime		= FRAMEGETTIME();
	m_dwPreDelayStartTime	= m_dwSkillStartTime;
	m_iCurrBullet			= m_iMaxBullet;

	StartPreDelayAnimation( pChar );
	pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), GetFireNeedGauge( pChar->GetCharName() ) );
	m_bReduceGauge = true;

	return true;
}

void ioEngineerMachineGunAttack::CreateDummy( ioBaseChar* pOwner, ioPlayStage *pStage, int iCurIndex )
{
	DestroyDummy( pOwner );

	D3DXVECTOR3 vDir		= pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vTargetPos	= pOwner->GetWorldPosition();

	if( pStage )
	{
		ioDummyChar *pNewDummy = pStage->CreateDummyChar( m_szDummyName, iCurIndex, pOwner->GetCharName(), vTargetPos, 0.0f, 0, true );
		if( pNewDummy )
		{
			m_iDummyCharIndex = iCurIndex;
		}
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << (byte)SSS_CREATE_DUMMY;
		kPacket << iCurIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioEngineerMachineGunAttack::CheckDummy( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if( m_SkillState == SS_PRE )
		return;

	if( GetDummy( pOwner ) == NULL )
	{
		m_SkillState = SS_END;
	}
}

void ioEngineerMachineGunAttack::DestroyDummy( ioBaseChar* pOwner )
{	
	if( pOwner && m_iDummyCharIndex > 0 )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_iDummyCharIndex );
		if( pDummy )
			pDummy->SetDieState( true );

		m_iDummyCharIndex = 0;
	}
}

ioEngineerGatlingDummy* ioEngineerMachineGunAttack::GetDummy( ioBaseChar* pOwner )
{
	ioDummyChar* pDummyChar			= g_DummyCharMgr.FindDummyChar( pOwner, m_iDummyCharIndex );
	ioEngineerGatlingDummy* pDummy	= ToEngineerGatlingDummy( pDummyChar );

	return pDummy;
}

void ioEngineerMachineGunAttack::CheckWeaponItemMesh( ioBaseChar* pChar )
{
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}

int ioEngineerMachineGunAttack::SetActionAni( IN ioBaseChar* pOwner, IN const ioHashString& szAnimation, IN float fTimeRate, OUT DWORD& dwStartTime, DWORD dwPreDelay )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllActionAni( FLOAT100 );
		pGrp->ClearAllLoopAni( FLOAT100 );

		int iAniID = pGrp->GetAnimationIdx( szAnimation );		
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fTimeRate, 0.0f, dwPreDelay, true );
		dwStartTime += pGrp->GetAnimationFullTime(iAniID) * fTimeRate;

		return iAniID;
	}

	return -1;
}

int ioEngineerMachineGunAttack::SetLoopAni( IN ioBaseChar* pOwner, IN const ioHashString& szAnimation, IN float fTimeRate )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllActionAni( FLOAT100 );
		pGrp->ClearAllLoopAni( FLOAT100 );

		int iAniID = pGrp->GetAnimationIdx( szAnimation );
		pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, fTimeRate );

		return iAniID;
	}

	return -1;
}

void ioEngineerMachineGunAttack::AttackFire( ioBaseChar* pOwner, DWORD dwStartTime, DWORD dwIndexBase )
{
	int iBase = 0;
	for( AttackWeaponInfoList::iterator iter = m_CurrAttackWeaponInfoList.begin(); iter != m_CurrAttackWeaponInfoList.end();  )
	{
		const AttackWeaponInfo& rkInfo = *iter;

		if( dwStartTime + rkInfo.m_dwFireTime < FRAMEGETTIME() )
		{			
			D3DXVECTOR3 vPos	= pOwner->GetWorldPosition();
			D3DXVECTOR3 vDir	= pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );

			D3DXVECTOR3 vSide;
			D3DXVec3Cross( &vSide, &vDir, &ioMath::UNIT_Y );
			D3DXVec3Normalize( &vSide, &vSide );

			vPos	+= vDir  * rkInfo.m_vOffset.z;
			vPos	+= vSide * rkInfo.m_vOffset.x;
			vPos.y	+= rkInfo.m_vOffset.y;

			ioWeapon* pWeapon = pOwner->SkillFireExplicit( rkInfo.m_WeaponInfo, GetName(), dwIndexBase + iBase++, vPos, vDir );
			ioSimpleBoxWeapon* pSimple = ToSimpleBoxWeapon( pWeapon );
			if( pSimple )
			{
				m_SimpleWeaponOffset = rkInfo.m_vOffset;
			}

			iter = m_CurrAttackWeaponInfoList.erase( iter );
		}
		else
		{
			++iter;
		}
	}
}

void ioEngineerMachineGunAttack::OnProcessState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	CheckDummy( pOwner, pStage );

	switch( m_SkillState )
	{
	case SS_PRE: 
		{
			CheckPreState( pOwner, pStage );
		}
		break;
	case SS_LOOP:
		{
			CheckLoopState( pOwner, pStage );
		}
		break;
	case SS_CHARGING:
		{
			CheckChargingState( pOwner, pStage );
		}
		break;
	case SS_GATLING_ATTACK:
		{
			CheckGatlingAttacktState( pOwner, pStage );
		}
		break;
	case SS_BEAM_ATTACK:
		{
			CheckBeamAttackState( pOwner, pStage );
		}
		break;
	case SS_SHOCK_ATTACK:
		{
			CheckShockAttackState( pOwner, pStage );
		}
		break;
	case SS_AIR_TRACE_ATTACK:
		{
			CheckAirTraceAttackState( pOwner, pStage );
		}
		break;
	case SS_END:
		{
		}
		break;
	}

	CheckKeyInput( pOwner );
	if( m_SkillState == SS_BEAM_ATTACK )
		ProcessRotate( pOwner, m_fBeamRotateSpeed );
	else
		ProcessRotate( pOwner, m_fRotateSpeed );
}

void ioEngineerMachineGunAttack::CheckPreState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwPreDelay = GetPreDelayTime();

	if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
	{
		m_dwPreDelayStartTime = 0;
		EndPreDelayEffect( pOwner );

		if( pOwner->IsNeedProcess() )
		{
			int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();	
			CreateDummy( pOwner, pStage, iCurIndex );
		}
		SetLoopState( pOwner, pStage );
	}
}

void ioEngineerMachineGunAttack::SetLoopState( ioBaseChar* pOwner, ioPlayStage* pStage, LoopInit eInit /* = LI_START */ )
{
	DWORD dwCurTime			= FRAMEGETTIME();
	m_SkillState			= SS_LOOP;
	m_dwChargingStartTime	= 0;	

	switch( eInit )
	{
	case LI_START:
		m_dwLoopStartTime = dwCurTime;
		break;
	default:
		{
			ioEngineerGatlingDummy* pDummy = GetDummy( pOwner );
			if( pDummy )
				pDummy->SetLoopDelayState();

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pOwner->GetCharName();
				kPacket << GetName();
				kPacket << (byte)SSS_LOOP;
				kPacket << m_iCurrBullet;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
		break;
	}

	RemoveWeapon( pOwner );
	SetLoopAni( pOwner, m_szLoopAnimation, m_fLoopAnimationRate );
}

void ioEngineerMachineGunAttack::CheckLoopState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwLoopStartTime + m_dwLoopDuration < dwCurTime || m_iCurrBullet == 0 )
	{
		m_SkillState = SS_END;
	}
	else
	{
		if( pOwner->IsAttackKey() && !pOwner->IsDefenseKey() && !pOwner->IsJumpKey() )
		{
			SetChargingState( pOwner, pStage );
		}
		else if( !pOwner->IsAttackKey() && pOwner->IsDefenseKey() && !pOwner->IsJumpKey() )
		{
			SetShockAttackState( pOwner, pStage );
		}
		else if( !pOwner->IsAttackKey() && !pOwner->IsDefenseKey() && pOwner->IsJumpKey() )
		{
			SetAirTraceAttackState( pOwner, pStage );
		}
	}
}

void ioEngineerMachineGunAttack::SetChargingState( ioBaseChar* pOwner, ioPlayStage* pStage )
{
	RemoveWeapon( pOwner );

	m_SkillState			= SS_CHARGING;
	m_dwChargingStartTime	= FRAMEGETTIME();

	SetLoopAni( pOwner, m_szChargingAnimation, m_fChargingAnimationRate );

	ioEngineerGatlingDummy* pDummy = GetDummy( pOwner );
	if( pDummy )
		pDummy->SetChargingState();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << (byte)SSS_CHARGING;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioEngineerMachineGunAttack::CheckChargingState( ioBaseChar* pOwner, ioPlayStage* pStage )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwChargingStartTime + m_dwChargingTime < FRAMEGETTIME() )
		{
			SetBeamAttackState( pOwner, pStage );
		}
	}
	else
	{
		SetGatlingAttackState( pOwner, pStage );
	}
}

void ioEngineerMachineGunAttack::SetGatlingAttackState( ioBaseChar* pOwner, ioPlayStage* pStage )
{
	RemoveWeapon( pOwner );

	m_SkillState				= SS_GATLING_ATTACK;
	m_dwGatlingAttackStartTime	= FRAMEGETTIME();
	m_dwGatlingAttackEndTime	= m_dwGatlingAttackStartTime;
	m_iCurrBullet				= max( 0, m_iCurrBullet - m_iGatlingAttackNeedBullet );
	m_CurrAttackWeaponInfoList	= m_GatlingAttackList;
	
	SetActionAni( pOwner, m_szGatlingAttackAnimation, m_fGatlingAttackAnimationRate, m_dwGatlingAttackEndTime, 0 );	
	
	ioEngineerGatlingDummy* pDummy = GetDummy( pOwner );
	if( pDummy )
		pDummy->SetGatlingAttackState();

	if( pOwner->IsNeedSendNetwork() )
	{
		m_dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();

		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << (byte)SSS_GATLING_ATTACK;
		kPacket << m_dwWeaponBaseIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioEngineerMachineGunAttack::CheckGatlingAttacktState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwGatlingAttackEndTime < dwCurTime )
	{
		SetLoopState( pOwner, pStage, LI_NONE );
	}
	else
	{
		FireWeaponList WeaponList;
		AttackFire( pOwner, m_dwGatlingAttackStartTime, m_dwWeaponBaseIndex );
	}
}

void ioEngineerMachineGunAttack::SetBeamAttackState( ioBaseChar* pOwner, ioPlayStage* pStage )
{
	RemoveWeapon( pOwner );

	m_SkillState						= SS_BEAM_ATTACK;
	m_iCurrBullet						= max( 0, m_iCurrBullet - m_iBeamAttackNeedBullet );
	m_dwBeamAattackBulletUseStartTime	= FRAMEGETTIME();
	m_CurrAttackWeaponInfoList			= m_BeamAttackList;

	SetLoopAni( pOwner, m_szBeamAttackAnimation, m_fBeamAttackAnimationRate );

	ioEngineerGatlingDummy* pDummy = GetDummy( pOwner );
	if( pDummy )
		pDummy->SetBeamAttackState();

	if( pOwner->IsNeedSendNetwork() )
	{
		m_dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();

		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << (byte)SSS_BEAM_ATTACK;
		kPacket << m_dwWeaponBaseIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioEngineerMachineGunAttack::CheckBeamAttackState( ioBaseChar *pOwner, ioPlayStage *pStage )
{

	if( pOwner->IsNeedProcess() && !pOwner->IsAttackKeyDown() )
	{
		SetLoopState( pOwner, pStage, LI_NONE );
	}
	else
	{
		if( pOwner->IsNeedProcess() )
		{
			DWORD dwCurTime = FRAMEGETTIME();
			if( m_dwBeamAattackBulletUseStartTime + m_dwBeamAattackBulletUseTime < dwCurTime )
			{
				m_dwBeamAattackBulletUseStartTime = dwCurTime;
				if( m_iCurrBullet == 0 )
				{
					SetLoopState( pOwner, pStage, LI_NONE );
				}
				else
				{				
					m_iCurrBullet = max( 0, m_iCurrBullet - m_iBeamAttackNeedBullet );
				}
			}
		}

		AttackFire( pOwner, m_dwBeamAattackBulletUseStartTime, m_dwWeaponBaseIndex );
		CheckWeapon( pOwner );
	}
}

void ioEngineerMachineGunAttack::CheckWeapon( ioBaseChar *pOwner )
{
	if( !m_BeamWeaponIndexList.empty() )
	{
		for( int i = 0; i < (int)m_BeamWeaponIndexList.size(); ++i )
		{
			ioSimpleBoxWeapon* pSimple = ToSimpleBoxWeapon( g_WeaponMgr.FindWeapon( pOwner, m_BeamWeaponIndexList[i] ) );
			if( pSimple )
			{
				D3DXVECTOR3 vPos	= pOwner->GetWorldPosition();
				D3DXVECTOR3 vDir	= pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
				D3DXVec3Normalize( &vDir, &vDir );

				D3DXVECTOR3 vSide;
				D3DXVec3Cross( &vSide, &vDir, &ioMath::UNIT_Y );
				D3DXVec3Normalize( &vSide, &vSide );

				vPos	+= vDir  * m_SimpleWeaponOffset.z;
				vPos	+= vSide * m_SimpleWeaponOffset.x;
				vPos.y	+= m_SimpleWeaponOffset.y;

				pSimple->SetPosition( vPos );
				pSimple->SetMoveDir( vDir );				
			}
		}
	}
}

void ioEngineerMachineGunAttack::RemoveWeapon( ioBaseChar *pOwner )
{
	for( IntVec::iterator iter = m_BeamWeaponIndexList.begin(); iter != m_BeamWeaponIndexList.end(); )
	{
		ioSimpleBoxWeapon* pSimple = ToSimpleBoxWeapon( g_WeaponMgr.FindWeapon( pOwner, *iter ) );
		if( pSimple )
		{
			pSimple->SetWeaponDead();			
			iter = m_BeamWeaponIndexList.erase( iter );
		}
		else
		{
			iter++;
		}
	}	
}

void ioEngineerMachineGunAttack::SetShockAttackState( ioBaseChar* pOwner, ioPlayStage* pStage )
{
	RemoveWeapon( pOwner );

	m_SkillState				= SS_SHOCK_ATTACK;
	m_dShockAttackStartTime		= FRAMEGETTIME();
	m_dShockAttackEndTime		= m_dShockAttackStartTime;
	m_iCurrBullet				= max( 0, m_iCurrBullet - m_iShockAttackNeedBullet );
	m_CurrAttackWeaponInfoList	= m_ShockAttackList;

	SetActionAni( pOwner, m_szShockAttackAnimation, m_fShockAttackAnimationRate, m_dShockAttackEndTime, 0 );

	ioEngineerGatlingDummy* pDummy = GetDummy( pOwner );
	if( pDummy )
		pDummy->SetShockAttackState();

	if( pOwner->IsNeedSendNetwork() )
	{
		m_dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();

		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << (byte)SSS_SHOCK_ATTACK;
		kPacket << m_dwWeaponBaseIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioEngineerMachineGunAttack::CheckShockAttackState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if( m_dShockAttackEndTime < FRAMEGETTIME() )
	{
		SetLoopState( pOwner, pStage, LI_NONE );
	}
	else
	{
		AttackFire( pOwner, m_dShockAttackStartTime, m_dwWeaponBaseIndex );
	}
}

void ioEngineerMachineGunAttack::SetAirTraceAttackState( ioBaseChar* pOwner, ioPlayStage* pStage )
{
	RemoveWeapon( pOwner );

	m_SkillState				= SS_AIR_TRACE_ATTACK;
	m_dwAirTraceAttackStartTime	= FRAMEGETTIME();
	m_dwAirTraceAttackEndTime	= m_dwAirTraceAttackStartTime;
	m_iCurrBullet				= max( 0, m_iCurrBullet - m_iAirTraceAttackNeedBullet );
	m_CurrAttackWeaponInfoList	= m_AirTraceAttackList;

	SetActionAni( pOwner, m_szAirTraceAttackAnimation, m_fAirTraceAttackAnimationRate, m_dwAirTraceAttackEndTime, 0 );

	ioEngineerGatlingDummy* pDummy = GetDummy( pOwner );
	if( pDummy )
		pDummy->SetAirTraceAttack();

	if( pOwner->IsNeedSendNetwork() )
	{
		m_dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();

		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << (byte)SSS_AIR_TRACE_ATTACK;
		kPacket << m_dwWeaponBaseIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioEngineerMachineGunAttack::CheckAirTraceAttackState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if( m_dwAirTraceAttackEndTime < FRAMEGETTIME() )
	{
		SetLoopState( pOwner, pStage, LI_NONE );
	}
	else
	{
		AttackFire( pOwner, m_dwAirTraceAttackStartTime, m_dwWeaponBaseIndex );
	}
}

void ioEngineerMachineGunAttack::CheckKeyInput( ioBaseChar* pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		m_CurDirKey = eNewDirKey;

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pOwner->GetCharName();
			kPacket << GetName();
			kPacket << (byte)SSS_DIR_KEY;
			kPacket << pOwner->GetTargetRot();
			kPacket << (int)m_CurDirKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioEngineerMachineGunAttack::ProcessRotate( ioBaseChar *pChar, float fRotateTime )
{
	if( !pChar )
		return;

	if( fRotateTime <= 0 )
		return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / fRotateTime;
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

	if( fDotValue >= 0.98f )
	{
		pChar->SetTargetRotToRotate( qtRotate, true );
		return;
	}

	D3DXVECTOR3 vAxis;
	if( fDotValue <= -0.98f )
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

void ioEngineerMachineGunAttack::OnSkillEnd( ioBaseChar *pChar )
{
	ioNormalSkill::OnSkillEnd( pChar );

	RemoveWeapon( pChar );

	if( GetOwnerItem() )
		GetOwnerItem()->ClearCurChargeRateForSkill();

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}
	if( !m_HandMesh.IsEmpty() )
	{
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	}

	DestroyDummy( pChar );

	Init();
}

bool ioEngineerMachineGunAttack::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_PRE:
	case SS_LOOP:
	case SS_CHARGING:
	case SS_GATLING_ATTACK:
	case SS_BEAM_ATTACK:
	case SS_SHOCK_ATTACK:
	case SS_AIR_TRACE_ATTACK:
		return false;
	}

	return true;
}

bool ioEngineerMachineGunAttack::IsAttackEndState() const
{
	switch( m_SkillState )
	{
	case SS_PRE:
	case SS_LOOP:
	case SS_CHARGING:
	case SS_GATLING_ATTACK:
	case SS_BEAM_ATTACK:
	case SS_SHOCK_ATTACK:
	case SS_AIR_TRACE_ATTACK:
		return false;
	}

	return true;
}

bool ioEngineerMachineGunAttack::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioEngineerMachineGunAttack::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioNormalSkill::ApplyAttributeToWeapon( pWeapon, pStage );

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

	ioSimpleBoxWeapon* pSimple = ToSimpleBoxWeapon( pWeapon );
	if( pSimple )
	{
		RemoveWeapon( pSimple->GetOwner() );
		m_BeamWeaponIndexList.push_back( pSimple->GetWeaponIndex() );
	}
}

void ioEngineerMachineGunAttack::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	byte eState;
	rkPacket >> eState;

	switch( eState )
	{
	case SSS_DIR_KEY:
		{			
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			int DirKey;
			rkPacket >> DirKey;
			
			m_CurDirKey = (ioUserKeyInput::DirKeyInput)DirKey;
			pChar->SetTargetRotToRotate( qtRot, true, false );
		}
		break;
	case SSS_CREATE_DUMMY:
		{
			int iCurIndex;
			rkPacket >> iCurIndex;
			CreateDummy( pChar, pStage, iCurIndex );
		}
		break;
	case SSS_CHARGING:
		{
			SetChargingState( pChar, pStage );
		}
		break;
	case SSS_GATLING_ATTACK:
		{
			rkPacket >> m_dwWeaponBaseIndex;
			pChar->SetWeaponIndexBase( m_dwWeaponBaseIndex );

			SetGatlingAttackState( pChar, pStage );
		}
		break;
	case SSS_BEAM_ATTACK:
		{
			rkPacket >> m_dwWeaponBaseIndex;
			pChar->SetWeaponIndexBase( m_dwWeaponBaseIndex );

			SetBeamAttackState( pChar, pStage );
		}
		break;
	case SSS_LOOP:
		{
			rkPacket >> m_iCurrBullet;
			SetLoopState( pChar, pStage, LI_NONE );
		}
		break;
	case SSS_SHOCK_ATTACK:
		{
			rkPacket >> m_dwWeaponBaseIndex;
			pChar->SetWeaponIndexBase( m_dwWeaponBaseIndex );

			SetShockAttackState( pChar, pStage );
		}
		break;
	case SSS_AIR_TRACE_ATTACK:
		{
			rkPacket >> m_dwWeaponBaseIndex;
			pChar->SetWeaponIndexBase( m_dwWeaponBaseIndex );

			SetAirTraceAttackState( pChar, pStage );
		}
		break;
	}
}

bool ioEngineerMachineGunAttack::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	// 스킬 무적 파괴
	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{		
		if( iDefenseBreakType == m_ProtectCancelType )
			return false;
	}

	if( m_dwSkillStartTime + m_dwSkillProtectTime < FRAMEGETTIME() )
		return false;
	
	return true;
}