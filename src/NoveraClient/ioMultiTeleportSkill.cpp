

#include "stdafx.h"

#include "ioNpcChar.h"
#include "ioPlayStage.h"

#include "ioMultiTeleportSkill.h"

#include "WeaponDefine.h"

ioMultiTeleportSkill::ioMultiTeleportSkill()
{
	ClearData();
}

ioMultiTeleportSkill::ioMultiTeleportSkill( const ioMultiTeleportSkill &rhs )
: ioAttackSkill( rhs ),
m_EndAnimation( rhs.m_EndAnimation ),
m_fEndAniRate( rhs.m_fEndAniRate ),
m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
m_dwCircleDuration( rhs.m_dwCircleDuration ),
m_fCircleRange( rhs.m_fCircleRange ),
m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
m_fCircleOffSet( rhs.m_fCircleOffSet ),
m_ReadyCircle( rhs.m_ReadyCircle ),
m_RedEffect( rhs.m_RedEffect ),
m_BlueEffect( rhs.m_BlueEffect ),
m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
m_fSkillLoopMotionRate( rhs.m_fSkillLoopMotionRate ),
m_dwSkillLoopLoopDuration( rhs.m_dwSkillLoopLoopDuration ),
m_dwFastValue( rhs.m_dwFastValue ),
m_dwGrapplingMaxValue( rhs.m_dwGrapplingMaxValue ),
m_dwEnableGrapplingCancelTime( rhs.m_dwEnableGrapplingCancelTime ),
m_dwEnableGrapplingCancelEndTime( rhs.m_dwEnableGrapplingCancelEndTime ),
m_szTargetCancelMotion( rhs.m_szTargetCancelMotion ),
m_iMaxTarget( rhs.m_iMaxTarget ),
m_vTargetCancelForce( rhs.m_vTargetCancelForce ),
m_fTargetAngle( rhs.m_fTargetAngle ),
m_fTargetRange( rhs.m_fTargetRange ),
m_TargetWoundType( rhs.m_TargetWoundType ),
m_bDisableColGrap( rhs.m_bDisableColGrap ),
m_GuideEffect( rhs.m_GuideEffect ),
m_iMaxEffectCount( rhs.m_iMaxEffectCount ),
m_iEnablePositionCnt( rhs.m_iEnablePositionCnt ),
m_fEnablePositionRange( rhs.m_fEnablePositionRange ),
m_fEnableMaxPositionRange( rhs.m_fEnableMaxPositionRange ),
m_bCheckDropZone( rhs.m_bCheckDropZone ),
m_fHightOffset( rhs.m_fHightOffset ),
m_fTargetHightOffset( rhs.m_fTargetHightOffset ),
m_TeleportStartEffect( rhs.m_TeleportStartEffect ),
m_TargetBuffList( rhs.m_TargetBuffList )
{
	ClearData();
}

ioMultiTeleportSkill::~ioMultiTeleportSkill()
{
}

void ioMultiTeleportSkill::ClearData()
{
	m_SkillState			= SS_NONE;
	m_GrapplingTargetState	= GTS_NONE;

	m_dwFireStartTime		= 0;
	m_dwMotionStartTime		= 0;
	m_dwControlStartTime	= 0;

	m_bPreLeft		= false;
	
	m_TargetName.Clear();
	m_SearchTargetNameList.clear();
	m_GrapplingTargetInfoList.clear();

	m_fMaxEffectRange = 0.0f;

	m_vCirclePos = ioMath::VEC3_ZERO;

	m_dwSkillMapEffect = -1;
	m_dwTeleportStartEffectID = -1;
	m_dwTeleportStartEffectIDList.clear();
}

void ioMultiTeleportSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	rkLoader.LoadString_e( "end_animation", "", szBuf, MAX_PATH );
	m_EndAnimation = szBuf;
	m_fEndAniRate = rkLoader.LoadFloat_e( "end_ani_rate", FLOAT1 );

	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	m_dwCircleDuration = (DWORD)rkLoader.LoadInt_e( "magic_circle_duration", 0 );
	m_fCircleRange = rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;

	rkLoader.LoadString_e( "red_team_circle", "", szBuf, MAX_PATH );
	m_RedEffect = szBuf;
	rkLoader.LoadString_e( "blue_team_circle", "", szBuf, MAX_PATH );
	m_BlueEffect = szBuf;

	// 스킬 루프 모션
	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_SkillLoopMotion = szBuf;
	m_fSkillLoopMotionRate = rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );
	m_dwSkillLoopLoopDuration = (DWORD)rkLoader.LoadInt_e( "skill_loop_duration", 0 );

	// 잡기캔슬 모션
	m_dwFastValue = (DWORD)rkLoader.LoadInt_e( "cancel_fast_value", 0 );
	m_dwGrapplingMaxValue = (DWORD)rkLoader.LoadInt_e( "cancel_max_value", 0 );
	m_dwEnableGrapplingCancelTime = (DWORD)rkLoader.LoadInt_e( "enable_cancel_time", 0 );
	m_dwEnableGrapplingCancelEndTime = (DWORD)rkLoader.LoadInt_e( "enable_cancel_end_time", 0 );

	rkLoader.LoadString_e( "target_grappling_cancel_motion", "", szBuf, MAX_PATH );
	m_szTargetCancelMotion = szBuf;

	_ENCSTR("target_grappling_cancel", szEncTargetGrapplingCancel);
	LoadForceInfo( szEncTargetGrapplingCancel, m_vTargetCancelForce, rkLoader);

	m_iMaxTarget = rkLoader.LoadInt_e( "max_target_count", 0 );

	// 타겟검색
	m_fTargetAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fTargetRange = rkLoader.LoadFloat_e( "target_range", 0.0f );
	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );
	m_bDisableColGrap = rkLoader.LoadBool_e( "zone_collision_disable_grap", false );

	// 가이드 이펙트
	rkLoader.LoadString_e( "guide_effect", "", szBuf, MAX_PATH );
	m_GuideEffect = szBuf;
	m_iMaxEffectCount = rkLoader.LoadInt_e( "guide_max_effect", 1 );

	// 타겟 워프
	m_iEnablePositionCnt = rkLoader.LoadInt_e( "enable_warp_position_cnt", 0 );
	m_fEnablePositionRange = rkLoader.LoadFloat_e( "enable_warp_position_range", 0.0f );
	m_fEnableMaxPositionRange = rkLoader.LoadFloat_e( "enable_warp_position_range_max", 0.0f );
	m_bCheckDropZone = rkLoader.LoadBool_e( "check_drop_zone", false );

	m_fHightOffset = rkLoader.LoadFloat_e( "warp_height_offset", 0.0f );
	m_fTargetHightOffset = rkLoader.LoadFloat_e( "warp_target_height_offset", 0.0f );

	rkLoader.LoadString_e( "teleport_start_effect", "", szBuf, MAX_PATH );
	m_TeleportStartEffect = szBuf;

	int iBuffCnt = rkLoader.LoadInt_e( "enemy_buff_cnt", 0 );
	m_TargetBuffList.clear();
	m_TargetBuffList.reserve( iBuffCnt );
	for( int i=0; i<iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "enemy_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_TargetBuffList.push_back( szBuf );
	}
}

void ioMultiTeleportSkill::LoadForceInfo( const char *szForce, ForceInfoList &rkForceInfoList, ioINILoader &rkLoader )
{
	rkForceInfoList.clear();

	char szKey[MAX_PATH];

	wsprintf_e( szKey, "%s_force_cnt", szForce );
	int iForceCnt = rkLoader.LoadInt( szKey, 0 );
	if( iForceCnt > 0 )
		rkForceInfoList.reserve( iForceCnt );

	for( int i=0; i < iForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szKey, "%s_force_move%d", szForce, i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "%s_force_friction%d", szForce, i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "%s_force_angle%d", szForce, i+1 );
		kInfo.m_fForceAngle = rkLoader.LoadFloat( szKey, 0.0f );

		rkForceInfoList.push_back( kInfo );
	}
}

ioSkill* ioMultiTeleportSkill::Clone()
{
	return new ioMultiTeleportSkill( *this );
}

bool ioMultiTeleportSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	ClearData();

	m_dwSkillStartTime = FRAMEGETTIME();

	m_bSetHandMesh = false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	SetCameraBuff( pChar );

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	FindTarget( pChar );
	DestroyGuideEffect();

	return true;
}

void ioMultiTeleportSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	int iSlot = GetOwnerItem()->GetType() - 1;

	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;

				EndPreDelayEffect( pChar );
				AttackSkillFire( pChar );
			}
		}
		break;
	case SS_ACTION:
		{
			if( m_dwFireStartTime > 0 && m_dwFireStartTime < dwCurTime )
			{
				int iCnt = m_SearchTargetNameList.size();
				for( int i=0; i<iCnt; ++i )
				{
					m_TargetName = m_SearchTargetNameList[i];
					D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
					pChar->SkillFireExplicit( GetAttribute().m_vWeaponInfoList.front(),
											  GetName(),
											  m_dwWeaponBaseIndex+i,
											  vPos );
				}
				if( iCnt == 0 )
				{
					m_TargetName.Clear();
					D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
					pChar->SkillFireExplicit( GetAttribute().m_vWeaponInfoList.front(),
											  GetName(),
											  m_dwWeaponBaseIndex+i,
											  vPos );
				}
				m_dwFireStartTime = 0;
			}
			
			if( m_dwMotionEndTime > 0 && m_dwMotionEndTime <= dwCurTime )
				SetSkillEndState( pChar );
		}
		break;
	case SS_LOOP:
		{
			//CheckTargetState( pChar );

			if( m_dwMotionEndTime < dwCurTime )
			{
				SetSkillEndState( pChar );
				return;
			}

			if( !m_GrapplingTargetInfoList.empty() )
			{
				SetSkillControlState( pChar );
				return;
			}
			else
			{
				SetSkillEndState( pChar );
				return;
			}
		}
		break;
	case SS_CONTROL:
		{
			ProcessWoundGrapplingState( pChar );
			CheckTargetState( pChar );
			ProcessGuideEffect( pChar );

			if( m_dwCircleDuration > 0 && m_dwControlStartTime + m_dwCircleDuration <= dwCurTime )
				SetSkillEndState( pChar );

			// 뱡향 변화 체크
			pChar->CheckChangeDirectionForMagicCircle();

			// MagicCircle 위치 갱신
			pChar->UpdateMagicCirclePos( m_fCircleMoveSpeed, m_fCircleRange, m_fCircleHeightGap, false );

			// MagicCircle 위치방향으로 캐릭터 회전
			D3DXVECTOR3 vTargetPos = pChar->GetMagicCirclePos();
			pChar->SetTargetRotToTargetPos( vTargetPos, true, false );

			if( pChar->IsNeedProcess() )
			{
				if( pChar->IsAttackKey() ||
					pChar->IsDefenseKey() ||
					pChar->IsJumpKey() ||
					pChar->GetSkillKeyInput() == iSlot )
				{
					m_vCirclePos = pChar->GetMagicCirclePos( m_fCircleOffSet, m_bCircleNoChangeY );

					if( pStage->GetMapHeightNoneEntity( m_vCirclePos.x, m_vCirclePos.z, m_vCirclePos.y+3.0f ) > 0.0f )
					{
						if( pChar->CheckMagicCircle() )
						{
							pChar->CheckMagicCircleVisible( false );
							pChar->UpdateMagicCirclePos( m_fCircleMoveSpeed, m_fCircleRange, m_fCircleHeightGap, false );
						}

						SetSkillTeleportState( pChar );
					}
				}
			}
		}
		break;
	case SS_TELEPORT:
		SetSkillEndState( pChar );
		break;
	case SS_END:
		break;
	}
}

void ioMultiTeleportSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

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

	//Destroy Magic Circle
	pChar->SetMagicCircleTarget( "" );
	pChar->DestroyMagicCircle();

	ioPlayStage *pStage = pChar->GetCreator();
	if( pStage )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwSkillMapEffect );
		if( pMapEffect )
		{
			pMapEffect->EndEffectGrace();
			m_dwSkillMapEffect = -1;
		}

		pMapEffect = pStage->FindMapEffect( m_dwTeleportStartEffectID );
		if( pMapEffect )
		{
			pMapEffect->EndEffectGrace();
			m_dwTeleportStartEffectID = -1;
		}

		int iCnt = m_dwTeleportStartEffectIDList.size();
		for( int i=0; i<iCnt; ++i )
		{
			pMapEffect = pStage->FindMapEffect( m_dwTeleportStartEffectIDList[i] );
			if( pMapEffect )
			{
				pMapEffect->EndEffectGrace();
			}
		}
		m_dwTeleportStartEffectIDList.clear();
	}
	DestroyGuideEffect();
	ClearData();
}

void ioMultiTeleportSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar )	return;
	if( !pStage )	return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case SSC_GRAPPLING:
		{
			SetSkillLoopState( pChar );

			D3DXQUATERNION qtCharRot;
			rkPacket >> qtCharRot;
			pChar->SetTargetRotToRotate( qtCharRot, true, false );
			
			ioHashString szTargetName;
			rkPacket >> szTargetName;
			bool bAlreadyGrappling = false;
			int iCnt = m_GrapplingTargetInfoList.size();
			for( int i=0; i<iCnt; ++i )
			{
				if( m_GrapplingTargetInfoList[i].m_TargetName == szTargetName )
					bAlreadyGrappling = true;
			}

			if( bAlreadyGrappling )
				return;

			ioBaseChar *pTarget = pChar->GetBaseChar( szTargetName );
			if( !pTarget ) return;

			GrapplingTargetInfo kInfo;
			kInfo.m_TargetName = szTargetName;
			kInfo.m_vPos = pTarget->GetWorldPosition();
			kInfo.m_dwCancelValue = 0;
			kInfo.m_dwEnableCancelTime = FRAMEGETTIME() + m_dwEnableGrapplingCancelTime;
			kInfo.m_dwEnableCancelEndTime = FRAMEGETTIME() + m_dwEnableGrapplingCancelEndTime;

			D3DXVECTOR3 vTargetDir = pTarget->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			float fDotValue = D3DXVec3Dot( &vTargetDir, &vDir );
			float fAngle = RADtoDEG(acosf(fDotValue));

			if( fAngle >=FLOAT90)
			{
				kInfo.m_GrapplingTargetState = GTS_STAND_FRONT;
			}
			else
			{
				kInfo.m_GrapplingTargetState = GTS_STAND_BACK;
			}

			m_GrapplingTargetInfoList.push_back( kInfo );

			D3DXVECTOR3 vDiff = pChar->GetWorldPosition() - pTarget->GetWorldPosition();
			m_fMaxEffectRange = D3DXVec3Length( &vDiff );

			if( pTarget && pTarget->IsNeedProcess() )
			{
				pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
			}
		}
		break;
	case SSC_TELEPORT:
		rkPacket >> m_vCirclePos;
		SetSkillTeleportState( pChar );
		break;
	case SSC_ESCAPE_TARGET:
		{
			ioHashString szTargetName;
			rkPacket >> szTargetName;
			ioBaseChar *pTarget = pStage->GetBaseChar( szTargetName );
			if( pTarget )
				SetEscapeTarget( pChar, pTarget );
		}
		break;
	}
}

void ioMultiTeleportSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	//
	ioGuideWeapon *pGuideWeapon = ToGuideWeapon( pWeapon );
	if( pGuideWeapon )
	{
		pGuideWeapon->SetTraceTarget( m_TargetName );
		pGuideWeapon->SetGrapplingTargetName( m_TargetName );
	}
}

void ioMultiTeleportSkill::FindTarget( ioBaseChar *pOwner )
{
	if( !pOwner )	return;
	//if( !pOwner->IsNeedProcess() )	return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )	return;

	// target
	TargetDistInfoList vTargetInfoList;
	vTargetInfoList.clear();

	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	float fPreDistSq = m_fTargetRange * m_fTargetRange;
	const BaseCharList &rkCharList = pStage->GetBaseCharList();


	BaseCharList::const_iterator iter;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pCurChar = *iter;
		if( !pCurChar )	continue;

		if( pCurChar == pOwner )
			continue;

		if( pCurChar->GetTeam() == pOwner->GetTeam() )
			continue;

		if( !IsEnableTargetState( pCurChar ) )
			continue;

		D3DXVECTOR3 vDiff = pCurChar->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		float fLengthSq = D3DXVec3LengthSq( &vDiff );
		float fLength = D3DXVec3Length( &vDiff ); // 테스트코드

		if( fLengthSq <= fPreDistSq )
		{
			D3DXVec3Normalize( &vDiff, &vDiff );
			float fValue = D3DXVec3Dot( &vDir, &vDiff );
			float fAngleGap = RADtoDEG( acosf(fValue) );
			if( fAngleGap < m_fTargetAngle * FLOAT05 )
			{
				TargetDistInfo TargetInfo;
				TargetInfo.m_TargetName = pCurChar->GetCharName();
				TargetInfo.m_fLenth = fLength;
				vTargetInfoList.push_back( TargetInfo );
			}
		}
	}

	if( !vTargetInfoList.empty() )
	{
		std::sort( vTargetInfoList.begin(), vTargetInfoList.end(), DistSort() );
		int iCnt = vTargetInfoList.size();
		iCnt = min( iCnt, m_iMaxTarget );

		for( int i=0; i<iCnt; ++i )
		{
			ioHashString	szTargetName;
			szTargetName = vTargetInfoList[i].m_TargetName;
			m_SearchTargetNameList.push_back( szTargetName );
		}
	}
}

bool ioMultiTeleportSkill::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;
			
	if( !pTarget->IsEnableTarget() )
		return false;

	if( m_TargetWoundType == TWT_AIR_WOUND_EXCEPT ) // 쓰러짐중 공중상태는 패스
	{
		if( pTarget->IsApplyDownState() && !pTarget->IsApplyDownState(false) )
			return false;
	}
	else if( m_TargetWoundType == TWT_DOWN_WOUND_EXCEPT ) // 쓰러짐중 바닥상태는 패스
	{
		if( pTarget->IsApplyDownState(false) )
			return false;
	}
	else if( m_TargetWoundType == TWT_ALL_WOUND_EXCEPT )
	{
		if( m_bDisableColGrap && pTarget->IsApplyDownState() ||
			( pTarget->GetState() == CS_GRAPPLING_WOUNDED ||
			pTarget->GetState() == CS_GRAPPLING_PUSHED ||
			pTarget->GetState() == CS_GRAPPLING_WOUNDED_SKILL ||
			pTarget->GetState() == CS_GRAPPLING_WOUNDED_SWING ) )
		{
			return false;
		}
		else if( pTarget->IsApplyDownState() )
		{
			return false;
		}

	}
	else if( m_TargetWoundType == TWT_FLOATING_STATE )	// 공중에 뜬 상태가 아니면 패스
	{
		if( !pTarget->IsFloating() )
			return false;
	}
	else if( m_TargetWoundType == TWT_DOWN_WOUNDED ) //바닥에 쓰러져 있지 않거나 공중에 뜬 상태면 패스
	{
		if( !pTarget->IsApplyDownState() || pTarget->IsFloating() )
			return false;
	}

	return true;
}

bool ioMultiTeleportSkill::SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName )
{
	if( !pChar ) return false;
	if( szTargetName.IsEmpty() ) return false;

	SetSkillLoopState( pChar );

	bool bAlreadyGrappling = false;
	int iCnt = m_GrapplingTargetInfoList.size();
	for( int i=0; i<iCnt; ++i )
	{
		if( m_GrapplingTargetInfoList[i].m_TargetName == szTargetName )
			bAlreadyGrappling = true;
	}
	
	if( bAlreadyGrappling )
		return false;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_GRAPPLING;
		kPacket << pChar->GetTargetRot();
		kPacket << szTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	ioBaseChar *pTarget = pChar->GetBaseChar( szTargetName );
	if( !pTarget )	return false;

	GrapplingTargetInfo kInfo;
	kInfo.m_TargetName = szTargetName;
	kInfo.m_vPos = pTarget->GetWorldPosition();
	kInfo.m_dwCancelValue = 0;
	kInfo.m_dwEnableCancelTime = FRAMEGETTIME() + m_dwEnableGrapplingCancelTime;
	kInfo.m_dwEnableCancelEndTime = FRAMEGETTIME() + m_dwEnableGrapplingCancelEndTime;

	D3DXVECTOR3 vTargetDir = pTarget->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	float fDotValue = D3DXVec3Dot( &vTargetDir, &vDir );
	float fAngle = RADtoDEG(acosf(fDotValue));

	if( fAngle >=FLOAT90)
	{
		kInfo.m_GrapplingTargetState = GTS_STAND_FRONT;
	}
	else
	{
		kInfo.m_GrapplingTargetState = GTS_STAND_BACK;
	}

	m_GrapplingTargetInfoList.push_back( kInfo );
	
	D3DXVECTOR3 vDiff = pChar->GetWorldPosition() - pTarget->GetWorldPosition();
	m_fMaxEffectRange = D3DXVec3Length( &vDiff );

	// for NPC
	bool bNPC = false;
	if( ToNpcChar( pTarget ) )
	{
		bNPC = true;
	}

	// local or 훈련소 or NPC
	if( !P2PNetwork::IsNetworkPlaying() || !pChar->GetCreator()->IsNetworkMode() || bNPC )
	{
		pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
	}

	return true;
}

D3DXVECTOR3 ioMultiTeleportSkill::GetGrapplingTargetPos( ioBaseChar *pAttacker, ioBaseChar *pTarget )
{
	if( !pAttacker )	return ioMath::VEC3_ZERO;
	if( !pTarget )		return ioMath::VEC3_ZERO;

	ioHashString szTargetName;
	szTargetName.Clear();
	D3DXVECTOR3 vTargetPos = ioMath::VEC3_ZERO;

	int iCnt = m_GrapplingTargetInfoList.size();
	for( int i=0; i<iCnt; ++i )
	{
		if( m_GrapplingTargetInfoList[i].m_TargetName == pTarget->GetCharName() )
		{
			vTargetPos = m_GrapplingTargetInfoList[i].m_vPos;
		}
	}

	return vTargetPos;
}

bool ioMultiTeleportSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_NONE:
	case SS_ACTION:
	case SS_LOOP:
	case SS_CONTROL:
	case SS_TELEPORT:
		break;
	case SS_END:
		if( m_dwMotionEndTime < dwCurTime )
			return true;
		break;
	}

	return false;
}

bool ioMultiTeleportSkill::IsEndState() const
{
	if( m_SkillState == SS_END )
		return true;
	return false;
}

bool ioMultiTeleportSkill::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{
		if( iDefenseBreakType == m_ProtectCancelType )   // 스킬 무적 파괴
			return false;
	}

	DWORD dwCurTime = FRAMEGETTIME();

	if( IsPreDelayProtect() )
	{
		if( m_dwPreDelayStartTime != 0 )
			return true;
	}

	if( m_dwPreDelayStartTime == 0 )
	{
		if( m_dwFireMotionEndTime > FRAMEGETTIME() )
			return true;

		if( m_dwSkillProtectTime > 0 && m_dwSkillStartTime + m_dwSkillProtectTime > FRAMEGETTIME() )
			return true;
	}

	

	return false;
}

bool ioMultiTeleportSkill::IsEnableReserve() const
{
	if( m_dwPreDelayStartTime != 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_END:
		if( m_dwEnableReserveTime == 0 || m_dwMotionStartTime >= m_dwEnableReserveTime )
			return false;

		if( m_dwEnableReserveTime < FRAMEGETTIME() )
			return true;
		break;
	}

	return false;
}

bool ioMultiTeleportSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioMultiTeleportSkill::AttackSkillFire( ioBaseChar *pChar )
{
	if( !pChar ) return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )	return;

	m_SkillState = SS_ACTION;

	float fTimeRate  = GetAttackTimeRate();
	DWORD dwPreDelay = GetAttribute().m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	// Col Skip
	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	pChar->SetReservedSliding( GetAttribute().m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_dwMotionStartTime = m_dwMotionEndTime = m_dwFireMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + GetAttribute().m_dwEndDelay;
	m_dwFireMotionEndTime += dwPreDelay;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;
	
	DWORD dwFireStartTime;
	dwFireStartTime = FRAMEGETTIME();
	dwFireStartTime += dwPreDelay;
	dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwFireStartTime = dwFireStartTime;

	m_dwAttackEndTime = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( m_dwAttackEndTime > 0 )
		m_dwAttackEndTime += FRAMEGETTIME();
	else
		m_dwAttackEndTime = 0;

	SetSkillActionState( pChar );
}

void ioMultiTeleportSkill::SetSkillActionState( ioBaseChar *pChar )
{
	if( !pChar )	return;
	m_SkillState = SS_ACTION;
}

void ioMultiTeleportSkill::SetSkillLoopState( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_SkillState == SS_LOOP )	return;

	m_SkillState = SS_LOOP;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_SkillLoopMotion );
	if( iAniID == -1 )	return;

	float fTimeRate  = m_fSkillLoopMotionRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += m_dwSkillLoopLoopDuration * fTimeRate;
}

void ioMultiTeleportSkill::SetSkillControlState( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_SkillState == SS_CONTROL )	return;

	m_SkillState = SS_CONTROL;

	// Magic Circle
	if( !pChar->CheckMagicCircle() )
	{
		ioHashString szDummyCircle;
		pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, szDummyCircle, m_fCircleOffSet );
	}

	m_dwControlStartTime = FRAMEGETTIME();
}

void ioMultiTeleportSkill::SetSkillTeleportState( ioBaseChar *pChar )
{
	if( !pChar )	return;
	ioPlayStage *pStage = pChar->GetCreator();
	if( !pStage )	return;

	m_SkillState = SS_TELEPORT;

	ioMapEffect *pTeamEffect = NULL;
	if( pChar->GetTeam() == TEAM_RED )
	{
		pTeamEffect = pChar->CreateMapEffectBySkill( m_RedEffect, m_vCirclePos );
	}
	else if( pChar->GetTeam() == TEAM_BLUE )
	{
		pTeamEffect = pChar->CreateMapEffectBySkill( m_BlueEffect, m_vCirclePos );
	}
	else
	{
		if( pChar->GetSingleConvertTeam() == TEAM_BLUE )
			pTeamEffect = pChar->CreateMapEffectBySkill( m_BlueEffect, m_vCirclePos );
		else
			pTeamEffect = pChar->CreateMapEffectBySkill( m_RedEffect, m_vCirclePos );
	}

	D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
	D3DXVECTOR3	vOrigPos = pChar->GetWorldPosition();
	ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_TeleportStartEffect, vOrigPos, vScale );
	if( pMapEffect )
		m_dwTeleportStartEffectID = pMapEffect->GetUniqueID();

	D3DXVECTOR3	vPos = m_vCirclePos;
	vPos.y += m_fHightOffset;
	pChar->SetWorldPosition( vPos );
	
	pStage->SetCameraToOwnerExplicit( pChar );

	pMapEffect = pStage->CreateMapEffect( m_SkillMapEffectName, vPos, vScale );
	if( pMapEffect )
		m_dwSkillMapEffect = pMapEffect->GetUniqueID();

	int iCnt = m_GrapplingTargetInfoList.size();
	for( int i=0; i<iCnt; ++i )
	{
		ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetInfoList[i].m_TargetName );
		if( pTarget )
		{
			ioMapEffect *pEff = pStage->CreateMapEffect( m_TeleportStartEffect, pTarget->GetWorldPosition(), vScale );
			if( pEff )
				m_dwTeleportStartEffectIDList.push_back( pEff->GetUniqueID() );

			D3DXVECTOR3 vWarpPos = pChar->GetEnableWarpPosition( pTarget, m_iEnablePositionCnt, m_fEnablePositionRange, m_fEnableMaxPositionRange, m_bCheckDropZone, false, 0.0f );
			vWarpPos.y += m_fTargetHightOffset;
			m_GrapplingTargetInfoList[i].m_vPos = vWarpPos;
			pTarget->SetWorldPosition( vWarpPos );
		}
	}

	AddTargetBuff( pChar );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_TELEPORT;
		kPacket << m_vCirclePos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMultiTeleportSkill::SetEscapeTarget( ioBaseChar *pChar, ioBaseChar *pTarget )
{
	if( !pChar )	return;
	if( !pTarget )	return;

	GrapplingTargetInfoList::iterator iter = m_GrapplingTargetInfoList.begin();
	while( iter != m_GrapplingTargetInfoList.end() )
	{
		if( iter->m_TargetName == pTarget->GetCharName() )
		{
			DestroyGuideEffect( pTarget->GetCharName() );
			iter = m_GrapplingTargetInfoList.erase(iter);
			
			ioEntityGroup *pTargetGrp = pTarget->GetGroup();
			int iAniID = pTargetGrp->GetAnimationIdx( m_szTargetCancelMotion );
			if( iAniID != -1 )
			{
				D3DXQUATERNION qtRot;
				D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
				ioMath::CalcDirectionQuaternion( qtRot, vDir );
				pTarget->SetTargetRotToRotate( qtRot, true, false );

				pTarget->SetWoundedState( m_szTargetCancelMotion, 0, FLOAT1, 0, false );
				pTarget->SetReservedSliding( m_vTargetCancelForce, iAniID, FLOAT1, 0 );
			}
		}
		else
			++iter;
	}
}

void ioMultiTeleportSkill::SetSkillEndState( ioBaseChar *pChar )
{
	if( m_SkillState == SS_END )	return;
	if( !pChar )	return;

	DestroyGuideEffect();
	m_SkillState = SS_END;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iCurSkillAniID = pGrp->GetAnimationIdx( m_EndAnimation );
	if( iCurSkillAniID == -1 )	return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fEndAniRate );

	m_dwMotionEndTime = FRAMEGETTIME();		
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iCurSkillAniID )*m_fEndAniRate;
}

void ioMultiTeleportSkill::ProcessWoundGrapplingState( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_GrapplingTargetInfoList.empty() )
		return;

	int iCnt = m_GrapplingTargetInfoList.size();
	for( int i=0; i<iCnt; ++i )
	{
		if( m_GrapplingTargetInfoList[i].m_GrapplingTargetState == GTS_STAND_BACK )
			continue;

		DWORD fMinTime = m_GrapplingTargetInfoList[i].m_dwEnableCancelTime;
		DWORD fMaxTime = m_GrapplingTargetInfoList[i].m_dwEnableCancelEndTime;
		if( fMinTime < fMaxTime && !COMPARE( FRAMEGETTIME(), fMinTime, fMaxTime ) )
			continue;

		ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetInfoList[i].m_TargetName );

		if( pTarget && pTarget->IsNeedProcess() )
		{
			if( pTarget->IsPrisonerMode() || pTarget->IsCatchMode() )
				continue;

			if(	( !m_bPreLeft && pTarget->CheckCurDirKey() == ioUserKeyInput::DKI_LEFT ) ||
				( m_bPreLeft && pTarget->CheckCurDirKey() == ioUserKeyInput::DKI_RIGHT ) )			
			{
				IORandom eRandom;
				eRandom.Randomize();

				DWORD dwCurFastValue = eRandom.Random( m_dwFastValue );
				dwCurFastValue++;

				m_GrapplingTargetInfoList[i].m_dwCancelValue += dwCurFastValue;

				m_bPreLeft ^= true;
			}

			if( m_GrapplingTargetInfoList[i].m_dwCancelValue >= m_dwGrapplingMaxValue )
			{
				SetEscapeTarget( pChar, pTarget );
				
				if( pTarget->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
					kPacket << pChar->GetCharName();
					kPacket << GetName();
					kPacket << SSC_ESCAPE_TARGET;
					kPacket << pTarget->GetCharName();
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
				return;
			}
		}
	}
}

void ioMultiTeleportSkill::CheckTargetState( ioBaseChar *pChar )
{
	if( !pChar )	return;
	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	if( !pStage )	return;
	if( m_GrapplingTargetInfoList.empty() )	return;

	GrapplingTargetInfoList::iterator iter = m_GrapplingTargetInfoList.begin();
	while( iter!=m_GrapplingTargetInfoList.end() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( iter->m_TargetName );
		if( !pTarget )
		{
			DestroyGuideEffect( iter->m_TargetName );
			iter = m_GrapplingTargetInfoList.erase( iter );
			continue;
		}
		if( pTarget )
		{
			if( m_dwControlStartTime > 0 && m_dwControlStartTime+g_App.GetGrapplingCheckDelay() < FRAMEGETTIME() &&
				pTarget->GetState() != CS_GRAPPLING_WOUNDED_SKILL )
			{
				DestroyGuideEffect( pTarget->GetCharName() );
				iter = m_GrapplingTargetInfoList.erase( iter );
				continue;
			}
		}

		++iter;
	}
}

void ioMultiTeleportSkill::AddTargetBuff( ioBaseChar* pChar )
{
	if( !pChar )	return;
	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	if( !pStage )	return;

	if( m_TargetBuffList.empty() )	return;

	int iCnt = m_GrapplingTargetInfoList.size();
	for( int i=0; i<iCnt; ++i )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetInfoList[i].m_TargetName );
		if( pTarget )
		{
			int iBuffCnt = m_TargetBuffList.size();
			for( int j=0; j<iBuffCnt; ++j )
			{
				pTarget->AddNewBuff( m_TargetBuffList[j], pChar->GetCharName(), "", NULL );
			}
		}
	}
}

bool ioMultiTeleportSkill::isEnableCancel( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;

	if( m_GrapplingTargetInfoList.empty() )
		return false;

	int iCnt = m_GrapplingTargetInfoList.size();
	for( int i=0; i<iCnt; ++i )
	{
		if( m_GrapplingTargetInfoList[i].m_TargetName == pTarget->GetCharName() &&
			m_GrapplingTargetInfoList[i].m_GrapplingTargetState != GTS_STAND_BACK )
		{
			DWORD fMinTime = m_GrapplingTargetInfoList[i].m_dwEnableCancelTime;
			DWORD fMaxTime = m_GrapplingTargetInfoList[i].m_dwEnableCancelEndTime;
			if( COMPARE( FRAMEGETTIME(), fMinTime, fMaxTime ) )
				return true;

			if( fMinTime == fMaxTime )
				return true;
		}
	}

	return false;
}

void ioMultiTeleportSkill::ProcessGuideEffect( ioBaseChar *pOwner )
{
	if( !pOwner )	return;

	if( !CheckGuideEffectCreated( pOwner ) )
		return;

	ArrangeGuideEffect( pOwner );
}

void ioMultiTeleportSkill::DestroyGuideEffect()
{
	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	if( !pStage )	return;

	int iCnt = m_GrapplingTargetInfoList.size();
	for( int i=0; i<iCnt; ++i )
	{
		std::vector< UniqueObjID >::iterator iter = m_GrapplingTargetInfoList[i].m_EffectList.begin();
		for( ; iter!=m_GrapplingTargetInfoList[i].m_EffectList.end() ; ++iter )
		{
			ioMapEffect *pMapEffect = pStage->FindMapEffect( *iter );
			if( pMapEffect )
				pMapEffect->EndEffectForce();
		}
		m_GrapplingTargetInfoList[i].m_EffectList.clear();
	}
}

void ioMultiTeleportSkill::DestroyGuideEffect( ioHashString szTargetName )
{
	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	if( !pStage )	return;
	if( m_GrapplingTargetInfoList.empty() )	return;
	if( szTargetName.IsEmpty() )	return;

	int iCnt = m_GrapplingTargetInfoList.size();
	for( int i=0; i<iCnt; ++i )
	{
		if( m_GrapplingTargetInfoList[i].m_TargetName == szTargetName )
		{
			std::vector< UniqueObjID >::iterator iter = m_GrapplingTargetInfoList[i].m_EffectList.begin();
			for( ; iter!=m_GrapplingTargetInfoList[i].m_EffectList.end() ; ++iter )
			{
				ioMapEffect *pMapEffect = pStage->FindMapEffect( *iter );
				if( pMapEffect )
					pMapEffect->EndEffectForce();
			}
			m_GrapplingTargetInfoList[i].m_EffectList.clear();
		}
	}
}

bool ioMultiTeleportSkill::CheckGuideEffectCreated( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )	return false;

	if( m_GrapplingTargetInfoList.empty() )
		return false;

	int iCnt = m_GrapplingTargetInfoList.size();
	for( int i=0; i<iCnt; ++i )
	{
		if( !m_GrapplingTargetInfoList[i].m_EffectList.empty() )
			continue;

		D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );

		m_GrapplingTargetInfoList[i].m_EffectList.reserve( m_iMaxEffectCount );
		for( int j=0; j<m_iMaxEffectCount; ++j )
		{
			ioMapEffect *pEffect = pStage->CreateMapEffect( m_GuideEffect, vScale );
			if( pEffect )
			{
				m_GrapplingTargetInfoList[i].m_EffectList.push_back( pEffect->GetUniqueID() );
			}
		}
	}

	return true;
}

void ioMultiTeleportSkill::ArrangeGuideEffect( ioBaseChar *pOwner )
{
	if( !pOwner )	return;
	
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )	return;
	
	if( m_iMaxEffectCount <= 0 || m_GrapplingTargetInfoList.empty() )
		return;

	int iCnt = m_GrapplingTargetInfoList.size();
	for( int i=0; i<iCnt; ++i )
	{
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_GrapplingTargetInfoList[i].m_TargetName );
		if( !pTarget )	continue;

		D3DXVECTOR3 vStartPos = pOwner->GetMidPositionByRate();
		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		D3DXVECTOR3 vDiff = vTargetPos - vStartPos;

		float fTargetLength = D3DXVec3Length( &vDiff );
		D3DXVec3Normalize( &vDiff, &vDiff );

		// Update Arrow Pos and Rotation
		D3DXVECTOR3 vCurPos = vStartPos;
		float fGapLength = min( m_fMaxEffectRange, fTargetLength ) / m_iMaxEffectCount;

		float fCurRate = FLOAT1;
		if( m_fMaxEffectRange > 0.0f )
			fCurRate = fTargetLength / m_fMaxEffectRange;

		float fCurScale = max( 0.1f, fCurRate );

		std::vector< UniqueObjID >::iterator iter = m_GrapplingTargetInfoList[i].m_EffectList.begin();
		for( ; iter!=m_GrapplingTargetInfoList[i].m_EffectList.end() ; ++iter )
		{
			vCurPos += vDiff * fGapLength;

			ioMapEffect *pMapEff = pStage->FindMapEffect( *iter );
			if( pMapEff )
			{
				pMapEff->SetWorldPosition( vCurPos );

				ioEffect *pEff = pMapEff->GetEffect();
				if( pEff )
				{
					D3DXVECTOR3 vScale( FLOAT1, FLOAT1, fCurScale );
					pEff->GetParentNode()->SetScale( vScale );

					D3DXQUATERNION qtRot = Help::ConvertDirToQuaternion( vDiff );
					pEff->GetParentNode()->SetOrientation( qtRot );
				}
			}
		}
	}
}