#include "stdafx.h"

#include "ioTwoStateAttack.h"

#include "ioBaseChar.h"

#include "ioPlayStage.h"

#include "WeaponDefine.h"

ioTwoStateAttack::ioTwoStateAttack()
{
	ClearData();
}

ioTwoStateAttack::ioTwoStateAttack( const ioTwoStateAttack &rhs )
	: ioAttackSkill( rhs ),
m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
m_SkillAirCenterLoopMotion( rhs.m_SkillAirCenterLoopMotion ),
m_SkillAirDownLoopMotion( rhs.m_SkillAirDownLoopMotion ),
m_skillAirUpLoopMotion( rhs.m_skillAirUpLoopMotion ),
m_fSkillLoopMotionRate( rhs.m_fSkillLoopMotionRate ),
m_fSkillAirLoopMotionRate( rhs.m_fSkillAirLoopMotionRate ),
m_dwSkillLoopDuration( rhs.m_dwSkillLoopDuration ),
m_dwSkillAirLoopDuration( rhs.m_dwSkillAirLoopDuration ),
m_dwSkillLoopProtectTime( rhs.m_dwSkillLoopProtectTime ),
m_dwSkillAirLoopProtectTime( rhs.m_dwSkillAirLoopProtectTime ),
m_dwSkillLoopRotateSpeed( rhs.m_dwSkillLoopRotateSpeed ),
m_SkillAirCenterMotion( rhs.m_SkillAirCenterMotion ),
m_SkillAirDownMotion( rhs.m_SkillAirDownMotion ), 
m_SkillAirUpMotion( rhs.m_SkillAirUpMotion ),
m_fAirAnimationTimeRate( rhs.m_fAirAnimationTimeRate ),
m_dwAirPreDelay( rhs.m_dwAirPreDelay ),
m_dwAirEndDelay( rhs.m_dwAirEndDelay ),
m_vWeaponInfoList( rhs.m_vWeaponInfoList ),
m_fCircleRange( rhs.m_fCircleRange ),
m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
m_fCircleOffSet( rhs.m_fCircleOffSet ),
m_CircleEffect( rhs.m_CircleEffect ),
m_RedEffect( rhs.m_RedEffect ),
m_BlueEffect( rhs.m_BlueEffect ),
m_SkillMapEffect( rhs.m_SkillMapEffect ),
m_SkillMapAirEffect( rhs.m_SkillMapAirEffect ),
m_FireGuidEffect( rhs.m_FireGuidEffect ),
m_ExtraGuidEffect( rhs.m_ExtraGuidEffect ),
m_fSkillMapOffset( rhs.m_fSkillMapOffset ),
m_fCharJumpPower( rhs.m_fCharJumpPower )
{
	ClearData();
}

ioTwoStateAttack::~ioTwoStateAttack()
{
}

void ioTwoStateAttack::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];
	
	//Loop
	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_SkillLoopMotion = szBuf;
	m_fSkillLoopMotionRate = rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );
	m_dwSkillLoopDuration = rkLoader.LoadInt_e( "skill_loop_duration", 0 );
	m_dwSkillLoopProtectTime = rkLoader.LoadInt_e( "skill_loop_protect_time", 1000 );
	m_dwSkillLoopRotateSpeed = (DWORD)rkLoader.LoadInt_e( "skill_loop_rotate_speed", 0 );
	
	rkLoader.LoadString_e( "skill_air_center_loop_motion", "", szBuf, MAX_PATH );
	m_SkillAirCenterLoopMotion = szBuf;

	rkLoader.LoadString_e( "skill_air_down_loop_motion", "", szBuf, MAX_PATH );
	m_SkillAirDownLoopMotion = szBuf;

	rkLoader.LoadString_e( "skill_air_up_loop_motion", "", szBuf, MAX_PATH );
	m_skillAirUpLoopMotion = szBuf;

	m_fSkillAirLoopMotionRate = rkLoader.LoadFloat_e( "skill_air_loop_motion_rate", FLOAT1 );
	m_dwSkillAirLoopDuration = rkLoader.LoadInt_e( "skill_air_loop_duration", 0 );
	m_dwSkillAirLoopProtectTime = rkLoader.LoadInt_e( "skill_air_loop_protect_time", 1000 );
	
	//action
	rkLoader.LoadString_e( "skill_map_effect", "", szBuf, MAX_PATH );
	m_SkillMapEffect = szBuf;

	rkLoader.LoadString_e( "skill_map_air_effect", "", szBuf, MAX_PATH );
	m_SkillMapAirEffect = szBuf;

	rkLoader.LoadString_e( "fire_guid_effect", "", szBuf, MAX_PATH );
	m_FireGuidEffect = szBuf;

	rkLoader.LoadString_e( "extra_guid_effect", "", szBuf, MAX_PATH );
	m_ExtraGuidEffect = szBuf;

	m_fSkillMapOffset = rkLoader.LoadFloat_e( "skill_map_offset", 0.0f );

	//Air Action
	rkLoader.LoadString_e( "skill_air_center_motion", "", szBuf, MAX_PATH );
	m_SkillAirCenterMotion = szBuf;

	rkLoader.LoadString_e( "skill_air_down_motion", "", szBuf, MAX_PATH );
	m_SkillAirDownMotion = szBuf;
	
	rkLoader.LoadString_e( "skill_air_up_motion", "", szBuf, MAX_PATH );
	m_SkillAirUpMotion = szBuf;

	m_fAirAnimationTimeRate = rkLoader.LoadFloat_e( "skill_animation_air_time_rate", FLOAT1 );
	m_dwAirPreDelay = rkLoader.LoadInt_e( "skill_animation_air_pre_delay", 0 );
	m_dwAirEndDelay = rkLoader.LoadInt_e( "skill_animation_air_end_delay", 0 );
	
	int iAttackTypeCnt = rkLoader.LoadInt_e( "skill_air_attack_type_count", 0 );
	m_vWeaponInfoList.clear();
	m_vWeaponInfoList.reserve( iAttackTypeCnt );

	for( int i=0 ; i<iAttackTypeCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "skill_air_attack_type%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "skill_air_attack_type%d_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "skill_air_attack_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "skill_air_attack_type%d_wounded_time", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "skill_air_attack_type%d_wounded_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_vWeaponInfoList.push_back( kInfo );
	}

	//magic circle
	m_fCircleRange = rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	rkLoader.LoadString_e( "magic_circle", "", szBuf, MAX_PATH );
	m_CircleEffect = szBuf;
	rkLoader.LoadString_e( "red_team_circle", "", szBuf, MAX_PATH );
	m_RedEffect = szBuf;
	rkLoader.LoadString_e( "blue_team_circle", "", szBuf, MAX_PATH );
	m_BlueEffect = szBuf;

	m_fCharJumpPower = rkLoader.LoadFloat_e( "skill_jump_power", 0.0f );	
}

ioSkill* ioTwoStateAttack::Clone()
{
	return new ioTwoStateAttack( *this );
}

void ioTwoStateAttack::ClearData()
{
	m_State = SS_PRE;

	m_bSetHandMesh = false;
	m_bReduceGauge = false;	

	m_dwJumpMotionEndTime = 0;
	m_dwLoopEndTime = 0;

	m_dwSkillLoopProtectEndTime = 0;
	
	m_dwSkillMapEffect = -1;
	m_dwFireGuidEffect = -1;
	m_dwExtraGuidEffect = -1;

	m_vAttackDir = ioMath::VEC3_ZERO;
	m_vMagicCirclePos = ioMath::VEC3_ZERO;

	m_fCurMotionRate = 0.0f;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_vAttackMoitonDir = ioMath::VEC3_ZERO;

	m_bAirState = false;
}

bool ioTwoStateAttack::OnSkillStart( ioBaseChar *pChar )
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

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	ClearData();

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	if( m_bInitJumpPower )
	{		
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}
	
	m_vAttackMoitonDir = pChar->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &m_vAttackMoitonDir, &m_vAttackMoitonDir );

	if( GetSkillUseType() == SUT_NORMAL )
	{		
		m_State = SS_PRE;
	}
	else
	{
		if( GetPreDelayAniName().IsEmpty() && GetPreDelayTime() <= 0 )
			SetAirLoopState( pChar, pChar->GetCreator() );
		else		
			SetJumpingState( pChar, pChar->GetCreator() );
	}

	return true;
}

void ioTwoStateAttack::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	
	switch( m_State )
	{
	case SS_PRE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );								
				SetLoopState( pChar, pStage );			
			}
		}
		break;
	case SS_JUMP_PRE:
		{
			if( 0 < m_dwJumpMotionEndTime && m_dwJumpMotionEndTime < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );
				SetAirLoopState( pChar, pStage );
			}
		}
		break;
	case SS_LOOP:
		{
			CheckKeyInput( pChar );
			ProcessRotate( pChar,  m_dwSkillLoopRotateSpeed );
			UpdateEffect( pChar, pStage );

			if( CheckLoopState( pChar, pStage ) )
			{
				SetActionState( pChar, pStage );
				return;
			}

			if( 0 < m_dwLoopEndTime && m_dwLoopEndTime < dwCurTime )
			{
				SetActionState( pChar, pStage );
			}
		}
		break;
	case SS_AIR_LOOP:
		{
			CheckMagicCircle( pChar, pStage );
			CheckMotion( pChar, pStage );

			if( CheckLoopState( pChar, pStage ) )
			{
				SetAirActionState( pChar, pStage );
				return;
			}

			if( 0 < m_dwLoopEndTime && m_dwLoopEndTime < dwCurTime )
			{
				SetAirActionState( pChar, pStage );
			}
		}
		break;
	case SS_ACTION:
		{
			if( 0 < m_dwMotionEndTime && m_dwMotionEndTime < dwCurTime )
			{
				SetEndState( pChar, pStage );
			}
		}
		break;
	case SS_AIR_ACTION:
		{
			if( 0 < m_dwMotionEndTime && m_dwMotionEndTime < dwCurTime )
			{
				SetEndState( pChar, pStage );
			}
		}
		break;
	case SS_END:
		{

		}
		break;
	}
}

void ioTwoStateAttack::SetJumpingState( ioBaseChar *pChar, ioPlayStage* pStage )
{
	m_State = SS_JUMP_PRE;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( GetPreDelayAniName() );
	if( iAniID == -1 )
		return;
		
	pChar->SetJumpPower( m_fCharJumpPower * m_fSkillGravityRate );
	

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1, 0.0f, GetPreDelayTime(), true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwJumpMotionEndTime = dwCurTime;
	m_dwJumpMotionEndTime += pGrp->GetAnimationFullTime( iAniID );
}

void ioTwoStateAttack::SetActionState( ioBaseChar *pChar, ioPlayStage* pStage, bool bSend )
{
	if( !pChar )
		return;
	
	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge( pChar->GetCharName() ) );
		m_bReduceGauge = true;
	}	

	if( m_dwSkillMapEffect != -1 )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwSkillMapEffect );
		if( pMapEffect )
			pMapEffect->EndEffectForce();

		m_dwSkillMapEffect = -1;
	}

	if( m_dwExtraGuidEffect != -1 )
	{
		pChar->EndEffect( m_dwExtraGuidEffect, false );
		m_dwExtraGuidEffect = -1;
	}

	if( m_dwFireGuidEffect != -1 )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwFireGuidEffect );
		if( pMapEffect )
			pMapEffect->EndEffectForce();

		m_dwFireGuidEffect = -1;
	}

	m_vAttackDir = pChar->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);

	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	vPos.y = pChar->GetMidPositionByRate( FLOAT09 ).y;
	vPos += m_fSkillMapOffset * m_vAttackDir;

	ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_FireGuidEffect, vPos, vScale );
	if( pMapEffect )
	{
		m_dwFireGuidEffect = pMapEffect->GetUniqueID();

		D3DXQUATERNION qtCharRot;
		ioMath::CalcDirectionQuaternion( qtCharRot, m_vAttackDir );
		pMapEffect->SetWorldOrientation( qtCharRot );
	}

	m_State = SS_ACTION;
	ioAttackSkill::AttackSkillFire( pChar );
	
	if( pChar->IsNeedProcess() && pChar->IsNeedSendNetwork() && bSend )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_ACTION;
		kPacket << pChar->GetTargetRot();
		kPacket << m_vAttackDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTwoStateAttack::SetAirActionState( ioBaseChar *pChar, ioPlayStage* pStage, bool bSend )
{
	if( !pChar )
		return;
		
	CheckMotion( pChar, pStage );

	m_State = SS_AIR_ACTION;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_SkillAirCenterMotion );
	if( iAniID == -1 )	return;

	pChar->AttachEffect( m_SkillEffectName );

	float fTimeRate  = m_fAirAnimationTimeRate;
	DWORD dwPreDelay = m_dwAirPreDelay;
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_bRefreshFireTimeList )
	{
		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
			iAniID,
			m_vWeaponInfoList,
			FTT_SKILL_ATTACK,
			fTimeRate,
			dwPreDelay );
	}
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );	

	if( m_vAttackMoitonDir.y == 0.0f || m_fCurMotionRate == 0.0f )
	{
		// center
		pGrp->SetActionAni( iAniID, 0.0f, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
	}
	else if( m_vAttackMoitonDir.y > 0.0f )	
	{
		// center & up
		pGrp->SetActionAni( m_SkillAirUpMotion, 0.0f, 0.0f, m_fCurMotionRate, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
		pGrp->SetActionAni( iAniID, 0.0f, 0.0f, FLOAT1 - m_fCurMotionRate, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
	}
	else
	{
		// center & down
		pGrp->SetActionAni( m_SkillAirDownMotion, 0.0f, 0.0f, m_fCurMotionRate, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
		pGrp->SetActionAni( iAniID, 0.0f, 0.0f, FLOAT1 - m_fCurMotionRate, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
	}

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + m_dwAirEndDelay;

	m_dwFireMotionEndTime += dwPreDelay;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
	ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_SkillMapAirEffect, pChar->GetMagicCirclePos(), vScale );
	if( pMapEffect )
	{
		m_dwSkillMapAirEffect = pMapEffect->GetUniqueID();
	}

	if( pChar->CheckMagicCircle() )
		pChar->DestroyMagicCircle();

	if( pChar->IsNeedProcess() && pChar->IsNeedSendNetwork() && bSend )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_AIR_ACTION;
		kPacket << pChar->GetTargetRot();
		kPacket << m_vAttackDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTwoStateAttack::SetLoopState( ioBaseChar* pChar, ioPlayStage* pStage )
{
	if( !pChar )
		return;

	if( !pStage )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_State = SS_LOOP;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	int iAniID = pGrp->GetAnimationIdx( m_SkillLoopMotion );
	float fAniRate = FLOAT1;
	if( m_fSkillLoopMotionRate > 0.0f )
		fAniRate = m_fSkillLoopMotionRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_dwLoopEndTime = dwCurTime;
	m_dwLoopEndTime += m_dwSkillLoopDuration;

	m_dwSkillLoopProtectEndTime = dwCurTime;
	m_dwSkillLoopProtectEndTime += m_dwSkillLoopProtectTime;

	// Map Effect
	ioMapEffect *pMapEffect = NULL;
	if( m_dwSkillMapEffect != -1 )
	{
		pMapEffect = pStage->FindMapEffect( m_dwSkillMapEffect );
		if( pMapEffect )
			pMapEffect->EndEffectForce();

		m_dwSkillMapEffect = -1;
	}

	m_vAttackDir = pChar->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	
	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	vPos.y = pChar->GetMidPositionByRate( FLOAT09 ).y;
	vPos += m_fSkillMapOffset * m_vAttackDir;

	pMapEffect = pStage->CreateMapEffect( m_SkillMapEffect, vPos, vScale );
	if( pMapEffect )
	{			
		m_dwSkillMapEffect = pMapEffect->GetUniqueID();

		D3DXQUATERNION qtCharRot;
		ioMath::CalcDirectionQuaternion( qtCharRot, m_vAttackDir );
		pMapEffect->SetWorldOrientation( qtCharRot );
	}

	ioEffect *pEffect = pChar->AttachEffect( m_ExtraGuidEffect );
	if( pEffect )
	{
		m_dwExtraGuidEffect = pEffect->GetUniqueID();
	}
	
}

void ioTwoStateAttack::SetAirLoopState( ioBaseChar* pChar, ioPlayStage* pStage )
{
	if( !pChar )
		return;

	if( !pStage )
		return;

	pChar->SetJumpPower( m_fCharJumpPower * m_fSkillGravityRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_State = SS_AIR_LOOP;
	m_bAirState = true;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;
		
	float fAniRate = FLOAT1;
	if( m_fSkillAirLoopMotionRate > 0.0f )
		fAniRate = m_fSkillAirLoopMotionRate;

	pGrp->ClearAllActionAni( FLOAT100 );
	pGrp->ClearAllLoopAni( FLOAT100 );

	pGrp->SetLoopAni( m_skillAirUpLoopMotion, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_SkillAirDownLoopMotion, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_SkillAirCenterLoopMotion, 0.0f, FLOAT1 );

	m_dwLoopEndTime = dwCurTime;
	m_dwLoopEndTime += m_dwSkillAirLoopDuration;

	m_dwSkillLoopProtectEndTime = dwCurTime;
	m_dwSkillLoopProtectEndTime += m_dwSkillAirLoopProtectTime;

	if( !pChar->CheckMagicCircle() )
	{
		ioHashString szDummyCircle;
		pChar->CreateMagicCircle( m_CircleEffect, m_CircleEffect, szDummyCircle, m_fCircleOffSet );		
	}
}

bool ioTwoStateAttack::CheckLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( pChar->IsNeedProcess() )
	{
		//키입력 여부 검사
		bool bReserveSkill = false;
		int iSkillNum = pChar->GetSkillKeyInput();
		if( iSkillNum >= 0 )
		{
			bReserveSkill = true;
		}

		if( pChar->IsAttackKey() && !bReserveSkill )
		{						
			return true;
		}		
	}

	return false;
}

void ioTwoStateAttack::CheckKeyInput( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( !pChar->IsNeedProcess() ) return;

	ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{	
		m_CurDirKey = eNewDirKey;

		if( pChar->IsNeedSendNetwork() )
		{
			if( m_State == SS_LOOP )
			{		
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pChar->GetCharName();
				kPacket << GetName();
				kPacket << SSC_LOOP;				
				kPacket << pChar->GetTargetRot();
				kPacket << (int)m_CurDirKey;
				P2PNetwork::SendToAllPlayingUser( kPacket );

			}	
		}
	}
}

void ioTwoStateAttack::ProcessRotate( ioBaseChar *pChar, float fRotateSpeed )
{
	if( !pChar ) return;
	if( fRotateSpeed <= 0 ) return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / fRotateSpeed;
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

	m_vAttackDir = pChar->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
}

void ioTwoStateAttack::UpdateEffect( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_dwSkillMapEffect == -1 )
		return;

	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	vPos.y = pChar->GetMidPositionByRate( FLOAT09 ).y;
	vPos += m_fSkillMapOffset * m_vAttackDir;

	ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwSkillMapEffect );
	if( pMapEffect )
	{		
		pMapEffect->SetWorldPosition( vPos );

		D3DXQUATERNION qtCharRot;
		ioMath::CalcDirectionQuaternion( qtCharRot, m_vAttackDir );
		pMapEffect->SetWorldOrientation( qtCharRot );	
	}
}

void ioTwoStateAttack::CheckMotion( ioBaseChar* pChar, ioPlayStage* pStage )
{
	m_vAttackMoitonDir = pChar->GetMagicCirclePos() - pChar->GetWorldPosition();
	D3DXVec3Normalize( &m_vAttackMoitonDir, &m_vAttackMoitonDir );

	//check sight direction
	D3DXVECTOR3 vSightDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vSightDir, &vSightDir );

	float fAngle = 0.0f;
	float fDotValue = D3DXVec3Dot( &m_vAttackMoitonDir, &vSightDir );
	fAngle = RADtoDEG( acosf(fDotValue) );
	
	//Check motion	
	m_fCurMotionRate = fAngle /FLOAT90;

	ioEntityGroup *pGrp = pChar->GetGroup();
	ioAnimation *pCenter, *pDown ,*pUp;
	pCenter = pDown = NULL;
		
	pUp = pGrp->GetLoopInPlay( m_skillAirUpLoopMotion );
	pCenter = pGrp->GetLoopInPlay( m_SkillAirCenterLoopMotion );
	pDown = pGrp->GetLoopInPlay( m_SkillAirDownLoopMotion );

	if( !pCenter || !pDown || !pUp )
	{
		LOG.PrintTimeAndLog( 0, "ioTwoStateAttack::CheckMotion - Motion Error" );
		return;
	}

	if( m_vAttackMoitonDir.y == 0.0f || m_fCurMotionRate == 0.0f )	// center
	{
		pUp->ForceWeight( 0.0f );
		pCenter->ForceWeight( FLOAT1 );
		pDown->ForceWeight( 0.0f );
	}
	else if( m_vAttackMoitonDir.y > 0.0f )							// center & up
	{
		pUp->ForceWeight( m_fCurMotionRate );
		pCenter->ForceWeight( FLOAT1 - m_fCurMotionRate );
		pDown->ForceWeight( 0.0f );
	}
	else															// center & down
	{
		pUp->ForceWeight( 0.0f );
		pCenter->ForceWeight( FLOAT1 - m_fCurMotionRate );
		pDown->ForceWeight( m_fCurMotionRate );
	}
	
}

void ioTwoStateAttack::CheckMagicCircle( ioBaseChar* pChar, ioPlayStage* pStage )
{
	// 뱡향 변화 체크
	pChar->CheckChangeDirectionForMagicCircle();

	// MagicCircle 위치 갱신
	pChar->UpdateMagicCirclePosAll( m_fCircleMoveSpeed, m_fCircleRange, m_fCircleHeightGap, false );

	// MagicCircle 위치방향으로 캐릭터 회전
	D3DXVECTOR3 vTargetPos = pChar->GetMagicCirclePos();
	pChar->SetTargetRotToTargetPos( vTargetPos, true, false );

	m_vAttackDir = pChar->GetMagicCirclePos() - pChar->GetMidPositionByRate();
	D3DXVec3Normalize( &m_vAttackDir, &m_vAttackDir );
}

void ioTwoStateAttack::SetEndState( ioBaseChar* pChar, ioPlayStage* pStage )
{
	if( !pChar )
		return;

	if( !pStage )
		return;

	m_State = SS_END;

	if( pChar->CheckMagicCircle() )
		pChar->DestroyMagicCircle();

	if( m_dwSkillMapEffect != -1 )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwSkillMapEffect );
		if( pMapEffect )
			pMapEffect->EndEffectForce();

		m_dwSkillMapEffect = -1;
	}

	if( m_dwExtraGuidEffect != -1 )
	{
		pChar->EndEffect( m_dwExtraGuidEffect, false );
		m_dwExtraGuidEffect = -1;
	}

	if( m_dwFireGuidEffect != -1 )
	{
		pChar->EndMapEffect( m_dwFireGuidEffect, true );
		m_dwFireGuidEffect = -1;
	}
}

void ioTwoStateAttack::OnSkillEnd( ioBaseChar *pChar )
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
		
	if( pChar->CheckMagicCircle() )
		pChar->DestroyMagicCircle();

	ioPlayStage *pStage = pChar->GetCreator();
	if( pStage )
	{
		if( m_dwSkillMapEffect != -1 )
		{
			ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwSkillMapEffect );
			if( pMapEffect )
				pMapEffect->EndEffectForce();

			m_dwSkillMapEffect = -1;
		}
	}

	if( m_dwExtraGuidEffect != -1 )
	{		
		pChar->EndEffect( m_dwExtraGuidEffect, false );
		m_dwExtraGuidEffect = -1;
	}

	if( m_dwFireGuidEffect != -1 )
	{
		pChar->EndMapEffect( m_dwFireGuidEffect, false );
		m_dwFireGuidEffect = -1;
	}

	ClearData();
}

bool ioTwoStateAttack::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_State )
	{
	case SS_PRE:
	case SS_JUMP_PRE:
	case SS_LOOP:	
	case SS_AIR_LOOP:
	case SS_ACTION:
	case SS_AIR_ACTION:
		return false;

	case SS_END:	
		if( m_bAirState )
			return false;
	}

	return true;
}

bool ioTwoStateAttack::IsAttackEndState() const
{	
	if( m_State == SS_END )
		return true;

	return false;
}

bool ioTwoStateAttack::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioTwoStateAttack::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	
	ioIgnoreMissileWeapon* pMissile = ToIgnoreMissileWeapon( pWeapon );

	if( pMissile )
	{
		pMissile->SetMoveDir( m_vAttackDir );
	}
}

void ioTwoStateAttack::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;

	switch( iState )
	{
		case SSC_LOOP:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			int iDirKey;
			rkPacket >> iDirKey;

			pChar->SetTargetRotToRotate( qtRot, true, false );
			m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDirKey;
		}
		break;
		case SSC_ACTION:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;			
			rkPacket >> m_vAttackDir;

			pChar->SetTargetRotToRotate( qtRot, true, false );
			SetActionState( pChar, pStage, false );
		}
		break;
		case SSC_AIR_ACTION:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			rkPacket >> m_vAttackDir;

			pChar->SetTargetRotToRotate( qtRot, true, false );
			SetAirActionState( pChar, pStage, false );
		}
		break;
	}
}

bool ioTwoStateAttack::IsNoDropState() const
{	
	if( m_State == SS_AIR_LOOP || m_State == SS_AIR_ACTION )
		return true;

	return false;
}

bool ioTwoStateAttack::IsProtected( int iDefenseBreakType ) const
{	

	if( m_bDisableSkillProtect )
		return false;

	// 스킬 무적 파괴
	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{		
		if( iDefenseBreakType == m_ProtectCancelType )
			return false;
	}

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_PRE:
		return true;
		break;

	case SS_JUMP_PRE:
		return true;
		break;

	case SS_LOOP:
		if( 0 < m_dwSkillLoopProtectEndTime && dwCurTime < m_dwSkillLoopProtectEndTime )
			return true;

		break;
	case SS_AIR_LOOP:
		if( 0 < m_dwSkillLoopProtectEndTime && dwCurTime < m_dwSkillLoopProtectEndTime )
			return true;

		break;
	case SS_ACTION:
		if( 0 < m_dwFireMotionEndTime && dwCurTime < m_dwFireMotionEndTime )
			return true;

		break;
	case SS_AIR_ACTION:
		if( 0 < m_dwFireMotionEndTime && dwCurTime < m_dwFireMotionEndTime )
			return true;
		break;
	}

	return false;
}