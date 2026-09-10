

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"


#include "ioTargetDummyCharSkill.h"

ioTargetDummyCharSkill::ioTargetDummyCharSkill()
{
	m_State = SS_NONE;

	m_bSetHandMesh = false;

	m_dwFireStartTime = 0;
	m_dwActionStartTime = 0;
}

ioTargetDummyCharSkill::ioTargetDummyCharSkill( const ioTargetDummyCharSkill &rhs )
: ioNormalSkill( rhs ),
 m_iMaxTarget( rhs.m_iMaxTarget ),
 m_TargetColType( rhs.m_TargetColType ),
 m_TargetWoundType( rhs.m_TargetWoundType ),
 m_fSkillRange( rhs.m_fSkillRange ),
 m_fSkillAngle( rhs.m_fSkillAngle ),
 m_fOffSet( rhs.m_fOffSet ),
 m_fHeightOffSet( rhs.m_fHeightOffSet ),
 m_fUpHeight( rhs.m_fUpHeight ),
 m_fUnderHeight( rhs.m_fUnderHeight ),
 m_fWidth( rhs.m_fWidth ),
 m_DummyCharName( rhs.m_DummyCharName ),
 m_fFrontOffSet( rhs.m_fFrontOffSet ),
 m_fSideOffSet( rhs.m_fSideOffSet ),
 m_WeaponInfo( rhs.m_WeaponInfo ),
 m_AirWeaponInfo( rhs.m_AirWeaponInfo ),
 m_SkillAttackType( rhs.m_SkillAttackType )
{
	m_State = SS_NONE;

	m_bSetHandMesh = false;
	
	m_dwFireStartTime = 0;
	m_dwActionStartTime = 0;
}

ioTargetDummyCharSkill::~ioTargetDummyCharSkill()
{
}

void ioTargetDummyCharSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	m_SkillAttackType = (SkillAttackType)rkLoader.LoadInt_e( "skill_attack_type", SAT_DUMMY );

	// dummy
	m_TargetColType = (TargetColType)rkLoader.LoadInt_e( "target_collision_type", TCT_SPHERE );
	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );

	m_iMaxTarget = rkLoader.LoadInt_e( "max_target_cnt", 0 );

	m_fSkillRange = rkLoader.LoadFloat_e( "target_range", 0.0f );
	m_fSkillAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fOffSet = rkLoader.LoadFloat_e( "target_offset", 0.0f );
	m_fHeightOffSet = rkLoader.LoadFloat_e( "target_height_offset", 0.0f );

	m_fUpHeight = rkLoader.LoadFloat_e( "target_up_height", 0.0f );
	m_fUpHeight = max( FLOAT1, m_fUpHeight );
	m_fUnderHeight = rkLoader.LoadFloat_e( "target_under_height", 0.0f );
	m_fUnderHeight = max( FLOAT1, m_fUnderHeight );
	m_fWidth = rkLoader.LoadFloat_e( "target_width", 0.0f );

	rkLoader.LoadString_e( "dummy_char_name", "", szBuf, MAX_PATH );
	m_DummyCharName = szBuf;

	m_fFrontOffSet = rkLoader.LoadFloat_e( "dummy_char_fr_offset", 0.0f );
	m_fSideOffSet = rkLoader.LoadFloat_e( "dummy_char_side_offset", 0.0f );

	//
	wsprintf_e( szKey, "weapon_attribute_index" );
	m_WeaponInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, 0 );
	wsprintf_e( szKey, "weapon_attribute_resist_index" );
	m_WeaponInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, 0 );

	wsprintf_e( szKey, "weapon_wounded_animation" );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	m_WeaponInfo.m_WoundedAnimation = szBuf;

	wsprintf_e( szKey, "weapon_wounded_duration" );
	m_WeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );
	wsprintf_e( szKey, "weapon_wounded_loop_ani" );
	m_WeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

	wsprintf_e( szKey, "air_weapon_attribute_index" );
	m_AirWeaponInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, 0 );
	wsprintf_e( szKey, "air_weapon_attribute_resist_index" );
	m_AirWeaponInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, 0 );

	wsprintf_e( szKey, "air_weapon_wounded_animation" );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	m_AirWeaponInfo.m_WoundedAnimation = szBuf;

	wsprintf_e( szKey, "air_weapon_wounded_duration" );
	m_AirWeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );
	wsprintf_e( szKey, "air_weapon_wounded_loop_ani" );
	m_AirWeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );
}

ioSkill* ioTargetDummyCharSkill::Clone()
{
	return new ioTargetDummyCharSkill( *this );
}

bool ioTargetDummyCharSkill::OnSkillStart( ioBaseChar *pChar )
{	
	if( !HasSkillMotion() )
		return false;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	pChar->AttachEffect( m_SkillEffectName );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	pChar->SetSkillProtection();

	m_State = SS_NONE;
	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		SetActionState( pChar );
	}

	SetCameraBuff( pChar );

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	SetSkillNameChatBubble( pChar );

	return true;
}

void ioTargetDummyCharSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioNormalSkill::OnSkillEnd( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	// 스킬 중간 동작에 사용된 메쉬들 제거
	m_bSetHandMesh = false;
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	EndCameraBuff( pChar );

	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;
	m_State = SS_NONE;
}

void ioTargetDummyCharSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				if( pChar && pChar->IsNeedProcess() )
				{
					m_dwPreDelayStartTime = 0;
					EndPreDelayEffect( pChar );
					SetActionState( pChar );
				}
			}
		}
		break;
	case SS_ACTION:
		CheckDummyChar( pChar, pStage );
		break;
	}
}

bool ioTargetDummyCharSkill::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}

void ioTargetDummyCharSkill::SetNormalSkill( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	return;

	float fAniRate = GetSkillMotionRate();

	float fTimeRate = fAniRate * m_fExtraAniRate;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	
	m_dwMotionStartTime = FRAMEGETTIME();
	DWORD dwDuration = pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;
	m_dwMotionEndTime = m_dwMotionStartTime + dwDuration;
	m_dwMotionEndTime += m_dwAnimationEndDelay;

	m_dwFireStartTime = m_dwFireMotionEndTime = m_dwActionStartTime = FRAMEGETTIME();
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "fire_s" ) * fTimeRate;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "firemotion_e" ) * fTimeRate;

	m_dwEnableReserveTime = FRAMEGETTIME();
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "key_reserve" ) * fTimeRate;

	if( m_dwFireStartTime >= m_dwFireMotionEndTime )
		m_dwFireMotionEndTime = m_dwMotionEndTime;

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
}

void ioTargetDummyCharSkill::SetActionState( ioBaseChar *pChar )
{
	m_State = SS_ACTION;

	SetNormalSkill( pChar );

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}

void ioTargetDummyCharSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iSyncState;
	rkPacket >> iSyncState;

	switch( iSyncState )
	{
	case SSS_DUMMY:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pChar->SetTargetRotToRotate( qtRot, true, false );

			int iWeaponCnt;
			rkPacket >> iWeaponCnt;
			for( int i=0; i < iWeaponCnt; ++i )
			{
				bool bAir;
				int iWeaponIndex;
				D3DXVECTOR3 vTargetPos;
				
				rkPacket >> bAir;
				rkPacket >> iWeaponIndex;
				rkPacket >> vTargetPos;

				if( bAir )
				{
					pChar->SkillFireExplicit( m_AirWeaponInfo,
											  GetName(),
											  iWeaponIndex,
											  vTargetPos );
				}
				else
				{
					pChar->SkillFireExplicit( m_WeaponInfo,
											  GetName(),
											  iWeaponIndex,
											  vTargetPos );
				}
			}

			int iCreateCnt;
			rkPacket >> iCreateCnt;

			for( int j=0; j < iCreateCnt; ++j )
			{
				int iCurIndex;
				ioHashString szDummyCharName;
				float fStartAngle;
				D3DXVECTOR3 vTargetPos;

				rkPacket >> szDummyCharName >> iCurIndex;
				rkPacket >> fStartAngle;
				rkPacket >> vTargetPos;

				ioDummyChar *pDummy = pStage->CreateDummyChar( szDummyCharName, iCurIndex,
															   pChar->GetCharName(),
															   vTargetPos,
															   fStartAngle,
															   0, false );

				D3DXVECTOR3 vDir = pChar->GetWorldPosition() - vTargetPos;
				vDir.y = 0.0f;
				D3DXVec3Normalize(&vDir, &vDir);

				D3DXQUATERNION qtTargetRot;
				ioMath::CalcDirectionQuaternion( qtTargetRot, -vDir );

				if( pDummy )
				{
					pDummy->SetControlSkillName( GetName() );
					pDummy->SetWorldOrientation( qtTargetRot );
				}
			}
		}
		break;
	}
}

bool ioTargetDummyCharSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioTargetDummyCharSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );
}

void ioTargetDummyCharSkill::CheckDummyChar( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !pStage ) return;
	if( !pChar->IsNeedProcess() ) return;

	if( m_SkillAttackType == SAT_DUMMY && m_DummyCharName.IsEmpty() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwFireStartTime <= 0 || m_dwFireStartTime > dwCurTime )
		return;

	m_dwFireStartTime = 0;

	BaseCharList vTargetList;
	TargetDistanceList vAutoTargetList;
	
	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
	D3DXVECTOR3 vSightDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vSightDir, &vSightDir );

	if( m_fOffSet != 0.0f )
		vPos += vSightDir * m_fOffSet;

	// 충돌타입에 따라 리스트 가져오기
	switch( m_TargetColType )
	{
	case TCT_SPHERE:
		pChar->GetTargetCharListBySphere( vPos, m_fSkillRange, m_fSkillAngle, vTargetList );
		break;
	case TCT_CYLINDER:
		pChar->GetTargetCharListByCylinder( vPos, m_fSkillRange, m_fSkillAngle, m_fUpHeight, m_fUnderHeight, vTargetList );
		break;
	case TCT_BOX:
		// 사각형의 center을 vPos로 설정하게 되므로 여기서 캐릭터앞쪽에 생성하고 높이를 보정한다.
		vPos += vSightDir * m_fSkillRange * FLOAT05; 
		D3DXVECTOR3 vOffset( 0.0f, ( ( m_fUpHeight + m_fUnderHeight ) * FLOAT05 ) - m_fUpHeight, 0.0f );
		vPos -= vOffset;
		pChar->GetTargetCharListByBox( vPos, m_fSkillRange, m_fWidth, m_fUpHeight, m_fUnderHeight, vTargetList );
		break;
	}

	if( pChar->GetExperienceMode() == EMS_EXPERIENCE )
		vTargetList.clear();

	BaseCharList::iterator iter;
	for( iter = vTargetList.begin(); iter != vTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);

		if( !pTarget )	continue;
				
		if( pChar->GetTeam() == pTarget->GetTeam() )
			continue;

		if( pTarget->IsSystemState() )
			continue;
			
		if( !pTarget->IsEnableTarget() )
			continue;

		if( !pTarget->CheckTargetWoundType(m_TargetWoundType) )
			continue;

		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - pChar->GetWorldPosition();
		float fDistanceSq = D3DXVec3LengthSq( &vDiff );

		TargetDistanceSq kTarget;
		kTarget.pChar = pTarget;
		kTarget.fDistanceSq = fDistanceSq;

		vAutoTargetList.push_back( kTarget );
	}

	if( vAutoTargetList.empty() )
		return;

	std::sort( vAutoTargetList.begin(), vAutoTargetList.end(), TargetDistanceSort() );

	int iCurCount = 0;
	int iWeaponCount = 0;
	int iLoopCnt = 0;

	vDummyCharCreateInfoList vCreateList;
	vCreateList.clear();

	TargetWeaponInfoList vWeaponList;
	vWeaponList.clear();

	TargetDistanceList::iterator iter_t;
	iter_t = vAutoTargetList.begin();

	while( iter_t != vAutoTargetList.end() )
	{
		ioBaseChar *pTarget = (*iter_t).pChar;
		if( !pTarget )
		{
			++iter_t;
			continue;
		}

		if( m_iMaxTarget > 0 && !COMPARE( iLoopCnt, 0, m_iMaxTarget ) )
			break;

		D3DXVECTOR3 vCreatePos = pTarget->GetWorldPosition();
		D3DXVECTOR3 vDir = pChar->GetWorldPosition() - vCreatePos;
		vDir.y = 0.0f;
		D3DXVec3Normalize(&vDir, &vDir);

		D3DXVECTOR3 vSideDir;
		D3DXVec3Cross( &vSideDir, &ioMath::UNIT_Y, &vDir );
		D3DXVec3Normalize( &vSideDir, &vSideDir );

		D3DXQUATERNION qtTargetRot;
		ioMath::CalcDirectionQuaternion( qtTargetRot, -vDir );

		D3DXVECTOR3 vTargetPos = vCreatePos + (m_fFrontOffSet * vDir);
		vTargetPos = vTargetPos + (m_fSideOffSet * vSideDir);
		vTargetPos.y += m_fHeightOffSet;

		// weapon
		if( m_SkillAttackType == SAT_WEAPON || m_SkillAttackType == SAT_ALL )
		{
			DWORD dwWeaponIndex = m_dwWeaponBaseIndex + iWeaponCount;
			bool bAir = pTarget->IsFloating();

			if( bAir )
			{
				pChar->SkillFireExplicit( m_AirWeaponInfo,
										  GetName(),
										  dwWeaponIndex,
										  vTargetPos );
			}
			else
			{
				pChar->SkillFireExplicit( m_WeaponInfo,
										  GetName(),
										  dwWeaponIndex,
										  vTargetPos );
			}

			TargetWeaponInfo kWeaponInfo;
			kWeaponInfo.m_bAir = bAir;
			kWeaponInfo.m_vTargetPos = vTargetPos;
			kWeaponInfo.m_iWeaponIndex = dwWeaponIndex;

			vWeaponList.push_back( kWeaponInfo );

			iWeaponCount++;
		}

		// dummy
		if( m_SkillAttackType == SAT_DUMMY || m_SkillAttackType == SAT_ALL )
		{
			int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();

			ioDummyChar *pDummy = pStage->CreateDummyChar( m_DummyCharName, iCurIndex,
														   pChar->GetCharName(),
														   vTargetPos,
														   0.0f,
														   0, true );

			if( pDummy )
			{
				iCurCount++;

				pDummy->SetControlSkillName( GetName() );
				pDummy->SetWorldOrientation( qtTargetRot );

				DummyCharCreateInfo kInfo;
				kInfo.m_DummyCharName = m_DummyCharName;
				kInfo.m_iDummyCharIdx = iCurIndex;
				kInfo.m_fDummyCharStartAngle = 0.0f;
				kInfo.m_vTargetPos = vTargetPos;

				vCreateList.push_back( kInfo );
			}
		}

		++iter_t;
		++iLoopCnt;
	}

	int iCreateCnt = vCreateList.size();
	int iWeaponCnt = vWeaponList.size();
	if( pChar->IsNeedSendNetwork() && (iCreateCnt > 0 || iWeaponCnt > 0) )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSS_DUMMY;
		kPacket << pChar->GetTargetRot();

		kPacket << iWeaponCnt;
		for( int i=0; i < iWeaponCnt; ++i )
		{
			kPacket << vWeaponList[i].m_bAir;
			kPacket << vWeaponList[i].m_iWeaponIndex;
			kPacket << vWeaponList[i].m_vTargetPos;
		}

		kPacket << iCreateCnt;

		for( int j=0; j < iCreateCnt; ++j )
		{
			kPacket << vCreateList[j].m_DummyCharName;
			kPacket << vCreateList[j].m_iDummyCharIdx;
			kPacket << vCreateList[j].m_fDummyCharStartAngle;
			kPacket << vCreateList[j].m_vTargetPos;
		}
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


