
#include "stdafx.h"

#include "ioAutoShotItem.h"

#include "ioBaseChar.h"


#include "WeaponDefine.h"

ioAutoShotItem::ioAutoShotItem()
{
	m_iCurBullet = 0;
	m_iMaxBullet = 0;
	m_fReloadTimeRate = FLOAT1;

	m_fAimAngle = 45.0f;
	m_fAimRange = FLOAT1000;
	m_fMinRange = FLOAT100;

	m_iCurBullet = 0;
	m_fMarkerRange = 0.0f;
	m_fReloadTimeModifyRate = FLOAT1;

	m_bSetReloadMoveAni = false;

	m_dwReloadStartTime = 0;
	m_dwReloadEndTime = 0;
	m_dwNextReloadTime = 0;
	m_dwReloadGapTime = 0;

	m_dwFireStartTime = 0;
	m_dwFireEndTime = 0;
	m_dwFireDelayTime = 0;

	m_dwChargeStartTime = 0;

	m_iCurCombo = 0;
	m_FireState = FS_NONE;
	m_JumpAutoShotState = JAS_NONE;

	m_LevelTimeRate.clear();

	m_bSetSkillAimed = false;
	m_bSetAimedDir = false;

	m_bFirstAimMoveMotion = true;
	m_bAimMotionSetted = false;
	m_fCurAngle = 0.0f;
	m_dwTrackingEndTime = 0;
	m_dwFireStateEndTime = 0;
	m_AimStartDirKey = ioUserKeyInput::DKI_NONE;

	m_bAimEffect = false;
	m_dwCheckAimCancelTime = 0;

	m_dwRotateReturnStartTime = 0;
	m_dwRotateReturnEndTime = 0;

	m_dwJumpAutoShotReadyStartTime = 0;
	m_fAirJumpPowerRate = 0.0f;
}

ioAutoShotItem::ioAutoShotItem( const ioAutoShotItem &rhs )
: ioWeaponItem( rhs ),
 m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
 m_vJumpAttributeList( rhs.m_vJumpAttributeList ),
  m_ReloadAnimation( rhs.m_ReloadAnimation ),
 m_ReloadEffect( rhs.m_ReloadEffect ),
 m_ReloadMoveAnimation( rhs.m_ReloadMoveAnimation ),
  m_AimEffect( rhs.m_AimEffect ),
  m_fAimMoveAniRate( rhs.m_fAimMoveAniRate ),
  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
  m_bEnableDownAim( rhs.m_bEnableDownAim ),
  m_MotionInfoList( rhs.m_MotionInfoList ),
  m_JumpMotionInfoList( rhs.m_JumpMotionInfoList ),
  m_AutoShotEndAni( rhs.m_AutoShotEndAni ),
  m_fAutoShotEndAniRate( rhs.m_fAutoShotEndAniRate ),
  m_iMaxBullet( rhs.m_iMaxBullet ),
  m_fReloadTimeRate( rhs.m_fReloadTimeRate ),
  m_fAimAngle( rhs.m_fAimAngle ),
  m_fAimRange( rhs.m_fAimRange ),
  m_fMinRange( rhs.m_fMinRange ),
  m_fMarkerRange( rhs.m_fMarkerRange ),
  m_LevelTimeRate( rhs.m_LevelTimeRate ),
  m_dwJumpAutoShotReadyDuration( rhs.m_dwJumpAutoShotReadyDuration ),
  m_FireEffectList( rhs.m_FireEffectList )
{
	if( m_iMaxBullet > 0 )
		m_iCurBullet = m_iMaxBullet;
	else
		m_iCurBullet = 0;

	m_fReloadTimeModifyRate = FLOAT1;

	m_bSetReloadMoveAni = false;

	m_dwReloadStartTime = 0;
	m_dwReloadEndTime = 0;
	m_dwNextReloadTime = 0;
	m_dwReloadGapTime = 0;

	m_dwFireStartTime = 0;
	m_dwFireEndTime = 0;
	m_dwFireDelayTime = 0;

	m_dwChargeStartTime = 0;

	m_iCurCombo = 0;
	m_FireState = FS_NONE;
	m_bSetSkillAimed = false;
	m_bSetAimedDir = false;

	m_bFirstAimMoveMotion = true;
	m_bAimMotionSetted = false;
	m_fCurAngle = 0.0f;
	m_dwTrackingEndTime = 0;
	m_dwFireStateEndTime = 0;
	m_AimStartDirKey = ioUserKeyInput::DKI_NONE;

	m_bAimEffect = false;
	m_dwCheckAimCancelTime = 0;
}

ioAutoShotItem::~ioAutoShotItem()
{
	m_LevelTimeRate.clear();
	m_MotionInfoList.clear();
	m_JumpMotionInfoList.clear();
	m_FireEffectList.clear();
}

void ioAutoShotItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	int iExtendCnt = rkLoader.LoadInt_e( "extend_attack_cnt", 0 );
	m_vExtendAttributeList.clear();
	m_vExtendAttributeList.reserve( iExtendCnt );
	for( int i=0 ; i<iExtendCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "extend_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vExtendAttributeList.push_back( kAttribute );
	}

	int iJumpCnt = rkLoader.LoadInt_e( "jump_attack_cnt", 0 );
	m_vJumpAttributeList.clear();
	m_vJumpAttributeList.reserve( iJumpCnt );
	for( int i=0 ; i<iJumpCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "jump_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vJumpAttributeList.push_back( kAttribute );
	}

	int iReadyCnt = rkLoader.LoadInt_e( "attack_ready_cnt", 0 );
	m_AttackReadyAniList.clear();
	m_AttackReadyAniList.reserve( iReadyCnt );
	for( i=0 ; i<iReadyCnt ; i++ )
	{
		wsprintf_e( szKey, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	m_MotionInfoList.clear();
	m_MotionInfoList.reserve( iExtendCnt );

	for( int i=0; i < iExtendCnt; ++i )
	{
		MotionInfo kInfo;

		wsprintf_e( szKey, "fire_up_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_FireUp = szBuf;

		wsprintf_e( szKey, "fire_center_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_FireCenter = szBuf;

		wsprintf_e( szKey, "fire_down_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_FireDown = szBuf;

		wsprintf_e( szKey, "aim_move_front%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_AimMoveFront = szBuf;

		wsprintf_e( szKey, "aim_move_back%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_AimMoveBack = szBuf;

		wsprintf_e( szKey, "aim_move_left%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_AimMoveLeft = szBuf;

		wsprintf_e( szKey, "aim_move_right%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_AimMoveRight = szBuf;

		m_MotionInfoList.push_back( kInfo );
	}

	m_fAimMoveAniRate = rkLoader.LoadFloat_e( "aim_move_ani_rate", FLOAT1 );

	m_JumpMotionInfoList.clear();
	m_JumpMotionInfoList.reserve( iExtendCnt );
	for( int i=0; i < iExtendCnt; ++i )
	{
		JumpMotionInfo kInfo;

		wsprintf_e( szKey, "jump_fire_up_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_FireUp = szBuf;

		wsprintf_e( szKey, "jump_fire_center_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_FireCenter = szBuf;

		wsprintf_e( szKey, "jump_fire_down_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_FireDown = szBuf;

		m_JumpMotionInfoList.push_back( kInfo );
	}

	m_FireEffectList.clear();
	m_FireEffectList.reserve( iExtendCnt );
	for( i=0; i < iExtendCnt; i++ )
	{
		wsprintf_e( szKey, "fire_effect%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_FireEffectList.push_back( ioHashString(szBuf) );
	}

	m_iMaxBullet = rkLoader.LoadInt_e( "max_bullet", 0 );
	m_fReloadTimeRate  = rkLoader.LoadFloat_e( "reload_time_rate", FLOAT1 );

	rkLoader.LoadString_e( "reload_animation", "", szBuf, MAX_PATH );
	m_ReloadAnimation = szBuf;
	rkLoader.LoadString_e( "reload_move_animation", "", szBuf, MAX_PATH );
	m_ReloadMoveAnimation = szBuf;
	rkLoader.LoadString_e( "reload_effect", "", szBuf, MAX_PATH );
	m_ReloadEffect = szBuf;

	rkLoader.LoadString_e( "auto_shot_end_animation", "", szBuf, MAX_PATH );
	m_AutoShotEndAni = szBuf;
	m_fAutoShotEndAniRate = rkLoader.LoadFloat_e( "auto_shot_end_time_rate", FLOAT1 );

	m_bEnableDownAim = rkLoader.LoadBool_e( "enable_down_aim", false );

	rkLoader.LoadString_e( "aim_effect", "", szBuf, MAX_PATH );
	m_AimEffect = szBuf;

	m_LevelTimeRate.clear();
	int iLevelCount = rkLoader.LoadInt_e( "level_count", 0 );
	for( i=0; i<iLevelCount; i++ )
	{
		wsprintf_e( szBuf, "level_time_rate%d", i);
		
		float fRate = rkLoader.LoadFloat( szBuf, -FLOAT1 );
		if( fRate == -FLOAT1 && i > 0 )
		{
			if( i>0 )
				fRate = m_LevelTimeRate[i-1];
			else
				fRate = FLOAT1;
		}

		m_LevelTimeRate.push_back( fRate );
	}

	m_fAimAngle = rkLoader.LoadFloat_e( "aim_angle", 45.0f );
	m_fAimRange = rkLoader.LoadFloat_e( "aim_range", FLOAT1000 );
	m_fMinRange = rkLoader.LoadFloat_e( "aim_min_range", 0.0f );

	m_fMarkerRange = rkLoader.LoadFloat_e( "marker_range", 0.0f );

	m_dwJumpAutoShotReadyDuration = rkLoader.LoadInt_e( "jump_auto_shot_ready_time", 0 );
}

ioItem* ioAutoShotItem::Clone()
{
	return new ioAutoShotItem( *this );
}

bool ioAutoShotItem::SetExtendDefenseState( ioBaseChar *pOwner )
{
	if( m_iCurBullet == m_iMaxBullet)
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( m_ReloadAnimation );
	if( iAniID == -1 )	return false;

	float fTimeRate = m_fReloadTimeRate;

	int iLevelCount = m_LevelTimeRate.size();
	if( iLevelCount > 1 )
	{
		int iDividCnt, iNeedCnt;
		iDividCnt = m_iMaxBullet / iLevelCount;
		iNeedCnt = m_iMaxBullet - m_iCurBullet;

		LevelTimeRate::iterator iter = m_LevelTimeRate.begin();
		for( int i=1; i <= iLevelCount; i++ )
		{
			if( iNeedCnt <= iDividCnt * i )
			{
				fTimeRate *= *iter;
				break;
			}

			++iter;
		}
	}
	else
	{
		fTimeRate = m_fReloadTimeRate;
	}

	m_fReloadTimeModifyRate = m_fReloadTimeRate;

	m_bSetReloadMoveAni = false;
	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/m_fReloadTimeModifyRate );

	m_dwReloadStartTime = FRAMEGETTIME();
	m_dwReloadEndTime = m_dwReloadStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate - 100;
	m_dwReloadGapTime = ( m_dwReloadEndTime - m_dwReloadStartTime ) / m_iMaxBullet;
	m_dwNextReloadTime = m_dwReloadStartTime + m_dwReloadGapTime;

	pOwner->SetState( CS_EXTEND_DEFENSE );
	pOwner->SetReloadMoveEnable( true );

	if( !m_ReloadEffect.IsEmpty() )
	{
		pOwner->ShowWeaponItemMesh( false );
		pOwner->AttachEffect( m_ReloadEffect );
	}

	return true;
}

void ioAutoShotItem::CheckExtendDefenseState( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwReloadEndTime + pOwner->GetActionStopTotalDelay() > dwCurTime )
	{
		// Check Skill ( Reload 일때만 )
		int iSkillNum = pOwner->GetSkillKeyInput();
		if( iSkillNum == 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(0, SUT_NORMAL))
		{
			m_bSetReloadMoveAni = false;
			m_FireState = FS_NONE;
			pOwner->SetReloadMoveEnable( false );

			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << false;	//AimJump
			kPacket << (int)m_FireState;
			kPacket << m_iCurCombo;
			kPacket << false;
			P2PNetwork::SendToAllPlayingUser( kPacket );
			return;
		}

		if( m_dwNextReloadTime < dwCurTime )
		{
			m_iCurBullet++;
			m_iCurBullet = min( m_iCurBullet, m_iMaxBullet );
			m_dwNextReloadTime += m_dwReloadGapTime;
		}
	}
	else
	{
		m_iCurBullet = m_iMaxBullet;
		m_bSetReloadMoveAni = false;
		m_FireState = FS_NONE;
		pOwner->SetReloadMoveEnable( false );
		pOwner->SetState( CS_DELAY );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << false;	//AimJump
			kPacket << (int)m_FireState;
			kPacket << m_iCurCombo;
			kPacket << true;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	g_TutorialMgr.ActionWeaponCharge();
}

void ioAutoShotItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );

	if( m_bAimEffect )
	{
		m_bAimEffect = false;
		pOwner->EndEffect( m_AimEffect, false );
	}

	int iReadyCnt = m_AttackReadyAniList.size();
	if( COMPARE( iCurCombo, 0, iReadyCnt ) )
	{
		m_FireState = FS_CHARGING;
		m_dwChargeStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;

		m_fCurAngle = 0.0f;
		m_dwTrackingEndTime = 0;
		m_dwCheckAimCancelTime = 0;

		pOwner->SetState( CS_ATTACK );
		pOwner->SetChargingState( true );

		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllActionAni( FLOAT10, true );
		pGrp->ClearAllLoopAni( FLOAT10, true );
		pGrp->SetActionAniForTime( m_AttackReadyAniList[iCurCombo], FLOAT10, 0.0f, m_dwChargeTime );
	}
	else
	{
		ioAttackableItem::SetNormalAttack( pOwner, iCurCombo );
		m_FireState = FS_NONE;
	}
}

void ioAutoShotItem::ChangeToAutoShot( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	pOwner->StopAutoTargetTracking();

	DWORD dwCurTime = FRAMEGETTIME();

	m_FireState = FS_AUTO_SHOT;
	pOwner->SetAttackMoveEnable( true );
	pOwner->ChangeDirectionExplicitIfDirChanged( true );

	m_AimStartDirKey = pOwner->GetCurDirKey();

	m_bFirstAimMoveMotion = true;
	m_bAimMotionSetted = true;

	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	m_vStartDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	m_AimedTarget.Clear();
	m_bAimedBall = false;

	m_bSetAimedDir = false;
	m_bAimedState = false;
	m_iCurCombo = 0;

	if( !m_bAimEffect )
	{
		pOwner->AttachEffect( m_AimEffect );
		m_bAimEffect = true;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
		pGrp->ClearAllLoopAni( FLOAT10, true );

	// 타겟 찾기
	ioHashString szTarget;
	m_AimedTarget.Clear();
	m_bAimedBall = false;

	if( pOwner->CheckAimedTargetByBow( m_fAimAngle, m_fMinRange, m_fAimRange, szTarget, m_bEnableDownAim ) )
	{
		m_AimedTarget = szTarget;
	}

	// 타겟 비교
	if( CompareAimedTarget( pOwner ) )
	{
		m_AimedTarget.Clear();
		m_bAimedBall = true;
	}

	pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );

	if( !m_AimedTarget.IsEmpty() || m_bAimedBall )
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
	else
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );

	m_dwTrackingEndTime = dwCurTime;

	SetChangeFireMotion( pOwner );
}

void ioAutoShotItem::ChangeToAutoShotEnd( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	m_FireState = FS_AUTO_SHOT_END;
	m_AimedTarget.Clear();
	m_bAimedBall = false;

	m_bSetSkillAimed = false;
	m_bSetAimedDir = false;
	m_bAimedState = false;

	pOwner->SetMoveDirByRotate( pOwner->GetTargetRot() );

	pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );

	int iAniID = pGrp->GetAnimationIdx( m_AutoShotEndAni );
	float fTimeRate = m_fAutoShotEndAniRate;

	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->ClearAllLoopAni( FLOAT10, true );

	if( iAniID == -1 )
	{
		m_dwRotateReturnStartTime = dwCurTime;
		m_dwRotateReturnEndTime = dwCurTime;
		return;
	}

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_dwRotateReturnStartTime = dwCurTime;
	m_dwRotateReturnEndTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate ;
}

void ioAutoShotItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if ( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_FireState = FS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << false;
		kPacket << (int)m_FireState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioAutoShotItem::ChangeToJumpNormalShot( ioBaseChar *pOwner )
{
	if( m_vJumpAttributeList.empty() ) return false;
	if( m_JumpMotionInfoList.empty() ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	m_iCurCombo = 0;
	m_JumpAutoShotState = JAS_NORMAL;


	m_AimedTarget.Clear();
	m_bAimedBall = false;

	pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );

	m_dwTrackingEndTime = dwCurTime;

	// FireDir 처리
	// 발사 모션 처리
	int	  iAniID = -1;
	float fTimeRate = FLOAT1;
	float fWeight = 0.0f;

	pGrp->ClearAllActionAni( 0.0f, true );

	const AttackAttribute *pAttr = &m_vJumpAttributeList[m_iCurCombo];
	if( pAttr )
	{
		iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
		fTimeRate = pAttr->m_fAttackAniRate;

		pOwner->RefreshFireTimeList( iAniID, pAttr->m_vWeaponInfoList, FTT_JUMP_ATTACK, fTimeRate, 0 );
		pOwner->IncreaseWeaponIndexBase();
	}

	ioHashString szFireUp, szFireCenter;
	if( COMPARE( m_iCurCombo, 0, (int)m_JumpMotionInfoList.size() ) )
	{
		szFireCenter = m_JumpMotionInfoList[m_iCurCombo].m_FireCenter;
		pGrp->SetActionAni( szFireCenter, 0.0f, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0, 0, true );
	}

	m_bAimedState = false;
	m_bSetAimedDir = false;


	// 시간값들 갱신
	m_dwFireStartTime = m_dwRotateStartTime = dwCurTime;
	m_dwFireEndTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwRotateEndTime = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwKeyReserveTime = 0;


	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << true;
		kPacket << (int)m_JumpAutoShotState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return true;
}

void ioAutoShotItem::ChangeToJumpAutoShot( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	pOwner->StopAutoTargetTracking();

	DWORD dwCurTime = FRAMEGETTIME();

	m_JumpAutoShotState = JAS_AUTO_SHOT;
	pOwner->ChangeDirectionExplicitIfDirChanged( true );
	m_AimStartDirKey = pOwner->GetCurDirKey();

	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	m_vStartDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	m_AimedTarget.Clear();
	m_bAimedBall = false;

	m_bSetAimedDir = false;
	m_bAimedState = false;
	m_iCurCombo = 0;

	if( !m_bAimEffect )
	{
		pOwner->AttachEffect( m_AimEffect );
		m_bAimEffect = true;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllActionAni( FLOAT10, true );
		pGrp->ClearAllLoopAni( FLOAT10, true );
	}

	// 타겟 찾기
	ioHashString szTarget;
	if( pOwner->CheckAimedTargetByBow( m_fAimAngle, m_fMinRange, m_fAimRange, szTarget, m_bEnableDownAim ) )
	{
		m_AimedTarget = szTarget;
	}

	// 타겟 비교
	if( CompareAimedTarget( pOwner ) )
	{
		m_bAimedBall = true;
		m_AimedTarget.Clear();
	}

	pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );

	if( !m_AimedTarget.IsEmpty() || m_bAimedBall )
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
	else
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );

	m_dwTrackingEndTime = dwCurTime;

	SetChangeJumpFireMotion( pOwner );
}

void ioAutoShotItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_FireState )
	{
	case FS_NORMAL_ATTACK:
		ioAttackableItem::CheckNormalAttack( pOwner );
		break;
	case FS_CHARGING:
		OnCharging( pOwner );
		break;
	case FS_AUTO_SHOT:
		OnAutoShot( pOwner );
		break;
	case FS_AUTO_SHOT_END:
		OnAutoShotEnd( pOwner );
		break;
	}

	if( pOwner->IsCanAttackMove() && m_FireState != FS_AUTO_SHOT )
	{
		pOwner->SetAttackMoveEnable( false );
	}

	if( pOwner->GetState() == CS_ATTACK )
		UpdateAimDir( pOwner );

	CheckAimMoveAniState( pOwner );
}

bool ioAutoShotItem::CheckJumpAutoShotState( ioBaseChar *pOwner )
{
	bool bResult = false;
	switch( m_JumpAutoShotState )
	{
	case JAS_READY:
		bResult = OnJumpAutoShotReady( pOwner );
		break;
	case JAS_NORMAL:
		bResult = OnJumpNormalShot( pOwner );
		break;
	case JAS_AUTO_SHOT:
		bResult = OnJumpAutoShot( pOwner );
		break;
	case JAS_NONE:
		bResult = false;
		break;
	}

	UpdateAimDir( pOwner );

	return bResult;
}

void ioAutoShotItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwChargeStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			if( m_iCurBullet >= 1 || m_iMaxBullet == 0 )
			{
				ChangeToAutoShot( pOwner );

				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << false;
					kPacket << (int)m_FireState;
					kPacket << m_iCurCombo;
					kPacket << m_AimedTarget;
					kPacket << m_bAimedBall;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
			else
			{
				pOwner->NormalAttackOver();

				if( m_iCurBullet < 1 && SetExtendDefenseState( pOwner ) )
				{
					if( pOwner->IsNeedSendNetwork() )
					{
						SP2Packet kPacket( CUPK_EXTEND_DEFENSE );
						kPacket << pOwner->GetCharName();
						kPacket << pOwner->GetHP().m_fCurValue;
						kPacket << m_iCurBullet;
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}
				}
			}
		}
	}
	else
	{
		pOwner->SetChargingState( false );
		ChangeToNormalAttack( pOwner );
	}
}

void ioAutoShotItem::OnAutoShot( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	// 시간체크...
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwFireStartTime+m_dwFireEndTime < dwCurTime )
	{
		// 버튼 뗌 : 종료동작으로...
		if( !pOwner->IsAttackKeyDown() )
		{
			pOwner->NormalAttackOver();

			if( SetExtendDefenseState( pOwner ) )
			{
				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_EXTEND_DEFENSE );
					kPacket << pOwner->GetCharName();
					kPacket << pOwner->GetHP().m_fCurValue;
					kPacket << m_iCurBullet;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
			return;
		}

		// 스킬 체크
		if( pOwner->GetState() != CS_JUMP && pOwner->IsDefenseKeyDown() && !pOwner->IsBuffLimitSkill() )
		{
			// Reload Skill
			if( pOwner->SetUseSkill(0, SUT_NORMAL))
			{
				m_FireState = FS_NONE;

				m_bSetSkillAimed = false;
				m_bSetAimedDir = false;
				m_bAimedState = false;

				return;
			}
		}

		// 점프
		if( pOwner->IsJumpKeyDown() && !pOwner->IsBuffLimitJump() )
		{
			pOwner->SetJumpState();
			return;
		}

		// 다음 발사
		if( IsCanFire( pOwner, 1 ) )
		{
			// 타겟 체크
			if( !CheckCurrentTargetValid( pOwner, true ) )
			{
				// 타겟 찾기
				ioHashString szTarget;

				m_AimedTarget.Clear();
				m_bAimedBall = false;

				if( pOwner->CheckAimedTargetByBow( m_fAimAngle, m_fMinRange, m_fAimRange, szTarget, m_bEnableDownAim ) )
				{
					m_AimedTarget = szTarget;
				}

				// 타겟 비교
				if( CompareAimedTarget( pOwner ) )
				{
					m_AimedTarget.Clear();
					m_bAimedBall = true;
				}

				pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );

				if( !m_AimedTarget.IsEmpty() || m_bAimedBall )
					pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
				else
					pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );

				m_dwTrackingEndTime = dwCurTime;
			}

			m_iCurCombo++;
			SetChangeFireMotion( pOwner );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << false;
				kPacket << (int)m_FireState;
				kPacket << m_AimedTarget;
				kPacket << m_bAimedBall;
				kPacket << m_iCurCombo;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return;
		}
		else
		{
			pOwner->NormalAttackOver();

			if( SetExtendDefenseState( pOwner ) )
			{
				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_EXTEND_DEFENSE );
					kPacket << pOwner->GetCharName();
					kPacket << pOwner->GetHP().m_fCurValue;
					kPacket << m_iCurBullet;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
			return;
			/*
			ChangeToAutoShotEnd( pOwner );

			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << false;	//AimJump
			kPacket << (int)m_FireState;
			kPacket << m_iCurCombo;
			P2PNetwork::SendToAllPlayingUser( kPacket );
			return;
			*/
		}
	}
}

void ioAutoShotItem::OnAutoShotEnd( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwRotateReturnEndTime < dwCurTime )
	{
		m_FireState = FS_NONE;
		
		pOwner->NormalAttackOver();
		pOwner->SetState( CS_DELAY );
		return;
	}
}

bool ioAutoShotItem::OnJumpAutoShotReady( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;
	if( !pOwner->IsNeedProcess() )
		return true;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwEndTime = m_dwJumpAutoShotReadyStartTime+m_dwJumpAutoShotReadyDuration;

	if( dwEndTime < dwCurTime )
	{
		if( pOwner->IsAttackKeyDown() )
		{
			ChangeToJumpAutoShot( pOwner );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << true;
				kPacket << (int)m_JumpAutoShotState;
				kPacket << m_iCurCombo;
				kPacket << m_AimedTarget;
				kPacket << m_bAimedBall;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
		else
		{
			if( !ChangeToJumpNormalShot( pOwner ) )
			{
				SetNoneState( pOwner );

				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << true;
					kPacket << (int)m_JumpAutoShotState;
					kPacket << m_iCurCombo;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
				return false;
			}
		}
	}
	else if( !pOwner->IsAttackKeyDown() )
	{
		if( !ChangeToJumpNormalShot( pOwner ) )
		{
			SetNoneState( pOwner );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << true;
				kPacket << (int)m_JumpAutoShotState;
				kPacket << m_iCurCombo;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return false;
		}
	}

	return true;
}

bool ioAutoShotItem::OnJumpNormalShot( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwEndTime = m_dwFireStartTime+m_dwFireEndTime;

	if( dwEndTime < dwCurTime )
	{
		SetNoneState( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << true;
			kPacket << (int)m_JumpAutoShotState;
			kPacket << m_iCurCombo;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		return false;
	}

	return true;
}

bool ioAutoShotItem::OnJumpAutoShot( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwEndTime = m_dwFireStartTime+m_dwFireEndTime;

	if( dwEndTime < dwCurTime )
	{
		if( !pOwner->IsNeedProcess() )
			return true;

		// 버튼 뗌 : 
		if( !pOwner->IsAttackKeyDown() )
		{
			SetNoneState( pOwner );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << true;
				kPacket << (int)m_JumpAutoShotState;
				kPacket << m_iCurCombo;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return false;
		}

		// 다음 발사
		if( IsCanFire( pOwner, 1 ) )
		{
			if( !CheckCurrentTargetValid( pOwner, true ) )
			{
				// 타겟 찾기
				ioHashString szTarget;

				m_AimedTarget.Clear();
				m_bAimedBall = false;

				if( pOwner->CheckAimedTargetByBow( m_fAimAngle, m_fMinRange, m_fAimRange, szTarget, m_bEnableDownAim ) )
				{
					m_AimedTarget = szTarget;
				}

				// 타겟 비교
				if( CompareAimedTarget( pOwner ) )
				{
					m_AimedTarget.Clear();
					m_bAimedBall = true;
				}

				pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );

				if( !m_AimedTarget.IsEmpty() && m_bAimedBall )
					pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
				else
					pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );

				m_dwTrackingEndTime = dwCurTime;
			}

			m_iCurCombo++;
			SetChangeJumpFireMotion( pOwner );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << true;
				kPacket << (int)m_JumpAutoShotState;
				kPacket << m_AimedTarget;
				kPacket << m_bAimedBall;
				kPacket << m_iCurCombo;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return true;
		}
		else
		{
			SetNoneState( pOwner );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << true;
				kPacket << (int)m_JumpAutoShotState;
				kPacket << m_iCurCombo;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return false;
		}
	}
	else
	{
		int iMaxCnt = m_vAirJumpTimeList.size();

		if( m_iCurAirJump < iMaxCnt )
		{
			DWORD dwCurAirJumpTime = 0;
			std::list< float >::iterator iter = m_vAirJumpTimeList.begin();

			for( int i=0; iter != m_vAirJumpTimeList.end(); i++ )
			{
				if( m_iCurAirJump == i )
				{
					dwCurAirJumpTime = (DWORD)(*iter);
					break;
				}

				iter++;
			}

			if( dwCurAirJumpTime > 0 && dwCurAirJumpTime < FRAMEGETTIME() )
			{
				if( m_fAirJumpPower >= 0.0f )
				{
					D3DXQUATERNION qtTargetRot = pOwner->GetTargetRot();
					pOwner->SetAirJumpPower( m_fAirJumpPower*m_fAirJumpPowerRate );
					D3DXVECTOR3 vAttDir =  qtTargetRot * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
					if( m_fAirForcePower < 0.0f )
						pOwner->SetForcePower( -vAttDir, fabs(m_fAirForcePower), FLOAT1, true );
					else
						pOwner->SetForcePower( vAttDir, fabs(m_fAirForcePower), FLOAT1, true );
				}
				m_iCurAirJump++;
			}
		}
	}

	return true;
}

bool ioAutoShotItem::SetChangeFireMotion( ioBaseChar *pOwner )
{
	if( !COMPARE( m_iCurCombo, 0, (int)m_vExtendAttributeList.size() ) )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	// FireDir 처리
	// 발사 모션 처리
	int	  iAniID = -1;
	float fTimeRate = FLOAT1;
	float fWeight = 0.0f;

	pGrp->ClearAllActionAni( 0.0f, true );

	const AttackAttribute *pAttr = &m_vExtendAttributeList[m_iCurCombo];
	if( pAttr )
	{
		iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
		fTimeRate = pAttr->m_fAttackAniRate;

		pOwner->RefreshFireTimeList( iAniID, pAttr->m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
		pOwner->IncreaseWeaponIndexBase();
	}

	ioBall *pBall = pOwner->FindBall();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( pTarget )
	{
		D3DXVECTOR3 vAimedDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		D3DXVec3Normalize( &vAimedDir, &vAimedDir );

		fWeight = GetAimedUpDownWeight( vAimedDir, 85.0f );
		m_vAimedDir = vAimedDir;

		if( m_vAimedDir.y >= 0.0f )
		{
			AimedFireToUp( pGrp, fWeight, fTimeRate, 0, false, true );
		}
		else
		{
			AimedFireToDown( pGrp, fWeight, fTimeRate, 0, false, true );
		}

		m_bAimedState = true;
		m_bSetAimedDir = true;
	}
	else if( m_bAimedBall && pBall )
	{
		D3DXVECTOR3 vAimedDir = pBall->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		D3DXVec3Normalize( &vAimedDir, &vAimedDir );

		fWeight = GetAimedUpDownWeight( vAimedDir, 85.0f );
		m_vAimedDir = vAimedDir;

		if( m_vAimedDir.y >= 0.0f )
		{
			AimedFireToUp( pGrp, fWeight, fTimeRate, 0, false, true );
		}
		else
		{
			AimedFireToDown( pGrp, fWeight, fTimeRate, 0, false, true );
		}

		m_bAimedState = true;
		m_bSetAimedDir = true;
	}
	else
	{
		ioHashString szFireUp, szFireCenter;
		if( COMPARE( m_iCurCombo, 0, (int)m_MotionInfoList.size() ) )
		{
			szFireCenter = m_MotionInfoList[m_iCurCombo].m_FireCenter;
			pGrp->SetActionAni( szFireCenter, 0.0f, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0, 0, true );
		}

		m_bAimedState = false;
		m_bSetAimedDir = false;
	}


	// 시간값들 갱신
	m_dwFireStartTime = m_dwRotateStartTime = dwCurTime;
	m_dwFireEndTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwRotateEndTime = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwKeyReserveTime = m_dwFireStartTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	return true;
}

bool ioAutoShotItem::SetChangeJumpFireMotion( ioBaseChar *pOwner )
{
	if( !COMPARE( m_iCurCombo, 0, (int)m_vJumpAttributeList.size() ) )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;


	// FireDir 처리
	// 발사 모션 처리
	int	  iAniID = -1;
	float fTimeRate = FLOAT1;
	float fWeight = 0.0f;

	pGrp->ClearAllActionAni( 0.0f, true );

	const AttackAttribute *pAttr = &m_vJumpAttributeList[m_iCurCombo];
	if( pAttr )
	{
		iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
		fTimeRate = pAttr->m_fAttackAniRate;

		pOwner->RefreshFireTimeList( iAniID, pAttr->m_vWeaponInfoList, FTT_JUMP_ATTACK, fTimeRate, 0 );
		pOwner->IncreaseWeaponIndexBase();

		// AirJumpPower
		SetAirJump( pOwner, iAniID, fTimeRate );
	}

	ioBall *pBall = pOwner->FindBall();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( pTarget )
	{
		D3DXVECTOR3 vAimedDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		D3DXVec3Normalize( &vAimedDir, &vAimedDir );

		fWeight = GetAimedUpDownWeight( vAimedDir, 85.0f );
		m_vAimedDir = vAimedDir;

		if( m_vAimedDir.y >= 0.0f )
		{
			AimedFireToUp( pGrp, fWeight, fTimeRate, 0, true, true );
		}
		else
		{
			AimedFireToDown( pGrp, fWeight, fTimeRate, 0, true, true );
		}

		m_bAimedState = true;
		m_bSetAimedDir = true;
	}
	else if( m_bAimedBall && pBall )
	{
		D3DXVECTOR3 vAimedDir = pBall->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		D3DXVec3Normalize( &vAimedDir, &vAimedDir );

		fWeight = GetAimedUpDownWeight( vAimedDir, 85.0f );
		m_vAimedDir = vAimedDir;

		if( m_vAimedDir.y >= 0.0f )
		{
			AimedFireToUp( pGrp, fWeight, fTimeRate, 0, true, true );
		}
		else
		{
			AimedFireToDown( pGrp, fWeight, fTimeRate, 0, true, true );
		}

		m_bAimedState = true;
		m_bSetAimedDir = true;
	}
	else
	{
		ioHashString szFireUp, szFireCenter;
		if( COMPARE( m_iCurCombo, 0, (int)m_JumpMotionInfoList.size() ) )
		{
			szFireCenter = m_JumpMotionInfoList[m_iCurCombo].m_FireCenter;
			pGrp->SetActionAni( szFireCenter, 0.0f, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0, 0, true );
		}

		m_bAimedState = false;
		m_bSetAimedDir = false;
	}


	// 시간값들 갱신
	m_dwFireStartTime = m_dwRotateStartTime = dwCurTime;
	m_dwFireEndTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwRotateEndTime = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	return true;
}

void ioAutoShotItem::UpdateAimDir( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	ioBiped *pBiped = pGrp->GetBiped("Bip01 Spine1");
	if(!pBiped)	return;

	DWORD dwCurTime = FRAMEGETTIME();
	bool bStateCheck = false;
	switch( m_FireState )
	{
	case FS_AUTO_SHOT:
	case FS_AUTO_SHOT_END:
		bStateCheck = true;
		break;
	}

	switch( m_JumpAutoShotState )
	{
	case JAS_AUTO_SHOT:
		bStateCheck = true;
		break;
	}

	ioBall *pBall = pOwner->FindBall();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( !bStateCheck || (!pTarget && !m_bAimedBall) || !CheckCurrentTargetValid( pOwner, true ) )
	{
		m_fCurAngle = 0.0f;
		pBiped->ReleaseExtraRotation();
		return;
	}

	D3DXVECTOR3 vTargetPos;
	if( pTarget )
	{
		vTargetPos = pTarget->GetWorldPosition();
	}
	else if( m_bAimedBall && pBall )
	{
		vTargetPos = pBall->GetWorldPosition();
	}

	float fRotAngle = 0.0f;

	if( m_FireState == FS_AUTO_SHOT || m_JumpAutoShotState == JAS_AUTO_SHOT )
	{
		D3DXVECTOR3 vSightDir = vTargetPos - pOwner->GetWorldPosition();
		D3DXVec3Normalize( &vSightDir, &vSightDir );

		D3DXVECTOR3 vXZDir( vSightDir.x, 0.0f, vSightDir.z );
		D3DXVec3Normalize( &vXZDir, &vXZDir );

		D3DXQUATERNION qtTargetRot;
		ioMath::CalcDirectionQuaternion( qtTargetRot, -vXZDir );

		float fCurYaw, fTargetYaw;
		fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
		fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
		fCurYaw = RADtoDEG( ioMath::QuaterToYaw(pOwner->GetTargetRot()) );
		fCurYaw = ioMath::ArrangeHead( fCurYaw );

		m_fCurAngle = DEGtoRAD( ioMath::AngleGap( fCurYaw, fTargetYaw ) );

		float fAimTimeRate = FLOAT1;
		DWORD dwTimeGap = dwCurTime - m_dwRotateStartTime;
		if( dwTimeGap < m_dwRotateEndTime )
		{
			fAimTimeRate = (float)dwTimeGap / (float)m_dwRotateEndTime;
		}

		fRotAngle = m_fCurAngle * fAimTimeRate;
	}
	else if( m_FireState == FS_AUTO_SHOT_END )
	{
		float fTotal, fGap;
		fTotal = fGap = 0.0f;

		fTotal = (float)(m_dwRotateReturnEndTime - m_dwRotateReturnStartTime);
		if( m_dwRotateReturnEndTime > dwCurTime )
			fGap = (float)(m_dwRotateReturnEndTime - dwCurTime);

		float fRate = 0.0f;
		if( fTotal > 0.0f )
			fRate = fGap / fTotal;

		fRotAngle = m_fCurAngle * fRate;
	}


	D3DXQUATERNION qtExtraRot;
	D3DXQuaternionRotationYawPitchRoll( &qtExtraRot, 0.0f, fRotAngle, 0.0f );
	pBiped->SetExtraRotation( qtExtraRot, true );
}

float ioAutoShotItem::GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle )
{
	D3DXVECTOR3 vXZDir( vAimDir.x, 0.0f, vAimDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	float fAngle = D3DXVec3Dot( &vXZDir, &vAimDir );
	fAngle = max( -FLOAT1, min( fAngle, FLOAT1 ) );
	fAngle = acosf( fAngle );
	fAngle = RADtoDEG( fAngle );
	fAngle = max( 0.0f, min( fAngle, fMaxAngle ) );

	float fWeight = fAngle / fMaxAngle;

	return fWeight;
}

void ioAutoShotItem::AimedFireToUp( ioEntityGroup *pGrp,
									float fWeight,
									float fTimeRate,
									DWORD dwPreDelay,
									bool bJump,
									bool bHold )
{
	float fFadeOut = 0.0f;

	bool bAniFx1 = false;
	bool bAniFx2 = true;

	if( fWeight > FLOAT05 )
	{
		bAniFx1 = true;
		bAniFx2 = false;
	}

	ioHashString szFireUp, szFireCenter;
	if( bJump )
	{
		if( COMPARE( m_iCurCombo, 0, (int)m_JumpMotionInfoList.size() ) )
		{
			szFireUp = m_JumpMotionInfoList[m_iCurCombo].m_FireUp;
			szFireCenter = m_JumpMotionInfoList[m_iCurCombo].m_FireCenter;
		}
	}
	else
	{
		if( COMPARE( m_iCurCombo, 0, (int)m_MotionInfoList.size() ) )
		{
			szFireUp = m_MotionInfoList[m_iCurCombo].m_FireUp;
			szFireCenter = m_MotionInfoList[m_iCurCombo].m_FireCenter;
		}
	}

	if( fWeight > 0.0f )
	{
		pGrp->SetActionAni( szFireUp,
							0.0f, fFadeOut,
							fWeight, FLOAT1/fTimeRate,
							0.0f, dwPreDelay,
							bHold, bAniFx1 );
	}

	pGrp->SetActionAni( szFireCenter,
						0.0f, fFadeOut,
						1.0f - fWeight, FLOAT1/fTimeRate,
						0.0f, dwPreDelay,
						bHold, bAniFx2 );
}

void ioAutoShotItem::AimedFireToDown( ioEntityGroup *pGrp,
									  float fWeight,
									  float fTimeRate,
									  DWORD dwPreDelay,
									  bool bJump,
									  bool bHold )
{
	float fFadeOut = 0.0f;

	bool bAniFx1 = true;
	bool bAniFx2 = false;
	
	if( fWeight > FLOAT05 )
	{
		bAniFx1 = false;
		bAniFx2 = true;
	}
	
	ioHashString szFireDown, szFireCenter;

	if( bJump )
	{
		if( COMPARE( m_iCurCombo, 0, (int)m_JumpMotionInfoList.size() ) )
		{
			szFireDown = m_JumpMotionInfoList[m_iCurCombo].m_FireDown;
			szFireCenter = m_JumpMotionInfoList[m_iCurCombo].m_FireCenter;
		}
	}
	else
	{
		if( COMPARE( m_iCurCombo, 0, (int)m_MotionInfoList.size() ) )
		{
			szFireDown = m_MotionInfoList[m_iCurCombo].m_FireDown;
			szFireCenter = m_MotionInfoList[m_iCurCombo].m_FireCenter;
		}
	}


	if( fWeight > 0.0f )
	{
		pGrp->SetActionAni( szFireCenter,
							0.0f, fFadeOut,
							1.0f - fWeight, FLOAT1/fTimeRate,
							0.0f, dwPreDelay,
							bHold, bAniFx1 );
	}

	pGrp->SetActionAni( szFireDown,
						0.0f, fFadeOut,
						fWeight, FLOAT1/fTimeRate,
						0.0f, dwPreDelay,
						bHold, bAniFx2 );
}

void ioAutoShotItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	bool bAimJumpState;
	rkPacket >> bAimJumpState;

	int iFireState;
	rkPacket >> iFireState;
	rkPacket >> m_AimedTarget;
	rkPacket >> m_bAimedBall;
	rkPacket >> m_iCurCombo;

	if( bAimJumpState )
	{
		switch( iFireState )
		{
		case JAS_AUTO_SHOT:
			{
				m_JumpAutoShotState = JAS_AUTO_SHOT;
				ApplySetChangeJumpFireMotion( pOwner );
			}
			break;
		}
	}
	else
	{
		switch( iFireState )
		{
		case FS_AUTO_SHOT:
			{
				m_FireState = FS_AUTO_SHOT;
				ApplySetChangeFireMotion( pOwner );
			}
			break;
		case FS_NONE:
			rkPacket >> m_vAimedDir;
			rkPacket >> m_bSetAimedDir;
			rkPacket >> m_bSetSkillAimed;
			break;
		}
	}
}

void ioAutoShotItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	bool bAimJumpState;
	rkPacket >> bAimJumpState;

	int iFireState;
	rkPacket >> iFireState;
	rkPacket >> m_iCurCombo;

	if( bAimJumpState )
	{
		switch( iFireState )
		{
		case JAS_NONE:
			SetNoneState( pOwner );
			break;
		case JAS_NORMAL:
			ChangeToJumpNormalShot( pOwner );
			break;
		case JAS_AUTO_SHOT:
			rkPacket >> m_AimedTarget;
			rkPacket >> m_bAimedBall;
			ApplyChangeJumpAutoShot( pOwner );
			ApplySetChangeJumpFireMotion( pOwner );
			break;
		}
	}
	else
	{
		switch( iFireState )
		{
		case FS_NONE:
			{
				m_FireState = FS_NONE;
				pOwner->SetAttackMoveEnable( false );
				pOwner->SetReloadMoveEnable( false );

				bool bDelay = false;
				rkPacket >> bDelay;

				if( bDelay )
					pOwner->SetState( CS_DELAY );
			}
			break;
		case FS_AUTO_SHOT:
			rkPacket >> m_AimedTarget;
			rkPacket >> m_bAimedBall;
			ApplyChangeAutoShot( pOwner );
			ApplySetChangeFireMotion( pOwner );
			break;
		case FS_AUTO_SHOT_END:
			ChangeToAutoShotEnd( pOwner );
			break;
		case FS_NORMAL_ATTACK:
			ChangeToNormalAttack( pOwner );
			break;
		}
	}
}

void ioAutoShotItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioArrowWeapon *pArrow = ToArrowWeapon( pWeapon );
	ioMissileWeapon *pMissile = ToMissileWeapon( pWeapon );
	ioBombMissileWeapon *pBombMissile = ToBombMissileWeapon( pWeapon );

	// m_bSetAimedDir 해제하기 전에 해야함
	if( pArrow || pMissile || pBombMissile )
	{
		WasteBullet();
	}

	D3DXVECTOR3 vAimedDir = m_vAimedDir;
	if( bJumpAttack )
	{
		if( m_bAimedState )
		{
			vAimedDir = ReCalcMoveDir( pWeapon->GetOwner(), pWeapon );
			pWeapon->SetMoveDir( vAimedDir );
		}

		if( pArrow || pMissile || pBombMissile )
			SetFireEffect( pWeapon->GetOwner(), vAimedDir );

		m_bSetAimedDir = false;
	}
	else
	{
		if( m_bAimedState && m_FireState == FS_AUTO_SHOT )
		{
			vAimedDir = ReCalcMoveDir( pWeapon->GetOwner(), pWeapon );
			pWeapon->SetMoveDir( vAimedDir );
		}

		if( pArrow || pMissile || pBombMissile )
			SetFireEffect( pWeapon->GetOwner(), vAimedDir );

		m_bSetAimedDir = false;
	}
}

ioWeaponItem::WeaponSubType ioAutoShotItem::GetSubType() const
{
	return WST_AUTO_SHOT;
}

void ioAutoShotItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );

	pOwner->SetTargetMarker( ioTargetMarker::MT_BOW_ITEM,
							 m_szReadyMarker, m_szAimingMarker, m_szAimedMarker, m_szZeroAimedMarker, "" );

	pOwner->SetTargetMarkerRange( m_fMarkerRange );
}

void ioAutoShotItem::OnReleased( ioBaseChar *pOwner )
{
	pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "", "" );
	m_fReloadTimeModifyRate = FLOAT1;

	ioWeaponItem::OnReleased( pOwner );
}

bool ioAutoShotItem::IsCanFire( const ioBaseChar* pOwner, int iFireCnt, bool bNormalAttack ) const
{
	if( m_iMaxBullet == 0 )
		return true;

	if( bNormalAttack )
		return true;

	if( m_iCurBullet >= iFireCnt )
		return true;

	return false;
}

void ioAutoShotItem::WasteBullet()
{
	if( m_iMaxBullet == 0 ) return;

	m_iCurBullet--;
	m_iCurBullet = max( m_iCurBullet, 0 );
}

void ioAutoShotItem::SetAimedSkillUse( bool bSkillUsed )
{
	m_bSetSkillAimed = bSkillUsed;
}

void ioAutoShotItem::SetAimedDirSet( bool bAimedSet )
{
	m_bSetAimedDir = bAimedSet;
}

D3DXVECTOR3 ioAutoShotItem::ReCalcMoveDir( ioBaseChar *pOwner, ioWeapon *pWeapon )
{
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( !pTarget && !m_bAimedBall )
		return m_vAimedDir;

	ioBall *pBall = pOwner->FindBall();
	if( !pTarget && !pBall ) return m_vAimedDir;

	D3DXVECTOR3 vTargetPos;
	D3DXVECTOR3 vWeaponPos = pWeapon->GetPosition();

	if( pTarget )
		vTargetPos = pTarget->GetMidPositionByRate( FLOAT08);
	else if( m_bAimedBall && pBall )
		vTargetPos = pBall->GetMidPositionByRate();

	D3DXVECTOR3 vAimedDir =  vTargetPos - vWeaponPos;
	D3DXVec3Normalize( &vAimedDir, &vAimedDir );
	
	vAimedDir.x = m_vAimedDir.x;
	vAimedDir.z = m_vAimedDir.z;

	D3DXVec3Normalize( &vAimedDir, &vAimedDir );

	return vAimedDir;
}

bool ioAutoShotItem::CheckAimState(bool bAimMove)
{
	switch( m_FireState )
	{
	case FS_AUTO_SHOT:
		{
			if( bAimMove )
				return true;

			DWORD dwCurTime = FRAMEGETTIME();
			DWORD dwMotionEndTime = m_dwFireStartTime+m_dwFireEndTime;
			if( m_dwKeyReserveTime > 0 && COMPARE(dwCurTime, m_dwKeyReserveTime, dwMotionEndTime) )
				return true;
		}
		break;
	}

	return false;
}

bool ioAutoShotItem::SetAimState( ioBaseChar *pChar )
{
	return false;
}

void ioAutoShotItem::SetNoneState( ioBaseChar* pOwner, int eNewState /* = 0 */ )
{
	m_AimedTarget.Clear();
	m_bAimedBall = false;

	m_bSetSkillAimed = false;
	m_bSetAimedDir = false;
	m_bAimedState = false;

	m_FireState = FS_NONE;
	m_JumpAutoShotState = JAS_NONE;
}

void ioAutoShotItem::SetJumpAutoShotState( ioBaseChar *pOwner )
{
	m_dwJumpAutoShotReadyStartTime = FRAMEGETTIME();
	m_JumpAutoShotState = JAS_READY;
}

bool ioAutoShotItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_FireState == FS_CHARGING ||
			m_FireState == FS_AUTO_SHOT )
			return true;
	}

	return false;
}

void ioAutoShotItem::CheckAimMoveAniState( ioBaseChar *pOwner )
{
	if( !pOwner->IsCanAttackMove() )	return;
	if( !COMPARE( m_iCurCombo, 0, (int)m_MotionInfoList.size() ) ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	if( pOwner->IsSettedDirection() )
	{
		if( pOwner->IsChangeDirection() || m_bFirstAimMoveMotion )
		{
			pGrp->ClearAllLoopAni( FLOAT10 );
			
			m_bAimMotionSetted = false;
			m_bFirstAimMoveMotion = false;

			ioUserKeyInput::DirKeyInput eXformDir;
			eXformDir = pOwner->GetXformDir( m_AimStartDirKey );

			switch( eXformDir )
			{
			case ioUserKeyInput::DKI_UP:
				pGrp->SetLoopAni( m_MotionInfoList[m_iCurCombo].m_AimMoveFront, FLOAT100, FLOAT1, m_fAimMoveAniRate );
				break;
			case ioUserKeyInput::DKI_RIGHTUP:
			case ioUserKeyInput::DKI_RIGHT:
			case ioUserKeyInput::DKI_RIGHTDOWN:
				pGrp->SetLoopAni( m_MotionInfoList[m_iCurCombo].m_AimMoveRight, FLOAT100, FLOAT1, m_fAimMoveAniRate );
				break;
			case ioUserKeyInput::DKI_DOWN:
				pGrp->SetLoopAni( m_MotionInfoList[m_iCurCombo].m_AimMoveBack, FLOAT100, FLOAT1, m_fAimMoveAniRate );
				break;
			case ioUserKeyInput::DKI_LEFTDOWN:
			case ioUserKeyInput::DKI_LEFT:
			case ioUserKeyInput::DKI_LEFTUP:
				pGrp->SetLoopAni( m_MotionInfoList[m_iCurCombo].m_AimMoveLeft, FLOAT100, FLOAT1, m_fAimMoveAniRate );
				break;
			}
			g_TutorialMgr.ActionAimingMove();
		}
	}
	else
	{
		if( !m_bAimMotionSetted )
		{
			pGrp->ClearAllLoopAni( FLOAT10 );

			m_bAimMotionSetted = true;
		}
	}
}

void ioAutoShotItem::OnEndExtendDefense( ioBaseChar *pOwner )
{
	if( !m_ReloadEffect.IsEmpty() )
	{
		pOwner->EndEffect( m_ReloadEffect, false );
		pOwner->ShowWeaponItemMesh( true );
	}
}

void ioAutoShotItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	if( m_bAimEffect )
	{
		if( bReleaseItem )
		{
			m_bAimEffect = false;
			pOwner->EndEffect( m_AimEffect, false );
		}
	}

	SetNoneState( pOwner );
}

const ioHashString& ioAutoShotItem::GetJumppingAnimation() const
{
	bool bAim = false;
	switch( m_FireState )
	{
	case FS_AUTO_SHOT:
		bAim = true;
		break;
	}

	return ioAttackableItem::GetJumppingAnimation();
}

bool ioAutoShotItem::CheckLanding( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	pGrp->ClearAllLoopAni( 0.0f );

	pChar->SetTargetMarkerState( ioTargetMarker::MS_NONE );
	pChar->GetGroup()->ReleaseAllExtraRotate();

	SetNoneState( pChar );
	
	return true;
}

bool ioAutoShotItem::CheckExtendDefenseEnable( ioBaseChar *pChar )
{
	if( m_iCurBullet == m_iMaxBullet)
	{
		return false;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_ReloadAnimation );
	if( iAniID == -1 )
	{
		return false;
	}

	return true;
}

void ioAutoShotItem::CheckDefenseMoveAniState( ioBaseChar *pOwner )
{
	if( !pOwner->IsCanReloadMove() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	if( pOwner->IsSettedDirection() )
	{
		if( !m_bSetReloadMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx(m_ReloadMoveAnimation);
			if( iAniID == -1 )
				return;

			DWORD dwPlayTime = 0;
			ioAnimation *pAni = pGrp->GetLoopInPlay( m_ReloadAnimation );
			
			if( pAni )
				dwPlayTime = pAni->GetCurTime();

			pGrp->ClearAllActionAni( FLOAT100, false );
			pGrp->ClearAllLoopAni( FLOAT100, false );
			pGrp->SetLoopAniInMid( iAniID, dwPlayTime, FLOAT100, FLOAT1, FLOAT1/m_fReloadTimeModifyRate );
			m_bSetReloadMoveAni = true;
		}
	}
	else
	{
		if( m_bSetReloadMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx(m_ReloadAnimation);
			if( iAniID == -1 )
				return;

			DWORD dwPlayTime = 0;
			ioAnimation *pAni = pGrp->GetLoopInPlay( m_ReloadMoveAnimation );
			
			if( pAni )
				dwPlayTime = pAni->GetCurTime();

			pGrp->ClearAllActionAni( FLOAT100, false );
			pGrp->ClearAllLoopAni( FLOAT100, false );
			pGrp->SetLoopAniInMid( iAniID, dwPlayTime, FLOAT100, FLOAT1, FLOAT1/m_fReloadTimeModifyRate );
			m_bSetReloadMoveAni = false;
		}
	}
}

void ioAutoShotItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	int iBullet = min( iCurBullet, m_iMaxBullet );
	iBullet = max( 0, iBullet );

	m_iCurBullet = iBullet;
}

float ioAutoShotItem::GetAttackSpeedRate() const
{
	return m_fAttackSpeedRate;
}

bool ioAutoShotItem::IsAimedState()
{
	if( m_FireState == FS_AUTO_SHOT )
		return true;

	return false;
}

void ioAutoShotItem::ApplyChangeAutoShot( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	pOwner->StopAutoTargetTracking();

	DWORD dwCurTime = FRAMEGETTIME();

	m_FireState = FS_AUTO_SHOT;
	pOwner->SetAttackMoveEnable( true );
	pOwner->ChangeDirectionExplicitIfDirChanged( true );

	m_AimStartDirKey = pOwner->GetCurDirKey();

	m_bFirstAimMoveMotion = true;
	m_bAimMotionSetted = true;

	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	m_vStartDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	m_bSetAimedDir = false;
	m_bAimedState = false;
	m_iCurCombo = 0;

	if( !m_bAimEffect )
	{
		pOwner->AttachEffect( m_AimEffect );
		m_bAimEffect = true;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
		pGrp->ClearAllLoopAni( FLOAT10, true );
}

void ioAutoShotItem::ApplySetChangeFireMotion( ioBaseChar *pOwner )
{
	if( !COMPARE( m_iCurCombo, 0, (int)m_vExtendAttributeList.size() ) )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );

	if( !m_AimedTarget.IsEmpty() || m_bAimedBall )
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
	else
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
	

	m_dwTrackingEndTime = dwCurTime;

	// FireDir 처리
	// 발사 모션 처리
	int	  iAniID = -1;
	float fTimeRate = FLOAT1;
	float fWeight = 0.0f;

	pGrp->ClearAllActionAni( FLOAT10, true );

	const AttackAttribute *pAttr = &m_vExtendAttributeList[m_iCurCombo];
	if( pAttr )
	{
		iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
		fTimeRate = pAttr->m_fAttackAniRate;

		pOwner->RefreshFireTimeList( iAniID, pAttr->m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
		pOwner->IncreaseWeaponIndexBase();
	}

	ioBall *pBall = pOwner->FindBall();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );

	if( pTarget )
	{
		D3DXVECTOR3 vAimedDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		D3DXVec3Normalize( &vAimedDir, &vAimedDir );

		fWeight = GetAimedUpDownWeight( vAimedDir, 85.0f );
		m_vAimedDir = vAimedDir;

		if( m_vAimedDir.y >= 0.0f )
		{
			AimedFireToUp( pGrp, fWeight, fTimeRate, 0, false, true );
		}
		else
		{
			AimedFireToDown( pGrp, fWeight, fTimeRate, 0, false, true );
		}

		m_bAimedState = true;
		m_bSetAimedDir = true;
	}
	else if( m_bAimedBall &&  pBall )
	{
		D3DXVECTOR3 vAimedDir = pBall->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		D3DXVec3Normalize( &vAimedDir, &vAimedDir );

		fWeight = GetAimedUpDownWeight( vAimedDir, 85.0f );
		m_vAimedDir = vAimedDir;

		if( m_vAimedDir.y >= 0.0f )
		{
			AimedFireToUp( pGrp, fWeight, fTimeRate, 0, false, true );
		}
		else
		{
			AimedFireToDown( pGrp, fWeight, fTimeRate, 0, false, true );
		}

		m_bAimedState = true;
		m_bSetAimedDir = true;
	}
	else
	{
		ioHashString szFireUp, szFireCenter;
		if( COMPARE( m_iCurCombo, 0, (int)m_MotionInfoList.size() ) )
		{
			szFireCenter = m_MotionInfoList[m_iCurCombo].m_FireCenter;
			pGrp->SetActionAni( szFireCenter, 0.0f, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0, 0, true );
		}

		m_bAimedState = false;
		m_bSetAimedDir = false;
	}


	// 시간값들 갱신
	m_dwFireStartTime = m_dwRotateStartTime = dwCurTime;
	m_dwFireEndTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwRotateEndTime = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwKeyReserveTime = m_dwFireStartTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	return;
}

void ioAutoShotItem::ApplyChangeJumpAutoShot( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	pOwner->StopAutoTargetTracking();

	DWORD dwCurTime = FRAMEGETTIME();

	m_JumpAutoShotState = JAS_AUTO_SHOT;
	pOwner->ChangeDirectionExplicitIfDirChanged( true );

	m_AimStartDirKey = pOwner->GetCurDirKey();

	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	m_vStartDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	m_bSetAimedDir = false;
	m_bAimedState = false;
	m_iCurCombo = 0;

	if( !m_bAimEffect )
	{
		pOwner->AttachEffect( m_AimEffect );
		m_bAimEffect = true;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
		pGrp->ClearAllLoopAni( FLOAT10, true );
}

void ioAutoShotItem::ApplySetChangeJumpFireMotion( ioBaseChar *pOwner )
{
	if( !COMPARE( m_iCurCombo, 0, (int)m_vJumpAttributeList.size() ) )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );

	if( !m_AimedTarget.IsEmpty() || m_bAimedBall )
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
	else
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );

	m_dwTrackingEndTime = dwCurTime;

	// FireDir 처리
	// 발사 모션 처리
	int	  iAniID = -1;
	float fTimeRate = FLOAT1;
	float fWeight = 0.0f;

	pGrp->ClearAllActionAni( 0.0f, true );

	const AttackAttribute *pAttr = &m_vJumpAttributeList[m_iCurCombo];
	if( pAttr )
	{
		iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
		fTimeRate = pAttr->m_fAttackAniRate;

		pOwner->RefreshFireTimeList( iAniID, pAttr->m_vWeaponInfoList, FTT_JUMP_ATTACK, fTimeRate, 0 );
		pOwner->IncreaseWeaponIndexBase();

		// AirJumpPower
		SetAirJump( pOwner, iAniID, fTimeRate );
	}

	ioBall *pBall = pOwner->FindBall();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( pTarget )
	{
		D3DXVECTOR3 vAimedDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		D3DXVec3Normalize( &vAimedDir, &vAimedDir );

		fWeight = GetAimedUpDownWeight( vAimedDir, 85.0f );
		m_vAimedDir = vAimedDir;

		if( m_vAimedDir.y >= 0.0f )
		{
			AimedFireToUp( pGrp, fWeight, fTimeRate, 0, true, true );
		}
		else
		{
			AimedFireToDown( pGrp, fWeight, fTimeRate, 0, true, true );
		}

		m_bAimedState = true;
		m_bSetAimedDir = true;
	}
	else if( m_bAimedBall && pBall )
	{
		D3DXVECTOR3 vAimedDir = pBall->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		D3DXVec3Normalize( &vAimedDir, &vAimedDir );

		fWeight = GetAimedUpDownWeight( vAimedDir, 85.0f );
		m_vAimedDir = vAimedDir;

		if( m_vAimedDir.y >= 0.0f )
		{
			AimedFireToUp( pGrp, fWeight, fTimeRate, 0, true, true );
		}
		else
		{
			AimedFireToDown( pGrp, fWeight, fTimeRate, 0, true, true );
		}

		m_bAimedState = true;
		m_bSetAimedDir = true;
	}
	else
	{
		ioHashString szFireUp, szFireCenter;
		if( COMPARE( m_iCurCombo, 0, (int)m_JumpMotionInfoList.size() ) )
		{
			szFireCenter = m_JumpMotionInfoList[m_iCurCombo].m_FireCenter;
			pGrp->SetActionAni( szFireCenter, 0.0f, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0, 0, true );
		}

		m_bAimedState = false;
		m_bSetAimedDir = false;
	}


	// 시간값들 갱신
	m_dwFireStartTime = m_dwRotateStartTime = dwCurTime;
	m_dwFireEndTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwRotateEndTime = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	return;
}

bool ioAutoShotItem::CheckCurrentTargetValid( ioBaseChar *pOwner, bool bAngleAlsoCheck )
{
	ioBall *pBall = pOwner->FindBall();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( !pTarget && !m_bAimedBall )
		return false;
	else if( !pTarget && !pBall )
	{
		m_bAimedBall = false;
		return false;
	}

	D3DXVECTOR3 vTargetPos;
	if( pTarget )
	{
		if( !pTarget->IsEnableAimTarget(m_bEnableDownAim) )
			return false;

		if( pTarget->HasHideBuff() )
			return false;

		if( pTarget->IsChatModeState( false ) )
			return false;

		vTargetPos = pTarget->GetWorldPosition();
	}
	else if( m_bAimedBall && pBall )
	{
		vTargetPos = pBall->GetWorldPosition();
	}


	D3DXVECTOR3 vDiff = vTargetPos - pOwner->GetWorldPosition();
	if( D3DXVec3LengthSq( &vDiff ) > m_fAimRange * m_fAimRange )
		return false;

	if( D3DXVec3LengthSq( &vDiff ) < m_fMinRange * m_fMinRange )
		return false;

	if( bAngleAlsoCheck )
	{
		vDiff.y = 0.0f;
		D3DXVec3Normalize( &vDiff, &vDiff );

		float fHalfCosine = cosf( DEGtoRAD( m_fAimAngle * FLOAT05 ) );
		if( D3DXVec3Dot( &m_vStartDir, &vDiff ) < fHalfCosine )
			return false;
	}

	return true;
}

void ioAutoShotItem::SetFireEffect( ioBaseChar *pOwner, const D3DXVECTOR3 &vDir )
{
	if( !pOwner ) return;
	
	int iSize = m_FireEffectList.size();
	if( iSize == 0 ) return;

	if( !COMPARE( m_iCurCombo, 0, iSize ) )
		return;

	ioHashString szCurEffect = m_FireEffectList[m_iCurCombo];
	if( m_bAimedState )
	{
		pOwner->CreateMapEffectWithDir( szCurEffect, vDir );
	}
	else
	{
		D3DXVECTOR3 vNormalDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		pOwner->CreateMapEffectWithDir( szCurEffect, vNormalDir );
	}
}

bool ioAutoShotItem::CompareAimedTarget( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	ioBall *pBall = pOwner->FindBall();
	if( !pBall ) return false;

	ioBaseChar *pChar = pOwner->GetBaseChar( m_AimedTarget );

	D3DXVECTOR3 vDiff;
	D3DXVECTOR3 vOwnerPos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vCharDir  = pOwner->GetMoveDir();

	float fHalfCosine = cosf( DEGtoRAD( m_fAimAngle ) * FLOAT05 );

	float fLimitDistSq = m_fAimRange * m_fAimRange;
	float fMinDistSq = m_fMinRange * m_fMinRange;

	float fBallDist = std::numeric_limits<float>::infinity();
	float fCharDist = std::numeric_limits<float>::infinity();

	// Ball
	if( pBall )
	{
		vDiff = pBall->GetWorldPosition() - vOwnerPos;
		float fCurDistSq = D3DXVec3LengthSq( &vDiff );

		if( (fLimitDistSq >= fCurDistSq) && (fMinDistSq <= fCurDistSq) )
		{
			vDiff.y = 0.0f;
			D3DXVec3Normalize( &vDiff, &vDiff );

			if( D3DXVec3Dot( &vCharDir, &vDiff ) >= fHalfCosine )
				return true;
		}
	}

	/*
	// Char
	if( pChar )
	{
		vDiff = pChar->GetWorldPosition() - vOwnerPos;
		float fCurDistSq = D3DXVec3LengthSq( &vDiff );

		if( (fLimitDistSq >= fCurDistSq) && (fMinDistSq <= fCurDistSq) )
		{
			vDiff.y = 0.0f;
			D3DXVec3Normalize( &vDiff, &vDiff );

			if( D3DXVec3Dot( &vCharDir, &vDiff ) >= fHalfCosine )
				fCharDist = fCurDistSq;
		}
	}

	if( fBallDist < fCharDist )
	{
		return true;
	}
	*/

	return false;
}

bool ioAutoShotItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	switch( m_FireState )
	{
	case FS_AUTO_SHOT:
		{
			DWORD dwCurTime = FRAMEGETTIME();
			DWORD dwMotionEndTime = m_dwFireStartTime+m_dwFireEndTime;
			if( m_dwKeyReserveTime > 0 && COMPARE(dwCurTime, m_dwKeyReserveTime, dwMotionEndTime) )
				return true;
		}
		break;
	}

	return false;
}

