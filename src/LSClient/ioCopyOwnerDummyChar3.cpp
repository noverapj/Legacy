
#include "stdafx.h"

#include "ioCopyOwnerDummyChar3.h"

#include "ioGaugeHelper.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioAniCollisionBoxGrp.h"
#include "ioWoundedCollisionBoxGrp.h"
#include "ioCollisionBoxGrpManager.h"

#include "WeaponDefine.h"

#include "ioGuildMarkMgr.h"

#include "GUI/LadderTeamWnd.h"

ioCopyOwnerDummyChar3::ioCopyOwnerDummyChar3( ioEntityGroup *pGrp, ioPlayMode *pMode )
	: ioDummyChar( pGrp, pMode )
{
	m_iFloatingWoundedCnt = 0;
	m_fCurGravityGapAmt = 0;

	m_fBlowGetUpTimeRate = FLOAT1;
	m_dwCurBlowGetUpTime = 0;
	m_dwMaxBlowWoundedGetUpTime = 0;

	m_bJumpedBlowWounded = false;
	m_bFallBlowWounded = false;
	m_bBlowEndAni = false;

	m_dwBlowGetUpCheckStartTime = 0;
	m_dwBlowAniEndTime = 0;
	m_dwGetUpDurationTime = 0;

	m_dwLastApplyWeaponIdx = 0;
	m_dwLastApplyWeaponTime = 0;
}

ioCopyOwnerDummyChar3::~ioCopyOwnerDummyChar3()
{
}

void ioCopyOwnerDummyChar3::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	m_dwGeneralBlowWoundedGetUpTime = rkLoader.LoadInt_e( "blow_wounded_getup_time_general", 1000 );
	m_dwExplosionWaitTime = rkLoader.LoadInt_e ("explosion_wait_time", 1000 );
	m_dwExplosionTime = rkLoader.LoadInt_e("explosion_time", 1000 );

	rkLoader.LoadString_e( "explosion_wait_effect", "", szBuf, MAX_PATH );
	m_szExplosionWaitEffect = szBuf;

	rkLoader.LoadString_e( "explosion_effect", "", szBuf, MAX_PATH );
	m_ExplosionEffect = szBuf;

	rkLoader.LoadString_e( "explosion_sound", "", szBuf, MAX_PATH );
	m_ExplosionSound = szBuf;


	m_WeaponOnWound.m_iWeaponIdx        = rkLoader.LoadInt_e( "explosion_attack_index", NO_WEAPON_ATTRIBUTE );
	m_WeaponOnWound.m_iResistanceIdx    = rkLoader.LoadInt_e( "explosion_attack_index_resist", NO_WEAPON_ATTRIBUTE );
	rkLoader.LoadString_e( "explosion_wounded", "", szBuf, MAX_PATH );
	m_WeaponOnWound.m_WoundedAnimation  = szBuf;
	m_WeaponOnWound.m_dwWoundedDuration = rkLoader.LoadInt_e( "explosion_wounded_time", 0 );
	m_WeaponOnWound.m_bWoundedLoopAni   = rkLoader.LoadBool_e( "explosion_wounded_loop_ani", false );

	m_fTargetTraceExplosionRange = rkLoader.LoadInt_e( "target_tarce_explosion_range", 0.f );

	m_dwMaxLifeTime = rkLoader.LoadInt_e( "max_life_time", 0 );

	rkLoader.LoadString_e( "same_team_effect", "", szBuf, MAX_PATH );
	m_stTeamEffect = szBuf;
}

void ioCopyOwnerDummyChar3::LoadPropertyByChar()
{
	// DCT_COPY_OWER
	ioINILoader_e kLoader( "config/sp2_char.ini" );
	kLoader.SetTitle_e( "common" );
	char szBuf[MAX_PATH] = "";

	m_fWeakAttackWoundedRate = kLoader.LoadFloat_e( "weak_stiff_rate", FLOAT1 );
	m_fMidStiffWoundedRate   = kLoader.LoadFloat_e( "mid_stiff_rate", 2.0f );
	m_fStrStiffWoundedRate   = kLoader.LoadFloat_e( "str_stiff_rate", 4.0f );
	m_fStandKoWoundedRate    = kLoader.LoadFloat_e( "stand_ko_rate", FLOAT1 );
	m_fStandKo2WoundedRate   = kLoader.LoadFloat_e( "stand_ko2_rate", FLOAT1 );

	m_fKOActionBlowPower = kLoader.LoadFloat_e( "ko_action_blow_power", 300.0f );
	m_fFloatingBlowDownRate = kLoader.LoadFloat_e( "floating_blow_down_rate", 0.0f );

	m_fForcePowerAddRate = kLoader.LoadFloat_e( "force_power_add_rate", FLOAT05 );

	kLoader.SetTitle_e( "character1" );
	kLoader.LoadString_e( "weak_wounded_animation", "", szBuf, MAX_PATH );
	m_WeakWounded = szBuf;
	kLoader.LoadString_e( "mid_stiff_wounded_animation", "", szBuf, MAX_PATH );
	m_MidStiffWounded = szBuf;
	kLoader.LoadString_e( "str_stiff_wounded_animation", "", szBuf, MAX_PATH );
	m_StrStiffWounded = szBuf;

	kLoader.LoadString_e( "back_weak_wounded_animation", "", szBuf, MAX_PATH );
	m_BackWeakWounded = szBuf;
	kLoader.LoadString_e( "back_mid_stiff_wounded_animation", "", szBuf, MAX_PATH );
	m_BackMidStiffWounded = szBuf;
	kLoader.LoadString_e( "back_str_stiff_wounded_animation", "", szBuf, MAX_PATH );
	m_BackStrStiffWounded = szBuf;

	kLoader.SetTitle_e( "animation" );
	kLoader.LoadString_e( "ani_blow_push", "public_push-m.ani", szBuf, MAX_PATH );
	m_AniBlowPush = szBuf;
	kLoader.LoadString_e( "ani_blow_getup", "public_getup_01_fr.ani", szBuf, MAX_PATH );
	m_AniBlowGetUp = szBuf;
	kLoader.LoadString_e( "ani_back_blow_getup", "public_getup_01_bk.ani", szBuf, MAX_PATH );
	m_AniBackBlowGetUp = szBuf;

	kLoader.LoadString_e( "ani_stand_ko_action", "public_ko_action_03_fr.ani", szBuf, MAX_PATH );
	m_AniStandKoAction = szBuf;
	kLoader.LoadString_e( "ani_back_stand_ko_action", "public_ko_action_02_fr.ani", szBuf, MAX_PATH );
	m_AniBackStandKoAction = szBuf;
	kLoader.LoadString_e( "ani_stand_ko_down", "public_ko_down_03_fr.ani", szBuf, MAX_PATH );
	m_AniStandKoDown = szBuf;
	kLoader.LoadString_e( "ani_back_stand_ko_down", "public_ko_down_03_fr.ani", szBuf, MAX_PATH );
	m_AniBackStandKoDown = szBuf;

	kLoader.LoadString_e( "ani_stand_ko2_action", "public_ko_action_03_fr.ani", szBuf, MAX_PATH );
	m_AniStandKo2Action = szBuf;
	kLoader.LoadString_e( "ani_back_stand_ko2_action", "public_ko_action_02_fr.ani", szBuf, MAX_PATH );
	m_AniBackStandKo2Action = szBuf;
	kLoader.LoadString_e( "ani_stand_ko2_down", "public_ko_down_03_fr.ani", szBuf, MAX_PATH );
	m_AniStandKo2Down = szBuf;
	kLoader.LoadString_e( "ani_back_stand_ko2_down", "public_ko_down_03_fr.ani", szBuf, MAX_PATH );
	m_AniBackStandKo2Down = szBuf;

	kLoader.LoadString_e( "ani_ko_action", "public_ko_down_01_fr.ani", szBuf, MAX_PATH );
	m_AniKoAction = szBuf;
	kLoader.LoadString_e( "ani_back_ko_action", "public_ko_down_01_bk.ani", szBuf, MAX_PATH );
	m_AniBackKoAction = szBuf;
	kLoader.LoadString_e( "ani_down_ko_action", "public_ko_down_02_fr.ani", szBuf, MAX_PATH );
	m_AniDownKoAction = szBuf;
	kLoader.LoadString_e( "ani_backdown_ko_action", "public_ko_down_02_bk.ani", szBuf, MAX_PATH );
	m_AniBackdownKoAction = szBuf;
	kLoader.LoadString_e( "ani_jump_ko_action", "public_ko_action_02_fr.ani", szBuf, MAX_PATH );
	m_AniJumpKoAction = szBuf;
	kLoader.LoadString_e( "ani_backjump_ko_action", "public_ko_action_02_bk.ani", szBuf, MAX_PATH );
	m_AniBackJumpKoAction = szBuf;

	kLoader.LoadString_e( "ani_ko_action2", "public_ko_down_01_fr.ani", szBuf, MAX_PATH );
	m_AniKoAction2 = szBuf;
	kLoader.LoadString_e( "ani_back_ko_action2", "public_ko_down_01_bk.ani", szBuf, MAX_PATH );
	m_AniBackKoAction2 = szBuf;
	kLoader.LoadString_e( "ani_down_ko_action2", "public_ko_down_02_fr.ani", szBuf, MAX_PATH );
	m_AniDownKoAction2 = szBuf;
	kLoader.LoadString_e( "ani_backdown_ko_action2", "public_ko_down_02_bk.ani", szBuf, MAX_PATH );
	m_AniBackdownKoAction2 = szBuf;

	kLoader.LoadString_e( "ani_ko_action3", "public_ko_down_01_fr.ani", szBuf, MAX_PATH );
	m_AniKoAction3 = szBuf;
	kLoader.LoadString_e( "ani_back_ko_action3", "public_ko_down_01_bk.ani", szBuf, MAX_PATH );
	m_AniBackKoAction3 = szBuf;
	kLoader.LoadString_e( "ani_down_ko_action3", "public_ko_down_02_fr.ani", szBuf, MAX_PATH );
	m_AniDownKoAction3 = szBuf;
	kLoader.LoadString_e( "ani_backdown_ko_action3", "public_ko_down_02_bk.ani", szBuf, MAX_PATH );
	m_AniBackdownKoAction3 = szBuf;

	kLoader.LoadString_e( "ani_fall_down_action", "public_ko_down_02_fr.ani", szBuf, MAX_PATH );
	m_AniFallDownAction = szBuf;
	kLoader.LoadString_e( "ani_fall_back_down_action", "public_ko_down_02_bk.ani", szBuf, MAX_PATH );
	m_AniFallBackDownAction = szBuf;

	// BoundBlow
	kLoader.LoadString_e( "ani_bound_blow_action", "", szBuf, MAX_PATH );
	m_AniBoundBlowAction = szBuf;
	kLoader.LoadString_e( "ani_bound_blow_back_action", "", szBuf, MAX_PATH );
	m_AniBoundBlowBackAction = szBuf;
	kLoader.LoadString_e( "ani_bound_blow2_action", "", szBuf, MAX_PATH );
	m_AniBoundBlowAction2 = szBuf;
	kLoader.LoadString_e( "ani_bound_blow2_back_action", "", szBuf, MAX_PATH );
	m_AniBoundBlowBackAction2 = szBuf;
	kLoader.LoadString_e( "ani_bound_down_action", "", szBuf, MAX_PATH );
	m_AniBoundDownAction = szBuf;
	kLoader.LoadString_e( "ani_bound_down_back_action", "", szBuf, MAX_PATH );
	m_AniBoundDownBackAction = szBuf;
}

void ioCopyOwnerDummyChar3::SetCopyOwnerInfo( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage* pStage = pOwner->GetCreator();
	ioBaseChar *pMyChar = NULL;
	if( !pStage )
		return;

	pMyChar = pStage->GetOwnerChar();
	if( !pMyChar )
		return;

	ioEquipSlot *pEquip = pOwner->GetEquipSlot();
	if( pEquip )
	{
		ioWeaponItem *pItem = pEquip->GetWeapon();
		if( pItem )
		{
			m_DelayAnimation = Help::ConvertTailName( pItem->GetDelayAnimation(), pOwner->IsMale() );
			char szFullNameDelay[MAX_PATH]="";
			StringCbPrintf( szFullNameDelay, sizeof( szFullNameDelay ), "resource\\animation\\%s" , m_DelayAnimation.c_str() );
			if( !g_ResourceLoader.IsFileExist( szFullNameDelay ) )
			{
				m_DelayAnimation = Help::ConvertTailName( pItem->GetDelayAnimation(), true ); // 남자로
			}

			//////////////////////////////////////////////////////////////////////////
			m_MoveAnimation = Help::ConvertTailName( pItem->GetRunAnimation(), pOwner->IsMale() );
			char szFullNameMove[MAX_PATH]="";
			StringCbPrintf( szFullNameMove, sizeof( szFullNameMove ), "resource\\animation\\%s" , m_MoveAnimation.c_str() );
			if( !g_ResourceLoader.IsFileExist( szFullNameMove ) )
			{
				m_MoveAnimation = Help::ConvertTailName( pItem->GetRunAnimation(), true ); // 남자로
			}
		}
	}

	m_HP.m_fMaxValue = pOwner->GetHP().m_fMaxValue;
	m_HP.m_fCurValue = pOwner->GetHP().m_fCurValue;

	// 팀셋팅
	TeamType eCurTeamType = pOwner->GetTeam();
	TeamType eCurSingleTeamType = pOwner->GetSingleConvertTeam();

	switch( eCurTeamType )
	{
	case TEAM_BLUE:
	case TEAM_RED:
		m_pGauge->SetTeam( eCurTeamType );
		break;
	default:
		m_pGauge->SetTeam( eCurSingleTeamType );
		break;
	}

	ioEffect* pEffect = NULL;
	//팀이 같으면 이펙트를 뿌려 주자
	if( pMyChar->GetTeam() == pOwner->GetTeam() )
		pEffect = AttachEffect( m_stTeamEffect, NULL );
	

	m_ArmorType = pOwner->GetArmorType();
	SetDummyState( DCS_DELAY );

	m_ExplosionState = ES_NONE;
}


bool ioCopyOwnerDummyChar3::IsCollisionAvailableWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon ) return false;

	if( m_State == DCS_DIE )
		return false;

	if( !IsCollisionAvailableRange( pWeapon ) )
		return false;

	if( pWeapon->IsAlreadyCollisioned( this ) )
		return false;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return false;

	ioBaseChar *pWeaponOwner = pWeapon->GetOwner();
	if( !pWeaponOwner )
		return false;

	if( pOwner->GetTeam() == pWeaponOwner->GetTeam() )
		return false;

	if( (m_State == DCS_BLOW_WOUNDED && m_dwBlowAniEndTime == 0) ||
		(m_State == DCS_BOUND_BLOW_WOUNDED && m_dwBlowAniEndTime == 0) )
	{
		if( !pWeapon->HasFeature( WF_DOWN_ATTACK_WEAPON ) )
			return false;
	}
	else	// 쓰러짐 상태가 아닌경우...
	{
		if( pWeapon->IsOnlyDownAttack() )
		{
			return false;
		}
	}

	int iIgnoreType = m_WeaponIgnoreInfo.m_iWeaponType;
	DWORD dwIgnoreTime = m_WeaponIgnoreInfo.m_dwIgnoreTime;
	if( iIgnoreType == pWeapon->GetType() )
	{
		if( dwIgnoreTime > 0 && dwIgnoreTime > FRAMEGETTIME() )
		{
			return false;
		}
	}

	// last applyweapon check
	int iAttribute = pWeapon->GetAttributeIndex();
	if( (m_dwLastApplyWeaponIdx == iAttribute) && m_dwLastApplyWeaponTime > FRAMEGETTIME() )
	{
		return false;
	}

	return true;
}

bool ioCopyOwnerDummyChar3::IsCharCollisionSkipState( TeamType eTeamType, int iState ) const
{
	if( m_CharColSkip == CCT_ALL )
	{
		return true;
	}

	if( (m_State == DCS_BLOW_WOUNDED && m_dwBlowAniEndTime == 0) ||
		(m_State == DCS_BOUND_BLOW_WOUNDED && m_dwBlowAniEndTime == 0) )
		return true;

	if( m_CharColSkip == CCT_TEAM )
	{
		if( GetTeam() == eTeamType )
		{
			return true;
		}
	}

	return false;
}

bool ioCopyOwnerDummyChar3::IsDummyCharCollisionSkipState( ioBaseChar *pChar )
{
	bool bSkip = ioDummyChar::IsDummyCharCollisionSkipState( pChar );
	if( bSkip )
		return true;

	return false;
}

void ioCopyOwnerDummyChar3::RenderGauge()
{
	if( !m_pGauge || !m_bGaugeVisible || m_bDisableHPVisible ) return;

	bool bCheckMode = g_GUIMgr.IsNoneUIMode();
	ioPlayStage *pStage = m_pModeParent->GetCreator();
	if( pStage && !bCheckMode )
	{
		if( pStage->HasCameraFixedStyle(CAMERA_STYLE_MOVIE) || pStage->HasCameraFixedStyle(CAMERA_STYLE_VIEW) )
			bCheckMode = true;
	}

	if( bCheckMode && !Setting::NoUIModeHPShow() ) return;

	D3DXVECTOR3 vPos = GetMidPositionByRate( FLOAT1 );
	vPos.y += 40.0f;

	int iXPos = 0, iYPos = 0;
	if( Setting::Check2DInScreen( vPos, iXPos, iYPos ) )
	{
		RenderGaugeOwnerInfo();
	}
}

void ioCopyOwnerDummyChar3::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_ExplosionState == ES_NONE )
	{
		switch( m_State )
		{
		case DCS_START:
			ProcessStartState( fTimePerSec );
			break;
		case DCS_DELAY:
			if( m_dwDelayStartTime > 0 && m_dwDelayStartTime+m_dwDelayDuration < dwCurTime )
			{
				m_dwDelayStartTime = 0;
				SetTraceState( DCTS_SENSING, false );
			}
			break;
		case DCS_TRACE:
			ProcessTraceState( fTimePerSec, dwPreTime );
			break;
		}
	}

	ProcessExplosionState();
	ProcessPos( fTimePerSec );

	if( m_pModeParent && m_pModeParent->IsZeroHP() )
	{
		if( m_HP.m_fCurValue > 0.0f )
			m_HP.m_fCurValue = 0.0f;
	}


	if( m_ExplosionState == ES_NONE )
	{
		if( (m_dwCreateTime + m_dwMaxLifeTime) < dwCurTime )
		{
			SetExplosionWaitState( false, NULL );
			return;
		}
	}
}


void ioCopyOwnerDummyChar3::SetTraceState( DummyCharTraceState iTraceState, bool bAniDelay )
{
	m_State = DCS_TRACE;
	m_TraceState = iTraceState;

	m_dwSensingStartTime = FRAMEGETTIME();

	if( bAniDelay )
	{
		int iAniID = m_pGroup->GetAnimationIdx( m_DelayAnimation );
		if( iAniID != -1 )
		{
			m_pGroup->ClearAllLoopAniExceptOne( iAniID, FLOAT100, true );
			m_pGroup->ClearAllActionAni( FLOAT100, true );
			m_pGroup->SetLoopAni( iAniID, FLOAT100 );
		}
	}

	//LOG.PrintConsoleLog( 0, "Dummy3(SetTrace) - %s", m_TargetTraceName );
}


void ioCopyOwnerDummyChar3::ProcessTraceState( float fTimePerSec, DWORD dwPreTime )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_TraceState )
	{
	case DCTS_NONE:
		break;
	case DCTS_SENSING:
		ProcessTraceSensing( fTimePerSec );
		break;
	case DCTS_TRACING:	
		ProcessTraceTracing( fTimePerSec );
		break;
	case DCTS_ATTACK:
		CheckReservedSliding();
		CheckExtraAniJump();
		ProcessTraceAttack( fTimePerSec, dwPreTime );
		break;
	case DCTS_TRACING_WAIT:
		ProcessTraceTracingWait( fTimePerSec, dwPreTime );
		break;
	}

	CheckTraceAttack();
	CheckTraceStateEnd();
}


void ioCopyOwnerDummyChar3::ProcessExplosionState()
{
	if( m_dwExplosionWaitTime == 0 )
		return;

	switch( m_ExplosionState )
	{
	case ES_NONE:
	case ES_EXPLOSION:
		return;
		break;
	case ES_EXPLOSION_WAIT:
		{
			if( m_dwExplosionTime > 0 && m_dwExplosionTime <= FRAMEGETTIME() )
			{
				m_dwExplosionTime = 0;
				SetExplosionState();
				SetDieState( false );
			}
		}
		break;
	}
}

void ioCopyOwnerDummyChar3::ProcessTraceSensing( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( !m_TargetTraceName.IsEmpty() )
		m_TargetTraceName.Clear();

	if( pOwner->GetExperienceMode() == EMS_EXPERIENCE )
		return;

	ioHashString szTarget;
	szTarget = GetTraceTarget( pStage, false );

	if( !szTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( szTarget );
		if( pTarget )//&& pOwner->IsNeedProcess() )
		{
			m_TargetTraceName = szTarget;
			SetTracingState( pStage, true );
		}
	}

	if( m_dwSensingStartTime + m_dwSensingDuration < dwCurTime )
	{
		SetDieState( true );
	}
}


void ioCopyOwnerDummyChar3::SetTracingState( ioPlayStage *pStage, bool bSendNetwork )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	m_TraceState = DCTS_TRACING;
	DWORD dwCurTime = FRAMEGETTIME();

	m_dwTracingEndTime = dwCurTime + m_dwTraceDuration;

	if( bSendNetwork )
	{
		if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
		{
			SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
			kPacket << m_OwnerName;
			kPacket << m_iCreateIndex;
			kPacket << SYNC_TRACING;
			kPacket << m_TargetTraceName;
			kPacket << true;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	m_pGroup->ClearAllLoopAni( FLOAT100 );
	m_pGroup->ClearAllActionAni( FLOAT100 );

	int iAniID = m_pGroup->GetAnimationIdx( m_MoveAnimation );
	float fRate = FLOAT1;
	if( m_fTracingAniRate > 0.0f )
		fRate = m_fTracingAniRate;

	if( iAniID != -1 )
		m_pGroup->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fRate );

	//LOG.PrintConsoleLog( 0, "Dummy3(SetTracingState) - %s", m_TargetTraceName );
}



void ioCopyOwnerDummyChar3::ProcessTraceTracing( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	D3DXVECTOR3 vCurDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vCurDir, &vCurDir );

	D3DXVECTOR3 vCurPos = GetMidPositionByRate();

	if( m_dwTracingEndTime > dwCurTime )
	{
		CheckTraceTarget( pStage );
		FindTraceTarget( pStage );

		ioBaseChar *pTarget = pStage->GetBaseChar( m_TargetTraceName );		
		if( !IsTargetWoundedState( pTarget ) )
		{
			SetTraceWaitState();
			return;
		}

		if( pTarget )
		{
			D3DXVECTOR3 vPrePos = GetWorldPosition();
			D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - vPrePos;

			float fRangeSq = D3DXVec3LengthSq( & vDiff );

			if( pOwner->IsNeedProcess() )
			{
				if( fRangeSq < m_fTargetTraceExplosionRange * m_fTargetTraceExplosionRange )
				{
					SetExplosionWaitState( true, pStage );
					return;
				}
			}

			if( fRangeSq < m_fTraceRange * m_fTraceRange )
			{
				D3DXVECTOR3 vTargetDir = vDiff;
				vTargetDir.y = 0;
				D3DXVec3Normalize( &vTargetDir, &vTargetDir );

				if( m_dwRotateSpeed > 0 )
				{
					D3DXQUATERNION	qtRot;
					ioMath::CalcDirectionQuaternion( qtRot, -vTargetDir );

					float fAngle = D3DXVec3Dot( &vTargetDir, &m_vTracingMoveDir );

					float fAngleGap = RADtoDEG( acosf(fAngle) );
					float fRate = (fTimePerSec*1000) / m_dwRotateSpeed;
					float fRotateAngle = 360.0f * fRate;

					if( fAngleGap < fRotateAngle )
					{
						m_vTracingMoveDir = vTargetDir;
					}
					else
					{
						D3DXQUATERNION qtRotate;
						ioMath::CalcDirectionQuaternion( qtRotate, -m_vTracingMoveDir );

						bool bLeftRot = false;
						if( D3DXVec3Dot( &vTargetDir, &m_vTracingMoveDir ) > -FLOAT1 )
						{
							float fCurYaw, fTargetYaw;
							fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );
							fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRot ) );

							fCurYaw = ioMath::ArrangeHead( fCurYaw );
							fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

							float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
							if( fRotAngle < 0.0f )
								bLeftRot = true;
						}

						if( bLeftRot )
							fRotateAngle *= -1;

						fRotateAngle = ioMath::ArrangeHead( fRotateAngle );

						D3DXQUATERNION	qtNewRot;
						D3DXQuaternionRotationYawPitchRoll( &qtNewRot, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );
						m_vTracingMoveDir = qtNewRot * m_vTracingMoveDir;
					}
				}
				else
				{
					m_vTracingMoveDir = vTargetDir;
				}
			}
		}
		else
		{
			m_TargetTraceName.Clear();
		}
	}
	else
	{
		SetExplosionWaitState( false, NULL );
		return;
	}

	float fMoveDist = m_fTraceSpeed * fTimePerSec;
	ProcessTraceMoving( pStage, fMoveDist, fTimePerSec );
}


void ioCopyOwnerDummyChar3::ProcessPos( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_State == DCS_START )
	{
		m_dwProcessTime = dwCurTime;
		return;
	}

	ProcessForceMove( fTimePerSec );

	if( m_bOwnerRotate )
	{
		ioBaseChar *pOwner = GetOwner();
		if(	pOwner )
		{
			SetWorldOrientation( pOwner->GetTargetRot() );
		}
	}

	// 아래는 높이값 처리.

	D3DXVECTOR3 vPrePos = GetWorldPosition();
	D3DXVECTOR3 vCurPos = vPrePos;
	float fMapHeight = GetBottomHeight();

	DWORD dwGapTime = dwCurTime - m_dwProcessTime;

	do 
	{
		DWORD dwCurGap = min( dwGapTime, 10 );
		if( dwGapTime >= 10 )
		{
			dwGapTime -= 10;
		}
		else
		{
			dwCurGap = dwGapTime;
			dwGapTime = 0;
		}

		float fNewTimePerSec = (float)dwCurGap / FLOAT1000;

		if( m_fCurJumpPowerAmt > 0.0f )
		{
			vCurPos.y += m_fCurJumpPowerAmt * fNewTimePerSec;
		}

		float fBoundBlowGravityRate = FLOAT1;

		if( vCurPos.y > fMapHeight )
		{
			float fCurRate = m_fGravityRate;
			if( fCurRate <= 0.0f )
				fCurRate = FLOAT1;

			m_fGravityAmt += (m_pModeParent->GetCreator()->GetRoomGravity() * fCurRate * fBoundBlowGravityRate)* fNewTimePerSec;
			vCurPos.y -= m_fGravityAmt * fNewTimePerSec;

			if( vCurPos.y <= fMapHeight )
			{
				m_fGravityAmt = 0.0f;
				m_fCurJumpPowerAmt = 0.0f;
			}
		}
	} while( dwGapTime > 0 );

	if( vCurPos.y < fMapHeight )
	{
		vCurPos.y = fMapHeight;
		m_fGravityAmt = 0.0f;
		m_fCurJumpPowerAmt = 0.0f;
	}

	SetWorldPosition( vCurPos );

	m_dwProcessTime = dwCurTime;
}

void ioCopyOwnerDummyChar3::ProcessForceMove( float fTimePerSec )
{
	if( m_fForceAmt <= 0.0f )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	D3DXVECTOR3 vMoveAmt;
	float fCurAccelAmt = 0.0f;
	if( !m_bForcePowerLinear || !IsFloatingState() )
	{
		fCurAccelAmt -= ( pStage->GetRoomFriction() * m_fForceFrictionRate ) * fTimePerSec;
	}

	// (v*dt) + (a*dt*dt)*0.5f 공식 적용
	// GetRoomFriction() * m_fForceFrictionRate 를 a로 보고
	// fCurAccelAmt 계산시 이미 fTimerPerSec를 한번 곱했기때문에 최종계산에서는 fTimerPerSec를 한번만 곱한다.
	float fFinalMoveAmt = m_fForceAmt * fTimePerSec + fCurAccelAmt * fTimePerSec * FLOAT05;

	m_fTestValue += fFinalMoveAmt;

	if( fFinalMoveAmt > 0.0f )	// 감속이 심해서 음수가 나올 경우는 제외시킴
	{
		vMoveAmt = m_vForceDir * fFinalMoveAmt;
		m_fForceAmt += fCurAccelAmt;
		m_fForceAmt = max( 0.0f, m_fForceAmt );

		if( !IsFloating() && vMoveAmt.y <= 0.0f )
			vMoveAmt.y = 0.0f;

		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( this, &vMoveAmt, bCol ) )
		{
			Translate( vMoveAmt );
		}

		if( m_bDestroyMapCol && bCol )
		{
			SendDieState( true );
			return;
		}

		return;
	}

	if( m_fForceAmt <= 5.0f )
		m_fForceAmt = 0.0f;
}

void ioCopyOwnerDummyChar3::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	/*ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		ioHashString DummyName = GetDummyCharName();

		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyCharToName( pOwner, DummyName );

		if ( pDummy )
		{
			D3DXVECTOR3 vPos = pDummy->GetWorldPosition();
			pOwner->SetWorldPosition( vPos );
		}

	}*/

	ioDummyChar::SetDieState( bEffect, bWoundedDie );
}


void ioCopyOwnerDummyChar3::SetExplosionWaitState( bool bSendNet, ioPlayStage *pStage )
{
	if( m_dwExplosionWaitTime == 0 )
		return;

	m_State = DCS_NONE;
	if( m_ExplosionState != ES_NONE )
		return;

	int iAniID = m_pGroup->GetAnimationIdx( m_DelayAnimation );
	if( iAniID != -1 )
	{
		m_pGroup->ClearAllLoopAniExceptOne( iAniID, FLOAT100, true );
		m_pGroup->ClearAllActionAni( FLOAT100, true );
		m_pGroup->SetLoopAni( iAniID, FLOAT100 );
	}

	switch( m_ExplosionState )
	{
	case ES_NONE:
		{
			AttachEffect( m_szExplosionWaitEffect, NULL );
			m_ExplosionState = ES_EXPLOSION_WAIT;
			m_dwExplosionTime = FRAMEGETTIME() + m_dwExplosionWaitTime;
		}
		break;
	case ES_EXPLOSION_WAIT:
		{
			m_dwExplosionTime = FRAMEGETTIME();
		}
		break;
	case ES_EXPLOSION:
		break;
	}

	if( bSendNet /*&& pStage*/ )
	{
		if( P2PNetwork::IsNetworkPlaying() /*&& pStage->IsNetworkMode()*/ )
		{
			SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
			kPacket << m_OwnerName;
			kPacket << m_iCreateIndex;
			kPacket << SYNC_EXPLOSION_WAIT;
			kPacket << GetWorldPosition();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}


void ioCopyOwnerDummyChar3::SetExplosionState()
{
	if( m_ExplosionState == ES_EXPLOSION )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	m_ExplosionState = ES_EXPLOSION;

	D3DXVECTOR3 vPos = GetMidPositionByRate();
	D3DXVECTOR3 vAttackDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vAttackDir, &vAttackDir );

	m_dwBaseWeaponIndex = pOwner->IncreaseWeaponIndexBase();

	FireTime kFireTime;
	kFireTime.dwStart              = FRAMEGETTIME();
	kFireTime.iAttributeIdx        = m_WeaponOnWound.m_iWeaponIdx;
	kFireTime.iResistanceIdx       = m_WeaponOnWound.m_iResistanceIdx;
	kFireTime.szWoundedAni         = m_WeaponOnWound.m_WoundedAnimation;
	kFireTime.dwWoundedAniDuration = m_WeaponOnWound.m_dwWoundedDuration;
	kFireTime.bLoopWoundedAni      = m_WeaponOnWound.m_bWoundedLoopAni;
	kFireTime.dwWeaponIdx          = m_dwBaseWeaponIndex;
	kFireTime.eFireTimeType        = FTT_DUMMY_CHAR_ATTACK;
	kFireTime.m_iExtraValue        = m_iCreateIndex;

	if( kFireTime.iAttributeIdx != NO_WEAPON_ATTRIBUTE )
	{
		ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vAttackDir, "" );
		ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
		if( pZone )
		{
			pZone->SetCollisionDir( vAttackDir );
			pZone->SetZoneValue( true, false );
		}
	}
}


void ioCopyOwnerDummyChar3::ProcessWoundedState( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_AnimationTime.EndTime() < dwCurTime )
	{
		if( m_BlowTargetType != BTT_BLOW &&
			m_BlowTargetType != BTT_BLOW2 &&
			m_BlowTargetType != BTT_BLOW3 &&
			m_BlowTargetType != BTT_STAND_KO &&
			m_BlowTargetType != BTT_STAND_KO2 )
		{
			SetDummyState( DCS_DELAY );
		}
		else if( m_BlowTargetType == BTT_STAND_KO || m_BlowTargetType == BTT_STAND_KO2 )
		{
			BlowWoundAniInfo kInfo;
			SetBlowWoundedState( false, kInfo );
		}
	}
}

ApplyWeaponExp ioCopyOwnerDummyChar3::ApplyWeapon( ioWeapon *pWeapon )
{
	ioBaseChar *pAttacker = pWeapon->GetOwner();
	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioDummyChar::ApplyWeapon - %s Not Exist", pWeapon->GetOwnerName().c_str() );
		return AWE_EXPLOSION;
	}

	if( pWeapon->IsNoWoundedWeapon() )
		return AWE_NO_EXPLOSION;

	if( GetOwner() && !GetOwner()->IsNeedProcess() )
		return AWE_EXPLOSION;

	D3DXVECTOR3 vAttackDir = pWeapon->GetAttackDir(this);
	D3DXVec3Normalize( &vAttackDir, &vAttackDir );

	AttackSide eAttackSide = GetAttackedSide( vAttackDir );

	if( eAttackSide == AS_FRONT )
	{
		D3DXQUATERNION qtRot;
		ioMath::CalcDirectionQuaternion( qtRot, vAttackDir );
		SetWorldOrientation( qtRot );
	}

	ApplyWeaponWounded( pWeapon, eAttackSide, vAttackDir );
	return AWE_EXPLOSION;
}


void ioCopyOwnerDummyChar3::ApplyWeaponWounded( ioWeapon *pWeapon, AttackSide eSide, const D3DXVECTOR3 &vAttackDir )
{
	SetExplosionWaitState( true, NULL );
	//if( !pWeapon )
	//	return;

	//ioBaseChar *pAttacker = pWeapon->GetOwner();
	//if( !pAttacker )
	//	return;

	//BlowTargetType ePreBlowType = m_BlowTargetType;
	//m_BlowTargetType = pWeapon->GetBlowType();
	//m_AttackedSide   = eSide;

	//// 밀림, 뛰움 값 처리
	//float fBaseDamage = pWeapon->GetBaseDamage( this );
	//float fDamage = pWeapon->GetDamage( this );
	//float fBoundDamage = 0.0f;
	//float fBoundJumpAmt = pWeapon->GetBoundBlowPower( fBoundDamage );
	//float fBoundForceAmt = pWeapon->GetBoundPushPower( fBoundDamage );
	//float fBoundGrvityRate = pWeapon->GetBoundBlowGravityRate();

	//BlowWoundAniInfo kBlowWoundAniInfo = pWeapon->GetBlowWoundAniInfo();

	//bool bForceLinear;
	//float fBlowPower, fPushPower;
	//if( IsFloatingState() )
	//{
	//	fBlowPower = pWeapon->GetAirBlowPower( fDamage );
	//	fPushPower = pWeapon->GetAirPushPower( fDamage );
	//	bForceLinear = pWeapon->HasFeature( WF_AIR_PUSH_POWER_LINEAR );
	//}
	//else
	//{
	//	fBlowPower = pWeapon->GetBlowPower( fDamage );
	//	fPushPower = pWeapon->GetPushPower( fDamage );
	//	bForceLinear = pWeapon->HasFeature( WF_PUSH_POWER_LINEAR );
	//}

	//// 밀림, 띄움 값 계산후 성장에 의한 데미지 적용
	//fDamage *= pWeapon->GetGrowthAttackRate();

	//if( m_State == DCS_BLOW_WOUNDED ||
	//	m_State == DCS_BOUND_BLOW_WOUNDED )
	//{
	//	m_iFloatingWoundedCnt++;

	//	float fBlowRate = m_fFloatingBlowDownRate * (float)m_iFloatingWoundedCnt;
	//	fBlowRate = max( 0.0f, ( FLOAT1 - fBlowRate ) );
	//	fBlowPower *= fBlowRate;

	//	if( !IsFloatingState() )
	//	{
	//		fBlowPower *= pWeapon->GetDownStateBlowPower();
	//		fPushPower *= pWeapon->GetDownStatePushPower();
	//	}
	//}

	////
	//bool bApplyPushPower = true;

	//ioHashString szWoundAni = pWeapon->GetWoundedAnimation();
	//DWORD dwWoundDuration = pWeapon->GetWoundedDuration();
	//bool bWoundedLoopAni = pWeapon->IsLoopWoundedAnimation();

	//float fDefenseEnableRate = pWeapon->GetDefenseEnableRate();

	//bool bAllWeakWounded = false;
	//BlowTargetType eNewBlowType = pWeapon->GetBlowType();

	//ioBaseChar *pOwner = GetOwner();
	//ioPlayStage *pStage = NULL;
	//if( pOwner )
	//	pStage = pOwner->GetCreator();

	//if( pStage )
	//{
	//	if( pStage->CheckRoomWeakWounded( m_TeamType ) )
	//	{
	//		bAllWeakWounded = true;
	//		fBlowPower = FLOAT1;
	//		//fPushPower = FLOAT1;
	//	}
	//}

	////대화 광장에서 대미지 안들어가게 수정
	//if( pStage && pStage->GetRoomStyle() == RSTYLE_PLAZA && pStage->GetPlazaModeType() == PT_COMMUNITY )
	//	fDamage = 0.f;

	//if( pOwner )
	//{
	//	pOwner->GetHP().m_fCurValue -= fDamage;
	//}

	//if( fDamage > 0.0f )
	//{
	//	AddHeadDamageInfo( fDamage );
	//	SetLastApplyWeapon( pWeapon->GetAttributeIndex(), pWeapon->GetIgnoreWeaponTime() );
	//}

	//if( bApplyPushPower )
	//{
	//	if( m_HP.m_fCurValue <= 0.0f )
	//	{

	//	}

	//	if( bAllWeakWounded )
	//	{
	//		if( m_BlowTargetType == BTT_WEAK )
	//		{
	//			SetWoundedState( szWoundAni, dwWoundDuration, bWoundedLoopAni );
	//		}
	//		else
	//		{
	//			m_BlowTargetType = BTT_WEAK;

	//			SetWoundedState( "", 0, bWoundedLoopAni );
	//		}
	//	}
	//	else if( m_BlowTargetType == BTT_BOUND_BLOW )
	//	{
	//		bool bGetUpState = false;

	//		SetBoundWoundedState( fBoundJumpAmt, fBoundForceAmt, fBoundGrvityRate, fBoundDamage, bGetUpState );
	//	}
	//	else if( m_BlowTargetType == BTT_GETUP )
	//	{
	//		SetWoundedState( szWoundAni, dwWoundDuration, bWoundedLoopAni );
	//	}
	//	else if( m_BlowTargetType == BTT_GETUP_STAND_KO )
	//	{
	//		m_BlowTargetType = BTT_STAND_KO2;
	//		eNewBlowType = BTT_STAND_KO2;

	//		SetWoundedState( szWoundAni, dwWoundDuration, bWoundedLoopAni );
	//	}
	//	else if( m_State == DCS_BLOW_WOUNDED ||
	//		m_State == DCS_BOUND_BLOW_WOUNDED ||
	//		m_BlowTargetType == BTT_BLOW ||
	//		m_BlowTargetType == BTT_BLOW2 ||
	//		m_BlowTargetType == BTT_BLOW3 ||
	//		m_BlowTargetType == BTT_FLATTEN )
	//	{
	//		bool bGetUpState = false;

	//		if( fBlowPower > m_fKOActionBlowPower )
	//			SetBlowWoundedState( true, kBlowWoundAniInfo, bGetUpState );
	//		else
	//			SetBlowWoundedState( false, kBlowWoundAniInfo, bGetUpState );
	//	}
	//	else if( m_State == DCS_WOUNDED )
	//	{
	//		if( ePreBlowType == BTT_M_STIFF )
	//		{
	//			if( m_BlowTargetType == BTT_M_STIFF )
	//			{
	//				m_BlowTargetType = BTT_STAND_KO;
	//				eNewBlowType = BTT_STAND_KO;
	//			}

	//			SetWoundedState( szWoundAni, dwWoundDuration, bWoundedLoopAni );
	//		}
	//		else if( ePreBlowType == BTT_M_BLOW && m_BlowTargetType == BTT_M_BLOW )
	//		{
	//			m_BlowTargetType = BTT_BLOW3;
	//			eNewBlowType = BTT_BLOW3;

	//			if( fBlowPower > m_fKOActionBlowPower )
	//				SetBlowWoundedState( true, kBlowWoundAniInfo );
	//			else
	//				SetBlowWoundedState( false, kBlowWoundAniInfo );
	//		}
	//		else if( ePreBlowType == BTT_M_BLOW && m_BlowTargetType == BTT_M_STIFF )
	//		{
	//			m_BlowTargetType = BTT_STAND_KO;
	//			eNewBlowType = BTT_STAND_KO;

	//			SetWoundedState( szWoundAni, dwWoundDuration, bWoundedLoopAni );
	//		}
	//		else if( ePreBlowType == BTT_STAND_KO || IsFloatingState() )
	//		{
	//			if( fBlowPower > m_fKOActionBlowPower )
	//				SetBlowWoundedState( true, kBlowWoundAniInfo );
	//			else
	//				SetBlowWoundedState( false, kBlowWoundAniInfo );
	//		}
	//		else if( ePreBlowType == BTT_STAND_KO2 || IsFloatingState() )
	//		{
	//			if( fBlowPower > m_fKOActionBlowPower )
	//				SetBlowWoundedState( true, kBlowWoundAniInfo );
	//			else
	//				SetBlowWoundedState( false, kBlowWoundAniInfo );
	//		}
	//		else if( ePreBlowType == BTT_GETUP && m_BlowTargetType == BTT_M_STIFF )
	//		{
	//			m_BlowTargetType = BTT_STAND_KO;
	//			eNewBlowType = BTT_STAND_KO;

	//			SetWoundedState( szWoundAni, dwWoundDuration, bWoundedLoopAni );
	//		}
	//		else
	//		{
	//			SetWoundedState( szWoundAni, dwWoundDuration, bWoundedLoopAni );
	//		}
	//	}
	//	else
	//	{
	//		SetWoundedState( szWoundAni, dwWoundDuration, bWoundedLoopAni );
	//	}
	//}

	//// 밀림, 띄움처리.
	//if( bApplyPushPower )
	//{
	//	float fPushAngle = pWeapon->GetPushAngle();
	//	D3DXVECTOR3 vPushDir = vAttackDir;

	//	if( vPushDir != ioMath::UNIT_Y )
	//	{
	//		D3DXVECTOR3 vAxis1, vAxis2;
	//		D3DXVec3Cross( &vAxis1, &ioMath::UNIT_Y, &vPushDir );
	//		D3DXVec3Normalize(&vAxis1,&vAxis1);

	//		D3DXVec3Cross( &vAxis2, &vPushDir, &vAxis1 );
	//		D3DXVec3Normalize(&vAxis2,&vAxis2);

	//		D3DXQUATERNION qtRot;
	//		if( vAxis2 == ioMath::VEC3_ZERO )
	//			D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fPushAngle) );
	//		else
	//			D3DXQuaternionRotationAxis( &qtRot, &vAxis2, DEGtoRAD(fPushAngle) );

	//		vPushDir = qtRot * vPushDir;
	//		D3DXVec3Normalize(&vPushDir,&vPushDir);

	//		SetForcePower( vPushDir, fPushPower, FLOAT1, bForceLinear );
	//	}

	//	SetJumpPower( fBlowPower );

	//	// 갱신할 필요가 있다.
	//	m_fCurGravityGapAmt = m_fGravityAmt - m_fCurJumpPowerAmt;
	//}
	//else
	//{
	//	fBlowPower = 0.0f;
	//}	

	//bool bEnableHitCount = true;
	//if( !bApplyPushPower || pWeapon->HasFeature(WF_DISABLE_HIT_COUNT) )
	//	bEnableHitCount = false;

	//if( pWeapon->GetTeam() != GetTeam() && bEnableHitCount && pAttacker )
	//{
	//	pAttacker->CheckComboSkillHit();
	//}

	//if( pWeapon->CheckWeaponByJump() && pAttacker )
	//{
	//	pAttacker->SetSuccessJumpAttack( true );
	//}

	//SetIgnoreWeaponInfo( pWeapon );

	//if( GetOwner() && GetOwner()->IsNeedSendNetwork() )
	//{
	//	SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
	//	kPacket << m_OwnerName;
	//	kPacket << m_iCreateIndex;
	//	kPacket << SYNC_APPLY_WEAPON;

	//	kPacket << GetWorldPosition();
	//	kPacket << GetWorldOrientation();

	//	kPacket << pWeapon->GetOwnerName();
	//	kPacket << pWeapon->GetAttributeIndex();

	//	kPacket << (int)m_AttackedSide;
	//	kPacket << (int)eNewBlowType;
	//	kPacket << m_HP.m_fCurValue;
	//	kPacket << fDamage;

	//	kPacket << m_vForceDir;
	//	kPacket << m_fForceAmt;
	//	kPacket << m_fForceFrictionRate;
	//	kPacket << m_bForcePowerLinear;

	//	kPacket << m_fCurJumpPowerAmt;
	//	kPacket << m_fGravityAmt;

	//	kPacket << IsFloatingState();
	//	kPacket << IsFloating();

	//	kPacket << pWeapon->GetTeam();

	//	kPacket << m_State;
	//	kPacket << m_bJumpedBlowWounded;
	//	kPacket << m_bFallBlowWounded;

	//	kPacket << bWoundedLoopAni;

	//	if( m_State == DCS_WOUNDED )
	//	{
	//		kPacket << szWoundAni;
	//		kPacket << dwWoundDuration;
	//		kPacket << bAllWeakWounded;
	//	}
	//	kPacket << bEnableHitCount;
	//	kPacket << pWeapon->CheckWeaponByJump();
	//	kPacket << pOwner->GetHP().m_fCurValue;
	//	P2PNetwork::SendToAllPlayingUser( kPacket );
	//}
}

//void ioCopyOwnerDummyChar3::ApplyWeaponWoundedInNetwork( ioPlayStage *pStage, SP2Packet &rkPacket )
//{
	//SetExplosionWaitState( false, NULL );

	/*D3DXVECTOR3 vPos;
	D3DXQUATERNION qtRot;
	rkPacket >> vPos;
	rkPacket >> qtRot;
	SetWorldPosition( vPos );
	SetWorldOrientation( qtRot );

	ioHashString szAttackerName;
	rkPacket >> szAttackerName;
	ioBaseChar *pAttacker = pStage->GetBaseChar( szAttackerName );
	int iAttributeIndex;
	rkPacket >> iAttributeIndex;

	int iAttackSide, iBlowType;
	float fDamage;
	rkPacket >> iAttackSide >> iBlowType;
	m_AttackedSide = (AttackSide)iAttackSide;
	m_BlowTargetType = (BlowTargetType)iBlowType;
	rkPacket >> m_HP.m_fCurValue;
	rkPacket >> fDamage;
	rkPacket >> m_vForceDir;
	rkPacket >> m_fForceAmt >> m_fForceFrictionRate >> m_bForcePowerLinear;

	rkPacket >> m_fCurJumpPowerAmt >> m_fGravityAmt;

	bool bFloatingState, bFloating;
	rkPacket >> bFloatingState >> bFloating;

	int iAttackerTeam;
	rkPacket >> iAttackerTeam;

	const WeaponAttribute *pAttr = g_WeaponMgr.FindAttribute( iAttributeIndex );

	if( pAttr && fDamage > 0.0f )
	{
		SetLastApplyWeapon( iAttributeIndex, pAttr->m_dwIgnoreWeaponTime );
	}

	int iCharState;	
	bool bJumpedBlow, bFallBlow;
	rkPacket >> iCharState;
	rkPacket >> bJumpedBlow >> bFallBlow;
	bool bWoundedLoopAni;
	rkPacket >> bWoundedLoopAni;

	if( iCharState == DCS_WOUNDED )
	{
		ioHashString szWoundedAni;
		DWORD dwWoundedDuration;
		bool bAllWeakWounded;

		rkPacket >> szWoundedAni >> dwWoundedDuration;
		rkPacket >> bAllWeakWounded;

		if( bAllWeakWounded )
		{
			if( m_BlowTargetType == BTT_WEAK )
			{
				SetWoundedState( szWoundedAni, dwWoundedDuration, bWoundedLoopAni );
			}
			else
			{
				m_BlowTargetType = BTT_WEAK;
				SetWoundedState( "", 0, bWoundedLoopAni);
			}
		}
		else
		{
			SetWoundedState( szWoundedAni, dwWoundedDuration, bWoundedLoopAni);
		}
	}

	bool bEnableHitCount;
	rkPacket >> bEnableHitCount;

	if( pAttacker && pAttacker->GetTeam() != GetTeam() && bEnableHitCount )
	{
		pAttacker->CheckComboSkillHit();
	}

	bool bSuccessJumpAttack;
	rkPacket >> bSuccessJumpAttack;
	if( bSuccessJumpAttack && pAttacker )
	{
		pAttacker->SetSuccessJumpAttack( true );
	}

	float fOwnerHpValue;
	rkPacket >> fOwnerHpValue;
	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		pOwner->SetCurHP( fOwnerHpValue );
	}	

	if ( iCharState == DCS_DIE )
	{
		SetDieState( false );
	}*/
//}

void ioCopyOwnerDummyChar3::ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pStage ) return;

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	/*case SYNC_APPLY_WEAPON:
		ApplyWeaponWoundedInNetwork( pStage, rkPacket );
		break;*/
	case SYNC_TRACING:
		{
			rkPacket >> m_TargetTraceName;
			SetTracingState( pStage, false );
		}
		return;
	case SYNC_EXPLOSION_WAIT:
		{
			D3DXVECTOR3 vWorldPos;
			rkPacket >> vWorldPos;
			SetWorldPosition( vWorldPos );
		
			SetExplosionWaitState( false, pStage );
		}
		return;
	default:
		ioDummyChar::ApplyDummyCharSyncByType( iType, rkPacket, pStage );
	}
}

void ioCopyOwnerDummyChar3::SetDummyState( DummyCharState eState )
{
	if( !m_pGroup )
		return;
	switch(eState)
	{
	case DCS_DELAY:
		{
			int iAniID = m_pGroup->GetAnimationIdx( m_DelayAnimation );
			if( iAniID != -1 )
			{
				m_pGroup->ClearAllLoopAniExceptOne( iAniID, FLOAT100, true );
				m_pGroup->ClearAllActionAni( FLOAT100, true );
				m_pGroup->SetLoopAni( iAniID, FLOAT100 );

				m_AnimationTime.dwStartTime = 0;
				m_AnimationTime.dwFullTime = 0;

				m_AttackedSide = AS_NONE;
				m_BlowTargetType = BTT_NONE;
			}
		}
		break;
	case DCS_BLOW_WOUNDED:
		{
			eState = DCS_DIE;
			SetDieState( false );
		}
		break;
	case DCS_BOUND_BLOW_WOUNDED:
		{
			eState = DCS_DIE;
			SetDieState( false );
		}

		break;
	}

	m_State = eState;
}

void ioCopyOwnerDummyChar3::SetWoundedState( const ioHashString &szWoundedAni, DWORD dwWoundedDuration, bool bLoopAni )
{
	if( !m_pGroup )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_pGroup->ClearAllActionAni( 0.0f, true );
	m_pGroup->ClearAllLoopAni( 0.0f, true );

	int iAniID = -1;
	float fTimeRate = FLOAT1;
	bool bCustomAni = true;

	bool bChangeAni = false;
	if( !szWoundedAni.IsEmpty() )
		bChangeAni = true;

	if( bChangeAni )
	{
		std::string szBuf;
		if( m_AttackedSide == AS_FRONT )
			szBuf = ioStringConverter::AddTailToFileName( szWoundedAni.c_str(), "_fr" );
		else
			szBuf = ioStringConverter::AddTailToFileName( szWoundedAni.c_str(), "_bk" );

		iAniID = m_pGroup->GetAnimationIdx( ioHashString( szBuf.c_str() ) );
		if( !bLoopAni && iAniID != -1 && dwWoundedDuration > 0 )
		{
			fTimeRate = (float)dwWoundedDuration/(float)m_pGroup->GetAnimationFullTime( iAniID );
		}
	}

	if( iAniID == -1 )
	{
		bCustomAni = false;
		iAniID = GetWoundedAnimationIdx( m_AttackedSide, m_BlowTargetType );

		switch( m_BlowTargetType )
		{
		case BTT_WEAK:
			fTimeRate = m_fWeakAttackWoundedRate;
			break;
		case BTT_M_STIFF:
		case BTT_M_BLOW:
		case BTT_GETUP:
			fTimeRate = m_fMidStiffWoundedRate;
			break;
		case BTT_S_STIFF:
			fTimeRate = m_fStrStiffWoundedRate;
			break;
		case BTT_STAND_KO:
			fTimeRate = m_fStandKoWoundedRate;
			break;
		case BTT_STAND_KO2:
			fTimeRate = m_fStandKo2WoundedRate;
			break;
		}
	}

	DWORD dwDuration = m_pGroup->GetAnimationFullTime( iAniID ) * fTimeRate;
	if( bLoopAni && bCustomAni )
	{
		if( iAniID != -1 )
			m_pGroup->SetLoopAni( iAniID, 0.0f, FLOAT1, FLOAT1 );

		dwDuration = dwWoundedDuration;
	}
	else
		m_pGroup->SetActionAni( iAniID, 0.0f, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_AnimationTime.dwStartTime = dwCurTime;
	m_AnimationTime.dwFullTime  = dwDuration;

	m_State = DCS_WOUNDED;
}

void ioCopyOwnerDummyChar3::SetBlowWoundedState( bool bJumped, const BlowWoundAniInfo &rkBlowWoundAniInfo, bool bGetUpState /* = false */, bool bFallState /* = false */ )
{
	m_bJumpedBlowWounded = bJumped;
	m_bFallBlowWounded = bFallState;
	m_bBlowEndAni = false;

	m_pGroup->ClearAllActionAni( FLOAT100, true );
	m_pGroup->ClearAllLoopAni( FLOAT100, true );

	// wound
	if( m_AttackedSide == AS_BACK )
	{
		m_CurBlowWoundAction = m_AniBackJumpKoAction;

		if( !m_bJumpedBlowWounded )
		{
			if( m_BlowTargetType == BTT_STAND_KO )
				m_CurBlowWoundAction = m_AniBackStandKoDown;
			else if( m_BlowTargetType == BTT_STAND_KO2 )
				m_CurBlowWoundAction = m_AniBackStandKo2Down;
			else if( m_BlowTargetType == BTT_BLOW2 )
				m_CurBlowWoundAction = m_AniBackKoAction2;
			else if( m_BlowTargetType == BTT_BLOW3 )
				m_CurBlowWoundAction = m_AniBackKoAction3;
			else
				m_CurBlowWoundAction = m_AniBackKoAction;
		}
	}
	else
	{
		m_CurBlowWoundAction = m_AniJumpKoAction;

		if( !m_bJumpedBlowWounded )
		{
			if( m_BlowTargetType == BTT_STAND_KO )
				m_CurBlowWoundAction = m_AniStandKoDown;
			else if( m_BlowTargetType == BTT_STAND_KO2 )
				m_CurBlowWoundAction = m_AniStandKo2Down;
			else if( m_BlowTargetType == BTT_BLOW2 )
				m_CurBlowWoundAction = m_AniKoAction2;
			else if( m_BlowTargetType == BTT_BLOW3 )
				m_CurBlowWoundAction = m_AniKoAction3;
			else
				m_CurBlowWoundAction = m_AniKoAction;
		}
	}

	// down
	if( m_AttackedSide == AS_BACK )
	{
		if( m_bFallBlowWounded )
			m_CurBlowDownAction = m_AniFallBackDownAction;
		else if( m_BlowTargetType == BTT_BLOW2 )
			m_CurBlowDownAction = m_AniBackdownKoAction2;
		else if( m_BlowTargetType == BTT_BLOW3 )
			m_CurBlowDownAction = m_AniBackdownKoAction3;
		else
			m_CurBlowDownAction = m_AniBackdownKoAction;
	}
	else
	{
		if( m_bFallBlowWounded )
			m_CurBlowDownAction = m_AniFallDownAction;
		else if( m_BlowTargetType == BTT_BLOW2 )
			m_CurBlowDownAction = m_AniDownKoAction2;
		else if( m_BlowTargetType == BTT_BLOW3 )
			m_CurBlowDownAction = m_AniDownKoAction3;
		else
			m_CurBlowDownAction = m_AniDownKoAction;
	}

	//
	if( m_AttackedSide == AS_BACK )
	{
		// blowing
		if( m_bJumpedBlowWounded && !rkBlowWoundAniInfo.m_JumpBlowingBackAni.IsEmpty() )
		{
			m_CurBlowWoundAction = rkBlowWoundAniInfo.m_JumpBlowingBackAni;
		}
		else if( !m_bJumpedBlowWounded && !rkBlowWoundAniInfo.m_BlowingBackAni.IsEmpty() )
		{
			m_CurBlowWoundAction = rkBlowWoundAniInfo.m_BlowingBackAni;
		}

		// down
		if( !m_bFallBlowWounded && !rkBlowWoundAniInfo.m_DownBackAni.IsEmpty() )
		{
			m_CurBlowDownAction = rkBlowWoundAniInfo.m_DownBackAni;
		}
	}
	else
	{
		// blowing
		if( m_bJumpedBlowWounded && !rkBlowWoundAniInfo.m_JumpBlowingAni.IsEmpty() )
		{
			m_CurBlowWoundAction = rkBlowWoundAniInfo.m_JumpBlowingAni;
		}
		else if( !m_bJumpedBlowWounded && !rkBlowWoundAniInfo.m_BlowingAni.IsEmpty() )
		{
			m_CurBlowWoundAction = rkBlowWoundAniInfo.m_BlowingAni;
		}

		// down
		if( !m_bFallBlowWounded && !rkBlowWoundAniInfo.m_DownAni.IsEmpty() )
		{
			m_CurBlowDownAction = rkBlowWoundAniInfo.m_DownAni;
		}
	}

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_bJumpedBlowWounded )
	{
		int iAniID = m_pGroup->GetAnimationIdx(m_CurBlowWoundAction);
		DWORD dwKOAniTime = m_pGroup->GetAnimationFullTime( iAniID );
		m_pGroup->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1, 0.0f, 0.0f, true );
		m_AnimationTime.dwStartTime = dwCurTime;
		m_AnimationTime.dwFullTime  = dwKOAniTime;
		m_dwBlowAniEndTime = dwCurTime + dwKOAniTime;
	}
	else
	{
		int iAniID = m_pGroup->GetAnimationIdx( m_CurBlowWoundAction );
		m_pGroup->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1, 0.0f, 0.0f, true );

		m_AnimationTime.dwStartTime = dwCurTime;
		m_AnimationTime.dwFullTime  = m_pGroup->GetAnimationFullTime( iAniID );
		m_dwBlowAniEndTime = dwCurTime + m_pGroup->GetAnimationEventTime_e( iAniID, "knock_down" );
	}

	m_dwMaxBlowWoundedGetUpTime = m_dwGeneralBlowWoundedGetUpTime * m_fBlowGetUpTimeRate;

	if( m_BlowTargetType == BTT_FLATTEN )
	{
		m_dwCurBlowGetUpTime = m_dwMaxBlowWoundedGetUpTime;
		m_dwBlowGetUpCheckStartTime = 0;
		m_dwGetUpDurationTime = 0;
	}
	else
	{
		if( !bGetUpState && m_dwBlowGetUpCheckStartTime == 0 )
		{
			m_dwCurBlowGetUpTime = m_dwMaxBlowWoundedGetUpTime;
			m_dwCurBlowGetUpTime = max( 0, m_dwCurBlowGetUpTime );

			m_dwGetUpDurationTime = 0;
		}
		else
		{
			if( bGetUpState )
			{
				m_dwBlowGetUpCheckStartTime = dwCurTime;
				m_dwCurBlowGetUpTime = 0;
			}
		}
	}

	SetDummyState( DCS_BLOW_WOUNDED );

	m_dwGetUpDurationTime = 0;
	m_iBackAttackedCnt = 0;	

	// Flatten
	if( m_BlowTargetType != BTT_FLATTEN )
	{
		D3DXVECTOR3 vCurScale = GetWorldScale();
		if( vCurScale.x != vCurScale.y )
		{
			vCurScale.y = vCurScale.x;
			SetWorldScale( vCurScale );
		}
	}
}

void ioCopyOwnerDummyChar3::SetBoundWoundedState( float fBoundJump, float fBoundForce, float fBoundGravity, float fBoundDamage, bool bGetUpState /* = false */ )
{
	m_bBlowEndAni = false;

	m_bJumpedBlowWounded = true;

	m_fBoundBlowJumpAmt = fBoundJump;
	m_fBoundBlowForceAmt = fBoundForce;
	m_fBoundBlowGravityRate = fBoundGravity;
	m_fBoundBlowBoundDamage = fBoundDamage;

	if( m_AttackedSide == AS_BACK )
	{
		m_CurBlowWoundAction = m_AniBoundBlowBackAction;
		m_CurBlowDownAction = m_AniBoundDownBackAction;
	}
	else
	{
		m_CurBlowWoundAction = m_AniBoundBlowAction;
		m_CurBlowDownAction = m_AniBoundDownAction;
	}

	m_pGroup->ClearAllActionAni( FLOAT100, true );
	m_pGroup->ClearAllLoopAni( FLOAT100, true );

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_bJumpedBlowWounded )		// 공중 피격상황
	{
		int iAniID = m_pGroup->GetAnimationIdx( m_CurBlowWoundAction );
		DWORD dwKOAniTime = m_pGroup->GetAnimationFullTime( iAniID );

		m_pGroup->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1, 0.0f, 0.0f, true );
		m_AnimationTime.dwStartTime = dwCurTime;
		m_AnimationTime.dwFullTime  = dwKOAniTime;
		m_dwBlowAniEndTime = dwCurTime + dwKOAniTime;
	}

	m_dwMaxBlowWoundedGetUpTime = m_dwGeneralBlowWoundedGetUpTime * m_fBlowGetUpTimeRate;

	if( m_BlowTargetType == BTT_FLATTEN )
	{
		m_dwCurBlowGetUpTime = m_dwMaxBlowWoundedGetUpTime;
		m_dwBlowGetUpCheckStartTime = 0;
		m_dwGetUpDurationTime = 0;
	}
	else
	{
		if( !bGetUpState && m_dwBlowGetUpCheckStartTime == 0 )
		{
			m_dwCurBlowGetUpTime = m_dwMaxBlowWoundedGetUpTime;
			m_dwCurBlowGetUpTime = max( 0, m_dwCurBlowGetUpTime );
			m_dwGetUpDurationTime = 0;

			if( bGetUpState )
			{
				m_dwBlowGetUpCheckStartTime = dwCurTime;
				m_dwCurBlowGetUpTime = 0;
			}
		}
	}

	SetDummyState( DCS_BOUND_BLOW_WOUNDED );

	m_dwGetUpDurationTime = 0;
	m_iBackAttackedCnt = 0;	

	// Flatten
	if( m_BlowTargetType != BTT_FLATTEN )
	{
		D3DXVECTOR3 vCurScale = GetWorldScale();
		if( vCurScale.x != vCurScale.y )
		{
			vCurScale.y = vCurScale.x;
			SetWorldScale( vCurScale );
		}
	}
}

int ioCopyOwnerDummyChar3::GetWoundedAnimationIdx( AttackSide eAttackSide, BlowTargetType eBlowTargetType ) const
{
	if( eAttackSide == AS_FRONT )
	{
		switch( eBlowTargetType )
		{
		case BTT_WEAK:
			return m_pGroup->GetAnimationIdx( m_WeakWounded );
		case BTT_M_STIFF:
		case BTT_M_BLOW:
		case BTT_GETUP:
			return m_pGroup->GetAnimationIdx( m_MidStiffWounded );
		case BTT_S_STIFF:
			return m_pGroup->GetAnimationIdx( m_StrStiffWounded );
		case BTT_STAND_KO:
			return m_pGroup->GetAnimationIdx( m_AniStandKoAction );
		case BTT_STAND_KO2:
			return m_pGroup->GetAnimationIdx( m_AniStandKo2Action );
		}
	}
	else 
	{
		switch( eBlowTargetType )
		{
		case BTT_WEAK:
			return m_pGroup->GetAnimationIdx( m_BackWeakWounded );
		case BTT_M_STIFF:
		case BTT_M_BLOW:
		case BTT_GETUP:
			return m_pGroup->GetAnimationIdx( m_BackMidStiffWounded );
		case BTT_S_STIFF:
			return m_pGroup->GetAnimationIdx( m_BackStrStiffWounded );
		case BTT_STAND_KO:
			return m_pGroup->GetAnimationIdx( m_AniBackStandKoAction );
		case BTT_STAND_KO2:
			return m_pGroup->GetAnimationIdx( m_AniBackStandKo2Action );
		}
	}

	return m_pGroup->GetAnimationIdx( m_AniBlowPush );
}

AttackSide ioCopyOwnerDummyChar3::GetAttackedSide( const D3DXVECTOR3 &vAttackDir ) const
{
	if( m_State == DCS_BLOW_WOUNDED || m_State == DCS_BOUND_BLOW_WOUNDED )
	{
		return m_AttackedSide;
	}

	D3DXVECTOR3 vDefenseDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vInvAttack = -vAttackDir;

	float fAttackedCosine = D3DXVec3Dot( &vDefenseDir, &vInvAttack );
	if( fAttackedCosine >= cosf(FLOAT90) )
		return AS_FRONT;

	return AS_BACK;
}

void ioCopyOwnerDummyChar3::SetIgnoreWeaponInfo( ioWeapon *pWeapon )
{
	if( !pWeapon ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwIgnoreTime = pWeapon->GetIgnoreDuration();

	if( dwIgnoreTime > 0 )
	{
		m_WeaponIgnoreInfo.m_iWeaponType = pWeapon->GetType();
		m_WeaponIgnoreInfo.m_dwIgnoreTime = dwCurTime + dwIgnoreTime;
	}
}

void ioCopyOwnerDummyChar3::SetLastApplyWeapon( int iIndex, DWORD dwTime )
{
	if( dwTime == 0 )
		return;

	m_dwLastApplyWeaponIdx = iIndex;
	m_dwLastApplyWeaponTime = FRAMEGETTIME() + dwTime;
}

bool ioCopyOwnerDummyChar3::IsDownState()
{
	float fCurPushAmt = 0.0f;
	float fCurJumpAmt = m_fCurJumpPowerAmt;
	D3DXVECTOR3 vYAxis( 0.0f, FLOAT1, 0.0f );

	float fRate = D3DXVec3Dot( &vYAxis, &m_vForceDir );
	fCurPushAmt = m_fForceAmt * fRate;

	if( fCurPushAmt <= 0.0f )
		fCurJumpAmt += fCurPushAmt;

	if( fCurJumpAmt > m_fGravityAmt )
		return false;

	return true;
}

bool ioCopyOwnerDummyChar3::IsFloating()
{
	float fCharHeight = GetWorldPosition().y;
	float fBottomHeight = GetBottomHeight();

	if( fCharHeight - fBottomHeight > FLOAT1 )
		return true;

	return false;
}

bool ioCopyOwnerDummyChar3::IsFloatingState()
{
	if( m_State == DCS_BLOW_WOUNDED )
		return true;

	if( m_State == DCS_BOUND_BLOW_WOUNDED )
		return true;

	return false;
}

void ioCopyOwnerDummyChar3::RenderGaugeOwnerInfo()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	int iXPos = 0;
	int iYPos = 0;

	if( m_pGauge )
	{
		D3DXVECTOR3 vPos = GetMidPositionByRate( FLOAT1 );
		vPos.y += 40.0f;

		if( Setting::Check2DInScreen( vPos, iXPos, iYPos ) )
		{
			g_FontMgr.SetTextStyle( TS_BOLD_OUTLINE_FULL );
			g_FontMgr.SetBkColor( 0, 0, 0 );

			TeamType eCurTeamType = pOwner->GetTeam();
			TeamType eCurSingleTeamType = pOwner->GetSingleConvertTeam();

			switch( eCurTeamType )
			{
			case TEAM_BLUE:
				g_FontMgr.SetTextColor( 0, 150, 255 );
				break;
			case TEAM_RED:
				g_FontMgr.SetTextColor( 255, 60, 0 );
				break;
			default: //개인전
				{
					if( eCurSingleTeamType == TEAM_BLUE )
						g_FontMgr.SetTextColor( 0, 150, 255 );
					else
						g_FontMgr.SetTextColor( 255, 60, 0 );
				}				
				break;
			}

			enum { UPPER_Y_OFFSET = -12, GUILD_Y_OFFSET = -11, LADDER_TITLE_Y_OFFSET = -16, };
			if( pStage->GetModeType() == MT_MYROOM && !pStage->IsNetworkMode() )
			{
				if( pOwner->IsOwnerChar() )
				{
					g_FontMgr.SetAlignType( TAT_LEFT );

					DWORD dwGuildIndex, dwGuildMark;
					g_UserInfoMgr.GetGuildInfo( g_MyInfo.GetPublicID(), dwGuildIndex, dwGuildMark );
					int iNameWidth = g_FontMgr.GetTextWidth( g_MyInfo.GetPublicID().c_str(), TS_BOLD_OUTLINE_FULL, FONT_SIZE_12 );
					int iHalfWidth = ( iNameWidth + 20 + g_GuildMarkMgr.GetGuildSmallMarkSize( dwGuildIndex, dwGuildMark ) + 1 ) / 2;	// 20 은 계급사이즈

					g_LevelMgr.RenderGrade( g_MyInfo.GetPublicID(), iXPos - iHalfWidth, iYPos + UPPER_Y_OFFSET, pOwner->GetLevel(), TEAM_BLUE );
					g_FontMgr.PrintText( iXPos - iHalfWidth + 20 - 2, iYPos + UPPER_Y_OFFSET, FONT_SIZE_12, g_MyInfo.GetPublicID().c_str() );

					g_GuildMarkMgr.RenderSmallMark( dwGuildIndex, dwGuildMark, ( iXPos - iHalfWidth ) + 20 + iNameWidth + 2, iYPos + GUILD_Y_OFFSET );

					// 래더전 타이틀
					if( g_LadderTeamMgr.IsLadderTeam() && g_LadderTeamMgr.IsHeroMatchMode() && pOwner->GetHeroTitle() != 0 )
					{
						g_FontMgr.SetAlignType( TAT_CENTER );
						g_FontMgr.PrintText( iXPos, (iYPos + UPPER_Y_OFFSET)+LADDER_TITLE_Y_OFFSET, FONT_SIZE_12, "[%s]", g_LevelMgr.GetHeroTitleString( pOwner->GetHeroTitle() ).c_str() );
					}
				}
			}
			else
			{
				g_FontMgr.SetAlignType( TAT_LEFT );

				char szCharName[MAX_PATH] = "";
				sprintf( szCharName, "%s", pOwner->GetCharName().c_str() );
				DWORD dwGuildIndex, dwGuildMark;
				g_UserInfoMgr.GetGuildInfo( pOwner->GetCharName(), dwGuildIndex, dwGuildMark );
				int iNameWidth = g_FontMgr.GetTextWidth( szCharName, TS_BOLD_OUTLINE_FULL, FONT_SIZE_12 );
				int iHalfWidth = ( iNameWidth + 20+ g_GuildMarkMgr.GetGuildSmallMarkSize( dwGuildIndex, dwGuildMark ) + 1 ) / 2;	// 20 은 계급사이즈

				switch( eCurTeamType )
				{
				case TEAM_BLUE:
					g_LevelMgr.RenderGrade( pOwner->GetCharName(), iXPos - iHalfWidth, iYPos + UPPER_Y_OFFSET, pOwner->GetLevel(), TEAM_BLUE );
					break;
				case TEAM_RED:
					g_LevelMgr.RenderGrade( pOwner->GetCharName(), iXPos - iHalfWidth, iYPos + UPPER_Y_OFFSET, pOwner->GetLevel(), TEAM_RED );
					break;
				default: //개인전
					if( eCurSingleTeamType == TEAM_BLUE )
						g_LevelMgr.RenderGrade( pOwner->GetCharName(), iXPos - iHalfWidth, iYPos + UPPER_Y_OFFSET, pOwner->GetLevel(), TEAM_BLUE );
					else
						g_LevelMgr.RenderGrade( pOwner->GetCharName(), iXPos - iHalfWidth, iYPos + UPPER_Y_OFFSET, pOwner->GetLevel(), TEAM_RED );
					break;
				}

				g_FontMgr.PrintText( iXPos - iHalfWidth + 20 - 2, iYPos + UPPER_Y_OFFSET, FONT_SIZE_12, szCharName );
				g_GuildMarkMgr.RenderSmallMark( dwGuildIndex, dwGuildMark, ( iXPos - iHalfWidth ) + 20 + iNameWidth + 2, iYPos + GUILD_Y_OFFSET );

				// 래더전 타이틀
				if( g_LadderTeamMgr.IsLadderTeam() && g_LadderTeamMgr.IsHeroMatchMode() && pOwner->GetHeroTitle() != 0 )
				{
					g_FontMgr.SetAlignType( TAT_CENTER );
					g_FontMgr.PrintText( iXPos, (iYPos + UPPER_Y_OFFSET)+LADDER_TITLE_Y_OFFSET, FONT_SIZE_12, "[%s]", g_LevelMgr.GetHeroTitleString( pOwner->GetHeroTitle() ).c_str() );
				}
			}

			enum { LEVEL_X_OFFSET = -1, LEVEL_Y_OFFSET = 16, };
			switch( eCurTeamType )
			{
			case TEAM_BLUE:
			case TEAM_RED:
				if(Help::IsMonsterDungeonMode(pStage->GetModeType()))
				{
					g_LevelMgr.RenderClassLevel( iXPos + LEVEL_X_OFFSET, iYPos + LEVEL_Y_OFFSET, pOwner->GetGrowthInstantLevel()+1, eCurTeamType, pOwner->IsOwnerChar() );
				}
				else
				{
					g_LevelMgr.RenderClassLevel( iXPos + LEVEL_X_OFFSET, iYPos + LEVEL_Y_OFFSET, pOwner->GetClassLevel(), eCurTeamType, pOwner->IsOwnerChar() );
				}
				break;
			default: //개인전
				if( eCurSingleTeamType == TEAM_BLUE )
					g_LevelMgr.RenderClassLevel( iXPos + LEVEL_X_OFFSET, iYPos + LEVEL_Y_OFFSET, pOwner->GetClassLevel(), TEAM_BLUE, pOwner->IsOwnerChar() );
				else
					g_LevelMgr.RenderClassLevel( iXPos + LEVEL_X_OFFSET, iYPos + LEVEL_Y_OFFSET, pOwner->GetClassLevel(), TEAM_RED, pOwner->IsOwnerChar() );
				break;
			}

			enum { GAUGE_Y_OFFSET = 6, };
			m_pGauge->RenderGauge( iXPos, iYPos + GAUGE_Y_OFFSET, false );
		}
	}
}

void ioCopyOwnerDummyChar3::SetJumpPower( float fPowerAmt )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_fCurJumpPowerAmt != 0.0f )
	{
		m_fCurJumpPowerAmt *= m_fForcePowerAddRate;
		m_fGravityAmt *= m_fForcePowerAddRate;

		m_fCurJumpPowerAmt += fPowerAmt;
	}
	else
	{
		m_fGravityAmt = 0.0f;
		m_fCurJumpPowerAmt = fPowerAmt;
	}
}