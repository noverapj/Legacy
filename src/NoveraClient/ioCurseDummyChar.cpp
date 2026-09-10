
#include "stdafx.h"

#include "ioCurseDummyChar.h"
#include "ioGaugeHelper.h"
#include "ioAniCollisionBoxGrp.h"
#include "ioWoundedCollisionBoxGrp.h"
#include "ioCollisionBoxGrpManager.h"
#include "WeaponDefine.h"
#include "ioGuildMarkMgr.h"
#include "ioNpcChar.h"

#include "GUI/LadderTeamWnd.h"

ioCurseDummyChar::ioCurseDummyChar( ioEntityGroup *pGrp, ioPlayMode *pMode )
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
	m_dwPreBackAttackedTime = 0;

	m_BlowState = BW_NONE;

	m_dwLastApplyWeaponIdx = 0;
	m_dwLastApplyWeaponTime = 0;

	m_fRecoveryCurSkillGauge = 0.0f;

	m_dwExplosionTime = 0;

	m_ExplosionState = ES_NONE;

	//////////////////////////////////////////////////////////////////////////
	m_nCurAttackCnt = 0;

	m_dwStartTime = FRAMEGETTIME();
}

ioCurseDummyChar::~ioCurseDummyChar()
{
}

void ioCurseDummyChar::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	m_dwGeneralBlowWoundedGetUpTime = rkLoader.LoadInt_e( "blow_wounded_getup_time_general", 1000 );
	m_fRecoverySkillGaugeOnWounded = rkLoader.LoadFloat_e( "recovery_skill_gauge_on_wounded", 0.0f );
	m_fRecoveryMaxSkillGauge = rkLoader.LoadFloat_e( "recovery_max_skill_gauge", 0.0f );

	rkLoader.LoadString_e( "recovery_skill_gauge_effect", "", szBuf, MAX_PATH );
	m_szRecoverySkillGaugeEffect = szBuf;

	m_dwExplosionWaitTime = (int)rkLoader.LoadInt_e( "explosion_time", 0 );

	m_WeaponOnWound.m_iWeaponIdx        = rkLoader.LoadInt_e( "attack_on_wounded_index", NO_WEAPON_ATTRIBUTE );
	m_WeaponOnWound.m_iResistanceIdx    = rkLoader.LoadInt_e( "attack_on_wounded_index_resist", NO_WEAPON_ATTRIBUTE );
	rkLoader.LoadString_e( "attack_on_wounded", "", szBuf, MAX_PATH );
	m_WeaponOnWound.m_WoundedAnimation  = szBuf;
	m_WeaponOnWound.m_dwWoundedDuration = rkLoader.LoadInt_e( "attack_on_wounded_time", 0 );
	m_WeaponOnWound.m_bWoundedLoopAni   = rkLoader.LoadBool_e( "attack_on_wounded_loop_ani", false );

	rkLoader.LoadString_e( "explosion_wait_effect", "", szBuf, MAX_PATH );
	m_szExplosionWaitEffect = szBuf;

	rkLoader.LoadString_e( "explosion_effect", "", szBuf, MAX_PATH );
	m_ExplosionEffect = szBuf;

	rkLoader.LoadString_e( "explosion_sound", "", szBuf, MAX_PATH );
	m_ExplosionSound = szBuf;

	//////////////////////////////////////////////////////////////////////////
	m_nMaxAttackCnt = rkLoader.LoadInt_e( "max_attack_cnt", 1 );
	m_dwMaxLifeTime = rkLoader.LoadInt_e( "lifetime", 0 );

	rkLoader.LoadString_e( "max_attack_buff", "", szBuf, MAX_PATH );
	m_MaxAttackBuff = szBuf;
}

void ioCurseDummyChar::LoadPropertyByChar()
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

void ioCurseDummyChar::SetCopyOwnerInfo( ioBaseChar *pOwner, bool bOnlyInfo )
{
	if( !pOwner )
		return;

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

	m_ArmorType = pOwner->GetArmorType();

	if( !bOnlyInfo )
		SetDummyState( DCS_DELAY );
}

bool ioCurseDummyChar::IsCollisionAvailableWeapon( ioWeapon *pWeapon )
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

bool ioCurseDummyChar::IsCharCollisionSkipState( TeamType eTeamType, int iState ) const
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

bool ioCurseDummyChar::IsDummyCharCollisionSkipState( ioBaseChar *pChar )
{
	bool bSkip = ioDummyChar::IsDummyCharCollisionSkipState( pChar );
	if( bSkip )
		return true;

	return false;
}

void ioCurseDummyChar::RenderGauge()
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

void ioCurseDummyChar::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case DCS_START:
		ProcessStartState( fTimePerSec );
		break;
	case DCS_DELAY:
		ProcessDelayState( fTimePerSec );
		break;
	case DCS_DIE:
		ProcessDieState( fTimePerSec );
		break;
	case DCS_WOUNDED:
		ProcessWoundedState( fTimePerSec );
		break;
	case DCS_BLOW_WOUNDED:
		ProcessBlowWoundedState( fTimePerSec );
		break;
	case DCS_BOUND_BLOW_WOUNDED:
		ProcessBoundBlowWoundedState( fTimePerSec );
		break;
	}

	ProcessPos( fTimePerSec );
	ProcessExplosionState();

	if( m_pModeParent && m_pModeParent->IsZeroHP() )
	{
		if( m_HP.m_fCurValue > 0.0f )
			m_HP.m_fCurValue = 0.0f;
	}

	if( (m_dwStartTime + m_dwMaxLifeTime) < dwCurTime )
		SetDieState(true);
}

void ioCurseDummyChar::ProcessPos( float fTimePerSec )
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
		if( m_State == DCS_BOUND_BLOW_WOUNDED && m_BlowState == BW_BLOWING )
		{
			fBoundBlowGravityRate = m_fBoundBlowGravityRate;
		}

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

void ioCurseDummyChar::ProcessForceMove( float fTimePerSec )
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

void ioCurseDummyChar::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	if( m_pModeParent && m_pModeParent->GetCreator() && !bEffect )
	{
		ioMapEffect *pEffect = m_pModeParent->GetCreator()->CreateMapEffect( m_ExplosionEffect, GetWorldPosition(), ioMath::UNIT_ALL );
		if( pEffect )
		{
			pEffect->SetWorldOrientation( GetWorldOrientation() );
			g_SoundMgr.PlaySound( m_ExplosionSound, m_pGroup->GetParentSceneNode(), PSM_THREAD );
		}
	}

	ioDummyChar::SetDieState( bEffect, bWoundedDie );
}

void ioCurseDummyChar::ProcessWoundedState( float fTimePerSec )
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

void ioCurseDummyChar::ProcessBlowWoundedState( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_BlowState )
	{
	case BW_BLOWING:
		if( m_bJumpedBlowWounded )
		{
			if( !m_bBlowEndAni )
			{
				if( IsDownState() && !IsFloating() )	// 쓰러져서 바닥에 쿵하고 부딪히는 상태
				{
					int iAniID = m_pGroup->GetAnimationIdx( m_CurBlowDownAction );

					m_pGroup->ClearAllActionAni( 0.0f, true );
					m_pGroup->ClearAllLoopAni( 0.0f, true );
					m_pGroup->SetActionAni( iAniID, 0.0f, FLOAT100, FLOAT1, FLOAT1, 0.0f, 0.0f, true );

					if( m_bFallBlowWounded )
					{
						m_AnimationTime.dwStartTime = dwCurTime;
						m_AnimationTime.dwFullTime = m_pGroup->GetAnimationFullTime( iAniID );
						m_bBlowEndAni = true;

						m_dwBlowAniEndTime = dwCurTime + m_pGroup->GetAnimationEventTime_e( iAniID, "knock_down" );

						m_pGroup->AttachEffect( "sp2_knock_down_dust.txt", NULL );
					}
					else
					{
						m_AnimationTime.dwStartTime = 0;
						m_AnimationTime.dwFullTime  = 0;

						m_BlowState = BW_BLOW_ENDED;
					}
				}
				else
				{
					if( m_dwBlowAniEndTime < dwCurTime && !IsFloating() )
					{
						int iAniID = m_pGroup->GetAnimationIdx( m_CurBlowDownAction );

						m_pGroup->ClearAllActionAni( 0.0f, true );
						m_pGroup->ClearAllLoopAni( 0.0f, true );
						m_pGroup->SetActionAni( iAniID, 0.0f, FLOAT100, FLOAT1, FLOAT1, 0.0f, 0.0f, true );

						if( m_bFallBlowWounded )
						{
							m_AnimationTime.dwStartTime = dwCurTime;
							m_AnimationTime.dwFullTime = m_pGroup->GetAnimationFullTime( iAniID );
							m_bBlowEndAni = true;

							m_pGroup->AttachEffect( "sp2_knock_down_dust.txt", NULL );
						}
						else
						{
							m_AnimationTime.dwStartTime = 0;
							m_AnimationTime.dwFullTime  = 0;

							m_BlowState = BW_BLOW_ENDED;
						}
					}
				}
			}
			else
			{
				if( m_dwBlowAniEndTime < dwCurTime )
				{
					m_AnimationTime.dwStartTime = 0;
					m_AnimationTime.dwFullTime  = 0;

					m_BlowState = BW_BLOW_ENDED;
				}
			}
		}
		else
		{
			if( m_dwBlowAniEndTime < dwCurTime && !IsFloating() )
			{
				m_AnimationTime.dwStartTime = 0;
				m_AnimationTime.dwFullTime  = 0;

				m_BlowState = BW_BLOW_ENDED;
			}
		}

		if( m_BlowState == BW_BLOW_ENDED )
		{
			if( m_dwBlowGetUpCheckStartTime == 0 )
			{
				m_dwBlowGetUpCheckStartTime = max( dwCurTime, m_AnimationTime.EndTime() );
			}

			m_dwBlowAniEndTime = 0;

			m_pGroup->AttachEffect( "sp2_knock_down_dust.txt", NULL );
			m_iFloatingWoundedCnt = 0;

			// Flatten
			if( m_BlowTargetType == BTT_FLATTEN )
			{
				D3DXVECTOR3 vCurScale = GetWorldScale();
				vCurScale.y = 0.1f;

				SetWorldScale( vCurScale );
			}
		}
		break;
	case BW_BLOW_ENDED:	// 바닥에 떨어진 시점부터 - 일어나기 전까지
		if( m_dwBlowGetUpCheckStartTime + m_dwCurBlowGetUpTime <= dwCurTime )
		{
			SetBlowWoundedGetUp();
		}
		break;
	case BW_GETUP:
		if( m_dwGetUpDurationTime < dwCurTime )
		{
			m_dwBlowGetUpCheckStartTime = 0;
			SetDummyState( DCS_DELAY );
		}
		break;
	}
}

void ioCurseDummyChar::ProcessBoundBlowWoundedState( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_BlowState )
	{
	case BW_BLOWING:
		if( IsDownState() && !IsFloating() )	// 쓰러져서 바닥에 쿵하고 부딪히는 상태
		{
			int iAniID = -1;

			if( m_AttackedSide == AS_BACK )
				iAniID = m_pGroup->GetAnimationIdx(m_AniBoundBlowBackAction2);
			else
				iAniID = m_pGroup->GetAnimationIdx(m_AniBoundBlowAction2);

			m_pGroup->ClearAllActionAni( 0.0f, true );
			m_pGroup->ClearAllLoopAni( 0.0f, true );
			m_pGroup->SetActionAni( iAniID, 0.0f, FLOAT100, FLOAT1, FLOAT1, 0.0f, 0.0f, true );

			// 모션 바꾸면서 튀어오르기, 밀림주기
			m_fGravityAmt = 0.0f;
			m_fCurJumpPowerAmt = 0.0f;
			m_fCurGravityGapAmt = 0.0f;

			SetJumpPower( m_fBoundBlowJumpAmt );
			SetForcePowerAmt( m_fBoundBlowForceAmt );

			m_pGroup->AttachEffect( "sp2_knock_down_dust.txt", NULL );

			m_AnimationTime.dwStartTime = 0;
			m_AnimationTime.dwFullTime  = 0;

			m_BlowState = BW_BOUND_BLOWING;
			m_bJumpedBlowWounded = true;
			m_bBlowEndAni = false;
		}
		break;
	case BW_BOUND_BLOWING:
		if( m_bJumpedBlowWounded )
		{
			if( !m_bBlowEndAni )
			{
				if( IsDownState() && !IsFloating() )	// 쓰러져서 바닥에 쿵하고 부딪히는 상태
				{
					int iAniID = m_pGroup->GetAnimationIdx( m_CurBlowDownAction );

					m_pGroup->ClearAllActionAni( 0.0f, true );
					m_pGroup->ClearAllLoopAni( 0.0f, true );
					m_pGroup->SetActionAni( iAniID, 0.0f, FLOAT100, FLOAT1, FLOAT1, 0.0f, 0.0f, true );

					m_AnimationTime.dwStartTime = 0;
					m_AnimationTime.dwFullTime  = 0;

					m_BlowState = BW_BLOW_ENDED;
				}
				else
				{
					if( m_dwBlowAniEndTime < dwCurTime && !IsFloating() )
					{
						int iAniID = m_pGroup->GetAnimationIdx( m_CurBlowDownAction );

						m_pGroup->ClearAllActionAni( 0.0f, true );
						m_pGroup->ClearAllLoopAni( 0.0f, true );
						m_pGroup->SetActionAni( iAniID, 0.0f, FLOAT100, FLOAT1, FLOAT1, 0.0f, 0.0f, true );

						m_AnimationTime.dwStartTime = 0;
						m_AnimationTime.dwFullTime  = 0;

						m_BlowState = BW_BLOW_ENDED;
					}
				}
			}
			else
			{
				if( m_dwBlowAniEndTime < dwCurTime )
				{
					m_AnimationTime.dwStartTime = 0;
					m_AnimationTime.dwFullTime  = 0;

					m_BlowState = BW_BLOW_ENDED;
				}
			}
		}
		else
		{
			if( m_dwBlowAniEndTime < dwCurTime && !IsFloating() )
			{
				m_AnimationTime.dwStartTime = 0;
				m_AnimationTime.dwFullTime  = 0;

				m_BlowState = BW_BLOW_ENDED;
			}
		}

		if( m_BlowState == BW_BLOW_ENDED )
		{
			if( m_dwBlowGetUpCheckStartTime == 0 )
			{
				m_dwBlowGetUpCheckStartTime = max( dwCurTime, m_AnimationTime.EndTime() );
			}

			m_dwBlowAniEndTime = 0;

			m_pGroup->AttachEffect( "sp2_knock_down_dust.txt", NULL );
			m_iFloatingWoundedCnt = 0;
		}
		break;
	case BW_BLOW_ENDED:	// 바닥에 떨어진 시점부터 - 일어나기 전까지
		if( m_dwBlowGetUpCheckStartTime + m_dwCurBlowGetUpTime <= dwCurTime )
		{
			SetBlowWoundedGetUp();
		}
		break;
	case BW_GETUP:
		if( m_dwGetUpDurationTime < dwCurTime )
		{
			m_dwBlowGetUpCheckStartTime = 0;
			SetDummyState( DCS_DELAY );
		}
		break;
	}
}

ApplyWeaponExp ioCurseDummyChar::ApplyWeapon( ioWeapon *pWeapon )
{
	ioBaseChar *pAttacker = pWeapon->GetOwner();
	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioDummyChar::ApplyWeapon - %s Not Exist", pWeapon->GetOwnerName().c_str() );
		return AWE_EXPLOSION;
	}

	if( pWeapon->IsNoWoundedWeapon() )
		return AWE_NO_EXPLOSION;

	D3DXVECTOR3 vAttackDir = pWeapon->GetAttackDir(this);
	D3DXVec3Normalize( &vAttackDir, &vAttackDir );

	AttackSide eAttackSide = GetAttackedSide( vAttackDir );
	if( pAttacker->GetTeam() == GetTeam() )
	{
		SetDieState( true );
		return AWE_EXPLOSION;
	}

	ApplyWeaponWounded( pWeapon, eAttackSide, vAttackDir );

	m_nCurAttackCnt++;
	if( m_nCurAttackCnt >= m_nMaxAttackCnt )
	{
		ioBaseChar *pOwner = GetOwner();
		if( pOwner )
			pOwner->AddNewBuff( m_MaxAttackBuff, "", "", NULL );
		
		SetDieState(true);
	}

	return AWE_EXPLOSION;
}

void ioCurseDummyChar::ApplyWeaponWounded( ioWeapon *pWeapon, AttackSide eSide, const D3DXVECTOR3 &vAttackDir )
{
	if( !pWeapon )
		return;

	ioBaseChar *pAttacker = pWeapon->GetOwner();
	if( !pAttacker )
		return;

	SetExplosionWaitState();

	BlowTargetType ePreBlowType = m_BlowTargetType;
	m_BlowTargetType = pWeapon->GetBlowType();
	m_AttackedSide   = eSide;

	// 밀림, 뛰움 값 처리
	float fDamage = pWeapon->GetDamage( this ) * m_fDamageRate;
	float fBoundDamage = 0.0f;
	float fBoundJumpAmt = pWeapon->GetBoundBlowPower( fBoundDamage );
	float fBoundForceAmt = pWeapon->GetBoundPushPower( fBoundDamage );
	float fBoundGrvityRate = pWeapon->GetBoundBlowGravityRate();

	BlowWoundAniInfo kBlowWoundAniInfo = pWeapon->GetBlowWoundAniInfo();

	float fBlowPower = 0.f;
	float fPushPower = 0.f;
	// 밀림, 띄움 값 계산후 성장에 의한 데미지 적용
	fDamage *= pWeapon->GetGrowthAttackRate();
	fBoundDamage *= pWeapon->GetGrowthAttackRate();

	//대화 광장에서 대미지 안들어가게 수정
	ioBaseChar *pOwner = GetOwner();
	ioPlayStage *pStage = NULL;
	if( pOwner ) 
		pStage = pOwner->GetCreator();

	if( pStage && pStage->GetRoomStyle() == RSTYLE_PLAZA && pStage->GetPlazaModeType() == PT_COMMUNITY )
	{
		fDamage = 0.f;
		fBoundDamage = 0.f;
	}

	if( m_State == DCS_BLOW_WOUNDED ||
		m_State == DCS_BOUND_BLOW_WOUNDED )
	{
		m_iFloatingWoundedCnt++;

		float fBlowRate = m_fFloatingBlowDownRate * (float)m_iFloatingWoundedCnt;
		fBlowRate = max( 0.0f, ( FLOAT1 - fBlowRate ) );
		fBlowPower *= fBlowRate;

		if( !IsFloatingState() )
		{
			fBlowPower *= pWeapon->GetDownStateBlowPower();
			fPushPower *= pWeapon->GetDownStatePushPower();
		}
	}

	bool bApplyPushPower = true;

	ioHashString szWoundAni = pWeapon->GetWoundedAnimation();
	DWORD dwWoundDuration = pWeapon->GetWoundedDuration();
	bool bWoundedLoopAni = pWeapon->IsLoopWoundedAnimation();

	float fDefenseEnableRate = pWeapon->GetDefenseEnableRate();

	bool bAllWeakWounded = false;
	BlowTargetType eNewBlowType = pWeapon->GetBlowType();

	if( pStage )
	{
		if( pStage->CheckRoomWeakWounded( m_TeamType ) )
		{
			bAllWeakWounded = true;
			fBlowPower = FLOAT1;
		}
	}

	if( pOwner )
	{
		pOwner->GetHP().m_fCurValue -= fDamage;
	}

	if( fDamage > 0.0f )
	{
		AddHeadDamageInfo( fDamage );
		SetLastApplyWeapon( pWeapon->GetAttributeIndex(), pWeapon->GetIgnoreWeaponTime() );

		ioBaseChar *pOwner = GetOwner();
		if( pOwner && m_fRecoverySkillGaugeOnWounded > 0.0f && m_fRecoveryCurSkillGauge < m_fRecoveryMaxSkillGauge )
		{
			for( int i=0; i<4; i++ )
			{
				ioItem *pCurItem = pOwner->GetEquipedItem( i );
				if( !pCurItem )
					continue;

				ioSkill *pSkill = pOwner->GetEquipedSkill( i );
				if( !pSkill )
					continue;

				if( pOwner->GetCurActiveSkill() == pSkill )
					continue;

				if( pSkill->CheckUseBuff(pOwner) )
					continue;

				int iCur = pCurItem->GetCurSkillGuage();
				iCur += m_fRecoverySkillGaugeOnWounded;
				pCurItem->SetCurSkillGauge( iCur );

				ioPassiveSkill *pPassiveSkill = ToPassiveSkill( pSkill );
				if( pPassiveSkill && pPassiveSkill->UsePassiveGauge() )
				{
					iCur = pPassiveSkill->GetCurPassiveGauge();
					iCur += m_fRecoverySkillGaugeOnWounded;
					pPassiveSkill->SetCurPassiveGauge( iCur );
				}
			}

			m_fRecoveryCurSkillGauge += m_fRecoverySkillGaugeOnWounded;

			if( pOwner->IsNeedProcess() && !m_szRecoverySkillGaugeEffect.IsEmpty() )
				pOwner->AttachEffect( m_szRecoverySkillGaugeEffect );
		}
	}

	bool bEnableHitCount = true;
	if( !bApplyPushPower || pWeapon->HasFeature(WF_DISABLE_HIT_COUNT) )
		bEnableHitCount = false;

	if( pWeapon->GetTeam() != GetTeam() && bEnableHitCount && pAttacker )
	{
		pAttacker->CheckComboSkillHit();
	}

	if( pWeapon->CheckWeaponByJump() && pAttacker )
	{
		pAttacker->SetSuccessJumpAttack( true );
	}

	SetIgnoreWeaponInfo( pWeapon );

	if( GetOwner() && GetOwner()->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << SYNC_APPLY_WEAPON;

		kPacket << GetWorldPosition();
		kPacket << GetWorldOrientation();

		kPacket << pWeapon->GetOwnerName();
		kPacket << pWeapon->GetAttributeIndex();

		kPacket << (int)m_AttackedSide;
		kPacket << (int)eNewBlowType;
		kPacket << m_HP.m_fCurValue;
		kPacket << fDamage;

		kPacket << m_vForceDir;
		kPacket << m_fForceAmt;
		kPacket << m_fForceFrictionRate;
		kPacket << m_bForcePowerLinear;

		kPacket << m_fCurJumpPowerAmt;
		kPacket << m_fGravityAmt;

		kPacket << IsFloatingState();
		kPacket << IsFloating();

		kPacket << pWeapon->GetTeam();

		kPacket << m_State;
		kPacket << m_bJumpedBlowWounded;
		kPacket << m_bFallBlowWounded;

		kPacket << bWoundedLoopAni;

		if( m_State == DCS_WOUNDED )
		{
			kPacket << szWoundAni;
			kPacket << dwWoundDuration;
			kPacket << bAllWeakWounded;
		}
		else if( m_State == DCS_BOUND_BLOW_WOUNDED )
		{
			kPacket << fBoundDamage;
			kPacket << fBoundJumpAmt;
			kPacket << fBoundForceAmt;
			kPacket << fBoundGrvityRate;
		}
		kPacket << bEnableHitCount;
		kPacket << pWeapon->CheckWeaponByJump();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCurseDummyChar::ApplyWeaponWoundedInNetwork( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	SetExplosionWaitState();

	D3DXVECTOR3 vPos;
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

		ioBaseChar *pOwner = GetOwner();
		if( pOwner && m_fRecoverySkillGaugeOnWounded > 0.0f && m_fRecoveryCurSkillGauge < m_fRecoveryMaxSkillGauge )
		{
			for( int i=0; i<4; i++ )
			{
				ioItem *pCurItem = pOwner->GetEquipedItem( i );
				if( !pCurItem )
					continue;

				ioSkill *pSkill = pOwner->GetEquipedSkill( i );
				ioPassiveSkill *pPassiveSkill = ToPassiveSkill( pSkill );
				if( !pSkill )
					continue;

				if( pSkill->GetTypeID() == ST_PASSIVE )
					continue;

				if( pOwner->GetCurActiveSkill() == pSkill )
					continue;

				if( pSkill->CheckUseBuff(pOwner) )
					continue;

				int iCur = pCurItem->GetCurSkillGuage();
				iCur += m_fRecoverySkillGaugeOnWounded;
				pCurItem->SetCurSkillGauge( iCur );
			}

			m_fRecoveryCurSkillGauge += m_fRecoverySkillGaugeOnWounded;

			if( pOwner->IsNeedProcess() && !m_szRecoverySkillGaugeEffect.IsEmpty() )
				pOwner->AttachEffect( m_szRecoverySkillGaugeEffect );
		}
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
	else if( iCharState == DCS_BOUND_BLOW_WOUNDED )
	{
		float fBoundDamage, fBoundJumpAmt, fBoundForceAmt, fBoundGravityRate;
		rkPacket >> fBoundDamage >> fBoundJumpAmt >> fBoundForceAmt >> fBoundGravityRate;

		SetBoundWoundedState( fBoundJumpAmt, fBoundForceAmt, fBoundGravityRate, fBoundDamage, false );
	}
	else if( iCharState == DCS_BLOW_WOUNDED )
	{
		BlowWoundAniInfo kInfo;

		if( pAttr )
		{
			kInfo = pAttr->m_BlowWoundAniInfo;
		}

		SetBlowWoundedState( bJumpedBlow, kInfo, false, bFallBlow );
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
}

void ioCurseDummyChar::ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pStage ) return;

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case SYNC_APPLY_WEAPON:
		ApplyWeaponWoundedInNetwork( pStage, rkPacket );
		break;
	default:
		ioDummyChar::ApplyDummyCharSyncByType( iType, rkPacket, pStage );
	}
}

void ioCurseDummyChar::SetDummyState( DummyCharState eState )
{
	if( !m_pGroup )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch(eState)
	{
	case DCS_START:
		{
			int iAniID = m_pGroup->GetAnimationIdx( m_StartAnimation );
			if( iAniID != -1 )
			{
				m_pGroup->ClearAllLoopAniExceptOne( iAniID, FLOAT100, true );
				m_pGroup->ClearAllActionAni( FLOAT100, true );
				m_pGroup->SetLoopAni( iAniID, FLOAT100 );

				m_AnimationTime.dwStartTime = dwCurTime;
				m_AnimationTime.dwFullTime = m_pGroup->GetAnimationFullTime( iAniID ) * m_fStartAniRate;

				m_AttackedSide = AS_NONE;
				m_BlowTargetType = BTT_NONE;
			}
		}
		break;
	case DCS_DELAY:
		{
			if( m_State == DCS_DELAY )
				break;

			int iAniID = m_pGroup->GetAnimationIdx( m_DelayAnimation );
			if( iAniID != -1 )
			{
				m_pGroup->ClearAllLoopAniExceptOne( iAniID, FLOAT100, true );
				m_pGroup->ClearAllActionAni( FLOAT100, true );
				m_pGroup->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1 / m_fDelayAniRate );

				m_AnimationTime.dwStartTime = 0;
				m_AnimationTime.dwFullTime = 0;

				m_AttackedSide = AS_NONE;
				m_BlowTargetType = BTT_NONE;
			}
		}
		break;
	case DCS_BLOW_WOUNDED:
		{
			if( eState != DCS_BLOW_WOUNDED && eState != DCS_BOUND_BLOW_WOUNDED )
			{
				m_dwBlowGetUpCheckStartTime = 0;
			}

			// Flatten
			D3DXVECTOR3 vCurScale = GetWorldScale();
			if( vCurScale.x != vCurScale.y )
			{
				vCurScale.y = vCurScale.x;
				SetWorldScale( vCurScale );
			}
		}
		break;
	case DCS_BOUND_BLOW_WOUNDED:
		if( eState != DCS_BLOW_WOUNDED && eState != DCS_BOUND_BLOW_WOUNDED )
		{
			m_dwBlowGetUpCheckStartTime = 0;

			m_fBoundBlowForceAmt = 0.0f;
			m_fBoundBlowJumpAmt = 0.0f;
		}

		// Flatten
		D3DXVECTOR3 vCurScale = GetWorldScale();
		if( vCurScale.x != vCurScale.y )
		{
			vCurScale.y = vCurScale.x;
			SetWorldScale( vCurScale );
		}
		break;
	}

	m_State = eState;
}

void ioCurseDummyChar::SetWoundedState( const ioHashString &szWoundedAni, DWORD dwWoundedDuration, bool bLoopAni )
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

void ioCurseDummyChar::SetBlowWoundedState( bool bJumped, const BlowWoundAniInfo &rkBlowWoundAniInfo, bool bGetUpState /* = false */, bool bFallState /* = false */ )
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
	m_dwPreBackAttackedTime = 0;
	m_iBackAttackedCnt = 0;	
	m_BlowState = BW_BLOWING;

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

void ioCurseDummyChar::SetBoundWoundedState( float fBoundJump, float fBoundForce, float fBoundGravity, float fBoundDamage, bool bGetUpState /* = false */ )
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
	m_dwPreBackAttackedTime = 0;
	m_iBackAttackedCnt = 0;	

	if( m_bJumpedBlowWounded )
		m_BlowState = BW_BLOWING;
	else
		m_BlowState = BW_BOUND_BLOWING;

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

void ioCurseDummyChar::SetBlowWoundedGetUp()
{
	int iAniID = 0;

	if( m_AttackedSide == AS_BACK )
		iAniID = m_pGroup->GetAnimationIdx(m_AniBackBlowGetUp);
	else
		iAniID = m_pGroup->GetAnimationIdx(m_AniBlowGetUp);

	m_pGroup->ClearAllActionAni( FLOAT100, true );
	m_pGroup->ClearAllLoopAni( FLOAT100, true );
	m_pGroup->SetActionAni( iAniID, 0.0f, FLOAT100, FLOAT1, FLOAT1, 0.0f, 0.0f, true );

	DWORD dwGetUpDuration = m_pGroup->GetAnimationFullTime( iAniID ) - 100;
	m_dwGetUpDurationTime = FRAMEGETTIME();
	m_dwGetUpDurationTime += dwGetUpDuration;

	m_BlowTargetType = BTT_NONE;
	m_BlowState = BW_GETUP;

	m_dwCurBlowGetUpTime = 0;
	m_dwBlowAniEndTime = 0;

	// Flatten
	D3DXVECTOR3 vCurScale = GetWorldScale();
	if( vCurScale.x != vCurScale.y )
	{
		vCurScale.y = vCurScale.x;
		SetWorldScale( vCurScale );
	}
}

int ioCurseDummyChar::GetWoundedAnimationIdx( AttackSide eAttackSide, BlowTargetType eBlowTargetType ) const
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

AttackSide ioCurseDummyChar::GetAttackedSide( const D3DXVECTOR3 &vAttackDir ) const
{
	if( m_State == DCS_BLOW_WOUNDED || m_State == DCS_BOUND_BLOW_WOUNDED )
	{
		if( m_BlowState == BW_BLOW_ENDED )
		{
			return m_AttackedSide;
		}
	}

	D3DXVECTOR3 vDefenseDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vInvAttack = -vAttackDir;

	float fAttackedCosine = D3DXVec3Dot( &vDefenseDir, &vInvAttack );
	if( fAttackedCosine >= cosf(FLOAT90) )
		return AS_FRONT;

	return AS_BACK;
}

void ioCurseDummyChar::SetIgnoreWeaponInfo( ioWeapon *pWeapon )
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

void ioCurseDummyChar::SetLastApplyWeapon( int iIndex, DWORD dwTime )
{
	if( dwTime == 0 )
		return;

	m_dwLastApplyWeaponIdx = iIndex;
	m_dwLastApplyWeaponTime = FRAMEGETTIME() + dwTime;
}

bool ioCurseDummyChar::IsDownState()
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

bool ioCurseDummyChar::IsFloating()
{
	float fCharHeight = GetWorldPosition().y;
	float fBottomHeight = GetBottomHeight();

	if( fCharHeight - fBottomHeight > FLOAT1 )
		return true;

	return false;
}

bool ioCurseDummyChar::IsFloatingState()
{
	if( m_State == DCS_BLOW_WOUNDED &&
		( m_BlowState == BW_BLOWING || m_BlowState == BW_BOUND_BLOWING ) )
		return true;

	if( m_State == DCS_BOUND_BLOW_WOUNDED &&
		(m_BlowState == BW_BLOWING || m_BlowState == BW_BOUND_BLOWING) )
		return true;

	return false;
}

void ioCurseDummyChar::RenderGaugeOwnerInfo()
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

				ioNpcChar *pNpc = ToNpcChar( pOwner );

				if( pNpc )
					sprintf( szCharName, "%s", pNpc->GetViewName().c_str() );
				else
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

void ioCurseDummyChar::SetJumpPower( float fPowerAmt )
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

void ioCurseDummyChar::SetExplosionWaitState()
{
	if( m_dwExplosionWaitTime == 0 )
		return;

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
}

void ioCurseDummyChar::ProcessExplosionState()
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
			if( m_dwExplosionTime > 0 && m_dwExplosionTime < FRAMEGETTIME() )
			{
				m_dwExplosionTime = 0;
				SetDieState( false );
			}
		}
		break;
	}
}

void ioCurseDummyChar::SetExplosion()
{
	if( m_dwExplosionWaitTime > 0 )
	{
		m_dwExplosionTime = 0;
	}

	SetDieState( false );
}

void ioCurseDummyChar::SetCharNameInfo( const ioHashString& stCurseName, const ioHashString& stCreaterNamme )
{
	m_CurseChar   = stCurseName;
	m_CreatorName = stCreaterNamme;
}

void ioCurseDummyChar::ProcessDelayState( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		SetDieState(true);
		return;
	}

	SetCopyOwnerInfo( pOwner );
}

bool ioCurseDummyChar::CheckOwnerDestroyState()
{
	return false;
}

void ioCurseDummyChar::Update( float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	float fSecPerTime = fTimePerSec * FLOAT1000;
	m_pGroup->Update( fSecPerTime );

	ProcessState( fTimePerSec, dwPreTime );

	CheckDieByHp();
	DropZoneCheck();
	CheckDestroyRange();

	if( CheckOwnerDestroyState() )
	{
		SendDieState( true );
	}
}

bool ioCurseDummyChar::IsEntityCollisionSkipState( ioGameEntity* pColEntity ) const
{
	//더미를 붙이고 있음으로 캐릭터는 Skip
	ioBaseChar* pChar = ToBaseChar( pColEntity );
	if( pChar && pChar->GetState() == CS_OBJECT_ATTACH )
		return true;

	return false;
}