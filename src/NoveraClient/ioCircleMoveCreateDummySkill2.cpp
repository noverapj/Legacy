#include "StdAfx.h"
#include "ioCircleMoveCreateDummySkill2.h"

#include "ioFloatThrowWeapon.h"
#include "ioZoneEffectWeapon.h"

ioCircleMoveCreateDummySkill2::ioCircleMoveCreateDummySkill2(void)
{
	ClearData();
}

ioCircleMoveCreateDummySkill2::ioCircleMoveCreateDummySkill2( const ioCircleMoveCreateDummySkill2 &rhs ):
ioNormalSkill( rhs ),
m_fCircleMinRange( rhs.m_fCircleMinRange ),
m_fCircleMaxRange( rhs.m_fCircleMaxRange ),
m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
m_fCircleOffSet( rhs.m_fCircleOffSet ),
m_ReadyCircle( rhs.m_ReadyCircle ),
m_szReadyCircleDropZone( rhs.m_szReadyCircleDropZone ),
m_RedEffect( rhs.m_RedEffect ),
m_BlueEffect( rhs.m_BlueEffect ),
m_szDummyName( rhs.m_szDummyName ),
m_vDummyOffset( rhs.m_vDummyOffset ),
m_iStartDummyAniCode( rhs.m_iStartDummyAniCode ),
m_iLoopDummyAniCode( rhs.m_iLoopDummyAniCode ),
m_iActionDummyAniCode( rhs.m_iActionDummyAniCode ),
m_dwLoopDuration( rhs.m_dwLoopDuration ),
m_bEnableDropZoneAttack( rhs.m_bEnableDropZoneAttack ),
m_fWeaponFloatPower( rhs.m_fWeaponFloatPower ),
m_fWeaponFloatMinPower( rhs.m_fWeaponFloatMinPower ),
m_fWeaponMinSpeed( rhs.m_fWeaponMinSpeed ),
m_fWeaponGravityRate( rhs.m_fWeaponGravityRate ),
m_fRevisionRate( rhs.m_fRevisionRate ),
m_fUpLimitHeight( rhs.m_fUpLimitHeight ),
m_fUpLimitRange( rhs.m_fUpLimitRange ),
m_iMaxAttackCount( rhs.m_iMaxAttackCount ),
m_CannonAttack( rhs.m_CannonAttack ),
m_bUseHide( rhs.m_bUseHide ),
m_fAlphaRate( rhs.m_fAlphaRate ),
m_bUseSkillProtect( rhs.m_bUseSkillProtect )
{
	ClearData();
}

ioCircleMoveCreateDummySkill2::~ioCircleMoveCreateDummySkill2(void)
{
}

ioSkill* ioCircleMoveCreateDummySkill2::Clone()
{
	return new ioCircleMoveCreateDummySkill2( *this );
}

void ioCircleMoveCreateDummySkill2::LoadProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];

	ioNormalSkill::LoadProperty( rkLoader );

	//마법진 설정
	m_fCircleMinRange = rkLoader.LoadFloat_e( "magic_circle_min_range", 0.0f );
	m_fCircleMaxRange = rkLoader.LoadFloat_e( "magic_circle_max_range", 0.0f );
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );
	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;
	rkLoader.LoadString_e( "ready_circle_drop_zone", "", szBuf, MAX_PATH );
	m_szReadyCircleDropZone= szBuf;
	//이펙트
	rkLoader.LoadString_e( "red_team_effect", "", szBuf, MAX_PATH );
	m_RedEffect = szBuf;
	rkLoader.LoadString_e( "blue_team_effect", "", szBuf, MAX_PATH );
	m_BlueEffect = szBuf;

	//더미
	rkLoader.LoadString_e( "dummy_name", "", szBuf, MAX_PATH );
	m_szDummyName = szBuf;
	m_vDummyOffset.x = rkLoader.LoadFloat_e( "dummy_offset_x", 0.0f );
	m_vDummyOffset.y = rkLoader.LoadFloat_e( "dummy_offset_y", 0.0f );
	m_vDummyOffset.z = rkLoader.LoadFloat_e( "dummy_offset_z", 0.0f );
	m_iStartDummyAniCode = rkLoader.LoadInt_e( "start_dummy_ani_code", 0 );
	m_iLoopDummyAniCode = rkLoader.LoadInt_e( "loop_dummy_ani_code", 0 );
	m_iActionDummyAniCode = rkLoader.LoadInt_e( "action_dummy_ani_code", 0 );

	m_dwLoopDuration = (DWORD)rkLoader.LoadInt_e( "loop_duration", 0 );
	m_bEnableDropZoneAttack = rkLoader.LoadBool_e( "enable_dropzone_attack", false );

	//Weapon
	m_fWeaponGravityRate = rkLoader.LoadFloat_e( "weapon_gravity_rate", FLOAT1 );
	m_fWeaponFloatPower = rkLoader.LoadFloat_e( "gathering_float_power", 0.0f );
	m_fWeaponFloatMinPower = rkLoader.LoadFloat_e( "gathering_float_min_power", 0.0f );
	m_fWeaponMinSpeed = rkLoader.LoadFloat_e( "gathering_min_speed", 0.0f );
	m_fRevisionRate = rkLoader.LoadFloat_e( "gathering_revision_rate", 0.0f );
	m_fUpLimitHeight = rkLoader.LoadFloat_e( "up_limit_height", 30.0f );
	m_fUpLimitRange = rkLoader.LoadFloat_e( "up_limit_range", 0.0f );

	m_iMaxAttackCount = rkLoader.LoadInt_e( "max_attack_count", 0 );

	LoadWeaponInfo( "dummy_attack", m_CannonAttack, rkLoader );

	m_bUseHide = rkLoader.LoadBool_e( "use_hide", false );
	m_fAlphaRate = rkLoader.LoadFloat_e( "alpha_rate", FLOAT1 );

	m_bUseSkillProtect = rkLoader.LoadBool_e( "use_skill_protect", false );
}

bool ioCircleMoveCreateDummySkill2::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioNormalSkill::OnSkillStart( pChar ) )
		return false;

	SetCameraBuff( pChar );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}
	//무기 변경
	if( m_bUseItemMesh && !m_bSetHandMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	ClearData();

	float fNeedGauge = GetNeedGauge(pChar->GetCharName());
	if ( fNeedGauge != 0.0f )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
	return true;
}

void ioCircleMoveCreateDummySkill2::OnSkillEnd( ioBaseChar *pChar )
{
	ioNormalSkill::OnSkillEnd( pChar );

	EndCameraBuff( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	// 스킬 중간 동작에 사용된 메쉬들 제거
	if( m_bSetHandMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = false;
	}

	pChar->SetMagicCircleTarget( "" );
	pChar->DestroyMagicCircle();

	ioPlayStage* pStage = pChar->GetCreator();
	if ( pStage )
	{
		ioMapEffect *pEff = pStage->FindMapEffect( m_dwCurCircleEffectID );
		if ( pEff )
			pEff->EndEffectForce();
		m_dwCurCircleEffectID = -1;
	}

	if ( m_bUseHide )
	{
		pChar->SetAlphaRateDirect( MAX_ALPHA_RATE );
		pChar->SetAttachEffectAlphaRateDirect( MAX_ALPHA_RATE, true );
		pChar->SetHideChar( false );
	}

	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pChar, m_dwDummyIndex );
	if ( pDummy )
		pDummy->SetDieState( true );

	ClearData();
}

void ioCircleMoveCreateDummySkill2::ClearData()
{
	m_SkillState = SS_NONE;
	m_dwLoopStartTime = 0;
	m_vCirclePos = ioMath::VEC3_ZERO;
	m_szCurReadyCircle.Clear();
	m_dwDummyIndex = -1;
	m_dwFireStartTime = 0;

	m_vWeaponDir = ioMath::VEC3_ZERO;
	m_fCurWeaponMoveSpeed = 0.0f;
	m_fCurWeaponFloatPower = 0.0f;
	m_iCurAttackCount = 0;

	m_dwCurCircleEffectID = -1;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
}

void ioCircleMoveCreateDummySkill2::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !CheckDummyLive( pChar, pStage ) )
	{
		SetEndState( pChar );
		return;
	}

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

				SetStartState( pChar, pStage );
			}
		}
		break;
	case SS_START:
		{
			if ( m_dwMotionEndTime < FRAMEGETTIME() )
			{
				SetLoopState( pChar );
				return;
			}
		}
		break;
	case SS_LOOP:
		{
			if ( m_dwLoopDuration && m_dwLoopStartTime + m_dwLoopDuration < dwCurTime )
			{
				SetEndState( pChar );
				return;
			}

			UpdateMagicCircle( pChar, pStage );
			CheckRotate( pChar, pStage );
			CheckInputKey( pChar, pStage );
		}
		break;
	case SS_ACTION:
		{
			if ( m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
			{
				if ( m_iMaxAttackCount <= m_iCurAttackCount )
				{
					SetEndState( pChar );
					return;
				}
				else
				{
					SetLoopState( pChar );
					return;
				}
			}

			if ( m_dwFireStartTime && m_dwFireStartTime < FRAMEGETTIME() )
			{
				m_dwFireStartTime = 0;
				CreateCannonWeapon( pChar );
			}
		}
		break;
	}
}

void ioCircleMoveCreateDummySkill2::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioFloatThrowWeapon *pFloatWeapon = ToFloatThrowWeapon(pWeapon);
	if( pFloatWeapon )
	{
		pFloatWeapon->SetMoveDir( m_vWeaponDir );
		pFloatWeapon->SetMoveSpeed( m_fCurWeaponMoveSpeed );
		pFloatWeapon->SetFloatPower( m_fCurWeaponFloatPower );
		pFloatWeapon->SetGravityRate( m_fWeaponGravityRate );
	}

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

void ioCircleMoveCreateDummySkill2::SetStartState( ioBaseChar* pChar, ioPlayStage *pStage )
{
	if ( !pStage || !pStage )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	m_SkillState = SS_START;

	m_dwMotionEndTime = 0;
	m_dwFireStartTime = 0;

	CreateDummy( pChar, pStage );

	CreateMagicCircle( pChar );

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	if ( m_iStartDummyAniCode != 0 )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pChar, m_dwDummyIndex );
		if ( !pDummy )
			return;

		pDummy->SetSkillDummyAniCode( m_iStartDummyAniCode );
		pDummy->SetSkillDummyActionAni( m_dwMotionEndTime, m_dwFireStartTime );
	}

	if ( m_bUseHide )
	{
		pChar->SetAlphaRateDirect( m_fAlphaRate );
		pChar->SetAttachEffectAlphaRateDirect( m_fAlphaRate, true );
		pChar->SetHideChar( true );
	}
}

void ioCircleMoveCreateDummySkill2::CreateDummy( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pStage || !pStage )
		return;

	int iIndex = g_DummyCharMgr.GetCurLocalIndex();
	D3DXVECTOR3 vDummyPos = pChar->GetWorldPosition() + pChar->GetWorldOrientation() * m_vDummyOffset;
	ioDummyChar *pDummy = pStage->CreateDummyChar( m_szDummyName, iIndex, pChar->GetCharName(), vDummyPos, 0.0f, 0 );
	if ( !pDummy )
		return;

	m_dwDummyIndex = pDummy->GetDummyCharIdx();
}

void ioCircleMoveCreateDummySkill2::SetLoopState( ioBaseChar* pChar )
{
	if ( !pChar )
		return;

	m_SkillState = SS_LOOP;	

	if ( m_dwLoopStartTime == 0 )
		m_dwLoopStartTime = FRAMEGETTIME();

	if ( m_iLoopDummyAniCode != 0 )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pChar, m_dwDummyIndex );
		if ( !pDummy )
			return;

		pDummy->SetSkillDummyAniCode( m_iLoopDummyAniCode );
		pDummy->SetSkillDummyLoopAni( m_dwFireStartTime );
	}
}

void ioCircleMoveCreateDummySkill2::CreateMagicCircle( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioPlayStage* pStage = pChar->GetCreator();
	if ( !pStage )
		return;

	if ( pChar->IsNeedProcess() )
	{
		if ( m_vCirclePos == ioMath::VEC3_ZERO )
		{
			D3DXVECTOR3 vPos = pChar->GetWorldPosition();
			D3DXVECTOR3 vOffSet = pChar->GetMoveDir() * m_fCircleOffSet;
			m_vCirclePos = vPos + vOffSet;
			m_vCirclePos.y = pStage->GetMapHeight( m_vCirclePos.x, m_vCirclePos.z );
			if( m_vCirclePos.y <= 0.0f )
				m_vCirclePos.y = pChar->GetBottomHeight();
		}

		D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
		ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_ReadyCircle, m_vCirclePos, vScale );
		if ( pMapEffect )
			m_dwCurCircleEffectID = pMapEffect->GetUniqueID();
	}
}

float ioCircleMoveCreateDummySkill2::GetGrowthCircleRange( ioBaseChar *pChar )
{
	float fValue = 0.0f;

	if ( !pChar )
		return fValue;

	ioPlayStage *pStage = pChar->GetCreator();
	if ( !pStage )
		return fValue;

	ioGrowthUpInfo* pGrowthInfo = GetGrowthUpInfoByType( GT_MAGIC_CIRCLE_RANGE, pChar->GetCharName() );
	if ( pGrowthInfo )
	{
		ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo( pGrowthInfo );
		if( pUpInfo )
			fValue = pUpInfo->GetValue(pStage);
	}

	return fValue;
}

void ioCircleMoveCreateDummySkill2::UpdateMagicCircle( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar )
		return;

	bool bChange = false;
	if ( pChar->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eDir = pChar->CheckCurDirKey();
		if ( eDir != m_CurDirKey )
		{
			m_CurDirKey = eDir;
			bChange = true;
		}
	}

	if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
	{
		float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );
		D3DXQUATERNION qtRotate;
		D3DXVECTOR3 vPos;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );

		vPos = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		vPos *= m_fCircleMoveSpeed * g_FrameTimer.GetSecPerFrame();
		m_vCirclePos += vPos;
	}

	D3DXVECTOR3 vCharPos = pChar->GetWorldPosition();
	D3DXVECTOR3 vCurXZ = m_vCirclePos;

	vCharPos.y = 0.0f;
	vCurXZ.y = 0.0f;

	D3DXVECTOR3 vDiff = vCurXZ - vCharPos;
	if( D3DXVec3Length( &vDiff ) > m_fCircleMaxRange )
	{
		D3DXVec3Normalize( &vDiff, &vDiff );
		vDiff *= m_fCircleMaxRange;

		float fCurHeight = m_vCirclePos.y;
		m_vCirclePos = vCharPos + vDiff;
		m_vCirclePos.y = fCurHeight;
	}
	else if( D3DXVec3Length( &vDiff ) < m_fCircleMinRange )
	{
		D3DXVec3Normalize( &vDiff, &vDiff );
		vDiff *= m_fCircleMinRange;

		float fCurHeight = m_vCirclePos.y;
		m_vCirclePos = vCharPos + vDiff;
		m_vCirclePos.y = fCurHeight;
	}

	float fMapHeight = pStage->GetMapHeight( m_vCirclePos.x, m_vCirclePos.z, pChar, false );
	if( fMapHeight > 0.0f )
		m_vCirclePos.y = fMapHeight;

	ioMapEffect *pEff = pStage->FindMapEffect( m_dwCurCircleEffectID );
	if( pEff )
	{
		pEff->SetWorldPosition( m_vCirclePos );
		pEff->SetWorldOrientation( pChar->GetTargetRot() );
	}

	if( bChange && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SST_DIR_CHANGE;
		kPacket << m_CurDirKey;
		kPacket << m_vCirclePos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCircleMoveCreateDummySkill2::CheckCircleByHeight( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( m_szReadyCircleDropZone.IsEmpty() )
		return;

	if( pStage->GetMapHeightNoneEntity( m_vCirclePos.x, m_vCirclePos.z, m_vCirclePos.y + 3.0f ) > 0.0f )
	{
		if( m_szCurReadyCircle != m_ReadyCircle )
		{
			m_szCurReadyCircle = m_ReadyCircle;

			ioMapEffect *pEff = pStage->FindMapEffect( m_dwCurCircleEffectID );
			if ( pEff )
				pEff->EndEffectForce();
			m_dwCurCircleEffectID = -1;
			
			D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
			ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_szCurReadyCircle, m_vCirclePos, vScale );
			if ( pMapEffect )
				m_dwCurCircleEffectID = pMapEffect->GetUniqueID();
		}
	}
	else
	{
		if( m_szCurReadyCircle != m_szReadyCircleDropZone )
		{
			m_szCurReadyCircle = m_szReadyCircleDropZone;
			ioMapEffect *pEff = pStage->FindMapEffect( m_dwCurCircleEffectID );
			if ( pEff )
				pEff->EndEffectForce();
			m_dwCurCircleEffectID = -1;

			D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
			ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_szCurReadyCircle, m_vCirclePos, vScale );
			if ( pMapEffect )
				m_dwCurCircleEffectID = pMapEffect->GetUniqueID();
		}
	}
}

void ioCircleMoveCreateDummySkill2::CheckRotate( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage )
		return;

	//마법진 방향으로 캐릭터 회전
	pChar->SetTargetRotToTargetPos( m_vCirclePos, true, false );

	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pChar, m_dwDummyIndex );
	if ( !pDummy )
		return;

	pDummy->SetWorldOrientation( pChar->GetTargetRot() );
}

void ioCircleMoveCreateDummySkill2::CheckInputKey( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( pChar->IsAttackKey() )
	{
		float fMapHeight = pStage->GetMapHeightOnlyTerrain( m_vCirclePos.x, m_vCirclePos.z );
		if ( fMapHeight <= 0.0f )
		{
			if ( m_bEnableDropZoneAttack )
				SetActionState( pChar, pStage );
		}
		else
			SetActionState( pChar, pStage );
	}
}

void ioCircleMoveCreateDummySkill2::SetActionState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if ( !pGrp )
		return;

	m_dwMotionEndTime = FRAMEGETTIME();
	m_dwFireStartTime = FRAMEGETTIME();
	if ( m_iActionDummyAniCode != 0 )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pChar, m_dwDummyIndex );
		if ( !pDummy )
			return;

		pDummy->SetSkillDummyAniCode( m_iActionDummyAniCode );
		pDummy->SetSkillDummyActionAni( m_dwMotionEndTime, m_dwFireStartTime );
	}
	CheckWeaponFloatInfo( pChar );

	m_iCurAttackCount++;

	m_SkillState = SS_ACTION;

	// 카메라 흔들림 현상을 위한 더미 애니 추가
	ioHashString szSkillMotion = GetSkillMotionName();
	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID != -1 )
	{
		float fAniRate = GetSkillMotionRate();
		if ( fAniRate <= 0.0f )
			fAniRate = 1.0f;
		float fTimeRate = fAniRate * m_fExtraAniRate;
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	}

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SST_ACTION;
		kPacket << m_vCirclePos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCircleMoveCreateDummySkill2::CheckWeaponFloatInfo( ioBaseChar *pChar )
{
	// WeaponMove
	D3DXVECTOR3 vCharPos = pChar->GetWorldPosition();
	D3DXVECTOR3 vDir = m_vCirclePos - vCharPos;
	vDir.y = 0.0f;

	float fRate = FLOAT1;
	float fRange = D3DXVec3Length( &vDir );
	D3DXVec3Normalize( &vDir, &vDir );

	// CharDir
	if( vDir == ioMath::VEC3_ZERO )
	{
		vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );
	}
	else
	{
		D3DXQUATERNION qtRot;
		ioMath::CalcDirectionQuaternion( qtRot, -vDir );
		pChar->SetTargetRotAndMoveDirChange( qtRot, false );
		pChar->SetWorldOrientation( qtRot );
	}

	float fCurWeaponFloatPower = 0.0f;
	float fCurWeaponMoveSpeed = 0.0f;
	if( m_fCircleMaxRange > 0.0f )
	{
		fRate = fRange / m_fCircleMaxRange;

		// CurFloat
		fCurWeaponFloatPower = m_fWeaponFloatPower * fRate;
		fCurWeaponFloatPower = max( fCurWeaponFloatPower, m_fWeaponFloatMinPower );

		float fCurGravity = 0.0f;
		float fWeaponMoveSpeed = 0.0f;
		ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
		if( pStage )
			fCurGravity = pStage->GetRoomGravity() * m_fWeaponGravityRate;

		float fFlightDuration = 0.0f;
		float fGapHeight = pChar->GetMidHeightByRate(m_fRevisionRate) - vCharPos.y;
		float fGapHeight2 = vCharPos.y - m_vCirclePos.y;
		if( fGapHeight2 > 0.0f )
		{
			fGapHeight += fGapHeight2;
			fFlightDuration = GetFlightDuration( fCurGravity, fCurWeaponFloatPower, fGapHeight );

			if( fFlightDuration > 0.0f )
				fWeaponMoveSpeed = fRange / (fFlightDuration / FLOAT1000);

			fCurWeaponMoveSpeed = max( m_fWeaponMinSpeed, fWeaponMoveSpeed );
		}
		else if( COMPARE(fGapHeight2, -m_fUpLimitHeight, -20.0f) && fRange > m_fUpLimitRange )
		{
			float fNewGap = fabs( fGapHeight2 ) - fGapHeight;
			fFlightDuration = GetTargetTime( fCurGravity, fNewGap, fCurWeaponFloatPower );
			fRange -= 35.0f;

			if( fFlightDuration > 0.0f )
				fCurWeaponMoveSpeed = fRange / (fFlightDuration / FLOAT1000);
		}
		else
		{
			fFlightDuration = GetFlightDuration( fCurGravity, fCurWeaponFloatPower, fGapHeight );

			if( fFlightDuration > 0.0f )
				fWeaponMoveSpeed = fRange / (fFlightDuration / FLOAT1000);

			fCurWeaponMoveSpeed = max( m_fWeaponMinSpeed, fWeaponMoveSpeed );
		}
	}

	m_vWeaponDir = vDir;
	m_fCurWeaponMoveSpeed = fCurWeaponMoveSpeed;
	m_fCurWeaponFloatPower = fCurWeaponFloatPower;
}

float ioCircleMoveCreateDummySkill2::GetFlightDuration( float fCurGravity, float fCurWeaponFloatPower, float fGapHeight )
{
	float fTopTime, fTopHeight, fGapTime;
	fTopTime = fTopHeight = fGapTime = 0.0f;

	float fNewGravity = fCurGravity;
	if( fNewGravity > 0.0f )
	{
		fTopTime = fCurWeaponFloatPower / fNewGravity;
		//fTopHeight = (fCurWeaponFloatPower*fTopTime) - (0.5f*fNewGravity*fTopTime*fTopTime);
	}

	float fCurFloatPower = -fCurWeaponFloatPower;
	float fCurGapHeight = 0.0f;
	while( fCurGapHeight < fGapHeight )
	{
		fCurGapHeight += fabs(fCurFloatPower * 0.01f);
		fCurFloatPower -= fNewGravity * 0.01;
		fGapTime += 0.01;
	}

	float fResult = 2 * fTopTime * FLOAT1000;
	fResult += fGapTime * FLOAT1000;
	return fResult;
}

float ioCircleMoveCreateDummySkill2::GetTargetTime( IN float fCurGravity, IN float fGapHeight, OUT float &fFloatPower )
{
	float fGapTime = 0.0f;
	float fCurFloatPower = 0;
	float fCurGapHeight = 0.0f;

	float fRevisionH = FLOAT100;
	float fTopHeight = fGapHeight + fRevisionH;

	// Top Check
	float fTopTime = 0.0f;
	do
	{
		fTopTime = fGapTime;

		fCurGapHeight += fabs(fCurFloatPower * 0.01f);
		fCurFloatPower += fCurGravity * 0.01;
		fGapTime += 0.01;
	} while( fCurGapHeight < fTopHeight );

	fFloatPower = fCurFloatPower;


	// Target Check
	fGapTime = 0.0f;
	fCurFloatPower = 0;
	fCurGapHeight = 0.0f;

	float fTargetTime = 0.18f;

	fTargetTime += fTopTime;
	fTargetTime *= FLOAT1000;
	return fTargetTime;
}

void ioCircleMoveCreateDummySkill2::CreateCannonWeapon( ioBaseChar *pChar )
{
	if ( !pChar )
		return;
	//무기 생성
	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
	pChar->SkillFireExplicit( m_CannonAttack, GetName(), pChar->IncreaseWeaponIndexBase(), vPos, vDir );
}

void ioCircleMoveCreateDummySkill2::SetEndState( ioBaseChar* pChar )
{
	m_SkillState = SS_END;
}

bool ioCircleMoveCreateDummySkill2::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pChar ) const
{
	if( m_SkillState == SS_END )
		return true;

	return false;
}

bool ioCircleMoveCreateDummySkill2::IsProtected( int iDefenseBreakType ) const
{
	if ( m_bUseSkillProtect )
		return true;
	else
		return ioNormalSkill::IsProtected( iDefenseBreakType );
}

bool ioCircleMoveCreateDummySkill2::CheckDummyLive( ioBaseChar* pChar, ioPlayStage* pStage )
{
	if ( !pChar || !pStage )
		return false;

	switch( m_SkillState )
	{
	case SS_NONE:
	case SS_END:
		return true;
	}

	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pChar, m_dwDummyIndex );
	if ( !pDummy || pDummy->GetState() == DCS_DIE )
		return false;

	return true;
}

void ioCircleMoveCreateDummySkill2::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case SST_DIR_CHANGE:
		{
			int iDir;
			rkPacket >> iDir;
			m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;
			rkPacket >> m_vCirclePos;
		}
		break;
	case SST_ACTION:
		{
			rkPacket >> m_vCirclePos;
			SetActionState( pChar, pStage );
		}
		break;
	}
}

