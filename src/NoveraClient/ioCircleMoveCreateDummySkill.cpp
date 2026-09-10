
#include "stdafx.h"

#include "ioCircleMoveCreateDummySkill.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioGrowthUpInfo.h"

#include "DummyCharDefine.h"

ioCircleMoveCreateDummySkill::ioCircleMoveCreateDummySkill()
{
	m_bSetHandMesh = false;
	m_bCreateDummy = false;
	m_dwFireStartTime = 0;
	m_dwCheckLoopTime = 0;
	m_dwCurSkillProtectTime = 0;
	m_SkillState = SS_NONE;
	m_dwFirstEffectID = -1;
	m_dwCircleMapEffectID = -1;
	m_fCircleMapEffectAngle = 0.0f;
	m_szCircleEffectName.Clear();
	D3DXQuaternionIdentity( &m_qtMapEffectRot);
	m_szCurReadyCircle.Clear();
}

ioCircleMoveCreateDummySkill::ioCircleMoveCreateDummySkill( const ioCircleMoveCreateDummySkill &rhs )
	: ioNormalSkill( rhs ),
	  m_fCircleMinRange( rhs.m_fCircleMinRange ),
	  m_fCircleMaxRange( rhs.m_fCircleMaxRange ),
	  m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
	  m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
	  m_fCircleOffSet( rhs.m_fCircleOffSet ),
	  m_ReadyCircle( rhs.m_ReadyCircle ),
	  m_szReadyCircleDropZone( rhs.m_szReadyCircleDropZone ),
	  m_RedEffect( rhs.m_RedEffect ),
	  m_BlueEffect( rhs.m_BlueEffect ),
	  m_vSecondWeaponEffectOffset( rhs.m_vSecondWeaponEffectOffset ),
	  m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	  m_szFirstDummyName( rhs.m_szFirstDummyName ),
	  m_szSecondDummyName( rhs.m_szSecondDummyName ),
	  m_dwLoopDuration( rhs.m_dwLoopDuration ),
	  m_szLoopAni( rhs.m_szLoopAni ),
	  m_szFirstWeaponEffect( rhs.m_szFirstWeaponEffect ),
	  m_vFirstWeaponEffectOffset( rhs.m_vFirstWeaponEffectOffset ),
	  m_fReduceSkillGauge( rhs.m_fReduceSkillGauge ),
	  m_OwnerBuffList( rhs.m_OwnerBuffList ),
	  m_szCircleEffectName( rhs.m_szCircleEffectName ),
	  m_fCircleMapEffectAngle( rhs.m_fCircleMapEffectAngle ),
	  m_bCreateDropZoneDummy( rhs.m_bCreateDropZoneDummy ),
	  m_bCheckUseOwnerBuff( rhs.m_bCheckUseOwnerBuff )
{	
	m_bSetHandMesh = false;
	m_bCreateDummy = false;
	m_dwFireStartTime = 0;
	m_dwCheckLoopTime = 0;
	m_dwCurSkillProtectTime = 0;
	m_SkillState = SS_NONE;
	m_dwFirstEffectID = -1;
	m_szCurReadyCircle.Clear();
}

ioCircleMoveCreateDummySkill::~ioCircleMoveCreateDummySkill()
{
}

void ioCircleMoveCreateDummySkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];
	//무적시간 설정
	m_dwSkillProtectTime    = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	//루프 설정
	rkLoader.LoadString_e( "loop_ani", "", szBuf, MAX_PATH );
	m_szLoopAni = szBuf;
	//루프 시간 설정
	m_dwLoopDuration = rkLoader.LoadInt_e( "loop_duration", 0 );

	//마법진 설정
	m_fCircleMinRange = rkLoader.LoadFloat_e( "magic_circle_min_range", 0.0f );
	m_fCircleMaxRange = rkLoader.LoadFloat_e( "magic_circle_max_range", 0.0f );
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );
	
	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;
	rkLoader.LoadString_e( "ready_circle_drop_zone", "", szBuf, MAX_PATH );
	m_szReadyCircleDropZone= szBuf;
	
	rkLoader.LoadString_e( "red_team_effect", "", szBuf, MAX_PATH );
	m_RedEffect = szBuf;
	rkLoader.LoadString_e( "blue_team_effect", "", szBuf, MAX_PATH );
	m_BlueEffect = szBuf;
	
	//무기 설정
	rkLoader.LoadString_e( "first_dummy", "", szBuf, MAX_PATH );
	m_szFirstDummyName = szBuf;

	rkLoader.LoadString_e( "second_dummy", "", szBuf, MAX_PATH );
	m_szSecondDummyName = szBuf;

	rkLoader.LoadString_e( "first_dummy_effect", "", szBuf, MAX_PATH );
	m_szFirstWeaponEffect = szBuf;
	
	m_vFirstWeaponEffectOffset.x = rkLoader.LoadFloat_e( "first_dummy_offset_x", 0.0f );
	m_vFirstWeaponEffectOffset.y = rkLoader.LoadFloat_e( "first_dummy_offset_y", 0.0f );
	m_vFirstWeaponEffectOffset.z = rkLoader.LoadFloat_e( "first_dummy_offset_z", 0.0f );

	m_vSecondWeaponEffectOffset.x = rkLoader.LoadFloat_e( "second_dummy_offset_x", 0.0f );
	m_vSecondWeaponEffectOffset.y = rkLoader.LoadFloat_e( "second_dummy_offset_y", 0.0f );
	m_vSecondWeaponEffectOffset.z = rkLoader.LoadFloat_e( "second_dummy_offset_z", 0.0f );

	m_fReduceSkillGauge = rkLoader.LoadFloat_e( "reduce_skill_gauge", 0.0f );

	m_OwnerBuffList.clear();
	ioHashString szBuffName;
	int nBuffCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	for( int i=0; i<nBuffCnt; ++i )
	{
		wsprintf_e( szKey, "owner_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		szBuffName = szBuf;
		m_OwnerBuffList.push_back( szBuffName );
	}

	rkLoader.LoadString_e( "circle_effect_name", "", szBuf, MAX_PATH );
	m_szCircleEffectName = szBuf;
	m_fCircleMapEffectAngle = rkLoader.LoadFloat_e( "circle_map_effect_angle", 0.0f );

	m_bCreateDropZoneDummy = rkLoader.LoadBool_e( "create_drop_zone_dummy", false );

	m_bCheckUseOwnerBuff = rkLoader.LoadBool_e( "check_use_owner_buff", false );
}

ioSkill* ioCircleMoveCreateDummySkill::Clone()
{
	return new ioCircleMoveCreateDummySkill( *this );
}

bool ioCircleMoveCreateDummySkill::OnSkillStart( ioBaseChar *pChar )
{	
	if ( !ioNormalSkill::OnSkillStart( pChar ) )
		return false;

	m_bSetHandMesh = false;
	m_bCreateDummy = false;
	m_dwFireStartTime = 0;
	m_dwCheckLoopTime = 0;
	m_dwCurSkillProtectTime = 0;
	m_SkillState = SS_NONE;
	m_dwFirstEffectID = -1;
	m_szCurReadyCircle.Clear();

	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = FRAMEGETTIME() + m_dwSkillProtectTime;

	SetCameraBuff( pChar );

	//기본 게이지 감소 (마법진 게이지)
	if ( m_fReduceSkillGauge != 0.0f )
	{
		pChar->ReduceNeedGaugeBySkill( this, m_fReduceSkillGauge );
		m_bReduceGauge = true;
	}
	
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

	m_dwCircleMapEffectID = -1;

	D3DXQuaternionIdentity( &m_qtMapEffectRot);

	return true;
}

void ioCircleMoveCreateDummySkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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

				//루프 상태로 전환
				SetLoopState( pChar );
				CreateFirstWeaponEffect( pChar, pStage );
				
				CreateMapEffect( pChar, pStage );
			}
		}
		break;
	case SS_LOOP:
		{
			//마법진 갱신
			UpdateMagicCircle( pChar, pStage );
			//마법진 방향으로 회전
			CheckRotate( pChar, pStage );
			//map effect가 magic서클을 따라가도록
			UpdateCircleMapEffect( pChar, pStage );
			//키입력
			CheckInputKey( pChar, pStage );

			//루프 종료
			if ( m_dwCheckLoopTime + m_dwLoopDuration < dwCurTime )
			{
				SetEndState( pChar, pStage );
			}
		}
		break;
	case SS_ACTION:
		{
			//무기 생성
			CreateDummies( pChar, pStage );

			//모션 종료
			if ( m_dwMotionEndTime < dwCurTime )
			{
				SetEndState( pChar, pStage );
			}
		}
		break;
	case SS_WAIT:
		break;
	}
}

void ioCircleMoveCreateDummySkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioNormalSkill::OnSkillEnd( pChar );

	EndCameraBuff( pChar );

	pChar->SetMagicCircleTarget( "" );
	pChar->DestroyMagicCircle();
	
	ioPlayStage *pPlayStage = pChar->GetCreator();
	if ( pPlayStage )
	{
		if ( m_dwFirstEffectID != -1 )
			pPlayStage->DestroyMapEffect( m_dwFirstEffectID );
	}

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


	if ( pPlayStage )
	{
		if ( m_dwCircleMapEffectID != -1 )
			pPlayStage->DestroyMapEffect( m_dwCircleMapEffectID );
	}
}

bool ioCircleMoveCreateDummySkill::IsProtected( int iDefenseBreakType ) const
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

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwCurSkillProtectTime > dwCurTime )
		return true;
	
	return false;
}

bool ioCircleMoveCreateDummySkill::IsEndState() const
{
	if( m_SkillState == SS_END )
		return true;

	return false;
}

bool ioCircleMoveCreateDummySkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_SkillState == SS_END )
		return true;

	return false;
}

bool ioCircleMoveCreateDummySkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioCircleMoveCreateDummySkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );
}

void ioCircleMoveCreateDummySkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int nState;
	rkPacket >> nState;

	switch( (SkillState)nState )
	{
	case SS_ACTION:
		{
			rkPacket >> m_vCirclePos;
			CreateSecondWeaponEffect( pChar, pStage );
			SetNormalSkill( pChar );
			
			m_SkillState = SS_WAIT;
		}
		break;
	case SS_CREATE_DUMMY:
		{
			rkPacket >> m_vFirstDummyPos;
			rkPacket >> m_vCirclePos;
			rkPacket >> m_qtMapEffectRot;
			int nCurIndex1,nCurIndex2;
			rkPacket >> nCurIndex1 >> nCurIndex2;
			CreateDummy( pChar, pStage, m_szFirstDummyName, m_vFirstDummyPos, nCurIndex1 );
			CreateDummy( pChar, pStage, m_szSecondDummyName, m_vCirclePos, nCurIndex2 );
		}
		break;
	case SS_END:
		{
			SetOwnerBuff( pChar );

			m_SkillState = SS_END;
			
			if ( m_dwFirstEffectID != -1 )
				pStage->DestroyMapEffect( m_dwFirstEffectID );

			if ( m_dwCircleMapEffectID != -1 )
				pStage->DestroyMapEffect( m_dwCircleMapEffectID );
		}
		break;
	}
}

void ioCircleMoveCreateDummySkill::SetNormalSkill( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	
		return;

	float fAniRate = GetSkillMotionRate();
	float fTimeRate = fAniRate * m_fExtraAniRate;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;
	m_dwMotionEndTime += m_dwAnimationEndDelay;

	m_dwFireStartTime = dwCurTime;
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "fire_s" ) * fTimeRate;

	//추가 게이지 감소
	ioItem *pItem = GetOwnerItem();
	if( pItem && !m_bReduceGauge )
	{
		float fUseGauge = pItem->GetMaxSkillGauge( pChar->GetCharName() );
		pChar->ReduceNeedGaugeBySkill( this, fUseGauge );
		m_bReduceGauge = true;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//성장 범위 얻기
float ioCircleMoveCreateDummySkill::GetGrowthCircleRange( ioBaseChar *pChar )
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

//마법진 생성
void ioCircleMoveCreateDummySkill::CreateMagicCircle( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	if( !pChar->CheckMagicCircle() )
	{
		ioHashString szDummyCircle;
		pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, szDummyCircle, m_fCircleOffSet );
	}
}

//마법진 업데이트
void ioCircleMoveCreateDummySkill::UpdateMagicCircle( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar )
		return;
		
	if( pChar->CheckMagicCircle() )
	{
		CheckCircleByHeight( pChar, pStage );
		float fCurMaxRange = m_fCircleMaxRange + GetGrowthCircleRange( pChar );

		//뱡향 체크
		pChar->CheckChangeDirectionForMagicCircle();
		//위치 갱신
		pChar->UpdateMagicCirclePosAll( m_fCircleMoveSpeed, fCurMaxRange, m_fCircleMinRange, m_fCircleHeightGap, false );
	}
}

//마법진 위치 얻기
void ioCircleMoveCreateDummySkill::GetMagicCirclePos( ioBaseChar *pChar, bool bVisible )
{
	if ( !pChar )
		return;

	if( pChar->CheckMagicCircle() )
	{
		pChar->CheckMagicCircleVisible( bVisible );
		m_vCirclePos = pChar->GetMagicCirclePos( m_fCircleOffSet, m_bCircleNoChangeY );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//루프 상태 전환
void ioCircleMoveCreateDummySkill::SetLoopState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_szLoopAni );
	if( iAniID == -1 )	
		return;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100 );	

	//마법진 생성
	CreateMagicCircle( pChar );
	
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwCheckLoopTime = dwCurTime;
	m_SkillState = SS_LOOP;	
}

//공격 상태로 전환
void ioCircleMoveCreateDummySkill::SetActionState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return;

	float fNeedGauge = GetNeedGauge(pChar->GetCharName());
	if ( fNeedGauge != 0.0f )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
	

	GetMagicCirclePos( pChar, false );
	CreateSecondWeaponEffect( pChar, pStage );

	SetNormalSkill( pChar );

	m_SkillState = SS_ACTION;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_ACTION;
		kPacket << m_vCirclePos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

//종료 상태 설정
void ioCircleMoveCreateDummySkill::SetEndState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return;

	SetOwnerBuff( pChar );

	m_SkillState = SS_END;

	if ( m_dwFirstEffectID != -1 )
		pStage->DestroyMapEffect( m_dwFirstEffectID );

	if ( m_dwCircleMapEffectID != -1 )
		pStage->DestroyMapEffect( m_dwCircleMapEffectID );

	//패킷 전송
	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//키입력 확인
void ioCircleMoveCreateDummySkill::CheckInputKey( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pChar->IsNeedProcess() || !pStage )
		return;	

	if( pChar->IsAttackKey() )
	{
		GetMagicCirclePos( pChar, true );

		float fMapHeight = pStage->GetMapHeightOnlyTerrain( m_vCirclePos.x, m_vCirclePos.z );
		if ( m_bCreateDropZoneDummy )
		{
			if ( fMapHeight <= 0.0f )
				SetActionState( pChar, pStage );
		}
		else
		{
			if ( fMapHeight > 0.0f )
				SetActionState( pChar, pStage );
		}
	}
	else if ( pChar->IsDefenseKey() )
	{
		if ( m_fCircleMapEffectAngle != 0.0f )
			RotateCircleMapEffect( pChar, pStage, m_fCircleMapEffectAngle );
	}
	else if ( pChar->IsJumpKey() )
	{
		if ( m_fCircleMapEffectAngle != 0.0f )
			RotateCircleMapEffect( pChar, pStage, -m_fCircleMapEffectAngle );
	}
	//
}

//마법진 방향으로 회전
void ioCircleMoveCreateDummySkill::CheckRotate( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage )
		return;

	//마법진 방향으로 캐릭터 회전
	D3DXVECTOR3 vTargetPos = pChar->GetMagicCirclePos();
	pChar->SetTargetRotToTargetPos( vTargetPos, true, false );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioCircleMoveCreateDummySkill::CreateFirstWeaponEffect( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pChar->IsNeedProcess() || !pStage  )
		return;
	
	m_vFirstDummyPos = pChar->GetWorldPosition() + ( pChar->GetWorldOrientation() * m_vFirstWeaponEffectOffset );
	float fMapHeight = pStage->GetMapHeightOnlyTerrain( m_vFirstDummyPos.x, m_vFirstDummyPos.z );
	if ( fMapHeight > 0.0f )
	{
		m_vFirstDummyPos.y = fMapHeight + m_vFirstWeaponEffectOffset.y;
		ioMapEffect *pEffect = pStage->CreateMapEffect( m_szFirstWeaponEffect, m_vFirstDummyPos, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1) );
		if ( pEffect )
			m_dwFirstEffectID = pEffect->GetUniqueID();
	}
	else
		SetEndState( pChar, pStage );
}

void ioCircleMoveCreateDummySkill::CreateSecondWeaponEffect( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage )
		return;

	D3DXVECTOR3 vPos = m_vCirclePos + m_vSecondWeaponEffectOffset;
	D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );

	if( pChar->GetTeam() == TEAM_RED )
		pStage->CreateMapEffect( m_RedEffect, vPos, vScale );
	else if( pChar->GetTeam() == TEAM_BLUE )
		pStage->CreateMapEffect( m_BlueEffect, vPos, vScale );
	else
	{
		if( pChar->GetSingleConvertTeam() == TEAM_BLUE )
			pStage->CreateMapEffect( m_BlueEffect, vPos, vScale );
		else
			pStage->CreateMapEffect( m_RedEffect, vPos, vScale );
	}
}

void ioCircleMoveCreateDummySkill::CreateDummies( ioBaseChar *pChar, ioPlayStage *pStage )
{	
	if ( !pChar || !pChar->IsNeedProcess() || m_bCreateDummy )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwFireStartTime > dwCurTime )
		return;
			
	int nCurIndex1 = g_DummyCharMgr.GetCurLocalIndex();
	CreateDummy( pChar, pStage, m_szFirstDummyName, m_vFirstDummyPos, nCurIndex1 );
	int nCurIndex2 = g_DummyCharMgr.GetCurLocalIndex();
	CreateDummy( pChar, pStage, m_szSecondDummyName, m_vCirclePos, nCurIndex2 );
	
	m_bCreateDummy = true;
	
	//패킷 전송
	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_CREATE_DUMMY;
		kPacket << m_vFirstDummyPos;
		kPacket << m_vCirclePos;
		kPacket << m_qtMapEffectRot;
		kPacket << nCurIndex1;
		kPacket << nCurIndex2;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCircleMoveCreateDummySkill::CreateDummy( ioBaseChar *pChar, ioPlayStage *pStage, ioHashString szDummyName, D3DXVECTOR3 vPos, int nIndex )
{
	if ( !pChar || !pStage || szDummyName.IsEmpty() )
		return;

	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyCharToName( pChar, szDummyName );
	if ( pDummy )
	{
		if ( pDummy->GetState() != DCS_DIE )
		{
			ioDummyWarpMine *pDummyWarpMine = ToDummyWarpMine( pDummy );
			if ( pDummyWarpMine )
				pDummyWarpMine->SetExplosionState();
			else
				pDummy->SetDieState( true );
		}
	}

	pDummy = pStage->CreateDummyChar( szDummyName, nIndex, pChar->GetCharName(), vPos, 0.0f, 0, true );
	if( pDummy )
		SetDummyByMapEffect( pChar, pStage, pDummy );
	
}

void ioCircleMoveCreateDummySkill::SetOwnerBuff( ioBaseChar *pChar )
{
	if ( !pChar )
		return;
	
	int nBuffCnt = (int)m_OwnerBuffList.size();

	for ( int i=0; i<nBuffCnt; ++i )
	{
		if ( !m_OwnerBuffList[i].IsEmpty() )
		{
			ioHashString szName = "";
			if ( GetOwnerItem() )
				szName = GetOwnerItem()->GetName();
			pChar->AddNewBuff( m_OwnerBuffList[i], pChar->GetCharName(), szName, NULL );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//magic circle을 따라다니는 map effect 생성
void ioCircleMoveCreateDummySkill::CreateMapEffect( ioBaseChar *pChar, ioPlayStage *pStage )
{
	//이 이펙트는 시전자만 보여야 하기 때문에
	if ( m_szCircleEffectName.IsEmpty() || !pChar || !pStage || !pChar->IsNeedProcess() )
		return;

	GetMagicCirclePos( pChar, true );
	D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );

	ioMapEffect *CircleMapEffect = pStage->CreateMapEffect( m_szCircleEffectName, m_vCirclePos, vScale );

	if( CircleMapEffect )
	{
		m_dwCircleMapEffectID = CircleMapEffect->GetUniqueID();
		CircleMapEffect->SetWorldOrientation( pChar->GetWorldOrientation() );
		m_qtMapEffectRot = CircleMapEffect->GetWorldOrientation();
	}
}

//magic circle을 따라다니는 map effect 업데이트
void ioCircleMoveCreateDummySkill::UpdateCircleMapEffect( ioBaseChar *pChar, ioPlayStage *pStage )
{
	//이 이펙트는 시전자만 보여야 하기 때문에
	if ( m_szCircleEffectName.IsEmpty() || !pChar ||  m_dwCircleMapEffectID == -1 || !pChar->IsNeedProcess() )
		return;

	GetMagicCirclePos( pChar, true );

	ioMapEffect *pEff = pStage->FindMapEffect( m_dwCircleMapEffectID );
	if( pEff )
	{
		pEff->SetWorldPosition( m_vCirclePos );
	}
}

//a,s키로 회전이 된 값을 세팅해준다.
void ioCircleMoveCreateDummySkill::SetDummyByMapEffect( ioBaseChar *pChar, ioPlayStage *pStage, ioDummyChar *pDummy )
{
	if ( !pChar || !pStage || !pDummy )
		return;
	if( m_szCircleEffectName.IsEmpty() )
		return;
	pDummy->SetWorldOrientation( m_qtMapEffectRot );
}

//a,s 키를 누를 때마다 해당 이펙트를 fAngle 만큼 회전시켜준다.
void ioCircleMoveCreateDummySkill::RotateCircleMapEffect( ioBaseChar *pChar, ioPlayStage *pStage, float fAngle )
{
	if ( !pChar || !pStage || m_dwCircleMapEffectID == -1 && !pChar->IsNeedProcess() )
		return;
	fAngle = ioMath::ArrangeHead( fAngle);
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll(&qtRotate, fAngle, 0.f, 0.f);
	ioMapEffect *pEff = pStage->FindMapEffect( m_dwCircleMapEffectID );
	if( pEff )
	{
		pEff->Rotate( ioMath::UNIT_Y, fAngle);
		m_qtMapEffectRot = pEff->GetWorldOrientation();
	}
}

bool ioCircleMoveCreateDummySkill::CheckUseBuff( ioBaseChar *pChar )
{
	if ( m_bCheckUseOwnerBuff )
	{
		if( pChar->HasBuff( m_OwnerSkillEndBuff ) )
			return true;
	}

	return false;
}

void ioCircleMoveCreateDummySkill::CheckCircleByHeight( ioBaseChar *pChar, ioPlayStage *pStage  )
{
	if ( m_szReadyCircleDropZone.IsEmpty() )
		return;

	D3DXVECTOR3 vPos = pChar->GetMagicCirclePos();

	if( pStage->GetMapHeightNoneEntity( vPos.x, vPos.z, vPos.y + 3.0f ) > 0.0f )
	{
		if( m_szCurReadyCircle != m_ReadyCircle )
		{
			m_szCurReadyCircle = m_ReadyCircle;
			ioHashString szDummyCircle;
			pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, szDummyCircle, m_fCircleOffSet );
			pChar->SetMagicCirclePos( false, vPos );
		}
	}
	else
	{
		if( m_szCurReadyCircle != m_szReadyCircleDropZone )
		{
			m_szCurReadyCircle = m_szReadyCircleDropZone;
			ioHashString szDummyCircle;
			pChar->CreateMagicCircle( m_szReadyCircleDropZone, m_szReadyCircleDropZone, szDummyCircle, m_fCircleOffSet );
			pChar->SetMagicCirclePos( false, vPos );
		}
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////